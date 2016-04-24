#include "CachingArnoldInterface.h"

#include "types/LumiverseFloat.h"
#include "ImfTestFile.h"    // Header checks
#include "ImfInputFile.h"   // Imf file IO
#include "ImfOutputFile.h"  // Imf file IO
#include "ImfRgbaFile.h"    // Imf RGBA scaneline
#include "ImfChannelList.h" // Imf channels
#include "ImathBox.h"

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
		this->setHDROutputBuffer(new Pixel4[m_width * m_height]);

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
			Pixel4 *pixels = new Pixel4[m_width * m_height]();
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
		float *buffer = new float[m_width * m_height * 4];

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

			Pixel4 *layer_buffer = layer->get_pixels();
			for (size_t idx = 0; idx < m_width * m_height; ++idx) {
				int buf_idx = idx * 4;
				layer_buffer[idx].r = buffer[buf_idx];
				layer_buffer[idx].g = buffer[buf_idx + 1];
				layer_buffer[idx].b = buffer[buf_idx + 2];
				layer_buffer[idx].a = buffer[buf_idx + 3];
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

	void CachingArnoldInterface::setHDROutputBuffer(Pixel4 *buffer) {

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
