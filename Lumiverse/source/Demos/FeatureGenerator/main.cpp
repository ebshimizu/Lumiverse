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

float getIntens(string id, Rig& rig) {
  return (((LumiverseFloat*)rig.getDevice(id)->getParam("intensity"))->getVal());
}

float getScaledIntens(string id, Rig& rig) {
  float intens = getIntens(id, rig);
  float trans = gelsTrans[rig[id]->getMetadata("gel")];
  return intens * trans;
}

float getScaledIntens(string id, Cue& cue, Rig& rig) {
  float intens = getIntens(id, cue);
  float trans = gelsTrans[rig[id]->getMetadata("gel")];
  return intens * trans;
}

Eigen::Vector3d getColor(string id, Rig& rig) {
  float intens = getIntens(id, rig);
  string gel = rig[id]->getMetadata("gel");
  return getApproxColor(gel, intens);
}

Eigen::Vector3d getColor(string id, Cue& cue, Rig& rig) {
  float intens = getIntens(id, cue);
  string gel = rig[id]->getMetadata("gel");
  return getApproxColor(gel, intens);
}

vector<double> getFeatureTupleFromRig(Rig& rig, string query) {
  vector<double> feats; // Intens, x, y.

  DeviceSet devices = rig.query(query);
  Eigen::Vector3d color(0,0,0);
  double intens = 0;
  for (auto& d : devices.getDevices()) {
    intens += getScaledIntens(d->getId(), rig);
    color += getColor(d->getId(), rig);
  }
  feats.push_back(intens);
  auto chroma = convXYZtoxyY(color);
  feats.push_back(chroma[0]);
  feats.push_back(chroma[1]);

  return feats;
}

vector<double> getFeatureTupleFromCue(Rig& rig, Cue& cue, string query) {
  vector<double> feats; // Intens, x, y.

  DeviceSet devices = rig.query(query);
  Eigen::Vector3d color(0,0,0);
  double intens = 0;
  for (auto& d : devices.getDevices()) {
    intens += getScaledIntens(d->getId(), cue, rig);
    color += getColor(d->getId(), cue, rig);
  }
  feats.push_back(intens);
  auto chroma = convXYZtoxyY(color);
  feats.push_back(chroma[0]);
  feats.push_back(chroma[1]);

  return feats;
}

float objective(vector<double> target, vector<double> source) {
  // vcetors are in intens, x, y format.
  // distance function pretty much.
  
  // if source and dest intensity is zero the objective function is 0
  if (target[0] == 0 && source[0] == 0)
    return 0;

  return sqrt((target[0] - source[0]) * (target[0] - source[0]) + (target[1] - source[1]) * (target[1] - source[1]) + (target[2] - source[2]) * (target[2] - source[2]));
}

// Approximates the derivative of the weird objective function we have for a given device.
float approxDeriv(string id, Rig& rig, Cue& cue, string query, string cQuery) {
  float eps = 0.001;
  float origVal;
  Device* d = rig.getDevice(id);
  d->getParam("intensity", origVal);

  d->setParam("intensity", origVal - (eps / 2));
  auto origFeat = getFeatureTupleFromRig(rig, query);

  d->setParam("intensity", origVal + (eps / 2));

  auto newFeat = getFeatureTupleFromRig(rig, query);

  // Reset rig
  d->setParam("intensity", origVal);
  auto cueFeat = getFeatureTupleFromCue(rig, cue, cQuery);
  float newobj = objective(newFeat, cueFeat);
  float oldobj = objective(origFeat, cueFeat);

  // calculate derivative of objective function
  return (newobj - oldobj) / eps;
}

// Optimizes a particular section in the feature vector and retuns the intensity for
// the closest match.
// rig is essentially the target, cue is the source
map<string, float> optimizeSection(Rig& rig, Cue& cue, string rigQuery, string cueQuery) {
  DeviceSet opt = rig.query(rigQuery);
  map<string, float> old_x;
  map<string, float> new_x;
  
  for (auto& d : opt.getDevices()) {
    float intens = getScaledIntens(d->getId(), rig);
    old_x[d->getId()] = intens;
    new_x[d->getId()] = intens;
  }

  bool allZero = true;
  DeviceSet cueSet = rig.query(cueQuery);
  for (auto& d : cueSet.getDevices()) {
    float intens = getScaledIntens(d->getId(), cue, rig);
    allZero &= (intens == 0);
  }

  if (allZero) {
    for (auto& d : opt.getDevices()) {
      old_x[d->getId()] = 0;
      rig.getDevice(d->getId())->setParam("intensity", 0.0f);
    }
    return old_x;
  }

  double eps = 0.0001;
  double lr = 0.005;

  while (1) {
    // For each component do gradient descent.
    for (auto& kvp : old_x) {
      old_x[kvp.first] = new_x[kvp.first];
      new_x[kvp.first] = kvp.second - lr * approxDeriv(kvp.first, rig, cue, rigQuery, cueQuery);
      new_x[kvp.first] = (new_x[kvp.first] > 1) ? 1 : ((new_x[kvp.first] < 0) ? 0 : new_x[kvp.first]);
    }

    // Update rig to be ready for next cycle
    for (auto& kvp : new_x) {
      rig.getDevice(kvp.first)->setParam("intensity", kvp.second);
    }

    // Check if we should abort
    bool abort = true;
    for (auto& kvp : new_x) {
      abort &= (abs(old_x[kvp.first] - new_x[kvp.first]) < eps);
    }

    if (abort) break;
  }

  float obj = objective(getFeatureTupleFromRig(rig, rigQuery), getFeatureTupleFromCue(rig, cue, cueQuery));
  cout << "Optimization finished. Objective function: " << obj << "\n";
  return new_x;
}

