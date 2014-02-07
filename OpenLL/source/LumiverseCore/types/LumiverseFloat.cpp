#include "LumiverseFloat.h"

// This is really not interesting huh.

LumiverseFloat::LumiverseFloat(float val, float def) : m_val(val), m_default(def) { }

LumiverseFloat::~LumiverseFloat() { }

JSONNode LumiverseFloat::toJSON(string name) {
  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));

  return node;
}