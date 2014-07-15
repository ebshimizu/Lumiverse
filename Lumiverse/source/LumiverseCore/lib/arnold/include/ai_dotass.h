/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * API for reading and writing the .ass scene file format
 */

#pragma once
#include "ai_node_entry.h"

/** \defgroup ai_dotass ASS file related API
 *
 *  \details
 *  Arnold has built-in support for writing scene data to a file and later
 *  reading the file in. Although not required, the extension for these files
 *  is usually .ass, which stands for <b>A</b>rnold <b>S</b>cene <b>S</b>ource.
 *  The file format is a straightforward mapping from Arnold \ref AtNode's 
 *  to human-readable ASCII. For example, a sphere node is written as:
 *  \code
 *  sphere          // this is the node class
 *  {               // any number of param/value pairs enclosed in curly braces
 *   center 0 0 0   //  parameter "center" of type AtPoint is set to value (0,0,0)
 *   radius 2.0     //  parameter "radius" of type float is set to value 2.0
 *  }               // end of node block
 *  \endcode
 *
 * \{
 */

AI_API int AiASSWrite(const char* filename, int mask = AI_NODE_ALL, bool open_procs = false, bool binary = true);
AI_API int AiASSLoad(const char* filename, int mask = AI_NODE_ALL);

/*\}*/
