
#include "ArnoldAnimationPatch.h"

#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
ArnoldAnimationPatch::ArnoldAnimationPatch(const JSONNode data)
: m_worker(NULL), m_startPoint(chrono::system_clock::from_time_t(0)),
    m_mode(ArnoldAnimationMode::INTERACTIVE) {
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

    // Starts a worker thread.
    m_worker = new std::thread(&ArnoldAnimationPatch::workerLoop, this);
}
    
void ArnoldAnimationPatch::update(set<Device *> devices) {
    if (m_mode == ArnoldAnimationMode::STOPPED)
        return ;
    
    FrameDeviceInfo frame;
    chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();

    // Sets the start point to the moment when update is called for the first time
    if (m_startPoint == chrono::system_clock::from_time_t(0))
	m_startPoint = current;

    frame.time = chrono::duration_cast<chrono::milliseconds>(current - m_startPoint).count();

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
	}
    }

    std::stringstream ss;
    ss << "Sent new frame: " << frame.time;
    Logger::log(LDEBUG, ss.str());

    if (m_mode == ArnoldAnimationMode::INTERACTIVE) {
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
    frame.devices.insert(NULL);

    m_queue.lock();
    m_queuedFrameDeviceInfo.push_back(frame);
    m_queue.unlock();

    // Sets to interactive mode, so worker can immediately process
    // the end info.
    startInteractive();
    
    // Waits until worker finishes its job
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

    m_frameManager->clear();
    
    m_mode = ArnoldAnimationMode::INTERACTIVE;
    
    m_queue.unlock();
}

void ArnoldAnimationPatch::stop() {
    m_mode = ArnoldAnimationMode::STOPPED;
}

//============================ Worker Code =========================

bool ArnoldAnimationPatch::isEndInfo(const FrameDeviceInfo &data) const {
    if (data.devices.size() == 1 && *(data.devices.begin()) == NULL)
	return true;
    return false;
}

void ArnoldAnimationPatch::workerLoop() {
    FrameDeviceInfo frame;

    while(1) {
	// Releases the lock immediately if the queue is still empty.
	while (frame.time < 0) {
	    m_queue.lock();
        if (m_queuedFrameDeviceInfo.size() > 0) {
            if (m_mode == ArnoldAnimationMode::RECORDING) {
                // shallow copy
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
	    m_queue.unlock();

	    if (isEndInfo(frame)) {
            Logger::log(INFO, "Worker finished...");
            return ;
	    }
	}

    std::stringstream ss;
    ss << "Received new frame: " << frame.time;
    Logger::log(LDEBUG, ss.str());
        
	updateLight(frame.devices);
    int code = m_interface.render();
            
	// Dumps only when the image was rendered successfully.
    // If the worker was reset while rendering, doesn't dump.
    if (code == AI_SUCCESS && m_mode == ArnoldAnimationMode::RECORDING) {
        m_frameManager->dump(frame.time, m_interface.getBufferPointer(),
                             m_interface.getWidth(), m_interface.getHeight());
    }

	// Releases copies of devices.
	frame.clear();
    }
}

}
