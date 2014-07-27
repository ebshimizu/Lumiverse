#include <string>
#include "LumiverseCore.h"
#include "Cue.h"
#include "CueList.h"
#include "Layer.h"
#include "Playback.h"
#include "Simulation/ArnoldAnimationPatch.h"

using namespace std;
using namespace Lumiverse;

void simulation() {
    Rig rig("/afs/andrew.cmu.edu/usr1/chenxil/Documents/Lumiverse/Lumiverse/data/arnold_photometric_cue.json");
    
    shared_ptr<CueList> list1(new CueList("list1"));
    shared_ptr<Layer> layer1(new Layer(&rig, "layer1", 1));
    
    layer1->setMode(Layer::BLEND_OPAQUE);
    layer1->activate();
    
    Playback pb(&rig);
    pb.attachToRig();
    
    rig.init();
    
    DeviceSet par = rig.query("par1");
    
    par.setParam("intensity", 0.0f);
    
    Cue cue1(&rig, 1.0f, 5.0f);
    list1->storeCue(1, cue1);
    
    par.setParam("intensity", 0.5f);
    
    Cue cue2(&rig);
    //list1->storeCue(2, cue2);
    
    par.reset();
    
    pb.addCueList(list1);
    pb.addLayer(layer1);
    pb.addCueListToLayer("list1", "layer1");
    
    layer1->goToCueAtTime(1, 5);
    //layer1->goToCueAtTime(2, 100);
    
    pb.start();
    rig.run();
    
    while (1) {
        float val;
        rig["par1"]->getParam("intensity", val);
        cout << "par1 Intensity: " << val << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void testArnoldAnimation() {
	Rig rig("/afs/andrew.cmu.edu/usr1/chenxil/Documents/Lumiverse/Lumiverse/data/arnold_photometric_cue.json");
	DeviceSet par1 = rig.query("par1");

	rig.init();
	rig.run();

	this_thread::sleep_for(chrono::seconds(2));

	par1.setParam("intensity", 0.5);

	this_thread::sleep_for(chrono::seconds(6));

	par1.setParam("intensity", 1.8);
    
    this_thread::sleep_for(chrono::seconds(1));
	rig.stop();
	ArnoldAnimationPatch *ap = (ArnoldAnimationPatch*)rig.getSimulationPatch();
	ap->close();
    
    while (1) {
        
    }
    
}

int main(int argc, char**argv) {
  testArnoldAnimation();
    
  return 0;
    
  Rig rig("/afs/andrew.cmu.edu/usr1/chenxil/Documents/Lumiverse/Lumiverse/data/movingLights.json");
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