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
  m_fp = other.m_fp;
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
  m_fp = other->m_fp;
}

Device::Device(string id, Device* other) {
  m_id = id;
  m_channel = other->m_channel;
  m_type = other->m_type;

  // Need to do a deep copy of the parameters
  for (auto kvp : other->m_parameters) {
    m_parameters[kvp.first] = LumiverseTypeUtils::copy(kvp.second);
  }

  m_metadata = other->m_metadata;
  m_fp = other->m_fp;
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
  unordered_map<string, LumiverseType*>::iterator it = m_parameters.find(param);
  if (it != m_parameters.end()) {
    return it->second;
  }
  return nullptr;
}

LumiverseFloat* Device::getFloat(string param) {
  auto ret = getParam(param);
  if (ret != nullptr) {
    if (ret->getTypeName() == "float") {
      return (LumiverseFloat*)(ret);
    }
    else {
      stringstream ss;
      ss << "Parameter " << param << " is not a LumiverseFloat";
      Logger::log(WARN, ss.str());
    }
  }
  return nullptr;
}

LumiverseEnum* Device::getEnum(string param) {
  auto ret = getParam(param);
  if (ret != nullptr) {
    if (ret->getTypeName() == "enum") {
      return (LumiverseEnum*)(ret);
    }
    else {
      stringstream ss;
      ss << "Parameter " << param << " is not a LumiverseEnum";
      Logger::log(WARN, ss.str());
    }
  }
  return nullptr;
}

LumiverseColor* Device::getColor(string param) {
  auto ret = getParam(param);
  if (ret != nullptr) {
    if (ret->getTypeName() == "color") {
      return (LumiverseColor*)(ret);
    }
    else {
      stringstream ss;
      ss << "Parameter " << param << " is not a LumiverseColor";
      Logger::log(WARN, ss.str());
    }
  }
  return nullptr;
}

