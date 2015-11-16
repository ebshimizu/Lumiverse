#include "CueList.h"

namespace Lumiverse {
namespace ShowControl {

CueList::CueList(string name, Playback* pb) : _name(name), _pb(pb)
{
}

CueList::CueList(JSONNode node, Playback* pb): _pb(pb) {
  auto cues = node.find("cues");
  if (cues != node.end()) {
    // Load cues
    auto it = cues->begin();
    while (it != cues->end()) {
      stringstream ss;
      ss << it->name();

      float cueNum;
      ss >> cueNum;

      _cues[cueNum] = it->as_string();

      ++it;
    }
  }
  else {
    Logger::log(WARN, "No cues found in cue list.");
  }

  _name = node.name();
}

CueList::~CueList()
{
}

bool CueList::storeCue(float num, string cueID, bool overwrite) {
  if (num < 0) {
    Logger::log(ERR, "Cue numbers must be positive.");
    return false;
  }

  if (_pb->getTimeline(cueID) == nullptr) {
    Logger::log(ERR, "Specified Cue does not exist: " + cueID);
    return false;
  }

  if (overwrite == true || _cues.count(num) == 0) {
    _cues[num] = cueID;
    stringstream ss;
    ss << "Recorded cue " << num;
    Logger::log(INFO, ss.str());
    return true;
  }

  stringstream ss;
  ss << "Cue " << num << " already exists in cue list";
  Logger::log(INFO, ss.str());
  return false;
}

void CueList::deleteCue(float num, bool totalDelete) {
  if (_cues.count(num) > 0) {
    if (totalDelete) {
      _pb->deleteTimeline(_cues[num]);
    }
    _cues.erase(num);

    stringstream ss;
    ss << "Cue " << num << " deleted from cue list";
    Logger::log(INFO, ss.str());
  }
}

//void CueList::update(float num, Rig* rig, bool track) {
  // Should update a series of cues in the cue list according to tracking rules.
//}

float CueList::getFirstCueNum() {
  if (_cues.size() == 0) {
    // No cues defaults this to -1.    
    return -1;
  }
  return _cues.begin()->first;
}

string CueList::getFirstCue() {
  if (_cues.size() == 0) {
    // No cues defaults this to -1.    
    return "";
  }
  return _cues.begin()->second;
}

float CueList::getLastCueNum() {
  if (_cues.size() == 0) {
    // No cues defaults this to -1
    return -1;
  }

  return _cues.rbegin()->first;
}

string CueList::getLastCue() {
  if (_cues.size() == 0) {
    // No cues defaults this to -1
    return "";
  }

  return _cues.rbegin()->second;
}

Cue* CueList::getCue(float num) {
  if (_cues.count(num) == 0)
    return nullptr;

  return (Cue*)_pb->getTimeline(_cues[num]).get();
}

string CueList::getCueName(float num) {
  if (_cues.count(num) == 0)
    return "";

  return _cues[num];
}

string CueList::getNextCue(float num) {
  auto current = _cues.find(num);
  if (current == _cues.end()) {
    return "";
  }

  current++;
  if (current == _cues.end()) {
    return "";
  }
  
  return current->second;
}

float CueList::getNextCueNum(float num) {
  auto current = _cues.find(num);
  if (current == _cues.end()) {
    return -1;
  }

  current++;
  return current->first;
}

string CueList::getPrevCue(float num) {
  auto current = _cues.find(num);
  if (current == _cues.begin()) {
    return "";
  }

  current--;

  return current->second;
}

float CueList::getPrevCueNum(float num) {
  auto current = _cues.find(num);
  if (current == _cues.begin()) {
    return -1;
  }

  current--;
  return current->first;
}

float CueList::getCueNumAtIndex(int index) {
  auto it = _cues.begin();
  
  // There's gotta be a faster way of finding this with the std lib
  for (int i = 0; i < index; i++) {
    it++;
  }
  
  return it->first;
}

JSONNode CueList::toJSON() {
  JSONNode list;
  list.set_name(_name);

  JSONNode cues;
  cues.set_name("cues");
  for (auto& kvp : _cues) {
    stringstream ss;
    ss << kvp.first;

    JSONNode cue(ss.str(), kvp.second);
    
    cues.push_back(cue);
  }

  list.push_back(cues);
  return list;
}

}
}