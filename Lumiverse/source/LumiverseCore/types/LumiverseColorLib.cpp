#include "LumiverseColorLib.h"

namespace Lumiverse {
namespace ColorUtils{ 

double blackbodySPD(unsigned int nm, unsigned int temp) {
  // For some reason the eqn provided didn't work so I pulled this from the color calculator
  // at Bruce Lindbloom's site.
  double C1 = 2.0 * M_PI * 6.626176 * 2.99792458 * 2.99792458;	// * 1.0e-18
  double C2 = (6.626176 * 2.99792458) / 1.380662;	// * 1.0e-3
  double lm = nm * 1.0e-3;
  double lm5 = pow(lm, 5);
  return C1 / (lm5 * 1.0e-12 * (exp(C2 / (temp * lm * 1.0e-3)) - 1.0));	// -12 = -30 - (-18)
}

Eigen::Vector3d getApproxColor(string gel, float intens) {
  // Multiple gels can be used (use a +)
  vector<string> gels;
  size_t gelbrk = gel.find("+");
  size_t firstchar = 0;
  while (gelbrk != string::npos) {
    gels.push_back(gel.substr(firstchar, gelbrk - firstchar));
    firstchar = gelbrk + 1;
    gelbrk = gel.find("+", firstchar);
  }
  gels.push_back(gel.substr(firstchar, string::npos));

  // We assume a linear ambershift of an incandescent fixture.
  // Assuming that a lamp approaches incandescent when it gets dim and 
  // approaches manufacturer spec of 3250K at full brightness.
  int temp = (int)(1800 + 1450 * intens);

  Eigen::Vector3d ret(0, 0, 0);
  double spectrum[471];

  // First generate spectrum
  for (int i = 0; i < 471; i++) {
    spectrum[i] = blackbodySPD(i + 360, temp);
  }

  // Then multiply by transmission
  for (int g = 0; g < gels.size(); g++) {
    if (gelsCoarse.count(gels[g]) == 0) {
      Logger::log(WARN, "Gel " + gels[g] + " not found in Lumiverse Color Library. Skipping...");
      continue;
    }
    auto color = gelsCoarse[gels[g]];

    // For each wavelength, calculate SPD, multiply by transmission, multiply by CMF,
    // keep running sum.
    // We'll iterate over the color's data points to make things a bit nicer.
    for (int i = 0; i < 20; i++) {
      if (i == 19) {
        // Special case for final element
        double trans = color[i];
        int idx = i * 20;

        spectrum[idx] *= trans;
      }
      else {
        for (int j = 0; j < 20; j++) {
          double trans = color[i] + (color[i + 1] - color[i]) * (double)(j / 20.0);
          int idx = i * 20 + j;

          spectrum[idx] *= trans;
        }
      }
    }
  }

  // Apply CIE functions
  for (int i = 0; i < 471; i++) {
    ret[0] += spectrum[i] * CIE1964X[i];
    ret[1] += spectrum[i] * CIE1964Y[i];
    ret[2] += spectrum[i] * CIE1964Z[i];
  }

  return ret;
}

Eigen::Vector3d getXYZTemp(unsigned int temp) {
  Eigen::Vector3d ret(0, 0, 0);
  double norm = blackbodySPD(560, temp);

  for (int i = 0; i < 471; i++) {
    double spd = blackbodySPD(i + 360, temp);
    ret[0] += spd * CIE1964X[i];
    ret[1] += spd * CIE1964Y[i];
    ret[2] += spd * CIE1964Z[i];
  }

  ret[0] /= ret[1];
  ret[2] /= ret[1];
  ret[1] = 1;
  ret *= 100;
  return ret;
}

Eigen::Vector3d getScaledColor(string gel, float intens) {
  auto color = getApproxColor(gel, intens);

  return Eigen::Vector3d(color[0] / color[1], 1, color[2] / color[1]) * 100;
}

Eigen::Vector3d convXYZtoRGB(Eigen::Vector3d color, RGBColorSpace cs) {
  // Vector is scaled by 1/100 bringing it inline withthe [0,1] range typically used by RGB.
  color /= 100;
#ifdef USE_C11_MAPS
  Eigen::Vector3d rgb = RGBToXYZ[cs].inverse() * color;
#else
  Eigen::Vector3d rgb = RGBToXYZ(cs).inverse() * color;
#endif

  if (cs == sRGB) {
    rgb[0] = XYZtosRGBCompand(rgb[0]);
    rgb[1] = XYZtosRGBCompand(rgb[1]);
    rgb[2] = XYZtosRGBCompand(rgb[2]);
  }

  rgb[0] = (rgb[0] < 0) ? 0 : rgb[0];
  rgb[1] = (rgb[1] < 0) ? 0 : rgb[1];
  rgb[2] = (rgb[2] < 0) ? 0 : rgb[2];

  return rgb;
}

Eigen::Vector3d convRGBtoXYZ(Eigen::Vector3d rgb, RGBColorSpace cs)
{
  return convRGBtoXYZ(rgb[0], rgb[1], rgb[2], cs);
}

Eigen::Vector3d convRGBtoXYZ(double r, double g, double b, RGBColorSpace cs)
{
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

Eigen::Vector3d convXYZtoxyY(Eigen::Vector3d color) {
  if (color[0] == 0 && color[1] == 0 && color[2] == 0)
    return Eigen::Vector3d(0, 0, 0);

  double denom = color[0] + color[1] + color[2];
  return Eigen::Vector3d(color[0] / denom, color[1] / denom, color[1]);
}

Eigen::Vector3d convXYZtoLab(Eigen::Vector3d xyz, ReferenceWhite rw)
{
  return convXYZtoLab(xyz, refWhites[rw]);
}

Eigen::Vector3d convXYZtoLab(Eigen::Vector3d xyz, Eigen::Vector3d rw)
{
  double L = 116 * labf(xyz[1] / rw[1]) - 16;
  double a = 500 * (labf(xyz[0] / rw[0]) - labf(xyz[1] / rw[1]));
  double b = 200 * (labf(xyz[1] / rw[1]) - labf(xyz[2] / rw[2]));
  return Eigen::Vector3d(L, a, b);
}

double labf(double val) {
  return (val > 216.0 / 24389.0) ? pow(val, 1.0 / 3.0) : ((24389.0 / 27.0) * val + 16) / 116.0;
}

Eigen::Vector2d convxytouv(Eigen::Vector3d xyY) {
  double u = (4 * xyY[0]) / (-2 * xyY[0] + 12 * xyY[1] + 3);
  double v = (9 * xyY[1]) / (-2 * xyY[0] + 12 * xyY[1] + 3);

  return Eigen::Vector2d(u, v);
}

Eigen::Vector2d convuvtoxy(Eigen::Vector2d uv) {
  double x = (9 * uv[0]) / (6 * uv[0] - 16 * uv[1] + 12);
  double y = (4 * uv[1]) / (6 * uv[0] - 16 * uv[1] + 12);

  return Eigen::Vector2d(x, y);
}

Eigen::Vector3d convXYZtoLUV(Eigen::Vector3d XYZ, ReferenceWhite rw) {
#ifdef USE_C11_MAPS
  return convXYZtoLUV(XYZ, refWhites[rw]);
#else
  return convXYZtoLUV(XYZ, refWhites(rw));
#endif
}

Eigen::Vector3d convXYZtoLUV(Eigen::Vector3d XYZ, Eigen::Vector3d rw) {
  auto rwuv = convxytouv(convXYZtoxyY(rw));
  auto uv = convxytouv(convXYZtoxyY(XYZ));

  double yn = (XYZ[1] / rw[1]);
  double lstar = (yn > pow(6.0 / 29.0, 3.0)) ? 116.0 * pow(yn, 1.0 / 3.0) - 16 : pow(29.0 / 3.0, 3.0) * yn;
  double ustar = 13 * lstar * (uv[0] - rwuv[0]);
  double vstar = 13 * lstar * (uv[1] - rwuv[1]);

  return Eigen::Vector3d(lstar, ustar, vstar);
}

Eigen::Vector3d convLUVtoXYZ(Eigen::Vector3d LUV, ReferenceWhite rw) {
#ifdef USE_C11_MAPS
  return convLUVtoXYZ(LUV, refWhites[rw]);
#else
  return convLUVtoXYZ(LUV, refWhites(rw));
#endif
}

Eigen::Vector3d convLUVtoXYZ(Eigen::Vector3d LUV, Eigen::Vector3d rw) {
  auto rwuv = convxytouv(convXYZtoxyY(rw));

  auto up = LUV[1] / (13 * LUV[0]) + rwuv[0];
  auto vp = LUV[2] / (13 * LUV[0]) + rwuv[1];

  auto Y = (LUV[0] > 8) ? rw[1] * pow((LUV[0] + 16.0) / 116.0, 3) : rw[1] * LUV[0] * pow(3.0 / 29.0, 3);
  auto X = Y * (9.0 * up) / (4.0 * vp);
  auto Z = Y * (12.0 - 3.0 * up - 20.0 * vp) / (4.0 * vp);
  return Eigen::Vector3d(X, Y, Z);
}

Eigen::Vector3d normalizeRGB(Eigen::Vector3d rgb) {
  double maxVal = max(rgb[0], max(rgb[1], rgb[2]));
  if (maxVal > 1)
    return rgb /= maxVal;
  else
    return rgb;
}

double clamp(double val, double min, double max) {
  double ret = val;
  ret = (ret < min) ? min : ret;
  ret = (ret > max) ? max : ret;

  return ret;
}

double sRGBtoXYZCompand(double val) {
  // this is some black magic right here but apparently it's a standard.
  return (val > 0.04045) ? pow(((val + 0.055) / 1.055), 2.4) : val / 12.92;
}

double XYZtosRGBCompand(double val) {
  return (val > 0.0031308) ? (1.055 * pow(val, 1 / 2.4) - 0.055) : val * 12.92;
}

double getTotalTrans(string gel) {
  // Multiple gels can be used (use a +)
  vector<string> gels;
  size_t gelbrk = gel.find("+");
  size_t firstchar = 0;
  while (gelbrk != string::npos) {
    gels.push_back(gel.substr(firstchar, gelbrk - firstchar));
    firstchar = gelbrk + 1;
    gelbrk = gel.find("+", firstchar);
  }
  gels.push_back(gel.substr(firstchar, string::npos));

  double trans = 1.0;
  for (const auto& g : gels) {
    if (gelsTrans.count(g) == 0) {
      Logger::log(WARN, "Gel " + g + " does not exist in the Lumiverse Color Library. Skipping...");
      continue;
    }
    trans *= gelsTrans[g];
  }

  return trans;
}

}
}