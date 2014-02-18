#include "Cue.h"

Cue::Cue(Rig* rig) : m_upfade(3.0f), m_downfade(3.0f) {
  update(rig);
}

Cue::Cue(Rig* rig, float time) : m_upfade(time), m_downfade(time) {
  update(rig);
}

Cue::Cue(Rig* rig, float up, float down) : m_upfade(up), m_downfade(down) {
  update(rig);
}

Cue::Cue(Cue& other) {
  m_upfade = other.m_upfade;
  m_downfade = other.m_downfade;

  // Fully copy over cue data. Other instance may go out of scope whenever and
  // delete the cue data.
  for (auto it : other.m_cueData) {
    for (auto param : it.second) {
      m_cueData[it.first][param.first] = LumiverseTypeUtils::copy(param.second);
    }
  }
}

Cue::~Cue() {
  for (auto m : m_cueData) {
    for (auto t : m.second) {
      delete t.second;
    }
  }
}

void Cue::operator=(const Cue& other) {
  m_upfade = other.m_upfade;
  m_downfade = other.m_downfade;

  // Fully copy over cue data. Other instance may go out of scope whenever and
  // delete the cue data.
  for (auto it : other.m_cueData) {
    for (auto param : it.second) {
      m_cueData[it.first][param.first] = LumiverseTypeUtils::copy(param.second);
    }
  }
}

Cue::changedParams Cue::update(Rig* rig) {
  changedParams params;
  
  for (auto d : *(rig->getDeviceRaw())) {
    if (m_cueData.count(d->getId()) == 0) {
      // New cues don't send back changed parameters since there weren't really
      // things to change before they got added.
      m_cueData[d->getId()] = getParams(d);
    }
    else {
      map<string, shared_ptr<LumiverseType>> changed;
      updateParams(d, changed);

      if (changed.size() > 0) {
        params[d->getId()] = changed;
      }
    }
  }

  return params;
}

void Cue::trackedUpdate(Cue::changedParams& oldVals, Rig* rig) {
  auto it = oldVals.begin();

  while (it != oldVals.end()) {
    auto params = it->second.begin();

    while (params != it->second.end()) {
      // Update if the param is the same as in oldVal
      if (LumiverseTypeUtils::equals(m_cueData[it->first][params->first], params->second.get())) {
        LumiverseTypeUtils::copyByVal(rig->getDevice(it->first)->getParam(params->first), m_cueData[it->first][params->first]);
        ++params;
      }
      // Otherwise stop tracking changes
      else {
        oldVals[it->first].erase(params++);
      }
    }

    // If a device has no changes to track, delete it from the tracking list.
    if (oldVals[it->first].size() == 0) {
      oldVals.erase(it++);
    }
    else {
      ++it;
    }
  }
}

void Cue::setTime(float time) {
  setTime(time, time);
}

void Cue::setTime(float up, float down) {
  m_upfade = up;
  m_downfade = down;
}

map<string, LumiverseType*> Cue::getParams(Device* d) {
  map<string, LumiverseType*> params;

  // Copy all parameters to cue data list
  for (auto a : *(d->getRawParameters())) {
    params[a.first] = LumiverseTypeUtils::copy(a.second);
  }

  return params;
}

void Cue::updateParams(Device* d, map<string, shared_ptr<LumiverseType>>& changed) {
  for (auto p : m_cueData[d->getId()]) {
    // If parameter data is new, replace this cue's data with the new data.
    if (!LumiverseTypeUtils::equals(d->getParam(p.first), p.second)) {
      // Make a full copy of the old data before overwriting old data.
      changed[p.first] = shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(p.second));

      LumiverseTypeUtils::copyByVal(d->getParam(p.first), p.second);
    }
  }
}