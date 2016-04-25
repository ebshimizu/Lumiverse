#include "EXRLayer.h"

#ifdef USE_ARNOLD_CACHING

namespace Lumiverse {

EXRLayer::EXRLayer(const char *file, const char *name) {

  if (name) {
    this->name = name;
  }

  // load single exr file?
  w = 0;
  h = 0;
  pixels = NULL;

  active = true;
  modulator = Pixel3(1, 1, 1);
}

EXRLayer::EXRLayer(Pixel4 *pixel_buffer, size_t width, size_t height,
             const char *name) {

  if (name) {
    this->name = name;
  }

  w = width;
  h = height;
  pixels = pixel_buffer;

  active = true;
  modulator = Pixel3(1, 1, 1);
}

EXRLayer::~EXRLayer() { delete[] pixels; }

std::string EXRLayer::get_name() { return name; }

size_t EXRLayer::get_width() { return w; }

size_t EXRLayer::get_height() { return h; }

size_t EXRLayer::get_size() { return w * h; }

bool EXRLayer::is_active() { return active; }

void EXRLayer::enable() { active = true; }

void EXRLayer::disable() { active = false; }

Pixel3 EXRLayer::get_modulator() { return modulator; }

void EXRLayer::set_modulator(Pixel3 modulator) { this->modulator = modulator; }

Pixel4 *EXRLayer::get_pixels() { return pixels; }

void EXRLayer::clear_buffer() {
	memset(pixels, 0, sizeof(Pixel4) * get_size());
}

}; // namespace LightmanCore

#endif // USE_ARNOLD_CACHING
