#include "CueList.h"

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
      //tracking->second.

      ++tracking;
    }

  }
  else {
    // Discard returned param. shared_ptr should free memory.
    m_cues[num].update(rig);
  }
}