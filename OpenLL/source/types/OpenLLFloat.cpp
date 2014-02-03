#include "OpenLLFloat.h"

// This is really not interesting huh.

OpenLLFloat::OpenLLFloat(float val, float def) : m_val(val), m_default(def) { }

OpenLLFloat::~OpenLLFloat() { }

JSONNode OpenLLFloat::toJSON(string name) {
  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("val", m_val));
  node.push_back(JSONNode("default", m_default));

  return node;
}