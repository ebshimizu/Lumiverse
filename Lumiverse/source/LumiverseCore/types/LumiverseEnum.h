/*! \file LumiverseEnum.h
* \brief Stores an enumeration in Lumiverse
*/
#ifndef _LUMIVERSEENUM_
#define _LUMIVERSEENUM_

#pragma once

#include "../LumiverseType.h"
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace Lumiverse {
  /*! \brief Defines an enumeration in Lumiverse.
  *
  *
  * Moving lights often have various parameters that are enumerated
  * and encoded in a DMX range (ex. 10-19 on channel 6 = use gobo 1).
  * This class allows specification of settings by name or number
  * and also allows users to tweak the values in the range for controls
  * that may use the range as a speed control.
  * Comparison op overloads are located in the Lumiverse namespace.
  * Note that arithmetic ops have for the most part not been implemented
  * for LumiverseEnum since it doesn't make much sense to add two enumerations together.
  *
  * Enumerations ranges are stored in a 
  * map, with the name of the enumeration option associated with the first value in the range.
  * The side of the enumeration range is then just the difference between the first value
  * and the next value. 
  *
  * This is a bit confusing, so let's have an example. Let's say we have a simple light
  * that has an enumeration with 3 values. "Red" from 0-100, "Blue" from 101-200, and
  * "Green" from 201-255. This enumeration would be stored in a LumiverseEnum like this:
  * `[{"Red", 0}, {"Blue", 101}, {"Green", 201}]` and `m_rangeMax = 255`
  * With the CENTER option selected, setting the value of the enum to "Red" would
  * result in the numeric value 50. The `tweak` adjusts the value inside of an option.
  * So in this example, if you set the value of the enum to "Red" with tweak = 0.75,
  * the numeric value would be 75.
  * \sa Lumiverse, LumiverseType
  */
  class LumiverseEnum : public LumiverseType
  {
  public:
    /*!
    * \brief When you select an enumeration, mode determines where in the range
    *  the calculated value will fall.
    * 
    * Note that this is only used when a tweak value is not provided.
    * For example, if setting this enumeration to a value between 10 and 19
    * means the light puts in a blue filter, FIRST will set the actual value of
    * this enumeration to 10, CENTER will set it to 14.5, and LAST will set
    * it to 19.
    */
    enum Mode {
      FIRST,  /*!< Set value to start of range */
      CENTER, /*!< Set value to middle of the range (default) */
      LAST    /*!< Set value to end of range */
    };

    /*!
    * \brief Sets the interpolation mode when transitioning between two LumiverseEnums
    * 
    * Enumerations on lighting devices can be tricky to deal with in
    * transitions. This setting provides a way for LumiverseTypeUtils to
    * do a linear (or other) interpolation between two enums.
    * Default is SMOOTH_WITHIN_OPTION.
    * \sa lerp()
    */
    enum InterpolationMode {
      SNAP, /*!< Snap to new value immediately (no interpolation) */
      /*! This mode will interpolate within a particular setting.
      * For example, if you have a color wheel rotate option where values closer to
      * 128 are slow, and closer to 255 are fast, this would interpolate the value
      * (the tweak) while you stayed within this particular option, but snap between options.
      */
      SMOOTH_WITHIN_OPTION,
      SMOOTH /*!< Smoothly interpolate everything. */
    };

    /*!
    * \brief Constructs an enumeration with nothing in it.
    *
    * Not particularly useful.
    * \param mode Default enumeration value selection mode
    * \param rangeMax Maximum numerical range this enumeration can generate values for.
    * Generally this will be set to 255 since that's the maximum number of DMX channels.
    * If something else comes along and still uses this kind of enumeration, it can be adjusted.
    * \param interpMode Interpolation mode
    */
    LumiverseEnum(Mode mode = CENTER, int rangeMax = 255, InterpolationMode interpMode = SMOOTH_WITHIN_OPTION);

    /*!
    * \brief Constructs an enumeration with the given keys and range start values.
    * 
    * More useful form of the default constructor.
    * \param keys Map of enumeration options to the first value in their active range
    * \param mode Default enumeration value selection mode
    * \param rangeMax Maximum numerical range this enumeratino can generate values for
    * \param def Default enumeration option to go to when reset is called. If not set, defaults to first
    * enumeration option in the keys map.
    * \param interpMode Interpolation mode
    */
    LumiverseEnum(map<string, int> keys, Mode mode = CENTER, int rangeMax = 255, string def = "", InterpolationMode interpMode = SMOOTH_WITHIN_OPTION);

    /*!
    * \brief Constructs an enumeration with the given keys and range start values.
    *
    * Pretty much the same as LumiverseEnum(map<string, int>, Mode, int, string, InterpolationMode)
    * except that Mode and InterpolationMode must be explicitly specified as strings.
    * \param keys Map of enumeration options to the first value in their active range
    * \param mode Default enumeration value selection mode as a string
    * \param interpMode Interpolation mode as a string
    * \param rangeMax Maximum numerical range this enumeratino can generate values for
    * \param def Default enumeration option to go to when reset is called. If not set, defaults to first
    * enumeration option in the keys map.
    * \sa stringToMode(), stringToInterpMode()
    */
    LumiverseEnum(map<string, int> keys, string mode, string interpMode, int rangeMax = 255, string def = "");

    /*!
    * \brief Copies an enumeration
    * \param other Pointer to the other enum to copy
    */
    LumiverseEnum(LumiverseEnum* other);

    /*!
    * \brief Copies an enumeration
    * \param other The object to copy
    */
    LumiverseEnum(const LumiverseEnum& other);

    /*!
    * \brief Copies a generic type
    * 
    * Marginally useful. If other isn't actually a LumiverseEnum, the default constructor
    * will be called, and you'll have an empty enumeration.
    * \param other Pointer to the other object to copy
    */
    LumiverseEnum(LumiverseType* other);

    /*!
    * \brief Destructor fun times.
    */
    ~LumiverseEnum();

    /*!
    * \brief Says that this object is an enum.
    * \return String containing `"enum"`
    */
    virtual string getTypeName() { return "enum"; }

    /*!
    * \brief Resets the enum to default
    */
    virtual void reset();

    // Returns this object as a JSON node
    virtual JSONNode toJSON(string name);

    /*!
    * \brief Returns the current enumeration value as a string.
    *
    * Returns in the format: Enumeration (tweak_val)
    */
    virtual string asString();

    /*!
    * \brief Adds an option to the enumeration
    *
    * Will overwrite an existing set of values if it already exists.
    * \param name Name of the option
    * \param start Starting value in the option's range
    */
    void addVal(string name, int start);

    /*!
    * \brief Removes an option from the enumeration
    *
    * Nothing bad happens if the option doesn't exist in the enumeration.
    * It's technically still removed from the enumeration at the end of the function anyway.
    * \param name Name of the option to remove.
    */
    void removeVal(string name);

    /*!
    * \brief Sets the value of the enumeration
    *
    * Sets the value to the specified option, using the active Mode of the
    * enum to get the actual value.
    * \param name Name of the option
    * \return True if option exists, false otherwise.
    */
    bool setVal(string name);

    /*!
    * \brief Sets the value of the enumeration and the tweak value
    * 
    * The tweak is a value between 0 and 1 that proportionally adjusts where in the option's
    * range the value actually sits.
    * \param name Name of the option
    * \param tweak Tweak value between 0 and 1. Clamped if value given outside of that range.
    * \return True if option exists, false otherwise.
    */
    bool setVal(string name, float tweak);

    /*!
    \brief Sets the value of the enumeration and other options
    \param name Name of the option
    \param tweaj Tweak value between 0 and 1.
    \param enumMode The Mode of the enumeration
    \param interpMode Interpolation mode of the enumeration.
    */
    bool setVal(string name, float tweak, Mode enumMode, InterpolationMode interpMode);

    /*!
    * \brief Sets the value of the enumeration numerically.
    *
    * This will take a value and convert it to the proper enumeration option
    * and tweak for the particular enum. If out of range, it will clamp.
    * \param val Value to convert to an enumeration. Typically between 0 and 255.
    */
    bool setVal(float val);

    /*!
    * \brief Tweaks the value of the enum
    *
    * Shifts it proportionally in the active option's range.
    * \param tweak Tweak value between 0 and 1
    */
    void setTweak(float tweak);

    /*!
    * \brief Sets the default value of the enumeration
    *
    * Note that this function doesn't check to see if the new default
    * value actually exists. It should probably do that and will probably be updated
    * in a new version soon.
    * \param name New default option name
    */
    void setDefault(string name) { m_default = name; }

    /*!
    * \brief Gets the current state of the enumeration
    * \return Active enumeration option
    */
    string getVal() { return m_active; }

    /*!
    \brief Gets the first value in the active range.
    */
    int getValIndex() { return m_nameToStart[m_active]; }

    /*!
    * \brief Gets the tweak value for the enumeration
    * \return The current tweak value
    */
    float getTweak() { return m_tweak; }

    /*!
    * \brief Gets the default value
    * \return Default option as a string
    */
    string getDefault() { return m_default; }

    /*!
    * \brief Sets the mode
    * \param mode Default value selection mode
    */
    void setMode(Mode mode) { m_mode = mode; }

    /*!
    * \brief Returns the mode
    * \return Currently active value selection mode
    */
    Mode getMode() { return m_mode; }

    /*!
    * \brief Sets the interpolation mode
    * \param mode New interpoplation mode
    */
    void setInterpMode(InterpolationMode mode) { m_interpMode = mode; }

    /*!
    * \brief Returns the interpolation mode
    * \return Currently active interpolation mode
    */
    InterpolationMode getInterpMode() { return m_interpMode; }

    /*!
    \brief Returns the maximum numeric value this LumiverseEnum can take
    */
    int getRangeMax() { return m_rangeMax; }

    /*!
    \brief Sets the maximum numeric value this LumiverseEnum can take.

    Setting this to a value below the highest valued option in the LumiverseEnum results in undefined behavior.
    */
    void setRangeMax(int newMax) { m_rangeMax = newMax; }

    /*!
    * \brief Does a linear interpolation based on the interpolation mode.
    *
    * The object this function is called on is treated as the left hand side.
    * The interp mode for the left hand side takes precedence.
    * If the enumerations don't have the same options, this function will have some weird
    * behavior, but otherwise will do the interpolation according to the numeric values.
    * \param rhs Initial value
    * \param t Value between 0 and 1. When `t = 0`, this function returns the value of this object. When
    * `t = 1`, this function returns the value of rhs.
    * \return New LumiverseEnum containing the result of the interpolation.
    * \sa setVal(float)
    */
    shared_ptr<LumiverseType> lerp(LumiverseEnum* rhs, float t);

    /*!
    * \brief Returns the exact value in the range given the active parameter and
    * the tweak value
    * \return Numeric value of the enum.
    */
    float getRangeVal();

    /*!
    \brief Sets the value of the LumiverseFloat proportionally
    */
    void setValAsPercent(float val) { setVal(val * (m_rangeMax - getLowestStartValue()) + getLowestStartValue()); }

    /*!
    \brief Gets the value of the enum in terms of a percentage.

    Example: if the range of the enum is [0,255] and the current numeric value is 127,
    this returns .498
    */
    float asPercent();

    // Operator overrides woo
    void operator=(string name);
    void operator=(const LumiverseEnum& val);

    /*!
    * \brief Returns true if the current value of the enumeration is equal to the default.
    */
    virtual bool isDefault();

    /*!
    \brief Gets a list of all the possible values the enumeration can take

    This list is sorted by enumeration option start value.
    */
    vector<string> getVals();

    /*!
    \brief Returns a reference to the map of values to the start of their range.
    */
    const map<string, int>& getValsToStart() { return m_nameToStart; }

    /*!
    \brief Returns a reference to the map of range starts to values
    */
    const map<int, string>& getStartToVals() { return m_startToName; }

    /*!
    \brief Gets the highest start value for an enumeration option.

    This should be a value less than m_rangeMax if the enumeration is formatted correctly.
    \return Value of the key with the higest starting value. -1 if there are no keys currently in the enumeration.
    */
    int getHighestStartValue();

    /*!
    \brief Gets the lowest start value for an enumeration option.
    */
    int getLowestStartValue();

  private:
    /*!
    * \brief Initializes the enumeration. Called from constructors.
    * 
    * Parameters are pretty much the same here as in the constructor.
    * \sa LumiverseEnum()
    */
    void init(map<string, int> keys, string active, Mode mode, string def,
      float tweak, int rangeMax, InterpolationMode interpMode);

    /*!
    * \brief Intializes the enumeration. Copies over the values too if they already exist.
    *
    * Primarily used to copy data from one enum to another.
    * \param keys Map of enumeration options to the first value in their active range
    * \param active Active option
    * \param mode Default enumeration value selection mode
    * \param def Default enumeration option to go to when reset is called. If not set, defaults to first
    * \param tweak Active tweak value
    * \param rangeMax Maximum numerical range this enumeratino can generate values for
    * enumeration option in the keys map.
    * \param interpMode Interpolation mode as a string
    * \param vals Reverse map of first value in active range to name of enumeration option
    */
    void init(map<string, int> keys, string active, Mode mode, string def,
      float tweak, int rangeMax, InterpolationMode interpMode, map<int, string> vals);

    /*!
    * \brief Sets the tweak value based on the mode.
    */
    void setTweakWithMode();

    /*!
    * \brief Returns the current mode as a string
    * 
    * Primarily used for internal serialization.
    * \return Mode in string form
    */
    string modeAsString();

    /*!
    * \brief Converts a string to a mode.
    *
    * Primarily used for internal deserialization
    * \param input String to convert
    * \return Mode corresponding to input string. Returns CENTER if an invalid string is passed in.
    */
    Mode stringToMode(string input);

    /*!
    * \brief Returns the interpolation mode as a string
    *
    * Primarily used for internal serialization
    * \return Interpolation Mode converted to a string
    */
    string interpModeAsString();

    /*!
    * \brief Converts a string to an interpolation mode
    *
    * Primarily used for internal deserialization
    * \param input String to convert
    * \return InterpolationMode corresponding to input string. Returns SMOOTH_WITHIN_OPTION
    * if invalid string passed in.
    */
    InterpolationMode stringToInterpMode(string input);

    /*! \brief The active enumeration option */
    string m_active;

    /*! \brief The default option for the enumeration */
    string m_default;

    /*!
    * \brief Map of the name of the enumeration option to the start of the range.
    * 
    * The start of the range is typically stated as a DMX value (0-255)
    */
    map<string, int> m_nameToStart;

    /*! \brief Maps the start of a range to the name of the enumeration option. */
    map<int, string> m_startToName;

    /*! \brief Enumeration mode */
    Mode m_mode;

    /*! \brief Interpolation mode */
    InterpolationMode m_interpMode;

    /*!
    * \brief Tweak determines where in the range we are.
    *
    * Middle = 0.5 and the
    * value can go from 0 to 1, with 0 being the front of the range, and 1 being the end.
    * This value can be adjusted to get the exact value in the active option.
    */
    float m_tweak;

    /*!
    * \brief Maximum value for the enumeration range.
    *
    * Typically this will be 255, but if
    * someone comes out with a different protocol not limited by DMX this will change.
    */
    int m_rangeMax;

    /*! \brief Protects the access of the m_nameToStart and m_startToName maps during assignment
    * and read (can't modify the map during a read) */
    mutex m_enumMapMutex;
  };

  // Ops time

  /*!
  * \brief Compares two enums for equality.
  *
  * Tests if the current options are the same and if the current tweak values
  * are the same. Does not check to see if the two enums have the same options.
  */
  inline bool operator==(LumiverseEnum& a, LumiverseEnum& b) {
    if (a.getTypeName() != "enum" || b.getTypeName() != "enum")
      return false;

    return (a.getVal() == b.getVal() && a.getTweak() == b.getTweak());
  }

  inline bool operator!=(LumiverseEnum& a, LumiverseEnum& b) {
    return !(a == b);
  }

  /*!
  * \brief Compares the numeric values of two enumerations 
  *
  * While it doesn't make too much sense to compare enums, you can compare
  * where they are in their numeric range. That's what that </> ops will compare 
  */
  inline bool operator<(LumiverseEnum& a, LumiverseEnum& b) {
    if (a.getTypeName() != "enum" || b.getTypeName() != "enum")
      return false;

    return a.getRangeVal() < b.getRangeVal();
  }

  /*! \sa operator<(LumiverseEnum&, LumiverseEnum&) */
  inline bool operator>(LumiverseEnum& a, LumiverseEnum& b) {
    return b < a;
  }

  /*! \sa operator<(LumiverseEnum&, LumiverseEnum&) */
  inline bool operator<=(LumiverseEnum& a, LumiverseEnum& b) {
    return !(a > b);
  }

  /*! \sa operator<(LumiverseEnum&, LumiverseEnum&) */
  inline bool operator>=(LumiverseEnum& a, LumiverseEnum& b) {
    return !(a < b);
  }

  // Arithmetic overrides will be a little complicated or non-existant as an enum.
}

#endif