#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>
#include <memory>

#ifdef _MSC_VER
#pragma warning(disable : 4503) // Debug symbol name too long for MSVS, pretty sure it's line 172
#endif

namespace Lumiverse {

struct Keyframe {
  /*! \brief Time at which this keyframe is located. t=0 is start of timeline. */
  float t;

  /*!
  \brief Value of the keyframe at time t
  
  If this is nullptr, the keyframe is at the end of a cue and should take its value
  from the next cue in the sequence.
  */
  shared_ptr<Lumiverse::LumiverseType> val;

  /*!
  \brief If true, t will be set to [previous keyframe time] + up/down fade time at runtime.

  If set to false, it will use the time specified in t.
  Has no effect if val is not-null.
  */
  bool useCueTiming;

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
  Keyframe(float time, shared_ptr<Lumiverse::LumiverseType> v, bool uct) :
    t(time), val(v), useCueTiming(uct) { }

  /*! \brief */
  Keyframe(JSONNode node);
};

/*!
\brief A cue stores data for a particular look (called a cue)

Cues in Lumiverse can be thought of as little mini-timelines.
Each cue is able to store keyframes for every device parameter
placed at arbitrary times in the cue. Cues can be run as stand-alone
timelines or can be chained together in a cue list. In that mode, the last
keyframe of each cue pulls its data from the destination cue.

Cues can be transitioned between, typically as a crossfade.
This class currently stores just a set look and transitions with upfade
and downfade time.

Note that the fade time applies when going from the current cue to the
next cue. So if you have two Cues, cue 1 with time 3, and cue 2 with time 5,
if you go from cue 1 to cue 2 the transition will happen in 3 seconds (assuming
there are no additional keyframes added).
*/
class Cue {
public:
  typedef map<string, map<string, shared_ptr<Lumiverse::LumiverseType> > > changedParams;

  /*!
  \brief Makes a blank cue.
  */
  Cue() : m_upfade(3.0f), m_downfade(3.0f) { }

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
  Cue(map<string, Device*> devices, float time);

  // Creates a cue with different up and down fades.
  Cue(Rig* rig, float up, float down);

  // Creates a cue with different up and down fades, and a different delay
  Cue(Rig* rig, float up, float down, float delay);

  /*! \brief Creates a cue from a JSON node. */
  Cue(JSONNode node);

  /*!
  \brief Copy a cue.
  */
  Cue(Cue& other);

  // Destructor
  ~Cue();

  /*!
  \brief Does a deep copy of the cue data.
  */
  void operator=(const Cue& other);

  // Modifiers

  /*!
  \brief Updates the changes between the rig and this cue.
  
  Tracking happens at the cue list level
  Returns a mapping of device id -> changed parameter and old value.
  Note that if you have internal cues, this will try to track changes through to those keyframes.
  \param rig Rig that we're looking at for the update
  */
  changedParams update(Rig* rig);

  // Slightly different arguments, acts the same as update(Rig*)
  changedParams update(map<string, Device*> devices);

  // Only updates the devices with IDs in the changedParams.
  // Will remove IDs in oldVals if parameters don't match the older values, which
  // is when you'd stop tracking the changes through.
  void trackedUpdate(changedParams& oldVals, Rig* rig);

  // TODO
  // Delay has a few scenarios that need to be addressed.
  // 1. Delay was previously 0 going to non-zero
  //    -Move all other keyframes forwards by delay
  //    -Add a keyframe with the same value as the first keyframe at time t=delay
  // 2. Delay was previously non-zero
  //    -Move all keyframes except the first by newDelay - oldDelay. May need to traverse in forwards or backwards
  //     order depending on if the diff is positive or negative
  // If at any point a keyframe would go below t=0, return false and print an error message.
  // Should probably test that before doing the actual operation
  // bool setDelay(float delay)

  // Sets the time for the cue.
  void setTime(float time);

  // Sets a time with an up and down fade that are different
  void setTime(float up, float down);

  // Keyframe modifiers
  // Does a raw insert of a single parameter's data into a keyframe
  inline void insertKeyframe(string id, string param, Lumiverse::LumiverseType* data, float time, bool uct = true);

  // Inserts a new keyframe for all selected devices and all parameters taken from the
  // current state of the rig.
  // Set uct to false if you don't want to use cue timing if the inserted point is at the end of a cue
  // Overwrites old keyframes if they exist.
  void insertKeyframe(float time, DeviceSet devices, bool uct = true);
  
  // Raw delete of keyframe given device id, parameter name, and time
  void deleteKeyframe(string id, string param, float time);
  
  // Deletes a keyframe at the given time for the given devices.
  void deleteKeyframe(float time, DeviceSet devices);

  // Not implemented yet but planned
  // If ripple = true, will move the next cue to the location of the deleted cue
  // and subtract time on future cues accordingly.
  // void deleteKeyframe(float time, DeviceSet devices, bool ripple = false);

  // Returns the cue data stored in this cue.
  map<string, map<string, set<Keyframe> > >& getCueData() { return m_cueData; }

  // Gets the upfade
  float getUpfade() { return m_upfade; }

  // Gets the downfade
  float getDownfade() { return m_downfade; }

  /*! \brief Gets the delay from a cue. */
  float getDelay() { return m_delay; }

  // Returns the cue data for a device's parameter
  set<Keyframe>& getParamData(string deviceId, string param) { return m_cueData[deviceId][param]; }

  /*! \brief Returns the JSON representation of the cue. */
  JSONNode toJSON();

private:
  // Upfade time
  float m_upfade;

  // Downfade time
  float m_downfade;

  // Delay before doing any fades, default timing.
  float m_delay;

  /*!
  \brief Data for this particular cue.
  
  Stored in a map from ID -> parameter -> set of keyframes in ascending order (t=0 first)
  It's pretty much the device without the metadata.
  */
  map<string, map<string, set<Keyframe> > > m_cueData;

  // Gets the parameters for the device and returns them in a map
  map<string, set<Keyframe> > getParams(Device* d);

  // Updates the parameters for a device in the cue.
  // If a parameter changes, returns the name of the param and the
  // old value of the param.
  void updateParams(Device* d, map<string, shared_ptr<Lumiverse::LumiverseType> >& changed);
  
  // Reserved for future use.
  // m_follow - cue follow time (time to wait before automatically taking the next cue)
};
}
#endif