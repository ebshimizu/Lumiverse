#include <string>
#include "LumiverseCore.h"

using namespace std;
using namespace Lumiverse;

int main(int argc, char**argv)
{
  Rig* r = new Rig();

  Device* arnoldLight = new Device("left", 1, "spot_light");
  arnoldLight->setMetadata("Arnold Node Name", "leftShape1");
  arnoldLight->setParam("intensity", new LumiverseFloat(8000, 0, 100000, 0));
  LumiverseColor* color = new LumiverseColor(ColorMode::BASIC_RGB);
  color->setHSV(28, 1, 0.753);
  arnoldLight->setParam("color", color);
  arnoldLight->setParam("polar", new LumiverseOrientation(55, DEGREE, 0, 180, -180));
  arnoldLight->setParam("azimuth", new LumiverseOrientation(-30, DEGREE, 0, 360, -360));
  arnoldLight->setParam("distance", new LumiverseFloat(40, 1, 10000, 0));
  arnoldLight->setParam("lookAtX", new LumiverseFloat(0, 0, 1000, -1000));
  arnoldLight->setParam("lookAtY", new LumiverseFloat(66.894, 66.894, 1000, -1000));
  arnoldLight->setParam("lookAtZ", new LumiverseFloat(15.855, 15.855, 1000, -1000));

  Device* arnoldLight2 = new Device("right", 1, "spot_light");
  arnoldLight2->setMetadata("Arnold Node Name", "|right|rightShape");
  arnoldLight2->setParam("intensity", new LumiverseFloat(2000, 0, 10000, 0));
  LumiverseColor* color2 = new LumiverseColor(ColorMode::BASIC_RGB);
  color2->setHSV(204, 0.758, 0.753);
  arnoldLight2->setParam("color", color2);
  arnoldLight2->setParam("polar", new LumiverseOrientation(55, DEGREE, 0, 180, -180));
  arnoldLight2->setParam("azimuth", new LumiverseOrientation(45, DEGREE, 0, 360, -360));
  arnoldLight2->setParam("distance", new LumiverseFloat(40, 1, 10000, 0));
  arnoldLight2->setParam("lookAtX", new LumiverseFloat(0, 0, 1000, -1000));
  arnoldLight2->setParam("lookAtY", new LumiverseFloat(66.894, 66.894, 1000, -1000));
  arnoldLight2->setParam("lookAtZ", new LumiverseFloat(15.855, 15.855, 1000, -1000));

  Device* arnoldLight3 = new Device("rim", 1, "spot_light");
  arnoldLight3->setMetadata("Arnold Node Name", "rimShape");
  arnoldLight3->setParam("intensity", new LumiverseFloat(2000, 0, 10000, 0));
  LumiverseColor* color3 = new LumiverseColor(ColorMode::BASIC_RGB);
  color3->setHSV(177, 0.198, 1);
  arnoldLight3->setParam("color", color3);
  arnoldLight3->setParam("polar", new LumiverseOrientation(-10, DEGREE, 0, 180, -180));
  arnoldLight3->setParam("azimuth", new LumiverseOrientation(0, DEGREE, 0, 360, -360));
  arnoldLight3->setParam("distance", new LumiverseFloat(20, 1, 10000, 0));
  arnoldLight3->setParam("lookAtX", new LumiverseFloat(0, 0, 1000, -1000));
  arnoldLight3->setParam("lookAtY", new LumiverseFloat(66.894, 66.894, 1000, -1000));
  arnoldLight3->setParam("lookAtZ", new LumiverseFloat(15.855, 15.855, 1000, -1000));

  r->addDevice(arnoldLight);
  r->addDevice(arnoldLight2);
  r->addDevice(arnoldLight3);

  ArnoldAnimationPatch* arnold = new ArnoldAnimationPatch();
  arnold->getArnoldInterface()->setPluginDirectory("C:/solidangle/mtoadeploy/2016/shaders;C:/Users/falindrith/Documents/Lumiverse/Core/Lumiverse/build/LumiverseCore/lib/arnold/Driver/Debug");
  arnold->getArnoldInterface()->setAssFile("C:/Users/falindrith/OneDrive/Documents/research/attributes_project/scenes/test/test.ass");
  arnold->setSamples(0);
  r->addPatch("arnold", arnold);

  r->init();
  r->updateOnce();

  arnold->renderSingleFrame(r->getDeviceRaw(), "C:/Users/falindrith/OneDrive/Documents/research/attributes_project/scenes", "test");
}


void test() {
  // Creates an empty rig.
  Rig rig("C:/Users/eshimizu/Documents/Lumiverse/Core/Lumiverse/data/Jules/MLData.rig.json");

  // Select a single ID
  DeviceSet singleID = rig.select("fresnel_E1_2");
  cout << singleID.info() << "\n";

  // Select multiple IDs
  DeviceSet multipleIDs = rig.select("fresnel_E1_2, s4_E_7");
  cout << multipleIDs.info() << "\n";

  // Select a channel range
  // Select a single channel with #[number], for example rig.select("#10")
  DeviceSet channels = rig.select("#1-10");
  cout << channels.info() << "\n";

  // Select using metadata
  DeviceSet area1 = rig.select("$Area=1");
  cout << area1.info() << "\n";

  // Select using metadata starts with
  DeviceSet startswith = rig.select("$filename^=S4 Fresnel");
  cout << startswith.info() << "\n";

  // Select all lights in area 2 with gel L201
  DeviceSet areaFilter = rig.select("$Area=2[$gel=L201]");
  cout << areaFilter.info() << "\n";

  // Select all lights in area 2 with gel L201 or L135
  DeviceSet areaFilter2 = rig.select("$Area=2[$gel=L201|$gel=L135]");
  cout << areaFilter2.info() << "\n";

  getchar();
}