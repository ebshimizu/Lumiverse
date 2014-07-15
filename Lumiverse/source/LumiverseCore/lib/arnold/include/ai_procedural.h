/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * API for writing procedural geometry nodes
 */

#pragma once
#include "ai_version.h"

// forward declaration
struct AtNode;

/** \defgroup ai_procedurals Procedural API
 * 
 * This API is used to create geometry at render time. This is accomplished
 * by providing the renderer some callback functions which are called as needed
 * (when the procedural is hit by a ray). Procedurals should only declare geometry
 * and perhaps shader nodes.
 *
 * Note that procedurals can recursively create other procedural nodes. This allows
 * small scene files to define huge scenes, where every piece of geometry is loaded
 * on-demand: geometry that is not visible by the camera (or by other types of rays,
 * e.g. reflection) will never be loaded in memory.
 *
 * There is a special mode, selected by setting load_at_init=true, that will force
 * the loading of a procedural during the initialization process. In that case, that
 * procedural (and any other with the same setting) will be loaded serially (unless
 * it was created from another procedural which is being created during rendering, in
 * which case it will be executed in the current thread, possibly in parallel with
 * others).
 *
 * So, in most cases, when rendering with multiple threads, the procedurals could be
 * expanded in parallel, depending on the circumstances of the render. Because of that,
 * when writing a new procedural node, make sure it is properly designed to be
 * re-entrant, and take into account any possible locks (currently, it only happens
 * when loading .ass files from a procedural).
 *
 * \{                                                                              
 */       

/**
 * Procedural init method.
 * 
 * This method will be called first and should perform any initialization required
 * by your procedural. You probably want to create new nodes inside this method
 * but you should return them through \ref AtProcGetNode and correctly return
 * the number of created nodes from \ref AtProcNumNodes, otherwise the behavior
 * is undefined. Alternatively, if you know ahead of time exactly how many nodes
 * you are going to create, you can create them in \ref AtProcGetNode too.
 *
 * \param      node      This is the procedural node itself
 * \param[out] user_ptr  This is a general-purpose, user-supplied data pointer that
 *                       Arnold will pass along to the other procedural methods.
 * \return               true upon success
 */
typedef int (*AtProcInit)(AtNode* node, void** user_ptr);

/**
 * Procedural cleanup method.
 * 
 * This method will be called last and should perform any cleanup required
 * by your procedural. Make sure you release any memory you allocated that is no
 * longer needed by Arnold.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \return          true upon success
 */
typedef int (*AtProcCleanup)(void* user_ptr);

/**
 * Procedural node count method.
 * 
 * This method will be called after initialization and should report the exact
 * number of nodes to be created.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \return          The number of nodes in the procedural
 */
typedef int (*AtProcNumNodes)(void* user_ptr);

/**
 * Procedural node fetching method.
 * 
 * This method will be called once for each node to be created (as determined by
 * \ref AtProcNumNodes). Note that if you created any node in \ref AtProcInit, they
 * also should be returned here, otherwise the behaviour would be undefined.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \param i         Node index, in the range 0 to \ref AtProcNumNodes - 1
 * \return          The i'th node in the procedural
 */
typedef AtNode* (*AtProcGetNode)(void* user_ptr, int i);

/**
 * Procedural function table
 * 
 * This structure is used to report the function pointers that the
 * renderer needs to call at runtime. The version field is used for runtime
 * compatibility checking.
 */
typedef struct AtProcVtable {
   AtProcInit     Init;                         /**< This is called first and should initialize the procedural */
   AtProcCleanup  Cleanup;                      /**< This is called last and should clean up any temporary memory used by the procedural */
   AtProcNumNodes NumNodes;                     /**< This is called to find out how many nodes this procedural will generate */
   AtProcGetNode  GetNode;                      /**< This is called NumNodes times, once for each node the procedural creates */
   char           version[AI_MAXSIZE_VERSION];  /**< You must copy the contents of the AI_VERSION macro here */
} AtProcVtable;

/**
 * Procedural entry-point symbol
 *
 * Procedural DSO's must define a symbol named "ProcLoader" of this
 * type. It should fill in the fields of a \ref AtProcVtable structure.
 *
 * \param[out] vtable  List of procedural methods to be supplied by the user
 * \return             true upon success
 */
typedef int (*AtProcLoader)(AtProcVtable* vtable);

/**
 * Handy shortcut for writing procedurals.
 *
 * This is a shorter, alternate way to declare the \ref AtProcLoader symbol.
 * Here is an example:
 * \code
 * node_loader 
 * {
 *    vtable->Init     = MyInit;
 *    vtable->Cleanup  = MyCleanup;
 *    vtable->NumNodes = MyNumNodes;
 *    vtable->GetNode  = MyGetNode;
 *    strcpy(vtable->version, AI_VERSION);
 *    return true;
 * }
 * \endcode
 */
#define proc_loader AI_EXPORT_LIB int ProcLoader(AtProcVtable* vtable)

/*\}*/
