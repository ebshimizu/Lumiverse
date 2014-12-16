#include "KiNetInterface.h"

#ifdef USE_KINET

namespace Lumiverse {
const unsigned char oldHeaderBytes[] = {
  0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0x00
};

const unsigned char newHeaderBytes[] = {
  0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x08, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x01, 0xFF, 0x00, 0xFF, 0x0F
};

KiNetInterface::KiNetInterface(string id, string host, int port, enum KinetProtocolType protocolType)
  : m_host(host), m_port(port)
{
  m_connected = false;
  m_ifaceId = id;
  m_type = protocolType;

  // Assign protocol info.
  switch (m_type) {
    case (OLD) :
      m_headerSize = 21;
      m_dataSize = 512;
      m_numChannels = 1;
      m_headerBytes = oldHeaderBytes;
      break;
    case (NEW) :
      m_headerSize = 24;
      m_dataSize = 512;
      m_numChannels = 16;
      m_headerBytes = newHeaderBytes;
      break;
    default :
      // defaults to old
      m_headerSize = 21;
      m_dataSize = 512;
      m_numChannels = 1;
      m_headerBytes = oldHeaderBytes;
      break;
  }
}

KiNetInterface::~KiNetInterface()
{
  free(m_buffer);
}

void KiNetInterface::init() {
  // Initialize buffer
  if (m_buffer == nullptr)
    free(m_buffer);

  m_buffer = (unsigned char*)malloc(getBufferSize() * sizeof(unsigned char));
  memset(m_buffer, (int)getBufferSize(), 0);
  for (int c = 0; c < getNumChannels(); c++)
  {
    memcpy(m_buffer + c * getPacketSize(), getHeaderBytes(), getHeaderSize());
    // Looks like channels are like multiple DMX streams on the same power supply
    // Current code doesn't account for this.
    if (getNumChannels() > 1)
      m_buffer[c * getPacketSize() + 16] = c + 1;
  }

  // Connect to power supply
  struct addrinfo hints;
  struct addrinfo *pResult, *pr;
  int result;
  int sock;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  stringstream port;
  port << m_port;

#ifdef _WIN32
  WSADATA wsaData;
  int res;

  res = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (res != NO_ERROR)
    Logger::log(ERR, "Error at WSAStartup()");
#endif
 
  result = getaddrinfo(m_host.c_str(), port.str().c_str(), &hints, &pResult);
  if (result != 0) {
    stringstream msg;
    msg << "getaddrinfo: " << gai_strerror(result);
    Logger::log(ERR, msg.str());
  }

  for (pr = pResult; pr != nullptr; pr = pr->ai_next) {
    sock = socket(pr->ai_family, pr->ai_socktype, pr->ai_protocol);
    if (sock == -1)
      continue;

    if (::connect(sock, pr->ai_addr, (int)pr->ai_addrlen) == 0)
      break;

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
  }

  if (pr == NULL)
  {
    Logger::log(ERR, "Could not connect to socket.");
    return;
  }

  m_port = ((sockaddr_in*)(pr->ai_addr))->sin_port;

  freeaddrinfo(pResult);

#ifdef _WIN32
  u_long mode = 0;

  res = ioctlsocket(sock, FIONBIO, &mode);
  if (res != NO_ERROR) {
    stringstream ss;
    int errCode = WSAGetLastError();
    ss << "ioctlsocket failed with error: " << errCode;
    Logger::log(ERR, ss.str());
  }
#else
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif

  m_socket = sock;
  m_connected = true;
}

void KiNetInterface::sendDMX(unsigned char* data, unsigned int universe) {
  // Currently this function only really supports the old protocol.
  // The new protocol layers universes, and this function assumes a single DMX universe (512)
  for (int channel = 0; channel < getNumChannels(); channel++)
  {
    memcpy(m_buffer + getHeaderSize(), data, 512);
    send(m_socket, (char *)m_buffer + getPacketSize() * channel, (int)getPacketSize(), 0);
  }
}

void KiNetInterface::closeInt() {
  if (m_connected) {
#ifdef _WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
    m_connected = false;
  }
}

void KiNetInterface::reset() {
  closeInt();
  init();
}

JSONNode KiNetInterface::toJSON() {
  JSONNode root;

  root.set_name(getInterfaceId());
  root.push_back(JSONNode("type", getInterfaceType()));
  root.push_back(JSONNode("host", m_host));
  root.push_back(JSONNode("port", m_port));
  root.push_back(JSONNode("protocolType", m_type));

  return root;
}

}

#endif