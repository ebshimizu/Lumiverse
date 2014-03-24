%module lumiversepython

%include "typemaps.i"
%include "stl.i"
%include "std_string.i"
using namespace std;

%{
#include "LumiverseCore.h"
%}

class Rig
{
  friend class DeviceSet;

public:
  // Makes an empty rig
  Rig();

  // Initialize Rig from JSON file
  Rig(string filename);

  // Destroys and frees all objects in the rig. Once this is called
  // all resources used by this class are free.
  ~Rig();

  // Initializes the rig. Called after all patching has been done
  // and configuration settings have been selected.
  void init();

  // Runs the update loop that sends updates to the network.
  void run();

  // Stops the update loop in preparation for shutting down the network
  // or potential reconfiguration.
  void stop();

  // Adds a device to the rig.
  void addDevice(Device * device);

  // Gets a device from the rig. Return value can be modified to change
  // the state of the device in the rig.
  // If the device doesn't exist in the rig, a nullptr will be returned.
  Device* getDevice(string id);

  // Removes the device with specified id from the rig. Also deletes it.
  void deleteDevice(string id);

  // Adds a patch to the rig. Expects the patch to be configured before
  // sending it to the rig. Not that it can't be edited later, but
  // the rig only knows what you tell it.
  void addPatch(string id, Patch* patch);

  // Gets a patch from the rig. Can be modified.
  // Returns a nullptr if patch with specified ID doesn't exist.
  Patch* getPatch(string id);

  // Deletes an entire patch from the rig.
  void deletePatch(string id);

  // Sets the refresh rate for the update loop in cycles / second
  void setRefreshRate(unsigned int rate);

  // Gets the refresh rate for the update loop.
  unsigned int getRefreshRate() { return m_refreshRate; }

  // Shorthand for getDevice(string)
  Device* operator[](string id);

  // This will actually probably replace operator[] at some point, but that point is not now.
  DeviceSet query(string q);

  // shorthand for getChannel(unsigned int channel);
  DeviceSet operator[](unsigned int channel);

  // Queries. Most everything starts with the creation of a DeviceSet.
  // Detailed filtering happens there, the Rig provides a few convenience functions
  // to get things started.

  // Returns a set consisting of all devices in the rig
  DeviceSet getAllDevices();

  // Gets all the devices in a channel
  DeviceSet getChannel(unsigned int channel);

  // Gets a range of channels
  DeviceSet getChannel(unsigned int lower, unsigned int upper);

  // Gets devices by metadata info. isEqual determines if the set consists
  // of all devices that have the same value as val or not.
  DeviceSet getDevices(string key, string val, bool isEqual);

  // Gets the raw list of devices.
  // Users are not allowed to modify devices through this direct method,
  // but may read the data.
  const set<Device *>* getDeviceRaw() { return &m_devices; }
private:
  // Loads the rig info from the parsed JSON data.
  void loadJSON(JSONNode root);

  // Loads the devices in the JSON file.
  void loadDevices(JSONNode root);

  // Load patches in the JSON file.
  void loadPatches(JSONNode root);

  // Actually updates the network and stuff.
  void update();

  // Thread that runs the update loop.
  thread* m_updateLoop;

  // Indicates the status of the update loop. True if running.
  bool m_running;

  // Sets the speed of the run loop. Default is 40 (DMX standard).
  unsigned int m_refreshRate;

  // Amount of time an update loop can take in s.
  float m_loopTime;

  // Raw list of devices for sending to the Patch->update function.
  set<Device *> m_devices;

  // Patches mapped by an identifier chosen by the user.
  map<string, Patch *> m_patches;

  // Devices mapped by their device ID.
  map<string, Device *> m_devicesById;

  // Devices mapped by channel number.
  multimap<unsigned int, Device *> m_devicesByChannel;

  // May have more indicies in the future, like mapping by channel number.
};

class DeviceSet
{
public:
  // Constructs a DeviceSet unassociated with a particular Rig
  DeviceSet() { };

