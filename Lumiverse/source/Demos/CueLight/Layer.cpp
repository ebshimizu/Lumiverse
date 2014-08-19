#include "Layer.h"

namespace Lumiverse {
  Layer::Layer(Rig* rig, string name, int priority, BlendMode mode) : m_mode(mode),
    m_name(name), m_priority(priority)
  {
    m_opacity = 1;
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
    m_opacity = 1;
    init(rig);
  }

  Layer::Layer(JSONNode node) {
    auto it = node.begin();
    while (it != node.end()) {
      string name = it->name();

      if (name == "name")
        m_name = it->as_string();
      else if (name == "currentCue")
        m_currentCue = it->as_float();
      else if (name == "priority")
        m_priority = it->as_int();
      else if (name == "invertFilter")
        m_invertFilter = it->as_bool();
      else if (name == "active")
        m_active = it->as_bool();
      else if (name == "mode")
        m_mode = StringToBlendMode[it->as_string()];
      else if (name == "opacity")
        m_opacity = it->as_float();
      // Cue list must be matched up one level above.
      else if (name == "state") {
        auto device = it->begin();

        while (device != it->end()) {
          m_layerState[device->name()] = new Device(device->name(), *device);
          device++;
        }
      }

      it++;
    }
  }

  void Layer::init(Rig* rig) {
    const set<Device*> devices = rig->getAllDevices().getDevices();
    for (auto d : devices) {
      // Copy and reset to defaults
      m_layerState[d->getId()] = new Device(*d);
      m_layerState[d->getId()]->reset();
    }

    // This line resolves performance issues with blank layers.
    // For some strange reason.
    // this is the definition of a hack.
    // goToCue(Cue(m_layerState, 0), Cue(m_layerState, 0), false);

    m_active = false;
    m_invertFilter = false;
    m_currentCue = -1;  // Generally people won't have negative cue numbers, so we'll use -1 to mean no active cue.
  }

  Layer::~Layer() {
    // Delete the devices
    for (auto kvp : m_layerState) {
      delete m_layerState[kvp.first];
    }
  }

  void Layer::setCueList(shared_ptr<CueList> list, bool resetCurrentCue) {
    m_cueList = list;

    if (resetCurrentCue)
      m_currentCue = -1;
  }

  void Layer::removeCueList() {
    m_cueList = nullptr;
    m_currentCue = -1;
  }

