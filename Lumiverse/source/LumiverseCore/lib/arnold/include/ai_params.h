/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Node parameters
 */

#pragma once
#include "ai_array.h"
#include "ai_color.h"
#include "ai_enum.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"
#include "ai_api.h"

// forward declaration
struct AtList;
struct AtNode;

/** \defgroup ai_params AtParamEntry API
 *  
 *  This API provides a mechanism for clients to query information about a node's
 *  parameters. Methods like AiNodeEntryGetParameter() return a AtParamEntry object
 *  that can be used to query information about a given parameter.
 *  For a discussion of Arnold's object-oriented system of pluggable nodes, please
 *  refer to \ref ai_nodes and \ref ai_node_entry.
 * \{
 */ 

/** \name Parameter Types
 *
 *  See AtParamValue for the actual primitive data types that these macros refer to.
 * \{
 */
#define AI_TYPE_BYTE          0x00  /**< Byte (an 8-bit sized unsigned integer) */
#define AI_TYPE_INT           0x01  /**< Integer */
#define AI_TYPE_UINT          0x02  /**< Unsigned integer */
#define AI_TYPE_BOOLEAN       0x03  /**< Boolean (either true or false) */
#define AI_TYPE_FLOAT         0x04  /**< Single-precision floating point number */
#define AI_TYPE_RGB           0x05  /**< RGB struct */
#define AI_TYPE_RGBA          0x06  /**< RGBA struct */
#define AI_TYPE_VECTOR        0x07  /**< XYZ vector */
#define AI_TYPE_POINT         0x08  /**< XYZ point */
#define AI_TYPE_POINT2        0x09  /**< XY point */
#define AI_TYPE_STRING        0x0A  /**< C-style character string */
#define AI_TYPE_POINTER       0x0B  /**< Arbitrary pointer */
#define AI_TYPE_NODE          0x0C  /**< Pointer to an Arnold node */
#define AI_TYPE_ARRAY         0x0D  /**< AtArray */
#define AI_TYPE_MATRIX        0x0E  /**< 4x4 matrix */
#define AI_TYPE_ENUM          0x0F  /**< Enumeration (see \ref AtEnum) */
#define AI_TYPE_UNDEFINED     0xFF  /**< Undefined, you should never encounter a parameter of this type */
#define AI_TYPE_NONE          0xFF  /**< No type */
/* \}*/

/** \name Parameter Categories for User-Data
 * \{
 */
#define AI_USERDEF_UNDEFINED  0     /**< Undefined, you should never encounter a parameter of this category */
#define AI_USERDEF_CONSTANT   1     /**< User-defined: per-object parameter */
#define AI_USERDEF_UNIFORM    2     /**< User-defined: per-face parameter */
#define AI_USERDEF_VARYING    3     /**< User-defined: per-vertex parameter */
#define AI_USERDEF_INDEXED    4     /**< User-defined: per-face-vertex parameter */
/* \}*/

/**
 * Actual parameter value for each supported type
 */
typedef union AtParamValue {
   AtByte       BYTE;
   int          INT;
   unsigned int UINT;
   bool         BOOL;
   float        FLT;
   AtRGB        RGB;
   AtRGBA       RGBA;
   AtVector     VEC;
   AtPoint      PNT;
   AtPoint2     PNT2;
   const char*  STR;
   void*        PTR;
   AtArray*     ARRAY;
   AtMatrix*    pMTX;
} AtParamValue;

/**
 * This represents a parameter of a given node type in Arnold.
 *
 * This holds details like the name, type and default value. The actual
 * contents of this struct are private. 
 */
struct AtParamEntry;

/**
 * \name AtParamEntry Methods
 * \{
 */
AI_API const char*         AiParamGetName    (const AtParamEntry* pentry);
AI_API int                 AiParamGetType    (const AtParamEntry* pentry);
AI_API const AtParamValue* AiParamGetDefault (const AtParamEntry* pentry);
AI_API AtEnum              AiParamGetEnum    (const AtParamEntry* pentry);
AI_API const char*         AiParamGetTypeName(AtByte type);
AI_API int                 AiParamGetTypeSize(AtByte type);
/* \}*/

/**
 * This represents a user-declared parameter in Arnold (user-data).
 *
 * This holds details like name, type, and category.  The actual
 * contents of this struct are private.
 */
struct AtUserParamEntry;

/**
 * \name AtUserParamEntry Methods
 * \{
 */
AI_API const char*  AiUserParamGetName     (const AtUserParamEntry* upentry);
AI_API int          AiUserParamGetType     (const AtUserParamEntry* upentry);
AI_API int          AiUserParamGetArrayType(const AtUserParamEntry* upentry);
AI_API int          AiUserParamGetCategory (const AtUserParamEntry* upentry);
AI_API int          AiUserParamGetIndex    (const AtUserParamEntry* upentry);  // DEPRECATED
/* \}*/

/**
 * \name Parameter Installers
 * These macros are to be called from a node's \c node_parameters method only.
 *
 * There is a different macro for each supported parameter data type. For example,
 * a shader node that implemented a typical fractal noise would declare three
 * parameters like this:
 * \code
 * node_parameters
 * {
 *    AiParameterInt("octaves", 3);
 *    AiParameterFlt("lacunarity", 1.92f);
 *    AiParameterBool("turbulence", false);
 * }
 * \endcode 
 *
 * Please refer to ai_params.h for a description of the functions called by
 * these macros. 
 *
 * \param n         the name of the parameter that will be installed 
 * \param default   the default value of the new parameter (the actual number
 *                  of arguments passed in depends on the parameter's data type,
 *                  for example three floats for a RGB type)
 * \{
 */
