%module lumiversepython

%include "typemaps.i"
%include "windows.i"

%include "stl.i"
%include "std_string.i"
%include "std_vector.i"
using namespace std;

%template(UCharVector) std::vector<unsigned char>;
%template(StringVector) std::vector<string>;

%apply const std::string& {std::string* m_id};
%apply const std::string& {std::string* m_type};

%{
#include "LumiverseCore.h"

using namespace Lumiverse;
%}

%include "LumiverseCore.h"
%include "Device.h"
%include "LumiverseType.h"
%include "LumiverseEnum.h"
%include "LumiverseFloat.h"
%include "LumiverseColor.h"
%include "Patch.h"
%include "DMXPatch.h"
%include "DMXInterface.h"
%include "DMXPro2Interface.h"
%include "DeviceSet.h"
%include "DynamicDeviceSet.h"
%include "Rig.h"
%include "LumiverseColorLib.h"
%include "LumiverseTypeUtils.h"
%include "KiNetInterface.h"
%include "DMXDevicePatch.h"
%include "LumiverseOrientation.h"
%include "SimulationAnimationPatch.h"
%include "SimulationPatch.h"
%include "PhotoPatch.h"