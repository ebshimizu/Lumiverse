#include "DMXPatch.h"
#include "DMXPatch.h"


DMXPatch::DMXPatch() {
  // Empty for now
}

DMXPatch::~DMXPatch() {
  // Deallocate all interfaces after closing them.
  for (auto& interfaces : m_interfaces) {
    interfaces.second->close();
    delete interfaces.second;
  }

  // Deallocate all patch objects
  for (auto& patches : m_patch) {
    delete patches.second;
  }
}

void DMXPatch::update(vector<Device *> devices) {
  for (Device* d : devices) {
    // Skip if there is no DMX patch for the device stored
    if (m_patch.count(d->getId()) == 0)
      continue;

    // For each device, find the device patch stored.
    DMXDevicePatch devPatch = *(m_patch)[d->getId()];
    unsigned int uni = devPatch.getUniverse();
    devPatch.updateDMX(&m_universes[uni].front(), d);
  }

  // Send updated data to interfaces
  for (auto& i : m_ifacePatch) {
    m_interfaces[i.first]->sendDMX(&m_universes[i.second].front(), i.second);
  }
}

void DMXPatch::init() {
  for (auto& iface : m_interfaces) {
    iface.second->init();
  }
}

void DMXPatch::close() {
  for (auto& interfaces : m_interfaces) {
    interfaces.second->close();
  }
}

void DMXPatch::assignInterface(DMXInterface* iface, unsigned int universe) {
  string id = iface->getInterfaceId();

  // Add to the interface list if doesn't exist.
  if (m_interfaces.count(id) == 0) {
    m_interfaces[id] = iface;
  }

  // Add to the interface patch.
  // But first check to see if the interface is already mapped to the given universe.
  auto ret = m_ifacePatch.equal_range(id);
  for (auto it = ret.first; it != ret.second; ++it) {
    if (it->second == universe)
      return;
  }

  m_ifacePatch.insert(make_pair(id, universe));

  // Update universe vector size.
  if (universe + 1 > m_universes.size()) {
    m_universes.resize(universe + 1);
    for (auto& uni : m_universes)
      uni.resize(512);
  }
}

void DMXPatch::deleteInterface(string id) {
  // Close and delete the interface
  m_interfaces[id]->close();
  delete m_interfaces[id];

  // Remove from the patch maps
  m_interfaces.erase(id);
  m_ifacePatch.erase(id);
}

void DMXPatch::moveInterface(string id, unsigned int universeFrom, unsigned int universeTo) {
  // Find and delete the from element.
  auto fromLoc = m_ifacePatch.equal_range(id);

  for (auto it = fromLoc.first; it != fromLoc.second; ++it) {
    if (it->second == universeFrom) {
      m_ifacePatch.erase(it);
      break;
    }
  }

  // Insert the to element.
  m_ifacePatch.insert(make_pair(id, universeTo));
}

void DMXPatch::patchDevice(Device* device, DMXDevicePatch* patch) {
  m_patch[device->getId()] = patch;
}

void DMXPatch::dumpUniverses() {
  for (int i = 0; i < m_universes.size(); i++) {
    dumpUniverse(i);
  }
}

void DMXPatch::dumpUniverse(unsigned int universe) {
  vector<unsigned char> uni = m_universes[universe];

  cout << "Universe " << universe << "\n";
  for (int i = 0; i < uni.size(); i++) {
    cout << i << ":" << (int)uni[i] << "\n";
  }
  cout << "\n";
}