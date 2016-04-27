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
		m_width = AiNodeGetInt(options, "xres");
		m_height = AiNodeGetInt(options, "yres");
		m_samples = AiNodeGetInt(options, "AA_samples");
		m_buffer = new float[m_width * m_height * 4];
		this->setHDROutputBuffer();

		// setup buffer driver
		AtNode *driver = AiNode("driver_buffer");
		m_bufDriverName = "buffer_driver";
		std::stringstream ss;
		ss << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() -
			chrono::system_clock::from_time_t(0)).count() % 1000;
		m_bufDriverName = m_bufDriverName.append(ss.str());

		AiNodeSetStr(driver, "name", "driver_buffer");
		AiNodeSetFlt(driver, "gamma", 1.f);
		AiNodeSetBool(driver, "predictive", m_predictive);

		delete[] m_bucket_pos;
		m_bucket_pos = NULL;
		m_bucket_num = std::thread::hardware_concurrency();
		m_bucket_pos = new BucketPositionInfo[m_bucket_num];
		AiNodeSetPtr(driver, "bucket_pos_pointer", m_bucket_pos);

		AiNodeSetPtr(driver, "progress_pointer", &m_progress);

		// create a filter - override filter in ass file
		AtNode *filter = AiNode("gaussian_filter");
		AiNodeSetStr(filter, "name", "filter");
		AiNodeSetFlt(filter, "width", 2);

		// use buffer driver for output
		AtArray *outputs = AiNodeGetArray(options, "outputs");
		AiArraySetStr(outputs, 0, "RGBA RGBA filter driver_buffer");

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

	bool CachingArnoldInterface::setDims(int w, int h) {
		force_cache_reload = true;
		compositor.update_dims(w, h);

		bool success = ArnoldInterface::setDims(w, h);
		if (success) {
			setHDROutputBuffer();
		}

		return success;
	}

	void CachingArnoldInterface::updateDevicesLayers(const std::set<Device *> &devices) {
		std::unordered_map<std::string, Device *> to_update;
		for (Device *device : devices) {
			std::string device_name = device->getMetadata("Arnold Node Name");
			if (!force_cache_reload && (cached_devices.count(device_name) > 0)) {
				Device *cached = cached_devices.at(device_name);
				if (!device->isValidCacheCopy(cached)) {
					to_update[device_name] = device;
				}
			} else {
				cached_devices[device_name] = device;
				to_update[device_name] = device;
			}
		}

		// If no updates are necessary just return to avoid the overhead
		// of calling malloc, iterating over the light nodes, etc
		if (to_update.size() == 0) {
			return;
		}

		float *buffer = new float[m_width * m_height * 4];
		memset(buffer, 0, m_width * m_height * 4 * sizeof(float));
		AtNode *driver = AiNodeLookUpByName("driver_buffer");
		AiNodeSetInt(driver, "width", m_width);
		AiNodeSetInt(driver, "height", m_height);
		AiNodeSetPtr(driver, "buffer_pointer", buffer);

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
			
			AiNodeSetRGB(light, "color", 1.f, 1.f, 1.f);
			// render image
			// AiNodeSetFlt(light, "intensity", intensity_float->getMax());
			AiRender(AI_RENDER_MODE_CAMERA);

			// copy to layer buffer
			EXRLayer *layer = compositor.get_layer_by_name(name.c_str());
			layer->clear_buffer();

			Pixel4 *layer_buffer = layer->get_pixels();
			for (size_t idx = 0; idx < m_width * m_height; ++idx) {
				int buf_idx = idx * 4;
				layer_buffer[idx].r = buffer[buf_idx];
				layer_buffer[idx].g = buffer[buf_idx + 1];
				layer_buffer[idx].b = buffer[buf_idx + 2];
				layer_buffer[idx].a = !!buffer[buf_idx + 3];
			}

			layer->enable();

			// disable light
			AiNodeSetDisabled(light, true);
			memset(buffer, 0, m_width * m_height * 4 * sizeof(float));
		}
		AiNodeIteratorDestroy(it);

		delete[] buffer;
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
				if (!pixels)
					std::cerr << "Failed to allocate memory for new layer" << std::endl;

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
