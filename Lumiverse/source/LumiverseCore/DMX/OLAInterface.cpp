#include "OLAInterface.h"

#ifdef USE_OLA

namespace Lumiverse {

OLAInterface::OLAInterface(string id) {
  m_ifaceId = id;
}

OLAInterface::~OLAInterface() {
  _olaClient.Stop();
}

void OLAInterface::init() {
  ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
  if (!_olaClient.Setup()) {
    Logger::log(ERR, "Failed to setup OLA Interface \"" + m_ifaceId + "\"");
  }
}

void OLAInterface::sendDMX(unsigned char* data, unsigned int universe) {
  _universes[universe].Set(data, 512);
  _olaClient.SendDmx(universe, _universes[universe]);
}

void OLAInterface::closeInt() {
  _olaClient.Stop();
}

void OLAInterface::reset() {
  closeInt();
  init();
}

JSONNode OLAInterface::toJSON() {
  JSONNode node;
  node.set_name(getInterfaceId());
  node.push_back(JSONNode("type", getInterfaceType()));
}

}

#endif