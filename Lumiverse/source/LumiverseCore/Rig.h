#ifndef _RIG_H_
#define _RIG_H_

#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>
#include <functional>

#include "LumiverseCoreConfig.h"
#include "Patch.h"
#include "DMX/DMXPatch.h"
#include "Device.h"
#include "Logger.h"
#include "DeviceSet.h"
#include "lib/libjson/libjson.h"

class DeviceSet;

// The Rig contains information about the state of the lighting system.
// It will manage devices and patches added to it by the user and
// provides functions for manipulating those devices.
// TODO: Right now if you change a device channel or id the rig doesn't
// know about it. Need to add functions in the Rig that allow
// this sort of change.
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

  // Gets a patch from a rig and treats it as a DMXPatch.
  // Use carefully.
  DMXPatch* getPatchAsDMXPatch(string id) { return (DMXPatch*)getPatch(id); }

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

  // Writes the rig out to a JSON file
  // Be default will not overwrite the file if it exists.
  // Returns true on success.
  bool save(string filename, bool overwrite = false);

  // Gets the JSON data for the rig as a string.
  JSONNode toJSON();
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

  // Allows a user to run an additional function during the update loop.
  vector<function<void()>> m_updateFunctions;

  // May have more indicies in the future, like mapping by channel number.
};

#endif