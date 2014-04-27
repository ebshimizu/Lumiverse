#ifndef _DEVICESET_H_
#define _DEVICESET_H_

#pragma once

#include <sstream>
#include <set>
#include <regex>
#include <functional>

#include "Logger.h"
#include "Device.h"
#include "Rig.h"

namespace Lumiverse {
  class Rig;
  class LumiverseType;

  // A DeviceSet is a set of devices. More specifically, the set resulting
  // from a particular query or series of filtering operations.
  // These devices can be manupulated by setting properties as a group,
  // further filtering them, adding devices, etc.
  // DeviceSets are returned by the Rig when asking for more than one device.
  // Device sets can be filtered in chains, as each filtering operation
  // will return a new DeviceSet:
  // rig.getDevices("angle", "back", true).remove("area", "3", true) ... etc.
  // Returning a new set after each operation may use more memory, but
  // it does allow for the construction of a query history and saving of
  // that query during any point of its construction. This history is currently
  // not saved, but may in the future be part of this class.
  // Alternately, DeviceSets can be constructed from concise queries: 
  // https://bitbucket.org/falindrith/OpenLL/wiki/Query%20Syntax%20Notes
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

    // Adds devices based on a query string
    DeviceSet add(string query);

    // Removes a device from the set. Overloads for other common removals.
    DeviceSet remove(Device* device);
    DeviceSet remove(unsigned int channel);

    // Removes a group of devices based on inclusive channel range
    DeviceSet remove(unsigned int lower, unsigned int upper);

    // Filters out devices matching/not matching a particular metadata value.
    DeviceSet remove(string key, string val, bool isEqual);

    // Filters out devices matching/not matching a particular metadata value specified by a regex.
    DeviceSet remove(string key, regex val, bool isEqual);

    // Removes a device based on a parameter comparison function provided by the caller
    DeviceSet remove(string key, LumiverseType* val, function<bool(LumiverseType* a, LumiverseType* b)> cmp, bool isEqual);

    // Removes all devices from the set that match the given query.
    DeviceSet remove(string query);

    // Inverts the selection.
    //DeviceSet invert();

    // Resets all the parameters in each device in the device set
    void reset();

    // These must mirror the device setparam functions.
    // This sets the value of a parameter on every device in the group
    // if the parameter already exists in the device (will not add params,
    // just modify).
    void setParam(string param, float val);

    // Sets an enumeration. See Device.h for more detailed info.
    void setParam(string param, string val, float val2 = -1.0f);

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

    // Equivalent of a set difference.
    void removeSet(DeviceSet otherSet);

    // Set of devices we're currently working with.
    set<Device *> m_workingSet;

    // Reference to the rig for accessing indexes and devices
    Rig* m_rig;
  };
}

#endif