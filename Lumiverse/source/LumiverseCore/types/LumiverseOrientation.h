/*! \file LumiverseOrientation.h
* \brief Stores a orientation value in Lumiverse
*/
#ifndef _LumiverseORIENTATION_H_
#define _LumiverseORIENTATION_H_
#pragma once

#include "../LumiverseType.h"
#include <string>
#include <stdio.h>
#include <unordered_map>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Lumiverse {
  /*! \brief Enumeration indicating the type of angular unit stored. */
  enum ORIENTATION_UNIT {
    DEGREE, RADIAN
  };

#ifdef USE_C11_MAPS
  /*! \brief Converts orientation to string */
  static unordered_map<int, string> oriToString = {
    { DEGREE, "degree" }, { RADIAN, "radian" }
  };

  /*! \biref Converts string to orientation. */
  static unordered_map<string, int> stringToOri = {
    { "degree", DEGREE }, { "radian", RADIAN }
  };
#else
  static string oriToString(ORIENTATION_UNIT u) {
		switch(u) {
		case DEGREE:
			return "degree";
		case RADIAN:
			return "radian";
		default:
			return "";
		}
  }

  static ORIENTATION_UNIT stringToOri(string u) {
		if (u == "degree") return DEGREE;
		if (u == "radian") return RADIAN;

		return RADIAN;
  }
