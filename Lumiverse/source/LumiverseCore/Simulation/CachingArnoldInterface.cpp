#include "CachingArnoldInterface.h"

#include "types/LumiverseFloat.h"
#include "ImfTestFile.h"    // Header checks
#include "ImfInputFile.h"   // Imf file IO
#include "ImfOutputFile.h"  // Imf file IO
#include "ImfRgbaFile.h"    // Imf RGBA scaneline
#include "ImfChannelList.h" // Imf channels
#include "ImathBox.h"
#include <assert.h>

#ifdef USE_ARNOLD_CACHING

namespace Lumiverse {
  CachingRenderContext::CachingRenderContext(Compositor * c, int w, int h) :
    _compositor(c), _w(w), _h(h)
  {
    _buffer = new float[4 * _w * _h];
  }

  CachingRenderContext::~CachingRenderContext()
  {
    // remove layers from compositor before delete, so we only do 1 delete
    _compositor->get_layers().clear();
    delete _compositor;
    
    if (_buffer != nullptr)
      delete _buffer;
  }

  void CachingRenderContext::setSize(int w, int h)
  {
    _w = w;
    _h = h;

    if (_buffer != nullptr) {
      delete[] _buffer;
      _buffer = new float[_w * _h * 4];
    }

    _compositor->update_dims(_w, _h);
  }

  void CachingRenderContext::render(const set<Device*>& d)
  {
    _compositor->render(d);
  }

  CachingArnoldInterface::CachingArnoldInterface() : ArnoldInterface(),
    _cache_aa_samples(-1), _cache_width(1920), _cache_height(980), _cache_file_path("")
  {
  }


  void CachingArnoldInterface::init() {
#ifdef USE_ARNOLD
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
		ArnoldInterface::setDims(_cache_width, _cache_height);
		m_width = _cache_width;
		m_height = _cache_height;
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
    // if we haven't loaded any. We assume when we load that we have all layers.
    if (_layers.size() == 0) {
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
        EXRLayer *layer = new EXRLayer(m_width, m_height, name.c_str());

        // Disable layer by default -- enable when we read light nodes from scene in render()
        layer->enable();

        // add layer to compositor (render later)
        _layers[name] = layer;
        std::cout << "Created layer: " << name << std::endl;

        // disable light
        AiNodeSetDisabled(light, true);

        // increment count
        num_lights++;
      }
      AiNodeIteratorDestroy(it);
    }
#endif
		/*
		 Note that we don't actually render any layers on init. We wait until
		 render is called with a set of devices because we want to be able
		 to check if any of the layers need to be re-rendered (i.e. if a
		 light is rotated).
		*/
    
    // We also set up the data needed for running multiple threads in this caching renderer.
    _maxThreads = thread::hardware_concurrency();

    for (int i = 0; i < _maxThreads; i++) {
      Compositor* c = new Compositor();
      for (const auto& l : _layers) {
        c->add_layer(l.second);
      }

      // correct width and height here if they are negative
      if (m_width < 0 || m_height < 0) {
        m_width = c->get_width();
        m_height = c->get_height();
      }

      CachingRenderContext* con = new CachingRenderContext(c, c->get_width(), c->get_height());
      _contexts.push_back(con);
    }


