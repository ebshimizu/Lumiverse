#pragma once
#include "Timeline.h"

namespace Lumiverse {
namespace ShowControl {

  /*!
  \brief This class implements a sine-wave effect

  A demonstration of how to extend timelines to make different kinds of effects.

  This class is intended for use as a sub-timeline as an effect.
  The function for this class is: f(t) = magnitude * sin(2 pi * _period * (t + _phase)) + _offset.
  Note that this current sine-wave effect class works on proportional values, meaning that if you have two
  different floats with different ranges, say for instance [0,1] and [0,100], the sine wave with magnitude 0.5
  will have a magnitude of 0.5 and 50 respectively. 
  
  This type of timelines does not support colors. Any attempt to use a LumiverseColor with this class
  will result with getValueAtTime() returning a nullptr.
  */
  class SineWave : public Timeline
  {
  public:
    /*!
    \brief Encodes the behavior for this effect.

    ABS = Absolute. Value from the sine function will fully override the current value for the device.
    REL = Relative. Value from the side function will be added to the the current value for the device.
    */
    enum Mode {
      ABS,
      REL
    };

    /*!
    \brief Creates a new SineWave effect.

    The default sine wave is a wave that has a 1s period, magnitude of 0.5 (which makes for an amplitude of 1.0),
    no phase, and is offset so the range of the sine wave is from [0,1].
    \param period Period of the sine wave, in seconds
    \param magnitude Magnitude of the sine wave. Note that the total height for the sine wave is doubled
    \param phase Phase of the sine wave. Adjusts the starting point on the curve.
    \param offset Vertical offset of the sinewave.
    \param mode Effect mode.
    */
    SineWave(float period = 1, float magnitude = 0.5, float phase = 0, float offset = 0.5,  Mode mode = ABS);
    
    ~SineWave();

    /*!
    \brief Returns the value of the requested parameter according to the sine wave parameters.
    */
    virtual shared_ptr<LumiverseType> getValueAtTime(Device* d, string paramName, size_t time, map<string, shared_ptr<Timeline> >& tls) override;

    /*!
    \brief Returns the amount of time it takes to cycle through the sine wave once in milliseconds.
    */
    virtual size_t getLoopLength() override;

    /*!
    \brief Returns the name of this type of timeline.
    */
    virtual string getTimelineTypeName() override { return "sinewave"; }

    /*!
    \brief Converts this timeline to a JSON node.
    */
    virtual JSONNode toJSON() override;

  private:
    /*! \brief Period of the sine wave */
    float _period;

    /*! \brief Magnitude of the sine wave */
    float _magnitude;

    /*! \brief Phase of the sine wave */
    float _phase;

    /*! \brief Vertical offset of the sine wave */
    float _offset;

    /*! \brief Sine wave mode. */
    Mode _mode;
  };

}
}