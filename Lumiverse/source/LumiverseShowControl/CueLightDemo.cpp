#include <string>
#include "LumiverseCore.h"
#include "LumiverseShowControl.h"

#ifdef USE_ARNOLD
#include "Simulation/ArnoldAnimationPatch.h"
#endif

using namespace std;
using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

#ifdef _MSC_VER
std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point loopEnd;
#else
std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> start;
std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> loopEnd;
#endif

float fps;

void getFPS() {
  loopEnd = chrono::high_resolution_clock::now();

  float elapsed = chrono::duration_cast<chrono::milliseconds>(loopEnd - start).count() / 1000.0f;
  fps = 1 / elapsed;
  start = loopEnd;
}

int main(int argc, char**argv) {
	//Logger::setLogLevel(ERR);
  Rig rig("C:/Users/eshimizu/Documents/Lumiverse/Core/Lumiverse/source/LumiverseShowControl/PBridge.rig.json");

  Playback pb(&rig);
  pb.attachToRig();

  shared_ptr<Layer> layer1(new Layer(&rig, &pb, "layer1", 1));

  layer1->setMode(Layer::ALPHA);
  layer1->activate();

  start = chrono::high_resolution_clock::now();
  rig.addFunction(2, std::function<void()>(getFPS));

  rig.init();
  pb.start();

  // Add layer to playback
  pb.addLayer(layer1);
  pb.getProgrammer()->clearAndReset();

  rig.run();

  cout << "Text to Lighting converter. Enter some text and watch the bridge react.\n";
  while (1) {
    cout << ">> ";

    string input;
    getline(cin, input);

		// Create a timeline to work with.
		pb.deleteTimeline("anim");
		shared_ptr<Timeline> anim(new Timeline());
		pb.addTimeline("anim", anim);

		vector<char> chars(input.begin(), input.end());

		size_t time = 0;
		for (const auto& c : chars) {
			// Effect is fade up and fade down to random color in 1 second. Will overlap.
			// Doubles of characters will cause some interesting effects. 

			cout << "Processing: " << c;
			// There should be a way to do this in blind however there is no function to do
			// that yet.
			pb.getProgrammer()->clearAndReset();
			int chanNum1 = ((int)c % 114) + 1;
			int chanNum2 = ((int)c*47 % 114) + 1;
			cout << " (" << chanNum1 << ") (" << chanNum2 << ")\n";


			DeviceSet panel = rig.getChannel(chanNum1);
			panel.add(chanNum2);
			float r = ((float)(rand() % 100)) / 100.0;
			float g = ((float)(rand() % 100)) / 100.0;
			float b = ((float)(rand() % 100)) / 100.0;
      cout << "(" << r << ", " << g << ", " << b << ")\n";
      pb.getProgrammer()->writeToTimeline(panel, anim, time);
			pb.getProgrammer()->setColorRGBRaw(panel, "color", r, g, b, 1);
      pb.getProgrammer()->writeToTimeline(panel, anim, time + 200);
			pb.getProgrammer()->setColorRGBRaw(panel, "color", r, g, b, 0);
      pb.getProgrammer()->writeToTimeline(panel, anim, time + 2000);

			time += 200;
		}

		pb.getProgrammer()->clearAndReset();
		cout << "Show created, playing back...\n";
    getchar();
		pb.getLayer("layer1")->play("anim");

    while (1) {
      std::cout << "FPS: " << fps << "\n";
      this_thread::sleep_for(chrono::milliseconds(100));
    }
  }
}
