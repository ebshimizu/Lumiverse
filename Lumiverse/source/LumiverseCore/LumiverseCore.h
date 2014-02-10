// Include file for all of LumiverseCore in one conveninent location
#include "LumiverseCoreConfig.h"
#include "Logger.h"
#include "Device.h"
#include "Rig.h"
#include "DeviceSet.h"
#include "Patch.h"
#include "LumiverseType.h"
#include "types/LumiverseFloat.h"
#include "DMX/DMXPatch.h"
#include "DMX/DMXDevicePatch.h"
#include "DMX/DMXInterface.h"
#include "lib/libjson/libjson.h"

#ifdef USE_DMXPRO2
  #include "DMX/DMXPro2Interface.h"
#endif