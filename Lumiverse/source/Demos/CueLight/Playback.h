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

    /*!
    * \brief Starts the playback loop in a separate thread
    */
    void start();

    /*!
    * \brief Stops the playback loop in a separate thread.
    */
    void stop();

    /*!
    * \brief Sets m_running to true but does not launch a separate thread.
    */
    void run();

    /*!
    * \brief Sets m_running to false but does not stop a separate thread.
    */
    void halt();

    /*!
    * \brief Sets the playback update rate
    * \param rate Update loop rate in cycles/second
    */
    void setRefreshRate(unsigned int rate);

    /*!
    \brief Updates the layers contained by the Playback object and updates the Rig.
    */
    void update();

    /*!
    \brief Adds a layer to the playback

    Name is extracted from the layer settings
    \param layer The layer to add to the Playback object.
    */
    void addLayer(shared_ptr<Layer> layer);

    /*!
    \brief Retrieves a pointer to the layer with the specified name.
    \param name Layer name
    \return Pointer to layer specified. nullptr if layer does not exist.
    */
    shared_ptr<Layer> getLayer(string name);

    /*!
    \brief Deletes a layer from the playback.
    */
    void deleteLayer(string name);

    /*!
    \brief Adds a cue list to the Playback

    \param id Cue list identifier. Should be unique to the Playback.
    \param cueList Cue list to add.
    */
    void addCueList(string id, shared_ptr<CueList> cueList);

    /*!
    \brief Retrieves a cue list from the Playback
    \param id Cue list id.
    \return Pointer to specified cue list. nullptr if cue list doesn't exist. 
    */
    shared_ptr<CueList> getCueList(string id);

    /*!
    \brief Deletes a cue list from the Playback
    */
    void deleteCueList(string id);

    /*!
    \brief Assigns a cue list to a layer

    If one of the specified items doesn't exist, nothing will happen and this
    function will return false.
    \param cueListId Cue list identifier
    \param layerName Name of the layer to assign the cue list to.
    */
    bool addCueListToLayer(string cueListId, string layerName);

    /*!
    \brief Removes a cue list assigned to a particular layer.

    \param layerName Name of the layer
    */
    void removeCueListFromLayer(string layerName);

  private:
    /*! \brief Map of layer names to layers. */
    map<string, shared_ptr<Layer> > m_layers;

    /*! \brief Map of cue lists. */
    map<string, shared_ptr<CueList> > m_cueLists;

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