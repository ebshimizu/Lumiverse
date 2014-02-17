#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>
#include <memory>

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
  void operator=(Cue& other);

  // Modifiers
  // Updates the changes between the rig and this cue.
  // Tracking happens at the cue list level
  // Returns a mapping of device id -> changed parameter and old value
  // Hopefully the use of unique_ptr will deal with the case where you don't care
  // about the return value and want to free the memory from LumiverseTypes.
  changedParams update(Rig* rig);

  // Only updates the devices with IDs in the changedParams.
  // Will remove IDs in oldVals if parameters don't match the older values, which
  // is when you'd stop tracking the changes through.
  void trackedUpdate(changedParams& oldVals, Rig* rig);

  // Sets the time for the cue.
  void setTime(float time);

  // Sets a time with an up and down fade that are different
  void setTime(float up, float down);

private:
  // Upfade time
  float m_upfade;

  // Downfade time
  float m_downfade;

  // Data for this particular cue.
  // Stored in a map from ID -> list of parameters
  // It's pretty much the device without the metadata.
  map<string, map<string, LumiverseType*>> m_cueData;

  // Gets the parameters for the device and returns them in a map
  map<string, LumiverseType*> getParams(Device* d);

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