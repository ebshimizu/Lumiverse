/*! \file KiNetInterface.h
* \brief Class for using KiNet.
*/
#ifndef _KINETINTERFACE_H_
#define _KINETINTERFACE_H_

#pragma once
#include "LumiverseCoreConfig.h"

#ifdef USE_KINET

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "DMXInterface.h"
#include "../lib/libjson/libjson.h"
#include "Logger.h"
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>
#include <fcntl.h>

namespace Lumiverse {
  /*!
  * \brief Describes which version of the KiNet Protocol to use when
  * making a KiNet Interface
  */
  enum KinetProtocolType {
    OLD,  /*!< Use the old header format */
    NEW   /*!< Use the new header format */
  };

  /*!
  * \brief Provides an interface to a KiNet system
  *
  * Based off of Mike Dewberry's implementation of KiNet, which
  * can be found in his Streetlight project: https://github.com/Dewb/streetlight
  */
  class KiNetInterface : public DMXInterface
  {
  public:
    /*!
    * \brief Creates a new KiNet Interface
    *
    * \param id Identifier for this interface
    * \param host Hostname or IP of the KiNet Power Supply to send data to
    * \param port Port to send data to
    * \param protocolType Tells this interface which interface to use.
    */
    KiNetInterface(string id, string host, int port = 6038, enum KinetProtocolType protocolType = KinetProtocolType::OLD);

    ~KiNetInterface();

    virtual void init();

    virtual void sendDMX(unsigned char* data, unsigned int universe);

    virtual void closeInt();

    virtual void reset();

    virtual JSONNode toJSON();

    virtual string getInterfaceType() { return "KiNetInterface"; }
  
    string getIP() { return m_host; }
    void setIP(string ip) { m_host = ip; }

    int getPort() { return m_port; }
    void setPort(int port) { m_port = port; }

    size_t getHeaderSize() const { return m_headerSize; }
    size_t getDataSize() const { return m_dataSize; }
    size_t getNumChannels() const { return m_numChannels; }
    size_t getPacketSize() const { return m_headerSize + m_dataSize; }
    size_t getBufferSize() const { return getPacketSize() * m_numChannels; }
    const unsigned char* getHeaderBytes() const { return m_headerBytes; }

  private:
    string m_host;
    int m_port;
    bool m_connected;
    int m_socket;

    unsigned char* m_buffer;

    // Protocol header
    const unsigned char* m_headerBytes;
    size_t m_headerSize;
    size_t m_dataSize;
    size_t m_numChannels;
    KinetProtocolType m_type;
  };
}

#endif

#endif