#ifndef _LumiverseFLOAT_H_
#define _LumiverseFLOAT_H_
#pragma once

#include "../LumiverseType.h"
#include <string>

class LumiverseType;

// Defines a float in Lumiverse
// Typically this will range from 0 to 1 inclusive, but it doesn't
// have to really.
class LumiverseFloat : LumiverseType
{
public:
  // Constructs a float, default value is 0.
  LumiverseFloat(float val = 0.0f, float def = 0.0f, float max = 1.0f, float min = 0.0f);

  // Copies a float
  LumiverseFloat(LumiverseFloat* other);

  // Copies a generic Lumiverse Type
  LumiverseFloat(LumiverseType* other);

  // Destroys the float.
  ~LumiverseFloat();

  // Says that this object is a float.
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

  // Gets the value
  float getVal() { return m_val; }

  // Sets the value
  void setVal(float val) { m_val = val; }
  
  // Set maximum value
  void setMax(float val) { m_max = val; }
  float getMax() { return m_max; }

  // Set miniumum value
  void setMin(float val) { m_min = val; }
  float getMin() { return m_min; }

  // Set the default value
  void setDefault(float val) { m_default = val; }
  float getDefault() { return m_default; }

  // Resets the value to default
  virtual void reset() { m_val = m_default; }

  // Returns the value of this float as a percentage in the range [min, max]
  float asPercent();

  // Converts a float to a JSON object with specified name.
  virtual JSONNode toJSON(string name);
  
private:
  // Ensures that the value of this float is between min and max.
  inline void clamp();

  // Um, it's a float.
  float m_val;

  // Default value for this float.
  float m_default;

  // Maximum value for the float (default 1.0)
  float m_max;

  // Minimum value for the float (default 0.0)
  float m_min;
};

// Ops ops ops all overloaded woo

// Compares two LumiverseFloats. Uses normal float comparison
inline bool operator==(LumiverseFloat& a, LumiverseFloat& b) {
  if (a.getTypeName() != "float" || b.getTypeName() != "float")
    return false;

  return a.getVal() == b.getVal();
}

inline bool operator!=(LumiverseFloat& a, LumiverseFloat& b) {
  return !(a == b);
}

// LumiverseFloat uses the normal < op for floats.
inline bool operator<(LumiverseFloat& a, LumiverseFloat& b) {
  if (a.getTypeName() != "float" || b.getTypeName() != "float")
    return false;

  return a.getVal() < b.getVal();
}

inline bool operator>(LumiverseFloat& a, LumiverseFloat& b) {
  return b < a;
}

inline bool operator<=(LumiverseFloat& a, LumiverseFloat& b) {
  return !(a > b);
}

inline bool operator>=(LumiverseFloat& a, LumiverseFloat b) {
  return !(a < b);
}

// Arithmetic overrides
inline LumiverseFloat operator+(LumiverseFloat& lhs, float rhs) {
  LumiverseFloat val = LumiverseFloat(lhs);
  val += rhs;
  return val;
}

inline LumiverseFloat operator+(LumiverseFloat& lhs, LumiverseFloat& rhs) {
  LumiverseFloat val = LumiverseFloat(lhs);
  val += rhs;
  return val;
}

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

#endif