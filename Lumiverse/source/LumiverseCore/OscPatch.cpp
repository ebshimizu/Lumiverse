#include "OscPatch.h"

namespace Lumiverse {

OscPatch::OscPatch(string address, int port, OscFormat mode, string pattern) :
  _address(address), _port(port), _mode(mode), _pattern(pattern), _running(false)
{
  _t = nullptr;
}

OscPatch::OscPatch(JSONNode data)
{
  loadJSON(data);
}

OscPatch::~OscPatch()
{
  if (_t != nullptr)
    delete _t;
}

void OscPatch::init()
{
  _t = new UdpTransmitSocket(IpEndpointName(_address.c_str(), _port));
  _running = true;

  stringstream ss;
  ss << "Osc Patch started on address " << _address << ":" << _port;
  Logger::log(INFO, ss.str());
}

void OscPatch::update(set<Device*> devices)
{
  for (auto d : devices) {
    // if somehow there's a message over 10000 characters long we may have a problem
    char buffer[10000];
    osc::OutboundPacketStream packet(buffer, 10000);

    deviceToOsc(packet, d);
    _t->Send(packet.Data(), packet.Size());
  }
}

void OscPatch::close()
{
  if (_t != nullptr) {
    delete _t;
    _t = nullptr;
  }

  _running = false;
  Logger::log(INFO, "Osc Patch closed");
}

JSONNode OscPatch::toJSON()
{
  JSONNode root;

  root.push_back(JSONNode("type", getType()));
  root.push_back(JSONNode("address", _address));
  root.push_back(JSONNode("port", _port));
  root.push_back(JSONNode("pattern", _pattern));
  root.push_back(JSONNode("mode", _mode));

  return root;
}

void OscPatch::deleteDevice(string id)
{
  // oscpatch maintains no state
}

void OscPatch::changeAddress(string address, int port)
{
  if (_running) {
    Logger::log(WARN, "Can't change OSC address/port while patch is running");
    return;
  }

  _address = address;
  _port = port;

  // call init again to confirm changes, user must do
}

string OscPatch::getAddress()
{
  return _address;
}

int OscPatch::getPort()
{
  return _port;
}

bool OscPatch::isRunning()
{
  return _running;
}

void OscPatch::deviceToOsc(osc::OutboundPacketStream & p, Device * d)
{
  if (_mode == PREFIXED_ADDR) {
    p << osc::BeginMessage(string("/" + _pattern + "/" + d->getId()).c_str());
  }
  else if (_mode == PER_DEVICE_ADDR) {
    p << osc::BeginMessage(string("/" + d->getId()).c_str());
  }

  // convert params
  for (auto dp : d->getRawParameters()) {
    LumiverseType* param = dp.second;
    string name = dp.first;

    p << name.c_str();

    if (param->getTypeName() == "float") {
      // floats returned as percentages
      p << "float";
      p << (float)((LumiverseFloat*)(param))->asPercent();
    }
    else if (param->getTypeName() == "color") {
      // colors are RGB
      LumiverseColor* c = (LumiverseColor*)(param);
      p << "color";
      auto rgb = c->getRGB();
      p << (float)rgb[0] << (float)rgb[1] << (float)rgb[2];
    }
    else if (param->getTypeName() == "enum") {
      // enums will send their value as a percent and the name of the current setting
      p << "enum";
      LumiverseEnum* e = (LumiverseEnum*)(param);
      p << (float)e->asPercent();
      p << e->getVal().c_str();
    }
    else if (param->getTypeName() == "orientation") {
      // orientations are basically floats but with an extra units value
      p << "orientation";
      LumiverseOrientation* o = (LumiverseOrientation*)(param);
      p << (float)o->getVal();
      p << o->getUnit();
    }
  }

  p << osc::EndMessage;
}

void OscPatch::loadJSON(JSONNode data)
{
  auto addr = data.find("address");
  if (addr != data.end())
    _address = addr->as_string();
  else
    _address = "localhost";

  auto port = data.find("port");
  if (port != data.end())
    _port = port->as_int();
  else
    _port = 8000;

  auto pattern = data.find("pattern");
  if (pattern != data.end())
    _pattern = pattern->as_string();
  else
    _pattern = "lumiverse";

  auto mode = data.find("mode");
  if (mode != data.end())
    _mode = (OscFormat)mode->as_int();
  else
    _mode = PREFIXED_ADDR;
}

}