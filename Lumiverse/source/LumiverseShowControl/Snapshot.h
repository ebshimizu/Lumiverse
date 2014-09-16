#pragma once
#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

#define pragma once

#include "LumiverseCore.h"

namespace Lumiverse {
namespace ShowControl {

  /*!
  \brief A Snapshot stores the state of the Playback and Rig at a particular time.

  Snapshots will by default store both the playback and rig, but users can choose to load
  them into the main playback or rig objects separately if desired.
  */
  class Snapshot
  {
  public:
    Snapshot();
    ~Snapshot();
  };
}
}
#endif