  // Constructs an empty set
  DeviceSet(Rig* rig);

  // Initializes the set with the given devices.
  DeviceSet(Rig* rig, set<Device *> devices);

  // Copy Constructor
  DeviceSet(const DeviceSet& dc);

  // Destructor woo
  ~DeviceSet();

  // string override
  std::ostream & operator<< (std::ostream &str) {
    str << info();
    return str;
  };

  // Main function to select devices from a rig. Follows specific syntax rules. See
  // the implementation for syntax details.
  DeviceSet select(string selector);

private:
  // Grouped here for convenience
  // Parses a single selector. Redirects to appropriate functions.
  // Boolean flag determines if selected fixtures are added or subtracted from
  // the current DeviceSet
  DeviceSet parseSelector(string selector, bool filter);

  // Processes a metadata selector-
  DeviceSet parseMetadataSelector(string selector, bool filter);

  // Processes a channel selector
  DeviceSet parseChannelSelector(string selector, bool filter);

  // Processes a parameter selector
  DeviceSet parseParameterSelector(string selector, bool filter);

  // Processes a float parameter selector
  DeviceSet parseFloatParameter(string param, string op, float val, bool filter, bool eq);

public:
  // Adds a device to the set. Overloads for other common additions.
  DeviceSet add(Device* device);
  DeviceSet add(string id);
  DeviceSet add(unsigned int channel);

  // Adds a group of devices based on inclusive channel range
  DeviceSet add(unsigned int lower, unsigned int upper);

  // Adds devices based on metadata. isEqual determines if a device should be
  // added if the values are equal/not equal
  DeviceSet add(string key, string val, bool isEqual);

  // And the regex version of add by popular demand (meaning that I wanted it)
  // Like the other add, isEqual determines if a device is added on a match or a non-match
  DeviceSet add(string key, regex val, bool isEqual);

  // Adds devices based on a parameter comparison function provided by the caller.
  DeviceSet add(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual);

  // Removes a device from the set. Overloads for other common removals.
  DeviceSet remove(Device* device);
  DeviceSet remove(string id);
  DeviceSet remove(unsigned int channel);

  // Removes a group of devices based on inclusive channel range
  DeviceSet remove(unsigned int lower, unsigned int upper);

  // Filters out devices matching/not matching a particular metadata value.
  DeviceSet remove(string key, string val, bool isEqual);

  // Filters out devices matching/not matching a particular metadata value specified by a regex.
  DeviceSet remove(string key, regex val, bool isEqual);

  // Removes a device based on a parameter comparison function provided by the caller
  DeviceSet remove(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual);

  // These must mirror the device setparam functions.
  // This sets the value of a parameter on every device in the group
  // if the parameter already exists in the device (will not add params,
  // just modify).
  void setParam(string param, float val);

  // Gets the devices managed by this set.
  inline const set<Device *>* getDevices() { return &m_workingSet; }

  // Returns a string containing info about the DeviceSet.
  // the heavy lifter of the toString override
  string info();

  // Returns the number of devices in the selected set.
  inline size_t size() { return m_workingSet.size(); }

private:
  // Adds to the set without returning a new copy.
  // Internal use only.
  void addDevice(Device* device);

  // Removes from the set without returning a new copy.
  // Internal use only.
  void removeDevice(Device* device);

  // Internal set oprations
  // Equivalent of a union.
  void addSet(DeviceSet otherSet);

  // Set of devices we're currently working with.
  set<Device *> m_workingSet;

  // Reference to the rig for accessing indexes and devices
  Rig* m_rig;
};

%apply const std::string& {std::string* m_id};
%apply const std::string& {std::string* m_type};

class Device
{
public:
  // Default constructor. Every device needs an id, channel, and type.
  // May in the future pull default parameter map from a database of known
  // fixture types.
  Device(string id, unsigned int channel, string type);

