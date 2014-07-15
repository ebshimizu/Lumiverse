/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Color types and utilities
 */

#pragma once
#include "ai_types.h"
#include "ai_constants.h"
#include "ai_api.h"

/** \defgroup ai_color AtColor API
 * \{
 */

/**
 * RGB color
 */
struct AtRGB
{
   float r, g, b;

   /*
    * NOTE: the absence of a constructor is for a reason - it appears that
    *       older versions of C++ don't like union members to have constructors,
    *       and this struct is used in the AtParamValue union
    */

   AtRGB operator+(const AtRGB& rgb) const
   {
      AtRGB temp;
      temp.r = r + rgb.r;
      temp.g = g + rgb.g;
      temp.b = b + rgb.b;
      return temp;
   }

   AtRGB& operator+=(const AtRGB& rgb)
   {
      r += rgb.r;
      g += rgb.g;
      b += rgb.b;
      return *this;
   }

   AtRGB operator+(float f) const
   {
      AtRGB temp;
      temp.r = r + f;
      temp.g = g + f;
      temp.b = b + f;
      return temp;
   }

   AtRGB& operator+=(float f)
   {
      r += f;
      g += f;
      b += f;
      return *this;
   }

   AtRGB operator-(const AtRGB& rgb) const
   {
      AtRGB temp;
      temp.r = r - rgb.r;
      temp.g = g - rgb.g;
      temp.b = b - rgb.b;
      return temp;
   }

   AtRGB& operator-=(const AtRGB& rgb)
   {
      r -= rgb.r;
      g -= rgb.g;
      b -= rgb.b;
      return *this;
   }

   AtRGB operator-(float f) const
   {
      AtRGB temp;
      temp.r = r - f;
      temp.g = g - f;
      temp.b = b - f;
      return temp;
   }

   AtRGB& operator-=(float f)
   {
      r -= f;
      g -= f;
      b -= f;
      return *this;
   }

   AtRGB operator-() const
   {
      AtRGB temp;
      temp.r = -r;
      temp.g = -g;
      temp.b = -b;
      return temp;
   }

   AtRGB operator*(const AtRGB& rgb) const
   {
      AtRGB temp;
      temp.r = r * rgb.r;
      temp.g = g * rgb.g;
      temp.b = b * rgb.b;
      return temp;
   }

   AtRGB operator*=(const AtRGB& rgb)
   {
      r *= rgb.r;
      g *= rgb.g;
      b *= rgb.b;
      return *this;
   }

   AtRGB operator*(float f) const
   {
      AtRGB temp;
      temp.r = r * f;
      temp.g = g * f;
      temp.b = b * f;
      return temp;
   }

   AtRGB operator*=(float f)
   {
      r *= f;
      g *= f;
      b *= f;
      return *this;
   }

   AtRGB operator/(const AtRGB& rgb) const
   {
      AtRGB temp;
      temp.r = r / rgb.r;
      temp.g = g / rgb.g;
      temp.b = b / rgb.b;
      return temp;
   }

   AtRGB operator/=(const AtRGB& rgb)
   {
      r /= rgb.r;
      g /= rgb.g;
      b /= rgb.b;
      return *this;
   }

   AtRGB operator/(float f) const
   {
      AtRGB temp;
      float inv = 1.0f / f;
      temp.r = r * inv;
      temp.g = g * inv;
      temp.b = b * inv;
      return temp;
   }

   AtRGB operator/=(float f)
   {
      float inv = 1.0f / f;
      r *= inv;
      g *= inv;
      b *= inv;
      return *this;
   }

   bool operator==(const AtRGB& rgb) const
   {
      return (r == rgb.r && g == rgb.g && b == rgb.b);
   }

   bool operator!=(const AtRGB& rgb) const
   {
      return !(*this == rgb);
   }

   AtRGB& operator=(float f)
   {
      r = f;
      g = f;
      b = f;
      return *this;
   }

