/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * User data functions for shaders
 */

#pragma once
#include "ai_shaderglobals.h"
#include "ai_array.h"
#include "ai_color.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"
#include "ai_nodes.h"

/** \defgroup ai_shader_userdef User-Data API
 *
 * \details
 * This API is for accessing user-defined data on geometry. User-defined data
 * (sometimes called "user-data") are new parameters that have been added to
 * existing node-instances via AiNodeDeclare().
 *
 * There are four kinds of user-defined data: 
 *  - \c constant -- constant parameters are data that are defined on a per-object
 *                   basis and do not vary across the surface of that object.
 *  - \c uniform  -- uniform parameters are data that are defined on a "per-face"
 *                   basis.  During subdivision (if appropriate) values are
 *                   not interpolated.  Instead, the newly subdivided faces will
 *                   contain the value of their "parent" face.
 *  - \c varying  -- varying parameters are data that are defined on a per-vertex
 *                   basis.  During subdivision (if appropriate), the values 
 *                   at the new vertices are interpolated from the values at the
 *                   old vertices.  The user should only create parameters of
 *                   "interpolatable" variable types (such as floats, colors, etc.) 
 *  - \c indexed  -- indexed parameters are data that are defined on a per-face-vertex
 *                   basis.  During subdivision (if appropriate), the values
 *                   at the new vertices are interpolated from the values at the
 *                   old vertices, preserving edges where values were not shared.
 *                   The user should only create parameters of "interpolatable"
 *                   variable types (such as floats, colors, etc.)
 *
 * \{
 */


/** \name User-Data Readers
 *
 * \details
 * These macros are used to retrieve user-data values from within
 * shaders.  If a varying-type user-data is requested, then the
 * user-data will be correctly interpolated across the face of the primitive.
 *
 * In the case of reading constant-type user-data, the usual node-parameter
 * reading functions, such as AiNodeGetInt(), may also be used.
 *
 * \param _name  a string with the name of the user-data value to read
 * \param _val   a pointer to the value in which to store the contents of the
 *               read operation
 * \return a boolean indicating whether the operation was successful
 *
 * Please refer to ai_shader_userdef.h for a description of the userdef-data
 * reading functions called by these macros.
 *
 * \{
 */
#define AiUDataGetBool(_name,_val)    AiUserGetBoolFunc  (_name, sg, _val)
#define AiUDataGetByte(_name,_val)    AiUserGetByteFunc  (_name, sg, _val)
#define AiUDataGetInt(_name,_val)     AiUserGetIntFunc   (_name, sg, _val)
#define AiUDataGetUInt(_name,_val)    AiUserGetUIntFunc  (_name, sg, _val)
#define AiUDataGetFlt(_name,_val)     AiUserGetFltFunc   (_name, sg, _val)
#define AiUDataGetRGB(_name,_val)     AiUserGetRGBFunc   (_name, sg, _val)
#define AiUDataGetRGBA(_name,_val)    AiUserGetRGBAFunc  (_name, sg, _val)
#define AiUDataGetVec(_name,_val)     AiUserGetVecFunc   (_name, sg, _val)
#define AiUDataGetPnt(_name,_val)     AiUserGetPntFunc   (_name, sg, _val)
#define AiUDataGetPnt2(_name,_val)    AiUserGetPnt2Func  (_name, sg, _val)
#define AiUDataGetStr(_name,_val)     AiUserGetStrFunc   (_name, sg, _val)
#define AiUDataGetPtr(_name,_val)     AiUserGetPtrFunc   (_name, sg, _val)
#define AiUDataGetNode(_name,_val)    AiUserGetNodeFunc  (_name, sg, _val)
#define AiUDataGetArray(_name,_val)   AiUserGetArrayFunc (_name, sg, _val)
#define AiUDataGetMatrix(_name,_val)  AiUserGetMatrixFunc(_name, sg, _val)
/*\}*/


