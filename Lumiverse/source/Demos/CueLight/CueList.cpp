#include "CueList.h"

namespace Lumiverse {

CueList::CueList()
{
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
  if (current == m_cues.begin()) {
    return nullptr;
  }

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

}