  // Make a device given a JSON node
  Device(string id, const JSONNode data);

  // Destructor. Obviously.
  ~Device();

  // string override
  std::ostream & operator<< (std::ostream &str) {
    str << toString();
    return str;
  };

  // Accessors for id
  inline string getId() { return m_id; }

  // Accessors for channel
  inline unsigned int getChannel() { return m_channel; }
  inline void setChannel(unsigned int newChan) { m_channel = newChan; }

  // Accesors for type
  inline string getType() { return m_type; }
  inline void setType(string newType) { m_type = newType; }

  // Gets a parameter value. Returns false if no parameter with the given name exists.
  // Returns true with the parameter value in val if successful.
  bool getParam(string param, float& val);

  // Returns a pointer to the raw LumiverseType data associated with a parameter
  LumiverseType* getParam(string param);
  
  // Sets a parameter and returns true if parameter changed does not exist prior to set.
  // Can give arbitrary data with this overload.
  bool setParam(string param, LumiverseType* val);

  // Sets a parameter and returns true if parameter changed does not exist prior to set.
  // Returns false otherwise.
  bool setParam(string param, float val);

  // Will need additional overloads for each new type. Which kinda sucks.

  // Simply returns true if the parameter exists.
  bool paramExists(string param);

  // Resets the values in the parameters to 0 (or equivalent default)
  void clearParamValues();

  // Returns number of parameters in the device.
  unsigned int numParams();

  // Returns list of parameters in the device.
  vector<string> getParamNames();

  // Gets the metadata for a given key. Returns false if no key exists.
  // Returns the value in val otherwise.
  bool getMetadata(string key, string& val);

  // Sets metadata and returns true if metadata key does not exist prior to set.
  // Returns false otherwise.
  bool setMetadata(string key, string val);

  // Resets metadata values to "" but leaves the keys intact.
  void clearMetadataValues();

  // Empties everything in the metadata hash: keys and values. All gone.
  void clearAllMetadata();

  // Returns the number of metadata keys.
  unsigned int numMetadataKeys();

  // Gets list of metadata keys the device has values for.
  vector<string> getMetadataKeyNames();

  // Converts the device into a string.
  // Data will be output in JSON format
  string toString();

  // Converts the device to a JSONNode.
  JSONNode toJSON();

  // Gets the raw map of parameters to type
  // User shouldn't modify this map directly.
  const map<string, LumiverseType*>* getRawParameters() { return &m_parameters; }
private:
  // Note that this is private because changing the unique ID after creation
  // can lead to a number of unintended side effects (DMX patch doesn't work,
  // Rig can't find the device, other indexes may fail). This is a little annoying,
  // but in the big scheme of devices and rigs, the ID shouldn't change after initial
  // creation. If the user needs a different display name, the metadata fields say hi.
  // In ACN terms, this would be the CID (if I remember this right), which is unique and
  // immutable for each device).
  void setId(string newId) { m_id = newId; }

  // Takes parsed JSON data and makes a device.
  void loadJSON(const JSONNode data);

  // Loads the parameters of the device from JSON data.
  void loadParams(const JSONNode data);

  // Serializes the parameters into a JSON node
  JSONNode parametersToJSON();

  // Serializes the metadata to a JSON node
  JSONNode metadataToJSON();

  // Unique identifier for the device.
  // Note that while you can use any characters you want in this, you really shouldn't
  // use special characters such as @#$%^=()[]/{} etc.
  // TODO: This should be built in to the set ID function at some point
  // Uniqueness isn't quite enforceable at the device level.
  string m_id;

  // Channel number for the fixture. Does not have to be unique.
  unsigned int m_channel;

  // Device type name. "Source Four ERS" for example.
  string m_type;

  // Map for time-varying parameters.
  // Type may change in the future as more specialized datatypes come up.
  map<string, LumiverseType*> m_parameters;

  // Map for static information.
  // User-defined data helps to add search filters and query devices.
  map<string, string> m_metadata;
};