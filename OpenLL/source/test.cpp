#include <string>
#include "Device.h"
#include "Patch.h"
#include "Rig.h"

#include "DMXInterface.h"
#include "DMXPro2Interface.h"
#include "DMXPatch.h"
using namespace std;

int main(int argc, char**argv) {
  Device* test = new Device("test", 0, "Source Four ERS");

  test->setParam("intensity", 1.0);

  test->setMetadata("color", "R80");
  test->setMetadata("angle", "back");
  test->setMetadata("area", "4");

  string data;
  test->getMetadata("color", data);

  test->clearMetadataValues();
  test->clearAllMetadata();

  // config patch
  DMXInterface* testInt = new DMXPro2Interface("DMXPRO1");
  DMXPatch patch;
  DMXDevicePatch* dev1 = new DMXDevicePatch(0, 0);
  dev1->addParameter("intensity", 0, conversionType::FLOAT_TO_SINGLE);

  patch.assignInterface(testInt, 0);
  patch.patchDevice(test, dev1);

  // Add to rig
  Rig rig;
  rig.addDevice(test);
  rig.addPatch("DMX1", &patch);

  // Init rig
  rig.init();
  rig.run();

  // Do things. Remember that the update loops is only 40Hz and changes aren't instant
  // in computer time
  Sleep(100);
  ((DMXPatch*)rig.getPatch("DMX1"))->dumpUniverses();
  _getch();

  rig["test"]->setParam("intensity", 0.5);
  Sleep(100);
  ((DMXPatch*)rig.getPatch("DMX1"))->dumpUniverses();

  _getch();
}