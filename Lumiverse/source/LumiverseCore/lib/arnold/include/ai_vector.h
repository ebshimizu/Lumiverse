/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Vector math types, operators and utilities
 */

#pragma once
#include "ai_constants.h"
#include "ai_types.h"
#include "ai_api.h"
#include <math.h>

/** \defgroup ai_vector AtVector API
 *
 * \details
 * This module contains vector math types and vector utilities. There
 * are two equivalent definitions of the basic 3D vector type (\c AtPoint,
 * and \c AtVector).
 *
 * \{
 */

/**
 * 3D point (single precision)
 */
struct AtPoint
{
   float x, y, z;

   AtPoint operator+(const AtPoint& p) const
   {
      AtPoint temp;
      temp.x = x + p.x;
      temp.y = y + p.y;
      temp.z = z + p.z;
      return temp;
   }

   AtPoint& operator+=(const AtPoint& p)
   {
      x += p.x;
      y += p.y;
      z += p.z;
      return *this;
   }

   AtPoint operator+(float f) const
   {
      AtPoint temp;
      temp.x = x + f;
      temp.y = y + f;
      temp.z = z + f;
      return temp;
   }

   AtPoint& operator+=(float f)
   {
      x += f;
      y += f;
      z += f;
      return *this;
   }

   AtPoint operator-(const AtPoint& p) const
   {
      AtPoint temp;
      temp.x = x - p.x;
      temp.y = y - p.y;
      temp.z = z - p.z;
      return temp;
   }

   AtPoint& operator-=(const AtPoint& p)
   {
      x -= p.x;
      y -= p.y;
      z -= p.z;
      return *this;
   }

   AtPoint operator-(float f) const
   {
      AtPoint temp;
      temp.x = x - f;
      temp.y = y - f;
      temp.z = z - f;
      return temp;
   }

   AtPoint& operator-=(float f)
   {
      x -= f;
      y -= f;
      z -= f;
      return *this;
   }

   AtPoint operator-() const
   {
      AtPoint temp;
      temp.x = -x;
      temp.y = -y;
      temp.z = -z;
      return temp;
   }

   AtPoint operator*(const AtPoint& p) const
   {
      AtPoint temp;
      temp.x = x * p.x;
      temp.y = y * p.y;
      temp.z = z * p.z;
      return temp;
   }

   AtPoint operator*=(const AtPoint& p)
   {
      x *= p.x;
      y *= p.y;
      z *= p.z;
      return *this;
   }

   AtPoint operator*(float f) const
   {
      AtPoint temp;
      temp.x = x * f;
      temp.y = y * f;
      temp.z = z * f;
      return temp;
   }

   AtPoint operator*=(float f)
   {
      x *= f;
      y *= f;
      z *= f;
      return *this;
   }

   AtPoint operator/(const AtPoint& p) const
   {
      AtPoint temp;
      temp.x = x / p.x;
      temp.y = y / p.y;
      temp.z = z / p.z;
      return temp;
   }

   AtPoint operator/=(const AtPoint& p)
   {
      x /= p.x;
      y /= p.y;
      z /= p.z;
      return *this;
   }

   AtPoint operator/(float f) const
   {
      AtPoint temp;
      float inv = 1.0f / f;
      temp.x = x * inv;
      temp.y = y * inv;
      temp.z = z * inv;
      return temp;
   }

   AtPoint operator/=(float f)
   {
      float inv = 1.0f / f;
      x *= inv;
      y *= inv;
      z *= inv;
      return *this;
   }

   bool operator==(const AtPoint& p) const
   {
      return (x == p.x && y == p.y && z == p.z);
   }

   bool operator!=(const AtPoint& p) const
   {
      return !(*this == p);
   }

   AtPoint& operator=(float f)
   {
      x = f;
      y = f;
      z = f;
      return *this;
   }

   float& operator[](unsigned int i)
   {
      return *(&x + i);  // no bounds checking!
   }

   const float& operator[](unsigned int i) const
   {
      return *(&x + i);  // no bounds checking!
   }

   friend AtPoint operator*(float f, const AtPoint& p);
   friend AtPoint operator+(float f, const AtPoint& p);
   friend AtPoint operator-(float f, const AtPoint& p);
};

inline AtPoint operator*(float f, const AtPoint& p)
{
   return p * f;
}

inline AtPoint operator+(float f, const AtPoint& p)
{
   return p + f;
}

inline AtPoint operator-(float f, const AtPoint& p)
{
   AtPoint temp;
   temp.x = f - p.x;
   temp.y = f - p.y;
   temp.z = f - p.z;
   return temp;
}

/**
 * 2D point
 */
