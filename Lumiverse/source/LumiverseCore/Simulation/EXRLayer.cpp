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

EXRLayer::EXRLayer(size_t width, size_t height,
             const char *name) {

  if (name) {
    this->name = name;
  }

  w = width;
  h = height;
  pixels = new Pixel4[w * h];
  memset(pixels, 0, w * h * sizeof(Pixel4));

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

void EXRLayer::clear_buffers() {
	for (auto i = pixel_size_bases.begin(); i != pixel_size_bases.end(); i++) {
		delete(i->second);
	}
	pixel_size_bases.clear();
	std::memset(pixels, 0, sizeof(Pixel4) * get_size());
}

int inline EXRLayer::get_size_key(int width, int height) {
	return width << 2 + height;
}

Pixel4 *EXRLayer::get_downsampled_pixels(int width, int height) {
	if ((width == w) && (height == h)) {
		return pixels;
	}

	int key = get_size_key(width, height);
	if (pixel_size_bases.count(key) > 0) {
		return pixel_size_bases.at(key);
	}

	Pixel4 *downsampled_pixels = new Pixel4[width * height];

	float *output_pixels = new float[width * height * 4];
	float *input_pixels = new float[w * h * 4];
	for (int i = 0; i < w * h; i++) {
		int input_idx = 4 * i;
		float r = pixels[i].r;
		float g = pixels[i].g;
		float b = pixels[i].b;
		float a = pixels[i].a;
		input_pixels[input_idx] = r;
		input_pixels[input_idx + 1] = g;
		input_pixels[input_idx + 2] = b;
		input_pixels[input_idx + 3] = a;
	}

	stbir_resize_float(
		input_pixels, w, h, 0,
		output_pixels, width, height, 0,
		4
	);

	for (int i = 0; i < width * height; i++) {
		int output_idx = 4 * i;
		downsampled_pixels[i].r = output_pixels[output_idx];
		downsampled_pixels[i].g = output_pixels[output_idx + 1];
		downsampled_pixels[i].b = output_pixels[output_idx + 2];
		downsampled_pixels[i].a = output_pixels[output_idx + 3];
	}

	delete[] input_pixels;
	delete[] output_pixels;

	// Add this buffer to the map for future use
	pixel_size_bases[key] = downsampled_pixels;

	return downsampled_pixels;
}

void EXRLayer::set_pixels(float *buffer) {
	
	if (pixels != NULL) {
		delete[] pixels;
	}
	pixels = new Pixel4[w * h];

	for (int i = 0; i < w * h; i++) {
		int buf_idx = 4 * i;
		pixels[i].r = buffer[buf_idx];
		pixels[i].g = buffer[buf_idx + 1];
		pixels[i].b = buffer[buf_idx + 2];
		pixels[i].a = buffer[buf_idx + 3];
	}
}

}; // namespace LightmanCore

#endif // USE_ARNOLD_CACHING
