#include "DeviceSet.h"

DeviceSet::DeviceSet(Rig* rig) : m_rig(rig) {
  // look it's empty
}

DeviceSet::DeviceSet(Rig* rig, set<Device *> devices) : m_rig(rig) {
  m_workingSet = set<Device *>(devices);
}

DeviceSet::DeviceSet(DeviceSet& dc) {
  m_workingSet = set<Device *>(dc.m_workingSet);
  m_rig = dc.m_rig;
}

DeviceSet::~DeviceSet() {
  // Nothing for now
}

DeviceSet DeviceSet::select(string selector) {
  // Select a group of ids \[?([^\]\[]+)\]? - after this will need to parse
  // Metadata selector (may need to be more general at some point) \[\$([\w\d\-]+)([\|\*~\$\^]?[=])([\w\d\-]+)\]

}

DeviceSet DeviceSet::add(Device* device) {
  DeviceSet newSet(*this);
  newSet.addDevice(device);

  return newSet;
}

DeviceSet DeviceSet::add(string id) {
  return add(m_rig->m_devicesById[id]);
}

DeviceSet DeviceSet::add(unsigned int channel) {
  DeviceSet newSet(*this);

  auto range = m_rig->m_devicesByChannel.equal_range(channel);
  for (auto it = range.first; it != range.second; it++) {
    newSet.addDevice(it->second);
  }

  return newSet;
}

DeviceSet DeviceSet::add(unsigned int lower, unsigned int upper) {
  DeviceSet newSet(*this);
  
  for (unsigned int i = lower; i <= upper; i++) {
    auto range = m_rig->m_devicesByChannel.equal_range(i);
    for (auto it = range.first; it != range.second; it++) {
      newSet.addDevice(it->second);
    }
  }

  return newSet;
}

DeviceSet DeviceSet::add(string key, string val, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_rig->m_devices) {
    string data;
    if (d->getMetadata(key, data)) {
      if ((data == val) == isEqual) {
        newSet.addDevice(d);
      }
    }
  }

  return newSet;
}

DeviceSet DeviceSet::remove(Device* device) {
  DeviceSet newSet(*this);
  newSet.removeDevice(device);
  return newSet;
}

DeviceSet DeviceSet::remove(string id) {
  return remove(m_rig->m_devicesById[id]);
}

DeviceSet DeviceSet::remove(unsigned int channel) {
  DeviceSet newSet(*this);

  auto range = m_rig->m_devicesByChannel.equal_range(channel);
  for (auto it = range.first; it != range.second; it++) {
    newSet.removeDevice(it->second);
  }

  return newSet;
}

DeviceSet DeviceSet::remove(unsigned int lower, unsigned int upper) {
  DeviceSet newSet(*this);

  for (unsigned int i = lower; i <= upper; i++) {
    auto range = m_rig->m_devicesByChannel.equal_range(i);
    for (auto it = range.first; it != range.second; it++) {
      newSet.removeDevice(it->second);
    }
  }

  return newSet;
}

DeviceSet DeviceSet::remove(string key, string val, bool isEqual) {
  DeviceSet newSet(*this);

  for (auto d : m_rig->m_devices) {
    string data;
    if (d->getMetadata(key, data)) {
      if ((data == val) == isEqual) {
        newSet.removeDevice(d);
      }
    }
  }

  return newSet;
}

void DeviceSet::addDevice(Device* device) {
  m_workingSet.insert(device);
}

void DeviceSet::removeDevice(Device* device) {
  m_workingSet.erase(device);
}

void DeviceSet::setParam(string param, float val) {
  for (auto d : m_workingSet) {
    if (d->paramExists(param)) {
      d->setParam(param, val);
    }
  }
}