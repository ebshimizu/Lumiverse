#include <string>
#include "Device.h"
#include "Patch.h"
#include "Rig.h"

#include "DMXInterface.h"
#include "DMXPro2Interface.h"
#include "DMXPatch.h"
using namespace std;

int main(int argc, char**argv) {
  Device* test = new Device("test", 1, "Source Four ERS");

  test->setParam("intensity", 1.0);

  test->setMetadata("color", "R80");
  test->setMetadata("angle", "back");
  test->setMetadata("area", "4");

  Device* test2 = new Device("Seachanger1.1", 2, "Source Four ERS");
  test2->setParam("intensity", 1.0);

  Device* test3 = new Device("Seachanger1.2", 2, "Ocean Optics Seachanger");
  test3->setParam("cyan", 0.0);
  test3->setParam("magenta", 0.0);
  test3->setParam("yellow", 0.0);
  test3->setParam("green", 1.0);

  // config patch
  DMXInterface* testInt = new DMXPro2Interface("DMXPRO1");
  DMXPatch* patch = new DMXPatch();

  // Make DMX Maps and stick them into the patch.
  map<string, patchData> dimmerMap;
  dimmerMap["intensity"] = patchData(0, conversionType::FLOAT_TO_SINGLE);

  map<string, patchData> seachangerMap;
  seachangerMap["cyan"] = patchData(0, conversionType::FLOAT_TO_SINGLE);
  seachangerMap["magenta"] = patchData(1, conversionType::FLOAT_TO_SINGLE);
  seachangerMap["yellow"] = patchData(2, conversionType::FLOAT_TO_SINGLE);
  seachangerMap["green"] = patchData(3, conversionType::FLOAT_TO_SINGLE);

  patch->addDeviceMap("dimmer", dimmerMap);
  patch->addDeviceMap("seachanger", seachangerMap);

  // Add the devices with proper mapping to DMX maps
  DMXDevicePatch* dev1 = new DMXDevicePatch("dimmer", 0, 0);
  DMXDevicePatch* dev2 = new DMXDevicePatch("dimmer", 6, 0);
  DMXDevicePatch* dev3 = new DMXDevicePatch("seachanger", 109, 1);

  patch->assignInterface(testInt, 0);
  patch->assignInterface(testInt, 1);
  patch->patchDevice(test, dev1);
  patch->patchDevice(test2, dev2);
  patch->patchDevice(test3, dev3);

  // Add to rig
  Rig rig;
  rig.addDevice(test);
  rig.addDevice(test2);
  rig.addDevice(test3);
  rig.addPatch("DMX1", patch);

  // Init rig
  //rig.init();
  //rig.run();

  // Do things. Remember that the update loops is only 40Hz and changes aren't instant
  // in computer time
  Sleep(100);
  _getch();

  rig["test"]->setParam("intensity", 0.5);
  rig["Seachanger1.1"]->setParam("intensity", 1.0);
  rig["Seachanger1.2"]->setParam("green", 0.0);
  Sleep(100);

  _getch();

  rig["Seachanger1.2"]->setParam("cyan", 1.0);
  rig["test"]->setParam("intensity", 0.75);

  _getch();

  rig["test"]->clearParamValues();
  rig["Seachanger1.1"]->clearParamValues();
  rig["Seachanger1.2"]->clearParamValues();

  _getch();
}