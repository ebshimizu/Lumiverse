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

	void ToneMapper::set_output_hdr(Pixel4 *buffer) {

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
		float e = sqrt(pow(2, level));
		for (size_t i = 0; i < w * h; ++i) {
			hdr_output_buffer[i].r = pow(input_buffer[i].r * e, g);
			hdr_output_buffer[i].g = pow(input_buffer[i].g * e, g);
			hdr_output_buffer[i].b = pow(input_buffer[i].b * e, g);
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
			bmp_output_buffer[i * 3] = clamp(pow(input_buffer[i].r * e, g));
			bmp_output_buffer[i * 3 + 1] = clamp(pow(input_buffer[i].g * e, g));
			bmp_output_buffer[i * 3 + 2] = clamp(pow(input_buffer[i].b * e, g));
		}
	}

	void ToneMapper::reset() {
		gamma = 2.2f;
		level = 1.0f;
	}

	/////////////////////////////
	// Reinhard Implementation //
	/////////////////////////////

	TMReinhard::TMReinhard() {

		type = "Reinhard";

		gamma = 2.2f;
		level = 1.0f;

		input_buffer = NULL;
		hdr_output_buffer = NULL;
		bmp_output_buffer = NULL;

		key = 0.18;
		wht = 1.0f;
		avg = 0.18;
	}

	float TMReinhard::get_key() { return key; }

	void TMReinhard::set_key(float key) { this->key = key; }

	float TMReinhard::get_wht() { return wht; }

	void TMReinhard::set_wht(float wht) { this->wht = wht; }

	void TMReinhard::auto_adjust() {

		float num_pixels = w * h;
		for (size_t i = 0; i < num_pixels; ++i) {

			// the small delta value below avoids singularity
			avg += log(0.0000001 + input_buffer[i].illum());
		}

		avg = exp(avg / num_pixels);
	}

	void TMReinhard::apply_hdr() {

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

			Pixel4 pixel = input_buffer[i];

			float l = pixel.illum();

			pixel *= key / avg;
			pixel *= ((l + 1) / (wht * wht)) / (l + 1);

			hdr_output_buffer[i].r = pow(pixel.r * e, g);
			hdr_output_buffer[i].g = pow(pixel.g * e, g);
			hdr_output_buffer[i].b = pow(pixel.b * e, g);
		}
	}

	void TMReinhard::apply_bmp() {

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

			Pixel3 pixel = input_buffer[i];

			float l = pixel.illum();

			pixel *= key / avg;
			pixel *= ((l + 1) / (wht * wht)) / (l + 1);

			bmp_output_buffer[i * 3] = clamp(pow(pixel.r * e, g));
			bmp_output_buffer[i * 3 + 1] = clamp(pow(pixel.g * e, g));
			bmp_output_buffer[i * 3 + 2] = clamp(pow(pixel.b * e, g));
		}
	}

	void TMReinhard::reset() {

		gamma = 2.2f;
		level = 1.0f;

		key = 0.18;
		wht = 1.0f;
		avg = 0.18;
	}

}; // namespace Lumiverse