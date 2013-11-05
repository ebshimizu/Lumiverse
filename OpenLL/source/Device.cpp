#include "Device.h"


Device::Device(string id, unsigned int channel, string type) {
  this->id = id;
  this->channel = channel;
  this->type = type;

  // Might auto-load parameters from device type file at some point.
  // Right now we just leave the maps empty and stuff.
}


Device::~Device() {
  // Hey look it's empty for now.
}

bool Device::getParam(string param, float& val) {
  if (parameters.count(param) > 0) {
    val = parameters[param];
    return true;
  }

  return false;
}

bool Device::setParam(string param, float val) {
  bool ret = false;

  if (parameters.count(param) == 0) {
    ret = true;
  }

  parameters[param] = val;
  return ret;
}

void Device::clearParamValues() {
  for (auto& kv : parameters) {
    kv.second = 0.0;
  }
}

unsigned int Device::numParams() {
  return parameters.size();
}

vector<string> Device::getParamNames() {
  vector<string> keys;
  for (const auto& kv : parameters) {
    keys.push_back(kv.first);
  }

  return keys;
}

bool Device::getMetadata(string key, string& val) {
  if (metadata.count(key) > 0) {
    val = metadata[key];
    return true;
  }
  
  return false;
}

bool Device::setMetadata(string key, string val) {
  bool ret = false;

  if (metadata.count(key) == 0) {
    ret = true;
  }

  metadata[key] = val;
  return ret;
}

void Device::clearMetadataValues() {
  for (auto& kv : metadata) {
    kv.second = "";
  }
}

void Device::clearAllMetadata() {
  metadata.clear();
}

unsigned int Device::numMetadataKeys() {
  return metadata.size();
}

vector<string> Device::getMetadataKeyNames() {
  vector<string> keys;
  for (const auto& kv : metadata) {
    keys.push_back(kv.first);
  }

  return keys;
}