
#include "ArnoldWindowDriver.h"
#include "types/LumiverseColor.h"

namespace Lumiverse {

ArnoldWindowDriver::ArnoldWindowDriver() {
    
}

// sets up the main window
static bool initializeWindow( int width, int height, const char* title )
{
    // set the caption
    SDL_WM_SetCaption( title, title );
    
    // used the preferred bpp
    unsigned int bits_per_pixel = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
    unsigned int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
    
    // create the window
    SDL_Surface* surface;
    if ( (surface = SDL_SetVideoMode( width, height, bits_per_pixel, flags )) == 0 ) {
        stringstream ss;
        ss << "Error initializing SDL surface: " << SDL_GetError() << ", aborting initialization." << std::endl;
        Logger::log(ERR, ss.str());
        return false;
    }

    return true;
}

void ArnoldWindowDriver::drawToSurface(SDL_Surface *surface, const size_t x, const size_t y,
                                       const AtRGBA &rgba) {
    unsigned char bytes[4];
    unsigned char *pixels = (unsigned char *)surface->pixels;
    
    for (size_t i = 0; i < 4 && i < surface->pitch; i++) {
        sRGBValueToByte(rgba[i], bytes[i]);
        pixels[(y * surface->w + x) * surface->pitch + i] = bytes[i];
    }
}
    
/////////////////////////////////////////////
    
AI_DRIVER_NODE_EXPORT_METHODS(SDLDriverMtd);
    
node_loader
{
    if (i > 0)
        return false;
    
    node->methods = (AtNodeMethods*) SDLDriverMtd;
    node->output_type = AI_TYPE_NONE;
    node->name = "driver_sdl";
    node->node_type = AI_NODE_DRIVER;
    strcpy(node->version, AI_VERSION);
    
    return true;
}
    
node_parameters
{
    AiParameterInt("width", 600);
    AiParameterInt("height", 800);
    AiParameterSTR("title", "Arnold Simulation");
}
    
    
node_initialize
{
    static const char *required_aovs[] = { "FLOAT Z", "FLOAT A", NULL };
        
    // Initialize the driver (set the required AOVs and indicate that
    // we want values at all the depths)
    AiDriverInitialize(node, true, required_aovs);
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
    int width = AiNodeGetInt(node, "width");
    int height = AiNodeGetInt(node, "height");
    const char *title = AiNodeGetStr(node, "title");
    
    initializeWindow(width, height, title);
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
    const char *raw = (const char *)AiDriverGetLocalData(node);
    
    SDL_Surface* surface = SDL_GetVideoSurface();
    
    int         pixel_type;
    const void* bucket_data;
    const char* aov_name;
    
    // Lock surface if needed
    if (SDL_MUSTLOCK(surface))
        if (SDL_LockSurface(surface) < 0)
            return;
    
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
                    rgba.a = 0.f;
                }

                ArnoldWindowDriver::drawToSurface(surface, x, y, rgba);
            }
        }

    }
    
    // Unlock if needed
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
        
    // Tell SDL to update the whole screen
    SDL_UpdateRect(surface, 0, 0, width, height);
    SDL_GL_SwapBuffers();
}
    
driver_close
{

}
    
node_finish
{
    AiDriverDestroy(node);
}


}
