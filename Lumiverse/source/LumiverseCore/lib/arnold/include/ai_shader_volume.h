/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Volume functions for shaders
 */
 
#pragma once
#include "ai_color.h"
#include "ai_api.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_shader_volume Volume Rendering API
 * \{
 */

AI_API void AiShaderGlobalsSetVolumeAbsorption(const AtShaderGlobals *sg, const AtRGB &absorption);
AI_API void AiShaderGlobalsSetVolumeAttenuation(const AtShaderGlobals *sg, const AtRGB &attenuation);
AI_API void AiShaderGlobalsSetVolumeEmission(const AtShaderGlobals *sg, const AtRGB &emission);
AI_API void AiShaderGlobalsSetVolumeScattering(const AtShaderGlobals *sg, const AtRGB &scattering, float g = 0.f);

/*\}*/
