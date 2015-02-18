#include "Keyframe.h"

namespace Lumiverse {
namespace ShowControl {

JSONNode Keyframe::toJSON() {
  JSONNode keyframe;
  keyframe.push_back(JSONNode("t", (unsigned long) t));
  if (val != nullptr) {
    keyframe.push_back(val->toJSON("val"));
  }
  keyframe.push_back(JSONNode("useCurrentState", useCurrentState));
  keyframe.push_back(JSONNode("timelineID", timelineID));
  keyframe.push_back(JSONNode("timelineOffset", (unsigned long) timelineOffset));

  return keyframe;
}

Event::Event(function<void()> cb, string id) : _id(id) {
  _callback = cb;
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