%module(directors="1") lumiversepython
%feature("director");

%include "typemaps.i"
%include "windows.i"
%include "cpointer.i"

%include "stl.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_shared_ptr.i"
using namespace std;

%template(UCharVector) std::vector<unsigned char>;
%template(StringVector) std::vector<string>;
%shared_ptr(Lumiverse::ShowControl::Timeline)
%shared_ptr(Lumiverse::ShowControl::SineWave)
%shared_ptr(Lumiverse::ShowControl::Layer)
%shared_ptr(Lumiverse::ShowControl::Event)
%shared_ptr(Lumiverse::LumiverseType)
%shared_ptr(Lumiverse::LumiverseEnum)
%shared_ptr(Lumiverse::LumiverseFloat)
%shared_ptr(Lumiverse::LumiverseColor)
%shared_ptr(Lumiverse::LumiverseOrientation)

%apply const std::string& {std::string* m_id};
%apply const std::string& {std::string* m_type};

%{
#include "LumiverseCore.h"
#include "LumiverseShowControl.h"

using namespace Lumiverse;
using namespace Lumiverse::ShowControl;
%}

%include "LumiverseCore.h"
%include "LumiverseShowControl.h"
%include "LumiverseType.h"
%include "types/LumiverseEnum.h"
%include "types/LumiverseFloat.h"
%include "types/LumiverseColor.h"
%include "types/LumiverseOrientation.h"
%include "DMX/DMXPatch.h"
%include "DMX/DMXInterface.h"
%include "DMX/DMXPro2Interface.h"
%include "DeviceSet.h"
%include "DynamicDeviceSet.h"
%include "types/LumiverseColorLib.h"
%include "types/LumiverseTypeUtils.h"
%include "DMX/KiNetInterface.h"
%include "DMX/DMXDevicePatch.h"
%include "DMX/ArtNetInterface.h"
%include "Device.h"
%include "Patch.h"
%include "Rig.h"
%include "Snapshot.h"