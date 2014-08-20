
#include "ArnoldAnimationPatch.h"

#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
ArnoldAnimationPatch::ArnoldAnimationPatch(const JSONNode data)
: m_worker(NULL), m_startPoint(chrono::system_clock::from_time_t(0)),
    m_mode(ArnoldAnimationMode::STOPPED), m_preview_samples(m_interface.getSamples()),
    m_render_samples(m_interface.getSamples()) {
    // TODO: type for frame manager
	m_frameManager = new ArnoldMemoryFrameManager();
    loadJSON(data);
}
    
ArnoldAnimationPatch::~ArnoldAnimationPatch() {
    delete m_frameManager;

    // If close() hasn't been called, closes here.
    if (m_worker != NULL)
        close();
}

void ArnoldAnimationPatch::init() {
    ArnoldPatch::init();

	/*
    // Starts a worker thread.
    m_worker = new std::thread(&ArnoldAnimationPatch::workerLoop, this);
	*/
}
    
void ArnoldAnimationPatch::update(set<Device *> devices) {
	// Doesn't respond if it's stopped.
    if (m_mode == ArnoldAnimationMode::STOPPED)
        return ;
    
    FrameDeviceInfo frame;
    chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();

    // Sets the start point to the moment when update is called for the first time
    if (m_startPoint == chrono::system_clock::from_time_t(0))
		m_startPoint = current;

    frame.time = chrono::duration_cast<chrono::milliseconds>(current - m_startPoint).count();

	// A new Interactive frame indicates the end of Rendering tasks.
	// Note that all Rendering frames are created by the worker thread.
    if (m_mode == ArnoldAnimationMode::RENDERING ||
        m_mode == ArnoldAnimationMode::INTERACTIVE)
        frame.mode = ArnoldAnimationMode::INTERACTIVE;
    else if (m_mode == ArnoldAnimationMode::RECORDING)
        frame.mode = ArnoldAnimationMode::RECORDING;
    
    // Checks if an update is needed.
    bool rerender_req = isUpdateRequired(devices);
    clearUpdateFlags();

    // There's no need to send this frame
    if (!rerender_req)
        return ;
    
    for (Device *d : devices) {
		// Checks if the device is connect to this patch
		if (m_lights.count(d->getId()) > 0) {
			// Makes copy of this device
			Device *d_copy = new Device(*d);
			frame.devices.insert(d_copy);
			if (d_copy->getId() == "vizi") {
				std::stringstream ss;
				LumiverseOrientation *ori = (LumiverseOrientation *)d_copy->getParam("pan");
				ss << "vizi: " << ori->getVal();
				Logger::log(LDEBUG, ss.str());
			}
		}
    }

    std::stringstream ss;
    ss << "Sent new frame: " << frame.time << "(" << frame.mode << ")";
    Logger::log(LDEBUG, ss.str());

	// Interrupts the current rendering if in the interactive mode.
    if (m_mode == ArnoldAnimationMode::INTERACTIVE ||
		m_mode == ArnoldAnimationMode::RECORDING) {
        m_interface.interrupt();
    }
    
    // Since there aren't many competitions (worker runs slowly),
    // it's okay to just use a coarse lock.
    m_queue.lock();
    // Checks if start point was reset during this update call.
    if (m_startPoint != chrono::system_clock::from_time_t(0))
        m_queuedFrameDeviceInfo.push_back(frame);
    m_queue.unlock();
}

void ArnoldAnimationPatch::close() {
    // Won't close immediately
    // Sends end signal to worker
    FrameDeviceInfo frame;
    frame.mode = ArnoldAnimationMode::STOPPED;

    m_queue.lock();
    m_queuedFrameDeviceInfo.push_back(frame);
    m_queue.unlock();

    // Sets to interactive mode, so worker can immediately process
    // the end info.
    startInteractive();
    
    // Waits until worker finishes its job
    if (m_worker != NULL)
        m_worker->join();
    m_worker = NULL;
    
    m_mode = ArnoldAnimationMode::STOPPED;
    
    // Close arnold interface
    ArnoldPatch::close();
}

ArnoldFrameManager *ArnoldAnimationPatch::getFrameManager() const {
    return m_frameManager;
}

void ArnoldAnimationPatch::reset() {
    // We want to block both worker and main thread during resetting
    m_queue.lock();
    m_mode = ArnoldAnimationMode::INTERACTIVE;
    
    // Resets start point to init.
    m_startPoint = chrono::system_clock::from_time_t(0);
    
    // Clears queue.
    for (FrameDeviceInfo d : m_queuedFrameDeviceInfo) {
        d.clear();
    }
    
    m_queuedFrameDeviceInfo.clear();
    
    // Just in case that close()'s already been called.
    if (m_worker != NULL) {
        // Manully interrupts worker here.
        m_interface.interrupt();
    }
    else {
        // Reset() can set a closed ArnoldAnimationPatch to its initial state.
        m_worker = new std::thread(&ArnoldAnimationPatch::workerLoop, this);
    }
    
	// Resets the frame manager to the beginning of current video.
    m_frameManager->reset();
    
    m_queue.unlock();
}

void ArnoldAnimationPatch::stop() {
    m_mode = ArnoldAnimationMode::STOPPED;
}

void ArnoldAnimationPatch::endRecording() {
    if (m_queuedFrameDeviceInfo.size() > 0)
        m_mode = ArnoldAnimationMode::RENDERING;
    else
        m_mode = ArnoldAnimationMode::INTERACTIVE;
}

