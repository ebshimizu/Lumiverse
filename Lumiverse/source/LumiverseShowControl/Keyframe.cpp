#include "Keyframe.h"

namespace Lumiverse {
namespace ShowControl {

Keyframe::Keyframe(JSONNode node) {
  auto jt = node.find("t");
  if (jt == node.end()) {
    Logger::log(ERR, "Keyframe has no assigned time. Defaulting to 0.");
    t = 0;
  }
  else {
    t = jt->as_int();
  }

  auto v = node.find("val");
  if (v == node.end()) {
    val = nullptr;
  }
  else {
    val = shared_ptr<LumiverseType>(LumiverseTypeUtils::loadFromJSON(*v));
  }

  auto ucs = node.find("useCurrentState");
  if (ucs == node.end()) {
    useCurrentState = false;
  }
  else {
    useCurrentState = ucs->as_bool();
  }

  auto tid = node.find("timelineID");
  if (tid == node.end()) {
    timelineID = "";
  }
  else {
    timelineID = tid->as_string();
  }

  auto to = node.find("timelineOffset");
  if (to == node.end()) {
    timelineOffset = 0;
  }
  else {
    timelineOffset = to->as_int();
  }
}

JSONNode Keyframe::toJSON() {
  JSONNode keyframe;
  keyframe.push_back(JSONNode("t", (unsigned long) t));
  if (val != nullptr) {
    keyframe.push_back(val->toJSON("val"));
  }
  keyframe.push_back(JSONNode("useCurrentState", useCurrentState));

  if (timelineID != "") {
    keyframe.push_back(JSONNode("timelineID", timelineID));
    keyframe.push_back(JSONNode("timelineOffset", (unsigned long)timelineOffset));
  }

  return keyframe;
}

Event::Event(function<void()> cb, string id) : _id(id) {
  _callback = cb;
}

Event::Event(JSONNode node) {
  _id = node.name();
  _callback = [](){ Logger::log(WARN, "Event loaded from JSON node has no callback. Update this in code."); };
}

Event::~Event() {
  // destructor if necessary
}

void Event::execute() {
  _callback();
}

void Event::reset() {
  // nothing for the base class.
}

void Event::setCallback(function<void()> cb) {
  _callback = cb;
}

JSONNode Event::toJSON() {
  // this is equivalent to saying "something happened here but I have no idea what it is"
  JSONNode e;
  e.set_name(_id);
  e.push_back(JSONNode("type", getType()));

  return e;
}

}
}