		m_open = true;
	}

	void CachingArnoldInterface::close() {
		// @TODO: Clean stuff up
		delete[] m_render_buffer;

    // delete layers and contexts
    for (auto l : _layers)
      delete l.second;

    for (auto c : _contexts)
      delete c;

    for (auto& device : cached_devices) {
      delete device.second;
    }

		ArnoldInterface::close();
	}

  float CachingArnoldInterface::getPercentage()
  {
    // here we don't actually do anything and I don't particularly want
    // to deal with determining which context we're asking for,
    // so just say we're done.
    return 100;
  }

	bool CachingArnoldInterface::setDims(int w, int h)  {
    // We skip setting the arnold options node dimensions and let the
    // cached compositing object do its work.
		// bool success = ArnoldInterface::setDims(w, h);

    // with each context maintaining its own width and height, this
    // operation is basicaly useless and does a nullop here.
    m_width = w;
    m_height = h;

		return true;
	}

  void CachingArnoldInterface::setCacheDims(int w, int h)
  {
    if (_cache_width != w || _cache_height != h)
    {
      force_cache_reload = true;

      _cache_width = w;
      _cache_height = h;

      // compositors will update their sizes at cache reload.
    }
  }

	const std::unordered_map<std::string, Device*> CachingArnoldInterface::getDevicesToUpdate(const std::set<Device *> &devices) {
		std::unordered_map<std::string, Device *> to_update;

		for (Device *device : devices) {
			std::string device_name = device->getMetadata("Arnold Node Name");
			if (!force_cache_reload && (cached_devices.count(device_name) > 0)) {
				Device *cached = cached_devices.at(device_name);
				if (!isValidCacheCopy(cached, device)) {
          if (cached_devices[device_name] != nullptr)
            delete cached_devices[device_name];

					cached_devices[device_name] = new Device(device);
					to_update[device_name] = device;
				}
			}
			else {
				cached_devices[device_name] = new Device(device);
				to_update[device_name] = device;
			}
		}

		return to_update;
	}

	void CachingArnoldInterface::updateDevicesLayers(const std::set<Device *> &devices) {
#ifdef USE_ARNOLD
    // lock this section down. Typically should go fairly fast due to not needing to update this very often.
    lock_guard<mutex> lock(_updateLock);

		const std::unordered_map<std::string, Device *> &to_update = getDevicesToUpdate(devices);

		// If no updates are necessary just return to avoid the overhead
		// of calling malloc, iterating over the light nodes, etc
		if (to_update.size() == 0) {
			return;
		}

		memset(m_render_buffer, 0, _cache_width * _cache_height * 4 * sizeof(float));

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
			
      // update width and height and samples for this operation.
      AtNode* options = AiUniverseGetOptions();
      AiNodeSetInt(options, "xres", _cache_width);
      AiNodeSetInt(options, "yres", _cache_height);
      setSamples(_cache_aa_samples);

			// render image
			AiNodeSetFlt(light, "intensity", intensity_float->getMax());

			Eigen::Vector3d modulator = curr_device->getColor()->getRGB();

			AiNodeSetRGB(light, "color", 1, 1, 1);
      Logger::log(INFO, "Rendering " + name + " for cache");
			AiRender(AI_RENDER_MODE_CAMERA);

			// copy to layer buffer
			EXRLayer *layer = _layers[name];
			layer->clear_buffers();

			Pixel4 *layer_buffer = layer->get_pixels();
			for (size_t idx = 0; idx < _cache_width * _cache_height; ++idx) {
				int buf_idx = idx * 4;
				layer_buffer[idx].r = m_render_buffer[buf_idx];
				layer_buffer[idx].g = m_render_buffer[buf_idx + 1];
				layer_buffer[idx].b = m_render_buffer[buf_idx + 2];
				layer_buffer[idx].a = !!m_render_buffer[buf_idx + 3];
			}

      layer->enable();

			// disable light
			AiNodeSetDisabled(light, true);
			memset(m_render_buffer, 0, _cache_width * _cache_height * 4 * sizeof(float));
		}
		AiNodeIteratorDestroy(it);

    // autosave to disk
    dumpCache();
#else
    // Logger::log(WARN, "Cannot rengenerate cache, compiled without Arnold support.");
