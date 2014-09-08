#include "Playback.h"

namespace Lumiverse {
  Playback::Playback(Rig* rig) : m_rig(rig) {
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
  }

  Playback::~Playback() {
    stop();
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

  bool Playback::addCueList(shared_ptr<CueList> cueList) {
    if (m_cueLists.count(cueList->getName()) == 0) {
      m_cueLists[cueList->getName()] = cueList;
      return true;
    }
    else {
      stringstream ss;
      ss << "Playback already has a cue list named " << cueList->getName();
      Logger::log(ERR, ss.str());
      return false;
    }
  }

  shared_ptr<CueList> Playback::getCueList(string id) {
    if (m_cueLists.count(id) > 0) {
      return m_cueLists[id];
    }

    return nullptr;
  }

  void Playback::deleteCueList(string id) {
    if (m_cueLists.count(id) > 0) {
      m_cueLists.erase(id);
    }
  }

  bool Playback::addCueListToLayer(string cueListId, string layerName, bool resetCurrentCue) {
    if (m_layers.count(layerName) > 0 && m_cueLists.count(cueListId) > 0) {
      m_layers[layerName]->setCueList(m_cueLists[cueListId], resetCurrentCue);
      return true;
    }

    return false;
  }

  void Playback::removeCueListFromLayer(string layerName) {
    if (m_layers.count(layerName) > 0) {
      m_layers[layerName]->removeCueList();
    }
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

    // Cue Lists first.
    JSONNode lists;
    lists.set_name("cueLists");

    for (auto& kvp : m_cueLists) {
      lists.push_back(kvp.second->toJSON());
    }
    pb.push_back(lists);

    // Layers next
    JSONNode layers;
    layers.set_name("layers");

    for (auto& kvp : m_layers) {
      layers.push_back(kvp.second->toJSON());
    }
    pb.push_back(layers);

    root.push_back(pb);
    return root;
  }

  vector<string> Playback::getCueListNames() {
    vector<string> names;

    for (const auto& kvp : m_cueLists) {
      names.push_back(kvp.first);
    }

    return names;
  }

  vector<string> Playback::getLayerNames() {
    vector<string> names;

    for (const auto& kvp : m_layers) {
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
    auto data = node.find("playback");
    if (data == node.end()) {
      Logger::log(ERR, "No Playback data found");
      return false;
    }
    
    auto cueLists = data->find("cueLists");
    if (cueLists == data->end()) {
      Logger::log(INFO, "No cue lists found for Playback.");
    }
    else {
      auto it = cueLists->begin();
      while (it != cueLists->end()) {
        m_cueLists[it->name()] = shared_ptr<CueList>(new CueList(*it));

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
        m_layers[it->name()] = shared_ptr<Layer>(new Layer(*it));

        auto cueList = it->find("cueList");
        if (cueList != it->end()) {
          // Got a cue list to assign
          addCueListToLayer(cueList->as_string(), it->name(), false);
        }

        it++;
      }
    }

    return true;
  }

}