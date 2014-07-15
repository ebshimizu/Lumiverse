/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Axis-aligned bounding box types and utilities.
 */

#pragma once
#include "ai_vector.h"
#include "ai_api.h"

/** \defgroup ai_bbox AtBBox API
 * \{
 */

/**
 * 3D axis-aligned bounding box (uses single-precision)
 */
struct AtBBox
{
   AtPoint min, max;
};

/** 
 * 2D axis-aligned bounding box (uses integers)
 */
struct AtBBox2
{
   int minx, miny, maxx, maxy;
};


/**
 * Expand a bounding box with some safety slack volume
 */
inline void AiBBoxAddSlack(AtBBox& bbox, float slack)
{
   bbox.min -= slack;
   bbox.max += slack;
}

/** 
 * Initialize a bounding box
 */
inline void AiBBoxInit(AtBBox& bbox, float bound)
{
   AiV3Create(bbox.min,  bound,  bound,  bound);
   AiV3Create(bbox.max, -bound, -bound, -bound);
}

/** 
 * Expand a bounding box with a point
 */
inline void AiBBoxExpand(AtBBox& bbox, const AtVector& v)
{
   bbox.min = AiV3Min(bbox.min, v);
   bbox.max = AiV3Max(bbox.max, v);
}

/** 
 * Compute the bbox of a triangle
 */
inline void AiBBoxTriangle(AtBBox& bbox, const AtPoint& p0, const AtPoint& p1, const AtPoint& p2)
{
   bbox.min = bbox.max = p0;
   bbox.min = AiV3Min(bbox.min, p1);
   bbox.max = AiV3Max(bbox.max, p1);
   bbox.min = AiV3Min(bbox.min, p2);
   bbox.max = AiV3Max(bbox.max, p2);
}

/** 
 * Compute the "union" of two bboxes
 *
 *  \note this name is misleading since it's the bbox of bboxes, not the
 *  union (which can be disjoint)
 */
inline void AiBBoxUnion(AtBBox& bbox, const AtBBox& b1, const AtBBox& b2)
{
   bbox.min = AiV3Min(b1.min, b2.min);
   bbox.max = AiV3Max(b1.max, b2.max);
}

/** 
 * Compute the intersection of two bboxes
 */
inline void AiBBoxIntersection(AtBBox& bbox, const AtBBox& b1, const AtBBox& b2)
{
   bbox.min = AiV3Max(b1.min, b2.min);
   bbox.max = AiV3Min(b1.max, b2.max);
}

/**
 * Check to see if the specified point is inside the bbox
 */
inline bool AiBBoxInside(const AtBBox& bbox, const AtPoint& p)
{
   return ((p.x >= bbox.min.x) && (p.y >= bbox.min.y) && (p.z >= bbox.min.z) &&
           (p.x <= bbox.max.x) && (p.y <= bbox.max.y) && (p.z <= bbox.max.z));
}

/** 
 * Compute the volume of a bbox
 */
inline float AiBBoxVolume(const AtBBox& bbox)
{
   return (bbox.max.x - bbox.min.x) *
          (bbox.max.y - bbox.min.y) *
          (bbox.max.z - bbox.min.z);
}

/** 
 * Returns whether or not the specified box is empty
 */
inline bool AiBBoxIsEmpty(const AtBBox& bbox)
{
   return (bbox.min.x > bbox.max.x) ||
          (bbox.min.y > bbox.max.y) ||
          (bbox.min.z > bbox.max.z);
}

/** 
 * Compute the surface area of a bbox
 */
inline float AiBBoxArea(const AtBBox& bbox)
{
   const AtPoint diag = bbox.max-bbox.min;
   return (diag.x * (diag.y+diag.z) + diag.y*diag.z) * 2;
}

/** 
 * Compute half the surface area of a bbox
 */
inline float AiBBoxHalfArea(const AtBBox& bbox)
{
   const AtPoint diag = bbox.max-bbox.min;
   return diag.x * (diag.y+diag.z) + diag.y*diag.z;
}

/**
 * Compute the center of a bbox
 */
inline void AiBBoxCenter(const AtBBox& bbox, AtPoint& c)
{
   c = (bbox.max + bbox.min) * 0.5f;
}

/**
 * Linear interpolation between two bboxes
 * (k=0 -> bbox=lo, k=1 -> bbox=hi) 
 */
inline void AiBBoxLerp(AtBBox& bbox, float k, const AtBBox& lo, const AtBBox& hi)
{
   AiV3Lerp(bbox.min, k, lo.min, hi.min);
   AiV3Lerp(bbox.max, k, lo.max, hi.max);
}

/**
 * Compute the area (# of pixels) of an integer bbox
 */
inline int AiBBox2Area(const AtBBox2& bbox)
{
   return (bbox.maxx - bbox.minx + 1) * 
          (bbox.maxy - bbox.miny + 1);
}


/** \name Constants
 * \{
 */
AI_API AtBBox AI_BBOX_UNIT;  /**< Unit bounding box */
AI_API AtBBox AI_BBOX_ZERO;  /**< Zero-width bounding box */
/*\}*/

/*\}*/
