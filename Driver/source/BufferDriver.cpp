
#include "BufferDriver.h"

namespace {

static float clamp(float f, float min, float max) {
    if (f < min) return min;
    if (f > max) return max;
    
    return f;
}
    
static void drawToBuffer(float *buffer, const float gamma, const size_t width,
                         const size_t x, const size_t y,
                         const AtRGBA &rgba) {
    //unsigned char bytes[4];
        
    for (size_t i = 0; i < 4; i++) {
        //sRGBValueToByte(rgba[i], bytes[i]);
        if (i != 3)
            buffer[(y * width + x) * 4 + i] = clamp(std::powf(rgba[i], 1 / gamma), 0.f, 1.f);
        else
            buffer[(y * width + x) * 4 + i] = rgba[i];
    }
    /*
    if (x == 0 && y < 10)
        printf("@@@  %zu, %zu: %f, %f, %f, %f\n", x, y, buffer[(y * width + x) * 4 + 0], buffer[(y * width + x) * 4 + 1],
               buffer[(y * width + x) * 4 + 2], buffer[(y * width + x) * 4 + 3]);
     */
}
    
/////////////////////////////////
    
AI_DRIVER_NODE_EXPORT_METHODS(SDLDriverMtd);
    
node_loader
{
    if (i > 0)
        return false;
    
    node->methods = (AtNodeMethods*) SDLDriverMtd;
    node->output_type = AI_TYPE_NONE;
    node->name = "driver_buffer";
    node->node_type = AI_NODE_DRIVER;
    strcpy(node->version, AI_VERSION);
    
    return true;
}
    
node_parameters
{
    AiParameterInt("width", 600);
    AiParameterInt("height", 800);
    AiParameterPTR("buffer_pointer", NULL);
    AiParameterFlt("gamma", 2.2);
}
    
    
node_initialize
{
    // Initialize the driver (set the required AOVs and indicate that
    // we want values at all the depths)
    AiDriverInitialize(node, true, NULL);
}
    
node_update
{

}
    
driver_supports_pixel_type
{
    // this driver will support RGB and RGBA formats
    switch (pixel_type)
    {
        case AI_TYPE_RGBA:
        case AI_TYPE_RGB:
            return true;
        default:
            return false;
    }
}
    
driver_open
{

}
    
driver_extension
{
    return NULL;
}
    
driver_needs_bucket
{
    return true;
}
    
driver_prepare_bucket
{
    // Could add interactive features here.
}
    
driver_process_bucket
{

}
    
driver_write_bucket
{
    int width = AiNodeGetInt(node, "width");
    int height = AiNodeGetInt(node, "height");
    float *buffer = (float *)AiNodeGetPtr(node, "buffer_pointer");
    float gamma = AiNodeGetFlt(node, "gamma");
    
    int         pixel_type;
    const void* bucket_data;
    const char* aov_name;
    
    while (AiOutputIteratorGetNext(iterator, &aov_name, &pixel_type, &bucket_data))
    {
        size_t x, y;
        
        for (int j = 0; j < bucket_size_y; ++j) {
            for (int i = 0; i < bucket_size_x; ++i) {
                y = j + bucket_yo;
                x = i + bucket_xo;
                size_t in_idx = j * bucket_size_x + i;
                
                if (x >= width || y >= height)
                    continue ;
                
                AtRGBA rgba;
                if (pixel_type == AI_TYPE_RGBA) {
                    rgba = ((AtRGBA*)bucket_data)[in_idx];
                }
                else if (pixel_type == AI_TYPE_RGB) {
                    AtRGB src = ((AtRGB*)bucket_data)[in_idx];
                    
                    rgba.r = src.r;
                    rgba.g = src.g;
                    rgba.b = src.b;
                    rgba.a = 1.f;
                }

                drawToBuffer(buffer, gamma, width, x, y, rgba);
            }
        }

    }
    
}
    
driver_close
{

}
    
node_finish
{
    AiDriverDestroy(node);
}


}
