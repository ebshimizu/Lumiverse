/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Shader globals struct, methods and macros
 */

#pragma once
#include "ai_params.h"
#include "ai_array.h"
#include "ai_color.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"
#include "ai_api.h"

// forward declarations
struct AtNode;
struct AtBucket;
struct AtShaderGlobalsPrivateInfo;

/** \defgroup ai_shader_globals  Shader Globals API
 * \{
 */

/** Shader globals data structure
 *
 * \details
 * An AtShaderGlobals structure represents a \e shading \e context that holds
 * all the information accesible to the different types of shaders. For
 * example, after a camera ray hits the surface of an object, a shading
 * context of type \c AI_CONTEXT_SURFACE is created that contains, amongst
 * other things, local geometric properties such as the surface normal, the
 * UV surface parameters, etc.
 */
typedef struct AtShaderGlobals {
   int              x;                      /**< X raster-space coordinate of this ray tree                  */
   int              y;                      /**< Y raster-space coordinate of this ray tree                  */
   float            sx;                     /**< X image-space coordinate of this ray, in [-1,1)             */
   float            sy;                     /**< Y image-space coordinate of this ray, in [-1,1)             */
   AtUInt16         si;                     /**< subpixel sample index                                       */
   AtUInt16         i;                      /**< path index (or light sample index for shadow rays)          */
   AtUInt16         n;                      /**< total number of samples to take                             */
   AtUInt16         Rt;                     /**< ray type                                                    */
   float            we;                     /**< sample weight                                               */
   AtUInt32         transp_index;           /**< transparency index                                          */
   AtPoint          Ro;                     /**< ray origin (typically the camera position)                  */
   AtVector         Rd;                     /**< ray direction                                               */
   double           Rl;                     /**< ray length (|Ro-P|)                                         */
   AtByte           tid;                    /**< thread ID                                                   */
   AtByte           Rr;                     /**< recursion level for the ray that created this hit           */
   AtByte           Rr_refl;                /**< ray reflection depth level                                  */
   AtByte           Rr_refr;                /**< ray refraction depth level                                  */
   AtByte           Rr_diff;                /**< ray diffuse depth level                                     */
   AtByte           Rr_gloss;               /**< ray glossy depth level                                      */
   bool             fhemi;                  /**< force hemispherical lighting                                */
   float            time;                   /**< absolute time, between shutter-open and shutter-close       */
   AtUInt32         fi;                     /**< primitive ID (triangle, curve segment, etc)                 */
   AtNode*          Op;                     /**< object pointer                                              */
   AtNode*          proc;                   /**< procedural object pointer (if exists)                       */
   AtNode*          shader;                 /**< pointer to the current shader                               */
   const AtShaderGlobals* psg;              /**< parent shader globals (last shaded)                         */
   AtBucket*        bkt;                    /**< bucket that the pixel being rendered belongs to             */
   AtPoint          Po;                     /**< shading point in object-space                               */
   AtPoint          P;                      /**< shading point in world-space                                */
   AtVector         dPdx;                   /**< surface derivative wrt screen X-axis (not normalized)       */
   AtVector         dPdy;                   /**< surface derivative wrt screen Y-axis (not normalized)       */
   AtVector         N;                      /**< shading normal                                              */
   AtVector         Nf;                     /**< face-forward shading normal                                 */
   AtVector         Ng;                     /**< geometric normal                                            */
   AtVector         Ngf;                    /**< face-forward geometric normal                               */
   AtVector         Ns;                     /**< smoothed normal (same as N but without bump)                */
   float            bu;                     /**< barycentric coordinate (aka alpha, or u)                    */
   float            bv;                     /**< barycentric coordinate (aka beta, or v)                     */
   float            u;                      /**< U surface parameter                                         */
   float            v;                      /**< V surface parameter                                         */
   AtMatrix         M;                      /**< local-to-world matrix transform                             */
   AtMatrix         Minv;                   /**< world-to-local matrix transform                             */
   AtNode**         lights;                 /**< array of active lights at this shading context              */
   AtNode*          Lp;                     /**< pointer to current light node                               */
   int              nlights;                /**< number of active lights                                     */
   float            Ldist;                  /**< distance from P to light source                             */
   AtVector         Ld;                     /**< incident direction from light source                        */
   AtColor          Li;                     /**< incoming intensity from light source                        */
   AtColor          Liu;                    /**< unoccluded incoming intensity from light source             */
   AtColor          Lo;                     /**< shadow occlusion from light source                          */
   AtColor          Ci;                     /**< input color (for volume shaders)                            */
   AtColor          Vo;                     /**< output volume radiance                                      */
   float            area;                   /**< in \c AI_CONTEXT_SURFACE this is the world-space area being
                                                 shaded; in \c AI_CONTEXT_DISPLACEMENT it's the average area
                                                 of all un-displaced triangles shared by the current vertex  */
   AtVector         dPdu;                   /**< surface derivative wrt U parameter (not normalized)         */
   AtVector         dPdv;                   /**< surface derivative wrt V parameter (not normalized)         */
   AtVector         dDdx;                   /**< ray direction derivative wrt screen X-axis (not normalized) */
   AtVector         dDdy;                   /**< ray direction derivative wrt screen Y-axis (not normalized) */
   AtVector         dNdx;                   /**< derivative of surface normal with respect to X-axis         */
   AtVector         dNdy;                   /**< derivative of surface normal with respect to Y-axis         */
   float            dudx;                   /**< U derivative wrt screen X-axis (not normalized)             */
   float            dudy;                   /**< U derivative wrt screen Y-axis (not normalized)             */
   float            dvdx;                   /**< V derivative wrt screen X-axis (not normalized)             */
   float            dvdy;                   /**< V derivative wrt screen Y-axis (not normalized)             */
   AtParamValue     out;                    /**< shader output                                               */
   AtColor          out_opacity;            /**< output opacity (used for automatic transparency)            */
   bool             inclusive_traceset;     /**< is the trace-set inclusive?                                 */
   bool             skip_shadow;            /**< if true, don't trace shadow rays                            */
   AtByte           sc;                     /**< type of shading context                                     */
   const char*      traceset;               /**< trace-set to assign to rays made from this SG               */

   AtShaderGlobalsPrivateInfo* privateinfo; /**< extra information for internal use                          */
} AtShaderGlobals;

/** \name Shading Contexts
 *
 * \details
 * These macros let you determine the purpose that the shader globals were defined for.
 * \{
 */
#define AI_CONTEXT_SURFACE       0x00  /**< for ray-surface intersections         */
#define AI_CONTEXT_VOLUME        0x01  /**< for volume points during ray-marching */
#define AI_CONTEXT_BACKGROUND    0x02  /**< for rays that don't hit any geometry  */
#define AI_CONTEXT_DISPLACEMENT  0x03  /**< for each vertex to be displaced       */
/* \}*/

AI_API AtShaderGlobals* AiShaderGlobals();
AI_API void             AiShaderGlobalsDestroy(AtShaderGlobals* sg);

/*\}*/
