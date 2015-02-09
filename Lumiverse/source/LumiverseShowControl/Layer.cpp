#include "Layer.h"

namespace Lumiverse {
namespace ShowControl {

  Layer::Layer(Rig* rig, Playback* pb, string name, int priority, BlendMode mode) : m_mode(mode),
    m_name(name), m_priority(priority), m_pb(pb)
  {
    m_opacity = 1;
    init(rig);
  }

  Layer::Layer(Rig* rig, Playback* pb, string name, int priority, float opacity) : m_mode(BLEND_OPAQUE),
    m_opacity(opacity), m_name(name), m_priority(priority), m_pb(pb)
  {
    init(rig);
  }

  Layer::Layer(Rig* rig, Playback* pb, string name, int priority, DeviceSet set) : m_mode(SELECTED_ONLY),
    m_selectedDevices(set), m_name(name), m_priority(priority), m_pb(pb)
  {
    m_opacity = 1;
    init(rig);
  }

  Layer::Layer(Playback* pb, JSONNode node) : m_pb(pb) {
    auto it = node.begin();
    while (it != node.end()) {
      string name = it->name();

      if (name == "name")
        m_name = it->as_string();
      else if (name == "priority")
        m_priority = it->as_int();
      else if (name == "invertFilter")
        m_invertFilter = it->as_bool();
      else if (name == "active")
        m_active = it->as_bool();
      else if (name == "mode")
#ifdef USE_C11_MAPS
        m_mode = StringToBlendMode[it->as_string()];
#else
        m_mode = StringToBlendMode(it->as_string());
#endif
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

    m_active = false;
    m_invertFilter = false;
    m_pause = false;
    m_stop = false;
  }

  Layer::~Layer() {
    // Delete the devices
    for (auto kvp : m_layerState) {
      delete m_layerState[kvp.first];
    }
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

  void Layer::play(string id) {
    // TODO: update current state keyframes
    // An assumption is made that each timeline isn't being played back multiple times at once
    m_pb->getTimeline(id)->setCurrentState(m_layerState);
    // for dynamic timelines we'll have to think of something else to transfer state.

    m_lastPlayedTimeline = id;

    PlaybackData pbd;
    pbd.timelineID = id;
    pbd.complete = false;
    pbd.start = chrono::high_resolution_clock::now();

    m_queuedPlayback.push_back(pbd);
  }

  void Layer::pause() {
    m_pause = true;
  }

  void Layer::resume() {
    m_pause = false;
  }

  void Layer::stop() {
    m_stop = true;
  }

  void Layer::update(chrono::time_point<chrono::high_resolution_clock> updateStart) {
    auto loopTime = updateStart - m_previousLoopStart;

    // Grab waiting playback objects from the queue
    m_queue.lock();
    if (m_queuedPlayback.size() > 0) {
      for (auto p : m_queuedPlayback) {
        m_playbackData[p.timelineID] = p;
      }
      m_queuedPlayback.clear();
    }
    m_queue.unlock();

    vector<string> toDelete;

    if (m_stop) {
      m_playbackData.clear();
    }
    else if (m_pause) {
      // Need to update the start time by the diff of previous and current loop.
      for (auto& pbd : m_playbackData) {
        pbd.second.start += loopTime;
      }
    }
    else {
      // Update playback data and set layer state if there is anything currently active
      // Note that in the event of conflicts this would be a Latest Takes Precedence system
      if (m_playbackData.size() > 0) {
        for (const auto& pbd : m_playbackData) {

          // Here's how this works.
          // - For each device, for each paramter, get the value at the given time.
          // - Set the value in the layer state to the returned value.
          // - End playback if the timeline says it's done.
          shared_ptr<Timeline> tl = m_pb->getTimeline(pbd.second.timelineID);
          size_t t = chrono::duration_cast<chrono::milliseconds>(updateStart - pbd.second.start).count();
          size_t tp = chrono::duration_cast<chrono::milliseconds>(m_previousLoopStart - pbd.second.start).count();

          for (const auto& device : m_layerState) {
            for (const auto& param : device.second->getParamNames()) {
              shared_ptr<LumiverseType> val = tl->getValueAtTime(tl->getTimelineKey(device.second, param), t);

              // A value of nullptr indicates that the Timeline doesn't have any data for the specified device/paramter pair.
              if (val == nullptr) {
                continue;
              }

              LumiverseTypeUtils::copyByVal(val.get(), m_layerState[device.first]->getParam(param));
            }
          }

          tl->executeEvents(tp, t);

          if (tl->isDone(t)) {
            toDelete.push_back(pbd.first);
            tl->executeEndEvents();
          }
        }

        if (toDelete.size() > 0) {
          for (auto d : toDelete) {
            m_playbackData.erase(d);
          }
        }
      }
    }

    m_previousLoopStart = updateStart;
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

  JSONNode Layer::toJSON() {
    JSONNode layer;
    layer.set_name(m_name);

    layer.push_back(JSONNode("name", m_name));
    layer.push_back(JSONNode("priority", m_priority));
    layer.push_back(JSONNode("invertFilter", m_invertFilter));
    layer.push_back(JSONNode("active", m_active));
#ifdef USE_C11_MAPS
    layer.push_back(JSONNode("mode", BlendModeToString[m_mode]));
#else
    layer.push_back(JSONNode("mode", BlendModeToString(m_mode)));
#endif
    layer.push_back(JSONNode("opacity", m_opacity));
    layer.push_back(JSONNode("lastPlayedTimeline", m_lastPlayedTimeline));

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

  void Layer::reset() {
    for (const auto& kvp : m_layerState) {
      kvp.second->reset();
    }

    m_lastPlayedTimeline = "";
    // stop/pause logic?
  }
  
}
}