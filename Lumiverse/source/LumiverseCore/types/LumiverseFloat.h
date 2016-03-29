/*! \file LumiverseFloat.h
* \brief Stores a floating point value in Lumiverse
*/
#ifndef _LumiverseFLOAT_H_
#define _LumiverseFLOAT_H_
#pragma once

#include "../LumiverseType.h"
#include <string>
#include <stdio.h>

namespace Lumiverse {
  /*!
  * \brief Defines a float in Lumiverse
  *
  * This class allows limits to be set on the minimum and maximum values for
  * the variable in question.
  * Overloads for comparison ops and arithmetic ops are located in Lumiverse namespace.
  * \sa Lumiverse
  */
  class LumiverseFloat : public LumiverseType
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
    LumiverseFloat(float val = 0.0f, float def = 0.0f, float max = 1.0f, float min = 0.0f);

    /*!
    * \brief Constructs a float with the contents of a different float
    * \param other The other object to copy from
    */
    LumiverseFloat(LumiverseFloat* other);

    /*!
    * \brief Constructs a float by copying from a generic LumiverseType
    *
    * If the other object isn't actually a float, this function will initialize with default values.
    * \param other The other object to copy from.
    */
    LumiverseFloat(LumiverseType* other);

    /*!
    * \brief Destroys the float.
    */
    ~LumiverseFloat();

    /*!
    * \brief Says that this object is a float.
    * \return String with contents: `"float"`
    */
    virtual string getTypeName() { return "float"; }

    // Override for =
    void operator=(float val);
    void operator=(LumiverseFloat val);

    // Arithmetic overrides
    LumiverseFloat& operator+=(float val);
    LumiverseFloat& operator+=(LumiverseFloat& val);

    LumiverseFloat& operator-=(float val);
    LumiverseFloat& operator-=(LumiverseFloat& val);

    LumiverseFloat& operator*=(float val);
    LumiverseFloat& operator*=(LumiverseFloat& val);

    LumiverseFloat& operator/=(float val);
    LumiverseFloat& operator/=(LumiverseFloat& val);

    /*! \brief Gets the value of the float 
    * \return Value of the object
    */
    float getVal() { return m_val; }

    /*!
    * \brief Sets the value of the float
    * \param val New value
    */
    void setVal(float val) { m_val = val; clamp(); }

    /*!
    \brief Sets values for all float params.
    */
    void setVals(float val, float def, float min, float max);

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
    virtual void reset();

    /*!
    \brief Sets the value of the LumiverseFloat proportionally
    */
    void setValAsPercent(float val);

    /*!
    * \brief Returns the value of this float as a percentage
    * \return Returns the value: `m_val / (m_max - m_min)`
    */
    float asPercent();

    // Converts a float to a JSON object with specified name.
    virtual JSONNode toJSON(string name);

    /*!
    * \brief Returns the value of the LumiverseFloat as a string
    *
    * Precision goes up to 4 decimal places.
    */
    virtual string asString();

    virtual bool isDefault();

  private:
    /*!
    * \brief Ensures that the value of this float is between min and max.
    */
    void clamp();

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
  };

  // Ops ops ops all overloaded woo

  // Compares two LumiverseFloats. Uses normal float comparison
  inline bool operator==(LumiverseFloat& a, LumiverseFloat& b) {
    if (a.getTypeName() != "float" || b.getTypeName() != "float")
      return false;

    return a.getVal() == b.getVal();
  }

  inline bool operator==(LumiverseFloat& a, float b) {
    if (a.getTypeName() != "float")
      return false;

    return a.getVal() == b;
  }

  inline bool operator!=(LumiverseFloat& a, LumiverseFloat& b) {
    return !(a == b);
  }

  inline bool operator!=(LumiverseFloat& a, float b) {
    return !(a == b);
  }

  // LumiverseFloat uses the normal < op for floats.
  inline bool operator<(LumiverseFloat& a, LumiverseFloat& b) {
    if (a.getTypeName() != "float" || b.getTypeName() != "float")
      return false;

    return a.getVal() < b.getVal();
  }

  inline bool operator<(LumiverseFloat& a, float b) {
    if (a.getTypeName() != "float")
      return false;

    return a.getVal() < b;
  }

  inline bool operator<(float a, LumiverseFloat& b) {
    if (b.getTypeName() != "float")
      return false;

    return a < b.getVal();
  }

  inline bool operator>(LumiverseFloat& a, LumiverseFloat& b) {
    return b < a;
  }

  inline bool operator>(LumiverseFloat& a, float b) {
    return b < a;
  }

  inline bool operator<=(LumiverseFloat& a, LumiverseFloat& b) {
    return !(a > b);
  }

  inline bool operator<=(LumiverseFloat& a, float b) {
    return !(a > b);
  }

  inline bool operator>=(LumiverseFloat& a, LumiverseFloat b) {
    return !(a < b);
  }

  inline bool operator>=(LumiverseFloat& a, float b) {
    return !(a < b);
  }

  // Arithmetic overrides
  inline LumiverseFloat operator+(LumiverseFloat& lhs, float rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val += rhs;
    return val;
  }

  //inline LumiverseFloat operator+(LumiverseFloat& lhs, LumiverseFloat& rhs) {
  //  LumiverseFloat val = LumiverseFloat(lhs);
  //  val += rhs;
  //  return val;
  //}

  // Apparently clang needs this form of the overload to make it happy
  inline LumiverseFloat operator+(LumiverseFloat lhs, LumiverseFloat rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val += rhs;
    return val;
  }

  inline LumiverseFloat operator-(LumiverseFloat& lhs, float rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val -= rhs;
    return val;
  }

  inline LumiverseFloat operator-(LumiverseFloat& lhs, LumiverseFloat& rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val -= rhs;
    return val;
  }

  inline LumiverseFloat operator*(LumiverseFloat& lhs, float rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val *= rhs;
    return val;
  }

  inline LumiverseFloat operator*(LumiverseFloat& lhs, LumiverseFloat& rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val *= rhs;
    return val;
  }

  inline LumiverseFloat operator/(LumiverseFloat& lhs, float rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val /= rhs;
    return val;
  }

  inline LumiverseFloat operator/(LumiverseFloat& lhs, LumiverseFloat& rhs) {
    LumiverseFloat val = LumiverseFloat(lhs);
    val /= rhs;
    return val;
  }
}

#endif