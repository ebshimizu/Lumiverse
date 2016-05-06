#ifndef LUMIVERSE_TONEMAPPER_H
#define LUMIVERSE_TONEMAPPER_H

#include "EXRLayer.h"

#ifdef USE_ARNOLD_CACHING

namespace Lumiverse {

/**
 * Compresses the dynamic range and maps illuminance to colors.
 * The tone mapper converts the illuminance-based pixels of the
 * HDR layer to standard 8-bit RGB colors and outputs clamped RGB bitmap.
 * The default tone mapper does only gamma correction and exposure adjustment.
 *
 * Credit is due to Sky Gao for writing most of this code.
 */
class ToneMapper {
public:
  /*!
   * Constructor.
   */
  ToneMapper();

  /*!
   * Destructor.
   */
  ~ToneMapper();

  /*!
   * \brief Get the tone mapping algorithm.
   * \return the name of tone mapping algorithm implemented.
   */
  std::string get_type();

  /*!
   * \brief Get display gamma.
   * \return the display gamma.
   */
  float get_gamma();

  /*!
   * \brief Set display gamma.
   * \param gamma the display gamma to be set.
   */
  void set_gamma(float gamma);

  /*!
   * \brief Get exposure level adjustment.
   * \return the exposure level adjustment (in F-stops).
   */
  float get_level();

  /*!
   * \brief Set exposure level adjustment.
   * \param level the exposure level adjustment to be set (in F-stops).
   */
  void set_level(float level);

  /*!
   * \brief Set the input of the tonemapper.
   * \param buffer pointer to the Pixel buffer to be set as input.
   * \param w width of the input.
   * \param h height of the input.
   */
  void set_input(Pixel4 *buffer, size_t w, size_t h);

  /*!
   * \brief Set the HDR Pixel output buffer of the tonemapper.
   * \param buffer pointer to the Pixel4 buffer to be set as output.
   */
  void set_output_hdr(float *buffer);

  /*!
   * \brief Set the bitmap output buffer of the tonemapper.
   * \param buffer pointer to the Pixel4 buffer to be set as output.
   */
  void set_output_bmp(char *buffer);

  /*!
   * \brief Compresses the dynamic range only, and save the result to
   * the HDR output buffer. If the output buffer has not been
   * set, the call has no effect.
   */
  virtual void apply_hdr();

  /*!
  \brief Applies HDR to the specified input and places the result in the 
  specified output buffer. Assumes both buffers are allocated and the same size.
  */
  virtual void apply_hdr_inplace(Pixel4* in, float* out, size_t w, size_t h);

  /*!
   * \brief Compresses the dynamic range and convert illuminance space pixels
   * to color space. The result is save to the the bitmap output buffer.
   * If the output buffer has not been set, the call has no effect.
   */
  virtual void apply_bmp();

  /*!
   * \brief Resets all parameters, while preserving input and output bindings.
   */
  virtual void reset();

  void update_dims(int width, int height);

protected:
  /*!
   * \brief Name of the the tone map implementation.
   */
  std::string type;

  /*!
   * \brief Width of the input.
   */
  size_t w;

  /*!
   * \brief Height of the input.
   */
  size_t h;

  /*!
   * \brief Display gamma.
   *
   * By default the display gamma is set to 2.2.
   */
  float gamma;

  /*!
   * \brief Exposure adjustment.
   *
   * The level changes the global exposure level of all pixels in the layer.
   * Note that this is defined in F-stops. A value of 1 would increment
   * the current exposure by a full stop. Likewise and a value of -1 would
   * decrement it by a full stop. By default this is set to 0.
   */
  float level;

  /*!
   * \brief The input HDR Pixel buffer of the tone mapper.
   *
   * Note that the tone mapper assumes a valid input buffer.
   * Behavior of applying the tone map is undefined if the
   * input buffer is corrupted.
   */
  Pixel4 *input_buffer;

  /*!
   * \brief The output HDR Pixel buffer of the tone mapper.
   *
   * Note that the tone mapper assumes a valid output buffer.
   * Behavior of applying the tone map is undefined if the
   * output buffer is corrupted.
   */
  float *hdr_output_buffer;

  /*!
   * \brief The output bitmap buffer of the tone mapper.
   * Note that the tone mapper assumes a valid output buffer.
   * Behavior of applying the tone map is undefined if the
   * output buffer is corrupted.
   */
  char *bmp_output_buffer;
};

}; // namespace LightmanCore

#endif // USE_ARNOLD_CACHING

#endif // LIGHTMAN_TONEMAPPER_H
