/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * SSS functions for shaders
 */

#pragma once
#include "ai_color.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_shader_sss Sub-Surface Scattering API
 * \{
 */

AI_API AtColor AiBSSRDFCubic(const AtShaderGlobals* sg, const float* radius, const AtColor* weight, unsigned int num = 1);
AI_API AtColor AiBSSRDFGaussian(const AtShaderGlobals* sg, const float* variance, const AtColor* weight, unsigned int num = 1);
AI_API AtColor AiSSSTraceSingleScatter(AtShaderGlobals* sg, AtColor Rd, AtColor mfp, float g, float eta);
AI_API bool    AiShaderGlobalsFromSSS(const AtShaderGlobals* sg);

/** \name Deprecated
 *
 * The pointcloud iterator interface will be removed in a future release.
 *
 * \{
 */

/*
 * Point cloud sample distributions
 */
#define AI_POINTCLOUD_BLUE_NOISE          0x00
#define AI_POINTCLOUD_TRIANGLE_MIDPOINT   0x01
#define AI_POINTCLOUD_POLYGON_MIDPOINT    0x02

// Iterator interface
struct AtPointCloudSample
{
   AtPoint  pref_position;
   AtPoint  world_position;
   AtVector normal;
   AtUInt32 face;
   AtPoint2 uv;
   float    area;
};
struct AtPointCloudIterator;
AI_API AtPointCloudIterator* AiPointCloudIteratorCreate  (const AtShaderGlobals* sg, int distribution, float spacing = 0.1f);
AI_API void                  AiPointCloudIteratorDestroy (AtPointCloudIterator* iter);
AI_API AtPointCloudSample    AiPointCloudIteratorGetNext (AtPointCloudIterator* iter);
AI_API bool                  AiPointCloudIteratorFinished(const AtPointCloudIterator* iter);
AI_API AtUInt32              AiPointCloudGetSampleCount  (const AtPointCloudIterator* iter);

/*\}*/

/*\}*/
