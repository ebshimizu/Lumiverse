/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Struct returned by AiTrace(), called "screen sample" for legacy reasons
 */

#pragma once
#include "ai_color.h"
#include "ai_vector.h"

// forward declarations
struct AtNode;

/** \defgroup ai_shader_sample  Screen Sample
 * \{
 */

/** Used to hold the result of AiTrace() calls */
typedef struct AtScrSample {
   AtColor       color;    /**< color                  */
   AtColor       opacity;  /**< opacity                */
   float         alpha;    /**< alpha                  */
   AtPoint       point;    /**< 3D hit point           */
   AtUInt32      face;     /**< triangle ID of hit     */
   const AtNode* obj;      /**< pointer to object hit  */
   double        z;        /**< hit distance (Z depth) */
} AtScrSample;

/*\}*/
