/*! \file OLAInterface.h
* \brief Class for using ArtNet.
*/
#ifndef _OLAINTERFACE_H_
#define _OLAINTERFACE_H_

#pragma once
#include "LumiverseCoreConfig.h"

#ifdef USE_OLA

#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/client/StreamingClient.h>
#include <lib/libjson/libjson.h>

#include "DMXInterface.h"
#include "Logger.h"
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

namespace Lumiverse {
  /*!
  * \brief Provides an interface to an OLA server
  *
  * This class uses the streaming client version of OLA.
  * As such, it doesn't utilize any of the more advanced RDMA features of OLA
  * at the moment.
  */
  class OLAInterface : public DMXInterface
  {
  public:
    /*!
    * \brief Creates a new OLA Interface
    *
    * \param id Identifier for this interface
    */
    OLAInterface(string id);

    ~OLAInterface();

    virtual void init();

    virtual void sendDMX(unsigned char* data, unsigned int universe);

    virtual void closeInt();

    virtual void reset();

    virtual JSONNode toJSON();

    virtual string getInterfaceType() { return "OLAInterface"; }
  
  private:
    /*!
    \brief List of universes used by OLA

    Universes are initialized the first time they get
    encountered in the update process.
    */
    unordered_map<unsigned int, ola::DmxBuffer> _universes;

    /*!
    \brief Connection to the OLA server.
    */
    ola::client::StreamingClient _olaClient;
  };
}

#endif

#endif