struct AtPoint2
{
   float x, y;

   AtPoint2 operator+(const AtPoint2& p) const
   {
      AtPoint2 temp;
      temp.x = x + p.x;
      temp.y = y + p.y;
      return temp;
   }

   AtPoint2& operator+=(const AtPoint2& p)
   {
      x += p.x;
      y += p.y;
      return *this;
   }

   AtPoint2 operator+(float f) const
   {
      AtPoint2 temp;
      temp.x = x + f;
      temp.y = y + f;
      return temp;
   }

   AtPoint2& operator+=(float f)
   {
      x += f;
      y += f;
      return *this;
   }

   AtPoint2 operator-(const AtPoint2& p) const
   {
      AtPoint2 temp;
      temp.x = x - p.x;
      temp.y = y - p.y;
      return temp;
   }

   AtPoint2& operator-=(const AtPoint2& p)
   {
      x -= p.x;
      y -= p.y;
      return *this;
   }

   AtPoint2 operator-(float f) const
   {
      AtPoint2 temp;
      temp.x = x - f;
      temp.y = y - f;
      return temp;
   }

   AtPoint2& operator-=(float f)
   {
      x -= f;
      y -= f;
      return *this;
   }

   AtPoint2 operator-() const
   {
      AtPoint2 temp;
      temp.x = -x;
      temp.y = -y;
      return temp;
   }

   AtPoint2 operator*(const AtPoint2& p) const
   {
      AtPoint2 temp;
      temp.x = x * p.x;
      temp.y = y * p.y;
      return temp;
   }

   AtPoint2 operator*=(const AtPoint2& p)
   {
      x *= p.x;
      y *= p.y;
      return *this;
   }

   AtPoint2 operator*(float f) const
   {
      AtPoint2 temp;
      temp.x = x * f;
      temp.y = y * f;
      return temp;
   }

   AtPoint2 operator*=(float f)
   {
      x *= f;
      y *= f;
      return *this;
   }

   AtPoint2 operator/(const AtPoint2& p) const
   {
      AtPoint2 temp;
      temp.x = x / p.x;
      temp.y = y / p.y;
      return temp;
   }

   AtPoint2 operator/=(const AtPoint2& p)
   {
      x /= p.x;
      y /= p.y;
      return *this;
   }

   AtPoint2 operator/(float f) const
   {
      AtPoint2 temp;
      float inv = 1.0f / f;
      temp.x = x * inv;
      temp.y = y * inv;
      return temp;
   }

   AtPoint2 operator/=(float f)
   {
      float inv = 1.0f / f;
      x *= inv;
      y *= inv;
      return *this;
   }

   bool operator==(const AtPoint2& p) const
   {
      return (x == p.x && y == p.y);
   }

   bool operator!=(const AtPoint2& p) const
   {
      return !(*this == p);
   }

   AtPoint2& operator=(float f)
   {
      x = f;
      y = f;
      return *this;
   }

   float& operator[](unsigned int i)
   {
      return *(&x + i);  // no bounds checking!
   }

   const float& operator[](unsigned int i) const
   {
      return *(&x + i);  // no bounds checking!
   }

   friend AtPoint2 operator*(float f, const AtPoint2& p);
   friend AtPoint2 operator+(float f, const AtPoint2& p);
   friend AtPoint2 operator-(float f, const AtPoint2& p);
};

inline AtPoint2 operator*(float f, const AtPoint2& p)
{
   return p * f;
}

inline AtPoint2 operator+(float f, const AtPoint2& p)
{
   return p + f;
}

inline AtPoint2 operator-(float f, const AtPoint2& p)
{
   AtPoint2 temp;
   temp.x = f - p.x;
   temp.y = f - p.y;
   return temp;
}

/**
 * Homogeneous point
 */
struct AtHPoint
{
   float x, y, z, w;
};

/** \name Derived Typedefs
 * \{
 */
typedef AtPoint AtVector;    /**< 3D vector (single precision) */
typedef AtPoint2 AtVector2;  /**< 2D vector */
/*\}*/

/** \name Vector Components
 * \{
 */
#define AI_X  0   /**< X component */
#define AI_Y  1   /**< Y component */
#define AI_Z  2   /**< Z component */
/*\}*/



/** \name 2D Vector Operations
 * \{
 */

/**
 * Create a 2D vector:   vout = (x, y)
 */
inline void AiV2Create(AtVector2& vout, float x, float y)
{
   vout.x = x;
   vout.y = y;
}

/**
 * Dot product:   <v1, v2>
 */
inline float AiV2Dot(const AtVector2& v1, const AtVector2& v2)
{
   return v1.x * v2.x + v1.y * v2.y;
}

