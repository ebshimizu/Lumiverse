/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * Message passing API for shader writers
 */

#pragma once
#include "ai_array.h"
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_types.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_shader_message Shader Message Passing API
 *
 * \details
 * This message passing API allows the shader to write a message into the
 * messsage bank which can be retrieved by other shaders in the current/active
 * shading network.  A "message" consists of a value and an associated name.  To
 * retrieve a message (a value), one needs to provide the name of that message.
 *
 * The "lifetime" of these messages is for a single screen sample (it's
 * attached to the \ref AtShaderGlobals of the current pixel/sample).  So, if a single
 * camera ray recursively fires new rays, then the messages created
 * anywhere in that ray-tree will be accessible by any shader in
 * that ray-tree.  When the next camera ray is fired, the message bank will be
 * completely emptied.
 *
 * The message bank has a fixed number of slots, so it is possible that one
 * could fill the message bank.  In that case, all subsequent writes of new messages 
 * will fail.
 *
 * \{
 */

/** \name Message Retrieval Macros
 *
 * \details
 * These macros allow a shader to retrieve messages which have been written to
 * the message bank.  If the named message does not exist (has not been written
 * yet?), then false is returned.  Otherwise, true indicates a successful read.
 *
 * Please refer to ai_shader_message.h for a description of the
 * message-reading functions called by these macros.  
 *
 * \{
 */
#define AiStateGetMsgByte(name,val)  AiMessageGetByteFunc (sg,name,val)
#define AiStateGetMsgBool(name,val)  AiMessageGetBoolFunc (sg,name,val)
#define AiStateGetMsgInt(name,val)   AiMessageGetIntFunc  (sg,name,val)
#define AiStateGetMsgUInt(name,val)  AiMessageGetUIntFunc (sg,name,val)
#define AiStateGetMsgFlt(name,val)   AiMessageGetFltFunc  (sg,name,val)
#define AiStateGetMsgRGB(name,val)   AiMessageGetRGBFunc  (sg,name,val)
#define AiStateGetMsgRGBA(name,val)  AiMessageGetRGBAFunc (sg,name,val)
#define AiStateGetMsgVec(name,val)   AiMessageGetVecFunc  (sg,name,val)
#define AiStateGetMsgPnt(name,val)   AiMessageGetPntFunc  (sg,name,val)
#define AiStateGetMsgPnt2(name,val)  AiMessageGetPnt2Func (sg,name,val)
#define AiStateGetMsgStr(name,val)   AiMessageGetStrFunc  (sg,name,val)
#define AiStateGetMsgPtr(name,val)   AiMessageGetPtrFunc  (sg,name,val)
#define AiStateGetMsgArray(name,val) AiMessageGetArrayFunc(sg,name,val)
/*\}*/

/** \name Message Writing Macros
 *
 * \details
 * These macros allow a shader to write messages to
 * the message bank.  If the write has failed for some reason (perhaps there are 
 * already too many messages in the bank?) then false is returned.  Otherwise,
 * true indicates a successful write.
 *
 * Please refer to ai_shader_message.h for a description of the
 * message-writing functions called by these macros.
 *
 * \{
 */
#define AiStateSetMsgByte(name,val)  AiMessageSetByteFunc (sg,name,val)
#define AiStateSetMsgBool(name,val)  AiMessageSetBoolFunc (sg,name,val)
#define AiStateSetMsgInt(name,val)   AiMessageSetIntFunc  (sg,name,val)
#define AiStateSetMsgUInt(name,val)  AiMessageSetUIntFunc (sg,name,val)
#define AiStateSetMsgFlt(name,val)   AiMessageSetFltFunc  (sg,name,val)
#define AiStateSetMsgRGB(name,val)   AiMessageSetRGBFunc  (sg,name,val)
#define AiStateSetMsgRGBA(name,val)  AiMessageSetRGBAFunc (sg,name,val)
#define AiStateSetMsgVec(name,val)   AiMessageSetVecFunc  (sg,name,val)
#define AiStateSetMsgPnt(name,val)   AiMessageSetPntFunc  (sg,name,val)
#define AiStateSetMsgPnt2(name,val)  AiMessageSetPnt2Func (sg,name,val)
#define AiStateSetMsgStr(name,val)   AiMessageSetStrFunc  (sg,name,val)
#define AiStateSetMsgPtr(name,val)   AiMessageSetPtrFunc  (sg,name,val)
#define AiStateSetMsgArray(name,val) AiMessageSetArrayFunc(sg,name,val)
/*\}*/


/** \name Message Inspection Functions
 *
 * \details
 * These functions let you loop over all available messages for a given sample.
 * This is mainly intended for debugging purposes.
 *
 * \{
 */

/**
 * This represents a message iterator. The actual contents of this struct are
 * private.
 */
typedef struct AtMessageIterator AtMessageIterator;

AI_API AtMessageIterator* AiMessageIterator(const AtShaderGlobals* sg);
AI_API bool               AiMessageIteratorGetNext(AtMessageIterator* iterator, const char** msg_name, int* msg_type);
/*\}*/

/*\}*/

AI_API bool AiMessageGetBoolFunc (const AtShaderGlobals* sg, const char* msg_name, bool* val        );
AI_API bool AiMessageGetByteFunc (const AtShaderGlobals* sg, const char* msg_name, AtByte* val      );
AI_API bool AiMessageGetIntFunc  (const AtShaderGlobals* sg, const char* msg_name, int* val         );
AI_API bool AiMessageGetUIntFunc (const AtShaderGlobals* sg, const char* msg_name, unsigned int* val);
AI_API bool AiMessageGetFltFunc  (const AtShaderGlobals* sg, const char* msg_name, float* val       );
AI_API bool AiMessageGetRGBFunc  (const AtShaderGlobals* sg, const char* msg_name, AtRGB* val       );
AI_API bool AiMessageGetRGBAFunc (const AtShaderGlobals* sg, const char* msg_name, AtRGBA* val      );
AI_API bool AiMessageGetVecFunc  (const AtShaderGlobals* sg, const char* msg_name, AtVector* val    );
AI_API bool AiMessageGetPntFunc  (const AtShaderGlobals* sg, const char* msg_name, AtPoint* val     );
AI_API bool AiMessageGetPnt2Func (const AtShaderGlobals* sg, const char* msg_name, AtPoint2* val    );
AI_API bool AiMessageGetStrFunc  (const AtShaderGlobals* sg, const char* msg_name, const char** val );
AI_API bool AiMessageGetPtrFunc  (const AtShaderGlobals* sg, const char* msg_name, void** val       );
AI_API bool AiMessageGetArrayFunc(const AtShaderGlobals* sg, const char* msg_name, AtArray** val    );

AI_API bool AiMessageSetBoolFunc (AtShaderGlobals* sg, const char* msg_name, bool        );
AI_API bool AiMessageSetByteFunc (AtShaderGlobals* sg, const char* msg_name, AtByte      );
AI_API bool AiMessageSetIntFunc  (AtShaderGlobals* sg, const char* msg_name, int         );
AI_API bool AiMessageSetUIntFunc (AtShaderGlobals* sg, const char* msg_name, unsigned int);
AI_API bool AiMessageSetFltFunc  (AtShaderGlobals* sg, const char* msg_name, float       );
AI_API bool AiMessageSetRGBFunc  (AtShaderGlobals* sg, const char* msg_name, AtRGB       );
AI_API bool AiMessageSetRGBAFunc (AtShaderGlobals* sg, const char* msg_name, AtRGBA      );
AI_API bool AiMessageSetVecFunc  (AtShaderGlobals* sg, const char* msg_name, AtVector    );
AI_API bool AiMessageSetPntFunc  (AtShaderGlobals* sg, const char* msg_name, AtPoint     );
AI_API bool AiMessageSetPnt2Func (AtShaderGlobals* sg, const char* msg_name, AtPoint2    );
AI_API bool AiMessageSetStrFunc  (AtShaderGlobals* sg, const char* msg_name, const char* );
AI_API bool AiMessageSetPtrFunc  (AtShaderGlobals* sg, const char* msg_name, void*       );
AI_API bool AiMessageSetArrayFunc(AtShaderGlobals* sg, const char* msg_name, AtArray*    );
