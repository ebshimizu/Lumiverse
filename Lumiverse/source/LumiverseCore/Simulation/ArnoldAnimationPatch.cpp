
#include "ArnoldAnimationPatch.h"

namespace Lumiverse {

ArnoldAnimationPatch::ArnoldAnimationPatch(const JSONNode data)
: m_worker(NULL), m_startPoint(chrono::system_clock::from_time_t(0)){
	m_frameManager = new ArnoldMemoryFrameManager();
	loadJSON(data);
}

void ArnoldAnimationPatch::init() {
	ArnoldPatch::init();
	m_worker = new std::thread(&ArnoldAnimationPatch::workerLoop, this);
}

void ArnoldAnimationPatch::update(set<Device *> devices) {
	FrameDeviceInfo frame;
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();

	// Sets the start point to the moment when update is called for the first time
	if (m_startPoint == chrono::system_clock::from_time_t(0))
		m_startPoint = current;

	frame.time = chrono::duration_cast<chrono::seconds>(current - m_startPoint).count();
	for (Device *d : devices) {
		// Checks if the device is connect to this patch
		if (m_lights.count(d->getId()) > 0) {
			// Makes copy of this device
			Device *d_copy = new Device(*d);
			frame.devices.insert(d_copy);
		}
	}

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

	// Close arnold interface
	ArnoldPatch::close();
}

ArnoldFrameManager *ArnoldAnimationPatch::getFrameManager() const {
	return m_frameManager;
}
//============================
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
				return ;
			}
		}

		// render
		bool render_req = updateLight(frame.devices);

		if (render_req) {
			renderLoop();

			for (Device* d : frame.devices) {
				std::string name = d->getId();
				if (m_lights.count(name) == 0)
					continue;
				m_lights[name].rerender_req = false;
			}
		}

		//dump
		m_frameManager->dump(frame.time, m_interface.getBufferPointer(), 
			m_interface.getWidth(), m_interface.getHeight());

		frame.clear();
	}
}

}
