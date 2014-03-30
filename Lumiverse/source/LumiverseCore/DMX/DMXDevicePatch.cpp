#include "DMXDevicePatch.h"
#include "DMXDevicePatch.h"


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
        floatToFine(data, instr.second.startAddress, val);
        break;
      }
      case (ENUM) :
      {
        LumiverseEnum* val = (LumiverseEnum*)device->getParam(instr.first);
        toEnum(data, instr.second.startAddress, val);
        break;
      }
      default:
      {
        // Unsupported conversion.
        throw logic_error("Device has an invalid conversion type specified.");
      }
    }
  }
}

void DMXDevicePatch::floatToSingle(unsigned char* data, unsigned int address, LumiverseFloat* val) {
  unsigned char cvt = (unsigned char)(255 * val->asPercent());
  setDMXVal(data, address, cvt);
}

void DMXDevicePatch::floatToFine(unsigned char* data, unsigned int address, LumiverseFloat* val) {
  unsigned short cvt = (unsigned short)(65535 * val->asPercent());
  unsigned char coarse = (unsigned char)(cvt >> 8);
  unsigned char fine = (unsigned char) cvt;
  setDMXVal(data, address, coarse);
  setDMXVal(data, address + 1, fine);
}

void DMXDevicePatch::toEnum(unsigned char* data, unsigned int address, LumiverseEnum* val) {
  // It should be noted here that this function currently expects that the range of the enum
  // is within the DMX value.
  unsigned short cvt = (unsigned char)val->getRangeVal();
  setDMXVal(data, address, cvt);
}

void DMXDevicePatch::setDMXVal(unsigned char* data, unsigned int address, unsigned char val) {
  if (m_baseAddress + address >= 512) {
    throw logic_error("Attempting to set data outside of DMX address range (0-511).");
  }
  data[m_baseAddress + address] = val;
}
