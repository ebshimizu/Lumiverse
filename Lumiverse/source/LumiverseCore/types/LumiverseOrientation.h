/*! \file LumiverseOrientation.h
* \brief Stores a orientation value in Lumiverse
*/
#ifndef _LumiverseORIENTATION_H_
#define _LumiverseORIENTATION_H_
#pragma once

#include "../LumiverseType.h"
#include <string>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Lumiverse {
  /*!
  * \brief Defines a float in Lumiverse
  *
  * This class allows limits to be set on the minimum and maximum values for
  * the variable in question.
  * Overloads for comparison ops and arithmetic ops are located in Lumiverse namespace.
  * \sa Lumiverse
  */
  class LumiverseOrientation : LumiverseType
  {
  public:
    /*!
    * \brief Constructs a float, default value is 0.
    * 
    * \param val Initial value
    * \param def Default value. When reset() is called, `val` will be set to `def`.
    * \param max Maximum allowed value
    * \param min Minimum allowed value
    * \sa reset()
    */
	  LumiverseOrientation(float val = 0.0f, string unit = "degree", float def = 0.0f, float max = 0.0f, float min = 360.0f);

    /*!
    * \brief Constructs a float with the contents of a different float
    * \param other The other object to copy from
    */
    LumiverseOrientation(LumiverseOrientation* other);

    /*!
    * \brief Constructs a float by copying from a generic LumiverseType
    *
    * If the other object isn't actually a float, this function will initialize with default values.
    * \param other The other object to copy from.
    */
    LumiverseOrientation(LumiverseType* other);

    /*!
    * \brief Destroys the float.
    */
    ~LumiverseOrientation();

    /*!
    * \brief Says that this object is a float.
    * \return String with contents: `"float"`
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

    /*! \brief Gets the value of the float 
    * \return Value of the object
    */
    float getVal() { return m_val; }

    /*!
    * \brief Sets the value of the float
    * \param val New value
    */
    void setVal(float val) { m_val = val; }

	/*!
	* \brief Set unit
	* \param val New unit name
	*/
	void setUnit(string unit) { m_unit = unit; }

	/*!
	* \brief Get the unit
	* \return Name of unit
	*/
	string getUnit() { return m_unit; }

    /*!
    * \brief Set maximum value
    * \param val New maximum value
    */
    void setMax(float val) { m_max = val; }
    
    /*!
    * \brief Get the maximum value
    * \return Maximum value for the float
    */
    float getMax() { return m_max; }

    /*!
    * \brief Set miniumum value
    * \param val New minimum value
    */
    void setMin(float val) { m_min = val; }
    
    /*!
    * \brief Get the minimum value
    * \return Minimum value for the float
    */
    float getMin() { return m_min; }

    /*!
    * \brief Set the default value for the float
    * \param val New default value
    */
    void setDefault(float val) { m_default = val; }
    
    /*!
    * \brief Gets the default value for the float
    * \return Default value
    */
    float getDefault() { return m_default; }

    /*!
    * \brief Resets the value to the default value
    */
    virtual void reset() { m_val = m_default; }

    /*!
    * \brief Returns the value of this float as a percentage
    * \return Returns the value: `m_val / (m_max - m_min)`
    */
    float asPercent();

	float asUnit(string unit);

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
    * \brief the value of this object
    */
    float m_val;

    /*! \brief Default value for this float. */
    float m_default;

    /*! \brief Maximum value for the float (default 1.0) */
    float m_max;

    /*! Minimum value for the float (default 0.0) */
    float m_min;

	string m_unit;
  };

  // Ops ops ops all overloaded woo

  // Compares two LumiverseOrientations. Uses normal float comparison
  inline bool operator==(LumiverseOrientation& a, LumiverseOrientation& b) {
    if (a.getTypeName() != "orientation" || b.getTypeName() != "orientation")
      return false;

	if (a.getUnit() == b.getUnit())
		return a.getVal() == b.getVal();
	return a.getVal() == b.asUnit(a.getUnit());
  }

  inline bool operator!=(LumiverseOrientation& a, LumiverseOrientation& b) {
    return !(a == b);
  }

  // LumiverseOrientation uses the normal < op for floats.
  inline bool operator<(LumiverseOrientation& a, LumiverseOrientation& b) {
    if (a.getTypeName() != "orientation" || b.getTypeName() != "orientation")
      return false;

	if (a.getUnit() == b.getUnit())
		return a.getVal() < b.getVal();
	return a.getVal() < b.asUnit(a.getUnit());
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

  inline LumiverseOrientation operator+(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val += rhs;
    return val;
  }

  // Apparently clang needs this form of the overload to make it happy
  inline LumiverseOrientation operator+(LumiverseOrientation lhs, LumiverseOrientation rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val += rhs;
    return val;
  }

  inline LumiverseOrientation operator-(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val -= rhs;
    return val;
  }

  inline LumiverseOrientation operator-(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val -= rhs;
    return val;
  }

  inline LumiverseOrientation operator*(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val *= rhs;
    return val;
  }

  inline LumiverseOrientation operator*(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val *= rhs;
    return val;
  }

  inline LumiverseOrientation operator/(LumiverseOrientation& lhs, float rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val /= rhs;
    return val;
  }

  inline LumiverseOrientation operator/(LumiverseOrientation& lhs, LumiverseOrientation& rhs) {
    LumiverseOrientation val = LumiverseOrientation(lhs);
    val /= rhs;
    return val;
  }
}

#endif