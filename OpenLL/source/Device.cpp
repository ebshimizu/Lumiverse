#include "Device.h"


Device::Device(string id, unsigned int channel, string type) {
  this->m_id = id;
  this->m_channel = channel;
  this->m_type = type;

  // Might auto-load parameters from device type file at some point.
  // Right now we just leave the maps empty and stuff.
}


Device::~Device() {
  for (auto& kv : m_parameters) {
    delete kv.second;
  }
}

bool Device::getParam(string param, float& val) {
  if (m_parameters.count(param) > 0) {
    val = ((OpenLLFloat*)m_parameters[param])->getVal();;
    return true;
  }

  return false;
}

bool Device::setParam(string param, float val) {
  bool ret = false;

  if (m_parameters.count(param) == 0) {
    ret = true;
    m_parameters[param] = (OpenLLType*) new OpenLLFloat();
  }

  *((OpenLLFloat *)m_parameters[param]) = val;
  return ret;
}

bool Device::paramExists(string param) {
  return (m_parameters.count(param) > 0);
}

void Device::clearParamValues() {
  for (auto& kv : m_parameters) {
    kv.second->reset();
  }
}

unsigned int Device::numParams() {
  return m_parameters.size();
}

vector<string> Device::getParamNames() {
  vector<string> keys;
  for (const auto& kv : m_parameters) {
    keys.push_back(kv.first);
  }

  return keys;
}

bool Device::getMetadata(string key, string& val) {
  if (m_metadata.count(key) > 0) {
    val = m_metadata[key];
    return true;
  }
  
  return false;
}

bool Device::setMetadata(string key, string val) {
  bool ret = false;

  if (m_metadata.count(key) == 0) {
    ret = true;
  }

  m_metadata[key] = val;
  return ret;
}

void Device::clearMetadataValues() {
  for (auto& kv : m_metadata) {
    kv.second = "";
  }
}

void Device::clearAllMetadata() {
  m_metadata.clear();
}

unsigned int Device::numMetadataKeys() {
  return m_metadata.size();
}

vector<string> Device::getMetadataKeyNames() {
  vector<string> keys;
  for (const auto& kv : m_metadata) {
    keys.push_back(kv.first);
  }

  return keys;
}