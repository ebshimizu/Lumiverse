#include "SimulationAnimationPatch.h"

#ifdef USE_ARNOLD
#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
SimulationAnimationPatch::SimulationAnimationPatch(const JSONNode data)
: m_worker(NULL), m_startPoint(chrono::system_clock::from_time_t(0)),
    m_mode(SimulationAnimationMode::STOPPED), 
	m_mem_frameManager(new ArnoldMemoryFrameManager()), m_file_frameManager(NULL) {
    // TODO: type for frame manager
	//m_mem_frameManager = new ArnoldMemoryFrameManager();
	//m_file_frameManager = NULL;
    loadJSON(data);
}
    
SimulationAnimationPatch::~SimulationAnimationPatch() {
    delete m_mem_frameManager;
	delete m_file_frameManager;

    // If close() hasn't been called, closes here.
    if (m_worker != NULL)
        close();
}

void SimulationAnimationPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();

	while (i != data.end()) {
		std::string nodeName = i->name();

		if (nodeName == "frameDirectory") {
			JSONNode fileName = *i;
			m_file_frameManager = new ArnoldFileFrameManager(fileName.as_string());
		}

		i++;
	}
}

void SimulationAnimationPatch::init() {
	// Cleans hooks to old callbacks
	m_onFinishedFunctions.clear();
}
    
void SimulationAnimationPatch::update(set<Device *> devices, 
	IsUpdateRequiredFunction isUpdateRequired,
	InterruptFunction interruptRender,
	ClearUpdateFlagsFunction clearUpdateFlags) {
	// Doesn't respond if it's stopped.
	if (m_mode == SimulationAnimationMode::STOPPED){
		return;
	}
    
    FrameDeviceInfo frame;
	
	// Fulls time and mode for frame info.
	createFrameInfoHeader(frame);

    // Checks if an update is needed.
    bool rerender_req = isUpdateRequired(devices);

    // There's no need to send this frame
	if (!rerender_req){
		return;
	}
    
	createFrameInfoBody(devices, frame);

    std::stringstream ss;
    ss << "Sent new frame: " << frame.time << "(" << frame.mode << ")";
    Logger::log(LDEBUG, ss.str());

	// Interrupts the current rendering if in the interactive mode.
    if (m_mode == SimulationAnimationMode::INTERACTIVE ||
		m_mode == SimulationAnimationMode::RECORDING) {
		interruptRender();
    }
    
	// Enqueues.
	enqueueFrameInfo(frame);

	// A flag cleared means the task has been indeed inserted
	clearUpdateFlags();
}

void SimulationAnimationPatch::enqueueFrameInfo(const FrameDeviceInfo &frame) {
	// Since there aren't many competitions (worker runs slowly),
	// it's okay to just use a coarse lock.
	//m_queue.lock();
	// Checks if start point was reset during this update call.
	if (m_startPoint != chrono::system_clock::from_time_t(0))
		m_queuedFrameDeviceInfo.push_back(frame);
	//m_queue.unlock();
}

void SimulationAnimationPatch::createFrameInfoHeader(FrameDeviceInfo &frame) {
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();

	// Sets the start point to the moment when update is called for the first time
	if (m_startPoint == chrono::system_clock::from_time_t(0))
		m_startPoint = current;

	frame.time = chrono::duration_cast<chrono::milliseconds>(current - m_startPoint).count();

	// A new Interactive frame indicates the end of Rendering tasks.
	// Note that all Rendering frames are created by the worker thread.
	if (m_mode == SimulationAnimationMode::RENDERING ||
		m_mode == SimulationAnimationMode::INTERACTIVE)
		frame.mode = SimulationAnimationMode::INTERACTIVE;
	else if (m_mode == SimulationAnimationMode::RECORDING)
		frame.mode = SimulationAnimationMode::RECORDING;

}

void SimulationAnimationPatch::close() {
    // Won't close immediately
    // Sends end signal to worker
    FrameDeviceInfo frame;
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
	frame.time = chrono::duration_cast<chrono::milliseconds>(current - m_startPoint).count();
    frame.mode = SimulationAnimationMode::STOPPED;

    m_queue.lock();
    m_queuedFrameDeviceInfo.push_back(frame);
    m_queue.unlock();

    // Sets to interactive mode, so worker can immediately process
    // the end info.
    startInteractive();
    
    // Waits until worker finishes its job
	if (m_worker != NULL) {
		m_worker->join();
	}

	delete m_worker;
    m_worker = NULL;
    
    m_mode = SimulationAnimationMode::STOPPED;
}

ArnoldFrameManager *SimulationAnimationPatch::getFrameManager() const {
    return m_mem_frameManager;
}

