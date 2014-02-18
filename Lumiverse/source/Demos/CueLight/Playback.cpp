#include "Playback.h"

Playback::Playback(Rig* rig, unsigned int refreshRate) : m_rig(rig) {
  setRefreshRate(refreshRate);
  m_running = false;
}

Playback::~Playback() {
  stop();
}

void Playback::start() {
  m_running = true;
  m_updateLoop = unique_ptr<thread>(new thread(&Playback::update, this));
  Logger::log(INFO, "Started playback update loop");
}

void Playback::stop() {
  if (m_running) {
    m_running = false;
    m_updateLoop->join();
    Logger::log(INFO, "Stopped playback update loop");
  }
}

void Playback::goToCue(Cue& first, Cue& next, bool assert) {
  PlaybackData pbData;
  pbData.from = first;
  pbData.to = next;

  // Process for getting everything setup has a few stages.
  // If we're not asserting the cue, figure out what changed from first to next
  if (assert) {
    // Force cue next to take precedence
  }
  else {
    pbData.activeParams = diff(first, next);
  }

  pbData.start = clock();

  stringstream ss;
  ss << "Added cue to playback at " << pbData.start << "\n";
  Logger::log(DEBUG, ss.str());

  m_playbackData.push_back(pbData);
}


void Playback::setRefreshRate(unsigned int rate) {
  m_refreshRate = rate;
  m_loopTime = 1.0f / (float)m_refreshRate;
}

void Playback::update() {
  while (m_running) {
    // Get start time
    clock_t start, end;
    start = clock();

    // Update playback data and set rig state if there is anything currently active
    // Note that in the event of conflicts this would be a Latest Takes Precedence system
    if (m_playbackData.size() > 0) {
      for (auto pb : m_playbackData) {
        float cueTime = (float)(start - pb.start) / CLOCKS_PER_SEC;
        auto params = pb.activeParams.begin();
        // Go through the active parameters and prepare to delete things
        while (params != pb.activeParams.end()) {
          if (cueTime >= params->second.time) {
            // We are past the time for this cue to be animating. Clamp to the "to" cue value.
            LumiverseTypeUtils::copyByVal(pb.to.getParamData(params->first, params->second.name),
              m_rig->getDevice(params->first)->getParam(params->second.name));

            pb.activeParams.erase(params++);
          }
          else {
            // Otherwise lerp between from and to based on the time.
            shared_ptr<LumiverseType> lerped = LumiverseTypeUtils::lerp(pb.from.getParamData(params->first, params->second.name),
              pb.to.getParamData(params->first, params->second.name), cueTime / params->second.time);
            LumiverseTypeUtils::copyByVal(lerped.get(), m_rig->getDevice(params->first)->getParam(params->second.name));
            ++params;
          }
        }
      }
    }

    // Sleep a bit depending on how long the update took.
    end = clock();
    float elapsed = (float)(end - start) / CLOCKS_PER_SEC;

    if (elapsed < m_loopTime) {
      unsigned int ms = (unsigned int)(1000 * (m_loopTime - elapsed));
      this_thread::sleep_for(chrono::milliseconds(ms));
    }
    else {
      Logger::log(WARN, "Playback Update loop running slowly");
    }
  }
}

map<string, paramTiming> Playback::diff(Cue& a, Cue& b) {
  map<string, paramTiming> data;

  // With this system, each cue stores the state of the entire rig, so comparison
  // in one direction is sufficient.
  map<string, map<string, LumiverseType*>>* cueAData = a.getCueData();
  map<string, map<string, LumiverseType*>>* cueBData = b.getCueData();

  // For all devices
  for (auto it : *cueAData) {
    // For all parameters in a device
    for (auto param : it.second) {
      // see if a and b have different data
      int result = LumiverseTypeUtils::cmp(param.second, (*cueBData)[it.first][param.first]);
      if (result == -1 || result == 1) {
        // If they do, then add it to the data with the upfade/downfade time
        if (result == -1) {
          // Upfade. a -> b uses cue a's timing.
          data[it.first] = { param.first, a.getUpfade(), 0 /* TODO: No delay for now */ };
        }
        else {
          // Downfade. a -> b uses cue a's timing
          data[it.first] = { param.first, a.getDownfade(), 0 /* TODO: No delay for now */ };
        }
      }
    }
  }

  return data;
}