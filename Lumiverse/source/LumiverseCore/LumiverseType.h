#ifndef _LumiverseTYPE_H_
#define _LumiverseTYPE_H_
#pragma once

#include <string>
#include "Logger.h"
#include "lib/libjson/libjson.h"

using namespace std;

// This class is a wapper around a variety of different possible
// data types that might be needed by a fixture.
// Defining a new type is as easy as deriving from this class.
class LumiverseType
{
public:
  // Destroys the object.
  virtual ~LumiverseType() { };

  // Gets the name of the type
  virtual string getTypeName() = 0;

  // Resets the data to a type-defined default.
  virtual void reset() = 0;

  // Converts the type to a JSON object with the specified name
  virtual JSONNode toJSON(string name) = 0;

  // Yeah actually there's not much here because types are
  // all different.
};

#endif