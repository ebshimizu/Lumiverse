#include "Snapshot.h"

namespace Lumiverse {
namespace ShowControl {

  Snapshot::Snapshot(Rig* rig, Playback* pb) {
    saveSnapshot(rig, pb);
  }

  Snapshot::Snapshot(Snapshot & other)
  {
    for (const auto& kvp : other.m_rigData) {
      m_rigData[kvp.first] = new Device(kvp.second);
    }

    m_playbackData = other.m_playbackData;
  }

  Snapshot::~Snapshot() {
    for (const auto& kvp : m_rigData) {
      // Delete all device data since we copied all of it
      delete kvp.second;
    }
  }

  void Snapshot::saveSnapshot(Rig* rig, Playback* pb) {
    DeviceSet allDevices = rig->getAllDevices();
    const set<Device*>& devices = allDevices.getDevices();
    for (auto d : devices) {
      m_rigData[d->getId()] = new Device(d);
    }

    if (pb != nullptr) {
      m_playbackData = pb->toJSON();
    }
  }

  void Snapshot::loadSnapshot(Rig* targetRig, Playback* targetPb) {
    loadRig(targetRig);

    if (targetPb != nullptr) {
      loadPlayback(targetPb);
    }
  }

  void Snapshot::loadRig(Rig* targetRig) {
    targetRig->setAllDevices(m_rigData);
  }

  void Snapshot::loadPlayback(Playback* targetPb) {
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
  set<Device*> Snapshot::getDevices()
  {
    set<Device*> devices;

    for (const auto& kvp : m_rigData) {
      devices.insert(kvp.second);
    }

    return devices;
  }
}
}