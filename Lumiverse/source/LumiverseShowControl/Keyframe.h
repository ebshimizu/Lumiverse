#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

#pragma once

#include "LumiverseCore.h"

namespace Lumiverse {
namespace ShowControl {

/*!
\brief A Keyframe stores the value of a parameter at the specified time.

Keyframes can be set to use the current state of the layer it's being played back on.
Useful for chaining timelines together, as in a series of cues.
*/
struct Keyframe {
  /*!
  \brief Time at which this keyframe is located. t=0 is start of timeline.

  The time is stored in terms of milliseconds (10e-3).
  */
  size_t t;

  /*!
  \brief Value of the keyframe at time t.
  */
  shared_ptr<Lumiverse::LumiverseType> val;

  /*!
  \brief If true, the value of this keyframe will be pulled from the state of the layer when playback starts. 

  Note that the keyframe will still have a value, but it won't be used unless there is no previous cue.
  */
  bool useCurrentState;

  // Planned interpolation mode selection here. Additional parameters probably needed
  // once this thing gets activated
  // enum interpMode

  bool operator<(Keyframe other) const {
    return t < other.t;
  }

  /*! \brief Empty constructor */
  Keyframe() { }

  /*!
  \brief Constructor with all values filled in.
  \param time Keyframe temporal location
  \param v Value at specified time
  \param uct Use Cue Timing (see useCueTiming member variable)
  */
  Keyframe(size_t time, shared_ptr<Lumiverse::LumiverseType> v, bool upv) :
    t(time), val(v), useCurrentState(upv) { }

  /*!
  \brief Constructor creates a blank keyframe at specified time
  */
  Keyframe(size_t time) : t(time) { }

  /*! \brief Creates a keyframe from a JSON node. */
  Keyframe(JSONNode node);
};

/*!
\brief Events are special keyframes that trigger events when encountered.

Events can really be used anywhere to do anything within the limits of the program.
There are a few pre-defied event classes to make certain common tasks easier (such
as start playback on Layer, go to next timeline, 
*/
class Event {
public:
  Event(size_t time, function<void()> cb);
  ~Event();

  void execute();

  size_t time;
  function<void()> callback;
};

}
}
#endif