#endif
	}

	void CachingArnoldInterface::setSamples(int samples) {
    if (_cache_aa_samples < samples)
      _cache_aa_samples = samples;

		ArnoldInterface::setSamples(_cache_aa_samples);
	}

	int CachingArnoldInterface::render(const std::set<Device *> &devices, int w, int h, int& cid) {
    // pick a context to use
    cid = 0;
    CachingRenderContext* selected = nullptr;

    while (selected == nullptr) {
      for (auto& c : _contexts) {
        if (c->_inUse.try_lock()) {
          selected = c;
          break;
        }
        cid++;
      }
      this_thread::sleep_for(chrono::microseconds(5));
    }

    // set up context
    selected->setSize(w, h);
    updateDevicesLayers(devices);
		tone_mapper.set_gamma(m_gamma);

    // do the render
    selected->render(devices);
    tone_mapper.apply_hdr_inplace(selected->_compositor->get_compose_buffer(), selected->_buffer, selected->_w, selected->_h);

    // note that we don't unlock the context yet we wait for the patch to copy the
    // proper buffer over and unlock it.

		force_cache_reload = false;
		return 0;
	}

	void CachingArnoldInterface::setHDROutputBuffer() {
		tone_mapper.set_output_hdr(m_buffer);
	}

  void CachingArnoldInterface::saveLayer(EXRLayer * l)
  {
    if (l == nullptr)
      return;

    // set file path
    string file = _cache_file_path + "/" + l->get_name() + ".exr";
    OPENEXR_IMF_INTERNAL_NAMESPACE::Header header(l->get_width(), l->get_height());
    header.channels().insert("R", OPENEXR_IMF_INTERNAL_NAMESPACE::Channel(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT));
    header.channels().insert("G", OPENEXR_IMF_INTERNAL_NAMESPACE::Channel(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT));
    header.channels().insert("B", OPENEXR_IMF_INTERNAL_NAMESPACE::Channel(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT));

    Pixel4* pixels = l->get_pixels();

    OPENEXR_IMF_INTERNAL_NAMESPACE::OutputFile out(file.c_str(), header);
    OPENEXR_IMF_INTERNAL_NAMESPACE::FrameBuffer fb;

    // gather data
    fb.insert("R", OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(
      OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].r,
      sizeof(Pixel4) * 1,
      sizeof(Pixel4) * l->get_width(),
      1, 1, 0)
    );

    fb.insert("G", OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(
      OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].g,
      sizeof(Pixel4) * 1,
      sizeof(Pixel4) * l->get_width(),
      1, 1, 0)
    );

    fb.insert("B", OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(
      OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].b,
      sizeof(Pixel4) * 1,
      sizeof(Pixel4) * l->get_width(),
      1, 1, 0)
    );

    // save data
    out.setFrameBuffer(fb);
    out.writePixels(l->get_height());
  }

#ifdef USE_ARNOLD
	void CachingArnoldInterface::setOptionParameter(const std::string &paramName, int val) {
		if (optionRequiresCacheReload(paramName)) {
			force_cache_reload = true;
		}

		ArnoldInterface::setOptionParameter(paramName, val);
	}
#endif

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

#ifdef USE_ARNOLD
	void CachingArnoldInterface::setOptionParameter(const std::string &paramName, float val) {
		if (optionRequiresCacheReload(paramName)) {
			force_cache_reload = true;
		}

		ArnoldInterface::setOptionParameter(paramName, val);
	}
