#ifndef _DMXDEVICEPATCH_H_
#define _DMXDEVICEPATCH_H_

#pragma once
#include "Device.h"
#include <sstream>

// Enumeration maps to a function that performs the appropriate conversion
// from OpenLL value to DMX value.
enum conversionType {
  FLOAT_TO_SINGLE
};

// Small struct containing information on where the DMX parameter starts and
// how to convert it to the right type.
struct patchData {
  unsigned int startAddress;
  conversionType type;

  patchData() : startAddress(0), type(conversionType::FLOAT_TO_SINGLE) { }
  patchData(unsigned int addr, conversionType t) : startAddress(addr), type(t) { }
};

// This class includes information on how to translate the device properties
// for a given device to dmx values.
// This information can be loaded from a JSON document while the base address
// is always set dynamically.
class DMXDevicePatch
{
public:
  // Constructs an empty patch
  DMXDevicePatch(unsigned int baseAddress, unsigned int universe);

  // Destroys the patch object
  ~DMXDevicePatch();

  // Given a universe of DMX, update the device.
  // This function will throw logic errors if the device and the patch don't match up.
  void updateDMX(unsigned char* data, Device* device);

  // Gets the universe the device is patched to.
  unsigned int getUniverse() { return m_universe; }

  // Sets the universe the device is patched to
  void setUniverse(unsigned char newUniverse) { m_universe = newUniverse; }

  // Gets the base address for the device
  unsigned int getBaseAddress() { return m_baseAddress; }

  // Sets the base address for the device
  void setBaseAddress(unsigned int newAddress) { m_baseAddress = newAddress; }

  // Map an OpenLL device parameter to a DMX parameter starting at given address
  // using the given conversion type.
  void addParameter(string id, unsigned int address, conversionType type);

private:
  // Base address for the device (zero-indexed)
  unsigned int m_baseAddress;

  // Universe number for the device (zero-indexed)
  unsigned int m_universe;

  // Number of addresses this device uses on a DMX network.
  unsigned int m_numAddresses;

  // Maps device parameters to a start DMX address. The actual translation is
  // handled by a function specified by the conversionType enum in the patchData
  // struct.
  map<string, patchData> m_dmxMap;

  // Conversion Functions
  // These functions all take in a universe of DMX and stick their converted
  // value in the right place.
  // --------------------------------------------------------------------------

  // Converts a float value to a single DMX channel of data. 0.0-1.0 -> 0-255.
  // Corresponds to the FLOAT_TO_SINGLE value of conversionType
  void floatToSingle(unsigned char* data, unsigned int address, float val);

  // Helper for setting DMX values. Not as helpful for small functions but better
  // to just write it once.
  inline void setDMXVal(unsigned char* data, unsigned int address, unsigned char val);
};

#endif