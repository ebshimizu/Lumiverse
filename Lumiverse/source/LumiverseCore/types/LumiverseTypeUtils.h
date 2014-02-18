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
  // Essentially just invokes operator= based on the type of the data.
  void copyByVal(LumiverseType* source, LumiverseType* target);
  
  // Compares two generic LumiverseType pointers for equality
  bool equals(LumiverseType* lhs, LumiverseType* rhs);

  // Returns the following:
  // 0 if lhs and rhs are equal
  // 1 if lhs > rhs
  // -1 if lhs < rhs
  // -2 if lhs and rhs are not the same type, are null, or are of unknown type
  int cmp(LumiverseType* lhs, LumiverseType* rhs);

  // Lerps the values of a LumiverseType and returns the value
  shared_ptr<LumiverseType> lerp(LumiverseType* lhs, LumiverseType* rhs, float t);

  // Returns true if lhs and rhs are not null and both the same type
  inline bool areSameType(LumiverseType* lhs, LumiverseType* rhs);
}

#endif