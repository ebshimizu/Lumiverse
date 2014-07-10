/*! \file LumiverseColor.h
* \brief A LumiverseColor contains information about a Device Color parameter.
*/

#ifndef _LUMIVERSECOLOR_H_
#define _LUMIVERSECOLOR_H_
#pragma once

#include <map>
#include <unordered_map>
#include <cmath>
#include "lib/Eigen/Dense"
#include "lib/clp/ClpSimplex.hpp"
#include "lib/clp/CoinError.hpp"
#include "../LumiverseType.h"

using namespace std;

namespace Lumiverse {
  /*! \brief Selects the color mode for a LumiverseColor */
  enum ColorMode {
    ADDITIVE,    /*!< Color represents a LED source light. */
    SUBTRACTIVE, /*!< Color represents a filter system. */
    BASIC_RGB,   /*!< Color has no basis vectors and assumes a default RGB valued color. */
    BASIC_CMY    /*!< Color has no basis vectors and assumes a default CMY subtractive system. */
  };

  /*! \brief Selects a RGB color space to use in color conversion functions. */
  enum RGBColorSpace {
    sRGB        /*!< sRGB color space. D65 reference white. See http://en.wikipedia.org/wiki/SRGB */
  };

  enum ReferenceWhite {
    D65,        /*< D65 illuminant. */
    D50,        /*< D50 illuminant. */
  };

  /*! \brief RGB to XYZ matrices for color calculations */
  static unordered_map<RGBColorSpace, Eigen::Matrix3d> RGBToXYZ = 
  {
    { sRGB, (Eigen::Matrix3d() << 0.4124564, 0.3575761, 0.1804375,
                                  0.2126729, 0.7151522, 0.0721750,
                                  0.0193339, 0.1191920, 0.9503041).finished() }
  };

  /*! \brief Maps Color space to the Reference White it uses */
  static unordered_map<RGBColorSpace, ReferenceWhite> ColorSpaceRefWhite =
  {
    { sRGB, D65 }
  };

  /*! \brief Reference White XYZ coordinates for standard illuminants */
  static unordered_map<ReferenceWhite, Eigen::Vector3d> refWhites =
  {
    { D65, Eigen::Vector3d(95.047, 100.00, 108.883) },
    { D50, Eigen::Vector3d(96.4212, 100.0, 82.5188) }
  };

  /*!
  * \brief This class describes a color.
  *
  * LumiverseColor objects are based off of the available color channels that
  * are controllable on the device. If you know the XYZ tristimulus values
  * for each channel, this class will offer a number of functions for
  * converting between XYZ, xyY, RGB, and other color spaces.
  *
  * Not knowing the XYZ values for a fixture won't prevent you from using the device,
  * but some of the color picking functions won't be available.
  *
  * Color mixing for LED devices is done by providing the coordinates of the
  * LEDs used in the lights and then taking a linear combination of those
  * basis vectors to match a user-specified target color.
  */
  class LumiverseColor : LumiverseType {
  public:
    /*! \brief Constructs a color. Default color is Black. */
    LumiverseColor(ColorMode mode = ADDITIVE);

    /*! \brief Constructs a color using the provided basis colors. 
    *
    * \param basis Map of axis name to basis vector. These vectors are
    * the colors produced by each individual LED color in the light.
    * If you don't have this information available, the color class will
    * still work but functionality will be limited.
    */
    LumiverseColor(map<string, Eigen::Vector3d> basis, ColorMode mode = ADDITIVE);

    /*! \brief Constructor for loading from JSON data */
    LumiverseColor(map<string, double> params, map<string, Eigen::Vector3d> basis, ColorMode mode, double weight);

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

    double getX(); /*!< Gets the X value. */
    double getY(); /*!< Gets the Y value. */
    double getZ(); /*!< Gets the Z value. */

    /*!
    * \brief Returns a vector representing the color in XYZ coordinates.
    * 
    * If there is no color basis defined, the returned vector will be (0, 0, 0).
    */
    Eigen::Vector3d getXYZ() { return Eigen::Vector3d(getX(), getY(), getZ()); }

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
    void setRGB(double r, double g, double b, double weight = 1.0, RGBColorSpace cs = sRGB);

    /*! \brief Gets the RGB color as a vector.
    *
    * Defaults to useing sRGB as the RGB color space.
    * \param cs Color space to use for the conversion
    * \return Eigen::Vector3d containing the RGB components of the color.
    */
    Eigen::Vector3d getRGB(RGBColorSpace cs = sRGB);

