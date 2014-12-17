/*! \file ArtNetInterface.h
* \brief Class for using ArtNet.
*/
#ifndef _ARTNETINTERFACE_H_
#define _ARTNETINTERFACE_H_

#pragma once
#include "LumiverseCoreConfig.h"

#ifdef USE_ARTNET

#include <lib/libartnet/artnet.h>
#include <lib/libjson/libjson.h>

#include "DMXInterface.h"
#include "Logger.h"
#include <string>
#include <sstream>
#include <iostream>
#include <set>

namespace Lumiverse {
  /*!
  * \brief Provides an interface to an ArtNet system
  *
  * Uses libartnet to provide ArtNet communication.
  * ArtNet broadcasts via UDP on a specified subnet. The user is responsible for picking
  * the right IPs to make the system work. This particular interface specifically sends out DMX ArtNet
  * packets. 
  */
  class ArtNetInterface : public DMXInterface
  {
  public:
    /*!
    * \brief Creates a new ArtNet Interface
    *
    * \param id Identifier for this interface
    * \param ip IP address for this ArtNet Node
    * \param broadcast Broadcast address
    * \param verbose Set to true to enable detailed logging to stdout
    * \param protocolType Tells this interface which interface to use.
    */
    ArtNetInterface(string id, string ip, string broadcast = "", bool verbose = false);

    ~ArtNetInterface();

    virtual void init();

    virtual void sendDMX(unsigned char* data, unsigned int universe);

    virtual void closeInt();

    virtual void reset();

    virtual JSONNode toJSON();

    virtual string getInterfaceType() { return "ArtNetInterface"; }
  
  private:
    void initUniverse(int universe);

    string m_ip;
    string m_broadcast;
    bool m_verbose;
    bool m_connected;

    artnet_node m_node;

    /*!
    \brief List of universes used by ArtNet.

    Universes are initialized the first time they get
    encountered in the update process. 
    */
    set<int> m_universes;
  };
}

#endif

#endif