void SimulationAnimationPatch::reset(InterruptFunction interruptRender) {
    // We want to block both worker and main thread during resetting
    m_queue.lock();
    m_mode = SimulationAnimationMode::INTERACTIVE;

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
        interruptRender();
    }
    else {
        // Reset() can set a closed SimulationAnimationPatch to its initial state.
        m_worker = new std::thread(&SimulationAnimationPatch::workerLoop, this);
    }
    
	// Resets the frame manager to the beginning of current video.
    m_mem_frameManager->reset();
	if (m_file_frameManager)
		m_file_frameManager->reset();
    
    m_queue.unlock();
}

void SimulationAnimationPatch::stop() {
    m_mode = SimulationAnimationMode::STOPPED;
}

void SimulationAnimationPatch::endRecording() {
    if (m_queuedFrameDeviceInfo.size() > 0)
        m_mode = SimulationAnimationMode::RENDERING;
    else
        m_mode = SimulationAnimationMode::INTERACTIVE;
}

int SimulationAnimationPatch::addFinishedCallback(FinishedCallbackFunction func) {
    size_t id = m_onFinishedFunctions.size();
    m_onFinishedFunctions[id] = func;
    
    return id;
}
    
void SimulationAnimationPatch::deleteFinishedCallback(int id) {
    if (m_onFinishedFunctions.count(id) > 0) {
        m_onFinishedFunctions.erase(id);
    }
}
    
//============================ Worker Code =========================

void SimulationAnimationPatch::onWorkerFinished(){
	onRecording();
	m_mode = SimulationAnimationMode::INTERACTIVE;

    for (auto func : m_onFinishedFunctions) {
        func.second();
    }
}
    


void SimulationAnimationPatch::workerLoop() {
    FrameDeviceInfo frame;

    while(1) {
		// Releases the lock immediately if the queue is still empty.
		while (frame.time < 0) {
			if (m_mode == SimulationAnimationMode::STOPPED)
				continue;

			m_queue.lock();
			if (m_queuedFrameDeviceInfo.size() > 0) {
				if (m_mode == SimulationAnimationMode::RECORDING) {
					// Clears irrelated info
					std::vector<FrameDeviceInfo>::iterator i;
					for (i = m_queuedFrameDeviceInfo.begin();
						 i != m_queuedFrameDeviceInfo.end() &&
						 i->mode == SimulationAnimationMode::INTERACTIVE; i++) {
						i->clear();
					}
					m_queuedFrameDeviceInfo.erase(m_queuedFrameDeviceInfo.begin(), i);
                
					// Deep copy (since we're going to clear it)
					for (i = m_queuedFrameDeviceInfo.begin();
						 i != m_queuedFrameDeviceInfo.end(); i++) {
						// Copies the last recording info to do preview
						if (i->mode == SimulationAnimationMode::RECORDING) {
							frame.copyByValue(*i);
							// Waits to be rendered with higher sampling rate
							i->mode = SimulationAnimationMode::RENDERING;
						}
					}
				}
				else if (m_mode == SimulationAnimationMode::RENDERING) {
					// No frame would be skipped.
					frame = m_queuedFrameDeviceInfo[0];
					m_queuedFrameDeviceInfo.erase(m_queuedFrameDeviceInfo.begin());
				}
				else if (m_mode == SimulationAnimationMode::INTERACTIVE) {
					frame = m_queuedFrameDeviceInfo.back();
					for (FrameDeviceInfo &info : m_queuedFrameDeviceInfo) {
						if (info.time != m_queuedFrameDeviceInfo.back().time)
							info.clear();
					}
					m_queuedFrameDeviceInfo.clear();
				}
			}
			// No more frames for rendering
			else if (m_mode == SimulationAnimationMode::RENDERING) {
				onWorkerFinished();
			}
			m_queue.unlock();

			// The frame is valid.
			if (frame.time >= 0) {
				if (frame.mode == SimulationAnimationMode::STOPPED) {
					m_mode = SimulationAnimationMode::STOPPED;
					Logger::log(INFO, "Worker stopped...");

					return ;
				}
				else if (frame.mode == SimulationAnimationMode::RECORDING) {
					onRecording();
				}
				else if (frame.mode == SimulationAnimationMode::RENDERING) {
					onRendering();
				}
				// Finished recording work
				else if (frame.mode == SimulationAnimationMode::INTERACTIVE &&
						 (m_mode == SimulationAnimationMode::RENDERING)) {
					onWorkerFinished();
				}
			}
		}

		// Releases copies of devices.
		workerRender(frame);
		frame.clear();
    }
}

}
#endif