#include <string>
#include "Device.h"
#include "Patch.h"

#include "DMXInterface.h"
#include "DMXPro2Interface.h"
#include "DMXPatch.h"
using namespace std;

int main(int argc, char**argv) {
  Device test("test", 0, "Source Four ERS");

  test.setParam("intensity", 1.0);

  test.setMetadata("color", "R80");
  test.setMetadata("angle", "back");
  test.setMetadata("area", "4");

  string data;
  test.getMetadata("color", data);

  test.clearMetadataValues();
  test.clearAllMetadata();

  DMXInterface* testInt = new DMXPro2Interface("DMXPRO1");
  _getch();

  DMXPatch patch;
  DMXDevicePatch* dev1 = new DMXDevicePatch(0, 0);
  dev1->addParameter("intensity", 0, conversionType::FLOAT_TO_SINGLE);

  patch.assignInterface(testInt, 0);
  patch.patchDevice(&test, dev1);

  vector<Device *> devices;
  devices.push_back(&test);

  patch.init();
  patch.update(devices);
  patch.dumpUniverses();
  _getch();

  test.setParam("intensity", 0.5);
  patch.update(devices);
  patch.dumpUniverses();
  _getch();
}