#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>

// A cue stores data for a particular look (called a cue)
// Cues can be transitioned between, typically as a crossfade.
// This class currently stores just a set look and transitions with upfade
// and downfade time.
class Cue {
public:
  // Creates a cue from the current state of the rig.
  // Default fade time is 3.
  Cue(Rig* rig, float time = 3.0f);

  // Creates a cue with different up and down fades.
  Cue(Rig* rig, float up = 3.0f, float down = 3.0f);

  // Destructor
  ~Cue();

  // Modifiers
  // Updates the changes between the rig and this cue.
  // Tracking happens at the cue list level
  void update(Rig* rig);

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
  void updateParams(Device* d);

  // Reserved for future use
  // map<string, timestruct[upfade, downfade, delay]> m_discreteTiming - for setting times on individual devices
  // m_delay - cue delay
  // m_follow - cue follow time (time to wait before automatically taking the next cue)
};

#endif