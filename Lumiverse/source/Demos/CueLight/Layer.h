#ifndef _LAYER_H_
#define _LAYER_H_

#pragma once

#include "LumiverseCore.h"
#include "CueList.h"
#include "Cue.h"

// A Layer stores a state of the Rig.
// Layers can contain a CueList, a static state, or an Effect (effects to
// be added later). Layers maintain their current state, blend mode, and
// visibility settings. The Playback object will interpret these settings
// and perform the appropriate functions to flatten the layers.
// Layers work by creating duplicates of the devices in a Rig (selected or
// global) and manipulating their state.
class Layer
{
public:
  Layer();
  ~Layer();


private:
  map<Device, 
};

#endif