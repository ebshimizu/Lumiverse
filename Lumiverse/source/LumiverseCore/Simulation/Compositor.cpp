
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
}

Compositor::~Compositor() {

	// free buffer
	if (compose_buffer) {
		delete[] compose_buffer;
	}
  // free all layers
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
		return NULL;
	}
}

void Compositor::del_layer_by_name(const char *layer_name) {

	if (layers.count(layer_name) > 0) {
		layers.erase(layer_name);
	} else {
		std::cerr << "Layer not found for deletion: " << layer_name << std::endl;
	}
}

void Compositor::update_dims(int w, int h) {
	if (compose_buffer != NULL) {
		delete[] compose_buffer;
	}

	compose_buffer = new Pixel4[w * h]();
}

void Compositor::render(const std::set<Device*> &devices) {

  // clear previous rendering
  std::memset((void *)compose_buffer, 0, sizeof(Pixel4) * w * h);

  // composite all active layers
  Eigen::Matrix3d sharp;
  /*
  sharp(0, 0) = 3.2406;
  sharp(0, 1) = -1.5372;
  sharp(0, 2) = -.4986;
  sharp(1, 0) = -.9689;
  sharp(1, 1) = 1.8758;
  sharp(1, 2) = .0415;
  sharp(2, 0) = .0557;
  sharp(2, 1) = -.2040;
  sharp(2, 2) = 1.0570;
  */
  sharp(0, 0) = .4124;
  sharp(0, 1) = .3576;
  sharp(0, 2) = .1805;
  sharp(1, 0) = .2126;
  sharp(1, 1) = .7152;
  sharp(1, 2) = .0722;
  sharp(2, 0) = .0193;
  sharp(2, 1) = .1192;
  sharp(2, 2) = .9505;

  for (Device *device : devices) {
	  std::string name = device->getMetadata("Arnold Node Name");

	  // Only composite active layers
	  EXRLayer *layer = this->get_layer_by_name(name.c_str());
	  if (!layer->is_active()) {
		  continue;
	  }

	  //float intensity_shift = device->getIntensity()->asPercent();
	  float intensity_shift = 1.f;

	  Eigen::Vector3d modulator = device->getColor()->getRGB();

	  // modulator = sharp * modulator;
	  	  /*

	  float r = modulator.x();
	  float g = modulator.y();
	  float b = modulator.z();
	  */

	  float r = 1.f;
	  float g = 1.f;
	  float b = 1.f;

	  Pixel4 *pixels = layer->get_downsampled_pixels(w, h);
	  if (layer->is_active()) {
		  for (int i = 0; i < w * h; i++) {
			  Pixel4 basis_pixel = pixels[i];
			  if (basis_pixel.a > 0) {
				  compose_buffer[i].r += (r * intensity_shift * basis_pixel.r);
				  compose_buffer[i].g += (g * intensity_shift * basis_pixel.g);
				  compose_buffer[i].b += (b * intensity_shift * basis_pixel.b);
			  }
		  }
	  }

	  delete[] pixels;
  }
}

}; // namespace Lumiverse

#endif // USE_ARNOLD_CACHING