   float& operator[](unsigned int i)
   {
      return *(&r + i);  // warning: no bounds checking!
   }

   const float& operator[](unsigned int i) const
   {
      return *(&r + i);  // warning: no bounds checking!
   }

   friend AtRGB operator*(float f, const AtRGB& rgb);
   friend AtRGB operator+(float f, const AtRGB& rgb);
   friend AtRGB operator-(float f, const AtRGB& rgb);
};

inline AtRGB operator*(float f, const AtRGB& rgb)
{
   return rgb * f;
}

inline AtRGB operator+(float f, const AtRGB& rgb)
{
   return rgb + f;
}

inline AtRGB operator-(float f, const AtRGB& rgb)
{
   AtRGB temp;
   temp.r = f - rgb.r;
   temp.g = f - rgb.g;
   temp.b = f - rgb.b;
   return temp;
}

/**
 * RGB color (same as AtRGB)
 */
typedef AtRGB AtColor;

/**
 * RGB color + alpha
 */
struct AtRGBA
{
   float r, g, b, a;

   AtRGB& rgb()
   {
      return *static_cast<AtRGB*>(static_cast<void*>(this));
   }

   const AtRGB& rgb() const
   {
      return *static_cast<const AtRGB*>(static_cast<const void*>(this));
   }

   /*
    * NOTE: the absence of a constructor is for a reason - it appears that
    *       older versions of C++ don't like union members to have constructors,
    *       and this struct is used in the AtParamValue union
    */

   AtRGBA operator+(const AtRGBA& rgba) const
   {
      AtRGBA temp;
      temp.r = r + rgba.r;
      temp.g = g + rgba.g;
      temp.b = b + rgba.b;
      temp.a = a + rgba.a;
      return temp;
   }

   AtRGBA& operator+=(const AtRGBA& rgba)
   {
      r += rgba.r;
      g += rgba.g;
      b += rgba.b;
      a += rgba.a;
      return *this;
   }

   AtRGBA operator+(float f) const
   {
      AtRGBA temp;
      temp.r = r + f;
      temp.g = g + f;
      temp.b = b + f;
      temp.a = a + f;
      return temp;
   }

   AtRGBA& operator+=(float f)
   {
      r += f;
      g += f;
      b += f;
      a += f;
      return *this;
   }

   AtRGBA operator-(const AtRGBA& rgba) const
   {
      AtRGBA temp;
      temp.r = r - rgba.r;
      temp.g = g - rgba.g;
      temp.b = b - rgba.b;
      temp.a = a - rgba.a;
      return temp;
   }

   AtRGBA& operator-=(const AtRGBA& rgba)
   {
      r -= rgba.r;
      g -= rgba.g;
      b -= rgba.b;
      a -= rgba.a;
      return *this;
   }

   AtRGBA operator-(float f) const
   {
      AtRGBA temp;
      temp.r = r - f;
      temp.g = g - f;
      temp.b = b - f;
      temp.a = a - f;
      return temp;
   }

   AtRGBA& operator-=(float f)
   {
      r -= f;
      g -= f;
      b -= f;
      a -= f;
      return *this;
   }

   AtRGBA operator-() const
   {
      AtRGBA temp;
      temp.r = -r;
      temp.g = -g;
      temp.b = -b;
      temp.a = -a;
      return temp;
   }

   AtRGBA operator*(const AtRGBA& rgba) const
   {
      AtRGBA temp;
      temp.r = r * rgba.r;
      temp.g = g * rgba.g;
      temp.b = b * rgba.b;
      temp.a = a * rgba.a;
      return temp;
   }

   AtRGBA operator*=(const AtRGBA& rgba)
   {
      r *= rgba.r;
      g *= rgba.g;
      b *= rgba.b;
      a *= rgba.a;
      return *this;
   }

   AtRGBA operator*(float f) const
   {
      AtRGBA temp;
      temp.r = r * f;
      temp.g = g * f;
      temp.b = b * f;
      temp.a = a * f;
      return temp;
   }

