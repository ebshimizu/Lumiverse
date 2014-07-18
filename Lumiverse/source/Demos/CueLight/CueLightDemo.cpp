#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"
#include "Layer.h"
#include "Playback.h"

using namespace std;
using namespace Lumiverse;

int main(int argc, char**argv) {
  Rig rig("E:/Users/falindrith/Documents/Programming/Lumiverse/Core/Lumiverse/data/movingLights.json");
  shared_ptr<CueList> list1(new CueList("list1"));
  shared_ptr<Layer> layer1(new Layer(&rig, "layer1", 1));
  shared_ptr<CueList> list2(new CueList("list2"));
  shared_ptr<Layer> layer2(new Layer(&rig, "layer2", 2));

  layer1->setMode(Layer::BLEND_OPAQUE);
  layer1->activate();
  layer2->activate();
  layer2->setOpacity(1);
  Playback pb(&rig);
  pb.attachToRig();

  rig.init();

  DeviceSet inno = rig.query("inno");

  pb.getProgrammer()->setParam("inno", "pan", 0.2f);

  LumiverseColor* color = (LumiverseColor*)rig.getDevice("inno")->getParam("color");
  color->setxy(0.4, 0.4);

  inno.setParam("intensity", 0.0f);
  inno.setParam("shutter", 0.95f);
  inno.setParam("tilt", 0.5f);
  inno.setParam("pan", 0.75f);

  Cue cue1(&rig, 5.0f, 1.0f);
  list1->storeCue(1, cue1);

  color->setxy(0.2, 0.3);
  inno.setParam("intensity", 1.0f);

  Cue cue2(&rig);
  list1->storeCue(2, cue2);

  inno.reset();
  inno.setParam("pan", 0.5f);

  Cue cue3(&rig);
  list2->storeCue(1, cue3);

  // Add cue list to playback
  pb.addCueList(list1);
  pb.addCueList(list2);

  // Add layer to playback
  pb.addLayer(layer1);
  pb.addLayer(layer2);

  // Add cue list to layer
  pb.addCueListToLayer("list1", "layer1");
  pb.addCueListToLayer("list2", "layer2");

  pb.save("E:/Users/falindrith/Documents/Programming/Lumiverse/Core/Lumiverse/data/movingLights_test.json", true);
  return 1;

  layer1->goToCueAtTime(2, 3);

  // Prepare playback
  pb.start();
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
  pb.getProgrammer()->clearAndReset();
  layer1->goToCue(1);
  layer2->goToCue(1);

  this_thread::sleep_for(chrono::seconds(5));

  layer1->go();

  this_thread::sleep_for(chrono::seconds(5));

  while (1) {
    float val;
    rig["inno"]->getParam("intensity", val);
    cout << "inno Intensity: " << val << "\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}