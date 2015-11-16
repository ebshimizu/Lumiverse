#include "Layer.h"

namespace Lumiverse {
namespace ShowControl {

  Layer::Layer(Rig* rig, Playback* pb, string name, int priority, BlendMode mode) : m_mode(mode),
    m_name(name), m_priority(priority), m_pb(pb)
  {
    m_opacity = 1;
    init(rig);
  }

  Layer::Layer(Rig* rig, Playback* pb, string name, int priority, float opacity) : m_mode(ALPHA),
    m_opacity(opacity), m_name(name), m_priority(priority), m_pb(pb)
  {
    init(rig);
  }

  Layer::Layer(DeviceSet set, Playback * pb, string name, int priority, BlendMode mode) :
    m_name(name), m_priority(priority), m_pb(pb), m_mode(mode)
  {
    auto devices = set.getDevices();
    for (const auto& d : devices) {
      for (const auto& p : d->getRawParameters()) {
        m_layerState[d->getId()][p.first] = LumiverseTypeUtils::copy(p.second);
      }
    }

    m_active = true;
    m_pause = false;
    m_stop = false;
    m_playing = false;
    m_playbackData = nullptr;
    m_queuedPlayback = nullptr;
  }

  Layer::Layer(Playback * pb, string name, int priority, BlendMode mode) :
    m_name(name), m_pb(pb), m_priority(priority), m_mode(mode)
  {
    // yup this layer's empty
    m_active = true;
    m_pause = false;
    m_stop = false;
    m_playing = false;
    m_playbackData = nullptr;
    m_queuedPlayback = nullptr;
  }

  Layer::Layer(Playback* pb, JSONNode node) : m_pb(pb) {
    auto it = node.begin();
    while (it != node.end()) {
      string name = it->name();

      if (name == "name")
        m_name = it->as_string();
      else if (name == "priority")
        m_priority = it->as_int();
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
      else if (name == "currentCue")
        m_currentCue = it->as_float();
      // Cue list must be matched up one level above.
      else if (name == "state") {
        auto device = it->begin();

        while (device != it->end()) {
          auto param = device->begin();
          while (param != device->end()) {
            m_layerState[device->name()][param->name()] = LumiverseTypeUtils::loadFromJSON(*param);
            param++;
          }
          device++;
        }
      }

      it++;
    }

    m_active = true;
    m_pause = false;
    m_stop = false;
    m_playing = false;
    m_playbackData = nullptr;
    m_queuedPlayback = nullptr;
  }

  void Layer::init(Rig* rig) {
    const set<Device*> devices = rig->getAllDevices().getDevices();
    for (auto d : devices) {
      for (auto p : d->getRawParameters()) {
        // Copy and reset to defaults
        m_layerState[d->getId()][p.first] = LumiverseTypeUtils::copy(p.second);
        m_layerState[d->getId()][p.first]->reset();
      }
    }

    m_active = true;
    m_pause = false;
    m_stop = false;
    m_playing = false;
    m_playbackData = nullptr;
    m_queuedPlayback = nullptr;
  }

  Layer::~Layer() {
    // Delete the devices
    for (auto kvp : m_layerState) {
      for (auto pkvp : kvp.second) {
        delete m_layerState[kvp.first][pkvp.first];
      }
    }
  }

  void Layer::setOpacity(float val) {
    m_opacity = (val > 1) ? 1 : ((val < 0) ? 0 : val);
  }

  bool Layer::addDevices(DeviceSet d)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback.");
      return false;
    }

    auto devices = d.getDevices();
    for (const auto& dv : devices) {
      for (const auto& p : dv->getRawParameters()) {
        m_layerState[dv->getId()][p.first] = LumiverseTypeUtils::copy(p.second);
      }
    }

