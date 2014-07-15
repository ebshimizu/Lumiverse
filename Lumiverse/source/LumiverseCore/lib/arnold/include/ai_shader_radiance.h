/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Radiance and irradiance lookup functions
 */

#pragma once
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_types.h"

// forward declarations
struct AtNode;

/** \defgroup ai_shader_radiance Irradiance / Radiance API
 * \{
 */       

// These two functions are deprecated, as they only work for a single thread
AI_API AtColor AiIrradiance(const AtPoint* p, const AtVector* n, AtByte tid, AtUInt32 pid);
AI_API AtColor AiRadiance(const AtPoint* p, const AtVector* dir, const AtVector* n, AtNode* obj, AtUInt32 face, float u, float v, AtNode* shader, AtByte tid, AtUInt32 pid);

AI_API AtRGB AiSSSEvaluateIrradiance(const AtShaderGlobals* sg, float u, float v, AtUInt32 face, const AtPoint* p, AtUInt32 index);

/*\}*/
