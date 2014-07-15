/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**  
 * \file
 * Generic array data type and methods
 */

#pragma once
#include "ai_color.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"
#include "ai_api.h"

/** \defgroup ai_array AtArray API
 *
 *  \details
 *  The AtArray object encapsulates an array of any of the Arnold built-in
 *  data types like \c AI_TYPE_BYTE, \c AI_TYPE_FLOAT, \c AI_TYPE_STRING, etc.
 *  Its API has easy-to-use accessor functions for reading and writing
 *  elements, and there are a number of functions for manipulating arrays 
 *  (such as copying them).
 *  An AtArray is specified by the element data type, the number of motion keys
 *  in the array, and the number of elements per motion key. The data is
 *  grouped together by motion keys.
 *
 * \{
 */       

/** Generic array data type */
typedef struct AtArray {
   void*     data;            /**< raw data                           */
   AtUInt32  nelements;       /**< number of elements per motion key  */
   AtByte    nkeys;           /**< number of motion keys              */
   AtByte    type;            /**< type of array                      */
} AtArray;

AI_API AtArray*  AiArray(AtUInt32 nelements, AtByte nkeys, AtByte type, ...);
AI_API AtArray*  AiArrayAllocate(AtUInt32 nelements, AtByte nkeys, AtByte type);
AI_API void      AiArrayDestroy(AtArray* array);
AI_API AtArray*  AiArrayConvert(AtUInt32 nelements, AtByte nkeys, AtByte type, const void* data);
AI_API AtArray*  AiArrayModify(AtArray* array, AtUInt32 nelements, AtByte nkeys, AtByte type, ...);
AI_API AtArray*  AiArrayCopy(const AtArray* array);
AI_API bool      AiArraySetKey(AtArray* array, AtByte key, const void* data);
AI_API AtPoint   AiArrayInterpolatePnt(const AtArray* array, float time, AtUInt32 idx);
AI_API AtVector  AiArrayInterpolateVec(const AtArray* array, float time, AtUInt32 idx);
AI_API AtColor   AiArrayInterpolateRGB(const AtArray* array, float time, AtUInt32 idx);
AI_API AtRGBA    AiArrayInterpolateRGBA(const AtArray* array, float time, AtUInt32 idx);
AI_API float     AiArrayInterpolateFlt(const AtArray* array, float time, AtUInt32 idx);
AI_API void      AiArrayInterpolateMtx(const AtArray* array, float time, AtUInt32 idx, AtMatrix result);

/** \name AtArray Getters
 *
 * \details
 * The following getter functions return the i'th element in an array of the
 * given type.
 * In case of out-of-bounds access, an error message is generated with the source
 * code location of the offending call.
 *
 * Note that, for ease of use, these are actually macros, which are shorter to
 * type than the full functions. The actual functions can be found in ai_array.h.
 *
 * \{
 */
#define AiArrayGetBool(a,i)      AiArrayGetBoolFunc (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetByte(a,i)      AiArrayGetByteFunc (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetInt(a,i)       AiArrayGetIntFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetUInt(a,i)      AiArrayGetUIntFunc (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetFlt(a,i)       AiArrayGetFltFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetRGB(a,i)       AiArrayGetRGBFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetRGBA(a,i)      AiArrayGetRGBAFunc (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetPnt(a,i)       AiArrayGetPntFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetPnt2(a,i)      AiArrayGetPnt2Func (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetVec(a,i)       AiArrayGetVecFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetMtx(a,i,out)   AiArrayGetMtxFunc  (a,i,out,__AI_FILE__,__AI_LINE__)
#define AiArrayGetStr(a,i)       AiArrayGetStrFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetPtr(a,i)       AiArrayGetPtrFunc  (a,i,__AI_FILE__,__AI_LINE__)
#define AiArrayGetArray(a,i)     AiArrayGetArrayFunc(a,i,__AI_FILE__,__AI_LINE__)
/*\}*/

