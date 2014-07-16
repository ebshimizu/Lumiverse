#include "Playback.h"

namespace Lumiverse {
  Playback::Playback(Rig* rig, unsigned int refreshRate) : m_rig(rig) {
    setRefreshRate(refreshRate);
    m_running = false;

    for (auto d : *(m_rig->getAllDevices().getDevices())) {
      // Copy and reset to defaults
      m_state[d->getId()] = new Device(*d);
      m_state[d->getId()]->reset();
    }
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

  void Playback::run() {
    m_running = true;
    Logger::log(INFO, "Playback update loop ready.");
  }

  void Playback::halt() {
    if (m_running) {
      m_running = false;
      Logger::log(INFO, "Playback update loop stopped.");
    }
  }

  void Playback::setRefreshRate(unsigned int rate) {
    m_refreshRate = rate;
    m_loopTime = 1.0f / (float)m_refreshRate;
  }

  void Playback::update() {
    while (m_running) {
      // Gets start time
      auto start = chrono::high_resolution_clock::now();

      // Update layers
      for (auto kvp : m_layers) {
        kvp.second->update(start);
      }

      // Flatten layers
      // Reset state to defaults to start.
      for (auto kvp : m_state) {
        kvp.second->reset();
      }

      // Sort active layers
      set<shared_ptr<Layer>, function<bool(shared_ptr<Layer>, shared_ptr<Layer>)> >
        sortedLayers([](shared_ptr<Layer> lhs, shared_ptr<Layer> rhs) { return (*lhs) < (*rhs); });

      for (auto kvp : m_layers) {
        if (kvp.second->isActive()) {
          // sorting is handled automatically by set<> according to the stl spec
          sortedLayers.insert(kvp.second);
        }
      }

      // Blend active layers
      // Blending is done from the bottom up, with the state being passed to each
      // layer in order.
      for (auto l : sortedLayers) {
        l->blend(m_state);
      }

      // Write state to rig.
      m_rig->setAllDevices(m_state);

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
}