#ifndef _LUMIVERSEENUM_
#define _LUMIVERSEENUM_

#pragma once

#include "../LumiverseType.h"
#include <map>
#include <memory>
#include <mutex>

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

  // Enumerations on lighting devices can be tricky to deal with in
  // transitions. This setting provides a way for LumiverseTypeUtils to
  // do a linear (or other) interpolation between two enums.
  // Default is SMOOTH_WITHIN_OPTION
  enum InterpolationMode {
    // Snap to new value immediately
    SNAP,
    // This mode will interpolate within a particular setting.
    // For example, if you have a color wheel rotate option where values closer to
    // 128 are slow, and closer to 255 are fast, this would interpolate the value
    // (the tweak) while you stayed within this particular option.
    SMOOTH_WITHIN_OPTION,
    // Smoothly interpolate everything.
    SMOOTH
  };
  
  // Constructs an enumeration with nothing in it
  LumiverseEnum(Mode mode = CENTER, int rangeMax = 255, InterpolationMode interpMode = SMOOTH_WITHIN_OPTION);

  // Constructs an enumeration with the given keys and range start values.
  // If nothing is passed in for default, the first key (sorted by range start)
  // will be set as default
  LumiverseEnum(map<string, int> keys, Mode mode = CENTER, int rangeMax = 255, string def = "", InterpolationMode = SMOOTH_WITHIN_OPTION);

  // Same as normal contsructor except mode is passed as a string.
  LumiverseEnum(map<string, int> keys, string mode, string interpMode, int rangeMax = 255, string def = "");

  // Copies an enumeration.
  LumiverseEnum(LumiverseEnum* other);

  // Copies an enumeration
  LumiverseEnum(const LumiverseEnum& other);

  // Copies a generic type
  LumiverseEnum(LumiverseType* other);

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

  // Sets the value of the enumeration numerically.
  // This will take a value and convert it to the proper enumeration
  // and tweak for the particular enum. If out of range, it will clamp.
  bool setVal(float val);

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

  // Sets the mode
  void setMode(Mode mode) { m_mode = mode; }

  // Returns the mode
  Mode getMode() { return m_mode; }

  // Sets the interpolation mode
  void setInterpMode(InterpolationMode mode) { m_interpMode = mode; }

  // Returns the interpolation mode
  InterpolationMode getInterpMode() { return m_interpMode; }

  // Does a linear interpolation based on the interpolation mode
  // The object this function is called on is treated as the left hand side
  // The interp mode for the left hand side takes precedence
  shared_ptr<LumiverseType> lerp(LumiverseEnum* rhs, float t);

  // Returns the exact value in the range given the active parameter and
  // the tweak value
  float getRangeVal();

  // Operator overrides woo
  void operator=(string name);
  void operator=(const LumiverseEnum& val);

private:
  // Initializes the enumeration. Called from constructors.
  void init(map<string, int> keys, string active, Mode mode, string def,
    float tweak, int rangeMax, InterpolationMode interpMode);

  // Intializes the enumeration. Copies over the values too if they already exist.
  void init(map<string, int> keys, string active, Mode mode, string def,
    float tweak, int rangeMax, InterpolationMode interpMode, map<int, string> vals);

  // Sets the tweak value based on the mode.
  void setTweakWithMode();

  // Returns the current mode as a string
  string modeAsString();

  // Converts a string to a mode.
  Mode stringToMode(string input);

  // Returns the interpolation mode as a string
  string interpModeAsString();

  // Converts a string to an interpolation mode
  InterpolationMode stringToInterpMode(string input);

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

  // Interpolation mode
  InterpolationMode m_interpMode;

  // Tweak determines where in the range we are. Middle = 0.5 and the
  // value can go from 0 to 1, with 0 being the front of the range, and 1 being the end.
  // This value can be adjusted to get the exact value in the desired range.
  float m_tweak;

  // Maximum value for the enumeration range. Typically this will be 255, but if
  // someone comes out with a different protocol not limited by DMX this will change.
  int m_rangeMax;

  // Protects the access of the m_nameToStart and m_startToName maps during assignment
  // and read (can't modify the map during a read)
  mutex m_enumMapMutex;
};

// Ops time
inline bool operator==(LumiverseEnum& a, LumiverseEnum& b) {
  if (a.getTypeName() != "enum" || b.getTypeName() != "enum")
    return false;

  return (a.getVal() == b.getVal() && a.getTweak() == b.getTweak());
}

inline bool operator!=(LumiverseEnum& a, LumiverseEnum& b) {
  return !(a == b);
}

// While it doesn't make too much sense to compare enums, you can compare
// where they are in their numeric range. That's what that </> ops will compare
inline bool operator<(LumiverseEnum& a, LumiverseEnum& b) {
  if (a.getTypeName() != "enum" || b.getTypeName() != "enum")
    return false;

  return a.getRangeVal() < b.getRangeVal();
}

inline bool operator>(LumiverseEnum& a, LumiverseEnum& b) {
  return b < a;
}

inline bool operator<=(LumiverseEnum& a, LumiverseEnum& b) {
  return !(a > b);
}

inline bool operator>=(LumiverseEnum& a, LumiverseEnum& b) {
  return !(a < b);
}

// Arithmetic overrides will be a little complicated as an enum.

#endif