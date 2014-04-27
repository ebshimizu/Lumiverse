#include "DeviceSet.h"
namespace Lumiverse {

DeviceSet::DeviceSet(Rig* rig) : m_rig(rig) {
  // look it's empty
}

DeviceSet::DeviceSet(Rig* rig, set<Device *> devices) : m_rig(rig) {
  m_workingSet = set<Device *>(devices);
}

DeviceSet::DeviceSet(const DeviceSet& dc) {
  m_workingSet = set<Device *>(dc.m_workingSet);
  m_rig = dc.m_rig;
}

DeviceSet::~DeviceSet() {
  // Nothing for now
}

DeviceSet DeviceSet::select(string selector) {
  // First step is to split the entire string into groups.
  vector<string> groups;

  size_t lbracket = selector.find('[', 0);
  size_t rbracket;

  // If we're not starting with a bracket, that's ok just treat everything before a bracket as a group.
  if (lbracket != 0) {
    groups.push_back(selector.substr(0, lbracket));
  }

  while (lbracket != string::npos) {
    rbracket = selector.find(']', lbracket);

    if (rbracket == string::npos) {
      stringstream ss;
      ss << "Selector parse error: no matching ] for [ in " << selector << " (" << lbracket << ")";
      Logger::log(LOG_LEVEL::ERR, ss.str());
    }

    groups.push_back(selector.substr(lbracket + 1, rbracket - lbracket - 1));
    lbracket = selector.find('[', rbracket);
  }

  // The first selector is always an add.
  bool filter = false;

  // Group by group, add devices to the DeviceSet according to the selectors
  for (string& s : groups) {
    size_t start = 0;
    size_t end = 0;
    vector<DeviceSet> queryResults;

    while (start != string::npos) {
      // Flag indicating the end of an or section.
      bool consolidate = true;

      // Skip whitespace
      while (s[start] == ' ' || s[start] == '\n' || s[start] == '\t') {
        start++;
      }

      // Time to handle |
      end = s.find(',', start);

        size_t bar = s.find('|', start);
        if (bar < end) {
          end = bar;
          consolidate = false;
        }

      string sel = s.substr(start, end - start);
      DeviceSet temp = parseSelector(sel, filter);

      // Consolidation step. Either wait until all the or ops have finished
      // and then consolidate results into one set, or just replace selector result with current
      // working set.
      if (!consolidate) {
        queryResults.push_back(temp);
      }
      else {
        m_workingSet = set<Device*>(temp.m_workingSet);

        for (auto& res : queryResults) {
          m_workingSet.insert(res.m_workingSet.begin(), res.m_workingSet.end());
        }
      }

      start = (end == string::npos) ? end : end + 1;
    }

    filter = true;
  }

  return *this;
}

DeviceSet DeviceSet::parseSelector(string selector, bool filter) {
  char type;
  // first check for !
  type = (selector[0] == '!') ? selector[1] : selector[0];

  // First check for #, @ or $
  switch (type) {
    // Channel selector
    case '#':
      return parseChannelSelector(selector, filter);
      break;
    // Parameter selector
    case '@':
      return parseParameterSelector(selector, filter);
      break;
    // Metadata seletor
    case '$': 
      return parseMetadataSelector(selector, filter);
    // Everything else is an ID
    default:
      return (filter) ? remove(m_rig->m_devicesById[selector]) : add(m_rig->m_devicesById[selector]);
  }
}

DeviceSet DeviceSet::parseMetadataSelector(string selector, bool filter) {
  regex metadataRegex("(!\?)\\$([\\w\\d\\-]+)([\\!\\*~\\$\\^]?[=])(.+)");
  smatch matches;
  regex_match(selector, matches, metadataRegex);

  // Matches size is 5 since entire string is the first match
  if (matches.size() != 5) {
    stringstream ss;
    ss << "Selector parse error: invalid metadata selector format: " << selector;
    Logger::log(LOG_LEVEL::ERR, ss.str());
  }

  string invert = matches[1];
  bool eq = (invert.size() > 0) ? false : true;

  string metadataKey = matches[2];
  string op = matches[3];
  string arg = matches[4];
  regex testArg;

  switch (op[0]) {
    // Contains
    case '*':
      testArg = regex(".*" + arg + ".*");
      break;
    // Ends with
    case '$':
      testArg = regex(".*" + arg + "$");
      break;
    // Starts with
    case '^':
      testArg = regex("^" + arg + ".*");
      break;
    // Not equal to
    case '!':
      eq = !eq;
      testArg = regex(arg);
      break;
    // Exactly equal to
    case '=':
      // Anything else is same as =
    default:
      testArg = regex("^" + arg + "$");
      break;
  }

  return (filter) ? remove(metadataKey, testArg, !eq) : add(metadataKey, testArg, eq);
}

DeviceSet DeviceSet::parseChannelSelector(string selector, bool filter) {
  regex metadataRegex("(!\?)#(\\d+)-\?(\\d*)");
  smatch matches;
  regex_match(selector, matches, metadataRegex);

  // Matches size is 4 since entire string is the first match
  if (matches.size() != 4) {
    stringstream ss;
    ss << "Selector parse error: invalid channel selector format: " << selector;
    Logger::log(LOG_LEVEL::ERR, ss.str());
  }

  string invert = matches[1];
  bool eq = (invert.size() > 0) ? false : true;

  size_t first, second;
  stringstream(matches[2]) >> first;

  string secondStr = matches[3];
  if (secondStr.size() > 0) {
    stringstream(matches[3]) >> second;

    if (first > second) {
      // Flip channel ranges if the first value is greater than the second value
      size_t tmp = first;
      first = second;
      second = tmp;
    }
  }

  // Non-inverted range
  if (eq) {
    if (secondStr.size() == 0) {
      return (filter) ? this->remove(first) : this->add(first);
    }
    else {
      return (filter) ? this->remove(first, second) : this->add(first, second);
    }
  }
  // Inverted range
  else {
    if (secondStr.size() == 0) {
      size_t lowerEnd = first - 1;
      size_t upperStart = first + 1;
      size_t maxChan = m_rig->m_devicesByChannel.rbegin()->first;

      return (filter) ? this->remove(0, lowerEnd).remove(upperStart, maxChan) : this->add(0, lowerEnd).add(upperStart, maxChan);
    }
    else
    {
      size_t lowerEnd = first - 1;
      size_t upperStart = second + 1;
      size_t maxChan = m_rig->m_devicesByChannel.rbegin()->first;

      return (filter) ? this->remove(0, lowerEnd).remove(upperStart, maxChan) : this->add(0, lowerEnd).add(upperStart, maxChan);
    }
  }
}

DeviceSet DeviceSet::parseParameterSelector(string selector, bool filter) {
  // Paramters are special in that we need to know the type of parameter we're filtering
  // before we can construct the actual query in C++

  // Supported Types: LumiverseFloat
  regex paramRegex("(!\?)@(\\w+)([><!]\?[><=])(\\d*\\.\?\\d*)([f])");
  smatch matches;
  regex_match(selector, matches, paramRegex);
  
  // Matches size is 6 since entire string is the first match
  if (matches.size() != 6) {
    stringstream ss;
    ss << "Selector parse error: invalid parameter selector format: " << selector;
    Logger::log(LOG_LEVEL::ERR, ss.str());
  }

  string type = matches[5];
  string invert = matches[1];
  bool eq = (invert.size() > 0) ? false : true;

  switch (type[0]) {
    // LumiverseFloat
    case 'f':
      float val;
      stringstream(matches[4]) >> val;
      return parseFloatParameter(matches[2], matches[3], val, filter, eq);
    // Error
    default:
      stringstream ss;
      ss << "Selector parse error: invalid parameter selector type: " << type << " in " << selector;
      Logger::log(LOG_LEVEL::ERR, ss.str());
      return DeviceSet(*this);
  }
}

DeviceSet DeviceSet::parseFloatParameter(string param, string op, float val, bool filter, bool eq) {
  LumiverseFloat oVal(val);
  LumiverseType* gVal = (LumiverseType *)(&oVal);

  // There is some serious boxing and unboxing here to make this thing work with multiple types.
  // Not sure if worth.
  if (op == "<") {
    function<bool(LumiverseType* a, LumiverseType* b)> ltFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) < (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, ltFunc, eq) : add(param, gVal, ltFunc, eq);
  }
  if (op == ">") {
    function<bool(LumiverseType* a, LumiverseType* b)> gtFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) > (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, gtFunc, eq) : add(param, gVal, gtFunc, eq);
  }
  if (op == "<=") {
    function<bool(LumiverseType* a, LumiverseType* b)> leqFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) <= (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, leqFunc, eq) : add(param, gVal, leqFunc, eq);
  }
  if (op == ">=") {
    function<bool(LumiverseType* a, LumiverseType* b)> geqFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) >= (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, geqFunc, eq) : add(param, gVal, geqFunc, eq);
  }
  if (op == "!=") {
    function<bool(LumiverseType* a, LumiverseType* b)> neqFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) != (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, neqFunc, eq) : add(param, gVal, neqFunc, eq);
  }
  // Defaults to =
  else {
    function<bool(LumiverseType* a, LumiverseType* b)> eqFunc = [](LumiverseType* a, LumiverseType* b){ return (*(LumiverseFloat *)a) == (*(LumiverseFloat *)b); };
    return (filter) ? remove(param, gVal, eqFunc, eq) : add(param, gVal, eqFunc, eq);
  }
}

