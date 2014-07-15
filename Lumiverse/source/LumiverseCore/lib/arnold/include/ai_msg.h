/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file 
 * API for logging messages of different severities - info, warnings, errors
 */

#pragma once
#include "ai_types.h"
#include "ai_api.h"

/** \defgroup ai_msg Message Logging API
 * \{
 */
#if !defined(__AI_FILE__)
#define __AI_FILE__ __FILE__
#endif
#if !defined(__AI_LINE__)
#define __AI_LINE__ __LINE__
#endif
#if !defined(__AI_FUNCTION__)
#define __AI_FUNCTION__ __FUNCTION__
#endif
/**
 * \name Severity Codes
 * \{
 */
#define AI_SEVERITY_INFO       0x00    /**< regular information message */ 
#define AI_SEVERITY_WARNING    0x01    /**< warning message             */
#define AI_SEVERITY_ERROR      0x02    /**< error message               */
#define AI_SEVERITY_FATAL      0x03    /**< fatal error message         */
/*\}*/

/**
 * \name Logging Flags
 * \{
 */
#define AI_LOG_NONE            0x0000  /**< don't show any messages                                  */
#define AI_LOG_INFO            0x0001  /**< show all regular information messages                    */
#define AI_LOG_WARNINGS        0x0002  /**< show warning messages                                    */
#define AI_LOG_ERRORS          0x0004  /**< show error messages                                      */
#define AI_LOG_DEBUG           0x0008  /**< show debug messages                                      */
#define AI_LOG_STATS           0x0010  /**< show detailed render statistics                          */
#define AI_LOG_ASS_PARSE       0x0020  /**< show .ass-file parsing details                           */
#define AI_LOG_PLUGINS         0x0040  /**< show details about plugins loaded                        */
#define AI_LOG_PROGRESS        0x0080  /**< show progress messages at 5% increments while rendering  */
#define AI_LOG_NAN             0x0100  /**< show warnings for pixels with NaN's                      */
#define AI_LOG_TIMESTAMP       0x0200  /**< prefix messages with a timestamp (elapsed time)          */
#define AI_LOG_BACKTRACE       0x0400  /**< show stack contents after abnormal program termination (\c SIGSEGV, etc) */
#define AI_LOG_MEMORY          0x0800  /**< prefix messages with current memory usage                */
#define AI_LOG_COLOR           0x1000  /**< add colors to log messages based on severity             */
#define AI_LOG_SSS             0x2000  /**< show messages about sub-surface scattering pointclouds   */
/** set all flags at once */
#define AI_LOG_ALL                                            \
 ( AI_LOG_INFO       | AI_LOG_WARNINGS   | AI_LOG_ERRORS    | \
   AI_LOG_DEBUG      | AI_LOG_STATS      | AI_LOG_PLUGINS   | \
   AI_LOG_PROGRESS   | AI_LOG_NAN        | AI_LOG_ASS_PARSE | \
   AI_LOG_TIMESTAMP  | AI_LOG_BACKTRACE  | AI_LOG_MEMORY    | \
   AI_LOG_COLOR      | AI_LOG_SSS)
/*\}*/

#ifndef AI_PRINTF_ARGS
#  ifdef __GNUC__
      // Enable printf-like warnings with gcc by attaching
      // AI_PRINTF_ARGS to printf-like functions.  Eg:
      //
      // void foo (const char* fmt, ...) AI_PRINTF_ARGS(1,2);
      //
      // The arguments specify the positions of the format string and the the
      // beginning of the varargs parameter list respectively.
#     define AI_PRINTF_ARGS(fmtarg_pos, vararg_pos) __attribute__ ((format (printf, fmtarg_pos, vararg_pos) ))
#  else
#     define AI_PRINTF_ARGS(fmtarg_pos, vararg_pos)
#  endif
#endif

/** Custom message callback, as passed to AiMsgSetCallback() */ 
typedef void (*AtMsgCallBack)(int logmask, int severity, const char* msg_string, int tabs);

AI_API void AiMsgSetLogFileName(const char* filename);
AI_API void AiMsgSetLogFileFlags(int flags);
AI_API void AiMsgSetConsoleFlags(int flags);
AI_API void AiMsgSetMaxWarnings(int max_warnings);
AI_API void AiMsgSetCallback(AtMsgCallBack func);
AI_API void AiMsgResetCallback();

AI_API void AiMsgInfo(const char* format, ...) AI_PRINTF_ARGS(1,2);
AI_API void AiMsgDebug(const char* format, ...) AI_PRINTF_ARGS(1,2);
AI_API void AiMsgWarning(const char* format, ...) AI_PRINTF_ARGS(1,2);
AI_API void AiMsgError(const char* format, ...) AI_PRINTF_ARGS(1,2);
AI_API void AiMsgFatal(const char* format, ...) AI_PRINTF_ARGS(1,2);
AI_API void AiMsgTab(int tabinc);

AI_API AtUInt64 AiMsgUtilGetUsedMemory();
AI_API AtUInt32 AiMsgUtilGetElapsedTime();

/*\}*/
