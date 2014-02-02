#include <string>
#include "Device.h"
#include "Patch.h"
#include "Rig.h"
#include "DeviceSet.h"

#include "DMX/DMXInterface.h"
#include "DMX/DMXPro2Interface.h"
#include "DMX/DMXPatch.h"
using namespace std;

int main(int argc, char**argv) {
  //Logger::setLogFile("OLLlog.txt");
  
  Rig rig("E:/Users/falindrith/Dropbox/College_Senior/52401/code/OpenLL/OpenLL/data/testRig.json");

  // TODO: (roughly in order of importance)
  // -Sample command line control
  // -Color Mixing and other types
  // -Saving rigs
  // -Robust file reads

  // In OpenLL FX
  // -Presets
  // -Named Groups
  // -Cues / timeline

  // Init rig

  rig.init();
  rig.run();

  DeviceSet channelRange = rig.getChannel(1, 10);
  channelRange = channelRange.remove(5, 7);
  channelRange.setParam("intensity", 1.0f);

  DeviceSet query = rig.query("!$color=R80");

  _getch();

  // get all devices that aren't in a back angle
  DeviceSet myDevices = rig.getAllDevices().remove("angle", "back", false);

  // Do things. Remember that the update loops is only 40Hz and changes aren't instant
  // in computer time
  Sleep(100);
  _getch();

  rig.getDevices("angle", "front", true).add("angle", "front left", true).add("angle", "front right", true).setParam("intensity", 0.75f);

  _getch();
}