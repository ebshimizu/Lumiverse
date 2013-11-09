#include "Rig.h"


Rig::Rig() {
  m_running = false;
  setRefreshRate(40);
}

Rig::~Rig() {
  // Stop the update thread.
  stop();
  delete m_updateLoop;
  
  // Delete Devices
  for (auto& d : m_devices) {
    delete d;
  }

  // Delete Patches
  for (auto& p : m_patches) {
    delete p.second;
  }
}

void Rig::init() {
  for (auto& p : m_patches) {
    p.second->init();
  }
}

void Rig::run() {
  m_running = true;
  m_updateLoop = new thread(&Rig::update, this);
}

void Rig::stop() {
  m_running = false;
  m_updateLoop->join();
}

void Rig::addDevice(Device* device) {
  // Don't add duplicates.
  if (m_devicesById.count(device->getId()) > 0)
    return;

  m_devices.push_back(device);
  m_devicesById[device->getId()] = device;
}

Device* Rig::getDevice(string id) {
  // If it doesn't exist, just return a null pointer.
  if (m_devicesById.count(id) == 0) {
    return nullptr;
  }
  return m_devicesById[id];
}

void Rig::deleteDevice(string id) {
  if (m_devicesById.count(id) == 0)
    return;

  // Find the device in the vector and delete it. Yay vectors.
  m_devices.erase(find(m_devices.begin(), m_devices.end(), m_devicesById[id]));

  // Delete the memory used by the device using the id->device map
  delete m_devicesById[id];
  m_devicesById.erase(id);
}

void Rig::addPatch(string id, Patch* patch) {
  // No duplicates.
  if (m_patches.count(id) > 0)
    return;

  m_patches[id] = patch;
}

Patch* Rig::getPatch(string id) {
  return (m_patches.count(id) > 0) ? m_patches[id] : nullptr;
}

void Rig::deletePatch(string id) {
  if (m_patches.count(id) == 0)
    return;

  // Free up space and erase the patch from the map
  delete m_patches[id];
  m_patches.erase(id);
}

void Rig::setRefreshRate(unsigned int rate) {
  m_refreshRate = rate;
  m_loopTime = 1.0f / (float)m_refreshRate;
}

void Rig::update() {
  while (m_running) {
    // Get start time
    clock_t start, end;
    start = clock();

    // Run the whole update thing for all patches
    for (auto& p : m_patches) {
      p.second->update(m_devices);
    }

    // Sleep a bit depending on how long the update took.
    end = clock();
    float elapsed = (float)(end - start) / CLOCKS_PER_SEC;

    if (elapsed < m_loopTime) {
      unsigned int ms = (unsigned int)(1000 * (m_loopTime - elapsed));
      this_thread::sleep_for(chrono::milliseconds(ms));
    }
    else {
      cerr << "Warning: Rig Update loop running slowly.\n";
    }
  }
}

Device* Rig::operator[](string id) {
  return getDevice(id);
}