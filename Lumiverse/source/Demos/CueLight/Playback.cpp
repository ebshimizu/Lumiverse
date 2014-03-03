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

  // Process for getting everything setup has a few stages.
  // If we're not asserting the cue, figure out what changed from first to next
  if (assert) {
    // Force cue next to take precedence
  }
  // Running a cue is as simple as running the keyframes in it
  // Of course, simple is relative as we have to populate the list of active
  // keyframes with keyframes that actually do a change to the device during the cue.
  else {
    pbData.activeKeyframes = diff(first, next);
  }

  pbData.start = chrono::high_resolution_clock::now();

  stringstream ss;
  ss << "Added cue to playback at " << chrono::duration_cast<chrono::seconds>(pbData.start.time_since_epoch()).count() << "\n";
  Logger::log(DEBUG, ss.str());

  m_playbackData.push_back(pbData);
}


void Playback::setRefreshRate(unsigned int rate) {
  m_refreshRate = rate;
  m_loopTime = 1.0f / (float)m_refreshRate;
}

void Playback::update() {
  while (m_running) {
    // Gets start time
    auto start = chrono::high_resolution_clock::now();

    // Update playback data and set rig state if there is anything currently active
    // Note that in the event of conflicts this would be a Latest Takes Precedence system
    if (m_playbackData.size() > 0) {
      auto pb = m_playbackData.begin();

      while (pb != m_playbackData.end()) {
        float cueTime = chrono::duration_cast<chrono::milliseconds>(start - pb->start).count() / 1000.0f;

        auto devices = pb->activeKeyframes.begin();

        // Plan is to go through each active parameter, find which keyframes we should interpolate,
        // and do the interpolation. For keyframes at the end, we should clamp to the final value.
        // Keyframes are organized by device->parameter->keyframes
        while (devices != pb->activeKeyframes.end()) {
          auto parameters = devices->second.begin();
          while (parameters != devices->second.end()) {
            Keyframe first;
            Keyframe next;
            bool nextFound = false;
            // Find the keyframes that contain cueTime in their interval (first <= cueTime < next)
            for (auto keyframe = parameters->second.begin(); keyframe != parameters->second.end(); ) {
              // We know that keyframes are ordered in ascending order, so the first one that's greater
              // than cueTime is the "next" keyframe.
              if (keyframe->t > cueTime) {
                next = *keyframe;
                first = *prev(keyframe);
                nextFound = true;
                break;
              }

              ++keyframe;
            }

            // If we didn't find a next keyframe, we ended up at the end. Time to clamp
            if (!nextFound) {
              LumiverseTypeUtils::copyByVal(prev(parameters->second.end())->val.get(),
                m_rig->getDevice(devices->first)->getParam(parameters->first));

              pb->activeKeyframes[devices->first].erase(parameters++);
            }
            else {
              // Otherwise, do a lerp between keyframes.
              // First need to convert the cueTime to a position from 0-1
              float t = (cueTime - first.t) / (next.t - first.t);
              shared_ptr<LumiverseType> lerped = LumiverseTypeUtils::lerp(first.val.get(), next.val.get(), t);
              LumiverseTypeUtils::copyByVal(lerped.get(), m_rig->getDevice(devices->first)->getParam(parameters->first));
              ++parameters;
            }
          }

          if (pb->activeKeyframes[devices->first].size() == 0) {
            // Delete the device entry if no more things are active
            pb->activeKeyframes.erase(devices++);
          }
          else {
            devices++;
          }
        }

        // Delete the entire playback object if everything is done
        if (pb->activeKeyframes.size() == 0) {
          m_playbackData.erase(pb++);
          // Apparently when m_playbackData is 0 at the end of this weird stuff happens.
          if (m_playbackData.size() == 0)
            break;
        }
        else {
          ++pb;
        }
      }
    }

    // Sleep a bit depending on how long the update took.
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = end - start;
    float elapsedSec = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / 1000.0f;
    
    if (elapsedSec < m_loopTime) {
      unsigned int ms = (unsigned int)(1000 * (m_loopTime - elapsedSec));
      this_thread::sleep_for(chrono::milliseconds(ms));
    }
    else {
      Logger::log(WARN, "Playback Update loop running slowly");
    }
  }
}

map<string, map<string, set<Keyframe> > > Playback::diff(Cue& a, Cue& b) {
  map<string, map<string, set<Keyframe> > > data;

  // The entire rig is stored, so comparison from a to be should be sufficient.
  map<string, map<string, set<Keyframe> > >* cueAData = a.getCueData();
  map<string, map<string, set<Keyframe> > >* cueBData = b.getCueData();

  // For all devices
  for (auto it : *cueAData) {
    // For all parameters in a device
    for (auto param : it.second) {
      // The conditions for not animating a parameter are that it has two keyframes, and the
      // values in the keyframes are identical.
      if (param.second.size() == 2 &&
        LumiverseTypeUtils::equals(param.second.begin()->val.get(), (*cueBData)[it.first][param.first].begin()->val.get())) {
        continue;
      }
      else {
        // Otherwise just add all of cue a's keyframes into the active list and
        // fill in the blanks wiht cue b's data.
        for (auto keyframe = param.second.begin(); keyframe != param.second.end(); ++keyframe) {
          Keyframe k = Keyframe(*keyframe);

          if (keyframe->val == nullptr) {
            k.val = (*cueBData)[it.first][param.first].begin()->val;

            if (k.useCueTiming) {
              LumiverseType* nextVal = (*cueBData)[it.first][param.first].begin()->val.get();
              LumiverseType* thisVal = prev(keyframe)->val.get();
              int result = LumiverseTypeUtils::cmp(thisVal, nextVal);
              
              if (result == -1) {
                // Upfade. a -> b uses cue a's timing.
                k.t = prev(keyframe)->t + a.getUpfade();
              }
              else if (result == 1) {
                // Downfade
                k.t = prev(keyframe)->t + a.getDownfade();
              }
            }
          }

          data[it.first][param.first].insert(k);
        }
      }
    }
  }

  return data;
}