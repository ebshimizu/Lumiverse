/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Enum data type and utility functions
 */

#pragma once
#include "ai_api.h"

/** \defgroup ai_enum  AtEnum API
 * \{
 */

/** String-based enumerated data type
 *
 * \ref AtNode parameters of type \c AI_TYPE_ENUM are stored as \ref AtEnum.
 * The \ref AtEnum data type is just an array of strings that ends with
 * a NULL string (not an empty string, but a NULL pointer).
 */
typedef const char** AtEnum;

AI_API int         AiEnumGetValue(AtEnum enum_type, const char* string);
AI_API const char* AiEnumGetString(AtEnum enum_type, int index);

/*\}*/
