#include "LumiverseColor.h"

namespace Lumiverse {

  LumiverseColor::LumiverseColor(ColorMode mode) : m_mode(mode) {
    // Initialize color   
    reset();
  }

  LumiverseColor::LumiverseColor(map<string, Eigen::Vector3d> basis, ColorMode mode) : m_mode(mode) {
    reset();
    m_basisVectors = basis;
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
    
    // We have now generated the target XYZ coordinate. If basis vectors were provided,
    // we'll try to match the xyY coordinate found from this converted XYZ vector.

    matchChroma(XYZ[0] / (XYZ[0] + XYZ[1] + XYZ[2]), XYZ[1] / (XYZ[0] + XYZ[1] + XYZ[2]));

//    m_X = XYZ[0];
//   m_Y = XYZ[1];
//    m_Z = XYZ[2];
  
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

  void LumiverseColor::matchChroma(double x, double y) {
    if (m_basisVectors.size() == 0) {
      // No basis vectors, can't do this calculation
      Logger::log(ERR, "matchChroma did not run since this Color does not have any basis vectors defined.");
      return;
    }

    try {
      // Set up the CLP model.
      ClpSimplex model;
      vector<int> indices;

      // Number of variables equal to number of basis vectors.
      int numCols = m_basisVectors.size();
      model.resize(0, numCols);

      // Maximize c1 + c2 + c3... equivalent to minimize -(c1 + c2 + c3...)
      for (int i = 0; i < numCols; i++) {
        model.setObjectiveCoefficient(i, -1);

        // Set objective function variable constraints. In range [0,1].
        model.setColBounds(i, 0, 1);

        indices.push_back(i);
      }

      vector<double> xCoef;
      vector<double> yCoef;

      for (auto it = m_basisVectors.begin(); it != m_basisVectors.end(); it++) {
        Eigen::Vector3d bv;

        // Calculate X coefficients. Equal to (X1 - x(X1+Y1+Z1))
        xCoef.push_back(bv[0] - x * (bv[0] + bv[1] + bv[2]));
        
        // Calculate Y coefficients. Equal to (Y1 - y(X1+Y1+Z1))
        yCoef.push_back(bv[1] - y * (bv[0] + bv[1] + bv[2]));
      }

      model.addRow(numCols, &indices[0], &xCoef[0], x, x);
      model.addRow(numCols, &indices[0], &yCoef[0], y, y);
      
      model.primal();

      const double* res = model.getColSolution();

      // Debugging only for now. Doesn't set things properly just logs.
      stringstream ss;
      ss << "Optimization ended with ";
      for (int i = 0; i < numCols; i++) {
        ss << i << ": " << res[i] << " ";
      }

      Logger::log(LDEBUG, ss.str());
    }
    catch (CoinError e) {
      e.print();
      if (e.lineNumber() >= 0)
        std::cout << "This was from a CoinAssert" << std::endl;
    }
  }
}