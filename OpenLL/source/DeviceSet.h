#ifndef _DEVICESET_H_
#define _DEVICESET_H_

#pragma once

#include <sstream>
#include <set>

#include "Logger.h"
#include "Device.h"
#include "Rig.h"

class Rig;

// Notes on a query syntax that's more easily readable:
// -strings with no special characters are treated as IDs
// -#[number] is treated as a channel number: #14 (channel 14)
// -#[number]-[number] is a range: #10-20 (channels 10-20)
// -@parameter=value selects all devices where the parameter is equal to value
// --@parameter= syntax also handles >=, <=, <, > for numeric types. It'll compare strings but might have odd results
// --@parameter= corresponds to openLL types, so might get a bit weird
// -$metadata=value selects all devices where the metadata parameter is equal to value
// --$metadata is similar to jquery and accepts |= (prefix), *= (contains), ~= (contains word), $= (ends with), ^= (starts with), 
// -@ and $ filters can be placed after channel ranges or ids with [] to further filter: #10-20[$color=R80]
// -![selector] is everything that's not the selector: !id1 (all devices that aren't id1)
// -Multiple selectors can be combined with a ,: $color=R80,$angle=front
// -Selectors can be grouped with []: [id1,id2,id3,id4][$color=R80] (selects the lights that have color R80 within the group specified previously)


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

  // Main function to select devices from a rig. Follows specific syntax rules. See
  // the implementation for syntax details.
  DeviceSet select(string selector);

  // Adds a device to the set. Overloads for other common additions.
  DeviceSet add(Device* device);
  DeviceSet add(string id);
  DeviceSet add(unsigned int channel);

  // Adds a group of devices based on inclusive channel range
  DeviceSet add(unsigned int lower, unsigned int upper);

  // Adds devices based on metadata. isEqual determines if a device should be
  // added if the values are equal/not equal
  DeviceSet add(string key, string val, bool isEqual);

  // Removes a device from the set. Overloads for other common removals.
  DeviceSet remove(Device* device);
  DeviceSet remove(string id);
  DeviceSet remove(unsigned int channel);

  // Removes a group of devices based on inclusive channel range
  DeviceSet remove(unsigned int lower, unsigned int upper);

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