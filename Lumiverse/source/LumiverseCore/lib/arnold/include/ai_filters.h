/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * API for writing pixel-filtering nodes
 */

#pragma once
#include "ai_nodes.h"
#include "ai_vector.h"
#include "ai_types.h"

// forward declaration
struct AtAOVSampleIterator;

/** \defgroup ai_filters Pixel Sample Filter Nodes
 *
 *  \details
 *  Arnold has support for pluggable sample filters. The filter will process a
 *  bucket of AOV subsamples and filter them down to a single AOV sample per
 *  pixel.
 *
 *  Filters operate on a "primary" AOV;  this is the AOV which is specified on
 *  the same "outputs" line as where the filter is specified.    However,
 *  filters can also query supporting, or auxiliary, AOVs which can assist in
 *  filtering.  These  are specified when a filter is initialized (see
 *  AiFilterInitialize()).
 *
 * \{
 */

/** Filter Node methods structure */
typedef struct AtFilterNodeMethods {
   AtByte (*FilterOutputType)(const AtNode*, const AtParamValue*, AtByte);
   void   (*FilterPixel)(AtNode*, AtParamValue*, AtAOVSampleIterator*, void*, AtByte);
} AtFilterNodeMethods;

/** Filter Node methods exporter */
#define AI_FILTER_NODE_EXPORT_METHODS(tag)      \
AI_INSTANCE_COMMON_METHODS                      \
filter_output_type;                             \
filter_pixel;                                   \
static AtFilterNodeMethods ai_filter_mtds = {   \
   FilterOutputType,                            \
   FilterPixel                                  \
};                                              \
static AtNodeMethods ai_node_mtds = {           \
   &ai_common_mtds,                             \
   &ai_filter_mtds                              \
};                                              \
AtNodeMethods *tag = &ai_node_mtds;

/**
 * \name Node Method Declarations
 * \{
 */

/** Filter's filter_output_type method declaration
 *
 *  This function describes how a pixel sample filter will map a particular
 *  input type to an output type.  For example, if a filter is asked to 
 *  filter an Integer AOV, then the filter might decide to convert the 
 *  integers to floats first and then output a float containing the filtered
 *  values.  Most filters' "output" type will match their "input" type.  The
 *  system will use this information to ensure that a filter is only connected
 *  to an output driver which can write the filter's output type.
 * 
 * \param node  The filter node
 * \param params The filter node's parameters array
 * \param input_type  A particular input type (e.g. \c AI_TYPE_RGB, AI_TYPE_FLOAT, etc)
 * \return returns the output type of this filter for a given input type
 */
#define filter_output_type \
static AtByte FilterOutputType(const AtNode* node, const AtParamValue* params, AtByte input_type)

/** Filter's filter_pixel method declaration
 * 
 *  This function peforms pixel filtering.  Generally, this function will contain a
 *  loop which iterates over all the samples of the primary AOV which are contained in
 *  this pixel.
 *
 *  \param node      The filter node
 *  \param params    The filter node's parameter list
 *  \param iterator  An iterator which the filter uses to iterate over all the samples of the
 *                   primary AOV for the current pixel.  The user advances to the next sample
 *                   by calling AiAOVSampleIteratorGetNext().
 *  \param data_out  A pointer to where the filter writes the filtered pixel output.
 *  \param data_type This contains the data-type of the primary AOV.
 */
#define filter_pixel \
static void FilterPixel(AtNode* node, AtParamValue* params, AtAOVSampleIterator* iterator, void* data_out, AtByte data_type)

/* \}*/

/**
 * \name API Methods for Filter Writers
 * \{
 */
AI_API void  AiFilterInitialize(AtNode* node, bool requires_depth, const char** required_aovs, void* data);
AI_API void  AiFilterUpdate(AtNode* node, float width);
AI_API void  AiFilterDestroy(AtNode* node);
AI_API void* AiFilterGetLocalData(const AtNode *node);
/* \}*/

/**
 * \name API Methods to Loop over Samples
 * \{
 */
AI_API void     AiAOVSampleIteratorInitPixel(AtAOVSampleIterator* iter, int x, int y);
AI_API void     AiAOVSampleIteratorReset(AtAOVSampleIterator* iter);
AI_API bool     AiAOVSampleIteratorGetNext(AtAOVSampleIterator* iter);
AI_API bool     AiAOVSampleIteratorGetNextDepth(AtAOVSampleIterator* iter);
AI_API AtPoint2 AiAOVSampleIteratorGetOffset(const AtAOVSampleIterator* iter);
AI_API float    AiAOVSampleIteratorGetInvDensity(const AtAOVSampleIterator* iter);
AI_API int      AiAOVSampleIteratorGetDepth(const AtAOVSampleIterator* iter);
AI_API bool     AiAOVSampleIteratorHasValue(const AtAOVSampleIterator* iter);
AI_API bool     AiAOVSampleIteratorHasAOVValue(const AtAOVSampleIterator* iter, const char* name, AtByte type);
/* \}*/

/**
 * \name API Methods to Get Sample Value from Iterator
 * \{
 */
AI_API bool        AiAOVSampleIteratorGetBool(const AtAOVSampleIterator* iter);
AI_API int         AiAOVSampleIteratorGetInt (const AtAOVSampleIterator* iter);
AI_API float       AiAOVSampleIteratorGetFlt (const AtAOVSampleIterator* iter);
AI_API AtRGB       AiAOVSampleIteratorGetRGB (const AtAOVSampleIterator* iter);
AI_API AtRGBA      AiAOVSampleIteratorGetRGBA(const AtAOVSampleIterator* iter);
AI_API AtVector    AiAOVSampleIteratorGetVec (const AtAOVSampleIterator* iter);
AI_API AtPoint     AiAOVSampleIteratorGetPnt (const AtAOVSampleIterator* iter);
AI_API AtPoint2    AiAOVSampleIteratorGetPnt2(const AtAOVSampleIterator* iter);
AI_API const void* AiAOVSampleIteratorGetPtr (const AtAOVSampleIterator* iter);
/* \}*/

/**
 * \name API Methods to Get Sample Value from Iterator for an Arbitrary AOV
 * \{
 */
AI_API bool        AiAOVSampleIteratorGetAOVBool(const AtAOVSampleIterator* iter, const char* name);
AI_API int         AiAOVSampleIteratorGetAOVInt (const AtAOVSampleIterator* iter, const char* name);
AI_API float       AiAOVSampleIteratorGetAOVFlt (const AtAOVSampleIterator* iter, const char* name);
AI_API AtRGB       AiAOVSampleIteratorGetAOVRGB (const AtAOVSampleIterator* iter, const char* name);
AI_API AtRGBA      AiAOVSampleIteratorGetAOVRGBA(const AtAOVSampleIterator* iter, const char* name);
AI_API AtVector    AiAOVSampleIteratorGetAOVVec (const AtAOVSampleIterator* iter, const char* name);
AI_API AtPoint     AiAOVSampleIteratorGetAOVPnt (const AtAOVSampleIterator* iter, const char* name);
AI_API AtPoint2    AiAOVSampleIteratorGetAOVPnt2(const AtAOVSampleIterator* iter, const char* name);
AI_API const void* AiAOVSampleIteratorGetAOVPtr (const AtAOVSampleIterator* iter, const char* name);
/* \}*/

/*\}*/
