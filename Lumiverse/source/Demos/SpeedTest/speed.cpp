#include <string>
#include "LumiverseCoreConfig.h"
#include "LumiverseCore.h"
#include "LumiverseShowControl.h"

using namespace std;
using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> start;
std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> loopEnd;
float fps;

void getFPS() {
  loopEnd = chrono::high_resolution_clock::now();

  float elapsed = chrono::duration_cast<chrono::milliseconds>(loopEnd - start).count() / 1000.0f;
  fps = 60 / elapsed;
  start = loopEnd;
}

int main(int argc, char**argv) {
  Logger::setLogLevel(ERR);
  Rig* rig = new Rig("../../../data/movingLightsStress.rig.json");
  Playback* pb = new Playback(rig);
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 1", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 2", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 3", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 4", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 5", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 6", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 7", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 8", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 9", 1)));
  pb->addLayer(shared_ptr<Layer>(new Layer(rig, pb, "layer 10", 1)));

  start = chrono::high_resolution_clock::now();

  rig->addFunction(2, std::function<void()>(getFPS));
  rig->init();
  pb->attachToRig();
  pb->start();
  rig->run();

  while (1) {
    std::cout << "FPS: " << fps << "\n";
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}