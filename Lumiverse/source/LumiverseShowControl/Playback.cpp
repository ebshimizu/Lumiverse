#include "Playback.h"
#include "SineWave.h"

namespace Lumiverse {
namespace ShowControl {

  Playback::Playback(Rig* rig, float gm) : m_rig(rig), m_grandmaster(gm) {
    // setRefreshRate(refreshRate);
    m_running = false;

    auto devices = m_rig->getAllDevices().getDevices();
    for (Device* d : devices) {
      // Copy and reset to defaults
      m_state[d->getId()] = new Device(*d);
      m_state[d->getId()]->reset();
    }

    m_funcId = -1;

    // Make a single programmer for this playback object
    m_prog = unique_ptr<Programmer>(new Programmer(m_rig));
  }

  Playback::Playback(Rig* rig, string filename) : m_rig(rig) {
    m_running = false;

    auto devices = m_rig->getAllDevices().getDevices();
    for (Device* d : devices) {
      // Copy and reset to defaults
      m_state[d->getId()] = new Device(*d);
      m_state[d->getId()]->reset();
    }

    m_funcId = -1;

    // Make a single programmer for this playback object
    m_prog = unique_ptr<Programmer>(new Programmer(m_rig));

    // Load up cue and layer data.
    load(filename);
    m_grandmaster = 1;
  }

  Playback::~Playback() {
    stop();

    for (auto& d : m_state) {
      delete d.second;
    }
  }

  void Playback::start() {
    m_running = true;
    Logger::log(INFO, "Started playback update loop");
  }

  void Playback::stop() {
    if (m_running) {
      m_running = false;
      Logger::log(INFO, "Stopped playback update loop");
    }
  }

  //void Playback::setRefreshRate(unsigned int rate) {
  //  m_refreshRate = rate;
  //  m_loopTime = 1.0f / (float)m_refreshRate;
  //}

  void Playback::update() {
    if (m_running) {
      // Gets start time
      auto start = chrono::high_resolution_clock::now();

      // Update layers
      for (auto& kvp : m_layers) {
        kvp.second->update(start);
      }

      // Flatten layers
      // Reset state to defaults to start.
      for (auto& kvp : m_state) {
        kvp.second->reset();
      }

      // Sort active layers
      set<shared_ptr<Layer>, function<bool(shared_ptr<Layer>, shared_ptr<Layer>)> >
        sortedLayers([](shared_ptr<Layer> lhs, shared_ptr<Layer> rhs) { return (*lhs) < (*rhs); });

      for (auto& kvp : m_layers) {
        if (kvp.second->isActive()) {
          // sorting is handled automatically by set<> according to the stl spec
          sortedLayers.insert(kvp.second);
        }
      }

      // Blend active layers
      // Blending is done from the bottom up, with the state being passed to each
      // layer in order.
      for (auto& l : sortedLayers) {
        l->blend(m_state);
      }

      // Blend the programmer layer
      // This layer sits on top of everything else and anything captured by it
      // will take precedence over everything.
      m_prog->blend(m_state);

      // If we have a GM value less than 1, do some scaling
      if (m_grandmaster < 1) {
        for (const auto& d : m_state) {
          for (auto p : d.second->getRawParameters()) {
            LumiverseTypeUtils::scaleParam(p.second, m_grandmaster);
          }
        }
      }

      // Write state to rig.
      m_rig->setAllDevices(m_state);

      // For now I'm locking this to the update loop in rig
      // We'll see how it goes

      // Sleep a bit depending on how long the update took.
      // auto end = chrono::high_resolution_clock::now();
      //auto elapsed = end - start;
      //float elapsedSec = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / 1000.0f;

      //if (elapsedSec < m_loopTime) {
      //  unsigned int ms = (unsigned int)(1000 * (m_loopTime - elapsedSec));
      //  this_thread::sleep_for(chrono::milliseconds(ms));
      //}
      //else {
      //  Logger::log(WARN, "Playback Update loop running slowly");
      //}
    }
  }

  bool Playback::addLayer(shared_ptr<Layer> layer) {
    if (m_layers.count(layer->getName()) > 0) {
      // Key already exists
      return false;
    }
    else {
      m_layers[layer->getName()] = layer;
      return true;
    }
  }

  shared_ptr<Layer> Playback::getLayer(string name) {
    if (m_layers.count(name) > 0) {
      return m_layers[name];
    }

    return nullptr;
  }

