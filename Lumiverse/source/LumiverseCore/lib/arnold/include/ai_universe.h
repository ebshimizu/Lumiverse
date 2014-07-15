/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Global entry point to the scene graph elements (options, camera, lights, etc)
 */

#pragma once
#include "ai_types.h"
#include "ai_bbox.h"
#include "ai_api.h"

// forward declares
struct AtNode;
struct AtNodeIterator;
struct AtNodeEntry;
struct AtNodeEntryIterator;
struct AtAOVIterator;

struct AtAOVEntry
{
   const char* name;
   AtByte      type;
   int         blend_mode;
};

/** \defgroup ai_universe AiUniverse API
 *
 * \details
 * The AiUniverse API provides access to global information about the scene.
 * There is only one universe at any given time.
 * 
 * \{                                                                              
 */       

/** \name Methods
 * \{
 */
AI_API bool                 AiUniverseIsActive();
AI_API bool                 AiUniverseCacheFlush(int cache_flags);
AI_API AtNode*              AiUniverseGetOptions();
AI_API AtNode*              AiUniverseGetCamera();
AI_API AtBBox               AiUniverseGetSceneBounds();
AI_API AtNodeIterator*      AiUniverseGetNodeIterator(unsigned int node_mask);
AI_API AtNodeEntryIterator* AiUniverseGetNodeEntryIterator(unsigned int node_mask);
AI_API AtAOVIterator*       AiUniverseGetAOVIterator();
/*\}*/

/** \name Node Iterator API
 * \{
 */
AI_API void    AiNodeIteratorDestroy(AtNodeIterator* iter);
AI_API AtNode* AiNodeIteratorGetNext(AtNodeIterator* iter);
AI_API bool    AiNodeIteratorFinished(const AtNodeIterator* iter);
/*\}*/

/** \name Node Entry Iterator API
 * \{
 */
AI_API void         AiNodeEntryIteratorDestroy(AtNodeEntryIterator* iter);
AI_API AtNodeEntry* AiNodeEntryIteratorGetNext(AtNodeEntryIterator* iter);
AI_API bool         AiNodeEntryIteratorFinished(const AtNodeEntryIterator* iter);
/*\}*/

/** \name AOV Iterator API
 * \{
 */
AI_API void              AiAOVIteratorDestroy(AtAOVIterator* iter);
AI_API const AtAOVEntry* AiAOVIteratorGetNext(AtAOVIterator* iter);
AI_API bool              AiAOVIteratorFinished(const AtAOVIterator* iter);
/*\}*/

/** \name Cache Types
 * \{
 */
#define AI_CACHE_TEXTURE        0x0001  /**< Flushes all texturemaps */
#define AI_CACHE_HAIR_DIFFUSE   0x0004  /**< Flushes hair diffuse cache */
#define AI_CACHE_BACKGROUND     0x0008  /**< Flushes all skydome importance tables for background */ 
#define AI_CACHE_QUAD           0x0010  /**< Flushes all quad lights importance tables */ 
#define AI_CACHE_ALL            0xFFFF  /**< Flushes all cache types simultaneously */
/*\}*/

/*\}*/
