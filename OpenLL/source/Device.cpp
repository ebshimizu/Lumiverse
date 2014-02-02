#include "Device.h"


Device::Device(string id, unsigned int channel, string type) {
  this->m_id = id;
  this->m_channel = channel;
  this->m_type = type;

  // Might auto-load parameters from device type file at some point.
  // Right now we just leave the maps empty and stuff.
}

Device::Device(string id, const JSONNode data) {
  m_id = id;
  loadJSON(data);
}

void Device::loadJSON(const JSONNode data) {
  JSONNode::const_iterator i = data.begin();

  // for this we want to iterate through all children and have the device class
  // parse the sub-element.
  while (i != data.end()){
    // get the node name and value as a string
    std::string nodeName = i->name();

    if (nodeName == "channel") {
      m_channel = i->as_int();
    }
    else if (nodeName == "type") {
      m_type = i->as_string();
    }
    else if (nodeName == "parameters") {
      loadParams(*i);
    }
    else if (nodeName == "metadata") {
      JSONNode metaData = *i;

      auto meta = metaData.begin();
      while (meta != metaData.end()) {
        setMetadata(meta->name(), meta->as_string());
        ++meta;
      }
    }
    else {
      stringstream ss;
      ss << "Unknown child " << nodeName << " found in " << m_id;
      Logger::log(LOG_LEVEL::WARN, ss.str());
    }

    //increment the iterator
    ++i;
  }

  stringstream ss;
  ss << "Loaded " << m_type << " Device " << m_id << " (Channel " << m_channel << ")";
  Logger::log(LOG_LEVEL::INFO, ss.str());
}

void Device::loadParams(const JSONNode data) {
  JSONNode::const_iterator i = data.begin();

  while (i != data.end()){
    bool err = false;

    // get the node name and value as a string
    std::string paramName = i->name();

    // Go into the child node that has all the param data
    JSONNode paramData = *i;

    auto type = paramData.find("type");
    if (type != paramData.end()) {
      // TODO: Currently the only supported OpenLLType is float
      if (type->as_string() == "float") {
        auto valNode = paramData.find("val");
        auto defNode = paramData.find("default");

        if (valNode != paramData.end() && defNode != paramData.end()) {
          OpenLLFloat* param = new OpenLLFloat(valNode->as_float(), defNode->as_float());
          setParam(paramName, (OpenLLType*) param);
        }
        else {
          err = true;
        }
      }
      else {
        stringstream ss;
        ss << "Unsupported type " << type->as_string() << " in " << paramName << " in " << m_id << ". Parameter not set.";
        Logger::log(LOG_LEVEL::WARN, ss.str());
      }
    }
    else {
      err = true;
    }

    if (err) {
      stringstream ss;
      ss << "Invalid format for paramter " << paramName << " in " << m_id << ". Parameter not set.";
      Logger::log(LOG_LEVEL::WARN, ss.str());
    }

    //increment the iterator
    ++i;
  }
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

OpenLLType* Device::getParam(string param) {
  if (m_parameters.count(param) > 0) {
    return m_parameters[param];
  }

  return nullptr;
}

bool Device::setParam(string param, OpenLLType* val) {
  bool ret = false;

  if (m_parameters.count(param) == 0) {
    ret = true;
  }

  m_parameters[param] = val;
  return ret;
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