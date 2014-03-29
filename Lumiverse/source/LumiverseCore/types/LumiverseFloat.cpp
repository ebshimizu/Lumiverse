#include "LumiverseFloat.h"

// This is really not interesting huh.

LumiverseFloat::LumiverseFloat(float val, float def, float max, float min) :
  m_val(val), m_default(def), m_max(max), m_min(min) { }

LumiverseFloat::LumiverseFloat(LumiverseFloat* other) :
  m_val(other->m_val), m_default(other->m_default) { }

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

float LumiverseFloat::asPercent() {
  return (-m_min + m_val) / (m_max - m_min);
}

// Override for =
void LumiverseFloat::operator=(float val) { m_val = val; clamp(); }
void LumiverseFloat::operator=(LumiverseFloat val) { m_val = val.m_val; }

// Arithmetic overrides
LumiverseFloat& LumiverseFloat::operator+=(float val) { m_val += val; clamp(); return *this; }
LumiverseFloat& LumiverseFloat::operator+=(LumiverseFloat& val) { m_val += val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator-=(float val) { m_val -= val; clamp();  return *this; }
LumiverseFloat& LumiverseFloat::operator-=(LumiverseFloat& val) { m_val -= val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator*=(float val) { m_val *= val; clamp();  return *this; }
LumiverseFloat& LumiverseFloat::operator*=(LumiverseFloat& val) { m_val *= val.m_val; return *this; }

LumiverseFloat& LumiverseFloat::operator/=(float val) { m_val /= val; clamp(); return *this; }
LumiverseFloat& LumiverseFloat::operator/=(LumiverseFloat& val) { m_val /= val.m_val; return *this; }