   AtRGBA operator*=(float f)
   {
      r *= f;
      g *= f;
      b *= f;
      a *= f;
      return *this;
   }

   AtRGBA operator/(const AtRGBA& rgba) const
   {
      AtRGBA temp;
      temp.r = r / rgba.r;
      temp.g = g / rgba.g;
      temp.b = b / rgba.b;
      temp.a = a / rgba.a;
      return temp;
   }

   AtRGBA operator/=(const AtRGBA& rgba)
   {
      r /= rgba.r;
      g /= rgba.g;
      b /= rgba.b;
      a /= rgba.a;
      return *this;
   }

   AtRGBA operator/(float f) const
   {
      AtRGBA temp;
      float inv = 1.0f / f;
      temp.r = r * inv;
      temp.g = g * inv;
      temp.b = b * inv;
      temp.a = a * inv;
      return temp;
   }

   AtRGBA operator/=(float f)
   {
      float inv = 1.0f / f;
      r *= inv;
      g *= inv;
      b *= inv;
      a *= inv;
      return *this;
   }


   bool operator==(const AtRGBA& rgba) const
   {
      return (r == rgba.r && g == rgba.g && b == rgba.b && a == rgba.a);
   }

   bool operator!=(const AtRGBA& rgba) const
   {
      return !(*this == rgba);
   }

   AtRGBA& operator=(float f)
   {
      r = f;
      g = f;
      b = f;
      a = f;
      return *this;
   }

   float& operator[](unsigned int i)
   {
      return *(&r + i);  // warning: no bounds checking!
   }

   const float& operator[](unsigned int i) const
   {
      return *(&r + i);  // warning: no bounds checking!
   }
   friend AtRGBA operator*(float f, const AtRGBA& rgba);
   friend AtRGBA operator+(float f, const AtRGBA& rgba);
   friend AtRGBA operator-(float f, const AtRGBA& rgba);
};

inline AtRGBA operator*(float f, const AtRGBA& rgba)
{
   return rgba * f;
}

inline AtRGBA operator+(float f, const AtRGBA& rgba)
{
   return rgba + f;
}

inline AtRGBA operator-(float f, const AtRGBA& rgba)
{
   AtRGBA temp;
   temp.r = f - rgba.r;
   temp.g = f - rgba.g;
   temp.b = f - rgba.b;
   temp.a = f - rgba.a;
   return temp;
}



/** \name Color Operations
 * \{
 */

/**
 * Create a RGB color using the same value for all components
 */
inline AtColor AiColor(float f)
{
   AtColor out = {f, f, f};
   return out;
}

/**
 * Create a RGB color from its components
 */
inline AtColor AiColor(float r, float g, float b)
{
   AtColor out = {r, g, b};
   return out;
}

/**
 * Create a RGBA color from its components
 */
inline AtRGBA AiRGBACreate(const float r, float g, float b, float a)
{
   AtRGBA out = {r, g, b, a};
   return out;
}

/**
 * Create a zero RGB color, c = (0, 0, 0)
 */
inline void AiColorReset(AtColor& c)
{
   c.r = 0.f;
   c.g = 0.f;
   c.b = 0.f;
}

/**
 * Create a zero RGBA color, c = (0, 0, 0, 0)
 */
inline void AiRGBAReset(AtRGBA& rgba)
{
   rgba.r = 0.f;
   rgba.g = 0.f;
   rgba.b = 0.f;
   rgba.a = 0.f;
}

