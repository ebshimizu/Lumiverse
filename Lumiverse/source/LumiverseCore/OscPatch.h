/*!
\file OscPatch.h
\brief Implementation of a patch for OSC
*/
#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_OSC

#include "Patch.h"
#include "lib/oscpack/osc/OscOutboundPacketStream.h"
#include "lib/oscpack/osc/OscPacketListener.h"
#include "lib/oscpack/osc/OscReceivedElements.h"
#include "lib/oscpack/ip/UdpSocket.h"

namespace Lumiverse {

enum OscFormat {
  PREFIXED_ADDR = 0,
  PER_DEVICE_ADDR = 1,
  ETC_EOS = 2
};

class OscPatch : public Patch, public osc::OscPacketListener {

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
  void changeInPort(int port);
  string getAddress();
  int getPort();
  int getInPort();
  bool isRunning();

  /*!
  \brief Only active in ETC_EOS mode. Synchronizes device values with what's in EOS
  \return true on success, false on failure

  Requires starting up an OSC receiver
  */
  bool sync(const set<Device *> devices);

  /*!
  \brief Determines how the OSC messages are sent
  Under PREFIXED_ADDR the OSC packet will be arranged as follows: /[pattern]/[id] {params}
  Under PER_DEVICE_ADDR the OSC packet will be arranged as follows: /[id] {params}
  User ETC_EOS the OSC packet will send Eos commands to the corresponding channel number
  */
  OscFormat _mode;

  /*! \brief In fixed mode, the pattern to which messages are sent */
  string _pattern;

protected:
  virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remote);

private:
  string _address;
  int _port;
  int _inPort;
  bool _running;

  UdpTransmitSocket* _t;

  /*!
  \brief Converts a device to OSC and places the conversion in the outbound packet stream
  */
  void deviceToOsc(osc::OutboundPacketStream& p, Device* d);

  /*!
  \brief Outputs a series of ETC Eos commands to update the state

  Note that this function operates on specified device paramter names. If the paramter names don't match,
  nothing will be transmitted.
  */
  void deviceToEos(Device* d);

  /*!
  \brief Resets the Eos command line
  */
  void newEosCmd();

  /*!
  \brief Loads a patch from JSON data
  */
  void loadJSON(JSONNode data);

  Device* _syncDevice;
  bool _syncReady;
  set<string> _syncParams;
};

}

#endif