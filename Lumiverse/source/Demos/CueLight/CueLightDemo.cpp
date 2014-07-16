#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"
#include "Layer.h"
#include "Playback.h"

using namespace std;

int main(int argc, char**argv) {
  Rig rig("E:/Users/falindrith/Documents/Programming/Lumiverse/Core/Lumiverse/data/movingLights.json");
  shared_ptr<CueList> list1(new CueList());
  shared_ptr<Layer> layer1(new Layer(&rig, "layer1", 1));
  layer1->setMode(Layer::BLEND_OPAQUE);
  layer1->activate();
  Playback pb(&rig, 60);

  // Bind update function to rig update function
  rig.addFunction([&]() { pb.update(); });

  rig.init();

  DeviceSet inno = rig.query("inno");

  LumiverseColor* color = (LumiverseColor*)rig.getDevice("inno")->getParam("color");
  color->setxy(0.4, 0.4);

  inno.setParam("intensity", 0.0f);
  inno.setParam("shutter", 0.95f);
  inno.setParam("tilt", 0.5f);

  Cue cue1(&rig, 5.0f, 1.0f);
  list1->storeCue(1, cue1);

  color->setxy(0.2, 0.3);
  inno.setParam("intensity", 1.0f);

  Cue cue2(&rig);
  list1->storeCue(2, cue2);

  // Add cue list to playback
  pb.addCueList("list1", list1);

  // Add layer to playback
  pb.addLayer(layer1);

  // Add cue list to layer
  pb.addCueListToLayer("list1", "layer1");

  // If this line isn't present, the update loop runs slowly.
  // This only happens in the MSVC 13 Debug build with no profiling.
  // If you profile it, it stops running slowly.
  // layer1->goToCue(1);

  // Prepare playback
  pb.run();
  rig.run();

  // Test keyframe insertion
  //DeviceSet chan1 = rig.query("#1");
  //chan1.setParam("intensity", 0.0f);
  //list1.getCue(1)->insertKeyframe(4, chan1);
  //list1.getCue(1)->insertKeyframe(5, chan1);

  //chan1.setParam("intensity", 1.0f);
  //list1.getCue(1)->insertKeyframe(4.5f, chan1);

  // Test keyframe overwrite
  //chan1.setParam("intensity", 0.0f);
  //list1.getCue(1)->insertKeyframe(4.5f, chan1);

  getchar();

  layer1->goToCue(1);

  this_thread::sleep_for(chrono::seconds(5));

  layer1->go();

  while (1) {
    float val;
    rig["inno"]->getParam("intensity", val);
    cout << "inno Intensity: " << val << "\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}