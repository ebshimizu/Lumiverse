#include <LumiverseCore.h>

using namespace Lumiverse;

int main(int argc, char** argv) {
  Rig* rig = new Rig("C:/Users/falindrith/OneDrive/Documents/research/attributes_project/scenes/debug/debug.rig.json");

  rig->init();

  ArnoldAnimationPatch* p = (ArnoldAnimationPatch*)rig->getPatch("arnold");

  Device* l = rig->getDevice("light");
  
  l->getIntensity()->setValAsPercent(1);
  l->setColorRGBRaw("color", 1, 1, 1);

  p->renderSingleFrame(rig->getDeviceRaw(), "./", "arnold_full");

  l->setColorRGBRaw("color", 0.75, 0.3, 0);
  p->renderSingleFrame(rig->getDeviceRaw(), "./", "arnold_tinted");
  delete rig;

  Rig crig("C:/Users/falindrith/OneDrive/Documents/research/attributes_project/scenes/debug/debug_cache.rig.json");
  ArnoldAnimationPatch* pc = (ArnoldAnimationPatch*)crig.getPatch("arnold");
  Device* cl = crig.getDevice("light");

  crig.init();
  cl->getIntensity()->setValAsPercent(1);
  cl->setColorRGBRaw("color", 1, 1, 1);

  pc->renderSingleFrame(crig.getDeviceRaw(), "./", "cache_full");
  cl->setColorRGBRaw("color", 0.75, 0.3, 0);
  pc->renderSingleFrame(crig.getDeviceRaw(), "./", "cache_tinted");

  return 0;
}