/**
 * Vector Length:   ||v1||
 */
inline float AiV2Length(const AtVector2& v1)
{
   return sqrtf(v1.x * v1.x + v1.y * v1.y);
}

/**
 * Distance between two points:   ||p1-p2||
 */
inline float AiV2Dist(const AtPoint2& p1, const AtPoint2& p2)
{
   return sqrtf(SQR(p1.x-p2.x) + SQR(p1.y - p2.y));
}

/**
 * 2D vector linear interpolation
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtVector2 AiV2Lerp(float t, const AtVector2& lo, const AtVector2& hi)
{
   AtVector2 out;
   out.x = LERP(t, lo.x, hi.x);
   out.y = LERP(t, lo.y, hi.y);
   return out;
}

/**
 * Clamp each vector coordinate to the range [lo,hi]
 */
inline AtVector2 AiV2Clamp(const AtVector2& in, float lo, float hi)
{
   AtVector2 out;
   out.x = CLAMP(in.x, lo, hi);
   out.y = CLAMP(in.y, lo, hi);
   return out;
}

/*\}*/



/** \name 3D Vector Operations
 * \{
 */

/**
 * Vector Length:   ||a||
 */
inline float AiV3Length(const AtVector& a)
{
   return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}

/**
 * Dot product:   <a, b>
 */
inline float AiV3Dot(const AtVector& a, const AtVector& b)
{
   return a.x*b.x + a.y*b.y + a.z*b.z;
} 

/**
 * Distance between two points:   ||a-b||
 */
inline float AiV3Dist(const AtVector& a, const AtVector& b)
{
   return sqrtf(SQR(a.x-b.x) + SQR(a.y-b.y) + SQR(a.z-b.z));
}

/**
 * Squared distance between two points:   ||a-b||^2
 */
inline float AiV3Dist2(const AtVector& a, const AtVector& b)
{
   return SQR(a.x-b.x) + SQR(a.y-b.y) + SQR(a.z-b.z); 
}

/**
 * Signed distance between point x and a plane defined by point p and normalized vector n
 */
inline float AiV3DistPlane(const AtPoint& x, const AtPoint& p, const AtVector& n)
{
   return AiV3Dot(x, n) - AiV3Dot(p, n);
}

/**
 * Cross product:   a x b
 */
inline AtVector AiV3Cross(const AtVector& a, const AtVector& b)
{
   AtVector out;
   out.x = a.y * b.z - a.z * b.y;
   out.y = a.z * b.x - a.x * b.z;
   out.z = a.x * b.y - a.y * b.x;
   return out;
}

/**
 * Normalize a vector:   a / ||a||
 */
inline AtVector AiV3Normalize(const AtVector& a)
{
   float tmp = AiV3Length(a);
   if (tmp != 0)
      tmp = 1 / tmp;
   AtVector out;
   out.x = a.x * tmp;
   out.y = a.y * tmp;
   out.z = a.z * tmp;
   return out;
}

/**
 * 3D vector linear interpolation
 * (t=0 -> result=lo, t=1 -> result=hi)
 */
inline AtVector AiV3Lerp(float t, const AtVector& lo, const AtVector& hi)
{
   AtVector out;
   out.x = LERP(t, lo.x, hi.x);
   out.y = LERP(t, lo.y, hi.y);
   out.z = LERP(t, lo.z, hi.z);
   return out;
}

/**
 * Clamp each vector coordinate to the range [lo,hi]
 */
inline AtVector AiV3Clamp(const AtVector& in, float lo, float hi)
{
   AtVector out;
   out.x = CLAMP(in.x, lo, hi);
   out.y = CLAMP(in.y, lo, hi);
   out.z = CLAMP(in.z, lo, hi);
   return out;
}

/**
 * Minimum of two vectors, component-wise
 */
inline AtVector AiV3Min(const AtVector& a, const AtVector& b)
{
   AtVector out;
   out.x = MIN(a.x, b.x);
   out.y = MIN(a.y, b.y);
   out.z = MIN(a.z, b.z);
   return out;
}

/**
 * Maximum of two vectors, component-wise
 */
inline AtVector AiV3Max(const AtVector& a, const AtVector& b)
{
   AtVector out;
   out.x = MAX(a.x, b.x);
   out.y = MAX(a.y, b.y);
   out.z = MAX(a.z, b.z);
   return out;
}

/**
 * Absolute value of each component
 */
inline AtVector ABS(const AtVector& a)
{
   AtVector out = { ABS(a.x), ABS(a.y), ABS(a.z) };
   return out;
}

/**
 * Barycentric interpolation of a point inside a triangle
 */
