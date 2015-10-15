#include "Timeline.h"

namespace Lumiverse {
namespace ShowControl {

Timeline::Timeline() {
  _loops = 1;
  _lengthIsUpdated = false;
  _loopLengthIsUpdated = false;
}

Timeline::Timeline(JSONNode data) {
  loadJSON(data);
}

Timeline::Timeline(const Timeline& other) {
  _loops = other._loops;
  _timelineData = other._timelineData;
  _events = other._events;
  _endEvents = other._endEvents;
}

Timeline::~Timeline() {
  // delete all the things
}

string Timeline::getTimelineKey(Device* d, string paramName) {
  return getTimelineKey(d->getId(), paramName);
}

string Timeline::getTimelineKey(string id, string paramName)
{
  return id + ":" + paramName;
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

void Timeline::setKeyframe(string identifier, size_t time, string timelineID, size_t offset) {
  _timelineData[identifier][time] = Keyframe(time, timelineID, offset);
  _lengthIsUpdated = false;
  _loopLengthIsUpdated = false;
}

void Timeline::setKeyframe(Device* d, size_t time, string timelineID, size_t offset) {
  for (const auto& param : d->getParamNames()) {
    setKeyframe(getTimelineKey(d, param), time, timelineID, offset);
  }
}

void Timeline::setKeyframe(DeviceSet devices, size_t time, string timelineID, size_t offset) {
  for (const auto& d : devices.getDevices()) {
    setKeyframe(d, time, timelineID, offset);
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
    for (auto it = r.first; it != r.second;) {
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

shared_ptr<LumiverseType> Timeline::getValueAtTime(string  id, string paramName, LumiverseType* currentVal, size_t time, map<string, shared_ptr<Timeline> >& tls) {
  string identifier = getTimelineKey(id, paramName);
  time = getLoopTime(time);

  try {
    auto keyframes = _timelineData.at(identifier);

    // If the id has a keyframe map, but no keyframes, we do nothing and return null.
    if (keyframes.size() == 0)
      return nullptr;

    Keyframe first;
    Keyframe next;
    bool nextFound = false;

    for (auto keyframe = keyframes.begin(); keyframe != keyframes.end();) {
      if (keyframe->first > time) {
        next = keyframe->second;

        // Special case if they keyframe we found is after the current time but there is no keyframe
        // before the keyframe we found. Example: no keyframe at t = 0 but keyframe at t = 1200, with
        // t currently equal to 50.
        if (keyframe == keyframes.begin()) {
          first = next;
        }
        else {
          first = prev(keyframe)->second;
        }
        nextFound = true;
        break;
      }

      ++keyframe;
    }

    if (!nextFound) {
      // We are at the end of the defined keyframes, so return the value of the most
      // recent keyframe
      auto last = keyframes.rbegin()->second;

      if (last.timelineID != "") {
        if (tls.count(last.timelineID) > 0) {
          return tls[last.timelineID]->getValueAtTime(id, paramName, currentVal, time - last.t + last.timelineOffset, tls);
        }
        else return nullptr;
      }

      return last.val;
    }

    // Note that in the instance when we use the current state, that value is pre-filled
    // at the time of timeline run initialization.

    // Otherwise we have our keyframes and can now do some ops.
    float a = (float)(time - first.t) / (float)(next.t - first.t);

    shared_ptr<LumiverseType> x = first.val;
    shared_ptr<LumiverseType> y = next.val;

    // Check if any keyframe references timelines
    // If no such timeline exists in the playback, return nullptr (indicate to layer to skip value for this)
    if (first.timelineID != "") {
      if (tls.count(first.timelineID) > 0) {
        x = tls[first.timelineID]->getValueAtTime(id, paramName, currentVal, time - first.t + first.timelineOffset, tls);
      }
      else return nullptr;
    }
    if (next.timelineID != "") {
      if (tls.count(next.timelineID) > 0) {
        y = tls[next.timelineID]->getValueAtTime(id, paramName, currentVal, time - next.t + next.timelineOffset, tls);
      }
      else return nullptr;
    }

    return LumiverseTypeUtils::lerp(x.get(), y.get(), a);
  }
  catch (exception e) {
    return nullptr;
  }
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
  JSONNode timeline;
  timeline.push_back(JSONNode("type", getTimelineTypeName()));
  timeline.push_back(JSONNode("loops", _loops));
  
  // timeline data
  JSONNode keyframes;
  keyframes.set_name("keyframes");
  for (auto& kvp : _timelineData) {
    JSONNode kfList;
    kfList.set_name(kvp.first);
    for (auto& kfs : kvp.second) {
      JSONNode keyframe = kfs.second.toJSON();
      stringstream ss;
      ss << kfs.first;
      keyframe.set_name(ss.str());
      kfList.push_back(keyframe);
    }
    keyframes.push_back(kfList);
  }
  timeline.push_back(keyframes);

  JSONNode events;
  events.set_name("events");
  for (auto& kvp : _events) {
    JSONNode e = kvp.second->toJSON();
    e.push_back(JSONNode("t", (unsigned long)kvp.first));
    events.push_back(e);
  }
  timeline.push_back(events);

  JSONNode endEvents;
  endEvents.set_name("endEvents");
  for (auto& kvp : _endEvents) {
    JSONNode endEvent = kvp.second->toJSON();
    endEvent.set_name(kvp.first);
    endEvents.push_back(endEvent);
  }
  timeline.push_back(endEvents);

  return timeline;
}

bool Timeline::isDone(size_t time, map<string, shared_ptr<Timeline> >& tls) {
  // Automatically return false if set to infinite loop.
  if (_loops == -1)
    return false;

  if (time > getLength()) {
    // Before returning true, check to see if any timelines are still running
    // in the end keyframes
    for (const auto& id : _timelineData) {
      if (id.second.size() == 0)
        continue;

      auto lastKeyframe = id.second.rbegin()->second;

      if (lastKeyframe.timelineID != "") {
        if (tls.count(lastKeyframe.timelineID) > 0) {
          if (!tls[lastKeyframe.timelineID]->isDone(time - lastKeyframe.t + lastKeyframe.timelineOffset, tls)) {
            return false;
          }
        }
      }

    }
    
    return true;
    
  }

  return false;
}

void Timeline::setCurrentState(map<string, map<string, LumiverseType*> >& state, shared_ptr<Timeline> active, size_t time) {
  for (const auto& d : state) {
    for (const auto& p : d.second) {
      updateKeyframeState(d.first, p.first, p.second, active, time);
    }
  }
}

void Timeline::updateKeyframeState(string id, string paramName, LumiverseType* param, shared_ptr<Timeline> tl, size_t time) {
  string kid = getTimelineKey(id, paramName);
  for (auto& kf : _timelineData[kid]) {
    if (kf.second.useCurrentState) {
      // check for active subtimelines
      if (tl != nullptr) {
        Keyframe activeKeyframe = tl->getPreviousKeyframe(getTimelineKey(id, paramName), time);
        if (activeKeyframe.timelineID != "") {
          kf.second.timelineID = activeKeyframe.timelineID;
          kf.second.timelineOffset = tl->getLoopTime(time) - activeKeyframe.t + activeKeyframe.timelineOffset + kf.second.t;
        }
        else {
          kf.second.val = shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(param));
        }
      }
      else {
        kf.second.val = shared_ptr<LumiverseType>(LumiverseTypeUtils::copy(param));
      }
    }
  }
}

Keyframe Timeline::getPreviousKeyframe(string identifier, size_t time) {
  // at the moment this function is horribly slow.
  // TODO: update keyframe datastructure to access elements faster.

  // get the keyframes if they exist, otherwise return null immediately.
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
    auto last = keyframes.rbegin()->second;
    return last;
  }

  return first;
}

size_t Timeline::getLength() {
  if (_lengthIsUpdated) {
    return _length;
  }
  else {
    if (_loops == -1) {
      // this should be max int in whatever unsigned int representation is used for size_t.
      _length = -1;
      _lengthIsUpdated = true;
      return _length;
    }
    else {
      _length = getLoopLength() * _loops;

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
      if (id.second.size() == 0)
        continue;
      
      auto lastKeyframe = id.second.rbegin()->first;

      // Time is equal to transition time + largest keyframe time.
      time = (lastKeyframe > time) ? lastKeyframe : time;
    }

    // also check events
    if (_events.size() > 0) {
      time = (_events.rbegin()->first > time) ? _events.rbegin()->first : time;
    }

    _loopLength = time;

    // Cache it.
    _loopLengthIsUpdated = true;
    return _loopLength;
  }
}

size_t Timeline::getLoopTime(size_t time) {
  // determine where we are in the loop
  if (getLoopLength() == 0)
    return 0;

  int loopNum = (int)(time / getLoopLength());
  if (_loops != -1 && loopNum >= _loops) {
    // if we've exceeded our number of loops, set to the end keyframe.
    return time;
  }
  else {
    time -= loopNum * getLoopLength();
  }

  return time;
}

void Timeline::loadJSON(JSONNode node) {
  auto type = node.find("type");
  if (type == node.end()) {
    Logger::log(ERR, "Can't load timeline. Unknown type.");
    return;
  }
  else {
    if (type->as_string() != getTimelineTypeName()) {
      Logger::log(ERR, "Can't loat timeline. Type \"" + type->as_string() + "\" is not a \"timeline\"");
      return;
    }
  }

  auto loops = node.find("loops");
  if (loops == node.end()) {
    _loops = 1;
  }
  else {
    _loops = loops->as_int();
  }

  auto keyframes = node.find("keyframes");
  if (keyframes == node.end()) {
    Logger::log(WARN, "No keyframes found in timeline.");
  }
  else {
    auto ident = keyframes->begin();
    // Loop over identifiers
    while (ident != keyframes->end()) {
      string identifier = ident->name();

      // loop over keyframes in each identifier
      auto kfList = ident->begin();
      while (kfList != ident->end()) {
        Keyframe kf(*kfList);
        _timelineData[identifier][kf.t] = kf;
        kfList++;
      }
      ident++;
    }
  }

  auto events = node.find("events");
  if (events == node.end()) {
    // We don't really need to note anything here
    // but we'll leave the option here.
  }
  else {
    auto e = events->begin();
    while (e != events->end()) {
      auto type = e->find("type");
      auto time = e->find("time");
      if (type != e->end() && time != e->end()) {
        string t = type->as_string();
        if (t == "event") {
          _events.insert(pair<size_t, shared_ptr<Event> >(time->as_int(), shared_ptr<Event>(new Event(*e))));
        }
      }
      else {
        Logger::log(ERR, "Unable to add event. Cannot determine type or time.");
      }

      e++;
    }
  }

  auto endEvents = node.find("endEvents");
  if (endEvents == node.end()) {
    // No notice needed, but maybe something here later
  }
  else {
    auto e = endEvents->begin();
    while (e != endEvents->end()) {
      auto type = e->find("type");
      if (type != e->end()) {
        string t = type->as_string();
        if (t == "event") {
          _endEvents[e->name()] = shared_ptr<Event>(new Event(*e));
        }
      }
      else {
        Logger::log(ERR, "Unable to add end event. Cannot determine type or time.");
      }

      e++;
    }
  }
}

}
}