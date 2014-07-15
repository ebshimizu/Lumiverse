/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * AtNodeEntry type and methods
 */

#pragma once
#include "ai_params.h"
#include "ai_api.h"

// forward declarations
struct AtNode;
struct AtList;
struct AtMetaDataStore;

/** \defgroup ai_node_entry AtNodeEntry API
 *  
 *  For a discussion of Arnold's object-oriented system of pluggable nodes, please
 *  refer to \ref ai_nodes.
 * \{
 */ 

/** \name Node Types
 * \{
 */
#define AI_NODE_UNDEFINED   0x0000  /**< Undefined type */
#define AI_NODE_OPTIONS     0x0001  /**< Options node (following the "singleton" pattern, there is only one options node) */
#define AI_NODE_CAMERA      0x0002  /**< Camera nodes (\c persp_camera, \c fisheye_camera, etc) */
#define AI_NODE_LIGHT       0x0004  /**< Light source nodes (\c spot_light, etc) */
#define AI_NODE_SHAPE       0x0008  /**< Geometry nodes (\c sphere, \c polymesh, etc) */
#define AI_NODE_SHADER      0x0010  /**< Shader nodes (\c lambert, etc) */
#define AI_NODE_OVERRIDE    0x0020  /**< EXPERIMENTAL: override nodes support "delayed parameter overrides" for \c procedural nodes */
#define AI_NODE_DRIVER      0x0040  /**< Output driver nodes (\c driver_tiff, etc) */
#define AI_NODE_FILTER      0x0080  /**< Pixel sample filter nodes (\c box_filter, etc */
#define AI_NODE_ALL         0xFFFF  /**< Bitmask including all node types, used by AiASSWrite() */
/* \}*/

/**
 * Methods common to all nodes
 */
struct AtCommonMethods
{
   void (*Parameters)(AtList*, AtMetaDataStore*);
   void (*Initialize)(AtNode*, AtParamValue*);
   void (*Update    )(AtNode*, AtParamValue*);
   void (*Finish    )(AtNode*);
};

/**
 * Node methods
 */
struct AtNodeMethods
{
   AtCommonMethods* cmethods;    /**< common methods    */
   void*            dmethods;    /**< derived methods   */
};

/**
 * \struct AtNodeEntry
 * 
 * This represents a node type in Arnold. There is a node entry for each
 * installed node, whether built-in or plug-in. The actual contents of this
 * struct are private. 
 */
struct AtNodeEntry;

struct AtMetaDataEntry
{
   const char*  name;
   const char*  param;
   AtByte       type;
   AtParamValue value;
};

struct AtParamIterator;
struct AtMetaDataIterator;

/**
 * \name AtNodeEntry Methods
 * \{
 */
AI_API const AtNodeEntry*  AiNodeEntryLookUp             (const char* name);
AI_API const char*         AiNodeEntryGetName            (const AtNodeEntry* nentry);
AI_API int                 AiNodeEntryGetType            (const AtNodeEntry* nentry);
AI_API const char*         AiNodeEntryGetTypeName        (const AtNodeEntry* nentry);
AI_API int                 AiNodeEntryGetOutputType      (const AtNodeEntry* nentry);
AI_API const char*         AiNodeEntryGetFilename        (const AtNodeEntry* nentry);
AI_API const char*         AiNodeEntryGetVersion         (const AtNodeEntry* nentry);
AI_API int                 AiNodeEntryGetCount           (const AtNodeEntry* nentry);
AI_API int                 AiNodeEntryGetNumParams       (const AtNodeEntry* nentry);
AI_API const AtParamEntry* AiNodeEntryGetParameter       (const AtNodeEntry* nentry, int i);
AI_API const AtParamEntry* AiNodeEntryLookUpParameter    (const AtNodeEntry* nentry, const char* param);
AI_API AtParamIterator*    AiNodeEntryGetParamIterator   (const AtNodeEntry* nentry);
AI_API AtMetaDataIterator* AiNodeEntryGetMetaDataIterator(const AtNodeEntry* nentry, const char* param = NULL);
AI_API void                AiNodeEntryInstall            (int type, AtByte output_type, const char* name, const char* filename, AtNodeMethods* methods, const char* version);
AI_API void                AiNodeEntryUninstall          (const char* name);
/* \}*/

/**
 * \name AtParamIterator Methods
 * \{
 */
AI_API void                AiParamIteratorDestroy(AtParamIterator* iter);
AI_API const AtParamEntry* AiParamIteratorGetNext(AtParamIterator* iter);
AI_API bool                AiParamIteratorFinished(const AtParamIterator* iter);
/* \}*/

/**
 * \name AtMetaDataIterator Methods
 * \{
 */
AI_API void                   AiMetaDataIteratorDestroy(AtMetaDataIterator* iter);
AI_API const AtMetaDataEntry* AiMetaDataIteratorGetNext(AtMetaDataIterator* iter);
AI_API bool                   AiMetaDataIteratorFinished(const AtMetaDataIterator* iter);
/* \}*/

/*\}*/
