/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Shader utilities for sampling direct lighting from light sources
 */

#pragma once
#include "ai_nodes.h"
#include "ai_shaderglobals.h"
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_ray.h"

/** \defgroup ai_shader_lights  Light source sampling API
 * \{
 */

/** \name Light Loop Methods
 * \details
 * This is the recommended API for looping over light source samples.
 * \{
 */
AI_API void  AiLightsPrepare(AtShaderGlobals* sg);
AI_API bool  AiLightsGetSample(AtShaderGlobals* sg);
AI_API AtRGB AiLightsGetShadowMatte(AtShaderGlobals* sg);
AI_API void  AiLightsResetCache(AtShaderGlobals* sg);
/*\}*/

/** \name Light Getter Methods
 * \details
 * These getters are to avoid slow AiNodeGet*() calls to access light parameters.
 * Only the most common attributes are exposed for now.
 * \{
 */
AI_API AtRGB AiLightGetColor(const AtNode* node);
AI_API float AiLightGetIntensity(const AtNode* node);
AI_API bool  AiLightGetAffectDiffuse(const AtNode* node);
AI_API bool  AiLightGetAffectSpecular(const AtNode* node);
AI_API float AiLightGetDiffuse(const AtNode* node);
AI_API float AiLightGetSpecular(const AtNode* node);
AI_API float AiLightGetSSS(const AtNode* node);
/*\}*/

/*\}*/
