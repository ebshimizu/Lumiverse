#ifndef _LUMIVERSETYPEUTILS_H_
#define _LUMIVERSETYPEUTILS_H_

#pragma once

#include "LumiverseCore.h"

// Functions to make life with these generic types easier.
namespace LumiverseTypeUtils {
  // Copies a LumiverseType and returns an abstracted pointer to the new value.
  // Returns nullptr if type is unknown to the Lumiverse system
  LumiverseType* copy(LumiverseType* data);

  // Copies the data from source into target.
  // Essentially just invokes operator= on each side based on the type of the
  // data.
  void copyByVal(LumiverseType* source, LumiverseType* target);
  
  // Compares two generic LumiverseType pointers
  bool equals(LumiverseType* lhs, LumiverseType* rhs);
}

#endif