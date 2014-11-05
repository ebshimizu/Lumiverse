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
  if (gelsCoarse.count(gel) == 0) {
    Logger::log(ERR, "Gel " + gel + " does not exist in the Lumiverse Color Library.");
    return refWhites[A];
  }

  // We assume a linear ambershift of an incandescent fixture.
  // Assuming that a lamp approaches incandescent when it gets dim and 
  // approaches manufacturer spec of 3250K at full brightness.
  int temp = (int)(2500 + 750 * intens);

  auto color = gelsCoarse[gel];
  Eigen::Vector3d ret(0, 0, 0);

  // For each wavelength, calculate SPD, multiply by transmission, multiply by CMF,
  // keep running sum.
  // We'll iterate over the color's data points to make things a bit nicer.
  for (int i = 0; i < 20; i++) {
    if (i == 19) {
      // Special case for final element
      double trans = color[i];
      int idx = i * 20;
      double spd = blackbodySPD(idx + 360, temp);
      
      ret[0] += spd * trans * CIE1964X[idx];
      ret[1] += spd * trans * CIE1964Y[idx];
      ret[2] += spd * trans * CIE1964Z[idx];
    }
    else {
      for (int j = 0; j < 20; j++) {
        double trans = color[i] + (color[i + 1] - color[i]) * (double)(j / 20.0);
        int idx = i * 20;
        double spd = blackbodySPD(idx + 360, temp);
        
        ret[0] += spd * trans * CIE1964X[idx];
        ret[1] += spd * trans * CIE1964Y[idx];
        ret[2] += spd * trans * CIE1964Z[idx];
      }
    }
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

Eigen::Vector3d convXYZtoxyY(Eigen::Vector3d color) {
  if (color[0] == 0 && color[1] == 0 && color[2] == 0)
    return Eigen::Vector3d(0, 0, 0);

  double denom = color[0] + color[1] + color[2];
  return Eigen::Vector3d(color[0] / denom, color[1] / denom, color[1]);
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

}
}