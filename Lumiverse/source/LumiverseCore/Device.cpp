#include "Device.h"
namespace Lumiverse {

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

Device::Device(const Device& other) {
  m_id = other.m_id;
  m_channel = other.m_channel;
  m_type = other.m_type;

  // Need to do a deep copy of the parameters
  for (auto kvp : other.m_parameters) {
    m_parameters[kvp.first] = LumiverseTypeUtils::copy(kvp.second);
  }

  m_metadata = other.m_metadata;
}

Device::Device(Device* other) {
  m_id = other->m_id;
  m_channel = other->m_channel;
  m_type = other->m_type;

  // Need to do a deep copy of the parameters
  for (auto kvp : other->m_parameters) {
    m_parameters[kvp.first] = LumiverseTypeUtils::copy(kvp.second);
  }

  m_metadata = other->m_metadata;
}

Device::~Device() {
  for (auto& kv : m_parameters) {
    delete kv.second;
  }
}

bool Device::getParam(string param, float& val) {
  if (m_parameters.count(param) > 0) {
    if (m_parameters[param]->getTypeName() == "float") {
      val = ((LumiverseFloat*)m_parameters[param])->getVal();
      return true;
    }
  }

  return false;
}

LumiverseType* Device::getParam(string param) {
  if (m_parameters.count(param) > 0) {
    return m_parameters[param];
  }

  return nullptr;
}

LumiverseColor* Device::getColor(string param) {
  if (m_parameters.count(param) > 0) {
      if (m_parameters[param]->getTypeName() == "color") {
          return (LumiverseColor*)(m_parameters[param]);
      }
  }

  return nullptr;
}

bool Device::setParam(string param, LumiverseType* val) {
  bool ret = true;

  if (m_parameters.count(param) == 0) {
    ret = false;
  }
  else {
    // Delete old value to avoid leaking memory.
    // tbh this function feels a bit unsafe, considering ways to change it.
    delete m_parameters[param];
  }

  m_parameters[param] = val;

  // callback
  onParameterChanged();
    
  return ret;
}


bool Device::setParam(string param, float val) {

  bool ret = true;

  // Checks param type
  if (m_parameters.count(param) == 0 ||
      (m_parameters[param]->getTypeName() != "float" &&
      m_parameters[param]->getTypeName() != "orientation")) {
      Logger::log(ERR, "Parameter doesn't exist or trying to assign float value to a non-float type.");
      
      return false;
  }
    
  if (m_parameters[param]->getTypeName() == "float"){
	  *((LumiverseFloat *)m_parameters[param]) = val;
  }
  else 
    *((LumiverseOrientation *)m_parameters[param]) = val;

  // callback
  onParameterChanged();
    
  return ret;
}

bool Device::setParam(string param, string val, float val2) {
  if (m_parameters.count(param) == 0) {
    return false;
  }

  // Checks param type
  if (m_parameters[param]->getTypeName() != "enum") {
    Logger::log(ERR, "Trying to assign enum value to a non-enum type.");
        
    return false;
  }
    
  LumiverseEnum* data = (LumiverseEnum *)m_parameters[param];
  if (!data->setVal(val))
    return false;

  if (val2 >= 0) {
    data->setTweak(val2);
  }

  // callback
  onParameterChanged();
    
  return true;
}

bool Device::setParam(string param, string val, float val2, LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "enum") {
    return false;
  }
    
  ((LumiverseEnum *)m_parameters[param])->setVal(val, val2, mode, interpMode);

  // callback
  onParameterChanged();
    
  return true;
}

bool Device::setParam(string param, string channel, double val) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  LumiverseColor* data = (LumiverseColor*)m_parameters[param];
  bool ret;
  if ((ret = data->setColorChannel(channel, val))) {
    // callback
    onParameterChanged();
  }
    
  return ret;
}

bool Device::setParam(string param, double x, double y, double weight) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  ((LumiverseColor*)m_parameters[param])->setxy(x, y, weight);

  // callback
  onParameterChanged();
    
  return true;
}

bool Device::setColorRGBRaw(string param, double r, double g, double b, double weight) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  ((LumiverseColor*)m_parameters[param])->setRGBRaw(r, g, b, weight);

  // callback
  onParameterChanged();
    
  return true;
}

bool Device::setColorRGB(string param, double r, double g, double b, double weight, RGBColorSpace cs) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  ((LumiverseColor*)m_parameters[param])->setRGB(r, g, b, weight, cs);

  // callback
  onParameterChanged();
    
  return true;
}
    
void Device::copyParamByValue(string param, LumiverseType* source) {
    LumiverseType *target = getParam(param);
    
	// Skips this copy if the target value equals the current value
    if (!LumiverseTypeUtils::areSameType(source, target) ||
		LumiverseTypeUtils::equals(target, source))
        return;
    
    if (source->getTypeName() == "float") {
        *((LumiverseFloat*)target) = *((LumiverseFloat*)source);
    }
    else if (source->getTypeName() == "enum") {
        *((LumiverseEnum*)target) = *((LumiverseEnum*)source);
    }
    else if (source->getTypeName() == "color") {
        *((LumiverseColor*)target) = *((LumiverseColor*)source);
    }
	else if (source->getTypeName() == "orientation") {
		*((LumiverseOrientation*)target) = *((LumiverseOrientation*)source);
	}
    else {
        return;
    }
    
    onParameterChanged();
}
    