    return true;
  }

  bool Layer::addDevice(Device * d, string param)
  {
    // Disallow layer modification if layer is playing.
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback.");
      return false;
    }

    // Delete old param value if exists
    if (m_layerState[d->getId()].count(param) != 0) {
      delete m_layerState[d->getId()][param];
    }

    m_layerState[d->getId()][param] = LumiverseTypeUtils::copy(d->getParam(param));

    return true;
  }

  bool Layer::addDevicesWithParams(DeviceSet d, set<string> params)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback.");
      return false;
    }

    auto devices = d.getDevices();
    for (const auto& dv : devices) {
      for (const auto& p : params) {
        // Delete old param value if exists
        if (m_layerState[dv->getId()].count(p) != 0) {
          delete m_layerState[dv->getId()][p];
        }

        // Add new param val
        m_layerState[dv->getId()][p] = LumiverseTypeUtils::copy(dv->getParam(p));
      }
    }

    return true;
  }

  bool Layer::addParamToAllDevices(string param, LumiverseType * type)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback.");
      return false;
    }

    for (auto& d : m_layerState) {
      // Delete old param if it exists
      if (d.second.count(param) != 0) {
        delete d.second[param];
      }

      d.second[param] = LumiverseTypeUtils::copy(type);
    }

    return true;
  }

  bool Layer::deleteParameter(string id, string param)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback");
      return false;
    }

    if (m_layerState[id].count(param) != 0) {
      delete m_layerState[id][param];
      m_layerState[id].erase(param);
    }

    return true;
  }

  bool Layer::deleteDevices(DeviceSet d)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback.");
      return false;
    }

    auto devices = d.getDevices();
    for (const auto& dv : devices) {
      auto params = m_layerState[dv->getId()];
      for (const auto& kvp : params) {
        deleteParameter(dv->getId(), kvp.first);
      }

      m_layerState.erase(dv->getId());
    }

    return true;
  }

  bool Layer::deleteParametersFromDevices(DeviceSet d, set<string> params)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback");
      return false;
    }

    auto devices = d.getDevices();
    for (const auto& dv : devices) {
      for (const auto& p : params) {
        deleteParameter(dv->getId(), p);
      }
    }

    return true;
  }

  bool Layer::deleteParametersFromAllDevices(set<string> params)
  {
    if (m_playing) {
      Logger::log(ERR, "Layer modification not allowed during playback");
      return false;
    }

    for (const auto& kvp : m_layerState) {
      for (const auto& p : params) {
        deleteParameter(kvp.first, p);
      }
    }

    return true;
  }

  void Layer::play(string id) {
    if (m_pb->getTimeline(id) == nullptr) {
      Logger::log(ERR, "Timeline with id " + id + " could not be found.");
      return;
    }

    // An assumption is made that each timeline isn't being played back multiple times at once
    // grab relevant data from current playback object
    string tlID = "";
    size_t time = 0;

    // Locking to make sure playback data doesn't get deleted from under our noses.
    m_queue.lock();
    
    if (m_playbackData != nullptr) {
      tlID = m_playbackData->timelineID;
      time = chrono::duration_cast<chrono::milliseconds>(m_playbackData->elapsed - m_playbackData->start).count();
    }

    m_pb->getTimeline(id)->setCurrentState(m_layerState, m_pb->getTimeline(tlID), time);

    m_queue.unlock();

    m_lastPlayedTimeline = id;

    PlaybackData* pbd = new PlaybackData();;
    pbd->timelineID = id;
    pbd->complete = false;
    pbd->start = chrono::high_resolution_clock::now();
    pbd->elapsed = pbd->start;
    pbd->length = m_pb->getTimeline(id)->getLength();

    // Delete anything in the up next slot if we got something there
    if (m_queuedPlayback != nullptr) {
      delete m_queuedPlayback;
    }

    m_queue.lock();
    m_queuedPlayback = pbd;
    m_queue.unlock();
  }

  void Layer::pause() {
    m_pause = true;
    m_playing = false;
  }

  void Layer::resume() {
    m_pause = false;
    m_stop = false;
    m_playing = true;
  }

  void Layer::stop() {
    m_stop = true;
    m_playing = false;
  }

  string Layer::getRecentTimeline() {
    return m_lastPlayedTimeline;
  }

  void Layer::update(chrono::time_point<chrono::high_resolution_clock> updateStart) {
    auto loopTime = updateStart - m_previousLoopStart;

    // Grab waiting playback objects from the queue
    m_queue.lock();
    if (m_queuedPlayback != nullptr) {
      m_playbackData = m_queuedPlayback;
      m_queuedPlayback = nullptr;

      m_pause = false;
      m_stop = false;
      m_playing = true;
    }
    m_queue.unlock();

    if (m_stop) {
      if (m_playbackData != nullptr) {
        delete m_playbackData;
        m_playbackData = nullptr;
      }
    }
    else if (m_pause) {
      // Need to update the start time by the diff of previous and current loop.
      if (m_playbackData != nullptr) {
        m_playbackData->start += loopTime;
      }
    }
    else {
      // Update playback data and set layer state if there is anything currently active
      // Note that in the event of conflicts this would be a Latest Takes Precedence system
      if (m_playbackData != nullptr) {
        m_playbackData->elapsed += loopTime;

        // Here's how this works.
        // - For each device, for each paramter, get the value at the given time.
        // - Set the value in the layer state to the returned value.
        // - End playback if the timeline says it's done.
        shared_ptr<Timeline> tl = m_pb->getTimeline(m_playbackData->timelineID);
        size_t t = chrono::duration_cast<chrono::milliseconds>(updateStart - m_playbackData->start).count();
        size_t tp = chrono::duration_cast<chrono::milliseconds>(m_previousLoopStart - m_playbackData->start).count();

        for (const auto& device : m_layerState) {
          for (auto& param : m_layerState[device.first]) {
            shared_ptr<LumiverseType> val = tl->getValueAtTime(device.first, param.first, param.second, t, m_pb->getTimelines());

            // A value of nullptr indicates that the Timeline doesn't have any data for the specified device/paramter pair.
            if (val == nullptr) {
              continue;
            }

            LumiverseTypeUtils::copyByVal(val.get(), param.second);
          }
        }

        tl->executeEvents(tp, t);

        // this is not optimal. at the moment the locking is necessary due to c++ stl container
        // access issues in some of the timeline methods (iterators getting reset, etc.)
        m_queue.lock();
        if (tl->isDone(t, m_pb->getTimelines())) {
          tl->executeEndEvents();
          delete m_playbackData;
          m_playbackData = nullptr;
          m_stop = true;
          m_pause = false;
          m_playing = false;
        }
        m_queue.unlock();
      }
    }

    m_previousLoopStart = updateStart;
  }

  void Layer::blend(map<string, Device*> currentState) {
    // We assume here that what you're passing in contains all the devices in the rig
    // and will not create new devices if they don't exist in the current state.

    map<string, map<string, LumiverseType*> > active = m_layerState;

    for (auto& device : active) {
      try {
        auto d = currentState.at(device.first);

        // Go through each parameter in the device
        for (auto& param : device.second) {
          string paramName = param.first;
          LumiverseType* src = param.second;
          LumiverseType* dest = d->getParam(param.first);
          
          if (dest == nullptr) {
            // Don't do anything if the destination doesn't have an existing value.
            continue;
          }

          if (m_mode == ALPHA) {
            if (m_opacity >= 1) {
              LumiverseTypeUtils::copyByVal(src, dest);
            }
            else {
              // Generic alpha blending formula is res = src * opacity + dest * (1 - opacity)
              // Looks an awful lot like a lerp no?
              shared_ptr<LumiverseType> res = LumiverseTypeUtils::lerp(dest, src, m_opacity);
              LumiverseTypeUtils::copyByVal(res.get(), dest);
            }
          }
          else if (m_mode == OVERWRITE) {
            LumiverseTypeUtils::copyByVal(src, dest);
          }
        }
      }
      catch (exception e) {
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
    layer.push_back(JSONNode("active", m_active));
#ifdef USE_C11_MAPS
    layer.push_back(JSONNode("mode", BlendModeToString[m_mode]));
#else
    layer.push_back(JSONNode("mode", BlendModeToString(m_mode)));
#endif
    layer.push_back(JSONNode("opacity", m_opacity));
    layer.push_back(JSONNode("lastPlayedTimeline", m_lastPlayedTimeline));
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
      JSONNode params;
      params.set_name(kvp.first);
      for (const auto& pkvp : kvp.second) {
        params.push_back(pkvp.second->toJSON(pkvp.first));
      }
      layerState.push_back(params);
    }

    layer.push_back(layerState);

    return layer;
  }

  void Layer::reset() {
    for (const auto& kvp : m_layerState) {
      for (const auto& pkvp : kvp.second) {
        pkvp.second->reset();
      }
    }

    m_lastPlayedTimeline = "";
    stop();
  }

  void Layer::setCueList(shared_ptr<CueList> list, bool resetCurrentCue) {
    m_cueList = list;

    if (resetCurrentCue) {
      m_currentCue = -1;
    }
  }

  void Layer::removeCueList() {
    m_cueList = nullptr;
    m_currentCue = -1;
  }

  bool Layer::hasCueList()
  {
    return m_cueList != nullptr;
  }

  const shared_ptr<CueList>& Layer::getCueList()
  {
    return m_cueList;
  }

  float Layer::getCurrentCue()
  {
    return m_currentCue;
  }
  
  void Layer::go() {
    if (hasCueList()) {
      if (m_currentCue >= 0) {
        string nextCue = m_cueList->getNextCue(m_currentCue);
        m_currentCue = m_cueList->getNextCueNum(m_currentCue);
        play(nextCue);
      }
      else if (m_currentCue < 0) {
        play(m_cueList->getFirstCue());
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
      string prevCue = m_cueList->getPrevCue(m_currentCue);
      m_currentCue = m_cueList->getPrevCueNum(m_currentCue);
      play(prevCue);
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to previous cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

  void Layer::goToCue(float num, float up, float down, float delay) {
    if (hasCueList()) {
      string nextCue = m_cueList->getCueName(num);

      // Note that at the moment this ignores any parameters sent to goToCue.
      m_currentCue = num;
      play(nextCue);
    }
    else {
      stringstream ss;
      ss << "Layer " << m_name << " cannot go to cue because it has no assigned cue list.";
      Logger::log(ERR, ss.str());
    }
  }

}
}