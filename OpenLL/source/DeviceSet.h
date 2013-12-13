#ifndef _DEVICESET_H_
#define _DEVICESET_H_

#pragma once

#include <sstream>
#include <set>

#include "Logger.h"
#include "Device.h"
#include "Rig.h"

class Rig;

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
class DeviceSet
{
public:
  // Constructs an empty set
  DeviceSet(Rig* rig);

  // Initializes the set with the given devices.
  DeviceSet(Rig* rig, set<Device *> devices);

  // Copy Constructor
  DeviceSet(DeviceSet& dc);

  // Destructor woo
  ~DeviceSet();

  // Adds a device to the set. Overloads for other common additions.
  DeviceSet add(Device* device);
  DeviceSet add(string id);
  DeviceSet add(unsigned int channel);

  // Adds devices based on metadata. isEqual determines if a device should be
  // added if the values are equal/not equal
  DeviceSet add(string key, string val, bool isEqual);

  // Removes a device from the set. Overloads for other common removals.
  DeviceSet remove(Device* device);
  DeviceSet remove(string id);
  DeviceSet remove(unsigned int channel);

  // Filters out devices matching/not matching a particular metadata key.
  DeviceSet remove(string key, string val, bool isEqual);

  // These are the filters we'll start with. Later we should have
  // ranged additions (channels 1-10), lists of ids, metadata contains,
  // metadata doesn't contain, metadata > some value, etc.

  // These must mirror the device setparam functions.
  // This sets the value of a parameter on every device in the group
  // if the parameter already exists in the device (will not add params,
  // just modify).
  void setParam(string param, float val);

private:
  // Adds to the set without returning a new copy.
  // Internal use only.
  void addDevice(Device* device);

  // Removes from the set without returning a new copy.
  // Internal use only.
  void removeDevice(Device* device);

  // Set of devices we're currently working with.
  set<Device *> m_workingSet;

  // Reference to the rig for accessing indexes and devices
  Rig* m_rig;
};

#endif