/** \name AtArray Setters
 *
 * \details
 * The following functions write an element of a given type into the i'th position
 * in an array. If the write was succesful, these functions will return true, otherwise
 * a detailed error message will be logged and false will be returned.
 *
 * \{
 */
#define AiArraySetBool(a,i,val)  AiArraySetBoolFunc (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetByte(a,i,val)  AiArraySetByteFunc (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetInt(a, i,val)  AiArraySetIntFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetUInt(a,i,val)  AiArraySetUIntFunc (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetFlt(a,i,val)   AiArraySetFltFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetRGB(a,i,val)   AiArraySetRGBFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetRGBA(a,i,val)  AiArraySetRGBAFunc (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetPnt(a,i,val)   AiArraySetPntFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetPnt2(a,i,val)  AiArraySetPnt2Func (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetVec(a,i,val)   AiArraySetVecFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetMtx(a,i,val)   AiArraySetMtxFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetStr(a,i,val)   AiArraySetStrFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetPtr(a,i,val)   AiArraySetPtrFunc  (a,i,val,__AI_FILE__,__AI_LINE__)
#define AiArraySetArray(a,i,val) AiArraySetArrayFunc(a,i,val,__AI_FILE__,__AI_LINE__)
/*\}*/

/*\}*/

AI_API bool         AiArrayGetBoolFunc (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtByte       AiArrayGetByteFunc (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API int          AiArrayGetIntFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtUInt32     AiArrayGetUIntFunc (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API float        AiArrayGetFltFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtRGB        AiArrayGetRGBFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtRGBA       AiArrayGetRGBAFunc (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtPoint      AiArrayGetPntFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtPoint2     AiArrayGetPnt2Func (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtVector     AiArrayGetVecFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API void         AiArrayGetMtxFunc  (const AtArray* a, AtUInt32 i, AtMatrix out, const char*, int line);
AI_API const char*  AiArrayGetStrFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API void*        AiArrayGetPtrFunc  (const AtArray* a, AtUInt32 i, const char*, int line);
AI_API AtArray*     AiArrayGetArrayFunc(const AtArray* a, AtUInt32 i, const char*, int line);

AI_API bool         AiArraySetBoolFunc (AtArray* a, AtUInt32 i, bool        val, const char*, int line);
AI_API bool         AiArraySetByteFunc (AtArray* a, AtUInt32 i, AtByte      val, const char*, int line);
AI_API bool         AiArraySetIntFunc  (AtArray* a, AtUInt32 i, int         val, const char*, int line);
AI_API bool         AiArraySetUIntFunc (AtArray* a, AtUInt32 i, AtUInt32    val, const char*, int line);
AI_API bool         AiArraySetFltFunc  (AtArray* a, AtUInt32 i, float       val, const char*, int line);
AI_API bool         AiArraySetRGBFunc  (AtArray* a, AtUInt32 i, AtRGB       val, const char*, int line);
AI_API bool         AiArraySetRGBAFunc (AtArray* a, AtUInt32 i, AtRGBA      val, const char*, int line);
AI_API bool         AiArraySetPntFunc  (AtArray* a, AtUInt32 i, AtPoint     val, const char*, int line);
AI_API bool         AiArraySetPnt2Func (AtArray* a, AtUInt32 i, AtPoint2    val, const char*, int line);
AI_API bool         AiArraySetVecFunc  (AtArray* a, AtUInt32 i, AtVector    val, const char*, int line);
AI_API bool         AiArraySetMtxFunc  (AtArray* a, AtUInt32 i, AtMatrix    val, const char*, int line);
AI_API bool         AiArraySetStrFunc  (AtArray* a, AtUInt32 i, const char* val, const char*, int line);
AI_API bool         AiArraySetPtrFunc  (AtArray* a, AtUInt32 i, void*       val, const char*, int line);
AI_API bool         AiArraySetArrayFunc(AtArray* a, AtUInt32 i, AtArray*    val, const char*, int line);
