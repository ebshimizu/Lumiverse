#include "LumiverseCore.h"

#include <string>
#include <ctime>

using namespace std;
using namespace Lumiverse;

/*
 * The code below is the simple Lumiverse application that turns on panels on
 * the Pausch bridge one at a time in order. The same code was used to collect
 * samples of each of the light panel that Lightman needs for relighting.
 */

typedef enum { DEVICE, PANEL } SampleMode;

void sample_devices(DeviceSet device_set, SampleMode mode) {
  set<Device *> devices = device_set.getDevices();
  Device *device;

  switch (mode) {
    case PANEL:

      // turn on all devices in the panel
      for (set<Device *>::iterator d = devices.begin(); d != devices.end();
           ++d) {
        device = (Device *)*d;

        // turn on decice and set to white
        device->getColor()->setRGBRaw(1, 1, 1, 1);
      }

      // hold till shutter releasd
      getchar();

      // turn off panel
      for (set<Device *>::iterator d = devices.begin(); d != devices.end();
           ++d) {
        device = (Device *)*d;

        // turn off device
        device->getColor()->setWeight(0);
      }

      break;

    case DEVICE:

      // turn on devices one at a time
      for (set<Device *>::iterator d = devices.begin(); d != devices.end();
           ++d) {
        device = (Device *)*d;

        // turn on decice and set to white
        device->getColor()->setRGB(1, 1, 1, 1);

        // hold till shutter released
        sleep(1);

        // turn off device
        device->getColor()->setWeight(0);
      }

      break;

    default:
      return;
  }
}

void sample_rig(Rig &rig, SampleMode mode) {
  set<Device *> devices = rig.getAllDevices().getDevices();

  // turn off all devices and record all panels
  Device *device;
  std::set<int> ids;
  for (set<Device *>::iterator d = devices.begin(); d != devices.end(); ++d) {
    device = (Device *)*d;

    // turn off device
    device->getColor()->setWeight(0);

    // get all panel id's
    ids.insert(stoi(device->getMetadata("panel")));
  }

  // sample each panel
  set<int>::iterator id = ids.begin();
  while (id != ids.end()) {
    // select matching device
    sample_devices(rig.getDevices("panel", to_string(*id), true), mode);
    ++id;
  }
}

/*
 * This code below used to create the simulation patch for the bridge that is
 * now saved within the rig.
 */
LMSPatch gen_patch(Rig &rig) {
  LMSPatch patch = LMSPatch();
  set<Device *> devices = rig.getAllDevices().getDevices();

  // group into panels
  std::set<int> ids;
  Device *device;
  set<Device *>::iterator d;
  for (d = devices.begin(); d != devices.end(); ++d) {
    device = (Device *)*d;
    ids.insert(stoi(device->getMetadata("panel")));
  }

  // patch devices by panel
  int panel_id;
  set<int>::iterator id = ids.begin();
  while (id != ids.end()) {
    // get panel id string representation
    panel_id = *id;

    // get layer they match to
    string panel_name;
    panel_name = to_string(panel_id);
    panel_name = panel_id < 10 ? "0" + panel_name : panel_name;
    panel_name = "panel_" + panel_name;

    // get devices on panel
    set<Device *> devices =
        rig.getDevices("panel", to_string(*id), true).getDevices();

    // patch all devices to the same panel layer
    Device *device;
    set<Device *>::iterator d;
    for (d = devices.begin(); d != devices.end(); ++d) {
      // patch device
      device = (Device *)*d;
      string device_id = device->getId();
      patch.patchDevice(device_id, panel_name);
    }

    // move on to next panel
    ++id;
  }

  return patch;
}

/*
 * fancy rainbow animation
 */
void rainbow(Rig &rig) {
  set<Device *> devices = rig.getAllDevices().getDevices();

  // set Initialial color
  Device *device;
  std::set<int> ids;
  for (set<Device *>::iterator d = devices.begin(); d != devices.end(); ++d) {
    device = (Device *)*d;

    // turn off device
    device->getColor()->setWeight(0);

    // get all panel id's
    ids.insert(stoi(device->getMetadata("panel")));
  }

  // convert to vector
  std::vector<int> panels;
  std::copy(ids.begin(), ids.end(), std::back_inserter(panels));

  // compute initial pivots
  int range = panels.size() / 3; // number of panels each channel covers
  int range2 = range * 2;        // for wrapping around
  int range3 = range * 3;        // for wrapping around
  float step = 1.0f / range;     // diff between two panels

  // assign initialial color to each panel
  int r_pivot = range;
  int g_pivot = range * 2;
  int b_pivot = range * 3;
  for (int p = 0; p < panels.size(); ++p) {
    DeviceSet deviceset = rig.getDevices("panel", to_string(panels[p]), true);
    set<Device *> devices = deviceset.getDevices();
    float r = 1.0f - std::max(0.0f, (p < range && r_pivot > range2 ? p : abs(r_pivot - p)) * step);
    float g = 1.0f - std::max(0.0f, (p < range && g_pivot > range2 ? p : abs(g_pivot - p)) * step);
    float b = 1.0f - std::max(0.0f, (p < range && b_pivot > range2 ? p : abs(b_pivot - p)) * step);
    for (auto d = devices.begin(); d != devices.end(); ++d) {
      device = (Device *)*d;
      device->getColor()->setRGBRaw(r, g, b, 1);
    }
  }

  // run animation
  std::clock_t start;
  double duration = 0;
  start = std::clock();
  while(1) {
    //r_pivot = ((int) (r_pivot + duration)) % range3;
    //g_pivot = ((int) (g_pivot + duration)) % range3;
    //b_pivot = ((int) (b_pivot + duration)) % range3;
    for (int p = 0; p < panels.size(); ++p) {
      DeviceSet deviceset = rig.getDevices("panel", to_string(panels[p]), true);
      set<Device *> devices = deviceset.getDevices();
      float r = 1.0f - std::max(0.0f, (p < range && r_pivot > range2 ? p : abs(r_pivot - p)) * step);
      float g = 1.0f - std::max(0.0f, (p < range && g_pivot > range2 ? p : abs(g_pivot - p)) * step);
      float b = 1.0f - std::max(0.0f, (p < range && b_pivot > range2 ? p : abs(b_pivot - p)) * step);
      for (auto d = devices.begin(); d != devices.end(); ++d) {
        device = (Device *)*d;
        device->getColor()->setRGBRaw(r, g, b, 1);
      }
    }
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  }
}

/*
 * print usage
 */
void print_help() {
  fprintf(
      stdout,
      "./BridgeDemo <path_to_rig>\n"
      " - use PBridge/PBridge.rig.json     to run on Pausch Bridge\n"
      " - use PBridge/PBridgeSim.rig.json  to run simulation with Lightman\n");
}

/*
 * if the loaded one is the DMX patch for the bridge, the app will be running
 * on the bridge, if the one loaded is the lightman simulation path, it will
 * run as a lightman simulation.
 */
int main(int argc, char **argv) {
  // load path -
  if (argc != 2) {
    print_help();
    return 0;
  }

  // load bridge simulation rig
  Rig PBrig(argv[1]);

  // create and add bridge simulation patch - saved in rig already
  // LMSPatch PBpatch = gen_patch(PBrig);
  // PBrig.addPatch("PBridge", &PBpatch);

  // Initialize the rig
  PBrig.init();  // Huh? Shoudn't this call patch init?

  // Run the rig
  PBrig.run();

  // sample each device in the rig
  // sample_rig(PBrig, PANEL);

  // fancy rainbow
  rainbow(PBrig);

  // hold
  getchar();
}
