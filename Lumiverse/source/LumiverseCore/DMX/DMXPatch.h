#ifndef _DMXPATCH_H_
#define _DMXPATCH_H_

#pragma once

#include "../Patch.h"
#include "DMXDevicePatch.h"
#include "DMXInterface.h"
#include "DMXPro2Interface.h"
#include "../lib/libjson/libjson.h"

#include <iostream>

// The DMX Patch object manages the communication between the DMX network
// and the Lumiverse devices.
// To add interfaces to this particular patch, you make a new interface object
// and assign it to a universe. You can assign multiple interfaces to the same
// universe, and you can assign a single device to multiple universes. However
// the latter option is probably only useful if your single device handles
// multiple universes.
// Note that the DMXPatch does not do additional configuration of the DMXInterface
// beyond what is described in the abstract DMXInterface class. You can access
// other implementation specific settings directly as permitted by the implementation.
class DMXPatch : public Patch
{
public:
  // Constructs a DMXPatch object
  DMXPatch();

  // Construct DMXPatch from JSON data.
  DMXPatch(const JSONNode data);

  // Destroys the object.
  virtual ~DMXPatch();

  // Updates the values sent to the DMX network given the list of devices
  // in the rig. The list of devices should be maintained outside of this class.
  // This function is able to do incremental updates, so if just one
  // device had parameters change, you are able to maintain the previous state
  // of the patch and just update that one device's value.
  virtual void update(set<Device *> devices);

  // Initializes connections and other network settings for the patch.
  // Call this AFTER all interfaces have been assigned. May need to call again
  // if interfaces change.
  virtual void init();

  // Closes connections to the interfaces.
  virtual void close();

  // Gets a mapping of device parameters to addresses for the patch type.
  // This is the full patch map.
  // Entries should look something like "deviceId.paramName -> 1 / 25" (DMX example)
  // virtual map<string, string> getPatchParams();

  // Maps Device IDs to starting addresses.
  // This is a more concise view of the patch map compared to getPatchParams()
  // virtual map<string, string> getPatchIDs();

  // Gets a map of patch data with implementation-defined options.
  // Allows flexible querying of patches with implementation-specific details.
  // virtual map<string, string> getPatchInfo(string opts);

  // Assigns interface "iface" to universe "universe".
  // Interfaces allocated by the user will be freed by the patch
  // either on program end or on unpatch. If patching an interface that already
  // exists in this patch, you may leave iface null, as this function will check to
  // see if an interface with the id already exists.
  void assignInterface(DMXInterface* iface, unsigned int universe);

  // Deletes an interface with id "id" from the patch.
  // Note that this will unmap ALL universes mapped to this device and deallocate it.
  void deleteInterface(string id);

  // Moves an interface from a specified universe to a specified universe.
  // If the interface isn't in "universeFrom" already it just gets assigned to
  // "universeTo"
  void moveInterface(string id, unsigned int universeFrom, unsigned int universeTo);

  // Returns the interface map. ID -> Universe.
  const multimap<string, unsigned int> getInterfaceInfo() { return m_ifacePatch; }

  // Patches a given device to the given DMXDevicePatch.
  // At some point this should get nicer and do some stuff automatically for you
  // (like looking up profiles on patch).
  void patchDevice(Device* device, DMXDevicePatch* patch);

  // Alternate patch function which just specifies an ID in a string.
  void patchDevice(string id, DMXDevicePatch* patch);

  // Adds a device map to the Patch's database of mappings.
  // This function will REPLACE a map that already exists.
  void addDeviceMap(string id, map<string, patchData> deviceMap);

  // Adds a parameter to a deviceMap.
  void addParameter(string mapId, string paramId, unsigned int address, conversionType type);

  // Debug function that prints out all DMX values for all universes in the patch.
  void dumpUniverses();

  // Debug funtion that prints out all DMX values for a single universe.
  void dumpUniverse(unsigned int universe);

private:
  // Loads data from a parsed JSON object
  void loadJSON(const JSONNode data);

  // Loads the device maps from a JSON node
  void loadDeviceMaps(const JSONNode data);

  // Stores the state of the DMX universes.
  // Note that DMX Universe 1 is index 0 here due to one-indexing.
  vector<vector<unsigned char> > m_universes;

  // Maps interface id to universe number (zero-indexed)
  // An interface can be mapped to multiple universes, since some devices have
  // more than one output.
  multimap<string, unsigned int> m_ifacePatch;

  // DMX Interfaces controlled by this patch. See DMXInterface.h for details.
  // Maps interface ids to interface. This is a unique mapping.
  map<string, DMXInterface*> m_interfaces;

  // Maps devices to DMX outputs. See the DMXDevicePatch class for details.
  // This class will free DevicePatch objects given to it on destruction.
  map<string, DMXDevicePatch*> m_patch;

  // Stores information about device maps, which can be reused across
  // devices. Key is the device map name.
  map<string, map<string, patchData> > m_deviceMaps;
};

#endif
