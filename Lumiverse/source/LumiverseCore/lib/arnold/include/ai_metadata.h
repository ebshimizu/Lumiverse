/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * API for writing and reading node metadata
 */

#pragma once
#include "ai_node_entry.h"
#include "ai_vector.h"
#include "ai_color.h"
#include "ai_api.h"

/** \defgroup ai_nodemeta Node Metadata API
 *
 * \details
 * This API allows the user to attach and retrieve extra information about nodes
 * and their parameters. This can be helpful to maintain extra annotations about
 * parameters for GUI tools for example.
 *
 * \{
 */

/** \name Metadata Storage Structure
 *
 * \details
 * This is a private structure that Arnold uses to maintain metadata information
 * internally. Its contents can only be set/queried by the API calls below.
 */
typedef struct AtMetaDataStore AtMetaDataStore;

/** \name Metadata Writing
 *
 * \details
 * These functions let the author of a node attach auxiliary information about
 * parameters or the node to the node itself. These methods may only be called
 * inside of the \c node_parameters method where the parameters are declared.
 * This function receives an opaque pointer to a AtMetaDataStore structure which
 * will hold the results until AiEnd() is called.
 *
 * Note that Arnold currently supports one special case for metadata. If you attach
 * a string named "synonym" to a parameter or to a node (by passing NULL for param),
 * Arnold will accept the alternative name when creating nodes or setting parameters.
 * This can be helpful for backwards compatibility.
 *
 * Here is an example:
 * \code
 * node_parameters
 * {
 *    AiParameterFlt("Kd", 0.7f);
 *
 *    // create a lower-case synonym for parameter Kd
 *    AiMetaDataSetStr(mds, "Kd", "synonym", "kd");
 *
 *    // describe the parameter
 *    AiMetaDataSetStr(mds, "Kd", "description",
 *       "Diffuse coefficient");
 *    // describe the node itself
 *    AiMetaDataSetStr(mds, NULL, "description",
 *       "This is a simple lambert shader for illustration purposes");
 * }
 * \endcode
 *
 * Note that you should never pass allocated strings, as they will not be freed.
 *
 * \param mds    this is the AtMetaDataStore that is located inside the node's AtNodeEntry (note that \e mds is an argument to \c node_parameters)
 * \param param  the name of the node parameter to which the metadata will be attached (or NULL if you want to attach to the node itself)
 * \param name   the name of the metadata you want to attach
 * \param value  the value of the metadata
 *
 * \{
 */
AI_API void AiMetaDataSetBool(AtMetaDataStore* mds, const char* param, const char* name, bool        value);
AI_API void AiMetaDataSetInt (AtMetaDataStore* mds, const char* param, const char* name, int         value);
AI_API void AiMetaDataSetFlt (AtMetaDataStore* mds, const char* param, const char* name, float       value);
AI_API void AiMetaDataSetPnt (AtMetaDataStore* mds, const char* param, const char* name, AtPoint     value);
AI_API void AiMetaDataSetVec (AtMetaDataStore* mds, const char* param, const char* name, AtVector    value);
AI_API void AiMetaDataSetPnt2(AtMetaDataStore* mds, const char* param, const char* name, AtPoint2    value);
AI_API void AiMetaDataSetRGB (AtMetaDataStore* mds, const char* param, const char* name, AtColor     value);
AI_API void AiMetaDataSetStr (AtMetaDataStore* mds, const char* param, const char* name, const char* value);
/*\}*/

/** \name Metadata Retrieval
 *
 * \details
 * These functions allow client code to examine metadata attached to specific
 * parameters or to a node.
 *
 * Following on the example above:
 * \code
 * const AtNodeEntry* entry = AiNodeEntryLookUp("my_simple_lambert");
 * char* desc;
 * bool success = AiMetaDataGetStr(entry, "Kd", "description", &desc)
 * if (success)
 *    printf("\nDescription for parameter Kd: %s", desc);
 * \endcode
 *
 * \param entry       the AtNodeEntry of the node you want to get metadata from
 * \param param       the name of the node parameter you want to get metadata from
 *                    (or NULL if you are looking for metadata on the node itself)
 * \param name        the name of the metadata you want to get
 * \param[out] value  if the read succeeds, the variable pointed to by \e value
 *                    will be overwritten with the metadata
 * \return            true when the lookup is succesful
 * \{
 */
AI_API bool AiMetaDataGetBool(const AtNodeEntry* entry, const char* param, const char* name, bool*        value);
AI_API bool AiMetaDataGetInt (const AtNodeEntry* entry, const char* param, const char* name, int*         value);
AI_API bool AiMetaDataGetFlt (const AtNodeEntry* entry, const char* param, const char* name, float*       value);
AI_API bool AiMetaDataGetPnt (const AtNodeEntry* entry, const char* param, const char* name, AtPoint*     value);
AI_API bool AiMetaDataGetVec (const AtNodeEntry* entry, const char* param, const char* name, AtVector*    value);
AI_API bool AiMetaDataGetPnt2(const AtNodeEntry* entry, const char* param, const char* name, AtPoint2*    value);
AI_API bool AiMetaDataGetRGB (const AtNodeEntry* entry, const char* param, const char* name, AtColor*     value);
AI_API bool AiMetaDataGetStr (const AtNodeEntry* entry, const char* param, const char* name, const char** value);
/*\}*/

/** \name Metadata Files
 *
 * \details
 * This function allows client code to manually load and apply a metadata file.
 *
 * Usage:
 * \code
 * const char* metadata_file = "metadata_file.mtd";
 * bool success = AiMetaDataLoadFile(metadata_file)
 * if (!success)
 *    printf("\nError loading metadata file %s", metadata_file);
 * \endcode
 *
 * \param filename    the name of the metadata file to load
 * \return            true when the file could be read succesfully
 * \{
 */
AI_API bool AiMetaDataLoadFile(const char* filename);
/*\}*/

/*\}*/