  void Layer::setOpacity(float val) {
    m_opacity = (val > 1) ? 1 : ((val < 0) ? 0 : val);
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
      if (m_currentCue >= 0) {
        Cue* firstCue = m_cueList->getCue(m_currentCue);
        Cue* nextCue = m_cueList->getNextCue(m_currentCue);

        if (firstCue != nullptr && nextCue != nullptr) {
          m_currentCue = m_cueList->getNextCueNum(m_currentCue);
          goToCue(*firstCue, *nextCue, false);
        }
        else {
          stringstream ss;
          ss << "Layer " << m_name << "Cannot go to next cue from cue " << m_currentCue;
          Logger::log(ERR, ss.str());
        }
      }
      else if (m_currentCue < 0) {
        goToCue(m_cueList->getFirstCueNum());
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
      Cue* firstCue = m_cueList->getCue(m_currentCue);
      Cue* nextCue = m_cueList->getPrevCue(m_currentCue);

      if (firstCue != nullptr && nextCue != nullptr) {
        m_currentCue = m_cueList->getPrevCueNum(m_currentCue);
        goToCue(*firstCue, *nextCue, false);
      }
      else {
        stringstream ss;
        ss << "Layer " << m_name << " cannot go to previous cue from cue " << m_currentCue;
        Logger::log(ERR, ss.str());
      }
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to previous cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

  void Layer::goToCue(float num, float up, float down, float delay) {
    if (hasCueList()) {
      Cue* nextCue = m_cueList->getCue(num);
      
      if (nextCue != nullptr) {
        Cue tempCue(m_layerState, up, down, delay);
        m_currentCue = num;
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
    pbData.delay = first.getDelay();

    stringstream ss;
    ss << "Layer " << m_name << " began a cue playback at " << chrono::duration_cast<chrono::seconds>(pbData.start.time_since_epoch()).count();
    Logger::log(LDEBUG, ss.str());

    lock_guard<mutex> lock(m_queue);
    m_queuedPlayback.push_back(pbData);
  }

  void Layer::goToCueAtTime(float num, float time) {
    if (hasCueList()) {
      Cue* currentCue = m_cueList->getCue(num);

      if (time > currentCue->getLength()) {
        stringstream ss;
        ss << "Cannot seek to time " << time << " in cue " << num << " (length: " << currentCue->getLength() << ")";
        Logger::log(WARN, ss.str());
        return;
      }

      if (currentCue != nullptr) {
        m_currentCue = num;
        Cue* nextCue = m_cueList->getNextCue(num);
        if (nextCue == nullptr) {
          // If there is no next cue, we stick to the one that does exist if the cue is
          // not a standalone cue.
          if (nextCue->getType() != "Standalone") {
            goToCue(num, 0.01f, 0.01f, 0);
            return;
          }
        }

        // Interpolation time. For the seek function, we just pull all the relevant values,
        // and then interpolate between them.

        map<string, map<string, set<Keyframe> > >& currentData = currentCue->getCueData();
        map<string, map<string, set<Keyframe> > >& nextData = nextCue->getCueData();
        float cueTime = time;

        // For each device
        for (auto& device : m_layerState) {
          // For each parameter
          for (auto& param : device.second->getRawParameters()) {
            // Find the relevant keyframes.
            Keyframe current;
            Keyframe next;
            bool nextFound = false;

            for (auto keyframe = currentData[device.first][param.first].begin(); keyframe != currentData[device.first][param.first].end();) {
              // We know that keyframes are ordered in ascending order, so the first one that's greater
              // than cueTime is the "next" keyframe.
              if (keyframe->t > cueTime) {
                next = *keyframe;
                current = *prev(keyframe);
                nextFound = true;
                break;
              }

              ++keyframe;
            }

            // If we didn't find a next keyframe, we ended up at the end. Time to clamp to nextCue values
            if (!nextFound) {
              Keyframe nextCue = *(nextData[device.first][param.first].begin());
              LumiverseTypeUtils::copyByVal(nextCue.val.get(), param.second);
            }
            else {
              // Otherwise, do a lerp between keyframes.
              // First need to convert the cueTime to a position from 0-1
              float t = (cueTime - current.t) / (next.t - current.t);
              shared_ptr<Lumiverse::LumiverseType> lerped = LumiverseTypeUtils::lerp(current.val.get(), next.val.get(), t);
              LumiverseTypeUtils::copyByVal(lerped.get(), param.second);
            }
          }
        }
      }
    }
  }

  void Layer::update(chrono::time_point<chrono::high_resolution_clock> updateStart) {
    // Grab waiting playback objects from the queue
    m_queue.lock();
    if (m_queuedPlayback.size() > 0) {
      for (auto p : m_queuedPlayback) {
        m_playbackData.push_back(p);
      }
      m_queuedPlayback.clear();
    }
    m_queue.unlock();

    // Update playback data and set layer state if there is anything currently active
    // Note that in the event of conflicts this would be a Latest Takes Precedence system
    if (m_playbackData.size() > 0) {
      auto pb = m_playbackData.begin();

      while (pb != m_playbackData.end()) {
        float cueTime = chrono::duration_cast<chrono::milliseconds>(updateStart - pb->start).count() / 1000.0f;
        cueTime -= pb->delay;

        // If we have a delay on the cue, don't do anything while the cue time is negative
        if (cueTime < 0) {
          pb++; 
          continue;
        }

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

        ++pb;
      }

      // Delete things that no longer have active keyframes.
      m_playbackData.erase(std::remove_if(m_playbackData.begin(), m_playbackData.end(),
        [](PlaybackData p) { return p.activeKeyframes.size() == 0; }), m_playbackData.end());
    }
  }

  void Layer::blend(map<string, Device*> currentState) {
    // We assume here that what you're passing in contains all the devices in the rig
    // and will not create new devices if they don't exist in the current state.

    map<string, Device*> active = m_layerState;

    if (m_mode == SELECTED_ONLY) {
      map<string, Device*> selected;
      // Run things on a different set for this mode
      for (auto d : m_selectedDevices.getDevices()) {
        selected[d->getId()] = d;
      }
      active = selected;
    }

    for (auto& device : active) {
      if (currentState.count(device.first) > 0) {
        // Time to start dealin with layer specific blend modes.
        if (m_mode == NULL_INTENSITY) {
          // Skip devices with intensity 0
          if (device.second->paramExists("intensity")) {
            float val = -1;
            device.second->getParam("intensity", val);
            if (val == 0) continue;
          }
        }

        // Go through each parameter in the device
        for (auto& param : device.second->getRawParameters()) {
          string paramName = param.first;
          LumiverseType* src = param.second;
          LumiverseType* dest = currentState[device.first]->getParam(param.first);
          
          if (dest == nullptr) {
            // Don't do anything if the destination doesn't have an existing value.
            continue;
          }

          // Criteria for looking at a parameter.
          // Filter is empty OR (paramName is in the filter AND filter not inverted)
          // OR (paramName is not in filter AND filter is inverted)
          if ((m_parameterFilter.size() == 0) ||
              (m_parameterFilter.count(paramName) > 0 && !m_invertFilter) ||
              (m_parameterFilter.count(paramName) == 0 && m_invertFilter))
          {
            // if we're using NULL_DEFAULT mode, we'll need to check params to see
            // if they're equal to their default values
            if (m_mode == NULL_DEFAULT && src->isDefault()) {
              continue;
            }

            // Generic alpha blending formula is res = src * opacity + dest * (1 - opacity)
            // Looks an awful lot like a lerp no?
            shared_ptr<LumiverseType> res = LumiverseTypeUtils::lerp(dest, src, m_opacity);
            LumiverseTypeUtils::copyByVal(res.get(), dest);
          }
        }
      }
      else {
        stringstream ss;
        ss << "State given to layer " << m_name << " does not contain a device with id " << device.first;
        Logger::log(WARN, ss.str());
      }
    }
  }

  map<string, map<string, set<Keyframe> > > Layer::diff(Cue& a, Cue& b, bool assert) {
    map<string, map<string, set<Keyframe> > > data;

    // The entire rig is stored, so comparison from a to be should be sufficient.
    map<string, map<string, set<Keyframe> > >& cueAData = a.getCueData();
    map<string, map<string, set<Keyframe> > >& cueBData = b.getCueData();

    // For all devices
    for (auto& it : cueAData) {
      // For all parameters in a device
      for (auto& param : it.second) {
        // The conditions for not animating a parameter are that it has two keyframes, and the
        // values in the keyframes are identical and we're not asserting this cue
        if (!assert && param.second.size() == 2 &&
          LumiverseTypeUtils::equals(param.second.begin()->val.get(), cueBData[it.first][param.first].begin()->val.get())) {
          continue;
        }
        else {
          // Otherwise just add all of cue a's keyframes into the active list and
          // fill in the blanks wiht cue b's data.
          // For all the keyframes assigned to a parameter
          for (auto keyframe = param.second.begin(); keyframe != param.second.end(); ++keyframe) {
            Keyframe k = Keyframe(*keyframe);

            // If the keyframe's value is null, we need to get the data from cue b.
            if (keyframe->val == nullptr) {
              // Get the data from the first keyframe assigned to the parameter in cue b.
              k.val = cueBData[it.first][param.first].begin()->val;

              // If we're using the default timing for this cue.
              if (k.useCueTiming) {
                // Determine if the timing should pull from upfade or downfade.
                // Compares the most recent cue value to the value in cue b to determine this.
                Lumiverse::LumiverseType* nextVal = cueBData[it.first][param.first].begin()->val.get();
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

            // If the keyframe has a value, we can skip all that cue timing nonsense.
            data[it.first][param.first].insert(k);
          }
        }
      }
    }

    return data;
  }

  JSONNode Layer::toJSON() {
    JSONNode layer;
    layer.set_name(m_name);

    layer.push_back(JSONNode("name", m_name));
    layer.push_back(JSONNode("priority", m_priority));
    layer.push_back(JSONNode("invertFilter", m_invertFilter));
    layer.push_back(JSONNode("active", m_active));
    layer.push_back(JSONNode("mode", BlendModeToString[m_mode]));
    layer.push_back(JSONNode("opacity", m_opacity));
    layer.push_back(JSONNode("currentCue", m_currentCue));
    
    if (hasCueList()) {
      layer.push_back(JSONNode("cueList", m_cueList->getName()));
    }
    else {
      layer.push_back(JSONNode("cueList", "null"));
    }

    // Copy state
    JSONNode layerState;
    layerState.set_name("state");
    for (const auto& kvp : m_layerState) {
      layerState.push_back(kvp.second->toJSON());
    }

    layer.push_back(layerState);

    // Parameter filter, list of IDs really
    JSONNode paramFilter;
    paramFilter.set_name("paramFilter");
    for (const auto& id : m_selectedDevices.getIds()) {
      paramFilter.push_back(JSONNode(id, id));
    }
    layer.push_back(paramFilter.as_array());

    return layer;
  }

}