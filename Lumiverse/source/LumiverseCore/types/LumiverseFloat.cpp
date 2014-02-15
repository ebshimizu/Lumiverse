#include "LumiverseFloat.h"

// This is really not interesting huh.

LumiverseFloat::LumiverseFloat(float val, float def) : m_val(val), m_default(def) { }

LumiverseFloat::LumiverseFloat(LumiverseFloat* other) :
  m_val(other->m_val), m_default(other->m_default) { }

LumiverseFloat::LumiverseFloat(LumiverseType* other) {
  if (other->getTypeName() != "float") {
    // If this isn't actually a float, use defaults.
    m_val = 0.0f;
    m_default = 0.0f;
  }
  else {
    LumiverseFloat* otherFloat = (LumiverseFloat*)other;
    m_val = otherFloat->m_val;
    m_default = otherFloat->m_default;
  }
}

LumiverseFloat::~LumiverseFloat() { }

JSONNode LumiverseFloat::toJSON(string name) {
  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));

  return node;
}