/**
 * Linear interpolation between two RGB colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtColor AiColorLerp(float t, const AtColor& lo, const AtColor& hi)
{
   AtColor out;
   out.r = LERP(t, lo.r, hi.r);
   out.g = LERP(t, lo.g, hi.g);
   out.b = LERP(t, lo.b, hi.b);
   return out;
}

/**
 * Linear interpolation between two RGBA colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtRGBA AiColorLerp(float t, const AtRGBA& lo, const AtRGBA& hi)
{
   AtRGBA out;
   out.r = LERP(t, lo.r, hi.r);
   out.g = LERP(t, lo.g, hi.g);
   out.b = LERP(t, lo.b, hi.b);
   out.a = LERP(t, lo.a, hi.a);
   return out;
}

/**
 * Cubic Hermite interpolation between two RGB colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtColor AiColorHerp(float t, const AtColor& lo, const AtColor& hi)
{
   AtColor out;
   out.r = HERP(t, lo.r, hi.r);
   out.g = HERP(t, lo.g, hi.g);
   out.b = HERP(t, lo.b, hi.b);
   return out;
}

/**
 * Cubic Hermite interpolation between two RGBA colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtRGBA AiColorHerp(float t, const AtRGBA& lo, const AtRGBA& hi)
{
   AtRGBA out;
   out.r = HERP(t, lo.r, hi.r);
   out.g = HERP(t, lo.g, hi.g);
   out.b = HERP(t, lo.b, hi.b);
   out.a = HERP(t, lo.a, hi.a);
   return out;
}

/**
 * Linear interpolation between two RGBA colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtRGBA AiRGBALerp(float t, const AtRGBA& lo, const AtRGBA& hi)
{
   AtRGBA out;
   out.r = LERP(t, lo.r, hi.r);
   out.g = LERP(t, lo.g, hi.g);
   out.b = LERP(t, lo.b, hi.b);
   out.a = LERP(t, lo.a, hi.a);
   return out;
}

/**
 * Cubic Hermite interpolation between two RGBA colors.
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtRGBA AiRGBAHerp(float t, const AtRGBA& lo, const AtRGBA& hi)
{
   AtRGBA out;
   out.r = HERP(t, lo.r, hi.r);
   out.g = HERP(t, lo.g, hi.g);
   out.b = HERP(t, lo.b, hi.b);
   out.a = HERP(t, lo.a, hi.a);
   return out;
}

/**
 * Bilinear interpolation between four RGB colors.
 * (s,t)=(0,0) -> result = c00,
 * (s,t)=(1,1) -> result = c11,   (s,t) in [0,1]
 */
inline AtColor AiColorBiLerp(float s, float t, const AtColor& c00, const AtColor& c10, const AtColor& c01, const AtColor& c11)
{
   return AiColorLerp(s, AiColorLerp(t, c00, c01), AiColorLerp(t, c10, c11));
}

/**
 * Bicubic Hermite interpolation between four RGB colors.
 * (s,t)=(0,0) -> result = c00,
 * (s,t)=(1,1) -> result = c11,   (s,t) in [0,1]
 */
inline AtColor AiColorBiHerp(float s, float t, const AtColor& c00, const AtColor& c10, const AtColor& c01, const AtColor& c11)
{
   return AiColorHerp(s, AiColorHerp(t, c00, c01), AiColorHerp(t, c10, c11));
}

/**
 * Bilinear interpolation between four RGBA colors.
 * (s,t)=(0,0) -> result = c00,
 * (s,t)=(1,1) -> result = c11,   (s,t) in [0,1]
 */
inline AtRGBA AiRGBAbiLerp(float s, float t, const AtRGBA& c00, const AtRGBA& c10, const AtRGBA& c01, const AtRGBA& c11)
{
   return AiRGBALerp(s, AiRGBALerp(t, c00, c01), AiRGBALerp(t, c10, c11));
}

/**
 * Bicubic Hermite interpolation between four RGBA colors.
 * (s,t)=(0,0) -> result = c00,
 * (s,t)=(1,1) -> result = c11,   (s,t) in [0,1]
 */
inline AtRGBA AiRGBAbiHerp(float s, float t, const AtRGBA& c00, const AtRGBA& c10, const AtRGBA& c01, const AtRGBA& c11)
{
   return AiRGBAHerp(s, AiRGBAHerp(t, c00, c01), AiRGBAHerp(t, c10, c11));
}

