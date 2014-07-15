/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Memory allocation routines
 */

#pragma once
#include "ai_msg.h"
#include "ai_api.h"

/** \defgroup ai_allocate Memory Allocation API
 * \{                                                                              
 */       

#define AiMalloc(_size)         AiMalloc_func(_size, __AI_FILE__, __AI_LINE__, __AI_FUNCTION__)
#define AiRealloc(_addr, _size) AiRealloc_func(_addr, _size, __AI_FILE__, __AI_LINE__, __AI_FUNCTION__)
#define AiFree(_addr)           AiFree_func(_addr, __AI_FILE__, __AI_LINE__, __AI_FUNCTION__)

AI_API void* AiMalloc_func(size_t size, const char* file, int line, const char* func);
AI_API void* AiRealloc_func(void* addr, size_t size, const char* file, int line,const char* func);
AI_API void  AiFree_func(void* addr, const char* file, int line, const char* func);

/*\}*/
