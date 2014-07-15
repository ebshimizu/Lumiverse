/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * DLL export prefix for API functions (necessary for multi-platform development)
 */

#pragma once

#ifdef __cplusplus
#define AI_EXTERN_C extern "C"
#else
#define AI_EXTERN_C extern
#endif

/*
 * Public functions need a special declaration in Win32
 */
#ifdef _WIN32
#ifdef _ARNOLDDLL
#define AI_API AI_EXTERN_C __declspec(dllexport)
#else
#define AI_API AI_EXTERN_C __declspec(dllimport)
#endif
#else
#define AI_API AI_EXTERN_C __attribute__(( visibility("default") ))
#endif
