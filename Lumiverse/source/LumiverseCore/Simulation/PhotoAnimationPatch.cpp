
#include "PhotoAnimationPatch.h"

#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
PhotoAnimationPatch::PhotoAnimationPatch(const JSONNode data) :
	SimulationAnimationPatch() {
	m_mem_frameManager = new ArnoldMemoryFrameManager();
	loadJSON(data);
}
    
PhotoAnimationPatch::~PhotoAnimationPatch() {
    delete m_mem_frameManager;
	delete m_file_frameManager;

    // If close() hasn't been called, closes here.
    if (m_worker != NULL)
        close();
}

void PhotoAnimationPatch::loadJSON(const JSONNode data) {
	PhotoPatch::loadJSON(data);
	SimulationAnimationPatch::loadJSON(data);
}

void PhotoAnimationPatch::init() {
	PhotoPatch::init();
    SimulationAnimationPatch::init();
}
    
void PhotoAnimationPatch::update(set<Device *> devices) {
	SimulationAnimationPatch::IsUpdateRequiredFunction updateRequired = 
		std::bind(&SimulationPatch::isUpdateRequired, (SimulationPatch*)this, std::placeholders::_1);
	SimulationAnimationPatch::InterruptFunction interrupt =
		std::bind(&SimulationPatch::interruptRender, (SimulationPatch*)this);
	SimulationAnimationPatch::ClearUpdateFlagsFunction clearUpdateFlags =
		std::bind(&SimulationPatch::clearUpdateFlags, (SimulationPatch*)this);
	SimulationAnimationPatch::update(devices, updateRequired, interrupt, clearUpdateFlags);
}

void PhotoAnimationPatch::close() {
    // Close
	SimulationAnimationPatch::close();
	PhotoPatch::close();
}

void PhotoAnimationPatch::workerRender(FrameDeviceInfo frame) {
	std::stringstream ss;
	ss << "Received new frame: " << frame.time << "(" << frame.mode << ")";
	Logger::log(LDEBUG, ss.str());

	updateLight(frame.devices);
	bool success = PhotoPatch::renderLoop();

	// Dumps only when the image was rendered successfully for rendering.
	// If the worker was reset while rendering, doesn't dump.
	if (success && frame.mode == SimulationAnimationMode::RENDERING) {
		m_mem_frameManager->dump(frame.time, getBufferPointer(),
			getWidth(), getHeight());
		if (m_file_frameManager)
			m_file_frameManager->dump(frame.time, getBufferPointer(),
			getWidth(), getHeight());
	}
}

void PhotoAnimationPatch::createFrameInfoBody(set<Device *> devices, FrameDeviceInfo &frame) {
	for (Device *d : devices) {
		// Checks if the device is connect to this patch
		if (m_lights.count(d->getId()) > 0 &&
			m_lights[d->getId()]->rerender_req) {
			// Makes copy of this device
			Device *d_copy = new Device(*d);
			frame.devices.insert(d_copy);
		}
	}
}

void PhotoAnimationPatch::reset() {
	SimulationAnimationPatch::InterruptFunction interrupt =
		std::bind(&SimulationPatch::interruptRender, (SimulationPatch*)this);
	SimulationAnimationPatch::reset(interrupt);
}

}