#define AiParameterByte(n,c)       AiNodeParamByte (params,-1,n,c);
#define AiParameterInt(n,c)        AiNodeParamInt  (params,-1,n,c);
#define AiParameterUInt(n,c)       AiNodeParamUInt (params,-1,n,c);
#define AiParameterBool(n,c)       AiNodeParamBool (params,-1,n,c);
#define AiParameterFlt(n,c)        AiNodeParamFlt  (params,-1,n,c);
#define AiParameterRGB(n,r,g,b)    AiNodeParamRGB  (params,-1,n,r,g,b);
#define AiParameterRGBA(n,r,g,b,a) AiNodeParamRGBA (params,-1,n,r,g,b,a);
#define AiParameterVec(n,x,y,z)    AiNodeParamVec  (params,-1,n,x,y,z);
#define AiParameterPnt(n,x,y,z)    AiNodeParamPnt  (params,-1,n,x,y,z);
#define AiParameterPnt2(n,x,y)     AiNodeParamPnt2 (params,-1,n,x,y);
#define AiParameterStr(n,c)        AiNodeParamStr  (params,-1,n,c);
#define AiParameterPtr(n,c)        AiNodeParamPtr  (params,-1,n,c);
#define AiParameterNode(n,c)       AiNodeParamNode (params,-1,n,c);
#define AiParameterArray(n,c)      AiNodeParamArray(params,-1,n,c);
#define AiParameterMtx(n,c)        AiNodeParamMtx  (params,-1,n,c);
#define AiParameterEnum(n,c,e)     AiNodeParamEnum (params,-1,n,c,e);
/* \}*/

/*\}*/

/** \name Deprecated
 * \{
 */

#define AiParameterBYTE(n,c)       AiNodeParamByte (params,-1,n,c);
#define AiParameterINT(n,c)        AiNodeParamInt  (params,-1,n,c);
#define AiParameterUINT(n,c)       AiNodeParamUInt (params,-1,n,c);
#define AiParameterBOOL(n,c)       AiNodeParamBool (params,-1,n,c);
#define AiParameterFLT(n,c)        AiNodeParamFlt  (params,-1,n,c);
#define AiParameterVEC(n,x,y,z)    AiNodeParamVec  (params,-1,n,x,y,z);
#define AiParameterPNT(n,x,y,z)    AiNodeParamPnt  (params,-1,n,x,y,z);
#define AiParameterPNT2(n,x,y)     AiNodeParamPnt2 (params,-1,n,x,y);
#define AiParameterSTR(n,c)        AiNodeParamStr  (params,-1,n,c);
#define AiParameterPTR(n,c)        AiNodeParamPtr  (params,-1,n,c);
#define AiParameterNODE(n,c)       AiNodeParamNode (params,-1,n,c);
#define AiParameterARRAY(n,c)      AiNodeParamArray(params,-1,n,c);
#define AiParameterMTX(n,c)        AiNodeParamMtx  (params,-1,n,c);
#define AiParameterENUM(n,c,e)     AiNodeParamEnum (params,-1,n,c,e);

/*\}*/

/* for convenience, the macros above call these functions */
AI_API void  AiNodeParamByte (AtList* params, int varoffset, const char* pname, AtByte pdefault);
AI_API void  AiNodeParamInt  (AtList* params, int varoffset, const char* pname, int pdefault);
AI_API void  AiNodeParamUInt (AtList* params, int varoffset, const char* pname, unsigned int pdefault);
AI_API void  AiNodeParamBool (AtList* params, int varoffset, const char* pname, bool pdefault);
AI_API void  AiNodeParamFlt  (AtList* params, int varoffset, const char* pname, float pdefault);
AI_API void  AiNodeParamRGB  (AtList* params, int varoffset, const char* pname, float r, float g, float b);
AI_API void  AiNodeParamRGBA (AtList* params, int varoffset, const char* pname, float r, float g, float b, float a);
AI_API void  AiNodeParamVec  (AtList* params, int varoffset, const char* pname, float x, float y, float z);
AI_API void  AiNodeParamPnt  (AtList* params, int varoffset, const char* pname, float x, float y, float z);
AI_API void  AiNodeParamPnt2 (AtList* params, int varoffset, const char* pname, float x, float y);
AI_API void  AiNodeParamStr  (AtList* params, int varoffset, const char* pname, const char* pdefault);
AI_API void  AiNodeParamPtr  (AtList* params, int varoffset, const char* pname, void* pdefault);
AI_API void  AiNodeParamNode (AtList* params, int varoffset, const char* pname, AtNode* pdefault);
AI_API void  AiNodeParamArray(AtList* params, int varoffset, const char* pname, AtArray* pdefault);
AI_API void  AiNodeParamMtx  (AtList* params, int varoffset, const char* pname, AtMatrix matrix);
AI_API void  AiNodeParamEnum (AtList* params, int varoffset, const char* pname, int pdefault, AtEnum enum_type);
