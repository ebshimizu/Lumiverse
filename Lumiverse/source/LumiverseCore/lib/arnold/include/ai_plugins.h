/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Runtime loading of plugin nodes
 */

#pragma once
#include "ai_api.h"
#include "ai_types.h"
#include "ai_version.h"

#ifdef _WIN32
#define AI_DLL_EXPORT __declspec(dllexport)
#else
#define AI_DLL_EXPORT __attribute__ ((visibility("default")))
#endif

/** \defgroup ai_plugins Plug-in Loading API 
 * \{                                                                              
 */       

/**
 * Used by dynamically-linked nodes to export the entry-point function
 */
#define AI_EXPORT_LIB AI_EXTERN_C AI_DLL_EXPORT

/**
 * Used by dynamically-linked nodes to return node info
 */
typedef struct AtNodeLib {
   int         node_type;                    /**< type of node (\c AI_NODE_SHADER, \c AI_NODE_CAMERA, etc) */
   AtByte      output_type;                  /**< output type for shader nodes (\c AI_TYPE_RGB, etc)       */
   const char* name;                         /**< name of this plug-in node ("lambert", etc)               */
   void*       methods;                      /**< pointer to this node's methods                           */
   char        version[AI_MAXSIZE_VERSION];  /**< Arnold version that this node was compiled against       */
} AtNodeLib;

AI_API void AiLoadPlugins(const char* directory);
AI_API void AiLoadPlugin(const char* path);

/*\}*/
