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
  test3->setParam("magenta", 0.5);
  test3->setParam("yellow", 0.0);
  test3->setParam("green", 0.0);

  // config patch
  DMXInterface* testInt = new DMXPro2Interface("DMXPRO1");
  DMXPatch* patch = new DMXPatch();
  DMXDevicePatch* dev1 = new DMXDevicePatch(0, 0);
  dev1->addParameter("intensity", 0, conversionType::FLOAT_TO_SINGLE);

  DMXDevicePatch* dev2 = new DMXDevicePatch(6, 0);
  dev2->addParameter("intensity", 0, conversionType::FLOAT_TO_SINGLE);

  DMXDevicePatch* dev3 = new DMXDevicePatch(109, 1);
  dev3->addParameter("cyan", 0, conversionType::FLOAT_TO_SINGLE);
  dev3->addParameter("magenta", 1, conversionType::FLOAT_TO_SINGLE);
  dev3->addParameter("yellow", 2, conversionType::FLOAT_TO_SINGLE);
  dev3->addParameter("green", 3, conversionType::FLOAT_TO_SINGLE);

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
  rig.init();
  rig.run();

  // Do things. Remember that the update loops is only 40Hz and changes aren't instant
  // in computer time
  Sleep(100);
  ((DMXPatch*)rig.getPatch("DMX1"))->dumpUniverses();
  _getch();

  rig["test"]->setParam("intensity", 0.5);
  rig["Seachanger1.1"]->setParam("intensity", 1.0);
  rig["Seachanger1.2"]->setParam("magenta", 0.0);
  Sleep(100);
  ((DMXPatch*)rig.getPatch("DMX1"))->dumpUniverses();

  _getch();

  rig["Seachanger1.2"]->setParam("cyan", 1.0);
  rig["test"]->setParam("intensity", 0.75);

  _getch();

  rig["test"]->clearParamValues();
  rig["Seachanger1.1"]->clearParamValues();
  rig["Seachanger1.2"]->clearParamValues();

  _getch();
}