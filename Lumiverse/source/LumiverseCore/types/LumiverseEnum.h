#ifndef _LUMIVERSEENUM_
#define _LUMIVERSEENUM_

#pragma once

#include "../LumiverseType.h"
#include <map>

// Defines an enumeration in Lumiverse.
// Moving lights often have various parameters that are enumerated
// and encoded in a DMX range (ex. 10-19 on channel 6 = use gobo 1).
// This class will allow specification of specific settings by name or number
// and also will allow users to tweak the values in the range for controls
// that may use the range as a speed control.
class LumiverseEnum : LumiverseType
{
public:
  // When you select an enumeration, mode determines where in the range
  // the value will fall. FIRST sets the value to the start of the range,
  // CENTER sets the value to the middle of the range (default), and
  // LAST sets the value to the end of the range.
  enum Mode {
    FIRST,
    CENTER,
    LAST
  };
  
  // Constructs an enumeration with nothing in it
  LumiverseEnum(Mode mode = CENTER, int rangeMax = 255);

  // Constructs an enumeration with the given keys and range start values.
  // If nothing is passed in for default, the first key (sorted by range start)
  // will be set as default
  LumiverseEnum(map<string, int> keys, Mode mode = CENTER, int rangeMax = 255, string def = "");

  // Same as normal contsructor except mode is passed as a string.
  LumiverseEnum(map<string, int> keys, string mode, int rangeMax = 255, string def = "");

  // Destructor fun times.
  ~LumiverseEnum();

  // Says that this object is an enum.
  virtual string getTypeName() { return "enum"; }

  // Resets the enum to default
  virtual void reset();

  // Returns this object as a JSON node
  virtual JSONNode toJSON(string name);

  // Adds a value to the enumeration
  // Will overwrite an existing set of values if they already exist.
  void addVal(string name, int start);

  // Removes a value from the enumeration
  void removeVal(string name);

  // Sets the value of the enumeration
  // Returns true on success, false on failure.
  bool setVal(string name);

  // Sets the value of the enumeration and the tweak value
  // Returns true on success, false on failure.
  bool setVal(string name, float tweak);

  // Tweaks the value of the enum
  void setTweak(float tweak);

  // Sets the default value of the enumeration
  void setDefault(string name) { m_default = name; }

  // Gets the current state of the enumeration
  string getVal() { return m_active; }

  // Gets the tweak value for the enumeration
  float getTweak() { return m_tweak; }

  // Gets the default value
  string getDefault() { return m_default; }

  // Returns the exact value in the range given the active parameter and
  // the tweak value
  float getRangeVal();

  // Operator overrides woo
  void operator=(string name);
  void operator=(LumiverseEnum val);

private:
  // Sets the tweak value based on the mode.
  void setTweakWithMode();

  // Returns the current mode as a string
  string modeAsString();

  // Converts a string to a mode.
  Mode stringToMode(string input);

  // The selected enumeration
  string m_active;

  // The default value for the enumeration
  string m_default;

  // Map of the name of the enumeration to the start of the range.
  // The start of the range is typically stated as a DMX value (0-255)
  map<string, int> m_nameToStart;

  // Maps the start of a range to the name of the enumeration.
  map<int, string> m_startToName;

  // Enumeration mode
  Mode m_mode;

  // Tweak determines where in the range we are. Middle = 0.5 and the
  // value can go from 0 to 1, with 0 being the front of the range, and 1 being the end.
  // This value can be adjusted to get the exact value in the desired range.
  float m_tweak;

  // Maximum value for the enumeration range. Typically this will be 255, but if
  // someone comes out with a different protocol not limited by DMX this will change.
  int m_rangeMax;
};

#endif