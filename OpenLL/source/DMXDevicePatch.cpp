#include "DMXDevicePatch.h"
#include "DMXDevicePatch.h"


DMXDevicePatch::DMXDevicePatch(unsigned int baseAddress, unsigned int universe)
  : m_baseAddress(baseAddress), m_universe(universe) {
  // Empty for now
}


DMXDevicePatch::~DMXDevicePatch() {
  // Empty for now
}

void DMXDevicePatch::updateDMX(unsigned char* data, Device* device) {
  for (auto& instr : m_dmxMap) {
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
      case (conversionType::FLOAT_TO_SINGLE):
      {
        float val;
        device->getParam(instr.first, val);
        floatToSingle(data, instr.second.startAddress, val);
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

void DMXDevicePatch::addParameter(string id, unsigned int address, conversionType type) {
  m_dmxMap[id] = patchData(address, type);
}

void DMXDevicePatch::floatToSingle(unsigned char* data, unsigned int address, float val) {
  unsigned char cvt = (unsigned char)(255 * val);
  setDMXVal(data, address, cvt);
}

void DMXDevicePatch::setDMXVal(unsigned char* data, unsigned int address, unsigned char val) {
  if (m_baseAddress + address < 0 || m_baseAddress + address >= 512) {
    throw logic_error("Attempting to set data outside of DMX address range (0-511).");
  }
  data[m_baseAddress + address] = val;
}