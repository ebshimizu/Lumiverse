#include "SineWave.h"

namespace Lumiverse {
namespace ShowControl {
  SineWave::SineWave(float period, float magnitude, float phase, float offset, Mode mode) :
    Timeline(), _period(period), _magnitude(magnitude), _phase(phase), _offset(offset), _mode(mode)
  {
    
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
  }

  size_t SineWave::getLoopLength() {
    return _period * 1000;
  }

}
}
