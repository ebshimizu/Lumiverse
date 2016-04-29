#include "CachingArnoldInterface.h"

#include "types/LumiverseFloat.h"
#include "ImfTestFile.h"    // Header checks
#include "ImfInputFile.h"   // Imf file IO
#include "ImfOutputFile.h"  // Imf file IO
#include "ImfRgbaFile.h"    // Imf RGBA scaneline
#include "ImfChannelList.h" // Imf channels
#include "ImathBox.h"
#include <assert.h>

#include <ai.h>

#ifdef USE_ARNOLD

#ifdef USE_ARNOLD_CACHING

namespace Lumiverse {

	void CachingArnoldInterface::init() {
		AiBegin();

		setLogFileName("arnold.log");

		// Keeps directory of plugins absolute.
		AiLoadPlugins(m_plugin_dir.c_str());

		// Load everything from the scene file
		AiASSLoad(toRelativePath(m_ass_file).c_str(), AI_NODE_ALL);

		// get size information
		AtNode *options = AiUniverseGetOptions();
		/*
		m_width = AiNodeGetInt(options, "xres");
		m_height = AiNodeGetInt(options, "yres");
		*/
		// By default, render a big image
		this->setDims(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		m_width = DEFAULT_WIDTH;
		m_height = DEFAULT_HEIGHT;
		AiNodeSetInt(options, "xres", m_width);
		AiNodeSetInt(options, "yres", m_height);
		m_samples = AiNodeGetInt(options, "AA_samples");
		delete[] m_buffer;
		m_buffer = new float[m_width * m_height * 4];
		this->setHDROutputBuffer();

		// Set a driver to output result into a float buffer
		AtNode *driver = AiNode("cache_buffer");

		m_bufDriverName = "cache_buffer";
		std::stringstream ss;
		ss << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() -
			chrono::system_clock::from_time_t(0)).count() % 1000;
		m_bufDriverName = m_bufDriverName.append(ss.str());

		AiNodeSetStr(driver, "name", m_bufDriverName.c_str());
		AiNodeSetInt(driver, "width", m_width);
		AiNodeSetInt(driver, "height", m_height);
		AiNodeSetFlt(driver, "gamma", 1.f);

		m_render_buffer = new float[m_width * m_height * 4];
		AiNodeSetPtr(driver, "buffer_pointer", m_render_buffer);
		// Swapping threads more than hardware supports may cause problem.
		m_bucket_num = std::thread::hardware_concurrency();

		delete[] m_bucket_pos;
		m_bucket_pos = new BucketPositionInfo[m_bucket_num];
		AiNodeSetPtr(driver, "bucket_pos_pointer", m_bucket_pos);

		AiNodeSetPtr(driver, "progress_pointer", &m_progress);

		// create a filter - override filter in ass file
		AtNode *filter = AiNode("gaussian_filter");
		AiNodeSetStr(filter, "name", "filter");
		AiNodeSetFlt(filter, "width", 2);

		std::string command("RGBA RGBA filter ");
		appendToOutputs(command.append(m_bufDriverName).c_str());

		// add layers
		// this first records each of the light's color and intensity
		// as the layer multiplier information and then disables all lights.
		// Then enable one light at a time and generates per-light renderings
		// using arnold.
		std::cout << "Filling cache..." << std::endl;

		// find all mesh lights
		size_t num_lights = 0;
		// AtString rgb_str("color");
		AtNodeIterator *it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
		while (!AiNodeIteratorFinished(it)) {

			AtNode *light = AiNodeIteratorGetNext(it);

			// create new layer
			string name = AiNodeGetStr(light, "name");
			Pixel4 *pixels = new Pixel4[m_width * m_height]();
			EXRLayer *layer = new EXRLayer(pixels, m_width, m_height, name.c_str());

			// Disable layer by default -- enable when we read light nodes from scene in render()
			layer->disable();

			AtRGB rgb = AiNodeGetRGB(light, "color");
			layer->set_modulator(Pixel3(rgb.r, rgb.g, rgb.b));

			// add layer to compositor (render later)
			compositor.add_layer(layer);
			std::cout << "Created layer: " << name << std::endl;

			// disable light
			// note that this does not disable the mesh
			// and the light shape will be rendered as white
			// will need to override the mesh light metarial
			// color to completely take the light out of
			// the scene
			AiNodeSetDisabled(light, true);

			// increment count
			num_lights++;
		}
		AiNodeIteratorDestroy(it);

		/*
		 Note that we don't actually render any layers on init. We wait until
		 render is called with a set of devices because we want to be able
		 to check if any of the layers need to be re-rendered (i.e. if a
		 light is rotated).
		*/

		m_open = true;
	}

	void CachingArnoldInterface::close() {
		
		// @TODO: Clean stuff up
		delete[] m_render_buffer;

		ArnoldInterface::close();
	}

	bool CachingArnoldInterface::setDims(int w, int h)  {
		bool success = ArnoldInterface::setDims(w, h);

		if (success) {
			setHDROutputBuffer();
			compositor.update_dims(w, h);
			tone_mapper.update_dims(w, h);
		}
		return success;
	}

