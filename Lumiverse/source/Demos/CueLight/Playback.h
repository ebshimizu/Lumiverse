#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_

#pragma once

#include <memory>
#include <chrono>

#include <LumiverseCore.h>
#include "Cue.h"
#include "CueList.h"
#include "Layer.h"
#include "Programmer.h"

namespace Lumiverse {

  /*!
  \brief A playback object manages layers and coordinates their actions and updates.

  Playbacks are typically attached to the main update loop in Rig and
  are limited in their update speed according to the speed of the Rig update loop.
  You can choose to run a Playback in a separate thread, though you'll need to
  create that thread yourself and call update() manually.

  Playbacks manage a series of Layers, which can each have cues running on them.
  The layers are updated by the Playback and then flattened down and sent to the Rig
  during each call to Playback::update().
  */
  class Playback
  {
  public:
    /*!
    \brief Initializes a playback object.
    \param rig Rig object associated with this playback.
    */
    Playback(Rig* rig);

    /*!
    \brief Loads a playback object from a file.

    Must specify a rig to associate with the playback object.
    \param rig Rig that will run the cues in. If there's a mis-match, some weird stuff will happen.
    \param filename File to load the playback from.
    */
    Playback(Rig* rig, string filename);

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
    * \brief Sets the playback update rate
    * \param rate Update loop rate in cycles/second
    */
    // void setRefreshRate(unsigned int rate);

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

    If a list already exists with the same name, this function will return false.
    \param cueList Cue list to add.
    */
    bool addCueList(shared_ptr<CueList> cueList);

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
    \param resetCurrenCue Set to true to reset the current cue of the layer (resets to -1)
    */
    bool addCueListToLayer(string cueListId, string layerName, bool resetCurrentCue = true);

    /*!
    \brief Removes a cue list assigned to a particular layer.

    \param layerName Name of the layer
    */
    void removeCueListFromLayer(string layerName);

    /*!
    \brief Binds the update function for this playback to the Rig's update function.
    \param pid ID to assign to the function. Defaults to 1. Must be positive.
    */
    void attachToRig(int pid = 1);

    /*!
    \brief Unbinds the update function for this playback from the Rig.
    */
    void detachFromRig();

    /*! \brief Gets a reference to the programmer object stored by the playback */
    const unique_ptr<Programmer>& getProgrammer() { return m_prog; }

    /*!
    \brief Saves a JSON file containing all information stored by the playback,
    including the Rig.

    File will contain the Rig and everything contained in this playback file.
    Loading of this output file should be done by Playback::load()
    \param filename Path to file
    \param overwrite If a file with the specified name already exists, that file
    will be overwritten if overwrite is set to true. Defaults to false.
    \return True on success, false on failure.
    */
    bool save(string filename, bool overwrite = false);

    /*!
    \brief Returns the JSON representation of this playback object.

    Some things don't need to be saved since they'll be reconstructed on load.
    The state of the playback and the programmer will be constructed on load.
    \return JSONNode containing all Playback information (includes the Rig)
    */
    JSONNode toJSON();

    /*! \brief Returns a pointer to the rig. */
    Rig* getRig() { return m_rig; }

    /*! \brief Returns a list of the cue list names contained in the Playback. */
    vector<string> getCueListNames();

    /*! \brief Returns a list of the layer names contained in the Playback. */
    vector<string> getLayerNames();

    /*! \brief Returns a reference to the layers in the Playback. */
    const map<string, shared_ptr<Layer> >& getLayers() { return m_layers; }

  private:
    /*! \brief Map of layer names to layers. */
    map<string, shared_ptr<Layer> > m_layers;

    /*! \brief Map of cue lists. */
    map<string, shared_ptr<CueList> > m_cueLists;

    /*! \brief Copy of all devices in the rig. Current state of the playback. */
    map<string, Device*> m_state;

    // Does the updating of the rig while running.
    // unique_ptr<thread> m_updateLoop;

    /*! \brief True when the update loop is running */
    bool m_running;

    /*! \brief ID of the attached function in the rig update loop */
    int m_funcId;

    // Refresh rate used by the update loop.
    // unsigned int m_refreshRate;

    // Loop time in seconds
    // float m_loopTime;

    /*! \brief Pointer to the rig that this playback runs on */
    Rig* m_rig;

    /*! \brief Programmer object owned by the Playback */
    unique_ptr<Programmer> m_prog;

    /*! \brief Load Playback data from a file. */
    bool load(string filename);

    /*! \brief Loads data from a JSON object */
    bool loadJSON(JSONNode node);
  };
}
#endif