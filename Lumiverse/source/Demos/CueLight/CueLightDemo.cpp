#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"

using namespace std;

int main(int argc, char**argv) {
  Rig rig("E:/Users/falindrith/Dropbox/College_Senior/52401/code/OpenLL/Lumiverse/data/testRig.json");

  rig.init();
  rig.run();

  rig.query("#1-10").setParam("intensity", 1.0f);
  Cue testCue(&rig, 3.0f, 5.0f);

  rig.query("#1-10").setParam("intensity", 0.75f);
  testCue.update(&rig);
}