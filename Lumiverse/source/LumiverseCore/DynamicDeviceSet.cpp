#include "DynamicDeviceSet.h"
namespace Lumiverse {

DynamicDeviceSet::DynamicDeviceSet(Rig* rig, string query) : m_rig(rig), m_query(query) {
  // look it's empty
}

DynamicDeviceSet::DynamicDeviceSet(Rig* rig, JSONNode data) : m_rig(rig) {
  m_query = data.as_string();
}

DynamicDeviceSet::DynamicDeviceSet(const DynamicDeviceSet& dc) {
  m_rig = dc.m_rig;
  m_query = dc.m_query;
}

DynamicDeviceSet::~DynamicDeviceSet() {
  // Nothing for now
}

DeviceSet DynamicDeviceSet::getDeviceSet() {
  return m_rig->select(m_query);
}

void DynamicDeviceSet::reset() {
  getDeviceSet().reset();
}

void DynamicDeviceSet::setParam(string param, float val) {
  getDeviceSet().setParam(param, val);
}

void DynamicDeviceSet::setParam(string param, string val, float val2) {
  getDeviceSet().setParam(param, val, val2);
}

void DynamicDeviceSet::setParam(string param, string val, float val2, LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode) {
  getDeviceSet().setParam(param, val, val2, mode, interpMode);
}

void DynamicDeviceSet::setParam(string param, string channel, double val) {
  getDeviceSet().setParam(param, channel, val);
}

void DynamicDeviceSet::setParam(string param, double x, double y, double weight) {
  getDeviceSet().setParam(param, x, y, weight);
}

void DynamicDeviceSet::setColorRGBRaw(string param, double r, double g, double b, double weight) {
  getDeviceSet().setColorRGBRaw(param, r, g, b, weight);
}

void DynamicDeviceSet::setColorRGB(string param, double r, double g, double b, double weight, RGBColorSpace cs) {
  getDeviceSet().setColorRGB(param, r, g, b, weight, cs);
}

vector<string> DynamicDeviceSet::getIds() {
  return getDeviceSet().getIds();
}

set<string> DynamicDeviceSet::getAllParams() {
  return getDeviceSet().getAllParams();
}

set<string> DynamicDeviceSet::getAllMetadata() {
  return getDeviceSet().getAllMetadata();
}

set<string> DynamicDeviceSet::getAllMetadataForKey(string key) {
  return getDeviceSet().getAllMetadataForKey(key);
}

string DynamicDeviceSet::info() {
  stringstream ss;

  ss << "Device set contains " << size() << " devices.\n";
  ss << "Query string: " << m_query << "\n";
  ss << "IDs: ";

  bool first = true;
  for (auto& id : getDeviceSet().getIds()) {
    ss << ((first) ? "" : ", ") << id;
    first = false;
  }

  return ss.str();
}

bool DynamicDeviceSet::hasSameIds(DynamicDeviceSet& devices) {
  DeviceSet other = devices.getDeviceSet();
  return getDeviceSet().hasSameIds(other);
}

bool DynamicDeviceSet::hasSameDevices(DynamicDeviceSet& devices) {
  DeviceSet other = devices.getDeviceSet();
  return getDeviceSet().hasSameDevices(other);
}

bool DynamicDeviceSet::contains(Device* d) {
  return getDeviceSet().contains(d);
}

bool DynamicDeviceSet::contains(string id) {
  return getDeviceSet().contains(id);
}

JSONNode DynamicDeviceSet::toJSON(string name) {
  JSONNode str;
  str.set_name(name);
  str = m_query;

  return str;
}

void DynamicDeviceSet::setQuery(string query) {
  m_query = query;
}

string DynamicDeviceSet::getQuery() {
  return m_query;
}

bool DynamicDeviceSet::isQueryNull() {
  return m_query == "";
}
}