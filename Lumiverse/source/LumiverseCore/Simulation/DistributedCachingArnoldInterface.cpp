#include "DistributedCachingArnoldInterface.h"
#include "CachingArnoldInterface.h"
#include "DistributedArnoldInterface.h"

#include "types/LumiverseFloat.h"
#include <assert.h>

#include <ai.h>

#ifdef USE_ARNOLD
#ifdef USE_DUMIVERSE

#ifdef USE_ARNOLD_CACHING

namespace Lumiverse {

	void DistributedCachingArnoldInterface::init(const JSONNode jsonPatch) {
		DistributedArnoldInterface::init(jsonPatch);
		DistributedArnoldInterface::setDims(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		setOptionParameter("m_gamma", 1.f);
		m_width = DEFAULT_WIDTH;
		m_height = DEFAULT_HEIGHT;
		m_open = true;
	}

	void DistributedCachingArnoldInterface::close() {
		delete[] m_render_buffer;
		compositor.get_layers().clear();

		DistributedArnoldInterface::close();
		m_open = false;
	}

	bool DistributedCachingArnoldInterface::setDims(int w, int h) {
		return CachingArnoldInterface::setDims(w, h);
	}

	void DistributedCachingArnoldInterface::updateDevicesLayers(const std::set<Device *> &devices) {
		const std::unordered_map<std::string, Device *> &to_update = getDevicesToUpdate(devices);

		// Maps to hold the intensities and colors of the devices
		std::unordered_map<std::string, LumiverseFloat*> current_intensities;
		std::unordered_map<std::string, LumiverseColor*> current_channels;

		// Add devices to compositor if they don't exist
		for (auto i = to_update.begin(); i != to_update.end(); i++) {
			std::string device_name = i->first;
			if (!compositor.contains_layer(device_name.c_str())) {
				Pixel4 *pixels = new Pixel4[m_width * m_height];
				EXRLayer *new_layer = new EXRLayer(pixels, m_width, m_height, device_name.c_str());
				compositor.add_layer(new_layer);
			}
		}

		// Set intensity to 0 and color to 1.f 1.f 1.f for all devices
		for (Device *device : devices) {
			std::string device_name = device->getMetadata("Arnold Node Name");
			current_intensities[device_name] = new LumiverseFloat(device->getIntensity());
			current_channels[device_name] = new LumiverseColor(device->getColor());
			device->setIntensity(0.0);
			device->setColorRGB("color", 1.f, 1.f, 1.f);
		}

		// foreach device, check if we need to render, and fire off request to distributed renderer
		for (auto i = to_update.begin(); i != to_update.end(); i++) {
			Device *render_device = i->second;
			std::string device_name = i->first;

			// Set intensity to max
			render_device->setIntensity(current_intensities[device_name]->getMax());
			DistributedArnoldInterface::render(devices);
			EXRLayer *layer = compositor.get_layer_by_name(device_name.c_str());
			layer->set_pixels(DistributedArnoldInterface::m_buffer);

			// Now that we have the buffer in the layer, set intensity back to 0 for next device
			render_device->setIntensity(0);
		}

		// Reset intensity and color
		for (Device *device : devices) {
			std::string device_name = device->getMetadata("Arnold Node Name");
			device->setParam("intensity", (LumiverseFloat*)current_intensities[device_name]);
			LumiverseColor *color = current_channels[device_name];
			device->setParam("color", color);
		}
	}

	void DistributedCachingArnoldInterface::setSamples(int samples) {
		force_cache_reload = true;

		ArnoldInterface::setSamples(samples);
	}

	int DistributedCachingArnoldInterface::render(const std::set<Device *> &devices) {

		updateDevicesLayers(devices);

		tone_mapper.set_gamma(2.2f);

		dumpHDRToBuffer(devices);

		force_cache_reload = false;

		return AI_SUCCESS;
	}

	void DistributedCachingArnoldInterface::dumpHDRToBuffer(const std::set<Device *> &devices) {

		if (DistributedArnoldInterface::m_buffer == NULL) {
			std::cerr << "Buffer not set" << std::endl;
			return;
		}

		compositor.render(devices);
		tone_mapper.set_input(compositor.get_compose_buffer(), CachingArnoldInterface::m_width, CachingArnoldInterface::m_height);
		setHDROutputBuffer();
		tone_mapper.apply_hdr();
	}

	void DistributedCachingArnoldInterface::setHDROutputBuffer() {
		if (m_render_buffer != NULL) {
			delete[] m_render_buffer;
		}
		m_render_buffer = new float[m_width * m_height * 4];

		tone_mapper.set_output_hdr(m_render_buffer);
	}

	float *DistributedCachingArnoldInterface::getBufferPointer() {
		return m_render_buffer;
	}

	/*!
	\brief Sets a parameter found in the global options node in arnold
	*/
	void DistributedCachingArnoldInterface::setOptionParameter(const std::string &paramName, int val) {
		DistributedArnoldInterface::setOptionParameter(paramName, val);
		CachingArnoldInterface::setOptionParameter(paramName, val);
	}

	void DistributedCachingArnoldInterface::setOptionParameter(const std::string &paramName, float val) {
		DistributedArnoldInterface::setOptionParameter(paramName, val);
		CachingArnoldInterface::setOptionParameter(paramName, val);
	}
}

#endif // USE_ARNOLD_CACHING

#endif // USE_DUMIVERSE

#endif // USE_ARNOLD
