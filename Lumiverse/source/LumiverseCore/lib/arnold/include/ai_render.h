/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * API for managing rendering sessions
 */

#pragma once
#include "ai_api.h"

/** \defgroup ai_render Rendering API
 * \{
 */

/** \name Rendering Modes
 * \{
 */
#define AI_RENDER_MODE_CAMERA              0x00  /**< Render from a camera */
#define AI_RENDER_MODE_FREE                0x01  /**< Process arbitrary ray-tracing requests, acting as a "ray server" */
/*\}*/

/** \name Error Codes
 * \{
 */
#define AI_SUCCESS                         0x00   /**< no error                    */
#define AI_ABORT                           0x01   /**< render aborted              */
#define AI_ERROR_WRONG_OUTPUT              0x02   /**< can't open output file      */
#define AI_ERROR_NO_CAMERA                 0x03   /**< camera not defined          */
#define AI_ERROR_BAD_CAMERA                0x04   /**< bad camera data             */
#define AI_ERROR_VALIDATION                0x05   /**< usage not validated         */
#define AI_ERROR_RENDER_REGION             0x06   /**< invalid render region       */
#define AI_ERROR_OUTPUT_EXISTS             0x07   /**< output file already exists  */
#define AI_ERROR_OPENING_FILE              0x08   /**< can't open file             */
#define AI_INTERRUPT                       0x09   /**< render interrupted by user  */
#define AI_ERROR_UNRENDERABLE_SCENEGRAPH   0x0A   /**< unrenderable scenegraph     */
#define AI_ERROR_NO_OUTPUTS                0x0B   /**< no rendering outputs        */
#define AI_ERROR                             -1   /**< generic error               */
/*\}*/

AI_API void AiBegin();
AI_API void AiEnd();
AI_API int  AiRender(int mode = AI_RENDER_MODE_CAMERA);
AI_API void AiRenderAbort();
AI_API void AiRenderInterrupt();
AI_API bool AiRendering();

/*\}*/