DeviceSet DeviceSet::add(Device* device) {
  DeviceSet newSet(*this);
  newSet.addDevice(device);

  return newSet;
}

DeviceSet DeviceSet::add(string query) {
  DeviceSet newSet(*this);
  newSet.addSet(m_rig->query(query));
  
  return newSet;
}

DeviceSet DeviceSet::add(unsigned int channel) {
  DeviceSet newSet(*this);

  auto range = m_rig->m_devicesByChannel.equal_range(channel);
  for (auto it = range.first; it != range.second; it++) {
    newSet.addDevice(it->second);
  }

  return newSet;
}

DeviceSet DeviceSet::add(unsigned int lower, unsigned int upper) {
  DeviceSet newSet(*this);
  
  for (unsigned int i = lower; i <= upper; i++) {
    auto range = m_rig->m_devicesByChannel.equal_range(i);
    for (auto it = range.first; it != range.second; it++) {
      newSet.addDevice(it->second);
    }
  }

  return newSet;
}

DeviceSet DeviceSet::add(string key, string val, bool isEqual) {
  return add(key, regex("^" + val + "$"), isEqual);
}

DeviceSet DeviceSet::add(string key, regex val, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_rig->m_devices) {
    string data;
    if (d->getMetadata(key, data)) {
      if (regex_match(data, val) == isEqual) {
        newSet.addDevice(d);
      }
    }
  }

  return newSet;
}

