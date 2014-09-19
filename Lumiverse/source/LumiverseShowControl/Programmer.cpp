#include "Programmer.h"
#include "types/LumiverseTypeUtils.h"

namespace Lumiverse {
namespace ShowControl {

Programmer::Programmer(Rig* rig) : m_rig(rig) {
  const set<Device*>& devices = m_rig->getDeviceRaw();

  for (Device* d : devices) {
    m_devices[d->getId()] = new Device(d);
  }

  captured = DeviceSet(m_rig);
}

Programmer::Programmer(Rig* rig, JSONNode data) : m_rig(rig) {
  loadJSON(data);
}

Programmer::~Programmer()
{
  for (auto& kvp : m_devices) {
    delete kvp.second;
  }
}

void Programmer::setParam(DeviceSet selection, string param, LumiverseType* val) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0 && m_devices[d->getId()]->paramExists(param)) {
      // copy data from val into the Programmer's device
      LumiverseTypeUtils::copyByVal(val, m_devices[d->getId()]->getParam(param));
    }
  }
}

void Programmer::setParam(string selection, string param, LumiverseType* val) {
  setParam(m_rig->query(selection), param, val);
}

void Programmer::setParam(DeviceSet selection, string param, float val) {
  // add selection to captured
  addCaptured(selection);;

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, val);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, string val, float val2) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, val, val2);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, string channel, double val) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, channel, val);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, double x, double y, double weight) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, x, y, weight);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, string val, float val2,
  LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode)
{
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, val, val2, mode, interpMode);
    }
  }
}

void Programmer::setColorRGB(DeviceSet selection, string param, double r, double g, double b, double weight, RGBColorSpace cs) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setColorRGB(param, r, g, b, weight, cs);
    }
  }
}

void Programmer::setColorRGBRaw(DeviceSet selection, string param, double r, double g, double b, double weight) {
  // add selection to captured
  addCaptured(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setColorRGBRaw(param, r, g, b, weight);
    }
  }
}

void Programmer::setParam(string selection, string param, float val) {
  setParam(m_rig->query(selection), param, val);
}

void Programmer::setParam(string selection, string param, string val, float val2) {
  setParam(m_rig->query(selection), param, val, val2);
}

void Programmer::setParam(string selection, string param, string channel, double val) {
  setParam(m_rig->query(selection), param, channel, val);
}

void Programmer::setParam(string selection, string param, double x, double y, double weight) {
  setParam(m_rig->query(selection), param, x, y, weight);
}

void Programmer::setParam(string selection, string param, string val, float val2,
  LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode)
{
  setParam(m_rig->query(selection), param, val, val2, mode, interpMode);
}

void Programmer::setColorRGB(string selection, string param, double r, double g, double b, double weight, RGBColorSpace cs) {
  setColorRGB(m_rig->query(selection), param, r, g, b, weight, cs);
}

void Programmer::setColorRGBRaw(string selection, string param, double r, double g, double b, double weight) {
  setColorRGBRaw(m_rig->query(selection), param, r, g, b, weight);
}

Device* Programmer::operator[](string id) {
  return getDevice(id);
}

Device* Programmer::getDevice(string id) {
  addCaptured(id);
  return m_devices.count(id) > 0 ? m_devices[id] : nullptr;
}

const Device* Programmer::readDevice(string id) {
  return m_devices.count(id) > 0 ? m_devices[id] : nullptr;
}

void Programmer::captureDevices(DeviceSet d) {
  addCaptured(d);
}

void Programmer::clearCaptured() {
  m_progMutex.lock();
  captured = DeviceSet(m_rig);
  m_progMutex.unlock();
}

void Programmer::reset() {
  for (const auto& kvp : m_devices) {
    kvp.second->reset();
  }
}

void Programmer::clearAndReset() {
  clearCaptured();
  reset();
}

map<string, Device*> Programmer::getCapturedDevices() {
  map<string, Device*> devices;

  m_progMutex.lock();
  for (Device* d : captured.getDevices()) {
    devices[d->getId()] = d;
  }
  m_progMutex.unlock();

  return devices;
}

bool Programmer::isCaptured(string id) {
  return captured.contains(id);
}

void Programmer::blend(map<string, Device*> state) {
  m_progMutex.lock();

  // Take each captured device, and write the parameters in.
  for (Device* d : captured.getDevices()) {
    for (auto& p : d->getRawParameters()) {
      LumiverseTypeUtils::copyByVal(m_devices[d->getId()]->getParam(p.first), state[d->getId()]->getParam(p.first));
    }
  }

  m_progMutex.unlock();
}

Cue Programmer::getCue(float upfade, float downfade, float delay) {
  Cue cue(m_devices, upfade, downfade, delay);
  return cue;
}

void Programmer::captureFromRig(DeviceSet devices) {
  for (Device* d : devices.getDevices()) {
    for (auto& p : d->getRawParameters()) {
      LumiverseTypeUtils::copyByVal(m_rig->getDevice(d->getId())->getParam(p.first), m_devices[d->getId()]->getParam(p.first));
    }
  }

  addCaptured(devices);
}

void Programmer::captureFromRig(string id) {
  Device* d = m_rig->getDevice(id);

  if (d == nullptr)
    return;

  for (auto& p : d->getRawParameters()) {
    LumiverseTypeUtils::copyByVal(d->getParam(p.first), m_devices[id]->getParam(p.first));
  }

  addCaptured(id);
}

JSONNode Programmer::toJSON() {
  JSONNode root;
  root.set_name("programmer");

  JSONNode devices;
  devices.set_name("devices");

  for (const auto& kvp : m_devices) {
    devices.push_back(kvp.second->toJSON());
  }

  root.push_back(devices);

  root.push_back(captured.toJSON("captured"));

  return root;
}

bool Programmer::loadJSON(JSONNode data) {
  auto devices = data.find("devices");
  if (devices == data.end()) {
    Logger::log(ERR, "No devices found in Programmer");
    return false;
  }

  auto it = devices->begin();
  while (it != devices->end()) {
    Device* device = new Device(it->name(), *it);

    // We're overwriting data here, so make sure to actually delete the old data.
    // if there is any
    if (m_devices.count(device->getId()) > 0)
      delete m_devices[device->getId()];

    m_devices[device->getId()] = device;
    it++;
  }

  auto c = data.find("captured");
  if (c == data.end()) {
    Logger::log(WARN, "No captured set found in Programmer");
    captured = DeviceSet(m_rig);
  }
  else {
    captured = DeviceSet(m_rig, *c);
  }
}

void Programmer::addCaptured(DeviceSet set) {
  m_progMutex.lock();

  captured = captured.add(set);

  m_progMutex.unlock();
}

void Programmer::addCaptured(string id) {
  m_progMutex.lock();

  captured = captured.add(id);

  m_progMutex.unlock();
}

}
}