#include "Timeline.h"

namespace Lumiverse {
namespace ShowControl {

Timeline::Timeline() {

}

Timeline::~Timeline() {
  // delete all the things
}

string Timeline::getTimelineKey(Device* d, string paramName) {
  return d->getId() + ":" + paramName;
}

Keyframe Timeline::getKeyframe(string identifier, size_t time) {
  return _timelineData[identifier][time];
}

map<string, Keyframe> Timeline::getKeyframes(Device* d, size_t time) {
  map<string, Keyframe> ret;
  for (const auto& param : d->getParamNames()) {
    if (_timelineData[getTimelineKey(d, param)].count(time) == 0) {
      ret[param] = Keyframe();
    }
    ret[param] = _timelineData[getTimelineKey(d, param)][time];
  }
}

map<string, map<size_t, Keyframe> >& const Timeline::getAllKeyframes() {
  _lengthIsUpdated = false;
  return _timelineData;
}

void Timeline::setKeyframe(string identifier, size_t time, LumiverseType* data, bool ucs) {
  _timelineData[identifier][time] = Keyframe(time, shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(data)), ucs);
  _lengthIsUpdated = false;
}

void Timeline::setKeyframe(Device* d, size_t time, bool ucs = false) {
  for (const auto& param : d->getParamNames()) {
    setKeyframe(getTimelineKey(d, param), time, d->getParam(param), ucs);
  }
}

void Timeline::setKeyframe(Rig* rig, size_t time, bool ucs = false) {
  for (const auto& d : rig->getAllDevices().getDevices()) {
    setKeyframe(d, time, ucs);
  }
}

void Timeline::setKeyframe(DeviceSet devices, size_t time, bool ucs) {
  for (const auto& d : devices.getDevices()) {
    setKeyframe(d, time, ucs);
  }
}

void Timeline::deleteKeyframe(string identifier, size_t time) {
  _timelineData[identifier].erase(time);
  _lengthIsUpdated = false;
}

void Timeline::deleteKeyframe(Device* d, size_t time) {
  for (const auto& param : d->getParamNames()) {
    deleteKeyframe(getTimelineKey(d, param), time);
  }
}

void Timeline::deleteKeyframe(DeviceSet devices, size_t time) {
  for (const auto& d : devices.getDevices()) {
    deleteKeyframe(d, time);
  }
}

shared_ptr<LumiverseType> Timeline::getValueAtTime(string identifier, size_t time) {
  // get the keyframes if they exist, otherwise return null immediately.
  if (_timelineData.count(identifier) == 0)
    return nullptr;

  auto keyframes = _timelineData[identifier];
  Keyframe first;
  Keyframe next;
  bool nextFound = false;

  for (auto keyframe = keyframes.begin(); keyframe != keyframes.end();) {
    if (keyframe->first > time) {
      next = keyframe->second;
      first = prev(keyframe)->second;
      nextFound = true;
      break;
    }

    ++keyframe;
  }

  if (!nextFound) {
    // We are at the end of the defined keyframes, so return the value of the most
    // recent keyframe
    return keyframes.rbegin()->second.val;
  }

  // Note that in the instance when we use the current state, that value is pre-filled
  // at the time of timeline run initialization.

  // Otherwise we have our keyframes and can now do some ops.
  float a = (time - first.t) / (next.t - first.t);
  return LumiverseTypeUtils::lerp(first.val.get(), next.val.get(), a);
}

size_t Timeline::getLength() {
  if (_lengthIsUpdated) {
    return _length;
  }
  else {
    size_t time = 0;

    // the hard way.
    // Go through and find the maximum time that a keyframe is set to.
    for (const auto& id : _timelineData) {
      // Get the last keyframe, this is sorted.
      auto lastKeyframe = id.second.rbegin()->first;

      // Time is equal to transition time + largest keyframe time.
      time = (lastKeyframe > time) ? lastKeyframe : time;
    }

    _length = time;

    // Cache it.
    _lengthIsUpdated = true;
    return _length;
  }
}

}
}