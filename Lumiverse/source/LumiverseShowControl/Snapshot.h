#pragma once
#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

#define pragma once

#include "LumiverseCore.h"
#include "Playback.h"

namespace Lumiverse {
namespace ShowControl {
  /*!
  \brief A Snapshot stores the state of the Rig at a particular time.

  Snapshots will only store the rig. To also save the Playback, use the
  PlaybackSnapshot class instead.

  Snapshots do not store state information about the rig's update loop, patches, or attached
  functions. Snapshots operate purely on the state of the devices at a particular time.
  */
  class Snapshot
  {
  public:
    Snapshot();
    Snapshot(Rig* rig);
    Snapshot(Snapshot& other);
    ~Snapshot();

    /*!
    \brief Saves a snapshot of the given Rig and Playback objects.

    \param rig Source rig
    */
    void saveSnapshot(Rig* rig);

    /*!
    \brief Loads the stored snapshot values into the given Rig and Playback objects.

    This function will stop the update loop from running in the Playback object, as
    running it during large data ops is generally a bad idea. It will restore the loop
    to the previou state after loading all values. This does mean that all cue playbacks
    will also stop and be reset on load.
    \param targetRig Rig to load the snapshot into.
    */
    void loadSnapshot(Rig* targetRig);

    /*!
    \brief Loads the stored rig state into the given Rig.

    \param targetRig Rig to load the snapshot into.
    */
    void loadRig(Rig* targetRig);

    /*!
    \brief Retrieves device data from the snapshot.
    */
    map<string, Device*>& getRigData() { return m_rigData; }

    /*!
    \brief Returns an unindexed set of the devices from the snapshot.
    */
    set<Device*> getDevices();

  protected:
    /*!
    \brief Stores the device data from the rig
    */
    map<string, Device*> m_rigData;
  };

  class PlaybackSnapshot : public Snapshot
  {
  public:
    PlaybackSnapshot(Rig* rig, Playback* pb);
    PlaybackSnapshot(PlaybackSnapshot& other);
    ~PlaybackSnapshot();

    void saveSnapshot(Rig* rig, Playback* pb);

    void loadSnapshot(Rig* targetRig, Playback* targetPb);

    void loadPlayback(Playback* targetPb);

  private:
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