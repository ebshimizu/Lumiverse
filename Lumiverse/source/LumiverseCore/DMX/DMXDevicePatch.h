/*! \file DMXDevicePatch.h
* \brief Tells a DMXPatch how to interpret the Device data.
*/
#ifndef _DMXDEVICEPATCH_H_
#define _DMXDEVICEPATCH_H_

#pragma once
#include "../Device.h"
#include <sstream>

namespace Lumiverse {
  /*!
  * \brief Enumeration maps to a function that performs the appropriate conversion
  * from Lumiverse value to DMX value.
  */
  enum conversionType {
    FLOAT_TO_SINGLE,  /*!< Converts a floating point to a single-byte DMX value (0-255) */
    FLOAT_TO_FINE,    /*!< Converts a floating point to a double-byte DMX value (0-65535) */
    ENUM,             /*!< Converts a LumiverseEnum to a single-byte DMX value (0-255) */
    RGB_REPEAT2,      /*!< Converts a floating point value to a single-byte DMX value and outputs it twice offset by 3. */
    RGB_REPEAT3,      /*!< Converts a floating point value to a single-byte DMX value and outputs it three times offset by 3. */
    RGB_REPEAT4,      /*!< Converts a floating point value to a single-byte DMX value and outputs it four times offset by 3. */
    COLOR_RGB,        /*!< Converts a color with RGB parameters to single-byte (0-255) DMX parameters. */
    COLOR_RGBW,       /*!< Converts a color with RGBW parameters to singly-byte (0-255) DMX parameters. */
    ORI_TO_FINE       /*!< Converts an orientation to a double-byte DMX value (0-65535) */
  };

  /*!
  * \brief Small struct containing information on where the DMX parameter starts and
  * how to convert it to the right type.
  * \sa DMXDevicePatch, DMXPatch, conversionType
  */
  struct patchData {
    /*!
    * \brief Starting address for the parameter
    *
    * Relative to where the Device is patched on the network.
    * For example, if a device is patched to address 35, and the Tilt
    * parameter has a startAddress of 2, then the Tilt parameter is sent out 
    * on address 37 (35+2).
    */
    unsigned int startAddress; 

    /*!
    * \brief How the LumiverseType should be converted to DMX.
    * \sa conversionType
    */
    conversionType type;

    /*! \brief Constructs a default patch entry. 
    *
    * Default assumes a starting address of 0 and a floating point to single DMX byte conversion.
    */
    patchData() : startAddress(0), type(FLOAT_TO_SINGLE) { }

    /*! \brief Constructs a patch entry
    *
    * \param addr Starting address for the parameter
    * \param t Conversion method for the parameter
    */
    patchData(unsigned int addr, conversionType t) : startAddress(addr), type(t) { }

    /*! \brief Constructs a patch entry from a string conversionType
    * \param addr Starting address
    * \param t Conversion method as a string.
    */
    patchData(unsigned int addr, string t) : startAddress(addr) {
      // Note to self: make a static dictionary with this instead.
      if (t == "FLOAT_TO_SINGLE") { type = FLOAT_TO_SINGLE; }
      else if (t == "FLOAT_TO_FINE") { type = FLOAT_TO_FINE; }
      else if (t == "ENUM") { type = ENUM; }
      else if (t == "RGB_REPEAT2") { type = RGB_REPEAT2; }
      else if (t == "RGB_REPEAT3") { type = RGB_REPEAT3; }
      else if (t == "RGB_REPEAT4") { type = RGB_REPEAT4; }
      else if (t == "COLOR_RGB") { type = COLOR_RGB; }
      else if (t == "COLOR_RGBW") { type = COLOR_RGBW; }
      else if (t == "ORI_TO_FINE") { type = ORI_TO_FINE; }
      else {
        Logger::log(WARN, "Unknown conversion type. Defaulting to float to single.");
        type = FLOAT_TO_SINGLE;
      }
    }
  };

  /*!
  * \brief This class includes information on how to translate the device properties
  * for a given device to DMX values.
  *
  * This information can be loaded from a JSON document while the base address
  * is always set dynamically.
  * Note that the map of parameter to relative DMX address is stored not in this object, but in the
  * DMX Patch. This allows reuse of the patch data for similar instruments.
  * \sa DMXPatch, DMXDevicePatch, patchData
  */
  class DMXDevicePatch
  {
  public:
    /*!
    * \brief Constructs an empty patch
    *
    * \param mapKey Key to lookup the DMX Map
    * \param baseAddress Base address for the device as a whole
    * \param universe Universe the device lives in
    */
    DMXDevicePatch(string mapKey, unsigned int baseAddress, unsigned int universe);

    /*! \brief Destroys the patch object */
    ~DMXDevicePatch();

