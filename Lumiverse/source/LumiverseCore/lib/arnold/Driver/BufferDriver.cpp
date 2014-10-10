
#include "BufferDriver.h"

namespace {
    
AtMatrix sharp_inv = { 0.8156, 0.0472, 0.1372, 0,
0.3791, 0.5769, 0.0440, 0,
-0.0123, 0.0167, 0.9955, 0,
0, 0, 0, 1 };

AtMatrix display = { 3.2410, -1.5374, -0.4986, 0,
-0.9692, 1.8760, 0.0416, 0,
0.0556, -0.2040, 1.0570, 0,
0, 0, 0, 1};

AtMatrix tone = { 2.0667, - 0.7423, - 0.1193,         0,
- 0.0798,    1.0372, - 0.0090,         0,
- 0.0450, - 0.0974,    1.0509,         0,
0,         0,         0,    1.0000 };

static AtRGBA tone_map(const AtRGBA &rgba, const float gamma, const bool predictive) {
	AtRGBA result = AiRGBACreate(rgba.r, rgba.g, rgba.b, rgba.a);

	/*
	if (predictive) {
		AtVector sharp;
		AtVector out;

		sharp.x = result.r;
		sharp.y = result.g;
		sharp.z = result.b;

		AiM4VectorByMatrixMult(&out, tone, &sharp);

		result = AiRGBACreate(out.x, out.y, out.z, rgba.a);
	}
	*/
	AiRGBAGamma(&result, gamma);
	result = AiRGBAClamp(result, 0.f, 1.f);

	return result;
}

static void drawToBuffer(float *buffer, const float gamma, const size_t width,
                         const size_t x, const size_t y,
                         const AtRGBA &rgba, const bool predictive) { 
	AtRGBA mapped = tone_map(rgba, gamma, predictive);

	for (size_t i = 0; i < 4; i++) {
		buffer[(y * width + x) * 4 + i] = mapped[i];
	}
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
	AiParameterPTR("bucket_pos_pointer", NULL);
	AiParameterPTR("progress_pointer", NULL);
    AiParameterFlt("gamma", 2.2);
	AiParameterBool("predictive", 0);
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
    float gamma = AiNodeGetFlt(node, "gamma");
	bool predictive = AiNodeGetBool(node, "predictive");
    
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

				drawToBuffer(buffer, gamma, width, x, y, rgba, predictive);
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
