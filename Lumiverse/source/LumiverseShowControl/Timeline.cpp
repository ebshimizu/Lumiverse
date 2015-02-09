#include "Timeline.h"

namespace Lumiverse {
namespace ShowControl {

Timeline::Timeline() {
  _loops = 1;
}

Timeline::Timeline(JSONNode data) {
  // TODO: fill in later
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

  return ret;
}

map<string, map<size_t, Keyframe> >& Timeline::getAllKeyframes() {
  _lengthIsUpdated = false;
  _loopLengthIsUpdated = false;
  return _timelineData;
}

void Timeline::setKeyframe(string identifier, size_t time, LumiverseType* data, bool ucs) {
  _timelineData[identifier][time] = Keyframe(time, shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(data)), ucs);
  _lengthIsUpdated = false;
  _loopLengthIsUpdated = false;
}

void Timeline::setKeyframe(Device* d, size_t time, bool ucs) {
  for (const auto& param : d->getParamNames()) {
    setKeyframe(getTimelineKey(d, param), time, d->getParam(param), ucs);
  }
}

void Timeline::setKeyframe(Rig* rig, size_t time, bool ucs) {
  setKeyframe(rig->getAllDevices(), time, ucs);
}

void Timeline::setKeyframe(DeviceSet devices, size_t time, bool ucs) {
  for (const auto& d : devices.getDevices()) {
    setKeyframe(d, time, ucs);
  }
}

void Timeline::deleteKeyframe(string identifier, size_t time) {
  _timelineData[identifier].erase(time);
  _lengthIsUpdated = false;
  _loopLengthIsUpdated = false;
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

bool Timeline::addEndEvent(string id, shared_ptr<Event> e) {
  if (_endEvents.count(id) > 0) {
    return false;
  }

  _endEvents[id] = e;
  return true;
}

void Timeline::deleteEndEvent(string id) {
  _endEvents.erase(id);
}

bool Timeline::addEvent(size_t time, shared_ptr<Event> e) {
  _events.insert(pair<size_t, shared_ptr<Event> >(time, e));
  return true;
}

void Timeline::deleteEvent(size_t time, string id) {
  if (id != "") {
    // get all elements in range
    auto r = _events.equal_range(time);
    for (auto it = r.first; it != r.second; ) {
      if (it->second->_id == id) {
        it = _events.erase(it);
      }
      else {
        ++it;
      }
    }
  }
  else {
    _events.erase(time);
  }
}

vector<shared_ptr<Event> > Timeline::getEvents(size_t time, string id) {
  vector<shared_ptr<Event> > ret;
  
  auto r = _events.equal_range(time);
  for (auto it = r.first; it != r.second; it++) {
    if (id != "") {
      if (id == it->second->_id) {
        ret.push_back(it->second);
      }
    }
    else {
      ret.push_back(it->second);
    }
  }

  return ret;
}

shared_ptr<Event> Timeline::getEndEvent(string id) {
  if (_endEvents.count(id) == 0) {
    return nullptr;
  }

  return _endEvents[id];
}

multimap<size_t, shared_ptr<Event> >& Timeline::getAllEvents() {
  return _events;
}

map<string, shared_ptr<Event> >& Timeline::getAllEndEvents() {
  return _endEvents;
}

shared_ptr<LumiverseType> Timeline::getValueAtTime(string identifier, size_t time) {
  // get the keyframes if they exist, otherwise return null immediately.
  if (_timelineData.count(identifier) == 0)
    return nullptr;

  time = getLoopTime(time);

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
  float a = (float)(time - first.t) / (float)(next.t - first.t);
  return LumiverseTypeUtils::lerp(first.val.get(), next.val.get(), a);
}

void Timeline::executeEvents(size_t prevTime, size_t currentTime) {
  prevTime = getLoopTime(prevTime);
  currentTime = getLoopTime(currentTime);

  auto low = _events.upper_bound(prevTime);
  auto high = _events.upper_bound(currentTime);

  if (low == _events.end()) {
    return;
  }

  for (auto it = low; it != high; it++) {
    it->second->execute();
  }
}

void Timeline::executeEndEvents() {
  for (const auto& kvp : _endEvents) {
    kvp.second->execute();
  }
}

int Timeline::getLoops() {
  return _loops;
}

void Timeline::setLoops(int loops) {
  _loops = loops;
}

JSONNode Timeline::toJSON() {
  // TODO: Fill in the blanks
  return JSONNode();
}

bool Timeline::isDone(size_t time) {
  // TODO: when nested timelines get added this will be more complicated.
  if (time > getLength())
    return true;

  return false;
}

void Timeline::setCurrentState(map<string, Device*>& state) {
  for (const auto& d : state) {
    for (const auto& p : d.second->getParamNames()) {
      updateKeyframeState(d.second, p);
    }
  }
}

void Timeline::updateKeyframeState(Device* d, string paramName) {
  string id = getTimelineKey(d, paramName);
  for (auto& kf : _timelineData[id]) {
    if (kf.second.useCurrentState) {
      kf.second.val = shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(d->getParam(paramName)));
    }
  }
}

size_t Timeline::getLength() {
  if (_lengthIsUpdated) {
    return _length;
  }
  else {
    if (_loops == -1) {
      // this should be max int in whatever unsigned int representation is used for size_t.
      return -1;
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

      _length = time * _loops;

      // Cache it.
      _lengthIsUpdated = true;
      return _length;
    }
  }
}

size_t Timeline::getLoopLength() {
  if (_loopLengthIsUpdated) {
    return _loopLength;
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

    _loopLength = time;

    // Cache it.
    _loopLengthIsUpdated = true;
    return _loopLength;
  }
}

size_t Timeline::getLoopTime(size_t time) {
  // determine where we are in the loop
  int loopNum = (int)(time / getLoopLength());
  if (loopNum >= _loops) {
    // if we've exceeded our number of loops, set to the end keyframe.
    time = getLoopLength();
  }
  else {
    time -= loopNum * getLoopLength();
  }

  return time;
}

}
}