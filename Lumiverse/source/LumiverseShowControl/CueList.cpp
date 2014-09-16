#include "CueList.h"

namespace Lumiverse {
namespace ShowControl {

CueList::CueList(string name) : m_name(name)
{
}

CueList::CueList(JSONNode node) {
  auto cues = node.find("cues");
  if (cues != node.end()) {
    // Load cues
    auto it = cues->begin();
    while (it != cues->end()) {
      stringstream ss;
      ss << it->name();

      float cueNum;
      ss >> cueNum;

      m_cues[cueNum] = Cue(*it);

      ++it;
    }
  }
  else {
    Logger::log(WARN, "No cues found in cue list.");
  }

  m_name = node.name();
}

CueList::~CueList()
{
}

bool CueList::storeCue(float num, Cue cue, bool overwrite) {
  if (overwrite == true || m_cues.count(num) == 0) {
    m_cues[num] = cue;
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

void CueList::deleteCue(float num) {
  if (m_cues.count(num) > 0) {
    m_cues.erase(num);

    stringstream ss;
    ss << "Cue " << num << " deleted";
    Logger::log(INFO, ss.str());
  }
}

void CueList::update(float num, Rig* rig, bool track) {
  if (track) {
    Cue::changedParams changed = m_cues[num].update(rig);

    auto tracking = m_cues.find(num);
    // Start at the cue after the one modified.
    ++tracking;

    while (tracking != m_cues.end()) {
      tracking->second.trackedUpdate(changed, rig);

      // Keep going until there's nothing left to track,
      // or the while loop reaches the end of the cue list.
      if (changed.size() == 0)
        break;

      ++tracking;
    }

  }
  else {
    // Discard returned param. shared_ptr should free memory.
    m_cues[num].update(rig);
  }
}

float CueList::getFirstCueNum() {
  if (m_cues.size() == 0) {
    // No cues defaults this to 0.    
    return 0;
  }
  return m_cues.begin()->first;
}

float CueList::getLastCueNum() {
  if (m_cues.size() == 0) {
    // No cues defaults this to 0
    return 0;
  }

  return m_cues.rbegin()->first;
}

Cue* CueList::getNextCue(float num) {
  auto current = m_cues.find(num);
  if (current == m_cues.end()) {
    return nullptr;
  }

  current++;
  if (current == m_cues.end()) {
    return nullptr;
  }
  
  return &current->second;
}

float CueList::getNextCueNum(float num) {
  auto current = m_cues.find(num);
  if (current == m_cues.end()) {
    return -1;
  }

  current++;
  return current->first;
}

Cue* CueList::getPrevCue(float num) {
  auto current = m_cues.find(num);
  if (current == m_cues.begin()) {
    return nullptr;
  }

  current--;

  return &current->second;
}

float CueList::getPrevCueNum(float num) {
  auto current = m_cues.find(num);
  if (current == m_cues.begin()) {
    return -1;
  }

  current--;
  return current->first;
}

float CueList::getCueNumAtIndex(int index) {
  auto it = m_cues.begin();
  
  // There's gotta be a faster way of finding this with the std lib
  for (int i = 0; i < index; i++) {
    it++;
  }
  
  return it->first;
}

JSONNode CueList::toJSON() {
  JSONNode list;
  list.set_name(m_name);

  JSONNode cues;
  cues.set_name("cues");
  for (auto& kvp : m_cues) {
    stringstream ss;
    ss << kvp.first;

    JSONNode cue = kvp.second.toJSON();
    cue.set_name(ss.str());
    
    cues.push_back(cue);
  }

  list.push_back(cues);
  return list;
}

}
}