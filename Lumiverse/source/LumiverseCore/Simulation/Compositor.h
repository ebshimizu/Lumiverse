#ifndef Lumiverse_COMPOSITOR_H
#define Lumiverse_COMPOSITOR_H

#include <unordered_map>
#include <string>

#include "EXRLayer.h"

namespace Lumiverse {

/**
 * Composes layers and generates output images.
 * Also responsible of managing the layers. Note that layers in a
 * compositor should have the same size. A compositor has no size
 * information when it is created, and the size will be set as the
 * first layer is added. Attempts to add layers of inconsistent
 * sizes will be ignored.
 *
 * Credit to Sky Gao for writing most of this code
 */
class Compositor {
public:
  /**
   * Constructor.
   * Creates a new lightman compositor instance.
   */
  Compositor();

  /**
   * Destructor.
   * Destroys the lightman compositor instance, freeing all assets used.
   */
  ~Compositor();

  /**
   * Get the width of the compositor.
   * @return the width of the compositor (0 when compositor is empty)
   */
  size_t get_width();

  /**
   * Get the height of the compositor.
   * @return the height of the compositor (0 when compositor is empty)
   */
  size_t get_height();

  /**
   * Gets the set of layers in the compositor.
   * @return the set of all layers in the compositor.
   */
  std::unordered_map<std::string, EXRLayer *>& get_layers();

  /**
   * Add a new render layer to the compositor.
   * Ignored if the layer is invalid or inconsistent in size
   * @param layer Pointer to the new layer.
   */
  void add_layer(EXRLayer *layer);

  /**
   * Removes a layer from the compositor.
   * Ignored if the layer is not in the scene.
   * @param layer Pointer to the layer to delete.
   */
  void del_layer(EXRLayer *layer);

  /**
   * Find a layer in the scene by its name.
   * @param layer_name Name of the layer to search for.
   * @return Pointer to the layer object if index is valid.
             NULL if index is invalid.
   */
  EXRLayer *get_layer_by_name(const char *layer_name);

  /**
   * Find a layer in the scene by its name.
   * Ignored if the layer name is invalid.
   * @param layer_name Name of the layer to search for.
   */
  void del_layer_by_name(const char *layer_name);

  /**
   * Set the output buffer of the compositor.
   * Applications may specify an output buffer that the compositor
   * writes the result to. If this is never set, the output will be
   * saved in the compositor and will only be used by internally.
   */
  void set_output_buffer(Pixel3 *buffer);

  /**
   * Render the scene.
   * Updates the frame buffer based on the the current state of the scene.
   */
  void render();

  /*!
  \brief Get the buffer of composed pixels (i.e. the composition of
  all of the exr layers)
  */
  Pixel3 *get_compose_buffer() { return compose_buffer; };

private:
  /**
   * Width.
   * The width of the compositor's outputs in pixels.
   */
  size_t w;

  /**
   * Height.
   * The height of the compositor's outputs in pixels.
   */
  size_t h;

  /**
   * EXRLayers.
   */
  std::unordered_map<std::string, EXRLayer *> layers;

  /**
   * The internal composition buffer.
   * The internal composition buffer that holds the result of the most
   * recent composition. When no application specified output buffer
   * is set, results are saved in the composition buffer.
   */
  Pixel3 *compose_buffer;
};

}; // namespace Lumiverse

#endif // Lumiverse_COMPOSITOR_H
