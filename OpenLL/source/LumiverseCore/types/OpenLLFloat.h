#ifndef _OPENLLFLOAT_H_
#define _OPENLLFLOAT_H_
#pragma once

#include "../OpenLLType.h"
#include <string>

// Defines a float in OpenLL
// Typically this will range from 0 to 1 inclusive, but it doesn't
// have to really.
class OpenLLFloat : OpenLLType
{
public:
  // Constructs a float, default value is 0.
  OpenLLFloat(float val = 0.0f, float def = 0.0f);

  // Destroys the float.
  ~OpenLLFloat();

  // Says that this object is a float.
  virtual string getTypeName() { return "float"; }

  // Override for =
  void operator=(float val) { m_val = val; }

  // Gets the value
  float getVal() { return m_val; }

  // Sets the value
  void setVal(float val) { m_val = val; }

  // Resets the value to default
  virtual void reset() { m_val = m_default; }

  // Converts a float to a JSON object with specified name.
  virtual JSONNode toJSON(string name);
  
private:
  // Um, it's a float.
  float m_val;

  // Default value for this float.
  float m_default;
};

// Ops ops ops all overloaded woo

// Compares two OpenLLFloats. Uses normal float comparison
inline bool operator==(OpenLLFloat& a, OpenLLFloat& b) {
  if (a.getTypeName() != "float" || b.getTypeName() != "float")
    return false;

  return a.getVal() == b.getVal();
}

inline bool operator!=(OpenLLFloat& a, OpenLLFloat& b) {
  return !(a == b);
}

// OpenLLFloat uses the normal < op for floats.
inline bool operator<(OpenLLFloat& a, OpenLLFloat& b) {
  if (a.getTypeName() != "float" || b.getTypeName() != "float")
    return false;

  return a.getVal() < b.getVal();
}

inline bool operator>(OpenLLFloat& a, OpenLLFloat& b) {
  return b < a;
}

inline bool operator<=(OpenLLFloat& a, OpenLLFloat& b) {
  return !(a > b);
}

inline bool operator>=(OpenLLFloat& a, OpenLLFloat b) {
  return !(a < b);
}

#endif