#ifndef _LUMIVERSEENUM_
#define _LUMIVERSEENUM_

#pragma once

#include "../LumiverseType.h"

// Defines an enumeration in Lumiverse.
// Moving lights often have various parameters that are enumerated
// and encoded in a DMX range (ex. 10-19 on channel 6 = use gobo 1).
// This class will allow specification of specific settings by name or number
// and also will allow users to tweak the values in the range for controls
// that may use the range as a speed control.
class LumiverseEnum
{
public:
  LumiverseEnum();
  ~LumiverseEnum();
};

#endif