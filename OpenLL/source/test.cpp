#include <string>
#include "Device.h"
#include "Patch.h"
#include "Rig.h"
#include "DeviceSet.h"

#include "DMXInterface.h"
#include "DMXPro2Interface.h"
#include "DMXPatch.h"
using namespace std;

int main(int argc, char**argv) {
  Rig rig("C:/Users/Falindrith/Dropbox/College_Senior/52401/code/OpenLL/OpenLL/data/testRig.json");

  // TODO: (roughly in order of importance)
  // -Sample command line control system
  // -Color Mixing and other types
  // -Saving rigs
  // -Robust file reads

  // Init rig
  rig.init();
  rig.run();

  // get all devices that aren't in a back angle
  DeviceSet myDevices = rig.getAllDevices().remove("angle", "back", false);

  // Do things. Remember that the update loops is only 40Hz and changes aren't instant
  // in computer time
  Sleep(100);
  _getch();

  rig.getDevices("angle", "front", true).add("angle", "front left", true).add("angle", "front right", true).setParam("intensity", 0.75f);

  _getch();
}