#include "Keyframe.h"

namespace Lumiverse {
namespace ShowControl {

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

}
}