inline AtVector AiBerpXYZ(float a, float b, const AtVector& p0, const AtVector& p1, const AtVector& p2)
{
   float c = 1 - (a + b);
   return c*p0 + a*p1 + b*p2;
}

/**
 * Check whether a vector has all valid components (not NaN and not infinite)
 */
AI_API bool AiV3Exists(const AtVector& a);

/**
 * Vector equality:   a == b
 */
inline bool AiV3Equal(const AtVector& a, const AtVector& b)
{
   return (a.x==b.x) && (a.y==b.y) && (a.z==b.z);
}

/**
 * Check for a zero vector, within a small tolerance:   ||a|| < epsilon
 */
inline bool AiV3IsSmall(const AtVector& a, float epsilon = AI_EPSILON)
{
   return ABS(a.x) < epsilon && ABS(a.y) < epsilon && ABS(a.z) < epsilon;
}

/**
 * Check for a zero vector:   ||a|| == 0
 */
inline bool AiV3IsZero(const AtVector& a)
{
   return AiV3IsSmall(a);
}

/**
 * Rotate vector a so that it aligns with frame {u,v,w}
 */
inline void AiV3RotateToFrame(AtVector& a, const AtVector& u, const AtVector& v, const AtVector& w)
{
   a = u * a.x + v * a.y + w * a.z;
}

/**
 * Barycentric interpolation of UV coordinates inside a 3D triangle
 */
inline void AiBerpUV(float a, float b, float u0, float v0, float u1, float v1, float u2, float v2, float* u, float* v)
{
   float c = 1.0f - (a + b);
   *u = c * u0 + a * u1 + b * u2;
   *v = c * v0 + a * v1 + b * v2;
}

/*\}*/

/** \name 4D Vector Operations
 * \{
 */

/**
 * Create a 4D vector:   pout = (x, y, z, w)
 */
inline void AiV4Create(AtHPoint& vout, float x, float y, float z, float w)
{
   vout.x = x;
   vout.y = y;
   vout.z = z;
   vout.w = w;
}

/**
 * Create a 4D point:   pout = (v.x, v.y, v.z, 1)
 */
inline void AiV4CreatePoint(AtHPoint& pout, const AtVector& v)
{
   pout.x = v.x;
   pout.y = v.y;
   pout.z = v.z;
   pout.w = 1.0f;
}

/**
 * Create a 4D vector:   vout = (v.x, v.y, v.z, 0)
 */
inline void AiV4CreateVector(AtHPoint& vout, const AtVector& v)
{
   vout.x = v.x;
   vout.y = v.y;
   vout.z = v.z;
   vout.w = 0.0f;
}

/**
 * Add two vectors:   vout = v1 + v2
 */
inline void AiV4Add(AtHPoint& vout, const AtHPoint& v1, const AtHPoint& v2)
{
   vout.x = v1.x + v2.x;
   vout.y = v1.y + v2.y;
   vout.z = v1.z + v2.z;
   vout.w = v1.w + v2.w;
}

/**
 * Substract two vectors:   vout = v1 - v2
 */
inline void AiV4Sub(AtHPoint& vout, const AtHPoint& v1, const AtHPoint& v2)
{
   vout.x = v1.x - v2.x;
   vout.y = v1.y - v2.y;
   vout.z = v1.z - v2.z;
   vout.w = v1.w - v2.w;
}

/**
 * Scale a vector by a constant:   vout = vin * k
 */
inline void AiV4Scale(AtHPoint& vout, const AtHPoint& vin, float k)
{
   vout.x = vin.x * k;
   vout.y = vin.y * k;
   vout.z = vin.z * k;
   vout.w = vin.w * k;
}

/**
 * Negate a vector:   vout = -vin
 */
inline void AiV4Neg(AtHPoint& vout, const AtHPoint& vin)
{
   vout.x = -vin.x;
   vout.y = -vin.y;
   vout.z = -vin.z;
   vout.w = -vin.w;
}

/**
 * Project a homogeneous vector back into 3d: vout = vin.w != 0 ? vin * (1 / vin.w) : (0,0,0)
 */
inline void AiV4Project(AtVector& vout, const AtHPoint& vin)
{
   if (vin.w != 0)
   {
      float f = 1.0f / vin.w;
      vout.x = vin.x * f;
      vout.y = vin.y * f;
      vout.z = vin.z * f;
   }
   else
   {
      vout.x = 0;
      vout.y = 0;
      vout.z = 0;
   }
}

/*\}*/

/** \name Deprecated
 * \{
 */

inline void AiV2Add(AtVector2& out, const AtVector2& v1, const AtVector2& v2)
{
   out = v1 + v2;
}

