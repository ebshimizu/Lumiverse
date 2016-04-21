
#include "Dumiverse.h"
#include <sstream>
#include <fstream>
#include <thread>
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

int renderWrapper(const char *jsonDevicesStr) {

    std::thread render_thread(render);
    render_thread.detach();

    return 0;
}

void interrupt() {
    m_patch->forceInterrupt();
}

