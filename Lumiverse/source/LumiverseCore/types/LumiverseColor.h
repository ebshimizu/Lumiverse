/*! \file LumiverseColor.h
* \brief A LumiverseColor contains information about a Device Color parameter.
*/

#ifndef _LUMIVERSECOLOR_H_
#define _LUMIVERSECOLOR_H_
#pragma once

#include <unordered_map>
#include <cmath>
#include "lib/Eigen/Dense"
#include "../LumiverseType.h"

// D50 96.4212, 100.0, 82.5188
using namespace std;

namespace Lumiverse {
  /*! \brief Selects the color mode for a LumiverseColor */
  enum ColorMode {
    ADDITIVE,   /*!< Color represents a LED source light. */
    SUBTRACTIVE /*!< Color represents a filter system. */
  };

  /*! \brief Selects a RGB color space to use in color conversion functions. */
  enum RGBColorSpace {
    sRGB        /*!< sRGB color space. See http://en.wikipedia.org/wiki/SRGB */
  };

  /*! \brief RGB to XYZ matrices for color calculations */
  static unordered_map<int, Eigen::Matrix3d> RGBToXYZ =
  {
    { sRGB, (Eigen::Matrix3d() << 0.4124564, 0.3575761, 0.1804375,
                                  0.2126729, 0.7151522, 0.0721750,
                                  0.0193339, 0.1191920, 0.9503041).finished() }
  };

  /*!
  * \brief This class describes a color.
  *
  * Lumiverse represents its colors internally as CIE XYZ tristimulus values.
  * These values are converted to other color spaces as needed.
  *
  * Color mixing for LED devices is done by providing the coordinates of the
  * LEDs used in the lights and then taking a linear combination of those
  * basis vectors to match a user-specified target color. In the event of a
  * device using more than 3 LEDs, the user will be able to specify a
  * metamer in the event of multiple possibilities for a single target color.
  */
  class LumiverseColor : LumiverseType {
  public:
    /*! \brief Constructs a color. Default color is Black. */
    LumiverseColor(ColorMode mode = ADDITIVE);

    /*! \brief Destroys a color */
    virtual ~LumiverseColor();

    /*! \brief Returns the name of the type
    * \return String containing "color"
    */
    virtual string getTypeName() { return "color"; }

    /*! \brief Resets the color to defaults.
    * Default color is Black (0, 0, 0).  
    */
    virtual void reset();

    /*! \brief Converts a color to a JSON object */
    virtual JSONNode toJSON(string name);

    /*! \brief Returns the current color.
    *
    * Returns the color in both XYZ and sRGB format, since I'm guessing
    * most people won't know what color some XYZ coordinate represents.
    */
    virtual string asString();

    double getX() { return m_X; } /*!< Gets the X value. */
    double getY() { return m_Y; } /*!< Gets the Y value. */
    double getZ() { return m_Z; } /*!< Gets the Z value. */

    /*! \brief Gets the x value.
    *
    * Capitalization is important here. This is x from the CIE chromaticity diagram.
    */
    double getx();

    /*! \brief Gets the y value.
    *
    * Capitalization is important here. This is y from the CIE chromaticity diagram.
    */
    double gety();

    /*! \brief Gets the z value.
    *
    * Capitalization is important here. This is z from the CIE chromaticity diagram.
    */
    double getz();

    /*!
    * \brief Sets the color using RGB values.
    *
    * Sets the color using normal RGB values like we're all familiar with.
    * \param r Red (0.0 - 1.0)
    * \param g Green (0.0 - 1.0)
    * \param b Blue (0.0 - 1.0)
    * \param cs Color Space to use for converting RGB to XYZ.
    */
    void setRGB(double r, double g, double b, RGBColorSpace cs = sRGB);

    /*! \brief Gets the RGB color as a vector.
    *
    * Defaults to useing sRGB as the RGB color space.
    * \param cs Color space to use for the conversion
    * \return Eigen::Vector3d containing the RGB components of the color.
    */
    Eigen::Vector3d getRGB(RGBColorSpace cs = sRGB);

  private:
    /*! \brief CIE X tristimulus value */
    double m_X;
    /*! \brief CIE Y tristimulus value */
    double m_Y;
    /*! \brief CIE Z tristimulus value */
    double m_Z;

    /*! \brief Color mode for this color. */
    ColorMode m_mode;

    /*! \brief Contains a map from device channel to current value.
    *
    * These are the actual values that get sent to the light after converting
    * from XYZ. Any time you change a value, this map gets recalculated.
    * The map is automatically populated based on the specified basis vectors.
    */
    unordered_map<string, double> m_deviceChannels;

    // Currently in progress. Not sure how I'll represent the vectors
    // unordered_map<string, vector3> basisVectors;

    /*! \brief Updates the deviceChannels after a color change. */
    void updateDeviceColor();

    /*! \brief Clamps a value between min and max. Returns the clamped value. */
    double clamp(double val, double min, double max);

    /*! \brief sRGB value companding function for RGB to XYZ */
    double sRGBtoXYZCompand(double val);

    /*! \brief sRGB value companding cuntion for XYZ to RGB */
    double XYZtosRGBCompand(double val);
  };
}

#endif