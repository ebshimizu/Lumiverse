/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Texture mapping system
 */

#pragma once
#include "ai_color.h"
#include "ai_api.h"
#include "ai_shaderglobals.h"

/** \defgroup ai_texture Texture Mapping API
 * \{                                                                              
 */       

/** \name Wrapping Modes
 *
 * Wrap mode describes what happens when texture coordinates describe
 * a value outside the usual [0,1] range where a texture is defined.
 * \{
 */
#define AI_WRAP_PERIODIC          0x00   /**< the texture repeats itself outside the [0,1] range in (s,t)-space */
#define AI_WRAP_BLACK             0x01   /**< return black outside the [0,1] range   */
#define AI_WRAP_CLAMP             0x02   /**< clamp to the closest texture edge      */
#define AI_WRAP_MIRROR            0x03   /**< mirror the image across the boundaries */
#define AI_WRAP_FILE              0x04   /**< use the wrap mode found in the file    */
/*\}*/

/**
 * \name Texture Look-Up/Interpolation Modes
 *
 * The look-up mode determines how we sample within a mimap level.
 * \{
 */
#define AI_TEXTURE_CLOSEST        0  /**< force the closest texel                           */
#define AI_TEXTURE_BILINEAR       1  /**< force bilinear look-up within a mip level         */
#define AI_TEXTURE_BICUBIC        2  /**< force bicubic look-up within a mip level          */
#define AI_TEXTURE_SMART_BICUBIC  3  /**< bicubic when maxifying, else use bilinear look-up */
/*\}*/

/**
* \name MIP modes
*
* The MIP mode determines how we sample between mipmap levels.
* \{
*/
#define AI_TEXTURE_MIPMODE_DEFAULT      0  /**< use the default mode (auto-selected)                  */
#define AI_TEXTURE_MIPMODE_NONE         1  /**< use highest-res mip level only                        */
#define AI_TEXTURE_MIPMODE_ONE          2  /**< just use one mip level (closest)                      */
#define AI_TEXTURE_MIPMODE_TRILINEAR    3  /**< trilinear blending of two closest mip levels          */
#define AI_TEXTURE_MIPMODE_ANISOTROPIC  4  /**< use two closest mip levels with anisotropic filtering */
/*\}*/

/** Structure that holds all of the available texture map look-up options */
typedef struct AtTextureParams {
   int    filter;          /**< texture look-up mode                              */
   int    mipmap_mode;     /**< mode for mipmap blending and filtering            */
   int    mipmap_bias;     /**< mipmap level bias                                 */
   bool   single_channel;  /**< treat image as single channel                     */
   AtByte start_channel;   /**< starting channel index to read from               */
   float  fill;            /**< value for nonexistent channels (e.g. alpha)       */
   bool   flip_s;          /**< flip S coordinate                                 */
   bool   flip_t;          /**< flip T coordinate                                 */
   bool   swap_st;         /**< enable swapping of S and T coordinates            */
   float  scale_s;         /**< scale S coordinate                                */
   float  scale_t;         /**< scale T coordinate                                */
   int    wrap_s;          /**< wrap mode for S coordinate                        */
   int    wrap_t;          /**< wrap mode for T coordinate                        */
   float  width_s;         /**< multiplicative widening of look-ups on the S axis */
   float  width_t;         /**< multiplicative widening of look-ups on the T axis */
   float  blur_s;          /**< additive blur in look-ups along the S axis        */
   float  blur_t;          /**< additive blur in look-ups along the T axis        */
} AtTextureParams;

AI_API void AiTextureParamsSetDefaults(AtTextureParams* params);

/**
 * \struct AtTextureHandle
 * 
 * \brief  Structure that holds a handle for a given texture
 * 
 * texture handles can be created through \ref AiTextureHandleCreate and destroyed
 * with \ref AiTextureHandleDestroy. Texture's accesses can be done with \ref AiTextureHandleAccess,
 * which is faster than \ref AiTextureAccess.
 */
struct AtTextureHandle;

AI_API AtTextureHandle* AiTextureHandleCreate(const char* filename);
AI_API AtRGBA           AiTextureHandleAccess(const AtShaderGlobals* sg, AtTextureHandle* handle, const AtTextureParams* params, bool* success = NULL);
AI_API void             AiTextureHandleDestroy(AtTextureHandle* handle);

AI_API AtRGBA AiTextureAccess(const AtShaderGlobals* sg, const char* filename, const AtTextureParams* params, bool* success = NULL);

AI_API bool        AiTextureGetResolution(const char* filename, unsigned int* width, unsigned int* height);
AI_API bool        AiTextureGetNumChannels(const char* filename, unsigned int* num_channels);
AI_API const char* AiTextureGetChannelName(const char* filename, unsigned int channel_index);
AI_API bool        AiTextureGetFormat(const char* filename, unsigned int* format);
AI_API bool        AiTextureGetBitDepth(const char* filename, unsigned int* bit_depth);
AI_API bool        AiTextureGetMatrices(const char* filename, AtMatrix world_to_screen, AtMatrix world_to_camera);

/*\}*/