void ArnoldAnimationPatch::setPreviewSamples(int preview) {
    m_preview_samples = preview;
    if (m_mode == ArnoldAnimationMode::INTERACTIVE)
        setSamples(m_preview_samples);
}
    
void ArnoldAnimationPatch::setRenderSamples(int render) {
    m_render_samples = render;
}

int ArnoldAnimationPatch::addFinishedCallback(FinishedCallbackFunction func) {
    size_t id = m_onFinishedFunctions.size();
    m_onFinishedFunctions[id] = func;
    
    return id;
}
    
void ArnoldAnimationPatch::deleteFinishedCallback(int id) {
    if (m_onFinishedFunctions.count(id) > 0) {
        m_onFinishedFunctions.erase(id);
    }
}
    
float ArnoldAnimationPatch::getPercentage() const {
    // Rough number.
    if (m_mode == ArnoldAnimationMode::INTERACTIVE)
		return ArnoldPatch::getPercentage();
	else if (m_mode == ArnoldAnimationMode::RENDERING) {
		size_t finished = m_frameManager->getFrameNum();
		size_t sum = finished + m_queuedFrameDeviceInfo.size() + 1;
		float renderingPer = m_interface.getPercentage();
		sum = (sum == 0) ? 1 : sum;

		return ((float)finished * 100.f + renderingPer) / sum;
	}
}
    
//============================ Worker Code =========================

void ArnoldAnimationPatch::onWorkerFinished(){
    for (auto func : m_onFinishedFunctions) {
        func.second();
    }
}
    
void ArnoldAnimationPatch::workerLoop() {
    FrameDeviceInfo frame;

    while(1) {
	// Releases the lock immediately if the queue is still empty.
	while (frame.time < 0) {
		if (m_mode == ArnoldAnimationMode::STOPPED)
			continue;

	    m_queue.lock();
        if (m_queuedFrameDeviceInfo.size() > 0) {
            if (m_mode == ArnoldAnimationMode::RECORDING) {
                // Clears irrelated info
                std::vector<FrameDeviceInfo>::iterator i;
                for (i = m_queuedFrameDeviceInfo.begin();
                     i != m_queuedFrameDeviceInfo.end() &&
                     i->mode == ArnoldAnimationMode::INTERACTIVE; i++) {
                    i->clear();
                }
                m_queuedFrameDeviceInfo.erase(m_queuedFrameDeviceInfo.begin(), i);
                
                // Deep copy (since we're going to clear it)
                for (i = m_queuedFrameDeviceInfo.begin();
                     i != m_queuedFrameDeviceInfo.end(); i++) {
                    // Copies the last recording info to do preview
                    if (i->mode == ArnoldAnimationMode::RECORDING) {
                        frame.copyByValue(*i);
                        // Waits to be rendered with higher sampling rate
                        i->mode = ArnoldAnimationMode::RENDERING;
                    }
                }
            }
            else if (m_mode == ArnoldAnimationMode::RENDERING) {
				// No frame would be skipped.
                frame = m_queuedFrameDeviceInfo[0];
                m_queuedFrameDeviceInfo.erase(m_queuedFrameDeviceInfo.begin());
            }
            else if (m_mode == ArnoldAnimationMode::INTERACTIVE) {
                frame = m_queuedFrameDeviceInfo.back();
                for (FrameDeviceInfo &info : m_queuedFrameDeviceInfo) {
                    if (info.time != m_queuedFrameDeviceInfo.back().time)
                        info.clear();
                }
                m_queuedFrameDeviceInfo.clear();
            }
        }
        // No more frames for rendering
        else if (m_mode == ArnoldAnimationMode::RENDERING) {
            setSamples(m_preview_samples);
            m_mode = ArnoldAnimationMode::INTERACTIVE;
            
            onWorkerFinished();
        }
	    m_queue.unlock();

		// The frame is valid.
        if (frame.time >= 0) {
            if (frame.mode == ArnoldAnimationMode::STOPPED) {
                m_mode = ArnoldAnimationMode::STOPPED;
                Logger::log(INFO, "Worker stopped...");
				onWorkerFinished();

                return ;
            }
            else if (frame.mode == ArnoldAnimationMode::RECORDING) {
                setSamples(m_preview_samples);
            }
            else if (frame.mode == ArnoldAnimationMode::RENDERING) {
                setSamples(m_render_samples);
            }
            // Finished recording work
            else if (frame.mode == ArnoldAnimationMode::INTERACTIVE &&
                     (m_mode == ArnoldAnimationMode::RENDERING)) {
                setSamples(m_preview_samples);
                m_mode = ArnoldAnimationMode::INTERACTIVE;
                
                onWorkerFinished();
            }
        }
	}

    std::stringstream ss;
    ss << "Received new frame: " << frame.time << "(" << frame.mode << ")";
    Logger::log(LDEBUG, ss.str());
        
	updateLight(frame.devices);
    int code = m_interface.render();
            
	// Dumps only when the image was rendered successfully for rendering.
    // If the worker was reset while rendering, doesn't dump.
    if (code == AI_SUCCESS &&
        (frame.mode == ArnoldAnimationMode::RENDERING)) {
        m_frameManager->dump(frame.time, m_interface.getBufferPointer(),
                             m_interface.getWidth(), m_interface.getHeight());
    }

	// Releases copies of devices.
	frame.clear();
    }
}

}
