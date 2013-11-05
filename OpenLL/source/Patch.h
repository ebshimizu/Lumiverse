#ifndef _PATCH_H_
#define _PATCH_H_

#pragma once

#include "Device.h"
using namespace std;

// An OpenLL Patch is a class that maps devices to output channels and
// handles the outputting of data to the network.
// This Patch class is an abstract base class that defines the actions any
// implementation of a patch must have.
class Patch
{
public:
  // Grabs values from Devices, translates them to proper format for the
  // given network, and outputs the values to the network.
  virtual void update() = 0;
  
  // Gets a mapping of device parameters to addresses for the patch type.
  // This is the full patch map.
  // Entries should look something like "deviceId.paramName -> 1 / 25" (DMX example)
  virtual map<string, string> getPatchParams() = 0;
  
  // Maps Device IDs to starting addresses.
  // This is a more concise view of the patch map compared to getPatchParams()
  virtual map<string, string> getPatchIDs() = 0;
  
  // Gets a map of patch data with implementation-defined options.
  // Allows flexible querying of patches with implementation-specific details.
  virtual map<string, string> getPatchInfo(string opts) = 0;
};

#endif