#ifndef _DEVICE_H_
#define _DEVICE_H_

#pragma once

#include <map>
#include <string>
using namespace std;

// A Device in OpenLL maintains information about a lighting device.
// This class is meant to hold information about different parameters in
// an OpenLL friendly way. Conversion to network values happens in a
// different class to separate the abstract representation of a device
// from the practical network control details.

class Device
{
public:
  // Default constructor. Every device needs an id, channel, and type.
  // May in the future pull default parameter map from a database of known
  // fixture types.
  Device(string id, unsigned int channel, string type);

  // Destructor. Obviously.
  ~Device();

  // Accessors for id
  string getId() { return id; }
  void setId(string newId) { id = newId; }

  //Accessors for channel
  unsigned int getChannel() { return channel; }
  void setChannel(unsigned int newChan) { channel = newChan; }

  // Accesors for type
  string getType() { return type; }
  void setType(string newType) { type = newType; }


private:
  // Unique identifier for the device.
  // Uniqueness isn't quite enforceable at the device level.
  string id;

  // Channel number for the fixture. Does not have to be unique.
  unsigned int channel;

  // Device type name. "Source Four ERS" for example.
  string type;

  // Map for time-varying parameters.
  // Type may change in the future as more specialized datatypes come up.
  map<string, float> parameters;

  // Map for static information.
  // User-defined data helps to add search filters and query devices.
  map<string, string> metadata;
};

#endif