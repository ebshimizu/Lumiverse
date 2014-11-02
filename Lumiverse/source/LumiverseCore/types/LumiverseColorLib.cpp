#include "LumiverseColorLib.h"

namespace Lumiverse {
namespace ColorUtils{ 

double blackbodySPD(unsigned int nm, unsigned int temp) {
  double h = 6.626176e-34;
  double c = 2.99792458e8;
  double k = 1.380662e-23;
  double m = nm * (double) 10e-9;
  double c1 = 2 * M_PI * h * c * c;
  double c2 = (h * c) / k;

  return c1 / (pow(h, 5) * (exp(c2 / (temp * m)) - 1));
}

Eigen::Vector3d getApproxColor(string gel, float intens) {
  if (gelsCoarse.count(gel) == 0) {
    Logger::log(ERR, "Gel " + gel + " does not exist in the Lumiverse Color Library.");
    return Eigen::Vector3d();
  }
  
  // We assume a linear ambershift of an incandescent fixture.
  // Assuming that a lamp approaches incandescent (source A) when it gets dim,
  // and approaches the manufacturer's spec of 3250 at full intensity.
  int temp = (int) (2700 + 550.0 * intens);

  auto color = gelsCoarse[gel];
  Eigen::Vector3d ret(0, 0, 0);

  // Normalization factor for SPD. SPD will return 100 at 560nm.
  double norm = blackbodySPD(560, temp);

  // For each wavelength, calculate SPD, multiply by transmission, multiply by CMF,
  // keep running sum.
  // We'll iterate over the color's data points to make things a bit nicer.
  for (int i = 0; i < 20; i++) {
    if (i == 19) {
      // Special case for final element
      double trans = color[i];
      int idx = i * 20;
      double spd = (blackbodySPD(idx + 360, temp) / norm) * 100;
      
      ret[0] += spd * trans * CIE1964X[idx];
      ret[1] += spd * trans * CIE1964Y[idx];
      ret[2] += spd * trans * CIE1964Z[idx];
    }
    else {
      for (int j = 0; j < 20; j++) {
        double trans = color[i] + (color[i + 1] - color[i]) * (double)(j / 20.0);
        int idx = i * 20;
        double spd = (blackbodySPD(idx + 360, temp) / norm) * 100;
        
        ret[0] += spd * trans * CIE1964X[idx];
        ret[1] += spd * trans * CIE1964Y[idx];
        ret[2] += spd * trans * CIE1964Z[idx];
      }
    }
  }

  return ret;
}

}
}