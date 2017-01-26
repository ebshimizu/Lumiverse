/*! \mainpage Main Page
*
* \section intro_sec Introduction
*
* Lumiverse is a framework for creating lighting control applications.
*
* \section install_sec Installation
*
* * Install CMake (> 2.8 suggested)
* * Install build dependencies for specific modules:
*      * DMX USB PRO MK2 driver: ftd2xx
*      * Bindings for other languages: SWIG (>= 3.0.0)
*    	     * Python: Python
*    	     * C#: .NET (any version that works with SWIG)
* * Make a folder to contain all of your build files
* * Inside of the folder you made, run `cmake ../source`
* * Build
*
* \section platforms Supported Platforms and Interfaces
* Lumiverse supports Windows, OS X, and iOS. Linux has some small issues at the
* moment preventing the project from compiling. I had some issues with the
* FTD2XX driver on OS X, so your mileage with OS X may vary.
*
* At the moment, Lumiverse only has drivers for ENTTEC's DMX PRO MkII devices,
* and has only been tested using one connected DMX PRO device at a time. iOS
* supports none of the USB interfaces. It should support Art-Net and other
* DMX over IP protocols at some point in the future, but I've run it with
* a Socket.IO interface just fine.
*/

/*! \namespace Lumiverse
* \brief Contains all core Lumiverse functions and variables.
*/

/*! \file LumiverseCore.h
* \brief Include file for all of LumiverseCore in one conveninent location.
* 
* This file gets configured according to the interfaces you selected in the cmake
* build settings. Only includes the headers in LumiverseCore. You must include
* other headers separately
*/
#pragma once

#ifndef LUMIVERSE_CORE_H
#define LUMIVERSE_CORE_H

#include "LumiverseCoreConfig.h"

#ifdef USE_KINET
#ifdef _WIN32
// I don't really know why I need this, but apparently the windows socket
// includes get really weird.
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include "lib/Eigen/Dense"
#include "Logger.h"
#include "Device.h"
#include "Rig.h"
#include "DeviceSet.h"
#include "DynamicDeviceSet.h"
#include "Patch.h"
#include "LumiverseType.h"
#include "types/LumiverseFloat.h"
#include "types/LumiverseEnum.h"
#include "types/LumiverseColor.h"
#include "types/LumiverseTypeUtils.h"
#include "types/LumiverseColorLib.h"
#include "DMX/DMXPatch.h"
#include "DMX/DMXDevicePatch.h"
#include "DMX/DMXInterface.h"
#include "lib/libjson/libjson.h"

#ifdef USE_DMXPRO2
#include "DMX/DMXPro2Interface.h"
#endif

#ifdef USE_KINET
#include "DMX/KiNetInterface.h"
#endif

#ifdef USE_ARTNET
#include "DMX/ArtNetInterface.h"
#endif

#if defined(USE_ARNOLD) || defined(USE_ARNOLD_CACHING)
#include <png.h>

#include "Simulation/ArnoldPatch.h"
#include "Simulation/ArnoldAnimationPatch.h"
#include "Simulation/SimulationPatch.h"
#include "Simulation/SimulationAnimationPatch.h"
#include "Simulation/PhotoPatch.h"
#include "Simulation/PhotoAnimationPatch.h"
#include "Simulation/ArnoldFrameManager.h"
#include "Simulation/ArnoldMemoryFrameManager.h"
#include "Simulation/CachingArnoldInterface.h"
#include "Simulation/DistributedArnoldInterface.h"
#endif

#endif