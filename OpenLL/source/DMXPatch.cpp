#include "DMXPatch.h"
#include "DMXPatch.h"


DMXPatch::DMXPatch() {
  // Empty for now
}

DMXPatch::DMXPatch(const JSONNode data) {
  loadJSON(data);
}

void DMXPatch::loadJSON(const JSONNode data) {
  string patchName = data.name();
  map<string, DMXInterface*> ifaceMap;

  auto i = data.begin();
  // This is a two pass process. First pass initializes the interfaces and Device mappings
  // Second pass actually patches devices and assigns the interfaces to universes.
  while (i != data.end()) {
    std::string nodeName = i->name();

    if (nodeName == "interfaces") {
      JSONNode interfaces = *i;

      auto iface = interfaces.begin();
      while (iface != interfaces.end()) {
        auto type = iface->find("type");

        if (type != iface->end()) {
          // Currently the only supported type is the DMX Pro Mk 2 Interface
          if (type->as_string() == "DMXPro2Interface") {
            auto proNumNode = iface->find("proNum");
            auto out1Node = iface->find("out1");
            auto out2Node = iface->find("out2");

            if (proNumNode != iface->end() && out1Node != iface->end() && out2Node != iface->end()) {
              DMXPro2Interface* intface = new DMXPro2Interface(iface->name(), proNumNode->as_int(), out1Node->as_int(), out2Node->as_int());
              ifaceMap[iface->name()] = (DMXInterface*)intface;
            }

            Logger::log(LOG_LEVEL::WARN, "Added DMX USB Pro Mk 2 Interface");
          }
          else {
            stringstream ss;
            ss << "Unsupported Interface Type " << type->name() << " in " << patchName;
            Logger::log(LOG_LEVEL::WARN, ss.str());
          }
        }

        ++iface;
      }
    }
    if (nodeName == "deviceMaps") {
      loadDeviceMaps(*i);
    }

    ++i;
  }

  // Assign universes to interfaces
  auto universes = data.find("universes");
  if (universes != data.end()) {
    auto universe = universes->begin();
    while (universe != universes->end()) {
      assignInterface(ifaceMap[universe->name()], universe->as_int());
      ++universe;
    }
  }
  else {
    Logger::log(LOG_LEVEL::WARN, "No interfaces assignments found in rig");
  }

  // Patch the devices
  auto devices = data.find("devicePatch");
  if (devices != data.end()) {
    auto device = devices->begin();
    while (device != devices->end()) {
      string mapKey = (*device)["mapType"].as_string();
      unsigned int addr = (*device)["addr"].as_int();
      unsigned int universe = (*device)["universe"].as_int();

      DMXDevicePatch* patch = new DMXDevicePatch(mapKey, addr, universe);
      patchDevice(device->name(), patch);

      stringstream ss;
      ss << "Patched " << device->name() << " to " << universe << "/" << addr << " using profile " << mapKey;
      Logger::log(LOG_LEVEL::INFO, ss.str());

      ++device;
    }
  }
  else {
    Logger::log(LOG_LEVEL::WARN, "No devices found in rig");
  }
}

void DMXPatch::loadDeviceMaps(const JSONNode data) {
  auto i = data.begin();

  while (i != data.end()) {
    string name = i->name();
    map<string, patchData> dmxMap;

    auto j = i->begin();
    while (j != i->end()) {
      string paramName = j->name();

      // This assumes the next piece of data is arranged in a [ int, string ] format
      unsigned int addr = (*j)[0].as_int();
      string conversion = (*j)[1].as_string();

      dmxMap[paramName] = patchData(addr, conversion);

      stringstream ss;
      ss << "Added DMX Map for " << paramName;
      Logger::log(LOG_LEVEL::INFO, ss.str());

      ++j;
    }

    addDeviceMap(name, dmxMap);
    ++i;
  }
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
    devPatch.updateDMX(&m_universes[uni].front(), d, m_deviceMaps[devPatch.getDMXMapKey()]);
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

void DMXPatch::patchDevice(string id, DMXDevicePatch* patch) {
  m_patch[id] = patch;
}

void DMXPatch::addDeviceMap(string id, map<string, patchData> deviceMap) {
  m_deviceMaps[id] = deviceMap; // Replaces existing maps.
}

void DMXPatch::addParameter(string mapId, string paramId, unsigned int address, conversionType type) {
  m_deviceMaps[mapId][paramId] = patchData(address, type);
}

void DMXPatch::dumpUniverses() {
  for (unsigned int i = 0; i < m_universes.size(); i++) {
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