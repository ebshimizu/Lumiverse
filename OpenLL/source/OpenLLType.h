#ifndef _OPENLLTYPE_H_
#define _OPENLLTYPE_H_
#pragma once

#include <string>
using namespace std;

// This class is a wapper around a variety of different possible
// data types that might be needed by a fixture.
// Defining a new type is as easy as deriving from this class.
class OpenLLType
{
public:
  // Destroys the object.
  virtual ~OpenLLType() { };

  // Gets the name of the type
  virtual string getTypeName() = 0;

  // Resets the data to a type-defined default.
  virtual void reset() = 0;

  // Yeah actually there's not much here because types are
  // all different.
};

#endif