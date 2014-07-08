#include "LumiverseColor.h"

namespace Lumiverse {

  LumiverseColor::LumiverseColor(ColorMode mode) : m_mode(mode) {
    // Initialize color   
    reset();
  }

  LumiverseColor::~LumiverseColor() {
    // Nothing at the moment
  }

  void LumiverseColor::reset() {
    m_X = 0;
    m_Y = 0;
    m_Z = 0;

    // Subtractive mode probably changes other things.
  }

  JSONNode LumiverseColor::toJSON(string name) {
    JSONNode node;
    node.set_name(name);

    return node;
  }

  string LumiverseColor::asString() {
    return ""; // Temporary
  }

  double LumiverseColor::getx() {
    if (m_X == 0 && m_Y == 0 && m_Z == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (m_X / (m_X + m_Y + m_Z));
  }

  double LumiverseColor::gety() {
    if (m_X == 0 && m_Y == 0 && m_Z == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (m_Y / (m_X + m_Y + m_Z));
  }

  double LumiverseColor::getz() {
    if (m_X == 0 && m_Y == 0 && m_Z == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (m_Z / (m_X + m_Y + m_Z));
  }

  void LumiverseColor::setRGB(double r, double g, double b, RGBColorSpace cs) {
    r = clamp(r, 0, 1);
    g = clamp(g, 0, 1);
    b = clamp(b, 0, 1);

    if (cs == sRGB) {
      r = sRGBtoXYZCompand(r);
      g = sRGBtoXYZCompand(g);
      b = sRGBtoXYZCompand(b);
    }

    Eigen::Matrix3d M = RGBToXYZ[cs];
    Eigen::Vector3d rgb(r, g, b);

    Eigen::Vector3d XYZ = M * rgb;
    
    m_X = XYZ[0];
    m_Y = XYZ[1];
    m_Z = XYZ[2];
  
    stringstream ss;
    ss << "Converted RGB (" << r << ", " << g << ", " << b << ") to XYZ (" << m_X << ", " << m_Y << ", " << m_Z << ")";
    Logger::log(LDEBUG, ss.str());
  }

  Eigen::Vector3d LumiverseColor::getRGB(RGBColorSpace cs) {
    Eigen::Vector3d rgb = RGBToXYZ[cs].inverse() * Eigen::Vector3d(m_X, m_Y, m_Z);

    if (cs == sRGB) {
      rgb[0] = XYZtosRGBCompand(rgb[0]);
      rgb[1] = XYZtosRGBCompand(rgb[1]);
      rgb[2] = XYZtosRGBCompand(rgb[2]);
    }

    stringstream ss;
    ss << "Converted XYZ (" << m_X << ", " << m_Y << ", " << m_Z << ") to RGB (" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ")";
    Logger::log(LDEBUG, ss.str());

    return rgb;
  }

  double LumiverseColor::clamp(double val, double min, double max) {
    double ret = val;
    ret = (ret < min) ? min : ret;
    ret = (ret > max) ? max : ret;

    return ret;
  }

  double LumiverseColor::sRGBtoXYZCompand(double val) {
    // this is some black magic right here but apparently it's a standard.
    return (val > 0.04045) ? pow(((val + 0.055) / 1.055), 2.4) : val / 12.92;
  }

  double LumiverseColor::XYZtosRGBCompand(double val) {
    return (val > 0.0031308) ? (1.055 * pow(val, 1 / 2.4) - 0.055) : val * 12.92;
  }
}