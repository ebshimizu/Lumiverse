
#include "Compositor.h"
#include "EXRLayer.h"

#include <string.h>
#include <chrono>

namespace Lumiverse {

Compositor::Compositor() {

  // layers
  layers = std::set<EXRLayer *>();
  ambient_layer = NULL;

  // buffers
  compose_buffer = NULL;
  output_buffer = NULL;

  //// threshold
  //amb_diff = 0.01;
}

Compositor::~Compositor() {

  // free frame buffer if allocated
  if (compose_buffer)
    delete[] compose_buffer;

  // free all layers
  layers.clear();
}

size_t Compositor::get_width() { return w; }

size_t Compositor::get_height() { return h; }

std::set<EXRLayer *> Compositor::get_layers() { return layers; }

void Compositor::add_layer(EXRLayer *layer) {

  if (!layers.size()) {

    // set size when first layer is added
    w = layer->get_width();
    h = layer->get_height();

    // allocate memory for composition buffer
    compose_buffer = new Pixel3[w * h]();
    if (!compose_buffer) {
      std::cerr << "Cannot create composite buffer" << std::endl;
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

  layers.insert(layer);
}

void Compositor::del_layer(EXRLayer *layer) { layers.erase(layer); }

EXRLayer *Compositor::get_layer_by_name(const char *layer_name) {

  EXRLayer *layer;
  std::set<EXRLayer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    if (layer->get_name() == layer_name) {
      return layer;
    }

    ++it;
  }

  return NULL;
}

void Compositor::del_layer_by_name(const char *layer_name) {

	EXRLayer *layer;
  std::set<EXRLayer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    if (layer->get_name() == layer_name) {
      layers.erase(layer);
      return;
    }

    ++it;
  }

  std::cerr << "Layer not found: " << layer_name << std::endl;
}

void Compositor::set_output_buffer(Pixel3 *buffer) {

  if (buffer)
    output_buffer = buffer;
}

void Compositor::render() {

  // get target buffer
  Pixel3 *target = output_buffer ? output_buffer : compose_buffer;

  // clear previous rendering
  memset((void *)target, 0, sizeof(Pixel3) * w * h);

  // composite all active layers
  EXRLayer *layer;
  std::set<EXRLayer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    Pixel3 m = layer->get_modulator();
    Pixel3 *pixels = layer->get_pixels();

    for (int i = 0; i < w * h; i++) {
      if (layer->is_active()) {
        target[i].r += m.r * pixels[i].r;
        target[i].g += m.g * pixels[i].g;
        target[i].b += m.b * pixels[i].b;
      }
    }
    it++;
  }
}

}; // namespace Lumiverse
