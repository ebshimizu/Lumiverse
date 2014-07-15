/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Basic utility functions for output drivers
 */

#pragma once
#include "ai_types.h"
#include "ai_api.h"

/** \addtogroup ai_drivers
 * \{
 */

/** \name Quantization and Dithering
 * \{
 */
AI_API AtUInt8  AiQuantize8bit(int x, int y, int i, float value, bool dither);
AI_API AtUInt16 AiQuantize16bit(int x, int y, int i, float value, bool dither);
/*\}*/

/*\}*/