LumiverseOrientation * Device::getOri(string param)
{
  auto ret = getParam(param);
  if (ret != nullptr) {
    if (ret->getTypeName() == "orientation") {
      return (LumiverseOrientation*)(ret);
    }
    else {
      stringstream ss;
      ss << "Parameter " << param << " is not a LumiverseOrientation";
      Logger::log(WARN, ss.str());
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

/*
bool Device::setParam(string param, shared_ptr<LumiverseFloat>* val) {
  return setParam(param, LumiverseTypeUtils::copy(val->get()));
}

bool Device::setParam(string param, shared_ptr<LumiverseEnum>* val) {
  return setParam(param, LumiverseTypeUtils::copy(val->get()));
}

bool Device::setParam(string param, shared_ptr<LumiverseColor>* val) {
  return setParam(param, LumiverseTypeUtils::copy(val->get()));
}

bool Device::setParam(string param, shared_ptr<LumiverseOrientation>* val) {
  return setParam(param, LumiverseTypeUtils::copy(val->get()));
}
*/

bool Device::setParam(string param, float val) {
  bool ret = true;

  // Checks param type
  if (m_parameters.count(param) == 0 ||
      (m_parameters[param]->getTypeName() != "float" &&
      m_parameters[param]->getTypeName() != "orientation")) {
      Logger::log(ERR, "Parameter doesn't exist or trying to assign float value to a non-float type.");
      
      return false;
  }
    
  if (m_parameters[param]->getTypeName() == "float")
    *((LumiverseFloat *)m_parameters[param]) = val;
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

bool Device::addFloatParam(string name, float val, float def, float max, float min)
{
  if (!paramExists(name)) {
    LumiverseFloat* newParam = new LumiverseFloat(val, def, max, min);
    return setParam(name, newParam);
  }

  return false;
}

bool Device::addColorParam(string name, int m)
{
  if (!paramExists(name)) {
    LumiverseColor* newParam = new LumiverseColor((ColorMode) m);
    return setParam(name, newParam);
  }

  return false;
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

bool Device::setColorHSV(string param, double H, double S, double V, double weight)
{
  if (m_parameters.count(param) == 0 ||
    m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  ((LumiverseColor*)m_parameters[param])->setHSV(H, S, V, weight);
  return true;
}

bool Device::setColorWeight(string param, double weight)
{
  if (m_parameters.count(param) == 0 ||
    m_parameters[param]->getTypeName() != "color") {
    return false;
  }

  ((LumiverseColor*)m_parameters[param])->setWeight(weight);
  return true;
}

bool Device::setRGBRaw(double r, double g, double b, double weight) {
  return setColorRGBRaw("color", r, g, b, weight);
}

LumiverseFloat* Device::getIntensity() {
  return (LumiverseFloat*)getParam("intensity");
}

bool Device::setColorChannel(string param, string channel, double val) {
  if (m_parameters.count(param) == 0 ||
      m_parameters[param]->getTypeName() != "color") {
      return false;
  }

  ((LumiverseColor*)m_parameters[param])->setColorChannel(channel, val);
  
  onParameterChanged();
  return true;
}
    
void Device::copyParamByValue(string param, LumiverseType* source) {
  LumiverseType *target = m_parameters[param];
    
	// Skips this copy if types don't match.
  if (!LumiverseTypeUtils::areSameType(source, target))
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
    
//  onParameterChanged();
}
    
bool Device::paramExists(string param) {
  unordered_map<string, LumiverseType*>::iterator it = m_parameters.find(param);
  if (it != m_parameters.end()) {
    if (it->second != nullptr)
      return true;
    else {
      // remove parameter to be safe if it's null
      m_parameters.erase(param);
      return false;
    }
  }
  return false;
}

size_t Device::numParams() {
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

void Device::deleteParameter(string key) {
  if (m_parameters.count(key) != 0) {
    delete m_parameters[key];
    m_parameters.erase(key);

    onParameterChanged();
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

size_t Device::numMetadataKeys() {
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

  // focus palettes
  JSONNode palettes;
  palettes.set_name("focusPalettes");
  for (auto& fp : m_fp) {
    JSONNode palette;
    palette.push_back(JSONNode("name", fp.second._name));
    palette.push_back(JSONNode("pan", fp.second._pan));
    palette.push_back(JSONNode("tilt", fp.second._tilt));
    palette.push_back(JSONNode("area", fp.second._area));
    palette.push_back(JSONNode("system", fp.second._system));
    palette.push_back(JSONNode("image", fp.second._image));

    palette.set_name(fp.first);
    palettes.push_back(palette);
  }
  
  root.push_back(palettes);

  return root;
}

int Device::addParameterChangedCallback(DeviceCallbackFunction func) {
    int id = (int)m_onParameterChangedFunctions.size();
    m_onParameterChangedFunctions[id] = func;

    return id;
}

int Device::addMetadataChangedCallback(DeviceCallbackFunction func) {
  int id = (int)m_onMetadataChangedFunctions.size();
  m_onMetadataChangedFunctions[id] = func;

  return (int)id;
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

void Device::addFocusPalette(FocusPalette fp)
{
  m_fp[fp._name] = fp;
}

FocusPalette * Device::getFocusPalette(string name)
{
  if (m_fp.count(name) > 0) {
    return &(m_fp[name]);
  }

  return nullptr;
}

void Device::deleteFocusPalette(string name)
{
  if (m_fp.count(name) > 0)
    m_fp.erase(name);
}

void Device::setFocusPalette(string name)
{
  // check palette exists
  if (m_fp.count(name) > 0) {
    FocusPalette fp = m_fp[name];
    // check for pan and tilt params, must be exactly named that
    if (paramExists("pan") && paramExists("tilt")) {
      // set pan and tilt
      getParam<LumiverseOrientation>("pan")->setValAsPercent(fp._pan);
      getParam<LumiverseOrientation>("tilt")->setValAsPercent(fp._tilt);

      // update metadata, if provided
      if (fp._area != "") {
        setMetadata("area", fp._area);
      }

      if (fp._system != "") {
        setMetadata("system", fp._system);
      }

      setMetadata("lastFocusPalette", fp._name);
    }
  }
}

vector<string> Device::getFocusPaletteNames()
{
  vector<string> names;
  for (auto& fp : m_fp) {
    names.push_back(fp.first);
  }

  return names;
}

FocusPalette * Device::closestPalette()
{
  if (m_fp.size() == 0)
    return nullptr;

  if (paramExists("pan") && paramExists("tilt")) {
    float minDist = FLT_MAX;
    FocusPalette* best = nullptr;
    float pan = getParam<LumiverseOrientation>("pan")->asPercent();
    float tilt = getParam<LumiverseOrientation>("tilt")->asPercent();

    // check all distances, euclidean
    for (auto& fp : m_fp) {
      float dist = sqrt(pow(fp.second._pan - pan, 2) + pow(fp.second._tilt - tilt, 2));

      if (dist < minDist) {
        minDist = dist;
        best = &(m_fp[fp.first]);
      }

      // if we ever have an exact match, break immediately
      if (minDist == 0)
        break;
    }

    return best;
  }
  else {
    return nullptr;
  }

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
    else if (nodeName == "focusPalettes") {
      JSONNode fp = *i;
      
      auto fpStart = fp.begin();
      while (fpStart != fp.end()) {
        JSONNode palette = *fpStart;
        addFocusPalette(FocusPalette(palette["name"].as_string(), palette["pan"].as_float(), palette["tilt"].as_float(),
          palette["area"].as_string(), palette["system"].as_string(), palette["image"].as_string()));

        ++fpStart;
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