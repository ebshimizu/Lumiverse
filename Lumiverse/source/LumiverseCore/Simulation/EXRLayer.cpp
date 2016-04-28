#include "EXRLayer.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../lib/stb/stb_image_resize.h"

#include <cstring>

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
	std::memset(pixels, 0, sizeof(Pixel4) * get_size());
}

void EXRLayer::resize_buffer(int new_width, int new_height) {
	w = new_width;
	h = new_height;
	if (pixels != NULL) {
		delete[] pixels;
	}

	pixels = new Pixel4[new_width * new_height];
}

Pixel4 *EXRLayer::get_downsampled_pixels(int width, int height) {
	if ((width == w) && (height == h)) {
		return pixels;
	}
	Pixel4 *downsampled_pixels = new Pixel4[width * height];

	float *output_pixels = new float[width * height * 4];
	float *input_pixels = new float[w * h * 4];
	for (int i = 0; i < w * h; i++) {
		int input_idx = 4 * i;
		input_pixels[input_idx] = pixels[i].r;
		input_pixels[input_idx + 1] = pixels[i].g;
		input_pixels[input_idx + 2] = pixels[i].b;
		input_pixels[input_idx + 3] = pixels[i].a;
	}

	stbir_resize_float(
		input_pixels, w, h, 0,
		output_pixels, width, height, 0,
		4
	);

	for (int i = 0; i < w * h; i++) {
		int output_idx = 4 * i;
		downsampled_pixels[i].r = output_pixels[output_idx];
		downsampled_pixels[i].g = output_pixels[output_idx + 1];
		downsampled_pixels[i].b = output_pixels[output_idx + 2];
		downsampled_pixels[i].a = output_pixels[output_idx + 3];
	}

	delete[] input_pixels;
	delete[] output_pixels;
	return downsampled_pixels;
}

}; // namespace LightmanCore

#endif // USE_ARNOLD_CACHING
