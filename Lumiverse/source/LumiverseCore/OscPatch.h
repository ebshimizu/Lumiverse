/*!
\file OscPatch.h
\brief Implementation of a patch for OSC
*/
#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_OSC

#include "Patch.h"
#include "lib/oscpack/osc/OscOutboundPacketStream.h"
#include "lib/oscpack/ip/UdpSocket.h"

namespace Lumiverse {

enum OscFormat {
  PREFIXED_ADDR = 0,
  PER_DEVICE_ADDR = 1
};

class OscPatch : public Patch {

public:
  OscPatch(string address, int port, OscFormat mode = PREFIXED_ADDR, string pattern = "lumiverse");
  OscPatch(JSONNode data);
  ~OscPatch();

  virtual void init() override;

  virtual void update(set<Device *> devices) override;

  virtual void close() override;

  virtual JSONNode toJSON() override;

  virtual void deleteDevice(string id) override;

  virtual string getType() { return "osc"; }

  void changeAddress(string address, int port);
  string getAddress();
  int getPort();
  bool isRunning();

  /*!
  \brief Determines how the OSC messages are sent
  Under PREFIXED_ADDR the OSC packet will be arranged as follows: /[pattern]/[id] {params}
  Under PER_DEVICE_ADDR the OSC packed will be arranged as follows: /[id] {params}
  */
  OscFormat _mode;

  /*! \brief In fixed mode, the pattern to which messages are sent */
  string _pattern;

private:
  string _address;
  int _port;
  bool _running;

  UdpTransmitSocket* _t;

  /*!
  \brief Converts a device to OSC and places the conversion in the outbound packet stream
  */
  void deviceToOsc(osc::OutboundPacketStream& p, Device* d);

  /*!
  \brief Loads a patch from JSON data
  */
  void loadJSON(JSONNode data);
};

}

#endif