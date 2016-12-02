#include "LumiverseCoreConfig.h"

#ifdef USE_ARNOLD_CACHING

#include "Compositor.h"
#include "EXRLayer.h"
#include "Device.h"

#include <string.h>
#include <chrono>
#include <unordered_map>
#include <set>
#include <assert.h>

namespace Lumiverse {

Compositor::Compositor() {

  // buffers
  compose_buffer = NULL;

  w = 0;
  h = 0;
  _exposure = 1;
}

Compositor::~Compositor() {

	// free buffer
	if (compose_buffer) {
		delete[] compose_buffer;
	}
  // free all layers
	for (auto i = layers.begin(); i != layers.end(); i++) {
		delete(i->second);
	}
  layers.clear();
}

size_t Compositor::get_width() { return w; }

size_t Compositor::get_height() { return h; }

std::unordered_map<std::string, EXRLayer *>& Compositor::get_layers() { return layers; }

void Compositor::add_layer(EXRLayer *layer) {

  if (!layers.size()) {

    // set size when first layer is added
    w = layer->get_width();
    h = layer->get_height();

	compose_buffer = new Pixel4[w * h]();
	if (!compose_buffer) {
		std::cerr << "Unable to allocate space for the compose buffer in the compositor" << std::endl;
		return;
	}

  } else {

    // check size consistency

    size_t layer_w = layer->get_width();
    size_t layer_h = layer->get_height();
    if (layer_w != w || layer_h != h) {
      return;
    }
  }

  std::pair<std::string, EXRLayer *> newLayer(layer->get_name(), layer);
  layers.insert(newLayer);
}

void Compositor::del_layer(EXRLayer *layer) { layers.erase(layer->get_name()); }

EXRLayer *Compositor::get_layer_by_name(const char *layer_name) {

	if (layers.count(layer_name) > 0) {
		return layers.at(layer_name);
	} else {
		return nullptr;
	}
}

bool Compositor::contains_layer(const char *layer_name) {
	return layers.count(layer_name) > 0;
}

void Compositor::del_layer_by_name(const char *layer_name) {

	if (layers.count(layer_name) > 0) {
		layers.erase(layer_name);
	} else {
		std::cerr << "Layer not found for deletion: " << layer_name << std::endl;
	}
}

void Compositor::update_dims(int width, int height) {
	if (compose_buffer != NULL) {
		delete[] compose_buffer;
	}

	this->w = width;
	this->h = height;
	compose_buffer = new Pixel4[width * height]();
}

void Compositor::render(const std::set<Device*> &devices) {

  if (layers.size() == 0)
    return;

  // clear previous rendering
  std::memset((void *)compose_buffer, 0, sizeof(Pixel4) * w * h);

  for (Device *device : devices) {
	  std::string name = device->getMetadata("Arnold Node Name");

	  // Only composite active layers
	  EXRLayer *layer = this->get_layer_by_name(name.c_str());
	  if (layer == nullptr || !layer->is_active()) {
		  continue;
	  }

	  float intensity_shift = device->getIntensity()->asPercent();

    if (intensity_shift == 0)
      continue;

    Eigen::Vector3d modulator(1, 1, 1);
    
    if (device->getColor() != nullptr)
	    modulator = device->getColor()->getRGB();

	  float r = modulator.x() * intensity_shift * _exposure;
	  float g = modulator.y() * intensity_shift * _exposure;
	  float b = modulator.z() * intensity_shift * _exposure;

	  Pixel4 *pixels = layer->get_downsampled_pixels(w, h);
    int numPixels = w * h;

    for (int i = 0; i < numPixels; i++) {
      Pixel4* basis_pixel = &pixels[i];
      //if (basis_pixel.a > 0) {
      compose_buffer[i].r += (r * basis_pixel->r);
      compose_buffer[i].g += (g * basis_pixel->g);
      compose_buffer[i].b += (b * basis_pixel->b);
      //}
    }
  }
}

}; // namespace Lumiverse

#endif // USE_ARNOLD_CACHING
