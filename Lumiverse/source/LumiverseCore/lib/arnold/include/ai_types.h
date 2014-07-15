/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Basic data types and scalar operations
 */

#pragma once
#include "ai_api.h"
#include "ai_constants.h"
#include <cmath>
#include <cstdlib> // for int abs(int)

/** \defgroup ai_types Basic Types
 *
 * \details
 * Portable definitions for common basic types (different sizes of integer, etc).
 * Using these types ensures portability of client code across platforms.
 *
 * \{
 */

/**
 * NULL value for null pointers
 */
#ifndef NULL
#define NULL 0
#endif


/****************************************************************************
   Basic data types
****************************************************************************/

typedef char               AtInt8;     /**<  8-bit signed integer   */
typedef short              AtInt16;    /**< 16-bit signed integer   */
typedef int                AtInt32;    /**< 32-bit signed integer   */
typedef signed long long   AtInt64;    /**< 64-bit signed integer   */
typedef unsigned char      AtByte;     /**<  8-bit unsigned integer (same as \ref AtUInt8) */
typedef unsigned char      AtUInt8;    /**<  8-bit unsigned integer */
typedef unsigned short     AtUInt16;   /**< 16-bit unsigned integer */
typedef unsigned int       AtUInt32;   /**< 32-bit unsigned integer */
typedef unsigned long long AtUInt64;   /**< 64-bit unsigned integer */


/****************************************************************************
   Scalar operations
****************************************************************************/

/**
 * Absolute value of 'a' 
 */
template <typename T>
inline T ABS(T a)
{
   return std::abs(a);
}

#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

/**
 * Minimum of 'a' and 'b' 
 */
template <typename T>
inline T MIN(T a, T b)
{
   return (a < b) ? a : b;
}

/**
 * Maximum of 'a' and 'b'
 */
template <typename T>
inline T MAX(T a, T b)
{
   return (a > b) ? a : b;
}

/**
 * Minimum of three values: a, b, c
 */
template <typename T>
inline T MIN3(T a, T b, T c)
{
   return MIN(MIN(a, b), c);
}

/**
 * Maximum of three values: a, b, c
 */
template <typename T>
inline T MAX3(T a, T b, T c)
{
   return MAX(MAX(a, b), c);
}

/**
 * Minimum of four values: a, b, c, d
 */
template <typename T>
inline T MIN4(T a, T b, T c, T d)
{
   return MIN(MIN(a, b), MIN(c, d));
}

/**
 * Maximum of four values: a, b, c, d
 */
template <typename T>
inline T MAX4(T a, T b, T c, T d)
{
   return MAX(MAX(a, b), MAX(c, d));
}

/**
 * Square of 'a'
 */
template <typename T>
inline T SQR(T a)
{
   return (a * a);
}

/**
 * Largest integer that is smaller than or equal to 'a'
 */
template <typename T>
inline AtInt64 FLOOR(T a)
{
   return (AtInt64) a - (a < (T) 0);
}

/**
 * Smallest integer that is bigger than or equal to 'a'
 */
template <typename T>
inline AtInt64 CEIL(T a)
{
   return (AtInt64) a + (a > (T) 0 && a != (AtInt64) a);
}

/**
 * Round 'a' to nearest int 
 */
template <typename T>
inline AtInt64 ROUND(T a)
{
   return FLOOR(a + (T) 0.5);
}

/**
 * Take binary sign of 'a' (either -1, or 1 if >= 0)
 */
template <typename T>
inline int SGN(T a)
{
   return (a < 0) ? -1 : 1;
}

/**
 * Swap 'a' and 'b'
 */
template <typename T>
inline void SWAP(T& a, T& b)
{
   T temp = a;
   a = b;
   b = temp;
}

/**
 * Clamp the input to the specified range 
 */
template <typename T>
inline T CLAMP(T v, T lo, T hi)
{
   return (v > lo) ? ((v > hi) ? hi : v) : lo;
}

/**
 * Safe arc cosine of 'x' (acos() returns NaN if x>1)
 */
template <typename T>
inline T ACOS(T x)
{
   return (x >= 1) ? 0 : std::acos(x);
}

/**
 * Linear interpolation between 'a' and 'b' using 't' (0<=t<=1)
 */
template <typename T>
inline T LERP(T t, T a, T b)
{
   return ((1 - t) * a) + (b * t);
}

/**
 * Hermite interpolation between 0 and 1 using 't' (0<=t<=1)
 */
template <typename T>
inline T HERP01(T t)
{
   return t * t * ((T) 3 - (T) 2 * t);
}

/**
 * Hermite interpolation between 'a' and 'b' using 't' (0<=t<=1)
 */
template <typename T>
inline T HERP(T t, T a, T b)
{
   return LERP(HERP01(t), a, b);
}

/**
 * Bilinear interpolation between four float values using 's' and 't' (0<=st<=1)
 */
template <typename T>
inline T BILERP(T s, T t, T c00, T c10, T c01, T c11)
{
   T c0x = LERP(t, c00, c01);
   T c1x = LERP(t, c10, c11);
   return LERP(s, c0x, c1x);
}

/**
 * Bicubic Hermite interpolation between four float values using 's' and 't' (0<=st<=1)
 */
template <typename T>
inline T BIHERP(T s, T t, T c00, T c10, T c01, T c11)
{
   T c0x = HERP(t, c00, c01);
   T c1x = HERP(t, c10, c11);
   return HERP(s, c0x, c1x);
}

/**
 * Bias function
 */
template <typename T>
inline T BIAS(T a, T b)
{
   return (a > 0) ? ((b == (T) 0.5) ? a : std::pow(a, (std::log(b) * (T)-1.442695041))) : 0;
}

/**
 * Gain function
 */
template <typename T>
inline T GAIN(T a, T g)
{
   return (g == (T) 0.5) ? a : ((a < (T) 0.5) ? (BIAS(a + a, (T) 1 - g) * (T) 0.5) : ((T) 1 - BIAS((T) 2 - a - a, (T) 1 - g) * (T) 0.5));
}

/**
 * Step function
 */
template <typename T>
inline T STEP(T x, T e)
{
   return (x < e) ? (T) 0 : (T) 1;
}

/**
 * Filtered-step function
 */
template <typename T>
inline T FILTERSTEP(T x, T e, T w)
{
   return CLAMP((x + w * (T) 0.5 - e) / w, (T) 0, (T) 1);
}

/**
 * Linearly interpolated step from 'lo' to 'hi'.
 * Returns x where (0 <= x <= 1). Similar to FILTERSTEP()
 * but slightly different mode of calling
 */
template <typename T>
inline T LINEARSTEP(T lo, T hi, T t)
{
   return CLAMP((t - lo) / (hi - lo), (T) 0, (T) 1);
}

/**
 * RenderMan's smoothstep() function.
 * Returns 0 if (t < e0) or 1 if (t > e1) or
 * a hermitian interpolation for (e0 < t < e1)
 */
template <typename T>
inline T SMOOTHSTEP(T e0, T e1, T t)
{
   return HERP01(CLAMP((t - e0) / (e1 - e0), (T) 0, (T) 1));
}
   
AI_API bool AiIsFinite(float x);

AI_API float fast_exp(float x);

/*\}*/
