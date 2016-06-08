#include "Snapshot.h"

namespace Lumiverse {
namespace ShowControl {
  
  Snapshot::Snapshot()
  {
    // do nothing, no data to fill
  }

  Snapshot::Snapshot(Rig* rig) {
    saveSnapshot(rig);
  }

  Snapshot::Snapshot(Snapshot & other)
  {
    for (const auto& kvp : other.m_rigData) {
      m_rigData[kvp.first] = new Device(kvp.second);
    }
  }

  Snapshot::~Snapshot() {
    for (const auto& kvp : m_rigData) {
      // Delete all device data since we copied all of it
      delete kvp.second;
    }
  }

  void Snapshot::saveSnapshot(Rig* rig) {
    // clear old devices
    for (auto d : m_rigData) {
      delete d.second;
    }
    m_rigData.clear();

    DeviceSet allDevices = rig->getAllDevices();
    const set<Device*>& devices = allDevices.getDevices();
    for (auto d : devices) {
      m_rigData[d->getId()] = new Device(d);
    }
  }

  void Snapshot::loadSnapshot(Rig* targetRig) {
    loadRig(targetRig);
  }

  void Snapshot::loadRig(Rig* targetRig) {
    targetRig->setAllDevices(m_rigData);
  }

  set<Device*> Snapshot::getDevices()
  {
    set<Device*> devices;

    for (const auto& kvp : m_rigData) {
      devices.insert(kvp.second);
    }

    return devices;
  }

  PlaybackSnapshot::PlaybackSnapshot(Rig * rig, Playback * pb) :
    Snapshot(rig)
  {
    if (pb != nullptr)
      m_playbackData = pb->toJSON();
  }

  PlaybackSnapshot::PlaybackSnapshot(PlaybackSnapshot & other)
  {
    for (const auto& kvp : other.m_rigData) {
      m_rigData[kvp.first] = new Device(kvp.second);
    }

    m_playbackData = other.m_playbackData;
  }

  PlaybackSnapshot::~PlaybackSnapshot()
  {
    for (const auto& kvp : m_rigData) {
      // Delete all device data since we copied all of it
      delete kvp.second;
    }
  }

  void PlaybackSnapshot::saveSnapshot(Rig * rig, Playback * pb)
  {
    Snapshot::saveSnapshot(rig);

    m_playbackData = pb->toJSON();
  }

  void PlaybackSnapshot::loadSnapshot(Rig * targetRig, Playback * targetPb)
  {
    Snapshot::loadSnapshot(targetRig);

    if (targetPb != nullptr)
      targetPb->loadJSON(m_playbackData);
  }

  void PlaybackSnapshot::loadPlayback(Playback* targetPb) {
    bool running = targetPb->isRunning();

    if (running) {
      targetPb->stop();
      // Doesn't happen instantly, wait for a sec
      this_thread::sleep_for(chrono::milliseconds(40));
    }

    targetPb->loadJSON(m_playbackData);

    if (running)
      targetPb->start();
  }
}
}