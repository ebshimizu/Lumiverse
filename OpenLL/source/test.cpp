#include <string>
#include "Device.h"

#include "DMXInterface.h"
#include "DMXPro2Interface.h"
using namespace std;

int main(int argc, char**argv) {
  Device test("test", 0, "Source Four ERS");

  test.setParam("intensity", 1.0);
  test.setParam("testParam", 1.0);

  test.clearParamValues();

  test.setMetadata("color", "R80");
  test.setMetadata("angle", "back");
  test.setMetadata("area", "4");

  string data;
  test.getMetadata("color", data);

  test.clearMetadataValues();
  test.clearAllMetadata();

  DMXInterface* testInt = new DMXPro2Interface();
  testInt->init();
  _getch();

  unsigned char dmx[512];
  dmx[1] = 124;
  dmx[2] = 156;

  testInt->sendDMX(dmx, 1);
  testInt->sendDMX(dmx, 2);
  _getch();
}