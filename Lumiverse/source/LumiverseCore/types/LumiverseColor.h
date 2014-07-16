/*! \file LumiverseColor.h
* \brief A LumiverseColor contains information about a Device Color parameter.
*/

#ifndef _LUMIVERSECOLOR_H_
#define _LUMIVERSECOLOR_H_
#pragma once

#include <map>
#include <unordered_map>
#include <cmath>
#include <mutex>
#include <memory>
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
    BASIC_RGB,   /*!< Color has no basis vectors and assumes a default RGB valued color. Uses the sRGB color space. */
    BASIC_CMY    /*!< Color has no basis vectors and assumes a default CMY subtractive system. */
  };

  /*! \brief Converts ColorMode to a string */
  static unordered_map<int, string> ColorModeToString =
  {
    { ADDITIVE, "ADDITIVE" }, { SUBTRACTIVE, "SUBTRACTIVE" },
    { BASIC_RGB, "BASIC_RGB" }, { BASIC_CMY, "BASIC_CMY" }
  };

  /*! \brief Converts a string to ColorMode. */
  static unordered_map<string, ColorMode> StringToColorMode =
  {
    { "ADDITIVE", ADDITIVE }, { "SUBTRACTIVE", SUBTRACTIVE },
    { "BASIC_RGB", BASIC_RGB }, { "BASIC_CMY", BASIC_CMY }
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
  static unordered_map<int, Eigen::Matrix3d> RGBToXYZ =
  {
    { sRGB, (Eigen::Matrix3d() << 0.4124564, 0.3575761, 0.1804375,
                                  0.2126729, 0.7151522, 0.0721750,
                                  0.0193339, 0.1191920, 0.9503041).finished() }
  };

  /*! \brief Maps Color space to the Reference White it uses */
  static unordered_map<int, ReferenceWhite> ColorSpaceRefWhite =
  {
    { sRGB, D65 }
  };

  /*! \brief Reference White XYZ coordinates for standard illuminants */
  static unordered_map<int, Eigen::Vector3d> refWhites =
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
  *
  * When intializing BASIC* type Colors, you'll find that it's easier to create
  * them programmatically instead of defining them in a Rig file.
  */
  class LumiverseColor : LumiverseType {
  public:
    /*! \brief Constructs a color. Default color is Black.
    *
    * Passing in different modes will initialize the light with different settings.
    * BASIC_RGB will base the color around RGB instead of XYZ, while BASIC_CMY
    * will base the color around a CMY color system. SUBTRACTIVE mode is not fully supported yet.
    */
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

    /*! \brief Copy constructor (from generic LumiverseType) */
    LumiverseColor(LumiverseType* other);

    /*! \brief Copy constructor (from pointer) */
    LumiverseColor(LumiverseColor* other);

    /*! \brief Copy constructor */
    LumiverseColor(const LumiverseColor& other);

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

    /*! \brief Gets the X tristimulus value for the color
    *
    * If this color is in BASIC_RGB mode, this value will be calculated
    * from the current RGB channel values using the sRGB color space.
    * If you have no basis vectors defined in ADDITIVE mode, -1 will be returned.
    */
    double getX();

    /*! \brief Gets the Y tristimulus value for the color
    *
    * If this color is in BASIC_RGB mode, this value will be calculated
    * from the current RGB channel values using the sRGB color space.
    * If you have no basis vectors defined in ADDITIVE mode, -1 will be returned.
    */
    double getY();

    /*! \brief Gets the Z tristimulus value for the color
    *
    * If this color is in BASIC_RGB mode, this value will be calculated
    * from the current RGB channel values using the sRGB color space.
    * If you have no basis vectors defined in ADDITIVE mode, -1 will be returned.
    */
    double getZ();

    /*!
    * \brief Returns a vector representing the color in XYZ coordinates.
    * 
    * If there is no color basis defined, the returned vector will be (-1, -1, -1).
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
    * If using BASIC_RGB, this function will act exactly like setRGBRaw.
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
    * \brief Retrieves the LCH(ab) coordintes for this color.
    *
    * See http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
    */
    Eigen::Vector3d getLCHab(ReferenceWhite refWhite);

    /*!
    * \brief Retrives the LCH(ab) coordinates with an arbitrary reference white.
    *
    * See http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
    */
    Eigen::Vector3d getLCHab(Eigen::Vector3d refWhite);

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
    bool setColorChannel(string name, double val);

    /*!
    * \brief Gets the weighted value for the color channel.
    *
    * You should use this function when retrieving data to send over the network. 
    */
    double getColorChannel(string name) { return m_deviceChannels[name] * m_weight; }

    /*!
    * \brief Subscript overload for accessing light color parameters.
    *
    * Note that this function returns the unweighted value for a channel.
    * Be careful when using it to send data over the network.
    */
    double& operator[](string name);

    /*!
    * \brief Sets the color weight, or overall brightness.
    *
    * Can be thought of as the "intensity" of the color. Used to make quick
    * color adjustments without modifying the chroma too much.
    * \param weight Weight between 0 and 1 for the color. Values outside the
    * valid range will be clamped to that range.
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

    /*! \brief Gets the weight. */
    double getWeight() { return m_weight; }

    // Arithmetic overrides
    void operator=(LumiverseColor& other);

    /*! \brief Adds a constant to the color. */
    LumiverseColor& operator+=(double val);            

    /*! \brief Subtract a constant from the color. */
    LumiverseColor& operator-=(double val);

    /*! \brief Multiplay a constant to the color. */
    LumiverseColor& operator*=(double val);
    
    /*! \brief Divide the color by a constant. */
    LumiverseColor& operator/=(double val);

    /*!
    * \brief Does a linear interpolation of the two colors.
    *
    * The object this function is called on is treated as the left hand side.
    * If the colors don't have the same channels (I don't know what you're doing then...)
    * the channels for the left hand side (object this is called on) will be used.
    * \param rhs Initial value
    * \param t Value between 0 and 1. When `t = 0`, this function returns the value of this object. When
    * `t = 1`, this function returns the value of rhs.
    * \return New LumiverseColor containing the result of the interpolation.
    */
    shared_ptr<LumiverseType> lerp(LumiverseColor* rhs, float t);

    /*!
    * \brief Compares two colors using the color channel values (device levels)
    *
    * Note that this definitely doesn't guarantee that the chroma values are the same,
    * just that the devices have the same value. You will need to use a different
    * equality function for chroma equality.
    */
    bool isEqual(LumiverseColor& other);

    /*! 
    * \brief Compares two colors based on the LCHab hue value.
    *
    * Uses H(ab) to compare colors. H(ab) ranges from 0-360, and
    * arranges colors in chromatic order (red -> violet)
    * \param refWhite Reference white. Defaults to D65.
    */
    int cmpHue(LumiverseColor& other, ReferenceWhite refWhite = D65);

    virtual bool isDefault();

  private:
    /*! \brief Parameter that controls the overall values of the device channels.
    *
    * Used to make dimmer versions of the same color. Only modifies the deviceChannels
    * map, not the XYZ values directly.
    */
    double m_weight;

    /*! \brief Color mode for this color. */
    ColorMode m_mode;

    /*! \brief Protects access to the deviceChannels map */
    mutex m_mapMutex;

    /*! \brief Contains a map from device channel to current value.
    *
    * These are the actual values that get sent to the light after converting
    * from XYZ. Any time you change a value, this map gets recalculated.
    * The map is automatically populated based on the specified basis vectors.
    */
    map<string, double> m_deviceChannels;

    /*! \brief Basis vectors for each LED source in the light. Represented in XYZ. */
    map<string, Eigen::Vector3d> m_basisVectors;

    /*! \brief Intialization steps for each particular mode. */
    void initMode();

    /*! \brief Calculates the value of the specified component at current device channel levels. */
    double sumComponent(int i);

    /*! \brief Clamps a value between min and max. Returns the clamped value. */
    double clamp(double val, double min, double max);

    /*! \brief Helper for converting RGB to XYZ */
    Eigen::Vector3d RGBtoXYZ(double r, double g, double b, RGBColorSpace cs);

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

    inline bool doubleEq(double a, double b) {
      return (abs(a - b) < DBL_EPSILON);
    }
  };

  // Operators time!
  inline bool operator==(LumiverseColor& a, LumiverseColor& b) {
    if (a.getTypeName() != "color" || b.getTypeName() != "color")
      return false;

    return a.isEqual(b);
  }
}

#endif