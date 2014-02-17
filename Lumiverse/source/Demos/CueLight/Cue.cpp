#include "Cue.h"

Cue::Cue(Rig* rig, float time) : m_upfade(time), m_downfade(time) {
  update(rig);
}

Cue::Cue(Rig* rig, float up, float down) : m_upfade(up), m_downfade(down) {
  update(rig);
}

Cue::~Cue() {
  for (auto m : m_cueData) {
    for (auto t : m.second) {
      delete t.second;
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