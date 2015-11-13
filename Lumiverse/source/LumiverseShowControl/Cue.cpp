#include "Cue.h"

namespace Lumiverse {
namespace ShowControl {

Cue::Cue(Rig* rig) : _upfade(3.0f), _downfade(3.0f), _delay(0) {
  update(rig);
}

Cue::Cue(Rig* rig, float time) : _upfade(time), _downfade(time), _delay(0) {
  update(rig);
}

Cue::Cue(DeviceSet devices, float up, float down, float delay)
  : _upfade(up), _downfade(down), _delay(delay) {
  update(devices);
}

Cue::Cue(Rig* rig, float up, float down) : _upfade(up), _downfade(down), _delay(0) {
  update(rig);
}

Cue::Cue(Rig* rig, float up, float down, float delay) : _upfade(up), _downfade(down), _delay(delay) {
  update(rig);
}

Cue::Cue(JSONNode node) {
  auto it = node.begin();
  while (it != node.end()) {
    string name = it->name();

    if (name == "upfade")
      _upfade = it->as_float();
    else if (name == "downfade")
      _downfade = it->as_float();
    else if (name == "delay")
      _delay = it->as_float();
    else if (name == "cueData") {
      // The big one. Need to load the giant cueData map.
      // devices are the top level
      Timeline::loadJSON(*it);
    }

    it++;
  }
}

Cue::Cue(const Cue& other) : Timeline(other) {
  _upfade = other._upfade;
  _downfade = other._downfade;
  _delay = other._delay;
}

Cue::~Cue() {
  // shared_ptr should keep an internal count of references and delete itself
  // after the container is destroyed.
}

void Cue::operator=(const Cue& other) {
  _upfade = other._upfade;
  _downfade = other._downfade;
  _delay = other._delay;
}

void Cue::update(Rig* rig) {
  return update(rig->getAllDevices());
}

void Cue::update(DeviceSet devices) {
  // Delete all keyframes
  deleteKeyframe(devices, 0);
  deleteKeyframe(devices, _upfade * 1000);
  deleteKeyframe(devices, _downfade * 1000);

  // Add new keyframes
  setKeyframe(devices, 0, true);
  setKeyframe(devices, _upfade * 1000, false);
}

void Cue::update(map<string, LumiverseType*> params) {
  for (auto id : params) {
    deleteKeyframe(id.first, 0);
    deleteKeyframe(id.first, _upfade * 1000);
    deleteKeyframe(id.first, _downfade * 1000);

    setKeyframe(id.first, 0, id.second, true);
    setKeyframe(id.first, _upfade * 1000, id.second, false);
  }
}

void Cue::setDelay(float delay) {
  setTime(_upfade, _downfade, delay);
}

void Cue::setTime(float time) {
  setTime(time, time, _delay);
}

void Cue::setTime(float up, float down) {
  setTime(up, down, _delay);
}

void Cue::setTime(float up, float down, float delay) {
  // Clear any delay frames first
  deleteKeyframesBetween(_upfade * 1000, _downfade * 1000);

  _upfade = up;
  _downfade = down;
  _delay = delay;
}

JSONNode Cue::toJSON() {
  JSONNode cue;
  cue.push_back(JSONNode("upfade", _upfade));
  cue.push_back(JSONNode("downfade", _downfade));
  cue.push_back(JSONNode("delay", _delay));
  cue.push_back(JSONNode("type", "cue"));

  JSONNode cueData = Timeline::toJSON();

  // Get the actual data
  cueData.set_name("cueData");
  cue.push_back(cueData);

  return cue;
}

size_t Cue::getLength() {
  // At the moment this is fine, since we're not considering
  // extra keyframe timing issues within a cue
  return _delay + max(_upfade, _downfade);
}

void Cue::setCurrentState(map<string, map<string, LumiverseType*> >& state, shared_ptr<Timeline> active, size_t time) {
  // Update keyframe times based on up or downfade
  for (const auto& d : state) {
    for (const auto& p : d.second) {
      string kid = getTimelineKey(d.first, p.first);

      // Get end keyframe
      auto lastKeyframe = _timelineData[kid].rbegin();

      // detect if up or down fade
      size_t fadeTime = 1000 * ((LumiverseTypeUtils::cmp(p.second, lastKeyframe->second.val.get()) == -1) ? _upfade : _downfade);

      // adjust keyframes
      // move last keyframe to proper position.
      moveKeyframe(kid, lastKeyframe->first, fadeTime + _delay * 1000);

      if (_delay > 0) {
        // Add new keyframe for delay
        setKeyframe(kid, (size_t) (_delay * 1000), p.second, true);
      }

      // Update as normal.
      updateKeyframeState(d.first, p.first, p.second, active, time);
    }
  }
}
  
}
}