bool Device::paramExists(string param) {
  return (m_parameters.count(param) > 0);
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

bool Device::metadataExists(string key) {
  return m_metadata.count(key) > 0;
}

bool Device::getMetadata(string key, string& val) {
  if (m_metadata.count(key) > 0) {
    val = m_metadata[key];
    return true;
  }

  return false;
}

string Device::getMetadata(string key) {
  if (m_metadata.count(key) > 0) {
    return m_metadata[key];
  }

  return "";
}

bool Device::setMetadata(string key, string val) {
  bool ret = true;

  if (m_metadata.count(key) == 0) {
    ret = false;
  }

  m_metadata[key] = val;
    
  // callback
  onMetadataChanged();
    
  return ret;
}

void Device::deleteMetadata(string key) {
	if (m_metadata.count(key) != 0) {
		m_metadata.erase(key);

		// callback
		onMetadataChanged();
	}
}

void Device::clearMetadataValues() {
  for (auto& kv : m_metadata) {
    kv.second = "";
  }
    
  // callback
  onMetadataChanged();
}

void Device::clearAllMetadata() {
  m_metadata.clear();
    
  // callback
  onMetadataChanged();
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

void Device::reset() {
  for (auto p : m_parameters) {
    p.second->reset();
  }
    
  // callback
  onParameterChanged();
  onMetadataChanged();
}

string Device::toString() {
  if (this == nullptr) {
    return "Device does not exist";
  }
  JSONNode dev = toJSON();

  string out = dev.write_formatted();
  return out;
}

JSONNode Device::toJSON() {
  JSONNode root;

  // Set node name to id.
  root.set_name(m_id);

  // Add the device's properties
  root.push_back(JSONNode("channel", m_channel));
  root.push_back(JSONNode("type", m_type));

  // Add the parameters
  root.push_back(parametersToJSON());

  // Add the metadata
  root.push_back(metadataToJSON());

  return root;
}

int Device::addParameterChangedCallback(DeviceCallbackFunction func) {
    size_t id = m_onParameterChangedFunctions.size();
    m_onParameterChangedFunctions[id] = func;
    return id;
}

int Device::addMetadataChangedCallback(DeviceCallbackFunction func) {
    size_t id = m_onMetadataChangedFunctions.size();
    m_onMetadataChangedFunctions[id] = func;

    return id;
}
    
void Device::deleteParameterChangedCallback(int id) {
  if (m_onParameterChangedFunctions.count(id) > 0) {
    m_onParameterChangedFunctions.erase(id);
  }
}

void Device::deleteMetadataChangedCallback(int id) {
  if (m_onMetadataChangedFunctions.count(id) > 0) {
    m_onMetadataChangedFunctions.erase(id);
  }
}

bool Device::isIdentical(Device* d) {
  if (m_id != d->getId()) return false;
  if (m_channel != d->getChannel()) return false;
  if (m_type != d->getType()) return false;

  // parameter check
  if (m_parameters.size() != d->m_parameters.size())
    return false;

  for (auto p : m_parameters) {
    // If a parameter doesn't exist in the other device, return false
    // immediately, they must have the same parameter count at this point.
    if (!d->paramExists(p.first))
      return false;

    if (!LumiverseTypeUtils::equals(p.second, d->getParam(p.first)))
      return false;
  }

  // metadata check
  if (m_metadata.size() != d->m_metadata.size())
    return false;

  for (auto m : m_metadata) {
    string otherMData;
    if (!getMetadata(m.first, otherMData))
      return false;

    if (m.second != otherMData)
      return false;
  }

  // We don't check the callback functions since they're more attached
  // to the device rather than intrinsic properties of the device.
  return true;
}

Eigen::Vector3d Device::getGelColor() {
  // must be exact paramter match.
  if (metadataExists("gel") && paramExists("intensity")) {
    float intens;
    getParam("intensity", intens);
    return ColorUtils::getScaledColor(m_metadata["gel"], intens);
  }

  // If no color known, return a N/C gel if intensity exists
  if (paramExists("intensity")) {
    float intens;
    getParam("intensity", intens);
    return ColorUtils::getScaledColor("N/C", intens);
  }

  // If everything else fails, return illuminant A
  return refWhites[A];
}

JSONNode Device::parametersToJSON() {
  JSONNode params;
  params.set_name("parameters");

  for (std::pair<string, LumiverseType*> p : m_parameters) {
    params.push_back(p.second->toJSON(p.first));
  }

  return params;
}

JSONNode Device::metadataToJSON() {
  JSONNode metadata;
  metadata.set_name("metadata");

  for (auto& m : m_metadata) {
    metadata.push_back(JSONNode(m.first, m.second));
  }

  return metadata;
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
      Logger::log(WARN, ss.str());
    }

    //increment the iterator
    ++i;
  }

  stringstream ss;
  ss << "Loaded " << m_type << " Device " << m_id << " (Channel " << m_channel << ")";
  Logger::log(INFO, ss.str());
}

void Device::loadParams(const JSONNode data) {
  JSONNode::const_iterator i = data.begin();

  while (i != data.end()){
    // get the node name and value as a string
    std::string paramName = i->name();

    // Go into the child node that has all the param data
    JSONNode paramData = *i;
    LumiverseType *val = LumiverseTypeUtils::loadFromJSON(paramData);
      
    if (val != nullptr)
        setParam(paramName, val);

    //increment the iterator
    ++i;
  }
}
    
void Device::onParameterChanged() {
    for (const auto& kvp : m_onParameterChangedFunctions) {
        kvp.second(this);
    }
}
    
void Device::onMetadataChanged(){
    for (const auto& kvp : m_onMetadataChangedFunctions) {
        kvp.second(this);
    }
}
    
}