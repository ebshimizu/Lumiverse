#include "Programmer.h"

namespace Lumiverse {

Programmer::Programmer(Rig* rig) : m_rig(rig) {
  const set<Device*>& devices = m_rig->getDeviceRaw();

  for (Device* d : devices) {
    m_devices[d->getId()] = new Device(d);
  }

  captured = DeviceSet(m_rig);
}

Programmer::~Programmer()
{
  for (auto& kvp : m_devices) {
    delete kvp.second;
  }
}

void Programmer::setParam(DeviceSet selection, string param, LumiverseType* val) {
  // add selection to captured
  captured = captured.add(selection);

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
  captured = captured.add(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, val);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, string val, float val2) {
  // add selection to captured
  captured = captured.add(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, val, val2);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, string channel, double val) {
  // add selection to captured
  captured = captured.add(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, channel, val);
    }
  }
}

void Programmer::setParam(DeviceSet selection, string param, double x, double y, double weight) {
  // add selection to captured
  captured = captured.add(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setParam(param, x, y, weight);
    }
  }
}

void Programmer::setColorRGB(DeviceSet selection, string param, double r, double g, double b, double weight, RGBColorSpace cs) {
  // add selection to captured
  captured = captured.add(selection);

  for (Device* d : selection.getDevices()) {
    if (m_devices.count(d->getId()) > 0) {
      m_devices[d->getId()]->setColorRGB(param, r, g, b, weight, cs);
    }
  }
}

void Programmer::setColorRGBRaw(DeviceSet selection, string param, double r, double g, double b, double weight) {
  // add selection to captured
  captured = captured.add(selection);

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
  captured = captured.add(id);
  return m_devices.count(id) > 0 ? m_devices[id] : nullptr;
}

void Programmer::clearCaptured() {
  captured = DeviceSet(m_rig);
}

void Programmer::reset() {
  for (auto& kvp : m_devices) {
    kvp.second->reset();
  }
}

void Programmer::clearAndReset() {
  clearCaptured();
  reset();
}

map<string, Device*> Programmer::getCapturedDevices() {
  map<string, Device*> devices;
  for (Device* d : captured.getDevices()) {
    devices[d->getId()] = d;
  }

  return devices;
}

void Programmer::blend(map<string, Device*> state) {
  // Take each captured device, and write the parameters in.
  for (Device* d : captured.getDevices()) {
    for (auto& p : d->getRawParameters()) {
      LumiverseTypeUtils::copyByVal(m_devices[d->getId()]->getParam(p.first), state[d->getId()]->getParam(p.first));
    }
  }
}

Cue Programmer::getCue(float time) {
  Cue cue(m_devices, time);
  return cue;
}

void Programmer::captureFromRig(DeviceSet devices) {
  for (Device* d : devices.getDevices()) {
    for (auto& p : d->getRawParameters()) {
      LumiverseTypeUtils::copyByVal(m_rig->getDevice(d->getId())->getParam(p.first), m_devices[d->getId()]->getParam(p.first));
    }
  }

  captured = captured.add(devices);
}

}