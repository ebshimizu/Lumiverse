#ifndef _RIG_H_
#define _RIG_H_

#pragma once
#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>

#include "Patch.h"
#include "Device.h"
#include "../lib/libjson/libjson.h"

// The Rig contains information about the state of the lighting system.
// It will manage devices and patches added to it by the user and
// provides functions for manipulating those devices.
class Rig
{
public:
  // Makes an empty rig
  Rig();

  // Initialize Rig from JSON file
  Rig(string filename);

  // Makes a rig that loads data from the specified file
  // Rig(string filename)

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
private:
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
  vector<Device *> m_devices;

  // Patches mapped by an identifier chosen by the user.
  map<string, Patch *> m_patches;

  // Devices mapped by their device ID.
  map<string, Device *> m_devicesById;

  // May have more indicies in the future, like mapping by channel number.
};

#endif