
#include "ArnoldAnimationPatch.h"

#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
ArnoldAnimationPatch::ArnoldAnimationPatch(const JSONNode data)
: m_worker(NULL), m_startPoint(chrono::system_clock::from_time_t(0)){
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

    // Since there aren't many competitions (worker runs slowly),
    // it's okay to just use a coarse lock.
    m_queue.lock();
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

    // Waits until worker finishes its job
    m_worker->join();
    m_worker = NULL;
    
    // Close arnold interface
    ArnoldPatch::close();
}

ArnoldFrameManager *ArnoldAnimationPatch::getFrameManager() const {
    return m_frameManager;
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
		// shallow copy
		frame = m_queuedFrameDeviceInfo[0];
		m_queuedFrameDeviceInfo.erase(m_queuedFrameDeviceInfo.begin());
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
	renderLoop();
            
	// dumps
	m_frameManager->dump(frame.time, m_interface.getBufferPointer(),
			     m_interface.getWidth(), m_interface.getHeight());

	// Releases copies of devices.
	frame.clear();
    }
}

}
