#include "LumiverseEnum.h"
namespace Lumiverse {

LumiverseEnum::LumiverseEnum(Mode mode, int rangeMax, InterpolationMode interpMode) {
  init(map<string, int>(), "", mode, "", 0.5f, rangeMax, interpMode);
}

LumiverseEnum::LumiverseEnum(map<string, int> keys, Mode mode, int rangeMax, string def, InterpolationMode interpMode) :
  m_mode(mode), m_rangeMax(rangeMax)
{
  init(keys, "", mode, def, 0.5f, rangeMax, interpMode);

  // Set the active enumeration to the first in the range.
  m_active = m_startToName.begin()->second;
  setTweakWithMode();

  if (def == "") m_default = m_active;
  else m_default = def;
}

LumiverseEnum::LumiverseEnum(map<string, int> keys, string mode, string interpMode, int rangeMax, string def) {
  init(keys, "", stringToMode(mode), def, 0.5f, rangeMax, stringToInterpMode(interpMode));

  // Set the active enumeration to the first in the range.
  m_active = m_startToName.begin()->second;
  setTweakWithMode();

  if (def == "") m_default = m_active;
  else m_default = def;
}

LumiverseEnum::LumiverseEnum(LumiverseEnum* other) {
  init(other->m_nameToStart, other->m_active, other->m_mode, other->m_default,
    other->m_tweak, other->m_rangeMax, other->m_interpMode, other->m_startToName);
}

LumiverseEnum::LumiverseEnum(const LumiverseEnum& other) {
  init(other.m_nameToStart, other.m_active, other.m_mode, other.m_default,
    other.m_tweak, other.m_rangeMax, other.m_interpMode, other.m_startToName);
}

LumiverseEnum::LumiverseEnum(LumiverseType* other) {
  if (other->getTypeName() != "enum") {
    // Initialize with defaults, which here means practically nothing
    m_active = "";
  }
  else {
    LumiverseEnum* otherEnum = (LumiverseEnum*)other;

    init(otherEnum->m_nameToStart, otherEnum->m_active, otherEnum->m_mode, otherEnum->m_default,
      otherEnum->m_tweak, otherEnum->m_rangeMax, otherEnum->m_interpMode, otherEnum->m_startToName);
  }
}

void LumiverseEnum::init(map<string, int> keys, string active, Mode mode, string def,
  float tweak, int rangeMax, InterpolationMode interpMode) {
  m_active = active;
  m_mode = mode;
  m_default = def;
  m_tweak = tweak;
  m_rangeMax = rangeMax;
  m_interpMode = interpMode;

  m_nameToStart = keys;

  for (const auto& kvp : keys) {
    m_startToName[kvp.second] = kvp.first;
  }
}

void LumiverseEnum::init(map<string, int> keys, string active, Mode mode, string def,
  float tweak, int rangeMax, InterpolationMode interpMode, map<int, string> vals) {
  m_active = active;
  m_mode = mode;
  m_default = def;
  m_tweak = tweak;
  m_rangeMax = rangeMax;
  m_interpMode = interpMode;

  m_nameToStart = keys;
  m_startToName = vals;
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

  for (const auto& kvp : m_startToName) {
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
  node.push_back(JSONNode("interpMode", interpModeAsString()));
  node.push_back(keys);

  return node;
}

string LumiverseEnum::asString() {
  stringstream ss;
  ss << m_active << " (" << m_tweak << ")";
  return ss.str();
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

bool LumiverseEnum::setVal(string name, float tweak, Mode enumMode, InterpolationMode interpMode) {
  if (setVal(name)) {
    setTweak(tweak);
    setMode(enumMode);
    setInterpMode(interpMode);
    return true;
  }

  return false;
}

bool LumiverseEnum::setVal(float val) {
  // Need to protect this section from someone writing stuff during the process
  // Clamp cases are trivial.
  if (val < m_startToName.begin()->first) {
    return setVal(m_startToName.begin()->second, 0.0f);
  }
  else if (val > m_rangeMax) {
    return setVal(m_startToName.rbegin()->second, 1.0f);
  }

  lock_guard<mutex> lock(m_enumMapMutex);
  // Otherwise we need to figure out what the value should be
  int start;
  for (const auto& kvp : m_startToName) {
    // So yeah this is pretty slow. It's a search for a point in a range
    // but in the interest of getting things running first we'll do the slow thing.
    if (val < kvp.first)
      break;
    start = kvp.first;
  }

  string name = m_startToName[start];
  auto it = m_startToName.find(start);
  it++;

  int end = (it == m_startToName.end()) ? m_rangeMax : it->first - 1;

  float tweak = ((float)(val - start) / (float)(end - start));
  return setVal(name, tweak);
}

void LumiverseEnum::setTweak(float tweak) {
  if (tweak < 0) tweak = 0;
  if (tweak > 1) tweak = 1;
  m_tweak = tweak;
}

float LumiverseEnum::getRangeVal() {
  // Protect access to m_nameToStart and other maps
  lock_guard<mutex> lock(m_enumMapMutex);

  int start = m_nameToStart[m_active];

  // Get the next value in the range. If at end use rangeMax.
  auto it = m_startToName.find(start);
  it++;
  int end;

  end = (it == m_startToName.end()) ? m_rangeMax : it->first - 1;

  return start + (end - start) * m_tweak;
}

shared_ptr<LumiverseType> LumiverseEnum::lerp(LumiverseEnum* rhs, float t) {
  // Protect access to maps during this process
  LumiverseEnum* newEnum = new LumiverseEnum(rhs);
  
  if (m_interpMode == SNAP) {
    // Default initialization of newEnum is to rhs already.
  }
  else if (m_interpMode == SMOOTH_WITHIN_OPTION) {
    if (rhs->getVal() == m_active) {
      // If we're in the same value, then the lerp is just a lerp between the tweak values.
      newEnum->setTweak(getTweak() * (1 - t) + rhs->getTweak() * t);
    }
  }
  else if (m_interpMode == SMOOTH) {
    // Lerp between the range values and let the enum figure things out.
    newEnum->setVal(getRangeVal() * (1 - t) + rhs->getRangeVal() * t);
  }

  return shared_ptr<LumiverseType>((LumiverseType*)newEnum);
}

void LumiverseEnum::operator=(string name) {
  setVal(name);
}

void LumiverseEnum::operator=(const LumiverseEnum& val) {
  m_active = val.m_active;
  m_default = val.m_default;
  m_mode = val.m_mode;
  m_rangeMax = val.m_rangeMax;
  m_tweak = val.m_tweak;

  // Before we do a copy of the maps, check to see if they aren't already the same.
  if (val.m_nameToStart.size() == m_nameToStart.size()) {
    for (const auto& kvp : m_nameToStart) {
      if (kvp.second != val.m_nameToStart.find(kvp.first)->second) {
        break;
      }
    }
    return;
  }

  // If they aren't the same, do the copy.
  lock_guard<mutex> lock(m_enumMapMutex);
  m_nameToStart = val.m_nameToStart;
  m_startToName = val.m_startToName;

  // Lock guard goes out of scope and releases mutex.
}

bool LumiverseEnum::isDefault() {
  // Also the tweak needs to match the default for the mode.
  // Default if not first or last is center.
  float target = (m_mode == FIRST) ? 0.0f : (m_mode == LAST) ? 1 : 0.5f;

  return (m_active == m_default) && (m_tweak == target);
}

vector<string> LumiverseEnum::getVals() {
  vector<string> vals;
  for (const auto& kvp : m_startToName) {
    vals.push_back(kvp.second);
  }
  return vals;
}

int LumiverseEnum::getHighestStartValue() {
  if (m_startToName.size() == 0)
    return -1;

  auto end = m_startToName.rbegin();
  return end->first;
}

int LumiverseEnum::getLowestStartValue() {
  if (m_startToName.size() == 0)
    return 0;

  auto first = m_startToName.begin();
  return first->first;
}

float LumiverseEnum::asPercent() {
  float val = getRangeVal();
  return (val - getLowestStartValue()) / (m_rangeMax - getLowestStartValue());
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
    Logger::log(ERR, "Invalid LumiverseEnum mode.");
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

string LumiverseEnum::interpModeAsString() {
  switch (m_interpMode)
  {
  case SNAP:
    return "SNAP";
  case SMOOTH_WITHIN_OPTION:
    return "SMOOTH_WITHIN_OPTION";
  case SMOOTH:
    return "SMOOTH";
  default:
    Logger::log(ERR, "Invalid LumiverseEnum interpolation mode.");
    return "";
  }
}

LumiverseEnum::InterpolationMode LumiverseEnum::stringToInterpMode(string input) {
  if (input == "SNAP") return SNAP;
  if (input == "SMOOTH_WITHIN_OPTION") return SMOOTH_WITHIN_OPTION;
  if (input == "SMOOTH") return SMOOTH;

  stringstream ss;
  ss << "Invalid mode string provided: " << input << ". Options are FIRST, CENTER, LAST";
  Logger::log(ERR, ss.str());
  return SMOOTH_WITHIN_OPTION;
}
}