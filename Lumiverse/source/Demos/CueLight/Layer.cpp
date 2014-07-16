#include "Layer.h"

namespace Lumiverse {
  Layer::Layer(Rig* rig, string name, int priority, BlendMode mode) : m_mode(mode),
    m_name(name), m_priority(priority)
  {
    init(rig);
  }

  Layer::Layer(Rig* rig, string name, int priority, float opacity) : m_mode(BLEND_OPAQUE),
    m_opacity(opacity), m_name(name), m_priority(priority)
  {
    init(rig);
  }

  Layer::Layer(Rig* rig, string name, int priority, DeviceSet set) : m_mode(SELECTED_ONLY),
    m_selectedDevices(set), m_name(name), m_priority(priority)
  {
    init(rig);
  }

  void Layer::init(Rig* rig) {
    const set<Device*>* devices = rig->getAllDevices().getDevices();
    for (auto d : *devices) {
      // Copy and reset to defaults
      m_layerState[d->getId()] = new Device(*d);
      m_layerState[d->getId()]->reset();
    }

    m_active = false;
    m_invertFilter = false;
  }

  Layer::~Layer() {
    // Delete the devices
    for (auto kvp : m_layerState) {
      delete m_layerState[kvp.first];
    }
  }

  void Layer::setCueList(shared_ptr<CueList> list) {
    m_cueList = list;
  }

  void Layer::removeCueList() {
    m_cueList = nullptr;
  }

  void Layer::addParamFilter(string param) {
    m_parameterFilter.insert(param);
  }

  void Layer::removeParamFilter(string param) {
    m_parameterFilter.erase(param);
  }

  void Layer::deleteParamFilter() {
    m_parameterFilter.clear();
  }

  void Layer::invertFilter() {
    m_invertFilter = !m_invertFilter;
  }

  void Layer::setSelectedDevices(DeviceSet devices) {
    m_selectedDevices = devices;
  }

  void Layer::removeSelectedDevices(DeviceSet devices) {
    m_selectedDevices = m_selectedDevices.remove(devices);
  }

  void Layer::addSelectedDevices(DeviceSet devices) {
    m_selectedDevices = m_selectedDevices.add(devices);
  }

  void Layer::clearSelectedDevices() {
    // Easiest way is to just select nothing from the rig
    m_selectedDevices = m_selectedDevices.select("");
  }

  void Layer::go() {
    if (hasCueList()) {
      float currentCue = m_cueList->getCurrentCue();
      Cue* firstCue = m_cueList->getCue(currentCue);
      Cue* nextCue = m_cueList->getNextCue(currentCue);

      if (firstCue != nullptr && nextCue != nullptr) {
        m_cueList->setCurrentCue(m_cueList->getNextCueNum(currentCue));
        goToCue(*firstCue, *nextCue, false);
      }
      else {
        stringstream ss;
        ss << "Layer " << m_name << "Cannot go to next cue from cue " << currentCue;
        Logger::log(ERR, ss.str());
      }
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to next cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

  void Layer::back() {
    if (hasCueList()) {
      float currentCue = m_cueList->getCurrentCue();
      Cue* firstCue = m_cueList->getCue(currentCue);
      Cue* nextCue = m_cueList->getPrevCue(currentCue);

      if (firstCue != nullptr && nextCue != nullptr) {
        m_cueList->setCurrentCue(m_cueList->getPrevCueNum(currentCue));
        goToCue(*firstCue, *nextCue, false);
      }
      else {
        stringstream ss;
        ss << "Layer " << m_name << "Cannot go to previous cue from cue " << currentCue;
        Logger::log(ERR, ss.str());
      }
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to previous cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

  void Layer::goToCue(float num, float time) {
    if (hasCueList()) {
      Cue* nextCue = m_cueList->getCue(num);
      
      if (nextCue != nullptr) {
        Cue tempCue(m_layerState, time);
        m_cueList->setCurrentCue(num);
        goToCue(tempCue, *nextCue, true);
      }
      else {
        stringstream ss;
        ss << "Layer " << m_name << "Cannot go to cue " << num;
        Logger::log(ERR, ss.str());
      }
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

  void Layer::goToCue(Cue& first, Cue& next, bool assert) {
    PlaybackData pbData;

    // Running a cue is as simple as running the keyframes in it
    // Of course, simple is relative as we have to populate the list of active
    // keyframes with keyframes that actually do a change to the device during the cue.
    pbData.activeKeyframes = diff(first, next, assert);

    pbData.start = chrono::high_resolution_clock::now();

    stringstream ss;
    ss << "Layer " << m_name << " began a cue playback at " << chrono::duration_cast<chrono::seconds>(pbData.start.time_since_epoch()).count() << "\n";
    Logger::log(LDEBUG, ss.str());

    m_playbackData.push_back(pbData);
  }

  void Layer::update(chrono::time_point<chrono::high_resolution_clock> updateStart) {
    // Update playback data and set layer state if there is anything currently active
    // Note that in the event of conflicts this would be a Latest Takes Precedence system
    if (m_playbackData.size() > 0) {
      auto pb = m_playbackData.begin();

      while (pb != m_playbackData.end()) {
        float cueTime = chrono::duration_cast<chrono::milliseconds>(updateStart - pb->start).count() / 1000.0f;

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
            for (auto keyframe = parameters->second.begin(); keyframe != parameters->second.end();) {
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
                m_layerState[devices->first]->getParam(parameters->first));

              pb->activeKeyframes[devices->first].erase(parameters++);
            }
            else {
              // Otherwise, do a lerp between keyframes.
              // First need to convert the cueTime to a position from 0-1
              float t = (cueTime - first.t) / (next.t - first.t);
              shared_ptr<Lumiverse::LumiverseType> lerped = LumiverseTypeUtils::lerp(first.val.get(), next.val.get(), t);
              LumiverseTypeUtils::copyByVal(lerped.get(), m_layerState[devices->first]->getParam(parameters->first));
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
  }

  map<string, map<string, set<Keyframe> > > Layer::diff(Cue& a, Cue& b, bool assert) {
    map<string, map<string, set<Keyframe> > > data;

    // The entire rig is stored, so comparison from a to be should be sufficient.
    map<string, map<string, set<Keyframe> > >* cueAData = a.getCueData();
    map<string, map<string, set<Keyframe> > >* cueBData = b.getCueData();

    // For all devices
    for (auto it : *cueAData) {
      // For all parameters in a device
      for (auto param : it.second) {
        // The conditions for not animating a parameter are that it has two keyframes, and the
        // values in the keyframes are identical and we're not asserting this cue
        if (!assert && param.second.size() == 2 &&
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
                Lumiverse::LumiverseType* nextVal = (*cueBData)[it.first][param.first].begin()->val.get();
                Lumiverse::LumiverseType* thisVal = prev(keyframe)->val.get();
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

}