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

  void Playback::addLayer(shared_ptr<Layer> layer) {
    m_layers[layer->getName()] = layer;
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

  void Playback::addCueList(string id, shared_ptr<CueList> cueList) {
    m_cueLists[id] = cueList;
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

  bool Playback::addCueListToLayer(string cueListId, string layerName) {
    if (m_layers.count(layerName) > 0 && m_cueLists.count(cueListId) > 0) {
      m_layers[layerName]->setCueList(m_cueLists[cueListId]);
      return true;
    }

    return false;
  }

  void Playback::removeCueListFromLayer(string layerName) {
    if (m_layers.count(layerName) > 0) {
      m_layers[layerName]->removeCueList();
    }
  }

  void Playback::attachToRig(int pid) {
    // Bind update function to rig update function
    if (pid > 0 && m_rig->addFunction(pid, [&]() { this->update(); })) {
      m_funcId = pid;
    }
  }
  
  void Playback::detachFromRig() {
    if (m_funcId > 0) {
      m_rig->removeFunction(m_funcId);
    }
  }

}