/* LIBRARY:   ImageIO Library v0.13
*
* FILE:      imageio.hpp
*
* COPYRIGHT: Copyright (c) 2003-2004 Adrien Treuille
*
* AUTHORS:   Adrien Treuille (treuille [AT] cs [DOT] washington [DOT] edu)
*            Kristin Siu
*
* LICENSE:   The ImageIO Library is supplied "AS IS".  The Authors
*            disclaim all warranties, expressed or implied,
*            including, without limitation, the warranties of
*            merchantability and of fitness for any purpose.
*            The Authors assume no liability for direct, indirect,
*            incidental, special, exemplary, or consequential
*            damages, which may result from the use of the ImageIO
*            Library, even if advised of the possibility of such
*            damage.
*
*            Permission is hereby granted to use, copy, modify,
*            and distribute this source code, or portions hereof,
*            for any purpose, without fee, for non-commercial
*            purposes. All rights reserved for commercial uses of
*            this source code.
*/

#ifndef IMAGEIO_HPP_
#define IMAGEIO_HPP_

#include <cstdlib>

namespace Lumiverse {

	// Sets tbe width and height to the appropriate values and mallocs
	// a char *buffer loading up the values in row-major, RGBA format.
	// The memory associated with the buffer can be deallocated with free().
	// If there was an error reading file, then 0 is returned, and
	// width = height are set to -1.
	unsigned char* imageio_load_image(const char* filename, int *width, int *height);

	// Saves image given by buffer with specicified width and height
	// to the given file name, returns true on success, false otherwise.
	// The image format is RGBA.
	bool imageio_save_image(const char* filename, unsigned char* buffer, int width, int height);

	// puts a default filename in name, up to len characters
	void imageio_gen_name(char* filename, size_t len);

	inline void floats_to_bytes(unsigned char *arr, float *rgba, int width, int height)
	{
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				int offset = j * width + i;

				// convert to ints
				arr[offset] = static_cast<unsigned char>(rgba[offset] * 0xff);
				arr[offset + 1] = static_cast<unsigned char>(rgba[offset + 1] * 0xff);
				arr[offset + 2] = static_cast<unsigned char>(rgba[offset + 2] * 0xff);
				arr[offset + 3] = static_cast<unsigned char>(rgba[offset + 3] * 0xff);
			}
		}
	}
} 

#endif /* IMAGEIO_HPP_ */