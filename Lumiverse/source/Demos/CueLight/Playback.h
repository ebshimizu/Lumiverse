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

    // Sets the playback update rate
    void setRefreshRate(unsigned int rate);

    /*!
    \brief Updates the layers contained by the Playback object and updates the Rig.
    */
    void update();

  private:
    /*! \brief Map of layer names to layers. */
    map<string, shared_ptr<Layer> > m_layers;

    /*! \brief Copy of all devices in the rig. Current state of the playback. */
    map<string, Device*> m_state;

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