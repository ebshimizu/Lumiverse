#include "Snapshot.h"

namespace Lumiverse {
namespace ShowControl {

  Snapshot::Snapshot(Rig* rig, Playback* pb) {
    saveSnapshot(rig, pb);
  }

  Snapshot::~Snapshot() {
    for (const auto& kvp : m_rigData) {
      // Delete all device data since we copied all of it
      delete kvp.second;
    }
  }

  void Snapshot::saveSnapshot(Rig* rig, Playback* pb) {
    for (const auto& d : rig->getAllDevices().getDevices()) {
      m_rigData[d->getId()] = new Device(d);
    }

    m_playbackData = pb->toJSON();
  }

  void Snapshot::loadSnapshot(Rig* targetRig, Playback* targetPb) {
    targetRig->setAllDevices(m_rigData);
    targetPb->loadJSON(m_playbackData);
  }

}
}