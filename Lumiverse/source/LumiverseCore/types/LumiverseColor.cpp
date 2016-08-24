#include "lib/clp/ClpSimplex.hpp"
#include "lib/clp/CoinError.hpp"

#include "LumiverseColor.h"

namespace Lumiverse {

  LumiverseColor::LumiverseColor(ColorMode mode) : m_mode(mode) {
    // Initialize color   
    reset();
    initMode();
    m_basisVectors = map<string, Eigen::Vector3d>();
  }

  LumiverseColor::LumiverseColor(map<string, Eigen::Vector3d> basis, ColorMode mode) : m_mode(mode) {
    reset();
    initMode();
    m_basisVectors = basis;
  }

  LumiverseColor::LumiverseColor(unordered_map<string, double> params, map<string, Eigen::Vector3d> basis, ColorMode mode, double weight) {
    m_weight = weight;
    m_mode = mode;
    m_XYZupdated = false;

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
      m_deviceChannels.clear();
      for (const auto& kvp : otherColor->m_deviceChannels) {
        m_deviceChannels[kvp.first] = kvp.second;
      }
      m_basisVectors = otherColor->m_basisVectors;
      m_mapMutex.unlock();
      m_XYZupdated = false; // Always reset XYZ cache
    }
  }
  
  LumiverseColor::LumiverseColor(LumiverseColor* other) {
    m_weight = other->m_weight;
    m_mode = other->m_mode;

    m_mapMutex.lock();
    m_deviceChannels.clear();
    for (const auto& kvp : other->m_deviceChannels) {
      m_deviceChannels[kvp.first] = kvp.second;
    }
    m_basisVectors = other->m_basisVectors;
    m_mapMutex.unlock();
    m_XYZupdated = false; // Always reset XYZ cache
  }

  LumiverseColor::LumiverseColor(const LumiverseColor& other) {
    m_weight = other.m_weight;
    m_mode = other.m_mode;

    m_mapMutex.lock();
    m_deviceChannels.clear();
    for (const auto& kvp : other.m_deviceChannels) {
      m_deviceChannels[kvp.first] = kvp.second;
    }
    m_basisVectors = other.m_basisVectors;
    m_mapMutex.unlock();
    m_XYZupdated = false; // Always reset XYZ cache
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

    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      it->second = 0;
    }

    m_XYZupdated = false;
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
    if (!m_XYZupdated)
      updateXYZ();

    if (m_XYZupdated)
      return m_XYZ[0];
    else {
      return -1;
    }
  }

  double LumiverseColor::getY() {
    if (!m_XYZupdated)
      updateXYZ();

    if (m_XYZupdated)
      return m_XYZ[1];
    else {
      return -1;
    }
  }

  double LumiverseColor::getZ() {
    if (!m_XYZupdated)
      updateXYZ();

    if (m_XYZupdated)
      return m_XYZ[2];
    else {
      return -1;
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
    
    m_XYZupdated = false;
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

    m_XYZupdated = false;
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
    if (m_mode == BASIC_RGB)
      refWhite /= 100.0;

    return ColorUtils::convXYZtoLab(Eigen::Vector3d(getX(), getY(), getZ()), refWhite);
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

  Eigen::Vector2d LumiverseColor::getupvp() {
    Eigen::Vector3d xyY = getxyY();
    double u = (4 * xyY[0]) / (-2 * xyY[0] + 12 * xyY[1] + 3);
    double v = (9 * xyY[1]) / (-2 * xyY[0] + 12 * xyY[1] + 3);

    return Eigen::Vector2d(u, v);
  }

  Eigen::Vector2d LumiverseColor::getuv()
  {
    Eigen::Vector2d cieuv = getupvp();
    cieuv[1] = (2.0 / 3.0) * cieuv[1];

    return cieuv;
  }

  Eigen::Vector3d LumiverseColor::getHSV(RGBColorSpace cs)
  {
    double R, G, B;

    if (m_mode == BASIC_RGB) {
      R = m_deviceChannels["Red"];
      G = m_deviceChannels["Green"];
      B = m_deviceChannels["Blue"];
    }
    else {
      auto RGB = getRGB(cs);
      R = RGB[0];
      G = RGB[1];
      B = RGB[2];
    }

    double M = max(R, max(G, B));
    double m = min(R, min(G, B));
    double C = M - m;

    double Hp;

    if (C == 0) {
      // Technically undefined, we'll default to 0 here
      Hp = 0;
    }
    else if (M == R) {
      Hp = fmod(((G - B) / C), 6);
    }
    else if (M == G) {
      Hp = (B - R) / C + 2;
    }
    else if (M == B) {
      Hp = (R - G) / C + 4;
    }
    double H = 60 * Hp;

    if (H < 0)
      H += 360;

    double V = M;

    double S = 0;
    if (V != 0)
      S = C / V;

    return Eigen::Vector3d(H, S, V);
  }

  Eigen::Vector2d LumiverseColor::getCCT()
  {
    Eigen::Vector2d uv = getuv();
    double u = uv[0];
    double v = uv[1];

    // Using the conversion method specified here:
    // http://www.cormusa.org/uploads/CORM_2011_Calculation_of_CCT_and_Duv_and_Practical_Conversion_Formulae.PDF
    // Slide 21

    double k[7][7] = {
      { -1.77348e-01, 1.115559e+00, -1.5008606E+00, 9.750013E-01, -3.307009E-01, 5.6061400E-02, -3.7146000E-03 },
      { 5.308409E-04, 2.1595434E-03, -4.3534788E-03, 3.6196568E-03, -1.589747E-03, 3.5700160E-04, -3.2325500E-05 },
      { -8.58308927E-01, 1.964980251E+00, -1.873907584E+00, 9.53570888E-01, -2.73172022E-01, 4.17781315E-02, -2.6653835E-03 },
      { -2.3275027E+02, 1.49284136E+03, -2.7966888E+03, 2.51170136E+03, -1.1785121E+03, 2.7183365E+02, -2.3524950E+01 },
      { -5.926850606E+08, 1.34488160614E+09, -1.27141290956E+09, 6.40976356945E+08, -1.81749963507E+08, 2.7482732935E+07, -1.731364909E+06 },
      { -2.3758158E+06, 3.89561742E+06, -2.65299138E+06, 9.60532935E+05, -1.9500061E+05, 2.10468274E+04, -9.4353083E+02 },
      { 2.8151771E+06, -4.11436958E+06, 2.48526954E+06, -7.93406005E+05, 1.4101538E+05, -1.321007E+04, 5.0857956E+02 }
    };

    double Lfp = sqrt(pow(u - 0.292, 2) + pow(v - 0.24, 2));
    double a1 = atan((v - 0.24) / (u - 0.292));

    double a = a1;
    if (a1 < 0)
      a = a1 + M_PI;

    double Lbb = k[0][6] * pow(a, 6) + k[0][5] * pow(a, 5) + k[0][4] * pow(a, 4) +
                 k[0][3] * pow(a, 3) + k[0][2] * pow(a, 2) + k[0][1] * a + k[0][0];
    double Duv = Lfp - Lbb;

    double T1, dTc1, T2, c, dTc2;

    if (a < 2.54) {
      T1 = 1 / (k[1][6] * pow(a, 6) + k[1][5] * pow(a, 5) + k[1][4] * pow(a, 4) + k[1][3] * pow(a, 3) + k[1][2] * a * a + k[1][1] * a + k[1][0]);
      dTc1 = (k[3][6] * pow(a, 6) + k[3][5] * pow(a, 5) + k[3][4] * pow(a, 4) + k[3][3] * pow(a, 3) + k[3][2] * a * a + k[3][1] * a + k[3][0]) * ((Lbb + 0.01) / Lfp) * (Duv / 0.01);
    }
    else if (a >= 2.54) {
      T1 = 1 / (k[2][6] * pow(a, 6) + k[2][5] * pow(a, 5) + k[2][4] * pow(a, 4) + k[2][3] * pow(a, 3) + k[2][2] * a * a + k[2][1] * a + k[2][0]);
      dTc1 = (k[4][6] * pow(a, 6) + k[4][5] * pow(a, 5) + k[4][4] * pow(a, 4) + k[4][3] * pow(a, 3) + k[4][2] * a * a + k[4][1] * a + k[4][0]) * ((Lbb + 0.01) / Lfp) * (Duv / 0.01);
    }

    T2 = T1 - dTc1;

    if (T2 <= 0) {
      // technically this should be -infinity, but we'll clamp it to a large
      // negative number to keep things from going nan.
      c = -100000;
    }
    else {
      c = log10(T2);
    }

    if (Duv >= 0) {
      dTc2 = (k[5][6] * pow(c, 6) + k[5][5] * pow(c, 5) + k[5][4] * pow(c, 4) + k[5][3] * pow(c, 3) + k[5][2] * c * c + k[5][1] * c + k[5][0]);
    }
    else if (Duv < 0) {
      dTc2 = (k[6][6] * pow(c, 6) + k[6][5] * pow(c, 5) + k[6][4] * pow(c, 4) + k[6][3] * pow(c, 3) + k[6][2] * c * c + k[6][1] * c + k[6][0]) * pow(abs(Duv / 0.03), 2);
    }

    double T = T2 - dTc2;

    return Eigen::Vector2d(T, Duv);
  }

  bool LumiverseColor::addColorChannel(string name) {
    if (m_deviceChannels.count(name) == 0) {
      m_deviceChannels[name] = 0;
      m_XYZupdated = false;
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
      m_XYZupdated = false;
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
      m_XYZupdated = false;
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
    m_XYZupdated = false;
    return m_deviceChannels[name];
  }

  void LumiverseColor::setWeight(double weight) {
    m_XYZupdated = false;
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
    m_XYZupdated = false;

    return true;
  }

  bool LumiverseColor::setHSV(double H, double S, double V, double weight)
  {
    if (m_mode != BASIC_RGB) {
      Logger::log(ERR, "Cannot set HSV for non-RGB color");
      return false;
    }

    // Normalize H
    while (H < 0)
    {
      H += 360;
    }
    while (H >= 360)
    {
      H -= 360;
    }

    if (S < 0 || S > 1 || V < 0 || V > 1) {
      Logger::log(LDEBUG, "Invalid HSV tuple specified. Clamping...");
			S = ColorUtils::clamp(S, 0, 1);
			V = ColorUtils::clamp(V, 0, 1);
    }

    m_weight = weight;

    double C = V * S;
    double Hp = H / 60;
    double X = C * (1 - abs(fmod(Hp, 2) - 1));

    double R, G, B;
    if (0 <= Hp && Hp < 1) {
      R = C;
      G = X;
      B = 0;
    }
    else if (1 <= Hp && Hp < 2) {
      R = X;
      G = C;
      B = 0;
    }
    else if (2 <= Hp && Hp < 3) {
      R = 0;
      G = C;
      B = X;
    }
    else if (3 <= Hp && Hp < 4) {
      R = 0;
      G = X;
      B = C;
    }
    else if (4 <= Hp && Hp < 5) {
      R = X;
      G = 0;
      B = C;
    }
    else if (5 <= Hp && Hp < 6) {
      R = C;
      G = 0;
      B = X;
    }

    double m = V - C;
    m_deviceChannels["Red"] = R + m;
    m_deviceChannels["Green"] = G + m;
    m_deviceChannels["Blue"] = B + m;
    m_XYZupdated = false;

    return true;
  }

  void LumiverseColor::operator=(LumiverseColor& other) {
    m_weight = other.m_weight;
    m_mode = other.m_mode;

    m_mapMutex.lock();
    for (const auto& kvp : other.m_deviceChannels) {
      m_deviceChannels[kvp.first] = kvp.second;
    }
    //m_basisVectors = other.m_basisVectors;
    m_mapMutex.unlock();
    m_XYZupdated = false;
  }

  LumiverseColor& LumiverseColor::operator+=(double val) {
    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      m_deviceChannels[it->first] = ColorUtils::clamp(it->second + val, 0, 1);
      m_XYZupdated = false;
    }

    return *this;
  }

  LumiverseColor& LumiverseColor::operator-=(double val) {
    return (*this += -val);
  }

  LumiverseColor& LumiverseColor::operator*=(double val) {
    for (auto it = m_deviceChannels.begin(); it != m_deviceChannels.end(); it++) {
      m_deviceChannels[it->first] = ColorUtils::clamp(it->second * val, 0, 1);
      m_XYZupdated = false;
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
    m_XYZupdated = false;
  }

  void LumiverseColor::removeBasisVector(string channel) {
    m_basisVectors.erase(channel);
    m_XYZupdated = false;
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
    return ColorUtils::convRGBtoXYZ(r, g, b, cs);
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
      
      model.setLogLevel(0);
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
  void LumiverseColor::updateXYZ()
  {
    if (m_mode == BASIC_RGB) {
      m_XYZ = RGBtoXYZ(m_deviceChannels["Red"] * m_weight, m_deviceChannels["Green"] * m_weight, m_deviceChannels["Blue"] * m_weight, sRGB);
    }
    else {
      if (m_basisVectors.size() == 0) {
        Logger::log(ERR, "Can't get XYZ color, no basis colors defined.");
        return;
      }

      m_XYZ[0] = sumComponent(0);
      m_XYZ[1] = sumComponent(1);
      m_XYZ[2] = sumComponent(2);
    }

    m_XYZupdated = true;
  }
}