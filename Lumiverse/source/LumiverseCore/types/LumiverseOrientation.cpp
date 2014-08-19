#include "LumiverseOrientation.h"
namespace Lumiverse {
// This is really not interesting huh.

LumiverseOrientation::LumiverseOrientation(float val, string unit, float def, float max, float min) :
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
	m_unit = "degree";
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
  node.push_back(JSONNode("unit", m_unit));
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));
  node.push_back(JSONNode("max", m_max));
  node.push_back(JSONNode("min", m_min));

  return node;
}

string LumiverseOrientation::asString() {
  char buf[32];
#ifndef _MSC_VER
  snprintf(buf, 31, "%.2f (%s)", m_val, m_unit.c_str());
#else
  _snprintf_s(buf, 31, "%.2f (%s)", m_val, m_unit.c_str());
#endif
  return string(buf);
}

bool LumiverseOrientation::isDefault() {
  return m_val == m_default;
}

void LumiverseOrientation::clamp() {
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
  return (-m_min + m_val) / (m_max - m_min);
}

float LumiverseOrientation::asUnit(string unit) {
	if (getUnit() == unit)
		return getVal();
	if (getUnit() == "degree")
		return getVal() * M_PI / 180.0f;
	return getVal() * 180.0f / M_PI;
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
}

// Arithmetic overrides
LumiverseOrientation& LumiverseOrientation::operator+=(float val) { m_val += val; clamp(); return *this; }
LumiverseOrientation& LumiverseOrientation::operator+=(LumiverseOrientation& val) { m_val += val.asUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator-=(float val) { m_val -= val; clamp();  return *this; }
LumiverseOrientation& LumiverseOrientation::operator-=(LumiverseOrientation& val) { m_val -= val.asUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator*=(float val) { m_val *= val; clamp();  return *this; }
LumiverseOrientation& LumiverseOrientation::operator*=(LumiverseOrientation& val) { m_val *= val.asUnit(m_unit); clamp(); return *this; }

LumiverseOrientation& LumiverseOrientation::operator/=(float val) { m_val /= val; clamp(); return *this; }
LumiverseOrientation& LumiverseOrientation::operator/=(LumiverseOrientation& val) { m_val /= val.asUnit(m_unit); clamp(); return *this; }
}