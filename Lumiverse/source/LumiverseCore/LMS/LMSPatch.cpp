#include "LMSPatch.h"

using namespace std;
using namespace LightmanCore;
using namespace LightmanSimulation;

namespace Lumiverse {

LMSPatch::LMSPatch() {

    devicemap = map<string, string>();
}

LMSPatch::LMSPatch(const JSONNode data) {

    loadJSON(data);

}

LMSPatch::~LMSPatch() {

    if (simulation) delete simulation;
    if (workspace)  delete workspace;

}

void LMSPatch::init() {

    // Design issue:
    // It is possible that a patch loading fails. In the case of a LMS patch
    // this would happen if the input file cannot be loaded and hence the
    // workspace cannot be initialized. When a patch init fails we should not
    // allow user code to use tge invalid patch. There should be error handling
    // in init here.

    // create a new workspace
    workspace = new Workspace("Lightman Simulation");

    // load a workspace from file
    workspace->load(input_file.c_str());

    // get components
    compositor = workspace->get_compositor();
    tonemapper = workspace->get_tonemapper();

    // create an simulation
    simulation = new Simulation(workspace);

    // run the simulation
    simulation->run();

}

void LMSPatch::update(set<Device *> devices) {

    Device* device; set<Device*>::iterator d;
    for (d = devices.begin(); d != devices.end(); ++d) {

        device = (Device*) *d;

        // if patched
        auto device_patch = devicemap.find(device->getId());
        if (device_patch != devicemap.end()) {

            string device_id  = (*device_patch).first;
            string layer_name = (*device_patch).second;

            // see if layer is in compositor
            // TODO: this can be done in init actually, store layer pointers
            // directly and leave out the ones that do not map to a valid
            // layer. This should save up a lot of string comparisons and
            // make update faster.
            Layer* layer = compositor->get_layer_by_name(layer_name.c_str());
            if (layer) {

                // convert color
                LumiverseColor *c = device->getColor();
                Eigen::Vector3d rgb = c->getRGB();
                float weight = c->getWeight();

                // set color
                vec3 modulator = weight * vec3(rgb.x(), rgb.y(), rgb.z());
                layer->set_modulator(modulator);
            }

        } else {
            continue;
        }
    }

    return;
}

void LMSPatch::close() {

    // quit simulation
    simulation->quit();

    // close the workspace
    workspace->close();

}

JSONNode LMSPatch::toJSON() {

  JSONNode root;

  // type
  root.push_back(JSONNode("type", getType()));

  // input
  JSONNode inputfile_node = saveInputFile();
  root.push_back(inputfile_node);

  // devicemap
  JSONNode devicemap_node = saveDeviceMap();
  root.push_back(devicemap_node);

  return root;
}

void LMSPatch::patchDevice(string device_id, string layer_name) {

    devicemap.insert(pair<string, string>(device_id, layer_name));

}

void LMSPatch::deleteDevice(string device_id) {

    devicemap.erase(device_id);
}

void LMSPatch::loadJSON(const JSONNode data) {

    string patchName = data.name();

    auto inputfile_node = data.find("input");
    if (inputfile_node != data.end()) loadInputFile(*inputfile_node);

    auto devicemap_node = data.find("devicemap");
    if (devicemap_node != data.end()) loadDeviceMap(*devicemap_node);
}

void LMSPatch::loadInputFile(const JSONNode data) {

    input_file = data.as_string();

}

JSONNode LMSPatch::saveInputFile() {

    JSONNode inputfile_node;
    inputfile_node.push_back(JSONNode("input", input_file.c_str()));

    return inputfile_node;
}

void LMSPatch::loadDeviceMap(const JSONNode data) {

    string device_id, layer_name;

    auto device = data.begin();
    while (device != data.end()) {

        // Note that since we have no information on the rig and the
        // simulator, the user code could create a bund of useless patches
        // for devices that do not exist, or mapping them to layers that
        // do not exist. Though this does is okay when running the rig,
        // as these will be ignored, its still a bad idea to be able to
        // do this. It makes more sense for the user code to only be able
        // to patch devices that actually exist to layers that exist. But
        // this means we will have to expose the internals of the rig and
        // the simulation. And this will also make it more complicated to
        // load from file.

        device_id  = device->name();
        layer_name = data[device_id.c_str()].as_string();
        devicemap.insert(pair<string, string>(device_id, layer_name));

        ++device;
    }
}

JSONNode LMSPatch::saveDeviceMap() {

  JSONNode devicemap_node;
  devicemap_node.set_name("devicemap");
  for (auto dm : devicemap) {
    devicemap_node.push_back(JSONNode(dm.first, dm.second));
  }

  return devicemap_node;
}

void LMSPatch::clearDeviceMap() {

    devicemap.clear();

}


}; // namespace Lumiverse
