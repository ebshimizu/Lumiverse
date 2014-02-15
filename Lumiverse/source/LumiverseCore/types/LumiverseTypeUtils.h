#ifndef _LUMIVERSETYPEUTILS_H_
#define _LUMIVERSETYPEUTILS_H_

#pragma once

#include "LumiverseCore.h"

namespace LumiverseTypeUtils {
  // Copies a LumiverseType and returns an abstracted pointer to the new value.
  // Returns nullptr is type is unknown to the Lumiverse system
  LumiverseType* copy(LumiverseType* data);
  
  // Compares two generic LumiverseType pointers
  bool equals(LumiverseType* lhs, LumiverseType* rhs);
}

#endif