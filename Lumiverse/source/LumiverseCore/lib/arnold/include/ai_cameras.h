/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * API for writing camera nodes
 */

#pragma once
#include "ai_nodes.h"
#include "ai_vector.h"

/** \defgroup ai_cameras  Camera Nodes
 * \{
 */

/** Camera ray creation inputs */
typedef struct AtCameraInput {
   float sx, sy;                     /**< screen-space coordinates - within the screen window */
   float dsx, dsy;                   /**< derivatives of the screen-space coordinates with respect to pixel coordinates */
   float lensx, lensy;               /**< lens sampling coordinates in [0,1)^2                */
   float relative_time;              /**< time relative to this camera (in [0,1))             */
} AtCameraInput;

/** Camera ray creation outputs
 *
 *  If the d*d* derivatives are left to their default value of zero,
 *  an accurate numerical estimate will be automatically computed for
 *  them to prevent catastrophic degradation of texture IO performance.
 *  Note that this estimate might not be as good as analytically computed
 *  derivatives but will often be good enough.
 */
typedef struct AtCameraOutput {
   AtPoint  origin;                  /**< ray origin in camera space (required)    */
   AtVector dir;                     /**< ray direction in camera space (required) */
   AtVector dOdx, dOdy;              /**< derivative of the ray origin with respect to the pixel coordinates (optional - defaults to 0)    */
   AtVector dDdx, dDdy;              /**< derivative of the ray direction with respect to the pixel coordinates (optional - defaults to 0) */
   float    weight;                  /**< weight of this ray (used for vignetting) (optional - defaults to 1) */
} AtCameraOutput;

/** Camera node methods structure */
typedef struct AtCameraNodeMethods {
   void (*CreateRay)(const AtNode*, const AtCameraInput*, AtCameraOutput*, int tid);
} AtCameraNodeMethods;

/** Camera node methods exporter */
#define AI_CAMERA_NODE_EXPORT_METHODS(tag)   \
AI_INSTANCE_COMMON_METHODS                   \
camera_create_ray;                           \
static AtCameraNodeMethods ai_cam_mtds = {   \
   CameraCreateRay                           \
};                                           \
static AtNodeMethods ai_node_mtds = {        \
   &ai_common_mtds,                          \
   &ai_cam_mtds                              \
};                                           \
AtNodeMethods* tag = &ai_node_mtds;

/**
 * \name API Methods for Camera Writers
 * \{
 */
AI_API void  AiCameraInitialize(AtNode* node, void* data);
AI_API void  AiCameraUpdate(AtNode* node, bool plane_distance);
AI_API void  AiCameraDestroy(AtNode* node);
AI_API void* AiCameraGetLocalData(const AtNode* node);
/*\}*/

/**
 * \name Node Method Declarations
 * \{
 */

/** Create Camera Ray method declaration */
#define camera_create_ray \
static void CameraCreateRay(const AtNode* node, const AtCameraInput* input, AtCameraOutput* output, int tid)

/* \}*/

/*\}*/