inline void AiV2Sub(AtVector2& out, const AtVector2& v1, const AtVector2& v2)
{
   out = v1 - v2;
}

inline void AiV2Scale(AtVector2& out, const AtVector2& v1, float k)
{
   out = v1 * k;
}

inline void AiV2Lerp(AtVector2& out, float t, const AtVector2& lo, const AtVector2& hi)
{
   out = AiV2Lerp(t, lo, hi);
}

inline void AiV2Clamp(AtVector2& out, const AtVector2& in, float lo, float hi)
{
   out = AiV2Clamp(in, lo, hi);
}
inline void AiV3Cross(AtVector& out, const AtVector& a, const AtVector& b)
{
   out = AiV3Cross(a, b);
}

inline void AiV3Normalize(AtVector& out, const AtVector& in)
{
   out = AiV3Normalize(in);
}

inline void AiV3Lerp(AtVector& out, float t, const AtVector& lo, const AtVector& hi)
{
   out = AiV3Lerp(t, lo, hi);
}

inline void AiV3Min(AtVector& out, const AtVector& a, const AtVector& b)
{
   out = AiV3Min(a, b);
}

inline void AiV3Max(AtVector& out, const AtVector& a, const AtVector& b)
{
   out = AiV3Max(a, b);
}

inline void AiV3Clamp(AtVector& out, const AtVector& in, float lo, float hi)
{
   out = AiV3Clamp(in, lo, hi);
}

inline void AiBerpXYZ(float a, float b, const AtVector& p0, const AtVector& p1, const AtVector& p2, AtVector& out)
{
   out = AiBerpXYZ(a, b, p0, p1, p2);
}

inline void AiV3Create(AtVector& vout, float x, float y, float z)
{
   vout.x = x;
   vout.y = y;
   vout.z = z;
}

inline void AiV3Copy(AtVector& vout, const AtVector& vin)
{
   vout = vin;
}

inline void AiV3Add(AtVector& vout, const AtVector& v1, const AtVector& v2)
{
   vout = v1 + v2;
}

inline void AiV3Sub(AtVector& vout, const AtVector& v1, const AtVector& v2)
{
   vout = v1 - v2;
}

inline void AiV3AddScalar(AtVector& vout, const AtVector& vin, float k)
{
   vout = vin + k;
}

inline void AiV3SubScalar(AtVector& vout, const AtVector& vin, float k)
{
   vout = vin - k;
}

inline void AiV3Scale(AtVector& vout, const AtVector& vin, float k)
{
   vout = vin * k;
}

inline void AiV3Neg(AtVector& vout, const AtVector& vin)
{
   vout = -vin;
}

inline void AiV3Invert(AtVector& vout, const AtVector& vin)
{
   vout.x = 1.0f / vin.x;
   vout.y = 1.0f / vin.y;
   vout.z = 1.0f / vin.z;
}

inline void AiV3Mult(AtVector& vout, const AtVector& v1, const AtVector& v2)
{
   vout = v1 * v2;
}

inline void AiV3Div(AtVector& vout, const AtVector& v1, const AtVector& v2)
{
   vout = v1 / v2;
}

inline void AiV3Midpoint(AtPoint& pout, const AtVector& v1, const AtVector& v2)
{
   pout = (v1 + v2) * 0.5f;
}

inline void AiV3ProjectPointOnLine(AtPoint& pout, const AtPoint& o, const AtVector& dir, const AtPoint& p)
{
   pout = o + dir * AiV3Dot(dir, p - o);
}

inline void AiV3RayPoint(AtPoint& pout, const AtPoint& origin, const AtVector& dir, double t)
{
   pout.x = (float) (origin.x + dir.x * t);
   pout.y = (float) (origin.y + dir.y * t);
   pout.z = (float) (origin.z + dir.z * t);
}

#define AiV3isZero AiV3IsSmall   // the wrong spelling is deprecated

/*\}*/


/** \name Constants
 * \{
 */
AI_API AtPoint  AI_P3_ZERO;
AI_API AtVector AI_V3_ZERO;
AI_API AtVector AI_V3_HALF;
AI_API AtVector AI_V3_ONE;
AI_API AtVector AI_V3_X;
AI_API AtVector AI_V3_Y;
AI_API AtVector AI_V3_Z;
AI_API AtVector AI_V3_NEGX;
AI_API AtVector AI_V3_NEGY;
AI_API AtVector AI_V3_NEGZ;
AI_API AtPoint2 AI_P2_ZERO;
AI_API AtPoint2 AI_P2_ONE;
/*\}*/

/*\}*/
