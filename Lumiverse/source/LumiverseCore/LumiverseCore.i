%module lumiversepython

%include "typemaps.i"
%include "stl.i"
%include "std_string.i"
%include "std_vector.i"
using namespace std;

%{
#include "LumiverseCore.h"
%}

%template(UCharVector) vector<unsigned char>;
%template(StringVector) vector<string>;

%apply const std::string& {std::string* m_id};
%apply const std::string& {std::string* m_type};

class Rig
{
  friend class DeviceSet;

public:
  Rig();
  Rig(string filename);
  ~Rig();

  void init();
  void run();
  void stop();
  void addDevice(Device * device);
  Device* getDevice(string id);
  void deleteDevice(string id);
  void addPatch(string id, Patch* patch);
  Patch* getPatch(string id);
  DMXPatch* getPatchAsDMXPatch(string id);
  void deletePatch(string id);
  void setRefreshRate(unsigned int rate);
  unsigned int getRefreshRate() { return m_refreshRate; }
  Device* operator[](string id);
  DeviceSet query(string q);
  DeviceSet operator[](unsigned int channel);
  DeviceSet getAllDevices();
  DeviceSet getChannel(unsigned int channel);
  DeviceSet getChannel(unsigned int lower, unsigned int upper);
  DeviceSet getDevices(string key, string val, bool isEqual);
  const set<Device *>* getDeviceRaw() { return &m_devices; }
};

class DeviceSet
{
public:
  DeviceSet() { };
  DeviceSet(Rig* rig);
  DeviceSet(Rig* rig, set<Device *> devices);
  DeviceSet(const DeviceSet& dc);
  ~DeviceSet();

  std::ostream & operator<< (std::ostream &str) {
    str << info();
    return str;
  };

  DeviceSet select(string selector);
  DeviceSet add(Device* device);
  DeviceSet add(string id);
  DeviceSet add(unsigned int channel);
  DeviceSet add(unsigned int lower, unsigned int upper);
  DeviceSet add(string key, string val, bool isEqual);
  DeviceSet add(string key, regex val, bool isEqual);
  DeviceSet add(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual);
  DeviceSet remove(Device* device);
  DeviceSet remove(string id);
  DeviceSet remove(unsigned int channel);
  DeviceSet remove(unsigned int lower, unsigned int upper);
  DeviceSet remove(string key, string val, bool isEqual);
  DeviceSet remove(string key, regex val, bool isEqual);
  DeviceSet remove(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual);
  void setParam(string param, float val);
  inline const set<Device *>* getDevices() { return &m_workingSet; }
  string info();
  inline size_t size() { return m_workingSet.size(); }
};

class Device
{
public:
  Device(string id, unsigned int channel, string type);
  Device(string id, const JSONNode data);
  ~Device();

  std::ostream & operator<< (std::ostream &str) {
    str << toString();
    return str;
  };

  inline string getId() { return m_id; }
  inline unsigned int getChannel() { return m_channel; }
  inline void setChannel(unsigned int newChan) { m_channel = newChan; }
  inline string getType() { return m_type; }
  inline void setType(string newType) { m_type = newType; }
  bool getParam(string param, float& val);
  LumiverseType* getParam(string param);
  bool setParam(string param, LumiverseType* val);
  bool setParam(string param, float val);
  bool paramExists(string param);
  void clearParamValues();
  unsigned int numParams();
  vector<string> getParamNames();
  bool getMetadata(string key, string& val);
  bool setMetadata(string key, string val);
  void clearMetadataValues();
  void clearAllMetadata();
  unsigned int numMetadataKeys();
  vector<string> getMetadataKeyNames();
  string toString();
  JSONNode toJSON();
  const map<string, LumiverseType*>* getRawParameters() { return &m_parameters; }
};

class Patch
{
public:
  virtual ~Patch() { };
  virtual void update(set<Device *> devices) = 0;
  virtual void init() = 0;
  virtual void close() = 0;
};

class DMXPatch : public Patch
{
public:
  DMXPatch();
  DMXPatch(const JSONNode data);
  virtual ~DMXPatch();
  virtual void update(set<Device *> devices);
  virtual void init();
  virtual void close();
  void assignInterface(DMXInterface* iface, unsigned int universe);
  void deleteInterface(string id);
  void moveInterface(string id, unsigned int universeFrom, unsigned int universeTo);
  const multimap<string, unsigned int> getInterfaceInfo() { return m_ifacePatch; }
  void patchDevice(Device* device, DMXDevicePatch* patch);
  void patchDevice(string id, DMXDevicePatch* patch);
  void addDeviceMap(string id, map<string, patchData> deviceMap);
  void addParameter(string mapId, string paramId, unsigned int address, conversionType type);
  void dumpUniverses();
  void dumpUniverse(unsigned int universe);
  bool setRawData(unsigned int universe, vector<unsigned char> univData);
};