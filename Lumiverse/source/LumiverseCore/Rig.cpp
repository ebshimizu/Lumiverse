#include "Rig.h"

namespace Lumiverse {

map<string, patchParseFunc> Rig::patchParsers = {};

Rig::Rig() {
  m_running = false;
  setRefreshRate(40);
  m_updateLoop = nullptr;
}

Rig::Rig(string filename) {
  m_running = false;
  setRefreshRate(40);
  m_updateLoop = nullptr;

  if (!load(filename)) {
    Logger::log(WARN, "Proceeding with default rig initialization");
  }
}

set<string> Rig::getMetadataValues(string key)
{
  set<string> vals;
  for (const auto& d : m_devices) {
    string v;
    if (d->getMetadata(key, v)) {
      vals.insert(v);
    }
  }

  return vals;
}

void Rig::loadJSON(JSONNode root) {
  //JSONNode::const_iterator i = root.begin();
  JSONNode::iterator i = root.begin();

  auto version = root.find("version");
  if (version == root.end()) {
    Logger::log(ERR, "No version specified for input file. Aborting load.");
    return;
  }
  else {
    stringstream ss;
    stringstream ss2(version->as_string());

    ss << LumiverseCore_VERSION_MAJOR << "." << LumiverseCore_VERSION_MINOR;
    
    float libVer;
    float fileVer;

    ss >> libVer;
    ss2 >> fileVer;

    if (fileVer < libVer) {
      // Friendly warning if you're loading an old file.
      Logger::log(WARN, "File created against earlier version of Lumiverse. Check logs for any load problems.");
    }
    else if (fileVer > libVer) {
      // Loading newer file with older library.
      Logger::log(WARN, "File created against newer version of Lumiverse. Check logs for any load problems.");
    }
  }

  while (i != root.end()){
    // get the node name and value as a string
    std::string nodeName = i->name();

    if (nodeName == "devices") {
      loadDevices(*i);
      Logger::log(INFO, "Device load complete");
    }
    else if (nodeName == "patches") {
	  i->push_back(*root.find("jsonPath"));
      loadPatches(*i);
      Logger::log(INFO, "Patch load complete");
    }
    else if (nodeName == "refreshRate") {
      setRefreshRate(i->as_int());
    }

    //increment the iterator
    ++i;
  }
}

void Rig::loadDevices(JSONNode root) {
  JSONNode::const_iterator i = root.begin();

  // for this we want to iterate through all children and have the device class
  // parse the sub-element.
  while (i != root.end()){
    // get the node name and value as a string
    std::string nodeName = i->name();

    // Node name is the Device id
    Device* device = new Device(nodeName, *i);
    addDevice(device);

    //increment the iterator
    ++i;
  }
}

void Rig::loadPatches(JSONNode root) {
  JSONNode::iterator i = root.begin();

  // for this we want to iterate through all children and have the device class
  // parse the sub-element.
  while (i != root.end()){
	if (i->name() == "jsonPath") {
		i++;
		continue;
	}

    // get the node name and value as a string
    std::string nodeName = i->name();

    stringstream ss;
    ss << "Loading patch " << nodeName;
    Logger::log(INFO, ss.str());

    Patch* patch;
    
    auto type = i->find("type");
    if (type == i->end()) {
      stringstream ss;
      ss << "Unable to determine Patch type for " << nodeName << ". Patch not loaded.";
      Logger::log(WARN, ss.str());
    }

    string patchType = type->as_string();    

    // New patch types will need new seralization definitions.
    if (patchType == "DMXPatch") {
      patch = (Patch*) new DMXPatch(*i);
      addPatch(nodeName, patch);
    }
#ifdef USE_OSC
    if (patchType == "osc") {
      patch = (Patch*) new OscPatch(*i);
      addPatch(nodeName, patch);
    }
#endif
#ifdef USE_ARNOLD
	else if (patchType == "PhotoPatch") {
		patch = (Patch*) new PhotoPatch(*i);
		addPatch(nodeName, patch);

		Device::DeviceCallbackFunction callback = std::bind(&PhotoPatch::onDeviceChanged,
			(PhotoPatch*)patch,
			std::placeholders::_1);
		for (Device *d : getDeviceRaw()) {
			d->addParameterChangedCallback(callback);
			d->addMetadataChangedCallback(callback);
		}
	}
	else if (patchType == "PhotoAnimationPatch") {
		patch = (Patch*) new PhotoAnimationPatch(*i);
		addPatch(nodeName, patch);

		Device::DeviceCallbackFunction callback = std::bind(&PhotoAnimationPatch::onDeviceChanged,
			(PhotoAnimationPatch*)patch,
			std::placeholders::_1);
		for (Device *d : getDeviceRaw()) {
			d->addParameterChangedCallback(callback);
			d->addMetadataChangedCallback(callback);
		}
	}
#endif
#if defined(USE_ARNOLD) || defined(USE_ARNOLD_CACHING)
    else if (patchType == "ArnoldAnimationPatch") {
      i->push_back(*root.find("jsonPath"));
      ArnoldAnimationPatch* aap = new ArnoldAnimationPatch(*i);
      patch = (Patch*) aap;
      addPatch(nodeName, patch);
      Device::DeviceCallbackFunction callback = std::bind(&ArnoldAnimationPatch::onDeviceChanged,
                                                            (ArnoldAnimationPatch*)patch,
                                                            std::placeholders::_1);
      for (Device *d : getDeviceRaw()) {
          d->addParameterChangedCallback(callback);
          d->addMetadataChangedCallback(callback);
      }

      // we assume here the devices are loaded and will also load the
      // cached files if applicable.
      if (aap->getArnoldInterface()->isUsingCaching()) {
        aap->getArnoldInterface()->loadIfUsingCaching(getDeviceRaw());
      }
    }
    else if (patchType == "ArnoldPatch") {
	  i->push_back(*i->find("jsonPath"));
      patch = (Patch*) new ArnoldPatch(*i);
      addPatch(nodeName, patch);
        
        Device::DeviceCallbackFunction callback = std::bind(&ArnoldPatch::onDeviceChanged,
                                                            (ArnoldPatch*)patch,
                                                            std::placeholders::_1);
        for (Device *d : getDeviceRaw()) {
            d->addParameterChangedCallback(callback);
            d->addMetadataChangedCallback(callback);
        }
    }
#endif
    else {
      // Check custom parsers
      try {
        patch = patchParsers.at(patchType)(*i);
        addPatch(nodeName, patch);
      }
      catch (exception e) {
        stringstream ss;
        ss << "Unknown Patch type " << patchType << " in Patch ID " << nodeName << "Patch not loaded.";
        Logger::log(WARN, ss.str());
      }
    }

    //increment the iterator
    ++i;
  }
}

void Rig::reset() {
  stop();

  // Delete Devices
  for (auto& d : m_devices) {
    delete d;
  }

  // Delete Patches
  for (auto& p : m_patches) {
    delete p.second;
  }

  m_devices.clear();
  m_patches.clear();
  m_devicesById.clear();
  m_devicesByChannel.clear();
  m_updateFunctions.clear();
}

Rig::~Rig() {
  // Stop the update thread.
  stop();

  if (m_updateLoop != nullptr)
    delete m_updateLoop;
  
  // Delete Devices
  for (auto& d : m_devices) {
    delete d;
  }

  // Delete Patches
  for (auto& p : m_patches) {
    delete p.second;
  }
}

void Rig::init() {
  for (auto& p : m_patches) {
    p.second->init();
  }
}

void Rig::run() {
  m_running = true;
  m_updateLoop = new thread(&Rig::update, this);
}

void Rig::stop() {
  if (m_running) {
    m_running = false;
    m_updateLoop->join();
  }
}

bool Rig::load(string filename) {
  stop();

  // Check to see if we can load the file.
  ifstream data;
  data.open(filename, ios::in | ios::binary | ios::ate);

  if (data.is_open()) {
    reset();

    // "+ 1" for the ending
    streamoff size = data.tellg();
    char* memblock = new char[(unsigned int)size + 1];

    data.seekg(0, ios::beg);

    stringstream ss;
    ss << "Loading " << size << " bytes from " << filename;
    Logger::log(INFO, ss.str());

    data.read(memblock, size);
    data.close();

    // It's not guaranteed that the following memory after memblock is blank.
    // C-style string needs an end.
    memblock[size] = '\0';
      
    JSONNode n = libjson::parse(memblock);

	// Pass in json path with the original json nodes.
	n.push_back(JSONNode("jsonPath", filename));

    // This could get to be a large function, so let's break off into a helper.
    loadJSON(n);

    delete memblock;
    return true;
  }
  else {
    stringstream ss;
    ss << "Error opening " << filename;
    Logger::log(ERR, ss.str());

    return false;
  }
}

void Rig::addDevice(Device* device) {
  if (m_running) {
    Logger::log(ERR, "Can't add Devices to the Rig while the Rig is running.");
    return;
  }

  // Don't add duplicates.
  if (m_devicesById.count(device->getId()) > 0)
  {
    stringstream ss;
    ss << "Failed to add device with ID " << device->getId() << " to rig because ID already exists";
    Logger::log(ERR, ss.str());
    return;
  }

  m_devices.insert(device);
  m_devicesById[device->getId()] = device;
  m_devicesByChannel.insert(make_pair(device->getChannel(), device));
}

Device* Rig::getDevice(string id) {
  // If it doesn't exist, just return a null pointer.
  if (m_devicesById.count(id) == 0) {
    return nullptr;
  }
  return m_devicesById[id];
}

void Rig::deleteDevice(string id) {
  if (m_running) {
    Logger::log(ERR, "Can't delete devices while Rig is running.");
    return;
  }

  if (m_devicesById.count(id) == 0)
    return;

  // Find the device pointer so we can delete it in the other indexes
  Device * toDelete = m_devicesById[id];
  auto range = m_devicesByChannel.equal_range(toDelete->getChannel());
  for (auto it = range.first; it != range.second; it++) {
    if (it->second->getId() == toDelete->getId()) {
      m_devicesByChannel.erase(it);
      break;
    }
  }

  // Find the device in the vector and delete it. Yay vectors.
  m_devices.erase(find(m_devices.begin(), m_devices.end(), m_devicesById[id]));

  // delete the Device from the patches
  for (const auto& p : m_patches) {
    p.second->deleteDevice(id);
  }

  // Delete the memory used by the device using the id->device map
  delete m_devicesById[id];
  m_devicesById.erase(id);
}

void Rig::addPatch(string id, Patch* patch) {
  if (m_running) {
    Logger::log(ERR, "Can't add patches while Rig is running.");
    return;
  }

  // No duplicates.
  if (m_patches.count(id) > 0)
    return;

  m_patches[id] = patch;
}

Patch* Rig::getPatch(string id) {
  return (m_patches.count(id) > 0) ? m_patches[id] : nullptr;
}

void Rig::deletePatch(string id) {
  if (m_running) {
    Logger::log(ERR, "Can't remove patches while Rig is running.");
    return;
  }

  if (m_patches.count(id) == 0)
    return;

  // Free up space and erase the patch from the map
  delete m_patches[id];
  m_patches.erase(id);
}

void Rig::setRefreshRate(unsigned int rate) {
  m_refreshRate = rate;
  m_loopTime = 1.0f / (float)m_refreshRate;
}

void Rig::update() {
  while (m_running) {
    // Get start time
    auto start = chrono::high_resolution_clock::now();

    updateOnce();

    // Sleep a bit depending on how long the update took.
    auto end = chrono::high_resolution_clock::now();
    float elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count() / 1000.0f;

    if (elapsed < m_loopTime) {
      m_slow = false;
      unsigned int ms = (unsigned int)(1000 * (m_loopTime - elapsed));
      this_thread::sleep_for(chrono::milliseconds(ms));
    }
    else {
      m_slow = true;
      // Logger::log(WARN, "Rig Update loop running slowly");
    }
  }
}

void Rig::updateOnce() {
  // Run additional functions before sending to patches
  // These functions can be update functions you run in your own code
  // or other things that need to be in sync with stuff going over the network.
  for (auto& f : m_updateFunctions) {
    f.second();
  }

  // Run the whole update thing for all patches
  for (auto& p : m_patches) {
    p.second->update(m_devices);
  }
}

void Rig::setAllDevices(map<string, Device*> devices) {
  for (auto& kvp : devices) {
    try {
      auto d = m_devicesById.at(kvp.first);
      auto params = kvp.second->getRawParameters();
      for (auto& param : params) {
        // We want to copy instead of assign since we don't know where that LumiverseType data
        // is going to end up. Maybe it'd be better if devices did a copy instead...
        //LumiverseTypeUtils::copyByVal(param.second, m_devicesById[kvp.first]->getParam(param.first));
        d->copyParamByValue(param.first, param.second);
      }
    }
    catch (exception e) {
      stringstream ss;
      ss << "Rig does not contain a device with id: " << kvp.first;
      Logger::log(WARN, ss.str());
    }
  }
}

Device* Rig::operator[](string id) {
  return getDevice(id);
}

DeviceSet Rig::select(string q) {
  DeviceSet working(this);
  return working.select(q);
}

DeviceSet Rig::operator[](unsigned int channel) {
  return getChannel(channel);
}

DeviceSet Rig::getAllDevices() {
  return DeviceSet(this, m_devices);
}

DeviceSet Rig::getChannel(unsigned int channel) {
  DeviceSet working(this);
  return working.add(channel);
}

DeviceSet Rig::getChannel(unsigned int lower, unsigned int upper) {
  DeviceSet working(this);
  return working.add(lower, upper);
}

DeviceSet Rig::getDevices(string key, string val, bool isEqual) {
  DeviceSet working(this);
  return working.add(key, val, isEqual);
}

set<string> Rig::getAllUsedParams() {
  set<string> params;
  
  for (auto& d : m_devices) {
    for (auto& s : d->getParamNames()) {
      params.insert(s);
    }
  }

  return params;
}

bool Rig::save(string filename, bool overwrite) {
  // Test if the file already exists.
  ifstream ifile(filename);
  if (ifile.is_open() && !overwrite) {
    return false;
  }
  ifile.close();
  
  ofstream rigFile;
  rigFile.open(filename, ios::out | ios::trunc);
  rigFile << toJSON().write_formatted();

  return true;
}

JSONNode Rig::toJSON() {
  JSONNode root;

  stringstream ss;
  ss << LumiverseCore_VERSION_MAJOR << "." << LumiverseCore_VERSION_MINOR;

  root.push_back(JSONNode("version", ss.str()));
  root.push_back(JSONNode("refreshRate", m_refreshRate));

  JSONNode devices;
  devices.set_name("devices");
  for (auto d : m_devices) {
    devices.push_back(d->toJSON());
  }
  root.push_back(devices);

  JSONNode patches;
  patches.set_name("patches");
  for (auto& p : m_patches) {
    JSONNode patch = p.second->toJSON();
    patch.set_name(p.first);
    patches.push_back(patch);
  }
  root.push_back(patches);

  return root;
}
    
Patch* Rig::getSimulationPatch(string type) {
    for (pair<string, Patch*> patch : m_patches) {
        if ((patch.second->getType() == "ArnoldPatch" ||
            patch.second->getType() == "ArnoldAnimationPatch" ||
			patch.second->getType() == "PhotoPatch" ||
			patch.second->getType() == "PhotoAnimationPatch") &&
            patch.second->getType() == type) {
            return patch.second;
        }
    }
        
    return NULL;
}

bool Rig::addFunction(int pid, function<void()> func) {
  // If the rig wasn't running, leave it that way.
  bool restart = false;
  bool success = false;

  if (m_running) {
    stop();
    restart = true;
  }

  if (m_updateFunctions.count(pid) == 0) {
    m_updateFunctions[pid] = func;
    success = true;

    stringstream ss;
    ss << "Adding additional function to update loop with pid " << pid;
    Logger::log(INFO, ss.str());
  }
  else {
    stringstream ss;
    ss << "Function with pid " << pid << " already exists in update loop. Cannot add new function";
    Logger::log(ERR, ss.str());
  }

  if (restart)
    run();

  return success;
}

bool Rig::removeFunction(int pid) {
  bool restart = false;
  bool ret = false;

  if (m_running) {
    stop();
    restart = true;
  }

  if (m_updateFunctions.count(pid) > 0) {
    m_updateFunctions.erase(pid);

    stringstream ss;
    ss << "Removed additional function from update loop with pid " << pid;
    Logger::log(INFO, ss.str());

    ret = true;
  }
  else {
    stringstream ss;
    ss << "Failed to remove additional function from update loop with pid " << pid << " (function does not exist)";
    Logger::log(WARN, ss.str());
  }
  
  if (restart)
    run();

  return ret;
}

void Rig::resetDevices() {
  for (const auto& d : m_devices) {
    d->reset();
  }
}

}