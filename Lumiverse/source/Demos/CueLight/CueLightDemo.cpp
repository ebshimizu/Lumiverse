#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"

using namespace std;

int main(int argc, char**argv) {
  Rig rig("E:/Users/falindrith/Dropbox/College_Senior/52401/code/OpenLL/Lumiverse/data/testRig.json");
  CueList list1;

  rig.init();
  rig.run();

  rig.query("#1-10").setParam("intensity", 1.0f);
  list1.storeCue(1,Cue(&rig, 3.0f, 5.0f));

  rig.query("#1-10").setParam("intensity", 0.75f);
  list1.storeCue(2, Cue(&rig));

  rig.query("#11-20").setParam("intensity", 1.0f);
  list1.storeCue(3, Cue(&rig));

  rig.query("#1-20").setParam("intensity", 0.0f);
  list1.storeCue(4, Cue(&rig));

  rig.query("#1-10").setParam("intensity", 0.5f);
  list1.update(2, &rig, true);
}