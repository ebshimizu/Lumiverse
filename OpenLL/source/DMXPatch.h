#ifndef _DMXPATCH_H_
#define _DMXPATCH_H_

#pragma once

#include "Patch.h"
#include "DMXDevicePatch.h"
#include "DMXInterface.h"

// The DMX Patch object manages the communication between the DMX network
// and the OpenLL devices.
// At the moment, this class contains code to communicate with a DMX Interface
// which can be any arbitrary device that communicates through DMX. See
// DMXInterface.h for more details.
class DMXPatch : public Patch
{
public:
  // Constructs a DMXPatch object
  DMXPatch();
  ~DMXPatch();

private:
  // Stores the state of the DMX universes.
  // Note that DMX Universe 1 is index 0 here due to one-indexing.
  vector<unsigned int[512]> m_universes;

  // DMX Interfaces controlled by this patch. See DMXInterface.h for details.
  vector<DMXInterface> m_interfaces;

  // Maps devices to DMX outputs. See the DMXDevicePatch class for details.
  map<string, DMXDevicePatch> m_patch;
};

#endif