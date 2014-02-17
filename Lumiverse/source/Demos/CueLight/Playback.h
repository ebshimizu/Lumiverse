#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_

#pragma once

#include <LumiverseCore.h>

// A playback takes a cue (or cues) and manages the live transion between them
// Eventually a playback may be able to run multiple cues at once
// and effects on top of those cues. Right now, it does a single stack.
// It's important to note that this playback will only animate the
// active lights in a cue unless otherwise noted.
class Playback
{
public:
  Playback();
  ~Playback();
};

#endif