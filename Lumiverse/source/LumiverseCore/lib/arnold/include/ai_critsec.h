/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Critical section type that helps when writing multi-platform threaded code
 */

#pragma once
#include "ai_api.h"

/** \defgroup ai_critsec Critical Section API
 * \{
 */

/**
 * Opaque data type for a critical section
 */
typedef void* AtCritSec;

AI_API void AiCritSecInit(AtCritSec* cs);
AI_API void AiCritSecInitRecursive(AtCritSec* cs);
AI_API void AiCritSecClose(AtCritSec* cs);
AI_API void AiCritSecEnter(AtCritSec* cs);
AI_API void AiCritSecLeave(AtCritSec* cs);

/*\}*/