/** \name User-Data Derivatives with respect to screen X and Y
 *
 * \details
 * These macros are used to retrieve user-data derivatives w.r.t. X and Y from
 * within shaders.  If a varying-type user-data is requested, then the
 * user-data derivatives will be correctly interpolated across the face of the
 * primitive.  These derivatives can be useful in determining filter sizes to
 * match pixel footprints, such as when texture mapping.
 *
 * In the case of reading constant-type user-data, the derivatives are zero.
 *
 * \param _name     a string with the name of the user-data value to read
 * \param _dx_val   a pointer to the value in which to store the contents of the
 *                  computed derivatives with respect to the X pixel direction
 * \param _dy_val   a pointer to the value in which to store the contents of the
 *                  computed derivatives with respect to the Y pixel direction
 * \return a boolean indicating whether the operation was successful
 *
 * Please refer to ai_shader_userdef.h for a description of the userdef-data
 * reading functions called by these macros.
 *
 * \{
 */
#define AiUDataGetDxyDerivativesFlt(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesFltFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesRGB(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesRGBFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesRGBA(_name,_dx_val,_dy_val)    AiUserGetDxyDerivativesRGBAFunc  (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesVec(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesVecFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesPnt(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesPntFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesPnt2(_name,_dx_val,_dy_val)    AiUserGetDxyDerivativesPnt2Func  (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesArray(_name,_dx_val,_dy_val)   AiUserGetDxyDerivativesArrayFunc (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesMatrix(_name,_dx_val,_dy_val)  AiUserGetDxyDerivativesMatrixFunc(_name, sg, _dx_val, _dy_val)
/*\}*/


/*\}*/


AI_API bool AiUserGetBoolFunc  (const char*, const AtShaderGlobals*, bool*        );
AI_API bool AiUserGetByteFunc  (const char*, const AtShaderGlobals*, AtByte*      );
AI_API bool AiUserGetIntFunc   (const char*, const AtShaderGlobals*, int*         );
AI_API bool AiUserGetUIntFunc  (const char*, const AtShaderGlobals*, unsigned int*);
AI_API bool AiUserGetFltFunc   (const char*, const AtShaderGlobals*, float*       );
AI_API bool AiUserGetRGBFunc   (const char*, const AtShaderGlobals*, AtRGB*       );
AI_API bool AiUserGetRGBAFunc  (const char*, const AtShaderGlobals*, AtRGBA*      );
AI_API bool AiUserGetVecFunc   (const char*, const AtShaderGlobals*, AtVector*    );
AI_API bool AiUserGetPntFunc   (const char*, const AtShaderGlobals*, AtPoint*     );
AI_API bool AiUserGetPnt2Func  (const char*, const AtShaderGlobals*, AtPoint2*    );
AI_API bool AiUserGetStrFunc   (const char*, const AtShaderGlobals*, const char** );
AI_API bool AiUserGetPtrFunc   (const char*, const AtShaderGlobals*, void**       );
AI_API bool AiUserGetNodeFunc  (const char*, const AtShaderGlobals*, AtNode**     );
AI_API bool AiUserGetArrayFunc (const char*, const AtShaderGlobals*, AtArray**    );
AI_API bool AiUserGetMatrixFunc(const char*, const AtShaderGlobals*, AtMatrix*    );

AI_API bool AiUserGetDxyDerivativesFltFunc   (const char*, const AtShaderGlobals*, float*,    float*   );
AI_API bool AiUserGetDxyDerivativesRGBFunc   (const char*, const AtShaderGlobals*, AtRGB*,    AtRGB*   );
AI_API bool AiUserGetDxyDerivativesRGBAFunc  (const char*, const AtShaderGlobals*, AtRGBA*,   AtRGBA*  );
AI_API bool AiUserGetDxyDerivativesVecFunc   (const char*, const AtShaderGlobals*, AtVector*, AtVector*);
AI_API bool AiUserGetDxyDerivativesPntFunc   (const char*, const AtShaderGlobals*, AtPoint*,  AtPoint* );
AI_API bool AiUserGetDxyDerivativesPnt2Func  (const char*, const AtShaderGlobals*, AtPoint2*, AtPoint2*);
AI_API bool AiUserGetDxyDerivativesArrayFunc (const char*, const AtShaderGlobals*, AtArray**, AtArray**);
AI_API bool AiUserGetDxyDerivativesMatrixFunc(const char*, const AtShaderGlobals*, AtMatrix*, AtMatrix*);
