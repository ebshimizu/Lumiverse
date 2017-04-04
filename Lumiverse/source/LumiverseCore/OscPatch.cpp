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

  if (_mode == ETC_EOS) {
    char buf[64];
    osc::OutboundPacketStream p(buf, 64);
    p << osc::BeginMessage("/eos/user") << 1 << osc::EndMessage;
    _t->Send(p.Data(), p.Size());
  }

  stringstream ss;
  ss << "Osc Patch started on address " << _address << ":" << _port;
  Logger::log(INFO, ss.str());
}

void OscPatch::update(set<Device*> devices)
{
  if (!_running)
    return;

  for (auto d : devices) {
    if (_mode == ETC_EOS) {
      deviceToEos(d);
    }
    else {
      // if somehow there's a message over 10000 characters long we may have a problem
      char buffer[10000];
      osc::OutboundPacketStream packet(buffer, 10000);

      deviceToOsc(packet, d);
      _t->Send(packet.Data(), packet.Size());
    }
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

void OscPatch::sync(const set<Device*> devices)
{
  // bit clunky here but Eos echoes current parameter settings when something is selected
  // so what we'll do is just go and select each device and listen for the echoed parameters
  // we do need to ensure order here though, so this process does disable transmit while running
  _running = false;
  // wait for loop to finish
  this_thread::sleep_for(chrono::milliseconds(500));

  // TODO: IN PORT CHANGABLE
  UdpListeningReceiveSocket rcv(IpEndpointName(_address.c_str(), 9000), this);
  std::thread rcvt(function<void()>([&rcv] { rcv.Run(); }));

  // feed it some stuff to clear
  char buf1[64];
  osc::OutboundPacketStream forceReset(buf1, 64);
  forceReset << osc::BeginMessage("/eos/reset") << osc::EndMessage;

  Logger::log(LDEBUG, "Started sync thread");
  _syncReady = true;

  for (auto d : devices) {
    // idle until previous packets processed
    while (!_syncReady) {
      this_thread::sleep_for(chrono::milliseconds(5));
    }
    
    // force Eos to select an invalid channel so it actually sends out
    // the update messages guaranteed
    _t->Send(forceReset.Data(), forceReset.Size());

    _syncDevice = d;
    _syncParams.clear();

    // sync params basically tells the listener what we're looking to sync
    // we will know we're done after all relevant parameters have been removed.
    // this is admittedly somewhat of a hacky way to do this
    for (auto p : d->getParamNames()) {
      if (p == "intensity")
        _syncParams.insert("/eos/out/active/chan");
      else if (p == "color")
        _syncParams.insert("/eos/out/color/hs");
    }

    _syncReady = false;

    if (_syncParams.size() == 0)
      continue;

    stringstream cmd;
    cmd << "/eos/newcmd/chan/" << d->getChannel() << "/Enter";

    char buf[64];
    osc::OutboundPacketStream p(buf, 64);
    p << osc::BeginMessage(cmd.str().c_str()) << osc::EndMessage;
    _t->Send(p.Data(), p.Size());
  }

  // reinit
  rcv.AsynchronousBreak();
  rcvt.join();
  _running = true;
}

void OscPatch::ProcessMessage(const osc::ReceivedMessage & m, const IpEndpointName & remote)
{
  try {
    // looking for particular things in _syncParams
    string pattern = string(m.AddressPattern());
    if (_syncParams.count(pattern) > 0) {
      // if we have what we're looking for
      if (pattern == "/eos/out/active/chan") {
        // intensity, we have to parse. looking for [###]
        auto args = m.ArgumentStream();
        const char* a1;
        args >> a1 >> osc::EndMessage;

        string arg(a1);

        if (arg == "")
          return;

        int start = arg.find_first_of('[');
        int end = arg.find_first_of(']');
        int intens = stoi(arg.substr(start + 1, (end - start - 1)));

        _syncDevice->getIntensity()->setValAsPercent((float)intens / 100.0f);
        _syncParams.erase(pattern);
        Logger::log(LDEBUG, "Updated intensity for " + _syncDevice->getId());
      }
      else if (pattern == "/eos/out/color/hs") {
        // color, we have two float args
        auto args = m.ArgumentStream();
        float h, s;
        args >> h >> s >> osc::EndMessage;

        auto currentHsv = _syncDevice->getColor()->getHSV();
        _syncDevice->getColor()->setHSV(h, s / 100.0f, currentHsv[2]);
        _syncParams.erase(pattern);
        Logger::log(LDEBUG, "Updated color for " + _syncDevice->getId());
      }
    }

    // if we have exhausted all params, signal ready for next device
    if (!_syncReady && _syncParams.size() == 0) {
      _syncReady = true;
    }
  }
  catch (osc::Exception& e) {
    Logger::log(ERR, "Error parsing osc message: " + string(m.AddressPattern()));
  }
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

void OscPatch::deviceToEos(Device * d)
{

  // select the proper channel and intensity at the same time
  char buffer[128];
  osc::OutboundPacketStream packet(buffer, 128);

  stringstream ss;
  ss << "/eos/newcmd/chan/" << d->getChannel() << "/at/" << (int)(d->getIntensity()->asPercent() * 100) << "/enter";
  packet << osc::BeginMessage(ss.str().c_str()) << osc::EndMessage;
  _t->Send(packet.Data(), packet.Size());

  // set the proper values
  for (auto param : d->getRawParameters()) {
    char pbuffer[256];
    osc::OutboundPacketStream p(pbuffer, 256);

    if (param.first == "color") {
      // color maps to RGB color right now.
      p << osc::BeginMessage("/eos/color/rgb");

      auto color = ((LumiverseColor*)(param.second))->getRGB();
      p << (float)color[0] << (float)color[1] << (float)color[2];
    }
    else {
      continue;
    }

    p << osc::EndMessage;

    // clear the command line
    //newEosCmd();

    // first resend the channel command

    // then send the paramer
    _t->Send(p.Data(), p.Size());
  }
}

void OscPatch::newEosCmd()
{
  char buffer[128];
  osc::OutboundPacketStream packet(buffer, 128);
  packet << osc::BeginMessage("/eos/newcmd") << osc::EndMessage;
  _t->Send(packet.Data(), packet.Size());
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