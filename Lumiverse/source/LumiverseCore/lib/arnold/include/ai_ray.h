/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Ray struct and various trace functions
 */

#pragma once
#include "ai_shader_sample.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"

// forward declaration
struct AtShaderGlobals;
struct AtBucket;

/** \defgroup ai_ray Ray Tracing API
 * \{
 */

/** \name Ray Types
 * \{
 */
#define AI_RAY_UNDEFINED   0x00         /**< undefined type                                  */
#define AI_RAY_CAMERA      0x01         /**< ray originating at the camera                   */
#define AI_RAY_SHADOW      0x02         /**< shadow ray towards a light source               */
#define AI_RAY_REFLECTED   0x04         /**< mirror reflection ray                           */
#define AI_RAY_REFRACTED   0x08         /**< mirror refraction ray                           */
#define AI_RAY_DIFFUSE     0x20         /**< indirect diffuse (also known as diffuse GI) ray */
#define AI_RAY_GLOSSY      0x40         /**< glossy/blurred reflection ray                   */
#define AI_RAY_ALL         0xFF         /**< mask for all ray types                          */
#define AI_RAY_GENERIC     AI_RAY_ALL   /**< mask for all ray types                          */
/*\}*/

/** Ray data structure */
typedef struct AtRay {
   AtUInt16     type;                   /**< Type of ray (\c AI_RAY_CAMERA, etc)             */
   AtByte       tid;                    /**< Thread ID                                       */
   AtByte       level;                  /**< Recursion level (0 for camera rays)             */
   AtByte       diff_bounces;           /**< Number of diffuse bounces so far                */
   AtByte       gloss_bounces;          /**< Number of glossy bounces so far                 */
   AtByte       refl_bounces;           /**< Number of reflection bounces so far             */
   AtByte       refr_bounces;           /**< Number of refraction bounces so far             */
   int          x;                      /**< Raster-space X coordinate                       */
   int          y;                      /**< Raster-space Y coordinate                       */
   float        sx;                     /**< Image-space X coordinate, in [-1,1)             */
   float        sy;                     /**< Image-space Y coordinate, in [-1,1)             */
   float        px;                     /**< Subpixel X coordinate in [0,1)                  */
   float        py;                     /**< Subpixel Y coordinate in [0,1)                  */
   AtPoint      origin;                 /**< Ray origin                                      */
   AtVector     dir;                    /**< Unit ray direction                              */
   double       mindist;                /**< Minimum useful distance from the origin         */
   double       maxdist;                /**< Maximum useful distance from the origin (volatile while ray is traced) */
   const AtShaderGlobals* psg;          /**< Parent shader globals (last shaded)             */
   void*        light_source;           /**< For shadow rays only                            */
   AtBucket*    bucket;                 /**< Parent bucket                                   */
   float        weight;                 /**< Ray weight, 1.0 for clean camera rays           */
   float        time;                   /**< Time at which the ray was created, in [0,1)     */
   AtVector     dOdx;                   /**< Partial derivative of ray origin wrt image-space X coordinate    */
   AtVector     dOdy;                   /**< Partial derivative of ray origin wrt image-space Y coordinate    */
   AtVector     dDdx;                   /**< Partial derivative of ray direction wrt image-space X coordinate */
   AtVector     dDdy;                   /**< Partial derivative of ray direction wrt image-space Y coordinate */
   const char*  traceset;               /**< Trace-set for this ray                          */
   bool         inclusive_traceset;     /**< Is the trace-set inclusive or exclusive?        */
   AtUInt16     sindex;                 /**< Sub-pixel sample index when supersampling       */
} AtRay;

AI_API void AiMakeRay(AtRay* ray, AtUInt32 type, const AtPoint* origin, const AtVector* dir, double maxdist, const AtShaderGlobals* sg);
AI_API void AiReflectRay(AtRay* ray, const AtVector* normal, const AtShaderGlobals* sg);
AI_API bool AiRefractRay(AtRay* ray, const AtVector* normal, float n1, float n2, const AtShaderGlobals* sg);
AI_API bool AiTrace(const AtRay* ray, AtScrSample* sample);
AI_API void AiTraceBackground(const AtRay* ray, AtScrSample* sample);
AI_API bool AiTraceProbe(const AtRay* ray, AtShaderGlobals* sgout);

/*\}*/
