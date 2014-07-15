/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * API for writing shader nodes
 */

#pragma once
#include "ai_nodes.h"
#include "ai_api.h"

// forward declarations
struct AtNode;
struct AtShaderGlobals;

/** \defgroup ai_shaders  Shader Nodes
 * \{
 */

/** Shader Node methods structure */
typedef struct AtShaderNodeMethods {
   void (*Evaluate)(AtNode*, AtShaderGlobals*);
} AtShaderNodeMethods;

/** Shader Node methods exporter */
#define AI_SHADER_NODE_EXPORT_METHODS(tag)      \
AI_INSTANCE_COMMON_METHODS                      \
shader_evaluate;                                \
static AtShaderNodeMethods ai_shader_mtds = {   \
   Evaluate                                     \
};                                              \
static AtNodeMethods ai_node_mtds = {           \
   &ai_common_mtds,                             \
   &ai_shader_mtds                              \
};                                              \
AtNodeMethods* tag = &ai_node_mtds;

/**
 * \name Node Method Declarations
 * \{
 */

/** Evaluate Shader method declaration */
#define shader_evaluate \
static void Evaluate(AtNode* node, AtShaderGlobals* sg)

/* \}*/

/**
 * \name Node Methods
 * \{
 */

AI_API void AiShaderEvaluate(AtNode* node, AtShaderGlobals* sg);

/* \}*/

/*\}*/