#endif

  void CachingArnoldInterface::dumpCache()
  {
    if (_cache_file_path == "") {
      Logger::log(ERR, "Can't save cache when path is not set.");
      return;
    }

    for (auto& d : cached_devices) {
      string name = d.second->getMetadata("Arnold Node Name");

      saveLayer(_layers[name.c_str()]);
    }
  }

  void CachingArnoldInterface::loadCache(const set<Device*>& devices)
  {
    for (auto& d : devices) {
      if (load_exr(d->getMetadata("Arnold Node Name")) == 0) {
        cached_devices[d->getMetadata("Arnold Node Name")] = new Device(d);
      }
    }

		if (_layers.size() > 0) {
			m_width = _layers.begin()->second->get_width();
			m_height = _layers.begin()->second->get_height();
			force_cache_reload = false;
		}
  }

  void CachingArnoldInterface::loadIfUsingCaching(const set<Device*>& devices)
  {
    loadCache(devices);
  }

  float * CachingArnoldInterface::getBufferForContext(int contextId)
  {
    return _contexts[contextId]->_buffer;
  }

  void CachingArnoldInterface::closeContext(int contextId)
  {
    _contexts[contextId]->_inUse.unlock();
  }

  JSONNode CachingArnoldInterface::toJSON()
  {
    JSONNode opts;
    opts.set_name("cache_options");
    opts.push_back(JSONNode("width", _cache_width));
    opts.push_back(JSONNode("height", _cache_height));
    opts.push_back(JSONNode("samples", _cache_aa_samples));
    opts.push_back(JSONNode("path", _cache_file_path));

    return opts;
  }

	bool CachingArnoldInterface::optionRequiresCacheReload(const std::string &paramName) {
		return paramName == "AA_samples";
	}

  int CachingArnoldInterface::load_exr(string& filename) {
    string file_path = _cache_file_path + "/" + filename + ".exr";

    // check for existence before loading
    ifstream fileCheck(file_path);
    if (!fileCheck.good())
      return -1;

    // check header
    bool isTiled;
    bool isValid = OPENEXR_IMF_INTERNAL_NAMESPACE::isOpenExrFile(file_path.c_str(), isTiled);
    if (isValid) {
      // check for tiled exr
      if (isTiled) {
        std::cerr << "Only scanline images are supported" << std::endl;
        return -2;
      }

    }
    else {
      // file not valid
      std::cerr << "Invalid input OpenEXR file: " << file_path;
      return -3;
    }

    // read dimensions
    OPENEXR_IMF_INTERNAL_NAMESPACE::InputFile file(file_path.c_str());
    IMATH_INTERNAL_NAMESPACE::Box2i dw = file.header().dataWindow();
    _cache_width = dw.max.x - dw.min.x + 1;
    _cache_height = dw.max.y - dw.min.y + 1;

    // Assuming the files were saved with Lumiverse, there will be 3 channels here.

    // read layers
    Pixel4 *pixels;
    // read pixels
    OPENEXR_IMF_INTERNAL_NAMESPACE::FrameBuffer frame_buffer;
    // each file is assumed to be one layer

    // allocate memory
    pixels = new Pixel4[_cache_width * _cache_height]();
    if (!pixels) {
      Logger::log(ERR, "Failed to allocate memory for new layer");
      return -4;
    }

    // layer.R
    frame_buffer.insert("R", // name
      OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].r,  // base
        sizeof(Pixel4) * 1,    // xstride
        sizeof(Pixel4) * _cache_width,    // ystride
        1, 1,                  // sampling
        0.0));                 // fill value
                     // layer.R
    frame_buffer.insert("G", // name
      OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].g,  // base
        sizeof(Pixel4) * 1,    // xstride
        sizeof(Pixel4) * _cache_width,    // ystride
        1, 1,                  // sampling
        0.0));                 // fill value
                     // layer.R
    frame_buffer.insert("B", // name
      OPENEXR_IMF_INTERNAL_NAMESPACE::Slice(OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT,
      (char *)&pixels[0].b,  // base
        sizeof(Pixel4) * 1,    // xstride
        sizeof(Pixel4) * _cache_width,    // ystride
        1, 1,                  // sampling
        0.0));                 // fill value

    // add layer
    EXRLayer *layer = new EXRLayer(_cache_width, _cache_height, filename.c_str());
    layer->set_pixels(pixels);
    layer->enable();
    _layers[filename] = layer;

    // read pixels
    file.setFrameBuffer(frame_buffer);
    file.readPixels(dw.min.y, dw.max.y);

    return 0;
  }
}

#endif // USE_ARNOLD_CACHING