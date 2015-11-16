#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>
#include <memory>
#include "Timeline.h"

using namespace std;

namespace Lumiverse {
namespace ShowControl {
/*!
\brief A cue stores data for a particular look (called a cue)

In the traditional theatrical sense, cues are preset configurations of lighting device parameters.
Cues are placed into cue lists and are then transitioned between with a simple cross fade
when a user presses a button (typically labeled "GO").
 
Cues in Lumiverse are timelines, where each parameter can be assigned a 
value at an arbitrary time. Cue transitions are handled by doing an linear interpolation between
the last keyframe of the previous cue, and the first keyframe of the current cue.
 
Cuees are specifically meant to replicate traditional lighting console cueing systems.
If you just need standard animation timeline interface, see Timeline.

Cues have all the functinoality of a Lumiverse Timeline while providing some convenience
functions to make programming easier.
*/
class Cue : public Timeline {
public:
  /*!
  \brief Makes a blank cue.
  */
  Cue() : _upfade(3.0f), _downfade(3.0f) { }

  /*!
  \brief Constructs a cue from a rig. Default time is 3.

  This constructor will make a cue by pulling all of the values from the Rig
  and storing them in the cue. It essentially "takes a picture" of the rig for use later.
  \param rig Rig to create the cue from.
  */
  Cue(Rig* rig);

  /*!
  \brief Creates a cue from the current state of the rig with the given time.
  \param time Fade time for the cue
  */
  Cue(Rig* rig, float time);

  /*!
  \brief Creates a cue from a set of devices.
  
  Generally this gets called by Layers or by the Programmer when creating a cue.
  \param devices Map of devices to store. 
  \param time Default cue timing to use
  */
  Cue(DeviceSet devices, float up, float down, float delay);

  // Creates a cue with different up and down fades.
  Cue(Rig* rig, float up, float down);

  // Creates a cue with different up and down fades, and a different delay
  Cue(Rig* rig, float up, float down, float delay);

  Cue(map<string, Device*> devices, float up, float down, float delay);

  /*! \brief Creates a cue from a JSON node. */
  Cue(JSONNode node);

  /*!
  \brief Copy a cue.
  */
  Cue(const Cue& other);

  // Destructor
  ~Cue();

  /*!
  \brief Does a deep copy of the cue data.
  */
  void operator=(const Cue& other);

  string getTimelineTypeName() override { return "cue"; }

  /*!
  \brief Updates the changes between the rig and this cue.
  
  Tracking happens at the cue list level
  Returns a mapping of device id -> changed parameter and old value.
  Note that if you have internal cues, this will try to track changes through to those keyframes.
  \param rig Rig that we're looking at for the update
  */
  void update(Rig* rig);

  // Slightly different arguments, acts the same as update(Rig*)
  void update(DeviceSet devices);

  /*!
  \brief Updates the cue with the values specified for a particular identifier.
  */
  void update(map<string, LumiverseType*> params);

  /*!
  \brief Updates a single parameter in the cue.
  */
  void update(string id, string param, LumiverseType* data);

  /*!
  \brief Sets the delay for a cue
  
  The delay tells the cue how long to wait before actually animating anything.
  \param delay Delay in seconds.
  */
  void setDelay(float delay);

  /*!
  \brief Sets the upfade and downfade to the specified value.
  \param time Upfade/downfade in seconds.
  */
  void setTime(float time);

  /*!
  \brief Sets a time with an up and down fade that are different.
  \param up Upfade in seconds
  \param down Downfade in seconds
  */
  void setTime(float up, float down);

  /*!
  \brief Sets a time with an up, down, and delay for the cue.
  \param up Upfade in seconds
  \param down Downfade in seconds
  \param delay Delay in seconds
  */
  void setTime(float up, float down, float delay);

  // Gets the upfade
  float getUpfade() { return _upfade; }

  // Gets the downfade
  float getDownfade() { return _downfade; }

  /*! \brief Gets the delay from a cue. */
  float getDelay() { return _delay; }

  /*! \brief Returns the JSON representation of the cue. */
  JSONNode toJSON() override;
  
  /*!
  \brief Returns the total transition time of the cue.
   
  \return `max(upfade, downfade) + delay`
  */
  float getTransitionTime() { return max(_upfade, _downfade) + _delay; }

  /*!
  Adjusts timing of cues depending on if they are an upfade or a downfade
  */
  void setCurrentState(map<string, map<string, LumiverseType*> >& state, shared_ptr<Timeline> active, size_t time) override;

  /*!
  \brief Gets the static value of the last keyframe in the cue if it exists.
  */
  shared_ptr<LumiverseType> getLastCueValue(string id, string paramName);

private:
  // Upfade time
  float _upfade;

  // Downfade time
  float _downfade;

  // Delay before doing any fades, default timing.
  float _delay;

  // Reserved for future use.
  // m_follow - cue follow time (time to wait before automatically taking the next cue)
};

}
}
#endif