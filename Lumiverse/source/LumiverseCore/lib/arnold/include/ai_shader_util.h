/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Various utility functions for shader writers
 */

#pragma once
#include "ai_bbox.h"
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_api.h"
#include "ai_shaderglobals.h"

// forward declaration
struct AtSampler;
struct AtNode;

/** \defgroup ai_shader_util  Shader Utility API
 * \{
 */

/** \name Coordinate Transformations
 * \{
 */
#define AI_WORLD_TO_OBJECT  1 /**< World-to-object coordinate transformation */
#define AI_OBJECT_TO_WORLD  2 /**< Object-to-world coordinate transformation */
/* \}*/

/** \name Wireframe Types
 * \{
 */
#define AI_WIREFRAME_TRIANGLES            0   /**< Tessellated triangles */
#define AI_WIREFRAME_POLYGONS             1   /**< Tessellated polygons */
#define AI_WIREFRAME_PATCHES              2   /**< Patches before subdivision */
/* \}*/

/** \name Lighting Functions
 * \{
 */
AI_API AtColor   AiOcclusion(const AtVector* N, const AtVector* Ng, AtShaderGlobals* sg, float mint, float maxt, float spread, float falloff, const AtSampler* sampler, AtVector* Nbent);
AI_API AtColor   AiDirectDiffuse(const AtVector* N, AtShaderGlobals* sg);
AI_API AtColor   AiHairDirectDiffuseCache(const struct AtShaderGlobals* sg);
AI_API AtColor   AiIndirectDiffuse(const AtVector* N, AtShaderGlobals* sg);

AI_API void      AiReflect(const AtVector* I, const AtVector* N, AtVector* R);
AI_API void      AiReflectSafe(const AtVector* I, const AtVector* N, const AtVector* Ng, AtVector* R);
AI_API bool      AiRefract(const AtVector* I, const AtVector* N, AtVector* T, float n1, float n2);
AI_API float     AiFresnelWeight(AtVector N, AtVector Rd, float Krn);
AI_API void      AiFresnelWeightRGB(const AtVector* N, const AtVector* Rd, const AtColor* Krn, AtColor* Kf);
AI_API void      AiFaceForward(AtVector* N, AtVector I);
AI_API void      AiBuildLocalFrameShirley(AtVector* u, AtVector* v, const AtVector* N);
AI_API void      AiBuildLocalFramePolar(AtVector* u, AtVector* v, const AtVector* N);
/* \}*/

/** \name Utility Helper Types
 * \{
 */

/** This function pointer points to float-based bump-mapping function for
 * use by AiShaderGlobalsEvaluateBump().
 *
 * This function would return the displacement height for the given shading context.
 * This function would be called three times (on three different shading contexts).
 * The returned displacements would be used to generate a triangle from which a
 * normal is calculated.
 *
 * \param sg    the current shading context
 * \param data  user-defined data pointer
 * \return bump/displacement height for the current shading context
 */
typedef float (*AtFloatBumpEvaluator)(AtShaderGlobals* sg, void* data);
/* \}*/

/** \name Utility Functions
 * \{
 */
