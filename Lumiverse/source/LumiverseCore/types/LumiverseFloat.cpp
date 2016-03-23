#include "LumiverseFloat.h"
namespace Lumiverse {
// This is really not interesting huh.

LumiverseFloat::LumiverseFloat(float val, float def, float max, float min) :
  m_val(val), m_default(def), m_max(max), m_min(min) { }

LumiverseFloat::LumiverseFloat(LumiverseFloat* other) :
  m_val(other->m_val), m_default(other->m_default), m_max(other->m_max), m_min(other->m_min) { }

LumiverseFloat::LumiverseFloat(LumiverseType* other) {
  if (other->getTypeName() != "float") {
    // If this isn't actually a float, use defaults.
    m_val = 0.0f;
    m_default = 0.0f;
    m_max = 1.0f;
    m_min = 0.0f;
  }
  else {
    LumiverseFloat* otherFloat = (LumiverseFloat*)other;
    m_val = otherFloat->m_val;
    m_default = otherFloat->m_default;
    m_max = otherFloat->m_max;
    m_min = otherFloat->m_min;
  }
}

LumiverseFloat::~LumiverseFloat() { }

JSONNode LumiverseFloat::toJSON(string name) {
  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));
  node.push_back(JSONNode("max", m_max));
  node.push_back(JSONNode("min", m_min));

  return node;
}

string LumiverseFloat::asString() {
  char buf[32];
#ifndef _MSC_VER
  snprintf(buf, 31, "%.2f", m_val);
#else
  _snprintf_s(buf, 31, "%.2f", m_val);
#endif
  return string(buf);
}

bool LumiverseFloat::isDefault() {
  return m_val == m_default;
}

void LumiverseFloat::clamp() {
  if (m_val < m_min) {
    m_val = m_min;
    return;
  }
  if (m_val > m_max) {
    m_val = m_max;
    return;
  }
}

void LumiverseFloat::reset()
{ 
  m_val = m_default;
  clamp();
}

void LumiverseFloat::setValAsPercent(float val)
{
  m_val = val * (m_max - m_min) + m_min;
  clamp();
}

float LumiverseFloat::asPercent() {
  if (m_max - m_min == 0)
    return 0;

  return (-m_min + m_val) / (m_max - m_min);
}

// Override for =
void LumiverseFloat::operator=(float val) { m_val = val; clamp(); }
void LumiverseFloat::operator=(LumiverseFloat val)
{
  m_val = val.m_val;
  m_max = val.m_max;
  m_min = val.m_min;
  m_default = val.m_default;
}

// Arithmetic overrides
LumiverseFloat& LumiverseFloat::operator+=(float val) { m_val += val; clamp(); return *this; }
LumiverseFloat& LumiverseFloat::operator+=(LumiverseFloat& val) { m_val += val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator-=(float val) { m_val -= val; clamp();  return *this; }
LumiverseFloat& LumiverseFloat::operator-=(LumiverseFloat& val) { m_val -= val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator*=(float val) { m_val *= val; clamp();  return *this; }
LumiverseFloat& LumiverseFloat::operator*=(LumiverseFloat& val) { m_val *= val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator/=(float val) { m_val /= val; clamp(); return *this; }
LumiverseFloat& LumiverseFloat::operator/=(LumiverseFloat& val) { m_val /= val.m_val; return *this; }
}