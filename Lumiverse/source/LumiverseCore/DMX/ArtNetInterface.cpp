#include "ArtNetInterface.h"

#ifdef USE_ARTNET

namespace Lumiverse {

ArtNetInterface::ArtNetInterface(string id, string ip, string broadcast, bool verbose) :
  m_ip(ip), m_broadcast(broadcast), m_verbose(verbose)
{
  m_connected = false;
  m_ifaceId = id;
}

ArtNetInterface::~ArtNetInterface()
{
}

void ArtNetInterface::init() {
  // Set up ArtNet Node
  if ((m_node = artnet_new(m_ip.c_str(), m_verbose)) == NULL) {
    stringstream ss;
    ss << "Could not start ArtNet node for address " << m_ip << ". ArtNet error: " << artnet_strerror();
    Logger::log(ERR, ss.str());
    return;
  }

  // Assign name
  artnet_set_short_name(m_node, m_ifaceId.c_str());
  artnet_set_long_name(m_node, ("Lumiverse ArtNet Node " + m_ifaceId).c_str());
  artnet_set_node_type(m_node, ARTNET_NODE);
  artnet_set_subnet_addr(m_node, 0);

  // Start. 
  if (artnet_start(m_node) != ARTNET_EOK) {
    stringstream ss;
    ss << "Failed to start ArtNet node. ArtNet error: " << artnet_strerror();
    Logger::log(ERR, ss.str());
    closeInt();
    return;
  }

  m_connected = true;
}

void ArtNetInterface::sendDMX(unsigned char* data, unsigned int universe) {
  if (m_universes.count(universe) == 0)
    initUniverse(universe);

  artnet_send_dmx(m_node, universe, ARTNET_DMX_LENGTH, data);
}

void ArtNetInterface::initUniverse(int universe) {
  artnet_set_port_type(m_node, universe, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, universe, ARTNET_OUTPUT_PORT, universe);
  
  m_universes.insert(universe);
}

void ArtNetInterface::closeInt() {
  if (m_connected) {
    artnet_destroy(m_node);

    m_connected = false;
  }
}

void ArtNetInterface::reset() {
  closeInt();
  init();
}

JSONNode ArtNetInterface::toJSON() {
  JSONNode root;

  root.set_name(getInterfaceId());
  root.push_back(JSONNode("type", getInterfaceType()));
  root.push_back(JSONNode("ip", m_ip));
  root.push_back(JSONNode("broadcast", m_broadcast));
  root.push_back(JSONNode("verbose", m_verbose));

  return root;
}

}

#endif