AI_API bool      AiShaderGlobalsGetTriangle(const AtShaderGlobals* sg, int key, AtPoint p[3]);
AI_API bool      AiShaderGlobalsGetVertexNormals(const AtShaderGlobals* sg, int key, AtVector n[3]);
AI_API bool      AiShaderGlobalsGetVertexUVs(const AtShaderGlobals* sg, AtPoint2 uv[3]);
AI_API AtUInt32  AiShaderGlobalsGetPolygon(const AtShaderGlobals* sg, int key, AtPoint* p);
AI_API void      AiShaderGlobalsGetPositionAtTime(const AtShaderGlobals* sg, float time, AtPoint* P, AtVector* N, AtVector* Ng, AtVector* Ns);
AI_API AtPoint2  AiShaderGlobalsGetPixelMotionVector(const AtShaderGlobals* sg, float time0, float time1);
AI_API AtBBox    AiShaderGlobalsGetBBoxLocal(const AtShaderGlobals* sg);
AI_API AtBBox    AiShaderGlobalsGetBBoxWorld(const AtShaderGlobals* sg);
AI_API AtVector  AiShaderGlobalsTransformNormal(const AtShaderGlobals* sg, AtVector N, int space);
AI_API AtPoint   AiShaderGlobalsTransformPoint(const AtShaderGlobals* sg, AtPoint P, int space);
AI_API AtVector  AiShaderGlobalsTransformVector(const AtShaderGlobals* sg, AtVector V, int space);
AI_API void      AiShaderGlobalsSetTraceSet(AtShaderGlobals* sg, const char* set, bool inclusive);
AI_API void      AiShaderGlobalsUnsetTraceSet(AtShaderGlobals* sg);
AI_API void*     AiShaderGlobalsQuickAlloc(const AtShaderGlobals* sg, AtUInt32 size);
AI_API AtVector  AiShaderGlobalsEvaluateBump(AtShaderGlobals* sg, AtFloatBumpEvaluator bump_func, void* data);
AI_API bool      AiShaderGlobalsApplyOpacity(AtShaderGlobals* sg, AtRGB opacity);
AI_API float     AiShaderGlobalsEdgeLength(const AtShaderGlobals* sg);
AI_API float     AiWireframe(const AtShaderGlobals* sg, float line_width, bool raster_space, int edge_type);

/**
 * Make the surface normal face the viewer
 *
 * Reverses vector sg->N if needed, in order to make it face forward
 * with respect to viewing vector sg->Rd (not necessarily a camera ray).
 *
 * \param sg      the current shading context
 * \param[out] n  viewer-facing normal vector
 */
inline void AiFaceViewer(const AtShaderGlobals* sg, AtVector& n)
{
   n = (AiV3Dot(sg->Ng, sg->Rd) > 0.0f) ? -sg->N : sg->N;
}

/* \}*/

/** \name Camera Information
 * \{
 */
AI_API float AiCameraGetShutterStart();
AI_API float AiCameraGetShutterEnd();
AI_API void  AiCameraToWorldMatrix(const AtNode* node, float time, AtMatrix out);
AI_API void  AiWorldToCameraMatrix(const AtNode* node, float time, AtMatrix out);
AI_API void  AiWorldToScreenMatrix(const AtNode* node, float time, AtMatrix out);
/* \}*/

/** \name Environment Mappings
 * \{
 */
AI_API void AiMappingMirroredBall(const AtVector* dir, float* u, float* v);
AI_API void AiMappingAngularMap  (const AtVector* dir, float* u, float* v);
AI_API void AiMappingLatLong     (const AtVector* dir, float* u, float* v);
AI_API void AiMappingCubicMap    (const AtVector* dir, float* u, float* v);
/* \}*/

/** \name Derivatives for Environment Mappings
 * \{
 */
AI_API void AiMappingMirroredBallDerivs(const AtVector* dir, const AtVector* dDdx, const AtVector* dDdy, float* u, float* v, float* dudx, float* dudy, float* dvdx, float* dvdy);
AI_API void AiMappingAngularMapDerivs  (const AtVector* dir, const AtVector* dDdx, const AtVector* dDdy, float* u, float* v, float* dudx, float* dudy, float* dvdx, float* dvdy);
AI_API void AiMappingLatLongDerivs     (const AtVector* dir, const AtVector* dDdx, const AtVector* dDdy, float* u, float* v, float* dudx, float* dudy, float* dvdx, float* dvdy);
AI_API void AiMappingCubicMapDerivs    (const AtVector* dir, const AtVector* dDdx, const AtVector* dDdy, float* u, float* v, float* dudx, float* dudy, float* dvdx, float* dvdy);
/* \}*/

/*\}*/
