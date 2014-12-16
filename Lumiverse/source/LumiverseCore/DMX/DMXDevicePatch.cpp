#include "DMXDevicePatch.h"
#include "DMXDevicePatch.h"
namespace Lumiverse {

DMXDevicePatch::DMXDevicePatch(string mapKey, unsigned int baseAddress, unsigned int universe)
  : m_baseAddress(baseAddress), m_universe(universe), m_dmxMapKey(mapKey) {
  // Empty for now
}


DMXDevicePatch::~DMXDevicePatch() {
  // Empty for now
}

void DMXDevicePatch::updateDMX(unsigned char* data, Device* device, map<string, patchData> dmxMap) {
  for (auto& instr : dmxMap) {
    // Validation checks.
    if (!device->paramExists(instr.first)) {
      // DMX mapping has a parameter that the device does not have.
      ostringstream ss;
      ss << "Device does not have a parameter named " << instr.first << "\n";
      throw logic_error(ss.str().c_str());
    }

    // Eventually might have to check the type of the data in the device to make
    // sure we can process it. Right now it's just floats all day erry day.

    switch (instr.second.type) {
      case (FLOAT_TO_SINGLE):
      {
        LumiverseFloat* val = (LumiverseFloat*) device->getParam(instr.first);
        floatToSingle(data, instr.second.startAddress, val);
        break;
      }
      case (FLOAT_TO_FINE):
      {
        LumiverseFloat* val = (LumiverseFloat*)device->getParam(instr.first);
        floatToFine(data, instr.second.startAddress, val->asPercent());
        break;
      }
      case (ENUM) :
      {
        LumiverseEnum* val = (LumiverseEnum*)device->getParam(instr.first);
        toEnum(data, instr.second.startAddress, val);
        break;
      }
      case (RGB_REPEAT2) :
      {
        LumiverseFloat* val = (LumiverseFloat*)device->getParam(instr.first);
        RGBRepeat(data, instr.second.startAddress, val, 2);
        break;
      }
      case (RGB_REPEAT3) :
      {
        LumiverseFloat* val = (LumiverseFloat*)device->getParam(instr.first);
        RGBRepeat(data, instr.second.startAddress, val, 3);
        break;
      }
      case (RGB_REPEAT4) :
      {
        LumiverseFloat* val = (LumiverseFloat*)device->getParam(instr.first);
        RGBRepeat(data, instr.second.startAddress, val, 4);
        break;
      }
      case(COLOR_RGB) :
      {
        LumiverseColor* val = (LumiverseColor*)device->getParam(instr.first);
        ColorToRGB(data, instr.second.startAddress, val);
        break;
      }
      case (COLOR_RGBW) :
      {
        LumiverseColor* val = (LumiverseColor*)device->getParam(instr.first);
        ColorToRGBW(data, instr.second.startAddress, val);
        break;
      }
      case (COLOR_LUSTRPLUS):
      {
        LumiverseColor* val = (LumiverseColor*)device->getParam(instr.first);
        ColorToLustrPlus(data, instr.second.startAddress, val);
        break;
      }
      case (ORI_TO_FINE) :
      {
        LumiverseOrientation* val = (LumiverseOrientation*)device->getParam(instr.first);
        floatToFine(data, instr.second.startAddress, val->asPercent());
        break;
      }
      default:
      {
        // Unsupported conversion. We're going to demand that the user fix this before
        // proceeding, otherwise you'll just end up with 10000000 log entries
        // for something that's easily fixed in the Rig file.
        stringstream ss;
        ss << "Device \"" << device->getId() << "\" has invalid conversion type specified (" << (int)instr.second.type << ")";
        Logger::log(FATAL, ss.str());

        throw logic_error("Device has an invalid conversion type specified.");
      }
    }
  }
}

void DMXDevicePatch::floatToSingle(unsigned char* data, unsigned int address, LumiverseFloat* val) {
  unsigned char cvt = (unsigned char)(255 * val->asPercent());
  setDMXVal(data, address, cvt);
}

void DMXDevicePatch::floatToFine(unsigned char* data, unsigned int address, float val) {
  unsigned short cvt = (unsigned short)(65535 * val);
  unsigned char coarse = (unsigned char)(cvt >> 8);
  unsigned char fine = (unsigned char) cvt;
  setDMXVal(data, address, coarse);
  setDMXVal(data, address + 1, fine);
}

void DMXDevicePatch::toEnum(unsigned char* data, unsigned int address, LumiverseEnum* val) {
  // It should be noted here that this function currently expects that the range of the enum
  // is within the DMX value.
  unsigned char cvt = (unsigned char)val->getRangeVal();
  setDMXVal(data, address, cvt);
}

void DMXDevicePatch::RGBRepeat(unsigned char* data, unsigned int address, LumiverseFloat* val, int repeats) {
  unsigned char cvt = (unsigned char)(255 * val->asPercent());
  for (int i = 0; i < repeats; i++) {
    setDMXVal(data, address + (i * 3), cvt);
  }
}

void DMXDevicePatch::ColorToRGB(unsigned char* data, unsigned int address, LumiverseColor* val) {
  // Missing parameters will just kinda end up undefined.
  unsigned char r = (unsigned char)(255 * val->getColorChannel("Red"));
  unsigned char g = (unsigned char)(255 * val->getColorChannel("Green"));
  unsigned char b = (unsigned char)(255 * val->getColorChannel("Blue"));

  setDMXVal(data, address, r);
  setDMXVal(data, address + 1, g);
  setDMXVal(data, address + 2, b);
}

void DMXDevicePatch::ColorToRGBW(unsigned char* data, unsigned int address, LumiverseColor* val) {
  unsigned char r = (unsigned char)(255 * val->getColorChannel("Red"));
  unsigned char g = (unsigned char)(255 * val->getColorChannel("Green"));
  unsigned char b = (unsigned char)(255 * val->getColorChannel("Blue"));;
  unsigned char w = (unsigned char)(255 * val->getColorChannel("White"));;

  setDMXVal(data, address, r);
  setDMXVal(data, address + 1, g);
  setDMXVal(data, address + 2, b);
  setDMXVal(data, address + 3, w);
}

void DMXDevicePatch::ColorToLustrPlus(unsigned char* data, unsigned int address, LumiverseColor* val) {
  setDMXVal(data, address, (unsigned char)(255 * val->getColorChannel("Red")));
  setDMXVal(data, address + 1, (unsigned char)(255 * val->getColorChannel("White")));
  setDMXVal(data, address + 2, (unsigned char)(255 * val->getColorChannel("Amber")));
  setDMXVal(data, address + 3, (unsigned char)(255 * val->getColorChannel("Green")));
  setDMXVal(data, address + 4, (unsigned char)(255 * val->getColorChannel("Cyan")));
  setDMXVal(data, address + 5, (unsigned char)(255 * val->getColorChannel("Blue")));
  setDMXVal(data, address + 6, (unsigned char)(255 * val->getColorChannel("Indigo")));
}

void DMXDevicePatch::setDMXVal(unsigned char* data, unsigned int address, unsigned char val) {
  if (m_baseAddress + address >= 512) {
    throw logic_error("Attempting to set data outside of DMX address range (0-511).");
  }
  data[m_baseAddress + address] = val;
}
}