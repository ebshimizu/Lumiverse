/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * A generic, two-dimensional sampler that is sometimes useful when writing shaders
 */

#pragma once
#include "ai_api.h"

// forward declarations (defined elsewhere)
struct AtShaderGlobals;

/** \defgroup ai_sampler Sampler API
 * \{
 */

/**
 * Opaque data type for a sampler
 */
typedef struct AtSampler AtSampler;

/**
 * Opaque data type for a sampler iterator
 */
typedef struct AtSamplerIterator AtSamplerIterator;

AI_API AtSampler*         AiSampler(int nsamples, int ndim);
AI_API AtSampler*         AiSamplerSeeded(int seed, int nsamples, int ndim);
AI_API AtSamplerIterator* AiSamplerIterator(const AtSampler* sampler, const AtShaderGlobals* sg);
AI_API bool               AiSamplerGetSample(AtSamplerIterator* iterator, float* sample);
AI_API int                AiSamplerGetSampleCount(const AtSamplerIterator* iterator);
AI_API float              AiSamplerGetSampleInvCount(const AtSamplerIterator* iterator);
AI_API void               AiSamplerDestroy(AtSampler* sampler);

/*\}*/
