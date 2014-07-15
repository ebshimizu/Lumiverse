/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Macros for backwards compatibility only - do not include this in new code
 * 
 * IMPORTANT: Only include this file from .c or .cpp. Never include this in a
 *            .h file. Also, make this the _last_ thing you include to avoid
 *            problems with system or 3rd party header files.
 */

#pragma once

#define AiGamma(g,c) AiColorGamma(c, g);

#define AI_PNT2_ZERO AI_P2_ZERO
#define AI_PNT2_ONE  AI_P2_ONE

#define _name AiNodeGetName(node)

#define AiNodeInstall   AiNodeEntryInstall
#define AiNodeUninstall AiNodeEntryUninstall
