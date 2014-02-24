#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>
#include <memory>

struct Keyframe {
  // Time at which this keyframe is located. t=0 is start of timeline.
  float t;

  // Value of the keyframe at time t
  // If this is nullptr, the keyframe is at the end of a cue and should take its value
  // from the next cue in the sequence.
  shared_ptr<LumiverseType> val;

  // If true, t will be set to [previous keyframe time] + up/down fade time at runtime.
  // If set to false, it will use the time specified in t.
  // Has no effect if val is not-null.
  bool useCueTiming;

  // Planned interpolation mode selection here. Additional parameters probably needed
  // once this thing gets activated
  // enum interpMode

  bool operator<(Keyframe other) const {
    return t < other.t;
  }

  // Empty constructor
  Keyframe() { }

  // Constructor with all values filled in.
  Keyframe(float time, shared_ptr<LumiverseType> v, bool uct) :
    t(time), val(v), useCueTiming(uct) { }
};

// A cue stores data for a particular look (called a cue)
// Cues can be transitioned between, typically as a crossfade.
// This class currently stores just a set look and transitions with upfade
// and downfade time.
class Cue {
public:
  typedef map<string, map<string, shared_ptr<LumiverseType>>> changedParams;

  // Makes a blank cue.
  Cue() : m_upfade(3.0f), m_downfade(3.0f) { }

  // Constructs a cue from a rig. Default time is 3.
  Cue(Rig* rig);

  // Creates a cue from the current state of the rig.
  // Set fade time manually
  Cue(Rig* rig, float time);

  // Creates a cue with different up and down fades.
  Cue(Rig* rig, float up, float down);

  // Copy a cue. Woooooo.
  Cue(Cue& other);

  // Destructor
  ~Cue();

  // Overloading the = to do a deep copy of the cue data.
  void operator=(const Cue& other);

  // Modifiers

  // Updates the changes between the rig and this cue.
  // Tracking happens at the cue list level
  // Returns a mapping of device id -> changed parameter and old value.
  // Note that if you have internal cues, this will try to track changes through to those keyframes.
  changedParams update(Rig* rig);

  // Only updates the devices with IDs in the changedParams.
  // Will remove IDs in oldVals if parameters don't match the older values, which
  // is when you'd stop tracking the changes through.
  void trackedUpdate(changedParams& oldVals, Rig* rig);

  // Sets the time for the cue.
  void setTime(float time);

  // Sets a time with an up and down fade that are different
  void setTime(float up, float down);

  // Returns the cue data stored in this cue.
  map<string, map<string, set<Keyframe> > >* getCueData() { return &m_cueData; }

  // Gets the upfade
  float getUpfade() { return m_upfade; }

  // Gets the downfade
  float getDownfade() { return m_downfade; }

  // Returns the cue data for a device's parameter
  set<Keyframe> getParamData(string deviceId, string param) { return m_cueData[deviceId][param]; }

private:
  // Upfade time
  float m_upfade;

  // Downfade time
  float m_downfade;

  // Data for this particular cue.
  // Stored in a map from ID -> parameter -> set of keyframes in ascending order (t=0 first)
  // It's pretty much the device without the metadata.
  map<string, map<string, set<Keyframe> > > m_cueData;

  // Gets the parameters for the device and returns them in a map
  map<string, set<Keyframe> > getParams(Device* d);

  // Updates the parameters for a device in the cue.
  // If a parameter changes, returns the name of the param and the
  // old value of the param.
  void updateParams(Device* d, map<string, shared_ptr<LumiverseType>>& changed);

  // Reserved for future use
  // map<string, timestruct[upfade, downfade, delay]> m_discreteTiming - for setting times on individual devices
  // m_delay - cue delay
  // m_follow - cue follow time (time to wait before automatically taking the next cue)
};

#endif