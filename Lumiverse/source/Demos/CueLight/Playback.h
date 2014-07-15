#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_

#pragma once

#include <memory>
#include <chrono>

#include <LumiverseCore.h>
#include "Cue.h"
#include "CueList.h"
#include "Layer.h"

namespace Lumiverse {
  // Data that tracks the progress of a cue and stores the data used in the cue transition.
  struct PlaybackData {
    chrono::time_point<chrono::high_resolution_clock> start;    // Cue start time. More accurate to take difference between now and start instead of summing.
    map<string, map<string, set<Keyframe> > > activeKeyframes;
  };

  // A playback object manages layers and coordinates their actions and updates.
  // A playback doesn't actually do the playback, as all of that logic
  // is now in Layers.
  class Playback
  {
  public:
    // Initializes a playback object with desired refresh rate in Hz
    Playback(Rig* rig, unsigned int refreshRate);

    // Changes to make to playback: Playback must now maintain a list of layers
    // Layers are update in the playback update loop with layer->update()
    // Each layer now takes over the cue playback controls
    // Playback also maintains the master list of CueLists for reference by the layers.


    // Deletes a playback object.
    ~Playback();

    // Starts the playback loop.
    void start();

    // Stops the playback loop.
    void stop();

    // Sets the current state of the rig to the specified cue.
    // Asserts itself over all other cues
    void goToCue(Cue& cue, float time = 3);

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

    // Goes to the next cue in order in a given list.
    void goToNextCue(CueList& list, bool assert = false);

    // Goes to the first cue in a list.
    void goToList(CueList& list, bool assert = false);

    // Sets the playback update rate
    void setRefreshRate(unsigned int rate);

  private:
    // Runs the cue update loop.
    void update();

    // Returns the set of parameters to animate along with their keyframes
    // in going from cue A to cue B
    map<string, map<string, set<Keyframe> > > diff(Cue& a, Cue& b, bool assert = false);

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
}
#endif