    /*! 
    * \brief Given a universe of DMX, update the device.
    *
    * This function will throw logic errors if the device and the patch don't match up.
    * \param data Buffer of 512 bytes representing the universe to update
    * \param device The Device to pull data from
    * \param dmxMap Table to DMX Maps to tell this function how to interpret the Device's data.
    */
    void updateDMX(unsigned char* data, Device* device, map<string, patchData> dmxMap);

    /*! \brief Gets the universe the device is patched to.
    * \return The Device's universe */
    unsigned int getUniverse() { return m_universe; }

    /*!
    * \brief Sets the universe the device is patched to
    * \param newUniverse The new universe to patch the device to.
    */
    void setUniverse(unsigned char newUniverse) { m_universe = newUniverse; }

    /*!
    * \brief Gets the base address for the device
    * \return The Device's base address
    */
    unsigned int getBaseAddress() { return m_baseAddress; }

    /*!
    * \brief Sets the base address for the device
    * \param newAddress The new base address for the device
    */
    void setBaseAddress(unsigned int newAddress) { m_baseAddress = newAddress; }

    /*!
    * \brief Gets the key for the DMX map this device should use to translate
    * its Lumiverse values to DMX values.
    * \return The DMX map key for this patch object.
    */
    string getDMXMapKey() { return m_dmxMapKey; }

  private:
    /*! \brief Base address for the device (zero-indexed) */
    unsigned int m_baseAddress;

    /*! \brief Universe number for the device (zero-indexed) */
    unsigned int m_universe;

    /*! \brief Number of addresses this device uses on a DMX network. */
    unsigned int m_numAddresses;

    /*!
    * \brief Key to get the map that maps device parameters to a start DMX address.
    *
    * The actual translation is
    * handled by a function specified by the conversionType enum in the patchData
    * struct.
    * This value can be shared by multiple devices.
    */
    //map<string, patchData> m_dmxMap;
    string m_dmxMapKey;

    // Conversion Functions
    // These functions all take in a universe of DMX and stick their converted
    // value in the right place.
    // --------------------------------------------------------------------------

    /*!
    * \brief Converts a float value to a single DMX channel of data. min-max -> 0-255.
    *
    * Corresponds to the FLOAT_TO_SINGLE value of conversionType
    * \param data DMX Universe buffer
    * \param address Address to write the value to
    * \param LumiverseFloat value to convert
    */
    void floatToSingle(unsigned char* data, unsigned int address, LumiverseFloat* val);

    /*!
    * \brief Converts a float value to two DMX channels of data. min-max -> 0 - 65535
    *
    * Corresponds to the FLOAT_TO_FINE and ORI_TO_FINE values of conversionType
    * The first channel is the upper bits (coarse) and the second channel is the lower (fine)
    * The given float must be between 0 and 1 otherwise overflow will occur.
    * \param data DMX Universe buffer
    * \param address First address to write the value to (the coarse bits)
    * \param val The float value to convert. Must be in the range [0, 1].
    */
    void floatToFine(unsigned char* data, unsigned int address, float val);

    /*!
    * \brief Converts an enum to a single DMX channel of data
    *
    * Corresponds to the ENUM value of conversionType
    * \param data DMX Universe buffer
    * \param address First address to write the value to
    * \param val the LumiverseEnum value to convert.
    */
    void toEnum(unsigned char* data, unsigned int address, LumiverseEnum* val);
    
    /*!
    * \brief Converts a float value to a single DMX channel of data. min-max -> 0-255 and repeats it
    * a number of times, offset by 3 each time
    *
    * This function is meant to assist the programming of fixtures where
    * less precision is required, but still have a large number of channels to deal with.
    * \param data DMX Universe buffer
    * \param address Address to write the value to
    * \param val LumiverseFloat value to convert
    * \param repeats Number of times to repeat the writing of the data.    
    */
    void RGBRepeat(unsigned char* data, unsigned int address, LumiverseFloat* val, int repeats);

    /*!
    * \brief Converts a LumiverseColor to 3 channels of DMX data.
    *
    * This function assumes colors are laid out in standard RGB order.
    * \param data DMX universe buffer
    * \param address Address to write the value to.
    * \param val LumiverseColor value to convert.
    */
    void ColorToRGB(unsigned char* data, unsigned int address, LumiverseColor* val);

    /*!
    * \brief Converts a LumiverseColor to 4 channels of DMX data.
    *
    * Conversion assumes that colors are laid out in standard RGBW order.
    * \param data DMX Universe buffer
    * \param address Address to write the value to
    * \param val LumiverseColor value to convert
    */
    void ColorToRGBW(unsigned char* data, unsigned int address, LumiverseColor* val);

    /*!
    * \brief Helper for setting DMX values.
    *
    * Not as helpful for small functions but better to just write it once.
    * \param data DMX Universe buffer
    * \param address Address to write the value to 
    * \param DMX value to write, single byte.
    */
    inline void setDMXVal(unsigned char* data, unsigned int address, unsigned char val);
  };
}

#endif
