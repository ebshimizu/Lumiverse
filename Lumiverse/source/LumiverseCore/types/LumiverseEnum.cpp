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

LumiverseEnum::LumiverseEnum(map<string, int> keys, string mode, int rangeMax, string def) :
  m_rangeMax(rangeMax)
{
  m_mode = stringToMode(mode);

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

LumiverseEnum::LumiverseEnum(LumiverseEnum* other) :
  m_active(other->m_active), m_default(other->m_default), m_mode(other->m_mode),
  m_rangeMax(other->m_rangeMax), m_tweak(other->m_tweak)
{
  m_nameToStart = map<string, int>(other->m_nameToStart);
  m_startToName = map<int, string>(other->m_startToName);
}

LumiverseEnum::LumiverseEnum(LumiverseType* other) {
  if (other->getTypeName() != "enum") {
    // Initialize with defaults, which here means practically nothing
    m_active = "";
  }
  else {
    LumiverseEnum* otherEnum = (LumiverseEnum*)other;
    m_active = otherEnum->m_active;
    m_default = otherEnum->m_default;
    m_mode = otherEnum->m_mode;
    m_rangeMax = otherEnum->m_rangeMax;
    m_tweak = otherEnum->m_tweak;

    m_nameToStart = map<string, int>(otherEnum->m_nameToStart);
    m_startToName = map<int, string>(otherEnum->m_startToName);
  }
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

  for (auto kvp : m_startToName) {
    keys.push_back(JSONNode(kvp.second, kvp.first));
  }

  JSONNode node;
  node.set_name(name);

  node.push_back(JSONNode("type", getTypeName()));
  node.push_back(JSONNode("active", m_active));
  node.push_back(JSONNode("tweak", m_tweak));
  node.push_back(JSONNode("mode", modeAsString()));
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
  return true;
}

bool LumiverseEnum::setVal(string name, float tweak) {
  if (setVal(name)) {
    // Protect ourself from accidentally making changes if the key doesn't exist
    setTweak(tweak);
    return true;
  }

  return false;
}

void LumiverseEnum::setTweak(float tweak) {
  if (tweak < 0) tweak = 0;
  if (tweak > 1) tweak = 1;
  m_tweak = tweak;
}

float LumiverseEnum::getRangeVal() {
  int start = m_nameToStart[m_active];

  // Get the next value in the range. If at end use rangeMax.
  auto it = m_startToName.find(start);
  it++;
  int end;

  end = (it == m_startToName.end()) ? m_rangeMax : it->first - 1;

  return start + (end - start) * m_tweak;
}

void LumiverseEnum::operator=(string name) {
  setVal(name);
}

void LumiverseEnum::operator=(LumiverseEnum val) {
  m_active = val.m_active;
  m_default = val.m_default;
  m_mode = val.m_mode;
  m_rangeMax = val.m_rangeMax;
  m_tweak = val.m_tweak;

  m_nameToStart = map<string, int>(val.m_nameToStart);
  m_startToName = map<int, string>(val.m_startToName);
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

string LumiverseEnum::modeAsString() {
  switch (m_mode)
  {
  case FIRST:
    return "FIRST";
  case CENTER:
    return "CENTER";
  case LAST:
    return "LAST";
  default:
    Logger::log(ERR, "Invalud LumiverseEnum mode.");
    return "";
  }
}

LumiverseEnum::Mode LumiverseEnum::stringToMode(string input) {
  if (input == "FIRST") return FIRST;
  if (input == "CENTER") return CENTER;
  if (input == "LAST") return LAST;

  stringstream ss;
  ss << "Invalid mode string provided: " << input << ". Options are FIRST, CENTER, LAST";
  Logger::log(ERR, ss.str());
  return CENTER;
}