vector<double> getFeatureVec(int area, Playback& pb, Rig& rig, float cue) {
  vector<double> features;
  auto scene = pb.getCueList("Scenes")->getCue(cue);

  if (area == 2) {
    // Feature set 1 - Front right (intens, x, y)
    features.push_back(getScaledIntens("s4_E_7", *scene, rig) + getScaledIntens("fresnel_E_8", *scene, rig));
    auto chroma = convXYZtoxyY(getColor("s4_E_7", *scene, rig) + getColor("fresnel_E_8", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 2 - Front (intens, x, y)
    features.push_back(getScaledIntens("s4_E1_3", *scene, rig) + getScaledIntens("fresnel_E1_4", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_E1_3", *scene, rig) + getColor("fresnel_E1_4", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 3 - Front left (intens, x, y)
    features.push_back(getScaledIntens("s4_E_16", *scene, rig) + getScaledIntens("fresnel_E_15", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_E_16", *scene, rig) + getColor("fresnel_E_15", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 4 - high side right (intens, x, y)
    features.push_back(getScaledIntens("s4_G_5", *scene, rig) + getScaledIntens("s4_G_6", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_5", *scene, rig) + getColor("s4_G_6", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 5 - high side left (intens, x, y)
    features.push_back(getScaledIntens("s4_G_11", *scene, rig) + getScaledIntens("s4_G_12", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_11", *scene, rig) + getColor("s4_G_12", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 6 - top (intens, x, y)
    features.push_back(getScaledIntens("s4_G_9", *scene, rig) + getScaledIntens("fresnel_G_10", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_9", *scene, rig) + getColor("fresnel_G_10", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 7 - back right (intens, x, y)
    features.push_back(getScaledIntens("par_H1_6", *scene, rig) + getScaledIntens("par_H1_7", *scene, rig));
    chroma = convXYZtoxyY(getColor("par_H1_6", *scene, rig) + getColor("par_H1_7", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 8 - back (intens, x, y)
    features.push_back(getScaledIntens("s4_H1_10", *scene, rig) + getScaledIntens("s4_H1_11", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_H1_11", *scene, rig) + getColor("s4_H1_11", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 9 - back left (intens, x, y)
    features.push_back(getScaledIntens("par_H1_13", *scene, rig) + getScaledIntens("par_H1_13", *scene, rig));
    chroma = convXYZtoxyY(getColor("par_H1_13", *scene, rig) + getColor("par_H1_13", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 10 - side left (intens, x, y)
    features.push_back(getScaledIntens("s4_BSR_1", *scene, rig) + getScaledIntens("s4_BSR_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_BSR_1", *scene, rig) + getColor("s4_BSR_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 11 - side right (intens, x, y)
    features.push_back(getScaledIntens("s4_BSL_1", *scene, rig) + getScaledIntens("s4_BSL_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_BSL_1", *scene, rig) + getColor("s4_BSL_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);
  }
  else {
    // Feature set 1 - Front right (intens, x, y)
    features.push_back(getScaledIntens("s4_E_3", *scene, rig) + getScaledIntens("fresnel_E_4", *scene, rig));
    auto chroma = convXYZtoxyY(getColor("s4_E_3", *scene, rig) + getColor("fresnel_E_4", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 2 - Front (intens, x, y)
    features.push_back(getScaledIntens("s4_E1_1", *scene, rig) + getScaledIntens("fresnel_E1_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_E1_1", *scene, rig) + getColor("fresnel_E1_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 3 - Front left (intens, x, y)
    features.push_back(getScaledIntens("s4_E_11", *scene, rig) + getScaledIntens("fresnel_E_12", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_E_11", *scene, rig) + getColor("fresnel_E_12", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 4 - high side right (intens, x, y)
    features.push_back(getScaledIntens("s4_G_1", *scene, rig) + getScaledIntens("s4_G_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_1", *scene, rig) + getColor("s4_G_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 5 - high side left (intens, x, y)
    features.push_back(getScaledIntens("s4_G_7", *scene, rig) + getScaledIntens("s4_G_8", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_7", *scene, rig) + getColor("s4_G_8", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 6 - top (intens, x, y)
    features.push_back(getScaledIntens("s4_G_3", *scene, rig) + getScaledIntens("fresnel_G_4", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_G_3", *scene, rig) + getColor("fresnel_G_4", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 7 - back right (intens, x, y)
    features.push_back(getScaledIntens("par_H1_1", *scene, rig) + getScaledIntens("par_H1_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("par_H1_1", *scene, rig) + getColor("par_H1_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 8 - back (intens, x, y)
    features.push_back(getScaledIntens("s4_H1_4", *scene, rig) + getScaledIntens("s4_H1_5", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_H1_4", *scene, rig) + getColor("s4_H1_5", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 9 - back left (intens, x, y)
    features.push_back(getScaledIntens("par_H1_8", *scene, rig) + getScaledIntens("par_H1_9", *scene, rig));
    chroma = convXYZtoxyY(getColor("par_H1_8", *scene, rig) + getColor("par_H1_9", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 10 - side left (intens, x, y)
    features.push_back(getScaledIntens("s4_BSR_1", *scene, rig) + getScaledIntens("s4_BSR_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_BSR_1", *scene, rig) + getColor("s4_BSR_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);

    // Feature set 11 - side right (intens, x, y)
    features.push_back(getScaledIntens("s4_BSL_1", *scene, rig) + getScaledIntens("s4_BSL_2", *scene, rig));
    chroma = convXYZtoxyY(getColor("s4_BSL_1", *scene, rig) + getColor("s4_BSL_2", *scene, rig));
    features.push_back(chroma[0]);
    features.push_back(chroma[1]);
  }

  return features;
}

int main(int argc, char**argv) {
  // Load rig, load cues
  // Go through each cue and _in the relevant area_ pick lights relevant to each feature
  // -Calculate intensity (modulated by gel transmission)
  // -Calculate chroma
  // Repeat until done

  Rig rig("C:/Users/eshimizu/Dropbox/10-701_project/data/MLData.rig.json");
  Playback pb(&rig, "C:/Users/eshimizu/Dropbox/10-701_project/data/MLData.playback.json");

  // Teting out attribute transfer using scene 10 -> scene 37 testing for attribute 2 (dark)
  // We assume the rig is pre-initialized to scene 10.
  pb.getProgrammer()->clearAndReset();
  Cue* c = pb.getCueList("Scenes")->getCue(37);
  optimizeSection(rig, *c, "$angle=front right[$Area=2]", "$angle=front right[$Area=1]");
  optimizeSection(rig, *c, "$angle=front[$Area=2]", "$angle=front[$Area=1]");
  optimizeSection(rig, *c, "$angle=front left[$Area=2]", "$angle=front left[$Area=1]");
  optimizeSection(rig, *c, "$angle=high side right[$Area=2]", "$angle=high side right[$Area=1]");
  optimizeSection(rig, *c, "$angle=high side left[$Area=2]", "$angle=high side left[$Area=1]");
  optimizeSection(rig, *c, "$angle=top[$Area=2]", "$angle=top[$Area=1]");
  optimizeSection(rig, *c, "$angle=back right[$Area=2]", "$angle=back right[$Area=1]");
  optimizeSection(rig, *c, "$angle=back[$Area=2]", "$angle=back[$Area=1]");
  optimizeSection(rig, *c, "$angle=back left[$Area=2]", "$angle=back left[Area=1]");
  optimizeSection(rig, *c, "$angle=side left", "$angle=side left");
  optimizeSection(rig, *c, "$angle=side right", "$angle=side right");

  Cue newCue(&rig);
  pb.getCueList("Transfer")->storeCue(1.0, newCue, true);
  rig.save("C:/Users/eshimizu/Dropbox/10-701_project/data/MLData_transfer.rig.json", true);
  pb.save("C:/Users/eshimizu/Dropbox/10-701_project/data/MLData_transfer.playback.json", true);

  cout << "Done\n";
  getch();

  // Overwrite file each time we run this.
  //ofstream outputFile;
  //outputFile.open("C:/Users/eshimizu/Dropbox/10-701_project/data/features_refactor.csv", ios::out | ios::trunc);

  //auto scenes = pb.getCueList("Scenes")->getCueList();
  //for (auto& scene : scenes) {
  //  if (scene.first == 1)
  //    continue;

    // Calculate scene features.
  //  cout << "Processing Scene " << scene.first << "...\n";
  //  auto features = getFeatureVec((scene.first < 25) ? 2 : 1, pb, rig, scene.first);

  //  bool first = true;
  //  for (const auto& f : features) {
  //    if (!first) {
  //      outputFile << ",";
  //    }
  //    outputFile << f;
  //    first = false;
  //  }

  //  outputFile << "\n";
  //}

  //outputFile.close();
}