	const std::unordered_map<std::string, Device*> CachingArnoldInterface::getDevicesToUpdate(const std::set<Device *> &devices) {
		std::unordered_map<std::string, Device *> to_update;

		for (Device *device : devices) {
			std::string device_name = device->getMetadata("Arnold Node Name");
			if (!force_cache_reload && (cached_devices.count(device_name) > 0)) {
				Device *cached = cached_devices.at(device_name);
				if (!isValidCacheCopy(cached, device)) {
					cached_devices[device_name] = device;
					to_update[device_name] = device;
				}
			}
			else {
				cached_devices[device_name] = device;
				to_update[device_name] = device;
			}
		}

		return to_update;
	}

	void CachingArnoldInterface::updateDevicesLayers(const std::set<Device *> &devices) {
		const std::unordered_map<std::string, Device *> &to_update = getDevicesToUpdate(devices);

		// If no updates are necessary just return to avoid the overhead
		// of calling malloc, iterating over the light nodes, etc
		if (to_update.size() == 0) {
			return;
		}

		memset(m_render_buffer, 0, m_width * m_height * 4 * sizeof(float));

		// render each per-light layer
		std::cout << "Rendering layers" << std::endl;
		AtNodeIterator *it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
		while (!AiNodeIteratorFinished(it)) {
			AtNode *light = AiNodeIteratorGetNext(it);
			std::string name = AiNodeGetStr(light, "name");

			// Only re-render if the node has been updated
			if (to_update.count(name) == 0) {
				continue;
			}

			Device *curr_device = to_update.at(name);
			LumiverseFloat *intensity_float = curr_device->getIntensity();

			// A light device should always have an intensity parameter
			assert(intensity_float != nullptr);

			// enable light
			AiNodeSetDisabled(light, false);
			
			// render image
			AiNodeSetFlt(light, "intensity", intensity_float->getMax());
			// AiNodeSetFlt(light, "intensity", intensity_float->getVal());

			Eigen::Vector3d modulator = curr_device->getColor()->getRGB();
			/*
			float r = modulator.x();
			float g = modulator.y();
			float b = modulator.z();
			*/
			// AiNodeSetRGB(light, "color", r, g, b);

			AiNodeSetRGB(light, "color", 1.f, 1.f, 1.f);
			AiRender(AI_RENDER_MODE_CAMERA);

			// copy to layer buffer
			EXRLayer *layer = compositor.get_layer_by_name(name.c_str());
			layer->clear_buffer();

			Pixel4 *layer_buffer = layer->get_pixels();
			for (size_t idx = 0; idx < m_width * m_height; ++idx) {
				int buf_idx = idx * 4;
				layer_buffer[idx].r = m_render_buffer[buf_idx];
				layer_buffer[idx].g = m_render_buffer[buf_idx + 1];
				layer_buffer[idx].b = m_render_buffer[buf_idx + 2];
				layer_buffer[idx].a = !!m_render_buffer[buf_idx + 3];
			}

			layer->enable();

			// disable light
			AiNodeSetDisabled(light, true);
			memset(m_render_buffer, 0, m_width * m_height * 4 * sizeof(float));
		}
		AiNodeIteratorDestroy(it);
	}

	void CachingArnoldInterface::setSamples(int samples) {
		force_cache_reload = true;

		ArnoldInterface::setSamples(samples);
	}

	int CachingArnoldInterface::render(const std::set<Device *> &devices) {

		updateDevicesLayers(devices);

		tone_mapper.set_gamma(m_gamma);

		dumpHDRToBuffer(devices);

		force_cache_reload = false;

		return AI_SUCCESS;
	}

	void CachingArnoldInterface::setHDROutputBuffer() {

		tone_mapper.set_output_hdr(m_buffer);
	}

	void CachingArnoldInterface::dumpHDRToBuffer(const std::set<Device *> &devices) {

		if (m_buffer == NULL) {
			std::cerr << "Buffer not set" << std::endl;
			return;
		}

		compositor.render(devices);
		tone_mapper.set_input(compositor.get_compose_buffer(), m_width, m_height);
		tone_mapper.apply_hdr();
	}

	/*!
	\brief Sets a parameter found in the global options node in arnold
	*/
	void CachingArnoldInterface::setOptionParameter(const std::string &paramName, int val) {
		if (optionRequiresCacheReload(paramName)) {
			force_cache_reload = true;
		}

		ArnoldInterface::setOptionParameter(paramName, val);
	}


