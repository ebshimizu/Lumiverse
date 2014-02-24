#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"
#include "Playback.h"

using namespace std;

int main(int argc, char**argv) {
  Rig rig("E:/Users/falindrith/Dropbox/College_Senior/52401/code/OpenLL/Lumiverse/data/testRig.json");
  CueList list1;
  Playback pb(&rig, 60);

  rig.init();
  rig.run();

  rig.query("#1-10").setParam("intensity", 0.0f);
  list1.storeCue(1,Cue(&rig, 3.0f, 5.0f));

  rig.query("#1-10").setParam("intensity", 1.0f);
  list1.storeCue(2, Cue(&rig));

  getch();

  pb.start();
  pb.goToCue(*list1.getCue(1), *list1.getCue(2));

  while (1) {
    float val;
    rig["s41"]->getParam("intensity", val);
    cout << "s41 Intensity: " << val << "\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}