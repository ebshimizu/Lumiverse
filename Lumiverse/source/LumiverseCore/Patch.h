#ifndef _PATCH_H_
#define _PATCH_H_

#pragma once

#include "Device.h"
#include <set>

using namespace std;

namespace Lumiverse {
  // An Lumiverse Patch is a class that maps devices to output channels and
  // handles the outputting of data to the network.
  // This Patch class is an abstract base class that defines the actions any
  // implementation of a patch must have.
  class Patch
  {
  public:
    virtual ~Patch() { };

    // Grabs values from list of Devices, translates them to proper format for the
    // given network, and outputs the values to the network if the device is patched.
    // This function should be able to do incremental updates, so if just one
    // device had parameters change, you should be able to maintain the previous state
    // of the patch and just update that one device's value.
    virtual void update(set<Device *> devices) = 0;

    // Initializes settings for the patch. This can be starting up serial interfaces,
    // network configuration, etc.
    virtual void init() = 0;

    // It's like an "uninit." Prepares for patch shutdown.
    virtual void close() = 0;

    // Returns a JSON node representing the patch
    virtual JSONNode toJSON() = 0;

    // Gets the type of the patch as a string
    virtual string getType() = 0;

    // Gets a mapping of device parameters to addresses for the patch type.
    // This is the full patch map.
    // Entries should look something like "deviceId.paramName -> 1 / 25" (DMX example)
    // virtual map<string, string> getPatchParams() = 0;

    // Maps Device IDs to starting addresses.
    // This is a more concise view of the patch map compared to getPatchParams()
    // virtual map<string, string> getPatchIDs() = 0;

    // Gets a map of patch data with implementation-defined options.
    // Allows flexible querying of patches with implementation-specific details.
    // virtual map<string, string> getPatchInfo(string opts) = 0;
  };
}

#endif