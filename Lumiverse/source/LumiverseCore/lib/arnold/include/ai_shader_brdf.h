/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Various BRDF functions available to shader writers
 */

#pragma once
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_shaderglobals.h"

/** \defgroup ai_shader_brdf BRDF API
 * \{
 */

/** \name BRDF Evaluation Methods
 *
 * \details
 * We provide an interface for users to define a BRDF and its
 * associated methods. All the functions receive an arbitrary
 * void pointer where the programmer can store needed parameters.
 * An "instance" of a BRDF is meant to be located at a certain
 * point and to a certain viewing direction. The programmer must
 * take care of this and store it in the arbitrary data pointer.
 * \{
 */
/** Returns a direction according to some distribution based on the reflectance of the BRDF. */
typedef AtVector(*AtBRDFEvalSampleFunc)(const void* brdf_data, float rx, float ry);
/** Returns the actual reflectance from a given direction to the viewer's direction. */
typedef AtColor (*AtBRDFEvalBrdfFunc)(const void* brdf_data, const AtVector* indir); 
/** Returns the probability density for the above sample function at a certain incoming direction. */
typedef float   (*AtBRDFEvalPdfFunc)(const void* brdf_data, const AtVector* indir);
/* \}*/
                                                      
/** \name BRDF Integration
 * \{
 */
AI_API AtColor   AiEvaluateLightSample(AtShaderGlobals* sg, const void* brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf);
AI_API AtColor   AiBRDFIntegrate(AtShaderGlobals *sg, const void *brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf, AtUInt16 ray_type);
/* \}*/

/** \name Pre-defined BRDFs
 * \{
 */
AI_API AtColor   AiOrenNayarMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API float     AiOrenNayarMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AtVector  AiOrenNayarMISSample(const void* brdf_data, float randx, float randy);
AI_API void*     AiOrenNayarMISCreateData(const AtShaderGlobals* sg, float r);

AI_API AtColor   AiCookTorranceMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API float     AiCookTorranceMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AtVector  AiCookTorranceMISSample(const void* brdf_data, float randx, float randy);
AI_API void*     AiCookTorranceMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);

AI_API AtColor   AiWardDuerMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API float     AiWardDuerMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AtVector  AiWardDuerMISSample(const void* brdf_data, float randx, float randy);
AI_API void*     AiWardDuerMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);

AI_API AtColor   AiAshikhminShirleyMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API float     AiAshikhminShirleyMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AtVector  AiAshikhminShirleyMISSample(const void* brdf_data, float randx, float randy);
AI_API void*     AiAshikhminShirleyMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);

AI_API AtColor   AiStretchedPhongMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API float     AiStretchedPhongMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AtVector  AiStretchedPhongMISSample(const void* brdf_data, float randx, float randy);
AI_API void*     AiStretchedPhongMISCreateData(const AtShaderGlobals* sg, float pexp);
/* \}*/

/** \name Deprecated BRDF Evaluation API
 * \{
 */
AI_API float AiOrenNayarBRDF       (const AtVector* L, const AtVector* V, const AtVector* N, float r);
AI_API float AiLommelSeeligerBRDF  (const AtVector* L, const AtVector* V, const AtVector* N);
AI_API float AiCookTorranceBRDF    (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API float AiWardBRDF            (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API float AiWardDuerBRDF        (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API float AiAshikhminShirleyBRDF(const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float nx, float ny);
AI_API float AiStretchedPhongBRDF  (const AtVector* L, const AtVector* V, const AtVector* N, float n, bool retro = false);
/* \}*/

/** \name Deprecated BRDF Integration API
 * \{
 */
AI_API AtColor AiOrenNayarIntegrate(const AtVector* N, AtShaderGlobals* sg, float r);
AI_API AtColor AiLommelSeeligerIntegrate(const AtVector* N, AtShaderGlobals* sg);
AI_API AtColor AiCookTorranceIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiWardIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiWardDuerIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiAshikhminShirleyIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float nx, float ny);
AI_API AtColor AiMicrofacetBTDFIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry, float eta_i, float eta_o, AtColor transmittance);
AI_API AtColor AiStretchedPhongIntegrate(const AtVector* N, AtShaderGlobals* sg, float exp);
/* \}*/

/*\}*/
