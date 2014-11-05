#include <string>
#include "LumiverseCore.h"
#include "LumiverseShowControl.h"
#include "lib/Eigen/Dense"

using namespace std;
using namespace Lumiverse;
using namespace Lumiverse::ShowControl;
using namespace Lumiverse::ColorUtils;

float getIntens(string id, Cue& cue) {
  auto kfs = cue.getParamData(id, "intensity");
  Keyframe k = *(kfs.begin());
  return ((LumiverseFloat*)k.val.get())->getVal();
}

float getScaledIntens(string id, Cue& cue, Rig& rig) {
  float intens = getIntens(id, cue);
  float trans = gelsTrans[rig[id]->getMetadata("gel")];
  return intens * trans;
}

Eigen::Vector3d getColor(string id, Cue& cue, Rig& rig) {
  float intens = getIntens(id, cue);
  string gel = rig[id]->getMetadata("gel");
  return getApproxColor(gel, intens);
}

int main(int argc, char**argv) {
  // Load rig, load cues
  // Go through each cue and _in the relevant area_ pick lights relevant to each feature
  // -Calculate intensity (modulated by gel transmission)
  // -Calculate chroma
  // Repeat until done

  Rig rig("E:/Users/falindrith/Dropbox/10-701_project/data/MLData.rig.json");
  Playback pb(&rig, "E:/Users/falindrith/Dropbox/10-701_project/data/MLData.playback.json");

  // Overwrite file each time we run this.
  ofstream outputFile;
  outputFile.open("E:/Users/falindrith/Dropbox/10-701_project/data/features.csv", ios::out | ios::trunc);

  auto scenes = pb.getCueList("Scenes")->getCueList();
  for (auto& scene : scenes) {
    if (scene.first == 1)
      continue;

    // Calculate scene features.
    auto data = scene.second.getCueData();
    cout << "Processing Scene " << scene.first << "...\n";

    if (scene.first < 25) {
      // Feature set 1 - Front right (intens, x, y)
      outputFile << getScaledIntens("s4_E_7", scene.second, rig) + getScaledIntens("fresnel_E_8", scene.second, rig) << ",";
      auto chroma = convXYZtoxyY(getColor("s4_E_7", scene.second, rig) + getColor("fresnel_E_8", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 2 - Front (intens, x, y)
      outputFile << getScaledIntens("s4_E1_3", scene.second, rig) + getScaledIntens("fresnel_E1_4", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_E1_3", scene.second, rig) + getColor("fresnel_E1_4", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 3 - Front left (intens, x, y)
      outputFile << getScaledIntens("s4_E_16", scene.second, rig) + getScaledIntens("fresnel_E_15", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_E_16", scene.second, rig) + getColor("fresnel_E_15", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 4 - high side right (intens, x, y)
      outputFile << getScaledIntens("s4_G_5", scene.second, rig) + getScaledIntens("s4_G_6", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_5", scene.second, rig) + getColor("s4_G_6", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 5 - high side left (intens, x, y)
      outputFile << getScaledIntens("s4_G_11", scene.second, rig) + getScaledIntens("s4_G_12", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_11", scene.second, rig) + getColor("s4_G_12", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 6 - top (intens, x, y)
      outputFile << getScaledIntens("s4_G_9", scene.second, rig) + getScaledIntens("fresnel_G_10", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_9", scene.second, rig) + getColor("fresnel_G_10", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 7 - back right (intens, x, y)
      outputFile << getScaledIntens("par_H1_6", scene.second, rig) + getScaledIntens("par_H1_7", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("par_H1_6", scene.second, rig) + getColor("par_H1_7", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 8 - back (intens, x, y)
      outputFile << getScaledIntens("s4_H1_10", scene.second, rig) + getScaledIntens("s4_H1_11", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_H1_11", scene.second, rig) + getColor("s4_H1_11", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 9 - back left (intens, x, y)
      outputFile << getScaledIntens("par_H1_13", scene.second, rig) + getScaledIntens("par_H1_13", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("par_H1_13", scene.second, rig) + getColor("par_H1_13", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 10 - side left (intens, x, y)
      outputFile << getScaledIntens("s4_BSR_1", scene.second, rig) + getScaledIntens("s4_BSR_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_BSR_1", scene.second, rig) + getColor("s4_BSR_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 11 - side right (intens, x, y)
      outputFile << getScaledIntens("s4_BSL_1", scene.second, rig) + getScaledIntens("s4_BSL_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_BSL_1", scene.second, rig) + getColor("s4_BSL_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1];
    }
    else {
      // Feature set 1 - Front right (intens, x, y)
      outputFile << getScaledIntens("s4_E_3", scene.second, rig) + getScaledIntens("fresnel_E_4", scene.second, rig) << ",";
      auto chroma = convXYZtoxyY(getColor("s4_E_3", scene.second, rig) + getColor("fresnel_E_4", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 2 - Front (intens, x, y)
      outputFile << getScaledIntens("s4_E1_1", scene.second, rig) + getScaledIntens("fresnel_E1_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_E1_1", scene.second, rig) + getColor("fresnel_E1_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 3 - Front left (intens, x, y)
      outputFile << getScaledIntens("s4_E_11", scene.second, rig) + getScaledIntens("fresnel_E_12", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_E_11", scene.second, rig) + getColor("fresnel_E_12", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 4 - high side right (intens, x, y)
      outputFile << getScaledIntens("s4_G_1", scene.second, rig) + getScaledIntens("s4_G_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_1", scene.second, rig) + getColor("s4_G_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 5 - high side left (intens, x, y)
      outputFile << getScaledIntens("s4_G_7", scene.second, rig) + getScaledIntens("s4_G_8", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_7", scene.second, rig) + getColor("s4_G_8", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 6 - top (intens, x, y)
      outputFile << getScaledIntens("s4_G_3", scene.second, rig) + getScaledIntens("fresnel_G_4", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_G_3", scene.second, rig) + getColor("fresnel_G_4", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 7 - back right (intens, x, y)
      outputFile << getScaledIntens("par_H1_1", scene.second, rig) + getScaledIntens("par_H1_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("par_H1_1", scene.second, rig) + getColor("par_H1_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 8 - back (intens, x, y)
      outputFile << getScaledIntens("s4_H1_4", scene.second, rig) + getScaledIntens("s4_H1_5", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_H1_4", scene.second, rig) + getColor("s4_H1_5", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 9 - back left (intens, x, y)
      outputFile << getScaledIntens("par_H1_8", scene.second, rig) + getScaledIntens("par_H1_9", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("par_H1_8", scene.second, rig) + getColor("par_H1_9", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 10 - side left (intens, x, y)
      outputFile << getScaledIntens("s4_BSR_1", scene.second, rig) + getScaledIntens("s4_BSR_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_BSR_1", scene.second, rig) + getColor("s4_BSR_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1] << ",";

      // Feature set 11 - side right (intens, x, y)
      outputFile << getScaledIntens("s4_BSL_1", scene.second, rig) + getScaledIntens("s4_BSL_2", scene.second, rig) << ",";
      chroma = convXYZtoxyY(getColor("s4_BSL_1", scene.second, rig) + getColor("s4_BSL_2", scene.second, rig));
      outputFile << chroma[0] << "," << chroma[1];
    }

    outputFile << "\n";
  }
}
