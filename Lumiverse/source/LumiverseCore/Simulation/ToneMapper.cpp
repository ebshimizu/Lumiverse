
#ifdef USE_ARNOLD_CACHING

/*!
* \brief File containing code for tonemapping images
*
* Credit is due to Sky Gao for writing most of this code
*/
#include "ToneMapper.h"

#include <cmath>

namespace Lumiverse {

	////////////////////////////
	// Default Implementation //
	////////////////////////////

	ToneMapper::ToneMapper() {

		type = "Default";

		gamma = 2.2f;
		level = 1.0f;

		input_buffer = NULL;
		hdr_output_buffer = NULL;
	}

	ToneMapper::~ToneMapper() {}

	std::string ToneMapper::get_type() { return type; }

	float ToneMapper::get_gamma() { return gamma; }

	void ToneMapper::set_gamma(float gamma) { this->gamma = gamma; }

	float ToneMapper::get_level() { return level; }

	void ToneMapper::set_level(float level) { this->level = level; }

	void ToneMapper::set_input(Pixel4 *buffer, size_t w, size_t h) {

		this->w = w;
		this->h = h;

		this->input_buffer = buffer;
	}

	void ToneMapper::set_output_hdr(float *buffer) {

		this->hdr_output_buffer = buffer;
	}

	void ToneMapper::set_output_bmp(char *buffer) {

		this->bmp_output_buffer = buffer;
	}

	void ToneMapper::apply_hdr() {

		if (!input_buffer) {
			std::cerr << "Tone mapper has no input buffer" << std::endl;
			return;
		}
		if (!hdr_output_buffer) {
			std::cerr << "Tone mapper has no HDR output buffer" << std::endl;
			return;
		}

		float g = 1.0f / gamma;
		// float e = sqrt(pow(2, level));
		float e = 1.f;

		std::memset(hdr_output_buffer, 0, w * h * 4 * sizeof(float));
		for (size_t i = 0; i < w * h; ++i) {
			int actual_index = 4 * i;
			Pixel4 curr_pixel = input_buffer[i];
			hdr_output_buffer[actual_index] = pow(curr_pixel.r * e, g);
			hdr_output_buffer[actual_index + 1] = pow(curr_pixel.g * e, g);
			hdr_output_buffer[actual_index + 2] = pow(curr_pixel.b * e, g);
			hdr_output_buffer[actual_index + 3] = 1.f;
		}
	}

	void ToneMapper::apply_bmp() {

		if (!input_buffer) {
			std::cerr << "Tone mapper has no input buffer" << std::endl;
			return;
		}
		if (!bmp_output_buffer) {
			std::cerr << "Tone mapper has no bitmap output buffer" << std::endl;
			return;
		}

		float g = 1.0f / gamma;
		float e = sqrt(pow(2, level));
		for (size_t i = 0; i < w * h; ++i) {
			int buf_idx = 4 * i;
			bmp_output_buffer[buf_idx] = clamp(pow(input_buffer[i].r * e, g));
			bmp_output_buffer[buf_idx + 1] = clamp(pow(input_buffer[i].g * e, g));
			bmp_output_buffer[buf_idx + 2] = clamp(pow(input_buffer[i].b * e, g));
		}
	}

	void ToneMapper::reset() {
		gamma = 2.2f;
		level = 1.0f;
	}

}; // namespace Lumiverse

#endif // USE_ARNOLD_CACHING