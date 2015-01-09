#include "lib/clp/ClpSimplex.hpp"
#include "lib/clp/CoinError.hpp"

#include "LumiverseColor.h"

namespace Lumiverse {

  LumiverseColor::LumiverseColor(ColorMode mode) : m_mode(mode) {
    // Initialize color   
    reset();
    initMode();
  }

  LumiverseColor::LumiverseColor(map<string, Eigen::Vector3d> basis, ColorMode mode) : m_mode(mode) {
    reset();
    initMode();
    m_basisVectors = basis;
  }

  LumiverseColor::LumiverseColor(map<string, double> params, map<string, Eigen::Vector3d> basis, ColorMode mode, double weight) {
    m_weight = weight;
    m_mode = mode;

    m_deviceChannels = params;
    m_basisVectors = basis;
  }

  LumiverseColor::LumiverseColor(LumiverseType* other) {
    if (other->getTypeName() != "color") {
      // Initialize to basic rgb in absence of any info.
      m_mode = BASIC_RGB;
      reset();
      initMode();
    }
    else {
      LumiverseColor* otherColor = (LumiverseColor*)other;
      m_weight = otherColor->m_weight;
      m_mode = otherColor->m_mode;

      m_mapMutex.lock();
      m_deviceChannels = otherColor->m_deviceChannels;
      m_basisVectors = otherColor->m_basisVectors;
      m_mapMutex.unlock();
    }
  }
  
  LumiverseColor::LumiverseColor(LumiverseColor* other) {
    m_weight = other->m_weight;
    m_mode = other->m_mode;

    m_mapMutex.lock();
    m_deviceChannels = other->m_deviceChannels;
    m_basisVectors = other->m_basisVectors;
    m_mapMutex.unlock();
  }

  LumiverseColor::LumiverseColor(const LumiverseColor& other) {
    m_weight = other.m_weight;
    m_mode = other.m_mode;

    m_mapMutex.lock();
    m_deviceChannels = other.m_deviceChannels;
    m_basisVectors = other.m_basisVectors;
    m_mapMutex.unlock();
  }

  void LumiverseColor::initMode() {
    // Create default channels for basic RGB mode
    if (m_mode == BASIC_RGB) {
      m_deviceChannels["Red"] = 0;
      m_deviceChannels["Green"] = 0;
      m_deviceChannels["Blue"] = 0;
    }
    if (m_mode == BASIC_CMY) {
      m_deviceChannels["Cyan"] = 0;
      m_deviceChannels["Magenta"] = 0;
      m_deviceChannels["Yellow"] = 0;
    }
  }

  LumiverseColor::~LumiverseColor() {
    // Nothing at the moment
  }

  void LumiverseColor::reset() {
    // Resets the color channels to 0.
    m_weight = 1;

    for (map<string, double>::iterator it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      it->second = 0;
    }
  }

  JSONNode LumiverseColor::toJSON(string name) {
    JSONNode channels;
    channels.set_name("channels");

    for (const auto& kvp : m_deviceChannels) {
      channels.push_back(JSONNode(kvp.first, kvp.second));
    }

    JSONNode basis;
    basis.set_name("basis");

    for (const auto& kvp : m_basisVectors) {
      JSONNode vec;
      vec.set_name(kvp.first);
      vec.push_back(JSONNode("X", kvp.second[0]));
      vec.push_back(JSONNode("Y", kvp.second[1]));
      vec.push_back(JSONNode("Z", kvp.second[2]));

      basis.push_back(vec.as_array());
    }
    
    JSONNode node;
    node.set_name(name);

    node.push_back(JSONNode("type", getTypeName()));
    node.push_back(channels);
    node.push_back(basis);
    node.push_back(JSONNode("weight", m_weight));
#ifdef USE_C11_MAPS
    node.push_back(JSONNode("mode", ColorModeToString[m_mode]));
#else
	node.push_back(JSONNode("mode", ColorModeToString(m_mode)));
#endif

    return node;
  }

  string LumiverseColor::asString() {
    stringstream ss;
    ss << "(";
    bool first = true;
    m_mapMutex.lock();
    for (const auto& kvp : m_deviceChannels) {
      if (!first)
        ss << ", ";
      if (first)
        first = false;

      ss << kvp.first << " : " << kvp.second;
    }
    m_mapMutex.unlock();
    ss << ")";
    return ss.str();
  }

  double LumiverseColor::getX() {
    if (m_mode == BASIC_RGB) {
      return RGBtoXYZ(m_deviceChannels["Red"] * m_weight, m_deviceChannels["Green"] * m_weight, m_deviceChannels["Blue"] * m_weight, sRGB)[0];
    }
    else {
      if (m_basisVectors.size() == 0) {
        Logger::log(ERR, "Cannot retrieve X value. No color basis defined.");
        return -1;
      }

      return sumComponent(0);
    }
  }

