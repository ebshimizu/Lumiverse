
#include "Dumiverse.h"
#include <sstream>
#include <fstream>
#include <thread>

/*!
* \brief The height of the result.
*/
int m_height;

/*!
* \brief If turn on (so-called) predictive rendering
*/
bool m_predictive;

/*!
* \brief The directory containing the plugin (buffer_driver).
*/
const char *m_plugins;

/*!
* \brief Pointer to the buckets that will be sent to this Arnold Node
*/
BucketPositionInfo *m_bucket_pos = nullptr;

/*!
* \brief Number of hardware buckets that can be supported on the
* distributed Arnold node.
*/
size_t m_bucket_num;

int m_samples = 1;

/*!
* \brief The pointer to the frame buffer filled by this arnold node's renderer.
*/
float *m_buffer = nullptr;

std::string m_bufDriverName;

/*!
* \brief If the interface is currently open
*/
bool m_open;

ProgressInfo m_progress;

const char *m_filename;

AtNode *driver = nullptr;

/*!
* \brief The gamma for gamma correction.
*/
float m_gamma;

/*!
* \brief The width of the result.
*/
int m_width;

float *getFrameBuffer() {
    printf("Frame buffer: %p\n", m_buffer);
    return m_buffer;
}

int getWidth() {
    return m_width;
}

int getHeight() {
    return m_height;
}

float getPercentage() {
    if( m_progress.bucket_sum < 0 ) {
        return 0.f;
    }

    return (0.f + m_progress.bucket_cur) / (m_progress.bucket_sum * 100.f);
}

void appendToOutputs(const std::string buffer_output) {
    // Append the new output to options (using the new filter)
    AtNode *options = AiUniverseGetOptions();
    AtArray *original = AiNodeGetArray(options, "outputs");
    AtUInt32 num_options = original->nelements + 1;

    AtArray *outputs_array = AiArrayAllocate(num_options, 1, AI_TYPE_STRING);

    for (size_t i = 0; i < num_options - 1; i++) {
        AiArraySetStr(outputs_array, i, AiArrayGetStr(original, i));
    }

    AiArraySetStr(outputs_array, num_options - 1, buffer_output.c_str());
    AiNodeSetArray(options, "outputs", outputs_array);
}

void setLogFileName(std::string filename, int flags)
{
    AiMsgSetLogFileName(filename.c_str());
    AiMsgSetLogFileFlags(flags);
}

void setDriverFileName(std::string base, std::string filename)
{
    AtNode* exr = AiNodeLookUpByName("defaultArnoldDriver@driver_exr.RGBA");
    if (exr != nullptr) {
        AiNodeSetStr(exr, "filename", (base + "/exr/" + filename + ".exr").c_str());
    }

    AtNode* png = AiNodeLookUpByName("defaultArnoldDriver@driver_png.RGBA");
    if (png != nullptr) {
        AiNodeSetStr(png, "filename", (base + "/png/" + filename + ".png").c_str());
    }
}

void init(float m_gamma_arg, bool m_predictive_arg, const char *m_plugins_arg, const char *m_filename_arg) {
    m_gamma = isnan(m_gamma_arg) ? 2.2f : m_gamma_arg;
    m_predictive = m_predictive_arg;
    m_plugins = m_plugins_arg;
    m_filename = m_filename_arg;

    // TODO : to use env var (different apis for linux and win)
    // Make sure your environment variables are set properly to check out an arnold license.

    // Starts a arnold session
    AiBegin();

    setLogFileName("arnold_.log", AI_LOG_ALL);

    // Keeps directory of plugins absolute.
    AiLoadPlugins(m_plugins);

    // Load everything from the scene file
    AiASSLoad(m_filename, AI_NODE_ALL);

    AtNode *options = AiUniverseGetOptions();
    m_width = AiNodeGetInt(options, "xres");
    m_height = AiNodeGetInt(options, "yres");
    m_samples = AiNodeGetInt(options, "AA_samples");

    // Set a driver to output result into a float buffer
    AtNode *driver = AiNode("driver_buffer");

    m_bufDriverName = "buffer_driver";
    std::stringstream ss;
    ss << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
        std::chrono::system_clock::from_time_t(0)).count() % 1000;
    m_bufDriverName = m_bufDriverName.append(ss.str());

    AiNodeSetStr(driver, "name", m_bufDriverName.c_str());
    AiNodeSetInt(driver, "width", m_width);
    AiNodeSetInt(driver, "height", m_height);
    AiNodeSetFlt(driver, "gamma", m_gamma);
    AiNodeSetBool(driver, "predictive", m_predictive);

    // Assume we are using RGBA
    delete[] m_buffer;
    m_buffer = NULL;
    delete[] m_bucket_pos;
    m_bucket_pos = NULL;

    m_buffer = new float[m_width * m_height * 4];
    AiNodeSetPtr(driver, "buffer_pointer", m_buffer);

    // Swapping threads more than hardware supports may cause problem.
    m_bucket_num = std::thread::hardware_concurrency();
    m_bucket_pos = new BucketPositionInfo[m_bucket_num];
    AiNodeSetPtr(driver, "bucket_pos_pointer", m_bucket_pos);

    AiNodeSetPtr(driver, "progress_pointer", &m_progress);

    // Create a filter
    AtNode *filter = AiNode("gaussian_filter");
    AiNodeSetStr(filter, "name", "filter");
    AiNodeSetFlt(filter, "width", 2);

    // Register the driver to the arnold options
    // The function keeps the output options from ass file
    std::string command("RGBA RGBA filter ");
    appendToOutputs(command.append(m_bufDriverName).c_str());

    m_open = true;
}

void close() {
    // Cleans buffer
    delete[] m_buffer;
    m_buffer = NULL;
    delete[] m_bucket_pos;
    m_bucket_pos = NULL;

    // Couple up begin-end would avoid errors caused by a single call to end.
    if (m_open) {

        AiEnd();
    }

    m_open = false;
}

void render() {

    int code;

    // Sets the sampling rate with the current rate
    //setSamplesOption();

    code = AiRender(AI_RENDER_MODE_CAMERA);

    std::stringstream ss;
    ss << "Done: " << code;

    std::ofstream buffer_output("libbuf_out.buf", std::ofstream::binary | std::ofstream::trunc);
    buffer_output.write((const char *)m_buffer, m_width * m_height * 16);
    buffer_output.flush();
    buffer_output.close();

    std::ofstream done_file("render_done.out", std::ofstream::trunc);
    done_file << code;
    done_file.flush();
    done_file.close();
}

int renderWrapper() {

    std::thread render_thread(render);
    render_thread.detach();

    return 0;
}

void interrupt() {
    if (AiRendering()) {
        AiRenderInterrupt();
    }
}

