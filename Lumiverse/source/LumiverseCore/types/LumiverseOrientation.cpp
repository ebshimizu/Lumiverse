#include "LumiverseOrientation.h"
namespace Lumiverse {
// This is really not interesting huh.

LumiverseOrientation::LumiverseOrientation(float val, ORIENTATION_UNIT unit, float def, float max, float min) :
  m_val(val), m_default(def), m_max(max), m_min(min), m_unit(unit) { }

LumiverseOrientation::LumiverseOrientation(LumiverseOrientation* other) :
  m_val(other->m_val), m_default(other->m_default), m_max(other->m_max), m_min(other->m_min), m_unit(other->m_unit) { }

LumiverseOrientation::LumiverseOrientation(LumiverseType* other) {
  if (other->getTypeName() != "orientation") {
    // If this isn't actually an orientation, use defaults.
    m_val = 0.0f;
    m_default = 0.0f;
    m_max = 360.0f;
    m_min = 0.0f;
    m_unit = DEGREE;
  }
  else {
    LumiverseOrientation* otherFloat = (LumiverseOrientation*)other;
    m_val = otherFloat->m_val;
    m_default = otherFloat->m_default;
    m_max = otherFloat->m_max;
    m_min = otherFloat->m_min;
    m_unit = otherFloat->m_unit;
  }
}

LumiverseOrientation::~LumiverseOrientation() { }

JSONNode LumiverseOrientation::toJSON(string name) {
  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
#ifdef USE_C11_MAPS
  node.push_back(JSONNode("unit", oriToString[m_unit]));
#else
  node.push_back(JSONNode("unit", oriToString(m_unit)));
#endif
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));
  node.push_back(JSONNode("max", m_max));
  node.push_back(JSONNode("min", m_min));

  return node;
}

string LumiverseOrientation::asString() {
  char buf[32];
#ifndef _MSC_VER
  if (m_unit == DEGREE)
    snprintf(buf, 31, "%.2f deg", m_val);
  else if (m_unit == RADIAN)
    snprintf(buf, 31, "%.2f rad", m_val);
#else
  if (m_unit == DEGREE)
    _snprintf_s(buf, 31, "%.2f deg", m_val);
  else if (m_unit == RADIAN)
    _snprintf_s(buf, 31, "%.2f rad", m_val);
#endif
  return string(buf);
}

void LumiverseOrientation::setUnit(ORIENTATION_UNIT unit) {
  // Don't need to do anything if they match already
  if (m_unit == unit)
    return;

  // otherwise we do a conversion for consistency.
  m_val = valAsUnit(unit);
  m_max = maxAsUnit(unit);
  m_min = minAsUnit(unit);
  m_default = defaultAsUnit(unit);
  m_unit = unit;
}

bool LumiverseOrientation::isDefault() {
  return m_val == m_default;
}

inline void LumiverseOrientation::clamp() {
  if (m_val < m_min) {
    m_val = m_min;
    return;
  }
  if (m_val > m_max) {
    m_val = m_max;
    return;
  }
}

float LumiverseOrientation::asPercent() {
  return m_val / (m_max - m_min);
}

float LumiverseOrientation::asUnit(ORIENTATION_UNIT valUnit, float val, ORIENTATION_UNIT targetUnit) {
  if (valUnit == targetUnit)
    return val;
  else if (valUnit == DEGREE && targetUnit == RADIAN)
    return val * (float)M_PI / 180.0f;
  else if (valUnit == RADIAN && targetUnit == DEGREE)
    return val * 180.0f / (float)M_PI;
  else {
    Logger::log(ERR, "Unknown orientation unit specified: " + targetUnit);
    return val;
  }
}

// Override for =
void LumiverseOrientation::operator=(float val) { m_val = val; clamp(); }
void LumiverseOrientation::operator=(LumiverseOrientation val)
{
  m_val = val.m_val;
  m_unit = val.m_unit;
  m_max = val.m_max;
  m_min = val.m_min;
  m_default = val.m_default;

  clamp();
}

// Arithmetic overrides
LumiverseOrientation& LumiverseOrientation::operator+=(float val) { m_val += val; clamp(); return *this; }
LumiverseOrientation& LumiverseOrientation::operator+=(LumiverseOrientation& val) { m_val += val.valAsUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator-=(float val) { m_val -= val; clamp();  return *this; }
LumiverseOrientation& LumiverseOrientation::operator-=(LumiverseOrientation& val) { m_val -= val.valAsUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator*=(float val) { m_val *= val; clamp();  return *this; }
LumiverseOrientation& LumiverseOrientation::operator*=(LumiverseOrientation& val) { m_val *= val.valAsUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator/=(float val) { m_val /= val; clamp(); return *this; }
LumiverseOrientation& LumiverseOrientation::operator/=(LumiverseOrientation& val) { m_val /= val.valAsUnit(m_unit); clamp(); return *this; }
}