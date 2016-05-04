#ifndef Lumiverse_PIXEL_H
#define Lumiverse_PIXEL_H

#ifdef USE_ARNOLD_CACHING

#include <iostream>

namespace Lumiverse {

/**
 * Pixels are defined as vectors of illuminance of the color channels.
 * Lightman uses GLM's single precision floating point vectors as a basis
 * for its pixel representation. Therefore they are compatible with all
 * the vector operations provided by GLM.
 */
class Pixel {
public:
  /**
   * Compute the illuminance value of the pixel.
   */
  virtual inline float illum() = 0;
};

/**
 * Pixel with RGB channels.
 * You may access individual channel through members r,g,b
 */
class Pixel3 : public Pixel {
public:
	Pixel3() {
		r = 1;
		g = 1;
		b = 1;
	}

	Pixel3(float x, float y, float z) {
		r = x;
		g = y;
		b = z;
	}

	float r, g, b;

	Pixel3 operator *(float scalar) {
		return Pixel3(r * scalar, g * scalar, b * scalar);
	}

	void operator *=(int scalar) {
		r *= scalar;
		g *= scalar;
		b *= scalar;
	}

	inline float illum() {
		return 0.2126*r + 0.7152*g + 0.0722*b;
	}
};

/**
 * Pixel with RGBA channels.
 * You may access individual channel through members r,g,b,a
 */
class Pixel4 : public Pixel {
public:
	Pixel4() {
		r = 1;
		g = 1;
		b = 1;
		a = 1;
	}

	Pixel4(float x, float y, float z, float w) {
		r = x;
		g = y;
		b = z;
		a = w;
	}

	float r, g, b, a;

  inline float illum() {
	  return 0.2126*r + 0.7152*g + 0.0722*b;
  }

  Pixel4 operator *(float scalar) {
	  return Pixel4(r * scalar, g * scalar, b * scalar, a);
  }

  void operator *=(int scalar) {
	  r *= scalar;
	  g *= scalar;
	  b *= scalar;
  }

};

/**
 * Clamp an illuminance value to 8 bit color
 */
inline int clamp(float x) { return x < 0 ? 0 : x > 1 ? 255 : int(x * 255); }
inline float clamp(float x, float min, float max) { return x < min ? min : (x > max) ? max : x; }

}; // namespace Lumiverse

#endif // USE_ARNOLD_CACHING

#endif // Lumiverse_PIXEL_H