/**
 * Clamp the RGB color vector to the specified range
 */
inline AtColor AiColorClamp(const AtColor& c, float lo, float hi)
{
   AtColor out;
   out.r = CLAMP(c.r, lo, hi);
   out.g = CLAMP(c.g, lo, hi);
   out.b = CLAMP(c.b, lo, hi);
   return out;
}

/**
 * Clamp the RGBA color vector to the specified range
 */
inline AtRGBA AiRGBAClamp(const AtRGBA& c, float lo, float hi)
{
   AtRGBA out;
   out.r = CLAMP(c.r, lo, hi);
   out.g = CLAMP(c.g, lo, hi);
   out.b = CLAMP(c.b, lo, hi);
   out.a = CLAMP(c.a, lo, hi);
   return out;
}

/**
 * Clip negative values
 */
inline void  AiColorClipToZero(AtColor& c)
{
   c.r = MAX(0.0f, c.r);
   c.g = MAX(0.0f, c.g);
   c.b = MAX(0.0f, c.b);
}

/**
 * Check for almost black
 */
inline bool AiColorIsSmall(const AtRGB& c, float epsilon = AI_EPSILON)
{
   return ABS(c.r) < epsilon && ABS(c.g) < epsilon && ABS(c.b) < epsilon;
}

/**
 * Check for almost black
 */
#define AiColorIsZero AiColorIsSmall

/**
 * Check if two colors are equal
 */
inline bool AiColorEqual(const AtColor& a, const AtColor& b)
{
   return (a.r == b.r) && (a.g == b.g) && (a.b == b.b);
}

/**
 * Check if two colors are equal, RGBA version (ignores alpha)
 */
inline bool AiColorEqual(const AtRGBA& a, const AtRGBA& b)
{
   return (a.r == b.r) && (a.g == b.g) && (a.b == b.b);
}

/**
 * Absolute value of color
 */
inline AtColor AiColorABS(const AtColor& c)
{
   AtColor out;
   out.r = ABS(c.r);
   out.g = ABS(c.g);
   out.b = ABS(c.b);
   return out;
}

/**
 * Absolute value of color, RGBA version
 */
inline AtRGBA AiColorABS(const AtRGBA& c)
{
   AtRGBA out;
   out.r = ABS(c.r);
   out.g = ABS(c.g);
   out.b = ABS(c.b);
   out.a = ABS(c.a);
   return out;
}

/**
 * Max RGB component of color
 */
inline float AiColorMaxRGB(const AtColor& c)
{
   return MAX3(c.r, c.g, c.b);
}

/**
 * Max RGB component of color, RGBA version (ignores alpha)
 */
inline float AiColorMaxRGB(const AtRGBA& c)
{
   return MAX3(c.r, c.g, c.b);
}

/**
 * Check to see if two colors differ by more than a threhsold
 */
inline bool AiColorThreshold(const AtColor& c1, const AtColor& c2, float t)
{
   return ABS(c1.r - c2.r) >= t || ABS(c1.g - c2.g) >= t || ABS(c1.b - c2.b) >= t;
}

/**
 * Convert a RGBA color to a RGB color (ignoring alpha)
 */
inline AtColor AiRGBAtoRGB(const AtRGBA& rgba)
{
   AtColor out;
   out.r = rgba.r;
   out.g = rgba.g;
   out.b = rgba.b;
   return out;
}

/**
 * Convert a RGB color to a RGBA color (setting alpha to 1.0)
 */
inline AtRGBA AiRGBtoRGBA(const AtColor& c)
{
   AtRGBA out;
   out.r = c.r;
   out.g = c.g;
   out.b = c.b;
   out.a = 1.f;
   return out;
}

/**
 * Convert a RGB color to grey scale (take average of R, G, B)
 */
inline float AiColorToGrey(const AtColor& c)
{
   return (c.r + c.g + c.b) / 3;
}

