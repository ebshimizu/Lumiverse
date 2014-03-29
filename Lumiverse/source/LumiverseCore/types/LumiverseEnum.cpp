#include "LumiverseEnum.h"


LumiverseEnum::LumiverseEnum(Mode mode, int rangeMax) :
  m_mode(mode), m_rangeMax(rangeMax) 
{
  m_active = "";
}

LumiverseEnum::LumiverseEnum(map<string, int> keys, Mode mode, int rangeMax, string def) :
m_mode(mode), m_rangeMax(rangeMax)
{
  m_nameToStart = keys;

  for (auto kvp : keys) {
    m_startToName[kvp.second] = kvp.first;
  }

  // Set the active enumeration to the first in the range.
  m_active = m_startToName.begin()->second;
  setTweakWithMode();

  if (def == "") m_default = m_active;
  else m_default = def;
}

LumiverseEnum::~LumiverseEnum()
{
}

void LumiverseEnum::reset() {
  setVal(m_default);
}

JSONNode LumiverseEnum::toJSON(string name) {
  JSONNode keys;
  keys.set_name("keys");

  for (auto kvp : m_nameToStart) {
    keys.push_back(JSONNode(kvp.first, kvp.second));
  }

  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("active", m_active));
  node.push_back(JSONNode("tweak", m_tweak));

  // TODO: Push mode as a string for readability
  node.push_back(JSONNode("mode", m_mode));

  node.push_back(JSONNode("default", m_default));
  node.push_back(JSONNode("rangeMax", m_rangeMax));
  node.push_back(keys);

  return node;
}

void LumiverseEnum::addVal(string name, int start) {
  if (m_nameToStart.count(name) > 0) {
    // Need to remove this value from the other map if we're overwriting
    m_startToName.erase(m_nameToStart[name]);
  }

  m_nameToStart[name] = start;
  m_startToName[start] = name;
}

void LumiverseEnum::removeVal(string name) {
  if (m_nameToStart.count(name) > 0) {
    // Remove if it actually exists
    m_startToName.erase(m_nameToStart[name]);
    m_nameToStart.erase(name);
  }
}

bool LumiverseEnum::setVal(string name) {
  if (m_nameToStart.count(name) < 1) {
    stringstream ss;
    ss << "LumiverseEnum has no enumeration " << name;
    Logger::log(WARN, ss.str());
    return false;
  }

  m_active = name;
  setTweakWithMode();
}

bool LumiverseEnum::setVal(string name, float tweak) {
  if (setVal(name)) {
    // Protect ourself from accidentally making changes if the key doesn't exist
    setTweak(tweak);
    return true;
  }

  return false;
}

void LumiverseEnum::operator=(string name) {
  setVal(name);
}

void LumiverseEnum::operator=(LumiverseEnum val) {
  setVal(val.getVal(), val.getTweak());
}

void LumiverseEnum::setTweakWithMode() {
  switch (m_mode)
  {
  case FIRST:
    m_tweak = 0.0f;
    break;
  case CENTER:
    m_tweak = 0.5f;
    break;
  case LAST:
    m_tweak = 1.0f;
    break;
  default:
    Logger::log(ERR, "Invalud LumiverseEnum mode.");
    break;
  }
}