  double LumiverseColor::getY() {
    if (m_mode == BASIC_RGB) {
      return RGBtoXYZ(m_deviceChannels["Red"] * m_weight, m_deviceChannels["Green"] * m_weight, m_deviceChannels["Blue"] * m_weight, sRGB)[1];
    }
    else {
      if (m_basisVectors.size() == 0) {
        Logger::log(ERR, "Cannot retrieve Y value. No color basis defined.");
        return -1;
      }

      return sumComponent(1);
    }
  }

  double LumiverseColor::getZ() {
    if (m_mode == BASIC_RGB) {
      return RGBtoXYZ(m_deviceChannels["Red"] * m_weight, m_deviceChannels["Green"] * m_weight, m_deviceChannels["Blue"] * m_weight, sRGB)[2];
    }
    else {
      if (m_basisVectors.size() == 0) {
        Logger::log(ERR, "Cannot retrieve Z value. No color basis defined.");
        return -1;
      }

      return sumComponent(2);
    }
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
    if (m_mode == BASIC_RGB) {
      // Basic RGB doesn't care about color space. It's a simple mode.
      setRGBRaw(r, g, b, weight);
    }
    else {
      Eigen::Vector3d XYZ = RGBtoXYZ(r, g, b, cs);

      // We have now generated the target XYZ coordinate. If basis vectors were provided,
      // we'll try to match the xyY coordinate found from this converted XYZ vector.
      matchChroma(XYZ[0] / (XYZ[0] + XYZ[1] + XYZ[2]), XYZ[1] / (XYZ[0] + XYZ[1] + XYZ[2]), weight);
    }
  }

  Eigen::Vector3d LumiverseColor::getRGB(RGBColorSpace cs) {
    if (m_mode == BASIC_RGB) {
      // BASIC_RGB is based off of the RGB channels and only the RGB channels
      return Eigen::Vector3d(m_deviceChannels["Red"], m_deviceChannels["Green"], m_deviceChannels["Blue"]);
    }

    return ColorUtils::convXYZtoRGB(Eigen::Vector3d(getX(), getY(), getZ()), cs);
  }

  void LumiverseColor::setxy(double x, double y, double weight) {
    if (m_mode == BASIC_RGB) {
      Logger::log(ERR, "Function setxy() not supported in BASIC_RGB mode. Use setRGB().");
      return;
    }

    matchChroma(x, y, weight);
  }

  Eigen::Vector3d LumiverseColor::getxyY() {
    if (m_mode == ADDITIVE && m_basisVectors.size() == 0) {
      Logger::log(ERR, "Cannot calculate xxY coordinates. No basis vectors defined.");
      return Eigen::Vector3d(0, 0, 0);
    }

    return Eigen::Vector3d(getx(), gety(), getY());
  }

  Eigen::Vector3d LumiverseColor::getLab(ReferenceWhite refWhite) {
#ifdef USE_C11_MAPS
    return getLab(refWhites[refWhite]);
#else
    return getLab(refWhites(refWhite));
#endif
  }

  Eigen::Vector3d LumiverseColor::getLab(Eigen::Vector3d refWhite) {
    double L = 116 * labf(getY() / refWhite[1]) - 16;
    double a = 500 * (labf(getX() / refWhite[0]) - labf(getY() / refWhite[1]));
    double b = 200 * (labf(getY() / refWhite[1]) - labf(getZ() / refWhite[2]));
    return Eigen::Vector3d(L, a, b);
  }

  Eigen::Vector3d LumiverseColor::getLCHab(ReferenceWhite refWhite) {
#ifdef USE_C11_MAPS
    return getLCHab(refWhites[refWhite]);
#else
	return getLCHab(refWhites(refWhite));
#endif
  }

  Eigen::Vector3d LumiverseColor::getLCHab(Eigen::Vector3d refWhite) {
    Eigen::Vector3d lab = getLab(refWhite);
    double C = sqrt(lab[1] * lab[1] + lab[2] * lab[2]);
    double H = atan2(lab[2], lab[1]) * (180 / M_PI);

    if (H < 0) H += 360;
    if (H >= 360) H -= 360;

    return Eigen::Vector3d(lab[0], C, H);
  }

  bool LumiverseColor::addColorChannel(string name) {
    if (m_deviceChannels.count(name) == 0) {
      m_deviceChannels[name] = 0;
      return true;
    }
    else {
      stringstream ss;
      ss << "Color already has a channel named " << name;
      Logger::log(WARN, ss.str());
      return false;
    }
  }

  bool LumiverseColor::deleteColorChannel(string name) {
    if (m_deviceChannels.count(name) > 0) {
      m_deviceChannels.erase(name);
      return true;
    }
    else {
      stringstream ss;
      ss << "Color does not have a channel named " << name;
      Logger::log(WARN, ss.str());
      return false;
    }
  }

