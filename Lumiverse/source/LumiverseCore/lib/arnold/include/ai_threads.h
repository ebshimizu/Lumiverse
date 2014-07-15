/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Multi-platform API for the creation and management of CPU threads
 */

#pragma once
#include "ai_api.h"

/** \defgroup ai_threads Thread-management API
 * \{
 */

AI_API void* AiThreadCreate(unsigned int (*fn)(void*), void* data, int priority);
AI_API void  AiThreadClose(void* thread);
AI_API void  AiThreadWait(void* thread);
AI_API void* AiThreadSelf();

#define AI_MAX_THREADS 64  /**< maximum number of threads */

/** \name Thread Priorities
 * \{
 */
#define AI_PRIORITY_LOWEST   0x00  /**< even lower priority than \c AI_PRIORITY_LOW */
#define AI_PRIORITY_LOW      0x01  /**< low thread priority */
#define AI_PRIORITY_NORMAL   0x02  /**< normal thread priority */
#define AI_PRIORITY_HIGH     0x03  /**< high thread priority */
/*\}*/

/*\}*/
