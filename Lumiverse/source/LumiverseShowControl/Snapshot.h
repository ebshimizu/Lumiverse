#pragma once
#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

#define pragma once

#include "LumiverseCore.h"
#include "Playback.h"

namespace Lumiverse {
namespace ShowControl {
  /*!
  \brief A Snapshot stores the state of the Playback and Rig at a particular time.

  Snapshots will by default store both the playback and rig, but users can choose to load
  them into the main playback or rig objects separately if desired.

  Snapshots do not store state information about the rig's update loop, patches, or attached
  functions. Snapshots operate purely on the state of the devices at a particular time.

  For Playback objects, snapshots will store the state of the Layers, Cue Lists, Cues, and Programmer.
  It will not store information about the state of the playback update loop.
  */
  class Snapshot
  {
  public:
    Snapshot(Rig* rig, Playback* pb);
    ~Snapshot();

    /*!
    \brief Saves a snapshot of the given Rig and Playback objects.

    \param rig Source rig
    \param pb Source playback
    */
    void saveSnapshot(Rig* rig, Playback* pb);

    /*!
    \brief Loads the stored snapshot values into the given Rig and Playback objects.

    This function will stop the update loop from running in the Playback object, as
    running it during large data ops is generally a bad idea. It will restore the loop
    to the previou state after loading all values. This does mean that all cue playbacks
    will also stop and be reset on load.
    \param targetRig Rig to load the snapshot into.
    \param targetPb Playback to load the snapshot into.
    */
    void loadSnapshot(Rig* targetRig, Playback* targetPb);

    /*!
    \brief Loads the stored rig state into the given Rig.

    \param targetRig Rig to load the snapshot into.
    */
    void loadRig(Rig* targetRig);

    /*!
    \brief Loads the stored playback object into the given Playback.

    \param targetPb Playback to load the snapshot into.
    */
    void loadPlayback(Playback* targetPb);

    /*!
    \brief Retrieves device data from the snapshot.
    */
    const map<string, Device*>& getRigData() { return m_rigData; }

  private:
    /*!
    \brief Stores the device data from the rig
    */
    map<string, Device*> m_rigData;

    /*!
    \brief The entire playback object. We're getting a little lazy here (or efficient
    depending on your point of view) and leveraging existing functions for saving
    and loading playback data.
    */
    JSONNode m_playbackData;
  };
}
}
#endif