	bool CachingArnoldInterface::isValidCacheCopy(Device *cached_device, Device *other_device) {
		if (cached_device->getId() != other_device->getId()) return false;
		if (cached_device->getType() != other_device->getType()) return false;

		// num parameter check
		if (cached_device->getRawParameters().size() != other_device->getRawParameters().size()) {
			return false;
		}

		// penumbra angle
		if (cached_device->paramExists("penumbraAngle")) {
			if (((LumiverseFloat *)(cached_device->getParam("penumbraAngle")))->getVal() != ((LumiverseFloat *)other_device->getParam("penumbraAngle"))->getVal()) {
				return false;
			}
		}

		// position
		if (cached_device->paramExists("lookAtX") && cached_device->paramExists("lookAtY") && cached_device->paramExists("lookAtZ")) {
			LumiverseFloat *cachedX = (LumiverseFloat *)cached_device->getParam("lookAtX");
			LumiverseFloat *cachedY = (LumiverseFloat *)cached_device->getParam("lookAtY");
			LumiverseFloat *cachedZ = (LumiverseFloat *)cached_device->getParam("lookAtZ");

			LumiverseFloat *otherX = (LumiverseFloat *)other_device->getParam("lookAtX");
			LumiverseFloat *otherY = (LumiverseFloat *)other_device->getParam("lookAtY");
			LumiverseFloat *otherZ = (LumiverseFloat *)other_device->getParam("lookAtZ");

			if ((cachedX->getVal() != otherX->getVal()) || (cachedY->getVal() != otherY->getVal()) || (cachedZ->getVal() != otherZ->getVal())) {
				return false;
			}
		}

		// rotation
		if (cached_device->paramExists("polar") && cached_device->paramExists("azimuth")) {
			LumiverseFloat *cachedPolar = (LumiverseFloat *)cached_device->getParam("polar");
			LumiverseFloat *cachedAzim = (LumiverseFloat *)cached_device->getParam("azimuth");

			LumiverseFloat *otherPolar = (LumiverseFloat *)cached_device->getParam("polar");
			LumiverseFloat *otherAzim = (LumiverseFloat *)cached_device->getParam("azimuth");

			if ((cachedPolar->getVal() != otherPolar->getVal()) || (cachedAzim->getVal() != otherAzim->getVal())) {
				return false;
			}
		}

		// distance
		if (cached_device->paramExists("distance")) {
			return ((LumiverseFloat *)cached_device->getParam("distance"))->getVal() == 
				((LumiverseFloat*)other_device->getParam("distance"))->getVal();
		}

		return true;
	}


	void CachingArnoldInterface::setOptionParameter(const std::string &paramName, float val) {
		if (optionRequiresCacheReload(paramName)) {
			force_cache_reload = true;
		}

		ArnoldInterface::setOptionParameter(paramName, val);
	}

	bool CachingArnoldInterface::optionRequiresCacheReload(const std::string &paramName) {
		return paramName == "AA_samples";
	}

	int CachingArnoldInterface::load_exr(const char *file_path) {

		// check header
		bool isTiled;
		bool isValid = OPENEXR_IMF_INTERNAL_NAMESPACE::isOpenExrFile(file_path, isTiled);
		if (isValid) {

			// check for tiled exr
			if (isTiled) {
				std::cerr << "Only scanline images are supported" << std::endl;
				return -1;
			}

		}
		else {

			// file not valid
			std::cerr << "Invalid input OpenEXR file: " << file_path;
			return -1;
		}

		// read dimensions
		OPENEXR_IMF_INTERNAL_NAMESPACE::InputFile file(file_path);
		IMATH_INTERNAL_NAMESPACE::Box2i dw = file.header().dataWindow();
		m_width = dw.max.x - dw.min.x + 1;
		m_height = dw.max.y - dw.min.y + 1;

		// read channel information
		const OPENEXR_IMF_INTERNAL_NAMESPACE::ChannelList &channels = file.header().channels();
		set<string> names;
		channels.layers(names);

		// read layers
		string layer_name;
		Pixel4 *pixels;
		if (names.size()) {

			// read pixels
			OPENEXR_IMF_INTERNAL_NAMESPACE::FrameBuffer frame_buffer;
			for (set<string>::iterator i = names.begin(); i != names.end(); ++i) {
				// load layers
				layer_name = *i;

				// allocate memory
				pixels = new Pixel4[m_width * m_height]();
				if (!pixels) {
					std::cerr << "Failed to allocate memory for new layer" << std::endl;
				}

				// layer.R
				frame_buffer.insert((layer_name + ".R").c_str(), // name
					OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
						(char *)&pixels[0].r,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * m_width,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value
											   // layer.R
				frame_buffer.insert((layer_name + ".G").c_str(), // name
					OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
						(char *)&pixels[0].g,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * m_width,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value
											   // layer.R
				frame_buffer.insert((layer_name + ".B").c_str(), // name
					OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
						(char *)&pixels[0].b,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * m_width,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value

											   // add layer
				EXRLayer *layer = new EXRLayer(pixels, m_width, m_height, layer_name.c_str());
				compositor.add_layer(layer);
				std::cout << " - Found layer: " << layer_name << std::endl;
			}

			// read pixels
			std::cout << " - Loading layers from multi-layered EXR file" << std::endl;
			file.setFrameBuffer(frame_buffer);
			file.readPixels(dw.min.y, dw.max.y);

			return 0;
		}
	}
}

#endif // USE_ARNOLD_CACHING

#endif // USE_ARNOLD
