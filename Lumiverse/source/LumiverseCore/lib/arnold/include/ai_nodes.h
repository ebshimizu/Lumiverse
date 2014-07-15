/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * AtNode struct and methods
 */

#pragma once
#include "ai_node_entry.h"
#include "ai_params.h"
#include "ai_api.h"

// forward declarations
struct AtList;
struct AtMetaDataStore;
struct AtNodeEntry;
struct AtUserParamEntry;
struct AtUserParamIterator;

/** \defgroup ai_nodes AtNode API
 *
 *  Arnold has a pluggable node-management system for the standard primitives
 *  such as lights, cameras, geometry, shaders, filters, drivers, etc.  Each
 *  node type has a set of associated methods (member functions) and
 *  parameters.   Like C++'s objects, Arnold's node system allows new nodes to
 *  inherit both parameters and methods from the "parent" node.  For example,
 *  all nodes have a "name" parameter and a "node_initialize{}" method (among
 *  others) because they are declared in the base node from which all nodes
 *  inherit.  
 *
 *  In addition to these standard methods and parameters, derived nodes may
 *  have their own type-specific parameters and methods.  For example, all
 *  shaders have a "shader_evaluate{}" method and all driver nodes have a
 *  "driver_write_bucket{}" method (among others).  User-derived nodes may have their
 *  own new parameters but are not permitted to add new methods.
 *
 *  Two of the important data-structures in this object-oriented-like system are:
 *  \li  AtNodeEntry -- this contains the "description" of a particular node type 
 *                      ("polymesh", "persp_camera", etc.)
 *  \li  AtNode -- this contains the "instantiation" of a particular node type
 * \{
 */

/**
 * \struct AtNode
 * 
 * This represents a node in Arnold.
 */
struct AtNode;

/**
 * \name Node Method Declarations
 * \{
 */

/** Parameter declaration method */
#define node_parameters \
static void Parameters(AtList* params, AtMetaDataStore* mds)

/** Node initialization method */
#define node_initialize \
static void Initialize(AtNode* node, AtParamValue* params)

/** Node update method */
#define node_update \
static void Update(AtNode* node, AtParamValue* params)

/** Node de-initialization method */
#define node_finish \
static void Finish(AtNode* node)

/** Node loading method (for plugin nodes in dynamic libraries) */
#define node_loader \
 AI_EXPORT_LIB bool NodeLoader(int i, AtNodeLib* node)
/* \}*/

/** Exporter for common methods */
#define AI_INSTANCE_COMMON_METHODS         \
node_parameters;                           \
node_initialize;                           \
node_update;                               \
node_finish;                               \
static AtCommonMethods ai_common_mtds = {  \
   Parameters,                             \
   Initialize,                             \
   Update,                                 \
   Finish                                  \
};

/**
 * \name AtNode Methods
 * \{
 */
AI_API AtNode*                 AiNode                (const char* name);
AI_API AtNode*                 AiNodeLookUpByName    (const char* name);
AI_API void                    AiNodeReset           (AtNode* node);
AI_API AtNode*                 AiNodeClone           (const AtNode* node);
AI_API bool                    AiNodeDestroy         (AtNode* node);
AI_API bool                    AiNodeIs              (const AtNode* node, const char* str);
AI_API bool                    AiNodeDeclare         (AtNode* node, const char* param, const char* declaration);
AI_API bool                    AiNodeLink            (AtNode* src,  const char* input, AtNode* target);
AI_API bool                    AiNodeLinkOutput      (AtNode* src,  const char* output, AtNode* target, const char* input);
AI_API bool                    AiNodeUnlink          (AtNode* node, const char* input);
AI_API bool                    AiNodeIsLinked        (const AtNode* node, const char* input);
AI_API AtNode*                 AiNodeGetLink         (const AtNode* node, const char* input, int* comp = NULL);
AI_API const char*             AiNodeGetName         (const AtNode* node);
AI_API const AtNodeEntry*      AiNodeGetNodeEntry    (const AtNode* node);
AI_API AtParamValue*           AiNodeGetParams       (const AtNode* node);
AI_API void*                   AiNodeGetLocalData    (const AtNode* node);
AI_API void                    AiNodeSetLocalData    (AtNode* node, void* data);
AI_API const AtUserParamEntry* AiNodeLookUpUserParameter(const AtNode* node, const char* param);
AI_API AtUserParamIterator*    AiNodeGetUserParamIterator(const AtNode* node);
/* \}*/

/**
 * \name AtUserParamIterator Methods
 * \{
 */
AI_API void                    AiUserParamIteratorDestroy(AtUserParamIterator* iter);
AI_API const AtUserParamEntry* AiUserParamIteratorGetNext(AtUserParamIterator* iter);
AI_API bool                    AiUserParamIteratorFinished(const AtUserParamIterator* iter);
/* \}*/

/**
 * \name Parameter Writers
 * \{
 */
AI_API void AiNodeSetByte  (AtNode* node, const char* param, AtByte val);
AI_API void AiNodeSetInt   (AtNode* node, const char* param, int val);
AI_API void AiNodeSetUInt  (AtNode* node, const char* param, unsigned int val);
AI_API void AiNodeSetBool  (AtNode* node, const char* param, bool val);
AI_API void AiNodeSetFlt   (AtNode* node, const char* param, float val);
AI_API void AiNodeSetRGB   (AtNode* node, const char* param, float r, float g, float b);
AI_API void AiNodeSetRGBA  (AtNode* node, const char* param, float r, float g, float b, float a);
AI_API void AiNodeSetVec   (AtNode* node, const char* param, float x, float y, float z);
AI_API void AiNodeSetPnt   (AtNode* node, const char* param, float x, float y, float z);
AI_API void AiNodeSetPnt2  (AtNode* node, const char* param, float x, float y);
AI_API void AiNodeSetStr   (AtNode* node, const char* param, const char* str);
AI_API void AiNodeSetPtr   (AtNode* node, const char* param, void* ptr);
AI_API void AiNodeSetArray (AtNode* node, const char* param, AtArray* array);
AI_API void AiNodeSetMatrix(AtNode* node, const char* param, AtMatrix matrix);
AI_API void AiNodeSetAttributes(AtNode *node, const char* attributes);
/* \}*/

/**
 * \name Parameter Readers
 * \{
 */
AI_API AtByte       AiNodeGetByte  (const AtNode* node, const char* param);
AI_API int          AiNodeGetInt   (const AtNode* node, const char* param);
AI_API unsigned int AiNodeGetUInt  (const AtNode* node, const char* param);
AI_API bool         AiNodeGetBool  (const AtNode* node, const char* param);
AI_API float        AiNodeGetFlt   (const AtNode* node, const char* param);
AI_API AtRGB        AiNodeGetRGB   (const AtNode* node, const char* param);
AI_API AtRGBA       AiNodeGetRGBA  (const AtNode* node, const char* param);
AI_API AtVector     AiNodeGetVec   (const AtNode* node, const char* param);
AI_API AtPoint      AiNodeGetPnt   (const AtNode* node, const char* param);
AI_API AtPoint2     AiNodeGetPnt2  (const AtNode* node, const char* param);
AI_API const char*  AiNodeGetStr   (const AtNode* node, const char* param);
AI_API void*        AiNodeGetPtr   (const AtNode* node, const char* param);
AI_API AtArray*     AiNodeGetArray (const AtNode* node, const char* param);
AI_API void         AiNodeGetMatrix(const AtNode* node, const char* param, AtMatrix matrix);
/* \}*/

/*\}*/
