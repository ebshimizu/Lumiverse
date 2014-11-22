#include <string>
#include "LumiverseCoreConfig.h"
#include "LumiverseCore.h"

using namespace std;
using namespace Lumiverse;

int main(int argc, char**argv) {
  // Logger::setLogFile("OLLlog.txt");
  
	AiLicenseSetServer("pike.graphics.cs.cmu.edu", 5053);

	// Starts a arnold session
	AiBegin();
	AiMsgSetLogFileName("J:/Lumiverse/Lumiverse/data/Jules/LOG");
	AiMsgSetLogFileFlags(AI_LOG_ALL);

	// Keeps directory of plugins absolute.
	AiLoadPlugins("J:/LumiverseInstall/lib/arnold/plugin;C:/lib/Lumiverse/lib/arnold/plugin;C:/solidangle/mtoadeploy/2015/shaders/");

	// Doesn't read light node and filter node from the ass file
	AiASSLoad("C:/Users/chenxil/Desktop/test_gobo.ass",
		AI_NODE_ALL);
	AtNode *light_ptr = AiNodeLookUpByName("aiPhotometricLightShape1");
	AtNode *gobo = AiNode("photometric_gobo");
	std::string name(AiNodeGetStr(light_ptr, "name"));
	AiNodeSetStr(gobo, "name", (name + "_gobo").c_str());

	AiNodeSetStr(gobo, "filename", "J:/gobo/000202_L.jpg");
	AiNodeSetFlt(gobo, "degree", 25);
	AiNodeSetFlt(gobo, "rotation", 90);

	AiNodeSetPtr(light_ptr, "filters", gobo);
	/*
	AtArray *outputs_array = AiArrayAllocate(1, 1, AI_TYPE_POINTER);
	AiArraySetPtr(outputs_array, 1, gobo);
	AiNodeSetArray(light_ptr, "filters", outputs_array);
	*/
	AiRender();

	AiEnd();

	return 0;

  auto color = ColorUtils::normalizeRGB(ColorUtils::convXYZtoRGB(ColorUtils::getXYZTemp(2856)));
  cout << color;
  getch();
  return 1;

  Rig rig("/afs/andrew.cmu.edu/usr1/chenxil/Documents/Lumiverse/Lumiverse/data/arnold_photometric.json");

  // Init rig
  rig.init();

  // Test extra funcs
  // rig.addFunction([]() { cout << "Testing additional functions\n"; });

  rig.run();

    bool flag = true;
  while (1) {
    Device *par1 = rig.getDevice("par1");
      LumiverseColor *par_color = (LumiverseColor *)par1->getParam("color");
      
      if (flag) {
          printf("%f, %f, %f\n", par_color->getRGB()[0], par_color->getRGB()[1], par_color->getRGB()[2]);
          flag = false;
      }
  }
}
