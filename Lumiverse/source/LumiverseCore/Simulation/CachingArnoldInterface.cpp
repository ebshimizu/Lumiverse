#include "CachingArnoldInterface.h"

#include "types/LumiverseFloat.h"

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
		this->setHDROutputBuffer(new Pixel3[m_width * m_height]);

		// setup buffer driver
		AtNode *driver = AiNode("driver_buffer");
		m_bufDriverName = "buffer_driver";
		std::stringstream ss;
		ss << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() -
			chrono::system_clock::from_time_t(0)).count() % 1000;
		m_bufDriverName = m_bufDriverName.append(ss.str());

		AiNodeSetStr(driver, "name", "driver_buffer");
		AiNodeSetInt(driver, "width", m_width);
		AiNodeSetInt(driver, "height", m_height);
		AiNodeSetFlt(driver, "gamma", 1);
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
		AtNodeIterator *it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
		while (!AiNodeIteratorFinished(it)) {

			AtNode *light = AiNodeIteratorGetNext(it);

			// create new layer
			string name = AiNodeGetStr(light, "name");
			Pixel3 *pixels = new Pixel3[m_width * m_height]();
			EXRLayer *layer = new EXRLayer(pixels, m_width, m_height, name.c_str());

			// Disable layer by default -- enable when we read light nodes from scene in render()
			layer->disable();

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

		// temp buffer to hold arnold output
		float buffer = new float[m_width * m_height * 4];

		// render each per-light layer
		std::cout << "Rendering layers" << std::endl;
		it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
		while (!AiNodeIteratorFinished(it)) {

			AtNode *light = AiNodeIteratorGetNext(it);

			// enable light
			AiNodeSetDisabled(light, false);

			// render to layer buffer
			// since RGB is not supported, we nned to use rgba here
			AiNodeSetPtr(driver, "buffer_pointer", buffer);

			// render image
			AiRender(AI_RENDER_MODE_CAMERA);

			// copy to layer buffer
			std::string name = AiNodeGetStr(light, "name");
			EXRLayer *layer = compositor.get_layer_by_name(name.c_str());

			Pixel3 *layer_buffer = layer->get_pixels();
			for (size_t idx = 0; idx < m_width * m_height; ++idx) {
				layer_buffer[idx].r = buffer[idx].r;
				layer_buffer[idx].g = buffer[idx].g;
				layer_buffer[idx].b = buffer[idx].b;
				layer_buffer[idx].a = buffer[idx].a;
			}

			// disable light
			AiNodeSetDisabled(light, true);
		}
		AiNodeIteratorDestroy(it);

		// free temp buffer
		delete[] buffer;

		AiEnd();
	}

	int CachingArnoldInterface::render() {
		tone_mapper.set_gamma(m_gamma);

		AtNodeIterator *it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);
		while (!AiNodeIteratorFinished(it)) {

			AtNode *light = AiNodeIteratorGetNext(it);

			// create new layer
			std::string name = AiNodeGetStr(light, "name");
			EXRLayer *layer = compositor.get_layer_by_name(name.c_str());
			AtRGB rgb = AiNodeGetRGB(light, "color");
			float intensity = AiNodeGetFlt(light, "intensity");
			layer->set_modulator(Pixel3(rgb.r, rgb.g, rgb.b) * intensity);
			layer->enable();
		}
		AiNodeIteratorDestroy(it);

		dumpHDRToBuffer();

		return AI_SUCCESS;
	}

	void CachingArnoldInterface::setHDROutputBuffer(Pixel3 *buffer) {

		if (buffer) {
			this->hdr_output_buffer = buffer;
		}

		tone_mapper.set_output_hdr(hdr_output_buffer);
	}

	void CachingArnoldInterface::dumpHDRToBuffer() {

		if (!hdr_output_buffer) {
			std::cerr << "No HDR output buffer is set" << std::endl;
			return;
		}

		compositor.render();
		tone_mapper.set_input(compositor.get_compose_buffer(), m_width, m_height);
		tone_mapper.apply_hdr();
	}

	int CachingArnoldInterface::load_exr(const char *file_path) {

		// check header
		bool isTiled;
		bool isValid = isOpenExrFile(file_path, isTiled);
		if (isValid) {

			// check for tiled exr
			if (isTiled) {
				std::cerr << "Only scanline images are supported" << std::endl;
				return -1;
			}

		} else {

			// file not valid
			std::cerr << "Invalid input OpenEXR file: " << file_path;
			return -1;
		}

		// read dimensions
		InputFile file(file_path);
		Box2i dw = file.header().dataWindow();
		w = dw.max.x - dw.min.x + 1;
		h = dw.max.y - dw.min.y + 1;

		// read channel information
		const ChannelList &channels = file.header().channels();
		set<string> names;
		channels.layers(names);

		// read layers
		string layer_name;
		Pixel3 *pixels;
		if (names.size()) {

			// read pixels
			FrameBuffer frame_buffer;
			for (set<string>::iterator i = names.begin(); i != names.end(); ++i) {
				// load layers
				layer_name = *i;

				// allocate memory
				pixels = new Pixel3[w * h]();
				if (!pixels)
					lmerr("Failed to allocate memory for new layer");

				// layer.R
				frame_buffer.insert((layer_name + ".R").c_str(), // name
					Slice(FLOAT,                 // type
						(char *)&pixels[0].r,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * w,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value
											   // layer.R
				frame_buffer.insert((layer_name + ".G").c_str(), // name
					Slice(FLOAT,                 // type
						(char *)&pixels[0].g,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * w,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value
											   // layer.R
				frame_buffer.insert((layer_name + ".B").c_str(), // name
					Slice(FLOAT,                 // type
						(char *)&pixels[0].b,  // base
						sizeof(Pixel3) * 1,    // xstride
						sizeof(Pixel3) * w,    // ystride
						1, 1,                  // sampling
						0.0));                 // fill value

											   // add layer
				Layer *layer = new Layer(pixels, w, h, layer_name.c_str());
				compositor->add_layer(layer);
				lmout(" - Found layer: " << layer_name);
			}

			// read pixels
			lmout(" - Loading layers from multi-layered EXR file");
			file.setFrameBuffer(frame_buffer);
			file.readPixels(dw.min.y, dw.max.y);

			return 0;
		}
}
