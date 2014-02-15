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

void Cue::update(Rig* rig) {
  for (auto d : *(rig->getDeviceRaw())) {
    if (m_cueData.count(d->getId()) == 0) {
      m_cueData[d->getId()] = getParams(d);
    }
    else {
      updateParams(d);
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

void Cue::updateParams(Device* d) {
  for (auto p : m_cueData[d->getId()]) {
    // If parameter data is new, replace this cue's data with the new data.
    if (!LumiverseTypeUtils::equals(d->getParam(p.first), p.second)) {
      // Remember that the LumiverseType* in the cue data was allocated by new
      // and delete it to prevent memory leaks.
      delete p.second;

      p.second = LumiverseTypeUtils::copy(d->getParam(p.first));
    }
  }
}