DeviceSet DeviceSet::add(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_rig->m_devices) {
    LumiverseType* data = d->getParam(key);
    if (data != nullptr) {
      if (cmp(data, val) == isEqual) {
        newSet.addDevice(d);
      }
    }
  }

  return newSet;
}

DeviceSet DeviceSet::remove(Device* device) {
  DeviceSet newSet(*this);
  newSet.removeDevice(device);
  return newSet;
}

DeviceSet DeviceSet::remove(string query) {
  DeviceSet newSet(*this);
  newSet.removeSet(m_rig->query(query));
  
  return newSet;
}

DeviceSet DeviceSet::remove(unsigned int channel) {
  DeviceSet newSet(*this);

  auto range = m_rig->m_devicesByChannel.equal_range(channel);
  for (auto it = range.first; it != range.second; it++) {
    newSet.removeDevice(it->second);
  }

  return newSet;
}

DeviceSet DeviceSet::remove(unsigned int lower, unsigned int upper) {
  DeviceSet newSet(*this);

  for (unsigned int i = lower; i <= upper; i++) {
    auto range = m_rig->m_devicesByChannel.equal_range(i);
    for (auto it = range.first; it != range.second; it++) {
      newSet.removeDevice(it->second);
    }
  }

  return newSet;
}

DeviceSet DeviceSet::remove(string key, string val, bool isEqual) {
  return remove(key, regex("^" + val + "$"), isEqual);
}

DeviceSet DeviceSet::remove(string key, regex val, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_workingSet) {
    string data;
    if (d->getMetadata(key, data)) {
      if (regex_match(data, val) == isEqual) {
        newSet.removeDevice(d);
      }
    }
    // reject devices that don't have the desired key.
    else {
      newSet.removeDevice(d);
    }
  }

  return newSet;
}

DeviceSet DeviceSet::remove(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_workingSet) {
    LumiverseType* data = d->getParam(key);
    if (data != nullptr) {
      if (cmp(data, val) == isEqual) {
        newSet.removeDevice(d);
      }
    }
    // reject devices that don't have the desired key.
    else {
      newSet.removeDevice(d);
    }
  }

  return newSet;
}

void DeviceSet::reset() {
  for (auto d : m_workingSet) {
    d->reset();
  }
}

void DeviceSet::addDevice(Device* device) {
  if (device != nullptr) {
    m_workingSet.insert(device);
  }
}

void DeviceSet::removeDevice(Device* device) {
  m_workingSet.erase(device);
}

void DeviceSet::addSet(DeviceSet otherSet) {
  m_workingSet.insert(otherSet.m_workingSet.begin(), otherSet.m_workingSet.end());
}

void DeviceSet::removeSet(DeviceSet otherSet) {
  for (auto d : otherSet.m_workingSet) {
    removeDevice(d);
  }
}

void DeviceSet::setParam(string param, float val) {
  for (auto d : m_workingSet) {
    if (d->paramExists(param)) {
      d->setParam(param, val);
    }
  }
}

void DeviceSet::setParam(string param, string val, float val2) {
  for (auto d : m_workingSet) {
    if (d->paramExists(param)) {
      d->setParam(param, val, val2);
    }
  }
}

string DeviceSet::info() {
  stringstream ss;

  ss << "Device set contains " << size() << " devices.\n";
  ss << "IDs: ";

  bool first = true;
  for (auto& d : m_workingSet) {
    ss << ((first) ? "" : ", ") << d->getId();
    first = false;
  }

  return ss.str();
}
}