/**
 * Convert a RGBA color to grey scale (take average of R, G, B - ignore alpha)
 */
inline float AiColorToGrey(const AtRGBA& rgba)
{
   return (rgba.r + rgba.g + rgba.b) / 3;
}

/**
 * Check to see if an RGB color has any corrupted components (nan or infinite).
 */
AI_API bool AiRGBCorrupted(const AtRGB& rgba);

/**
 * Check to see if an RGBA color has any corrupted components (nan or infinite).
 */
AI_API bool AiRGBACorrupted(const AtRGBA& rgba);

/**
 * Check to see if a color has any corrupted components (nan or infinite).
 */
inline bool AiColorCorrupted(const AtColor& c)
{
   return AiRGBCorrupted(c);
}

/**
 * Check to see if an RGBA color has any corrupted components (nan or infinite).
 */
inline bool AiColorCorrupted(const AtRGBA& rgba)
{
   return AiRGBACorrupted(rgba);
}

/**
 * Barycentric interpolation of triangle vertex colors
 */
inline AtColor AiBerpRGB(float a, float b, const AtColor& c0, const AtColor& c1, const AtColor& c2)
{
   float c = 1 - (a+b);
   return c*c0 + a*c1 + b*c2;
}

AI_API void    AiColorGamma(AtColor* color, float gamma);
AI_API void    AiRGBAGamma(AtRGBA* color, float gamma);
AI_API AtColor AiColorHeatMap(const AtColor* map_colors, const float* map_values, unsigned int map_length, float lookup);

/*\}*/

/** \name Deprecated
 * \{
 */

inline void AiColorLerp(AtColor& out, float t, const AtColor& lo, const AtColor& hi)
{
   out = AiColorLerp(t, lo, hi);
}

inline void AiColorLerp(AtRGBA& out, float t, const AtRGBA& lo, const AtRGBA& hi)
{
   //out = AiColorLerp(t, lo, hi);
   //   this would overwrite out.a, unlike the macro we are replacing,
   //   so let's do it in a bit more convoluted but safer way:

   AtRGBA temp = AiColorLerp(t, lo, hi);
   out.r = temp.r;
   out.g = temp.g;
   out.b = temp.b;
}

inline void AiColorLerp(AtColor& out, float t, const AtRGBA& lo, const AtRGBA& hi)
{
   AtRGBA temp = AiColorLerp(t, lo, hi);
   out.r = temp.r;
   out.g = temp.g;
   out.b = temp.b;
}

inline void AiColorHerp(AtColor& out, float t, const AtColor& lo, const AtColor& hi)
{
   out = AiColorHerp(t, lo, hi);
}

inline void AiColorHerp(AtRGBA& out, float t, const AtRGBA& lo, const AtRGBA& hi)
{
   //out = AiColorHerp(t, lo, hi);
   //   this would overwrite out.a, unlike the macro we are replacing,
   //   so let's do it in a bit more convoluted but safer way:

   AtRGBA temp = AiColorHerp(t, lo, hi);
   out.r = temp.r;
   out.g = temp.g;
   out.b = temp.b;
}

inline void AiRGBALerp(AtRGBA& out, float t, const AtRGBA& lo, const AtRGBA& hi)
{
   out = AiRGBALerp(t, lo, hi);
}

inline void AiRGBAHerp(AtRGBA& out, float t, const AtRGBA& lo, const AtRGBA& hi)
{
   out = AiRGBAHerp(t, lo, hi);
}

inline void AiColorBiLerp(AtColor& out, float s, float t, const AtColor& c00, const AtColor& c10, const AtColor& c01, const AtColor& c11)
{
   out = AiColorBiLerp(s, t, c00, c10, c01, c11);
}

inline void AiColorBiHerp(AtColor& out, float s, float t, const AtColor& c00, const AtColor& c10, const AtColor& c01, const AtColor& c11)
{
   out = AiColorBiHerp(s, t, c00, c10, c01, c11);
}