#endif

  /*!
  * \brief Defines an orientation in Lumiverse
  *
  * This class allows limits to be set on the minimum and maximum values for
  * the variable in question.
  * Overloads for comparison ops and arithmetic ops are located in Lumiverse namespace.
  * Orientations are essentially special LumiverseFloat objects that have a few built in
  * functions for getting rotation specfic data.
  * \sa Lumiverse
  */
  class LumiverseOrientation : LumiverseType
  {
  public:
    /*!
    * \brief Constructs an orientation, default value is 0.
    * 
    * \param val Initial value
    * \param def Default value. When reset() is called, `val` will be set to `def`.
    * \param max Maximum allowed value
    * \param min Minimum allowed value
    * \sa reset()
    */
	  LumiverseOrientation(float val = 0.0f, ORIENTATION_UNIT unit = DEGREE, float def = 0.0f, float max = 360.0f, float min = 0.0f);

    /*!
    * \brief Constructs an orientation with the contents of a different orientation
    * \param other The other object to copy from
    */
    LumiverseOrientation(LumiverseOrientation* other);

    /*!
    * \brief Constructs an orientation by copying from a generic LumiverseType
    *
    * If the other object isn't actually an orientation, this function will initialize with default values.
    * \param other The other object to copy from.
    */
    LumiverseOrientation(LumiverseType* other);

    /*!
    * \brief Destroys the orientation.
    */
    ~LumiverseOrientation();

    /*!
    * \brief Says that this object is an orientation.
    * \return String with contents: `"orientation"`
    */
    virtual string getTypeName() { return "orientation"; }

    // Override for =
    void operator=(float val);
    void operator=(LumiverseOrientation val);

    // Arithmetic overrides
    LumiverseOrientation& operator+=(float val);
    LumiverseOrientation& operator+=(LumiverseOrientation& val);

    LumiverseOrientation& operator-=(float val);
    LumiverseOrientation& operator-=(LumiverseOrientation& val);

    LumiverseOrientation& operator*=(float val);
    LumiverseOrientation& operator*=(LumiverseOrientation& val);

    LumiverseOrientation& operator/=(float val);
    LumiverseOrientation& operator/=(LumiverseOrientation& val);

    /*! \brief Gets the value of the orientation 
    * \return Value of the object
    */
    float getVal() { return m_val; }

    /*!
    * \brief Sets the value of the orientation
    * \param val New value
    */
    void setVal(float val, ORIENTATION_UNIT unit = DEGREE) { m_val = asUnit(unit, val, m_unit); clamp(); }

    /*!
    * \brief Set unit
    * \param val New unit
    */
    void setUnit(ORIENTATION_UNIT unit);

    /*!
    * \brief Get the unit
    * \return Name of unit
    */
    ORIENTATION_UNIT getUnit() { return m_unit; }

    /*!
    * \brief Set maximum value
    * \param val New maximum value
    */
    void setMax(float val, ORIENTATION_UNIT unit = DEGREE) { m_max = asUnit(unit, val, m_unit); }
    
    /*!
    * \brief Get the maximum value
    * \return Maximum value for the float
    */
    float getMax() { return m_max; }

    /*!
    * \brief Set miniumum value
    * \param val New minimum value
    */
    void setMin(float val, ORIENTATION_UNIT unit = DEGREE) { m_min = asUnit(unit, val, m_unit); }
    
    /*!
    * \brief Get the minimum value
    * \return Minimum value for the orientation in local units
    */
    float getMin() { return m_min; }

    /*!
    * \brief Set the default value for the orientation
    * \param val New default value
    */
    void setDefault(float val, ORIENTATION_UNIT unit = DEGREE) { m_default = asUnit(unit, val, m_unit); }
    
    /*!
    * \brief Gets the default value for the orientation
    * \return Default value in local units
    */
    float getDefault() { return m_default; }

    /*!
    * \brief Resets the value to the default value
    */
    virtual void reset() { m_val = m_default; }

    /*!
    \brief Sets the value of the LumiverseOrientation proportionally
    */
    void setValAsPercent(float val) { m_val = val * (m_max - m_min) + m_min; }
    
    /*!
    * \brief Returns the value of this orientation as a percentage
    * \return Returns the value: `m_val / (m_max - m_min)`
    */
    float asPercent();

    /*!
    * \brief Returns the value of this orientation with the specified units.
    */
    float valAsUnit(ORIENTATION_UNIT unit) { return asUnit(m_unit, m_val, unit); }

    /*!
    * \brief Returns the max value of the orientation with the specified units.
    */
    float maxAsUnit(ORIENTATION_UNIT unit) { return asUnit(m_unit, m_max, unit); }

    /*!
    * \brief Returns the min value of the orientation with the specified units.
    */
    float minAsUnit(ORIENTATION_UNIT unit) { return asUnit(m_unit, m_min, unit); }

    /*!
    * \brief Returns the default value of the orientation with the specified units.
    */
    float defaultAsUnit(ORIENTATION_UNIT unit) { return asUnit(m_unit, m_default, unit); }

    // Converts a float to a JSON object with specified name.
    virtual JSONNode toJSON(string name);

    /*!
    * \brief Returns the value of the LumiverseOrientation as a string
    *
    * Precision goes up to 4 decimal places.
    */
    virtual string asString();

    virtual bool isDefault();

  private:
    /*!
    * \brief Ensures that the value of this float is between min and max.
    */
    inline void clamp();

    /*!
    * \brief Returns the specified value as the specified unit.
    *
    * In the event that the orientation is already using the specified units,
    * the same value as getVal() will be returned.
    * \param valUnit Units of val.
    * \param val Quantity to convert
    * \param targetUnit desired final units
    */
    float asUnit(ORIENTATION_UNIT valUnit, float val, ORIENTATION_UNIT targetUnit);

    /*!
    * \brief the value of this object
    */
    float m_val;

    /*! \brief Default value for this orientation. */
    float m_default;

    /*! \brief Maximum value for the orientation (default 1.0) */
    float m_max;

    /*! \brief Minimum value for the orientation (default 0.0) */
    float m_min;

    /*! \brief Indicates the type of angle measurement used in the object. */
    ORIENTATION_UNIT m_unit;
  };

  // Ops ops ops all overloaded woo

  // Compares two LumiverseOrientations. Uses normal float comparison
  inline bool operator==(LumiverseOrientation& a, LumiverseOrientation& b) {
    if (a.getTypeName() != "orientation" || b.getTypeName() != "orientation")
      return false;

    // Equality/inequality shouldn't change based on a unit conversion.
    return a.getVal() == b.valAsUnit(a.getUnit());
  }

  inline bool operator!=(LumiverseOrientation& a, LumiverseOrientation& b) {
    return !(a == b);
  }

  // LumiverseOrientation uses the normal < op for floats.
  inline bool operator<(LumiverseOrientation& a, LumiverseOrientation& b) {
    if (a.getTypeName() != "orientation" || b.getTypeName() != "orientation")
      return false;

    // Equality/inequality shouldn't change based on a unit conversion.
    return a.getVal() < b.valAsUnit(a.getUnit());
  }

  inline bool operator>(LumiverseOrientation& a, LumiverseOrientation& b) {
    return b < a;
  }

  inline bool operator<=(LumiverseOrientation& a, LumiverseOrientation& b) {
    return !(a > b);
  }

  inline bool operator>=(LumiverseOrientation& a, LumiverseOrientation b) {
    return !(a < b);
  }

  // Arithmetic overrides
  inline LumiverseOrientation operator+(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val += rhs;
    return val;
  }

  //inline LumiverseOrientation operator+(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
  //  LumiverseOrientation val = LumiverseOrientation(lhs);
  //  val += rhs.valAsUnit(val.getUnit());
  //  return val;
  //}

  // Apparently clang needs this form of the overload to make it happy
  inline LumiverseOrientation operator+(LumiverseOrientation lhs, LumiverseOrientation rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val += rhs.valAsUnit(val.getUnit());
    return val;
  }

  inline LumiverseOrientation operator-(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val -= rhs;
    return val;
  }

  inline LumiverseOrientation operator-(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val -= rhs.valAsUnit(val.getUnit());
    return val;
  }

  inline LumiverseOrientation operator*(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val *= rhs;
    return val;
  }

  inline LumiverseOrientation operator*(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val *= rhs.valAsUnit(val.getUnit());;
    return val;
  }

  inline LumiverseOrientation operator/(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val /= rhs;
    return val;
  }

  inline LumiverseOrientation operator/(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val /= rhs.valAsUnit(val.getUnit());
    return val;
  }
}

#endif