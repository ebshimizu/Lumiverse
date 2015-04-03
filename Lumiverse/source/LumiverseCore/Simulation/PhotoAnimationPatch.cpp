#include "PhotoAnimationPatch.h"
#ifdef USE_ARNOLD
#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
PhotoAnimationPatch::PhotoAnimationPatch(const JSONNode data) :
	SimulationAnimationPatch(),m_preview_samples(-3),m_render_samples(-3) {
	m_mem_frameManager = new ArnoldMemoryFrameManager();
	loadJSON(data);
}
    
PhotoAnimationPatch::~PhotoAnimationPatch() {
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

/* rerender copied from ArnoldAnimationPatch */
void PhotoAnimationPatch::rerender() {
	// Sets all rerendering flags to true to preserve the first frame
	if (m_lights.size() > 0)
		for (auto light : m_lights)
			m_lights[light.first]->rerender_req = true;

	// Use this threshold to avoid deadlock caused by worry device settings
	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();

	// When patch is working, makes sure the request be processed
	bool flag = true;
	while (flag && m_mode != SimulationAnimationMode::STOPPED) {
		if (m_lights.size() > 0)
			for (auto light : m_lights) {
			flag &= m_lights[light.first]->rerender_req;
			if (!flag)
				return;
			}
		else
			return;

		chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
		if (chrono::duration_cast<chrono::seconds>(current - start).count() > 1)
			return;
	}
}

void PhotoAnimationPatch::interruptRender() {
	if (m_mode == SimulationAnimationMode::RECORDING ||
		m_mode == SimulationAnimationMode::INTERACTIVE)
		PhotoPatch::interruptRender();
	else if (m_mode == SimulationAnimationMode::RENDERING) {
		PhotoPatch::interruptRender();

		// Clear queue (turn rendering tasks into interactive tasks)
		m_queue.lock();
		for (auto i = m_queuedFrameDeviceInfo.begin();
			i != m_queuedFrameDeviceInfo.end(); i++) {
			i->mode = SimulationAnimationMode::INTERACTIVE;
		}
		m_queue.unlock();
	}
}

void PhotoAnimationPatch::setPreviewSamples(int preview) {
	m_preview_samples = preview;
}

void PhotoAnimationPatch::setRenderSamples(int render) {
	m_render_samples = render;
}

}
#endif