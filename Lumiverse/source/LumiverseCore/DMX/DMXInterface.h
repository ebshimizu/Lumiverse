#ifndef _DMXINTERFACE_H_
#define _DMXINTERFACE_H_

#pragma once

#include "../lib/libjson/libjson.h"
#include <string>
using namespace std;

// Provides a common interface to various DMX devices.
// Adding a new type of DMX device should be simple. Just derive
// from this class and implement the required functions.
class DMXInterface
{
public:
  // Virtual destructor yay
  virtual ~DMXInterface() { };

  // Initalizes the interface.
  // This function will throw exceptions if something comes up during
  // the initialization stage.
  virtual void init() = 0;

  // Sends DMX through the interface. Also includes the universe number, for
  // devices that may manage more than one universe or need to know the universe number.
  // NOTE: The universe passed in here is ZERO-INDEXED as it is in the DMXPatch class.
  virtual void sendDMX(unsigned char* data, unsigned int universe) = 0;

  // Closes the connection to the DMX device
  virtual void close() = 0;

  // Resets the interface. Is essentially a disconnect then reconnect and
  // any other refresher code needed.
  virtual void reset() = 0;

  // Returns the JSON representation of the interface
  virtual JSONNode toJSON() = 0;

  // Returns the name of the interface's type
  virtual string getInterfaceType() = 0;

  // Returns the name of this DMX interface.
  string getInterfaceName() { return m_ifaceName; }

  // Returns the ID of this DMX interface.
  string getInterfaceId() { return m_ifaceId; }

  // Sets the interface ID
  void setInterfaceId(string id) { m_ifaceId = id; }

protected:
  // Name of the DMX interface. Can (and probably should) be hard coded in a derived-class.
  string m_ifaceName;

  // Interface ID. Unique string identifying this particular interface.
  string m_ifaceId;
};

#endif