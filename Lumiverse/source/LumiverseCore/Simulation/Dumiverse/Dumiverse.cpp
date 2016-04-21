
#include "Dumiverse.h"
#include <sstream>
#include <fstream>
#include <thread>
#include <set>
#include "LumiverseCore.h"

using namespace Lumiverse;

static ArnoldPatch *m_patch;

float *getFrameBuffer() {
    return m_patch->getBufferPointer();
}

int getWidth() {
    return m_patch->getWidth();
}

int getHeight() {
    return m_patch->getHeight();
}

float getPercentage() {
    return m_patch->getPercentage();
}

void init(const char *jsonPatchStr, const char *m_filename_arg) {
    const JSONNode patch = libjson::parse(jsonPatchStr);
    m_patch = new ArnoldPatch(patch);
    m_patch->setAssFile("./tmp_ass/tmp.ass");

    m_patch->init();
}

void close() {
    m_patch->close();

    delete m_patch;
}

void render() {

    bool success = m_patch->renderLoop();

    std::stringstream ss;
    ss << "Done. Successful render: " << success;

    std::ofstream buffer_output("libbuf_out.buf", std::ofstream::binary | std::ofstream::trunc);
    buffer_output.write((const char *)m_patch->getBufferPointer(), getWidth() * getHeight() * 16);
    buffer_output.flush();
    buffer_output.close();

    std::ofstream done_file("render_done.out", std::ofstream::trunc);
    done_file << (success ? 0 : -1);
    done_file.flush();
    done_file.close();
}

int renderWrapper(const char *jsonDevicesStr, const char *jsonSettingsStr) {

    // Update devices
    const JSONNode jsonDevices = libjson::parse(jsonDevicesStr);
    std::set<Device *> devices;
    for( auto i = jsonDevices.begin(); i != jsonDevices.end(); i++ ) {
        std::string nodeName = i->name();
        Device *device = new Device(nodeName, *i);
        devices.insert(device);
    }

    m_patch->update(devices);

    for( Device *device : devices ) {
        delete device;
    }

    // Update settings
    const JSONNode jsonSettings = libjson::parse(jsonDevicesStr);
    int new_width = -1, new_height = -1;
    for( auto i = jsonSettings.begin(); i != jsonSettings.end(); i++ ) {
        float val = i->as_float();
        std::string name = i->name();
        if( strcmp(name.c_str(), "width") == 0 ) {
            new_width = (int)val;
        } else if( strcmp(name.c_str(), "height") == 0 ) {
            new_height = (int)val;
        }
        m_patch->setOptionParameter(name, val);
    }

    if( (new_width != -1) && (new_height != -1) ) {
        m_patch->setDims(new_width, new_height);
    } else if( new_width != -1 ) {
        m_patch->setDims(new_width, getHeight());
    } else if( new_height != -1 ) {
        m_patch->setDims(getWidth(), new_height);
    }

    std::thread render_thread(render);
    render_thread.detach();

    return 0;
}

void interrupt() {
    m_patch->forceInterrupt();
}

