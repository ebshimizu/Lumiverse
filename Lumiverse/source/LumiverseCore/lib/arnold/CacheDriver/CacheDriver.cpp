
#include "CacheDriver.h"

namespace {

static void drawToBuffer(float *buffer, const size_t width,
                         const size_t x, const size_t y,
                         const AtRGBA &rgba) {

	buffer[(y * width + x) * 4] = rgba.r;
	buffer[(y * width + x) * 4 + 1] = rgba.g;
	buffer[(y * width + x) * 4 + 2] = rgba.b;
	buffer[(y * width + x) * 4 + 3] = rgba.a;
}
    
/////////////////////////////////
    
AI_DRIVER_NODE_EXPORT_METHODS(SDLDriverMtd);
    
node_loader
{
    if (i > 0)
        return false;
    
    node->methods = (AtNodeMethods*) SDLDriverMtd;
    node->output_type = AI_TYPE_NONE;
    node->name = "cache_buffer";
    node->node_type = AI_NODE_DRIVER;
    strcpy(node->version, AI_VERSION);
    
    return true;
}
    
node_parameters
{
    AiParameterInt("width", 600);
    AiParameterInt("height", 800);
    AiParameterPTR("buffer_pointer", NULL);
	AiParameterPTR("bucket_pos_pointer", NULL);
	AiParameterPTR("progress_pointer", NULL);
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
    // cache driver supports only RGB and RGBA formats
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
	int *progress = (int *)AiNodeGetPtr(node, "progress_pointer");
	int data_width = data_window.maxx - data_window.minx;
	int data_height = data_window.maxy - data_window.miny;

	int num_buckets = ((data_width + bucket_size - 1) / bucket_size) * (
		(data_height + bucket_size - 1) / bucket_size);
	progress[0] = num_buckets;
	progress[1] = 0;
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
	int *bucket_pos = (int *)AiNodeGetPtr(node, "bucket_pos_pointer");
	int *self_pos = bucket_pos + 4 * tid;
	self_pos[0] = bucket_xo;
	self_pos[1] = bucket_yo;
	self_pos[2] = bucket_size_x;
	self_pos[3] = bucket_size_y;
}
    
driver_process_bucket
{
	int *bucket_pos = (int *)AiNodeGetPtr(node, "bucket_pos_pointer");
	int *self_pos = bucket_pos + 4 * tid;

	// Clean up the current bucket (ow, we don't know when the last group finishes)
	self_pos[0] = -1;
	self_pos[1] = -1;
	self_pos[2] = -1;
	self_pos[3] = -1;
}
    
driver_write_bucket
{
    int width = AiNodeGetInt(node, "width");
    int height = AiNodeGetInt(node, "height");
    float *buffer = (float *)AiNodeGetPtr(node, "buffer_pointer");
    
    int         pixel_type;
    const void* bucket_data;
    const char* aov_name;
    
	// Writes to the frame buffer
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

				drawToBuffer(buffer, width, x, y, rgba);
            }
        }

    }

	// Counts the processed buckets
	int *progress = (int *)AiNodeGetPtr(node, "progress_pointer");

	AtCritSec mycs;
	AiCritSecInit(&mycs);
	AiCritSecEnter(&mycs);
		progress[1]++;
	AiCritSecLeave(&mycs);
	AiCritSecClose(&mycs);
}
    
driver_close
{

}
    
node_finish
{
    AiDriverDestroy(node);
}


}