  void Playback::deleteLayer(string name) {
    if (m_layers.count(name) > 0) {
      m_layers.erase(name);
    }
  }

  bool Playback::addTimeline(string id, shared_ptr<Timeline> tl) {
    if (m_timelines.count(id) == 0) {
      m_timelines[id] = tl;
      return true;
    }

    return false;
  }

  void Playback::deleteTimeline(string id) {
    m_timelines.erase(id);
  }

  shared_ptr<Timeline> Playback::getTimeline(string id) {
    if (m_timelines.count(id) > 0) {
      return m_timelines[id];
    }

    return nullptr;
  }

  map<string, shared_ptr<Timeline> >& Playback::getTimelines() {
    return m_timelines;
  }

  bool Playback::attachToRig(int pid) {
    // Bind update function to rig update function
    if (pid > 0 && m_rig->addFunction(pid, [&]() { this->update(); })) {
      m_funcId = pid;
      return true;
    }
    else
      return false;
  }
  
  bool Playback::detachFromRig() {
    if (m_funcId > 0) {
      return m_rig->removeFunction(m_funcId);
    }
    else
      return false;
  }

  bool Playback::save(string filename, bool overwrite) {
    // Test if the file already exists.
    ifstream ifile(filename);
    if (ifile.is_open() && !overwrite) {
      return false;
    }
    ifile.close();

    ofstream pbFile;
    pbFile.open(filename, ios::out | ios::trunc);
    pbFile << toJSON().write_formatted();

    return true;
  }

  JSONNode Playback::toJSON() {
    JSONNode root;

    JSONNode pb;
    pb.set_name("playback");
    stringstream ss;
    ss << LumiverseCore_VERSION_MAJOR << "." << LumiverseCore_VERSION_MINOR;
    pb.push_back(JSONNode("version", ss.str()));

    // Easy stuff first
    pb.push_back(JSONNode("grandmaster", m_grandmaster));

    // Timelines first.
    JSONNode lists;
    lists.set_name("timelines");

    for (auto& kvp : m_timelines) {
      JSONNode timeline = kvp.second->toJSON();
      timeline.set_name(kvp.first);
      lists.push_back(timeline);
    }
    pb.push_back(lists);

    // Layers next
    JSONNode layers;
    layers.set_name("layers");

    for (auto& kvp : m_layers) {
      layers.push_back(kvp.second->toJSON());
    }
    pb.push_back(layers);

    // Then groups
    JSONNode groups;
    groups.set_name("groups");

    for (auto& kvp : m_groups) {
      groups.push_back(kvp.second.toJSON(kvp.first));
    }
    pb.push_back(groups);

    // Then dynamic groups
    JSONNode dynGroups;
    dynGroups.set_name("dynamic_groups");

    for (auto& kvp : m_dynGroups) {
      dynGroups.push_back(kvp.second.toJSON(kvp.first));
    }
    pb.push_back(dynGroups);

    pb.push_back(m_prog->toJSON());

    root.push_back(pb);
    return root;
  }

  vector<string> Playback::getLayerNames() {
    vector<string> names;

    for (const auto& kvp : m_layers) {
      names.push_back(kvp.first);
    }

    return names;
  }

  vector<string> Playback::getTimelineNames() {
    vector<string> names;

    for (const auto& kvp : m_timelines) {
      names.push_back(kvp.first);
    }

    return names;
  }

  bool Playback::load(string filename) {
    ifstream data;
    data.open(filename, ios::in | ios::binary | ios::ate);

    if (data.is_open()) {
      streamoff size = data.tellg();
      char* memblock = new char[(unsigned int)size];

      data.seekg(0, ios::beg);

      stringstream ss;
      ss << "Loading " << size << " bytes from " << filename;
      Logger::log(INFO, ss.str());

      data.read(memblock, size);
      data.close();

      JSONNode n = libjson::parse(memblock);

      delete memblock;
      return loadJSON(n);
    }
    else {
      stringstream ss;
      ss << "Unable to load playback data from " << filename;
      Logger::log(ERR, ss.str());
      return false;
    }
  }