  bool LumiverseColor::setColorChannel(string name, double val) {
    if (m_deviceChannels.count(name) > 0) {
      m_deviceChannels[name] = ColorUtils::clamp(val, 0, 1);
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
    m_weight = ColorUtils::clamp(weight, 0, 1);
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

  void LumiverseColor::operator=(LumiverseColor& other) {
    m_weight = other.m_weight;
    m_mode = other.m_mode;
    
    m_mapMutex.lock();
    m_deviceChannels = other.m_deviceChannels;
    m_basisVectors = other.m_basisVectors;
    m_mapMutex.unlock();
  }

  LumiverseColor& LumiverseColor::operator+=(double val) {
    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      m_deviceChannels[it->first] = ColorUtils::clamp(it->second + val, 0, 1);
    }

    return *this;
  }

  LumiverseColor& LumiverseColor::operator-=(double val) {
    return (*this += -val);
  }

  LumiverseColor& LumiverseColor::operator*=(double val) {
    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      m_deviceChannels[it->first] = ColorUtils::clamp(it->second * val, 0, 1);
    }

    return *this;
  }

  LumiverseColor& LumiverseColor::operator/=(double val) {
    return (*this *= (1 / val));
  }

  shared_ptr<LumiverseType> LumiverseColor::lerp(LumiverseColor* rhs, float t) {
    // We lerp the weights, and then we lerp the color params of the lhs.
    LumiverseColor* newColor = new LumiverseColor(this);

    for (const auto& kvp : m_deviceChannels) {
      // Standard lerp for each color channel: (1 - t) * lhs + t * rhs
      newColor->setColorChannel(kvp.first, (1 - t) * kvp.second + rhs->getColorChannel(kvp.first) * t);
    }

    // Lerp weights
    newColor->setWeight((1 - t) * m_weight + rhs->getWeight() * t);
    return shared_ptr<LumiverseType>((LumiverseType*)newColor);
  }

  bool LumiverseColor::isEqual(LumiverseColor& other) {
    for (const auto& kvp : m_deviceChannels) {
      if (!doubleEq(kvp.second, other.getColorChannel(kvp.first)))
        return false;
    }

    return true;
  }

  bool LumiverseColor::isDefault() {
    // All channels must be 0 and weight must be 1 for default.
    bool channelsNull = true;

    for (const auto& c : m_deviceChannels) {
      channelsNull &= (c.second == 0);
    }

    return (channelsNull && (m_weight == 1));
  }

  void LumiverseColor::changeMode(ColorMode newMode) {
    m_mode = newMode;

    if (newMode == BASIC_RGB || newMode == BASIC_CMY) {
      m_deviceChannels.clear();
      m_basisVectors.clear();
    }

    initMode();
  }

  void LumiverseColor::setBasisVector(string channel, double x, double y, double z) {
    m_basisVectors[channel] = Eigen::Vector3d(x, y, z);
  }

  void LumiverseColor::removeBasisVector(string channel) {
    m_basisVectors.erase(channel);
  }

  Eigen::Vector3d LumiverseColor::getBasisVector(string channel) {
    if (m_basisVectors.count(channel) > 0)
      return m_basisVectors[channel];
    else
      return Eigen::Vector3d(0, 0, 0);
  }

  int LumiverseColor::cmpHue(LumiverseColor& other, ReferenceWhite refWhite) {
    double thisH = getLCHab(refWhite)[2];
    double thatH = other.getLCHab(refWhite)[2];

    if (doubleEq(thisH, thatH)) return 0;
    return (thisH < thatH) ? -1 : 1;
  }

  double LumiverseColor::sumComponent(int i) {
    double ret = 0;

    for (const auto& kvp : m_deviceChannels) {
      if (m_basisVectors.count(kvp.first) == 0) {
        stringstream ss;
        ss << "No basis component named " << kvp.first << " contained in color basis. Ignoring...";
        Logger::log(WARN, ss.str());
        continue;
      }
      ret += kvp.second * m_basisVectors[kvp.first][i] * m_weight;
    }
    return ret;
  }

  Eigen::Vector3d LumiverseColor::RGBtoXYZ(double r, double g, double b, RGBColorSpace cs) {
    r = ColorUtils::clamp(r, 0, 1);
    g = ColorUtils::clamp(g, 0, 1);
    b = ColorUtils::clamp(b, 0, 1);

    if (cs == sRGB) {
      r = ColorUtils::sRGBtoXYZCompand(r);
      g = ColorUtils::sRGBtoXYZCompand(g);
      b = ColorUtils::sRGBtoXYZCompand(b);
    }

#ifdef USE_C11_MAPS
    Eigen::Matrix3d M = RGBToXYZ[cs];
#else
	Eigen::Matrix3d M = RGBToXYZ(cs);
#endif
    Eigen::Vector3d rgb(r, g, b);
    Eigen::Vector3d XYZ = M * rgb;

    return XYZ;
  }

  double LumiverseColor::labf(double val) {
    return (val > pow(6.0 / 29.0, 3)) ? pow(val, 1.0 / 3.0) : (1.0 / 3.0) * pow(29.0 / 6.0, 2) * val + (4.0 / 29.0);
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
      int numCols = (int)m_basisVectors.size();
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

      for (const auto& kvp : m_basisVectors) {
        Eigen::Vector3d bv = kvp.second;

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
      for (const auto& kvp : m_basisVectors) {
        m_deviceChannels[kvp.first] = res[index];
        index++;
      }
      m_weight = weight;

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