    /*!
    * \brief Sets the color to match the specified xy coordinate (xyY color space)
    *
    * Y is excluded since most lights won't be able to match an arbitrary Y coordinate
    * and it's rather dificult to guess at the correct value. We don't like guessing,
    * so use the weight parameter to tune the brightness of a color.
    */
    void setxy(double x, double y, double weight = 1.0);

    /*!
    * \brief Retrieves the xyY coordinate of the current color.
    * 
    * \return Vector containing (x, y, Y) for the color.
    */
    Eigen::Vector3d getxyY();

    /*!
    * \brief Retrieves the Lab coordinates for this color
    * 
    * See http://en.wikipedia.org/wiki/Lab_color_space.
    */
    Eigen::Vector3d getLab(ReferenceWhite refWhite);

    /*!
    * \brief Retrieves the Lab coordinates for this color with an arbitrary
    * reference white.
    *
    * See http://en.wikipedia.org/wiki/Lab_color_space
    * \sa getLab(ReferenceWhite)
    */
    Eigen::Vector3d getLab(Eigen::Vector3d refWhite);

    /*!
    * \brief Directly sets the value of a light parameter.
    *
    * Available parameters are defined by the user, though common ones will
    * include "Red", "Green", "Blue", "Cyan", etc. This function updates m_deviceChannels
    * and the value will be directly sent to the device.
    * \param name Parameter name (typically the name of a color axis, "Red", "Blue", etc.)
    * \param val Value to set the parameter to. Clamped between 0 and 1.
    * \return True on success. False when the specified axis does not exist.
    */
    bool setColorParam(string name, double val);

    /*!
    * \brief Subscript overload for accessing light color parameters.
    *
    * This will end up creating new parameters if you access something that
    * doesn't exist, so be careful. setColorParam is safer if you're worried about
    * that.
    */
    double& operator[](string name);

    /*!
    * \brief Sets the color weight, or overall brightness.
    *
    * Can be thought of as the "intensity" of the color. Used to make quick
    * color adjustments without modifying the chroma too much.
    */
    void setWeight(double weight);

    /*!
    * \brief Sets the device color parameters to the specified RGB value.
    * This will only work correctly if your device is specified to have RGB
    * parameters.
    *
    * For this to work, you must define m_deviceChannels to include only "Red", "Green"
    * and "Blue". If you construct a color in the SIMPLE_RGB mode, this will be handled
    * for you. Works like a more conventional RGB set method.
    */
    bool setRGBRaw(double r, double g, double b, double weight = 1.0);

    /*! \brief Gets the current values for the color parameters.
    * \return m_deviceChannels map
    */
    map<string, double> getColorParams() { return m_deviceChannels; }

  private:
    /*! \brief Parameter that controls the overall values of the device channels.
    *
    * Used to make dimmer versions of the same color. Only modifies the deviceChannels
    * map, not the XYZ values directly.
    */
    double m_weight;

    /*! \brief Color mode for this color. */
    ColorMode m_mode;

    /*! \brief Contains a map from device channel to current value.
    *
    * These are the actual values that get sent to the light after converting
    * from XYZ. Any time you change a value, this map gets recalculated.
    * The map is automatically populated based on the specified basis vectors.
    */
    map<string, double> m_deviceChannels;

    /*! \brief Basis vectors for each LED source in the light. Represented in XYZ. */
    map<string, Eigen::Vector3d> m_basisVectors;

    /*! \brief Calculates the value of the specified component at current device channel levels. */
    double sumComponent(int i);

    /*! \brief Clamps a value between min and max. Returns the clamped value. */
    double clamp(double val, double min, double max);

    /*! \brief sRGB value companding function for RGB to XYZ */
    double sRGBtoXYZCompand(double val);

    /*! \brief sRGB value companding cuntion for XYZ to RGB */
    double XYZtosRGBCompand(double val);

    /*! \brief Lab f() function.
    * 
    * Function used in the Lab color conversion.
    */
    double labf(double val);

    /*! \brief Runs a linear optimization to find a combination of the basis vectors
    *   that will match the target chroma value.
    *
    * This function prioritizes maintaining the target chromaticity when selecting
    * weights for the basis vectors. It runs a linear solver (CLP Simplex) over a
    * relatively small space. The weights are constrained between 0 and 1,
    * the x and y coordinates calculated from the weights must be equal
    * to the target x and y, and the solver attempts to maximize the sum of the weights.
    *
    * \param x Target x coordinate to match (xyY color space)
    * \param y Target y coordinate to match (xyY color space)
    * \param weight Controls the overall brightness of the resulting color.
    */
    void matchChroma(double x, double y, double weight = 1.0);
  };
}

#endif