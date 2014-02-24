#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_

#pragma once

#include <memory>

#include <LumiverseCore.h>
#include "Cue.h"
#include "CueList.h"

// Data that tracks the progress of a cue and stores the data used in the cue transition.
struct PlaybackData {
  clock_t start;    // Cue start time. More accurate to take difference between now and start instead of summing.
  map<string, map<string, set<Keyframe>>> activeKeyframes;
};

// A playback takes a cue (or cues) and manages the live transion between them
// Eventually a playback may be able to run multiple cues at once
// and effects on top of those cues. Right now, it does a single stack.
// It's important to note that this playback will only animate the
// active lights in a cue unless otherwise noted.
class Playback
{
public:
  // Initializes a playback object with desired refresh rate in Hz
  Playback(Rig* rig, unsigned int refreshRate);

  // Deletes a playback object.
  ~Playback();

  // Starts the playback loop.
  void start();

  // Stops the playback loop.
  void stop();

  // Sets the current state of the rig to the specified cue.
  void goToCue(Cue& cue);

  // Goes from the specified cue into the next specified cue.
  // Set assert to true to make the cue overwrite anything else running on
  // top of it. Useful for resetting, since only fixtures that change are
  // typically adjusted.
  void goToCue(Cue& first, Cue& next, bool assert = false);

  // Goes from one cue to another in a list.
  // Set assert to true to make all cue values animate.
  void goToCue(CueList& list, float first, float next, bool assert = false);

  // Goes from the specified cue in the Cue List to the next one in the list.
  // Set assert to true to make all cue values animate.
  void goToNextCue(CueList& list, float num, bool assert = false);

  // Goes to the first cue in a list.
  void goToList(CueList& list);

  // Sets the playback update rate
  void setRefreshRate(unsigned int rate);

private:
  // Runs the cue update loop.
  void update();

  // Returns the set of parameters to animate along with their keyframes
  // in going from cue A to cue B
  map<string, map<string, set<Keyframe>>> diff(Cue& a, Cue& b);

  // Stores the data used during playback.
  vector<PlaybackData> m_playbackData;

  // Does the updating of the rig while running.
  unique_ptr<thread> m_updateLoop;

  // True when the update loop is running
  bool m_running;

  // Refresh rate used by the update loop.
  unsigned int m_refreshRate;

  // Loop time in seconds
  float m_loopTime;

  // Pointer to the rig that this playback runs on
  Rig* m_rig;
};

#endif