  bool Playback::loadJSON(JSONNode node) {
    m_layers.clear();
    m_timelines.clear();

    auto data = node.find("playback");
    if (data == node.end()) {
      Logger::log(ERR, "No Playback data found");
      return false;
    }

    auto version = data->find("version");
    if (version == data->end()) {
      Logger::log(WARN, "Loading data from unknown version of Lumiverse. Load may not complete correctly.");
    }
    else {
      string ver = version->as_string();
      stringstream ss(ver);
      float verNum;
      ss >> verNum;

      if (verNum < 2.0) {
        Logger::log(WARN, "Playbacks older than version 2 cannot be loaded.");
        return false;
      }
    }

    auto gm = data->find("grandmaster");
    if (gm == data->end()) {
      Logger::log(INFO, "No Grandmaster value found, defaulting to 1.");
      m_grandmaster = 1;
    }
    else {
      m_grandmaster = gm->as_float();
    }
    
    auto timelines = data->find("timelines");
    if (timelines == data->end()) {
      Logger::log(INFO, "No timelines found for Playback.");
    }
    else {
      auto it = timelines->begin();
      while (it != timelines->end()) {
        auto type = it->find("type");
        if (type != it->end()) {
          string t = type->as_string();
          if (t == "timeline") {
            m_timelines[it->name()] = shared_ptr<Timeline>(new Timeline(*it));
          }
          else if (t == "sinewave") {
            m_timelines[it->name()] = shared_ptr<Timeline>(new SineWave(*it));
          }
        }
        Logger::log(INFO, "Loaded timeline " + it->name());
        it++;
      }
    }

    auto layers = data->find("layers");
    if (layers == data->end()) {
      Logger::log(INFO, "No layers found for Playback.");
    }
    else {
      auto it = layers->begin();
      while (it != layers->end()) {
        m_layers[it->name()] = shared_ptr<Layer>(new Layer(const_cast<Playback*>(this), *it));

        // TODO: Change how layers find what's assigned to them
        //auto cueList = it->find("cueList");
        //if (cueList != it->end()) {
          // Got a cue list to assign
        //  addCueListToLayer(cueList->as_string(), it->name(), false);
        //}

        it++;
      }
    }

    auto groups = data->find("groups");
    if (groups == data->end()) {
      Logger::log(INFO, "No groups found for Playback.");
    }
    else {
      auto it = groups->begin();
      while (it != groups->end()) {
        m_groups[it->name()] = DeviceSet(getRig(), *it);

        it++;
      }
    }

    auto dynGroups = data->find("dynamic_groups");
    if (dynGroups == data->end()) {
      Logger::log(INFO, "No dynamic groups found for Playback.");
    }
    else {
      auto it = dynGroups->begin();
      while (it != dynGroups->end()) {
        m_dynGroups[it->name()] = DynamicDeviceSet(getRig(), *it);

        it++;
      }
    }

    auto prog = data->find("programmer");
    if (prog == data->end()) {
      Logger::log(WARN, "No programmer data found");
    }
    else {
      m_prog->loadJSON(*prog);
    }

    return true;
  }

  int Playback::getNumLayers() {
    return m_layers.size();
  }

  bool Playback::storeGroup(string name, DeviceSet group, bool overwrite) {
    if (!overwrite && m_groups.count(name) > 0)
    {
      Logger::log(ERR, "Group with name " + name + " already exists");
      return false;
    }

    m_groups[name] = group;
    return true;
  }

  bool Playback::storeDynamicGroup(string name, DynamicDeviceSet group, bool overwrite) {
    if (!overwrite && m_dynGroups.count(name) > 0) {
      Logger::log(ERR, "Group with name " + name + " already exists");
      return false;
    }

    m_dynGroups[name] = group;
    return true;
  }

  bool Playback::deleteGroup(string name) {
    return m_groups.erase(name) > 0;
  }

  bool Playback::deleteDynamicGroup(string name) {
    return m_dynGroups.erase(name) > 0;
  }

  DeviceSet Playback::getGroup(string name) {
    if (m_groups.count(name) == 0) {
      Logger::log(WARN, "Group " + name + " not found. Returning default group...");
      return DeviceSet(getRig());
    }

    return m_groups[name];
  }

  DynamicDeviceSet Playback::getDynamicGroup(string name) {
    if (m_dynGroups.count(name) == 0) {
      Logger::log(WARN, "Group " + name + " not found. Returning default group...");
      return DynamicDeviceSet(getRig(), "");
    }

    return m_dynGroups[name];
  }

  bool Playback::groupExists(string name) {
    return m_groups.count(name) > 0;
  }

  bool Playback::dynamicGroupExists(string name) {
    return m_dynGroups.count(name) > 0;
  }

  void Playback::setGrandmaster(float val) {
    m_grandmaster = (val > 1) ? 1 : ((val < 0) ? 0 : val);
  }
}
}