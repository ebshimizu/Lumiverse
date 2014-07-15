/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Version number information and checking of backwards-compatibility
 */

#pragma once
#include "ai_api.h"

#define ARNOLD_XSTR(s) ARNOLD_STR(s)
#define ARNOLD_STR(s)  #s

#define AI_MAXSIZE_VERSION 32
 
/*
 * AI_VERSION_FIX *MUST* be a string!
 * (something like "0.test" is acceptable, too!)
 */
#define AI_VERSION_ARCH_NUM    4
#define AI_VERSION_MAJOR_NUM   1
#define AI_VERSION_MINOR_NUM   3
#define AI_VERSION_FIX         "3"

#define AI_VERSION_ARCH   ARNOLD_XSTR(AI_VERSION_ARCH_NUM)
#define AI_VERSION_MAJOR  ARNOLD_XSTR(AI_VERSION_MAJOR_NUM)
#define AI_VERSION_MINOR  ARNOLD_XSTR(AI_VERSION_MINOR_NUM)

/** \defgroup ai_version Versioning API
 * \{
 */

/** Full Arnold version as string */
#define AI_VERSION   AI_VERSION_ARCH "." AI_VERSION_MAJOR "." AI_VERSION_MINOR "." AI_VERSION_FIX

AI_API const char* AiGetVersion(char* arch, char* major, char* minor, char* fix);
AI_API const char* AiGetVersionInfo();
AI_API const char* AiGetCompileOptions();
AI_API bool        AiCheckAPIVersion(const char* arch, const char* major, const char* minor);

AI_API void        AiSetAppString(const char* appstr);

/*\}*/