inline void AiRGBAbiLerp(AtRGBA& out, float s, float t, const AtRGBA& c00, const AtRGBA& c10, const AtRGBA& c01, const AtRGBA& c11)
{
   out = AiRGBAbiLerp(s, t, c00, c10, c01, c11);
}

inline void AiRGBAbiHerp(AtRGBA& out, float s, float t, const AtRGBA& c00, const AtRGBA& c10, const AtRGBA& c01, const AtRGBA& c11)
{
   out = AiRGBAbiHerp(s, t, c00, c10, c01, c11);
}

inline void AiColorClamp(AtColor& out, const AtColor& c, float lo, float hi)
{
   out = AiColorClamp(c, lo, hi);
}

inline void AiRGBAClamp(AtRGBA& out, const AtRGBA& c, float lo, float hi)
{
   out = AiRGBAClamp(c, lo, hi);
}

inline void AiColorABS(AtColor& out, const AtColor& c)
{
   out = AiColorABS(c);
}

inline void AiColorABS(AtRGBA& out, const AtRGBA& c)
{
   // out = AiColorABS(c);
   //   this would overwrite out.a, unlike the macro we are replacing,
   //   so let's do it in a bit more convoluted but safer way:

   AtRGBA temp = AiColorABS(c);
   out.r = temp.r;
   out.g = temp.g;
   out.b = temp.b;
}

inline void AiRGBAtoRGB(const AtRGBA& rgba, AtColor& out)
{
   out = AiRGBAtoRGB(rgba);
}

inline void AiRGBtoRGBA(const AtColor& c, AtRGBA& out)
{
   out = AiRGBtoRGBA(c);
}

inline void AiBerpRGB(float a, float b, const AtColor& c0, const AtColor& c1, const AtColor& c2, AtColor& out)
{
   out = AiBerpRGB(a, b, c0, c1, c2);
}

// Use the identical (but shorter) AiColor() instead
inline AtColor AiColorCreate(float r, float g, float b)
{
   AtColor out = {r, g, b};
   return out;
}

inline void AiColorCreate(AtColor& out, float r, float g, float b)
{
   out.r = r;
   out.g = g;
   out.b = b;
}

inline void AiRGBACreate(AtRGBA& out, float r, float g, float b, float a)
{
   out.r = r;
   out.g = g;
   out.b = b;
   out.a = a;
}

inline void AiColorAdd(AtColor& a, const AtColor& b, const AtColor& c)
{
   a = b + c;
}

inline void AiColorSub(AtColor& a, const AtColor& b, const AtColor& c)
{
   a = b - c;
}

inline void AiColorScale(AtColor& a, const AtColor& b, float k)
{
   a = b * k;
}

inline void AiColorAddScale(AtColor& a, const AtColor& b, const AtColor& c, float k)
{
   a = b + c * k;
}

inline void AiColorMult(AtColor& a, const AtColor& b, const AtColor& c)
{
   a = b * c;
}

inline void AiColorDiv(AtColor& a, const AtColor& b, const AtColor& c)
{
   a = b / c;
}

/*\}*/

/** \name Constants
 * \{
 */
AI_API AtRGB  AI_RGB_BLACK;
AI_API AtRGB  AI_RGB_RED;
AI_API AtRGB  AI_RGB_GREEN;
AI_API AtRGB  AI_RGB_BLUE;
AI_API AtRGB  AI_RGB_50GREY;
AI_API AtRGB  AI_RGB_WHITE;

AI_API AtRGBA AI_RGBA_BLACK;
AI_API AtRGBA AI_RGBA_RED;
AI_API AtRGBA AI_RGBA_GREEN;
AI_API AtRGBA AI_RGBA_BLUE;
AI_API AtRGBA AI_RGBA_50GREY;
AI_API AtRGBA AI_RGBA_WHITE;
/*\}*/

/*\}*/
