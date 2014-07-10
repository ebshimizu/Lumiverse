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
    // Resets the color channels to 0.
    m_weight = 1;

    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      it->second = 0;
    }
  }

  JSONNode LumiverseColor::toJSON(string name) {
    JSONNode node;
    node.set_name(name);

    return node;
  }

  string LumiverseColor::asString() {
    return ""; // Temporary
  }

  double LumiverseColor::getX() {
    if (m_deviceChannels.size() == 0) {
      Logger::log(ERR, "Cannot retrieve X value. No color basis defined.");
      return -1;
    }

    return sumComponent(0);
  }

  double LumiverseColor::getY() {
    if (m_deviceChannels.size() == 0) {
      Logger::log(ERR, "Cannot retrieve Y value. No color basis defined.");
      return -1;
    }

    return sumComponent(1);
  }

  double LumiverseColor::getZ() {
    if (m_deviceChannels.size() == 0) {
      Logger::log(ERR, "Cannot retrieve Z value. No color basis defined.");
      return -1;
    }

    return sumComponent(2);
  }

  double LumiverseColor::getx() {
    if (getX() == 0 && getY() == 0 && getZ() == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (getX() / (getX() + getY() + getZ()));
  }

  double LumiverseColor::gety() {
    if (getX() == 0 && getY() == 0 && getZ() == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (getY() / (getX() + getY() + getZ()));
  }

  double LumiverseColor::getz() {
    if (getX() == 0 && getY() == 0 && getZ() == 0)
      return 0; // Not sure if actually correct, but should be fine.

    return (getZ() / (getX() + getY() + getZ()));
  }

  void LumiverseColor::setRGB(double r, double g, double b, double weight, RGBColorSpace cs) {
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
    matchChroma(XYZ[0] / (XYZ[0] + XYZ[1] + XYZ[2]), XYZ[1] / (XYZ[0] + XYZ[1] + XYZ[2]), weight);
  }

  Eigen::Vector3d LumiverseColor::getRGB(RGBColorSpace cs) {
    // Vector is scaled by 1/100 bringing it inline withthe [0,1] range typically used by RGB.
    Eigen::Vector3d XYZvec = (Eigen::Vector3d(getX(), getY(), getZ()) * m_weight) / 100;
    Eigen::Vector3d rgb = RGBToXYZ[cs].inverse() * XYZvec;

    if (cs == sRGB) {
      rgb[0] = clamp(XYZtosRGBCompand(rgb[0]), 0, 1);
      rgb[1] = clamp(XYZtosRGBCompand(rgb[1]), 0, 1);
      rgb[2] = clamp(XYZtosRGBCompand(rgb[2]), 0, 1);
    }

    return rgb;
  }

  void LumiverseColor::setxy(double x, double y, double weight) {
    matchChroma(x, y, weight);
  }

  Eigen::Vector3d LumiverseColor::getxyY() {
    return Eigen::Vector3d(getx(), gety(), getY());
  }

  Eigen::Vector3d LumiverseColor::getLab(ReferenceWhite refWhite) {
    return getLab(refWhites[refWhite]);
  }

  Eigen::Vector3d LumiverseColor::getLab(Eigen::Vector3d refWhite) {
    double L = 116 * labf(getY() / refWhite[1]) - 16;
    double a = 500 * (labf(getX() / refWhite[0]) - labf(getY() / refWhite[1]));
    double b = 200 * (labf(getY() / refWhite[1]) - labf(getZ() / refWhite[2]));
    return Eigen::Vector3d(L, a, b);
  }

  bool LumiverseColor::setColorParam(string name, double val) {
    if (m_deviceChannels.count(name) > 0) {
      m_deviceChannels[name] = val;
      return true;
    }
    else {
      stringstream ss;
      ss << "Color has no mapped channel named " << name;
      Logger::log(WARN, ss.str());
      return false;
    }
  }

  double& LumiverseColor::operator[](string name) {
    return m_deviceChannels[name];
  }

  void LumiverseColor::setWeight(double weight) {
    m_weight = weight;
  }

  bool LumiverseColor::setRGBRaw(double r, double g, double b, double weight) {
    if (m_deviceChannels.count("Red") == 0 ||
      m_deviceChannels.count("Green") == 0 ||
      m_deviceChannels.count("Blue") == 0) {
      Logger::log(ERR, "Color does not have required color parameters. Needs Red, Green, Blue. (in setRGBRaw)");
      return false;
    }

    m_deviceChannels["Red"] = r;
    m_deviceChannels["Green"] = g;
    m_deviceChannels["Blue"] = b;
    m_weight = weight;

    return true;
  }

  double LumiverseColor::sumComponent(int i) {
    double ret = 0;
    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      if (m_basisVectors.count(it->first) == 0) {
        stringstream ss;
        ss << "No basis component named " << it->first << " contained in color basis. Ignoring...";
        Logger::log(WARN, ss.str());
        continue;
      }
      ret += it->second * m_basisVectors[it->first][i];
    }
    return ret;
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

  double LumiverseColor::labf(double val) {
    return (val > pow(6 / 29, 3)) ? pow(val, 1 / 3) : (1 / 3) * pow(29 / 6, 2) * val + (4 / 29);
  }

  void LumiverseColor::matchChroma(double x, double y, double weight) {
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
        Eigen::Vector3d bv = it->second;

        // Calculate X coefficients. Equal to (X1 - x(X1+Y1+Z1))
        xCoef.push_back(bv[0] - x * (bv[0] + bv[1] + bv[2]));
        
        // Calculate Y coefficients. Equal to (Y1 - y(X1+Y1+Z1))
        yCoef.push_back(bv[1] - y * (bv[0] + bv[1] + bv[2]));
      }

      model.addRow(numCols, &indices[0], &xCoef[0], 0, 0);
      model.addRow(numCols, &indices[0], &yCoef[0], 0, 0);
      
      model.dual();

      const double* res = model.getColSolution();

      // Set value for device channels if model is optimal
      int index = 0;
      for (auto it = m_basisVectors.begin(); it != m_basisVectors.end(); it++) {
        m_deviceChannels[it->first] = res[index];
        index++;
      }

      // Just warn if it doesn't work quite right. User can always change.
      if (model.isProvenOptimal())
        Logger::log(LDEBUG, "Optimal color match found");
      else
        Logger::log(WARN, "Non-optimal color solution. Color may be out of gamut.");
    }
    catch (CoinError e) {
      e.print();
      if (e.lineNumber() >= 0)
        std::cout << "This was from a CoinAssert" << std::endl;
    }
  }
}