#include "SineWave.h"

namespace Lumiverse {
namespace ShowControl {
  SineWave::SineWave(float period, float magnitude, float phase, float offset, Mode mode) :
    Timeline(), _period(period), _magnitude(magnitude), _phase(phase), _offset(offset), _mode(mode)
  {

  }

  SineWave::SineWave(JSONNode node) {
    auto period = node.find("period");
    if (period == node.end()) {
      _period = 1;
    }
    else {
      _period = period->as_float();
    }

    auto magnitude = node.find("magnitude");
    if (magnitude == node.end()) {
      _magnitude = 0.5;
    }
    else {
      _magnitude = magnitude->as_float();
    }

    auto phase = node.find("phase");
    if (phase == node.end()) {
      _phase = 0;
    }
    else {
      _phase = phase->as_float();
    }

    auto offset = node.find("offset");
    if (offset == node.end()) {
      _offset = 0.5;
    }
    else {
      _offset = offset->as_float();
    }

    auto mode = node.find("mode");
    if (mode == node.end()) {
      _mode = ABS;
    }
    else {
      _mode = (Mode)mode->as_int();
    }
  }

  SineWave::~SineWave() {
    // nothing at the moment.
  }

  shared_ptr<LumiverseType> SineWave::getValueAtTime(Device* d, string paramName, size_t time, map<string, shared_ptr<Timeline> >& tls) {
    LumiverseType* currentVal = d->getParam(paramName);

    // Stop if the current value doesn't exist (unknonwn type)
    if (currentVal == nullptr)
      return nullptr;

    float t = (float)time / 1000.0f;

    // Clamp to end time if we're done with our sine loops.
    if (time > getLength()) {
      t = (float)getLength() / 1000.0f;
    }

    float wave = _magnitude * sin(M_PI * 2 * (1.0f / _period) * (t + _phase)) + _offset;
    string type = currentVal->getTypeName();

    if (type == "float") {
      LumiverseFloat* newVal = (LumiverseFloat*) LumiverseTypeUtils::copy(currentVal);
      if (_mode == ABS) {
        newVal->setValAsPercent(wave);
      }
      else if (_mode == REL) {
        newVal->setValAsPercent(newVal->asPercent() + wave);
      }
      return shared_ptr<LumiverseType>((LumiverseType*)newVal);
    }
    else if (type == "orientation") {
      LumiverseOrientation* newVal = (LumiverseOrientation*)LumiverseTypeUtils::copy(currentVal);
      if (_mode == ABS) {
        newVal->setValAsPercent(wave);
      }
      else if (_mode == REL) {
        newVal->setValAsPercent(newVal->asPercent() + wave);
      }
      return shared_ptr<LumiverseType>((LumiverseType*)newVal);
    }
    else if (type == "enum") {
      LumiverseEnum* newVal = (LumiverseEnum*)LumiverseTypeUtils::copy(currentVal);
      if (_mode == ABS) {
        newVal->setValAsPercent(wave);
      }
      else if (_mode == REL) {
        newVal->setValAsPercent(newVal->asPercent() + wave);
      }
      return shared_ptr<LumiverseType>((LumiverseType*)newVal);
    }
    else {
      Logger::log(WARN, "Unsupported type for SineWave Timeline: " + type);
    }
    return nullptr;
  }

  size_t SineWave::getLoopLength() {
    return _period * 1000;
  }

  JSONNode SineWave::toJSON() {
    JSONNode wave;
    wave.push_back(JSONNode("type", getTimelineTypeName()));
    wave.push_back(JSONNode("period", _period));
    wave.push_back(JSONNode("magnitude", _magnitude));
    wave.push_back(JSONNode("phase", _phase));
    wave.push_back(JSONNode("offset", _offset));
    wave.push_back(JSONNode("mode", _mode));

    return wave;
  }

}
}
