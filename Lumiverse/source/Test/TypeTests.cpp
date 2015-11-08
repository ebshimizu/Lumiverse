#include "TypeTests.h"

int TypeTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->floatTests(); }, "floatTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->enumTests(); }, "enumTests", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->colorTests(); }, "colorTests", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->oriTests(); }, "oriTests", 4)) ? numPassed++ : numPassed;

  return numPassed;
}

bool TypeTests::runTest(std::function<bool()> t, string testName, int testNum) {
  bool pass;

  if ((pass = t())) {
    cout << "[ OK ]";
  }
  else {
    cout << "[FAIL]";
  }
  cout << " (" << testNum << "/" << m_numTests << ") " << testName << "\n";
  return pass;
}

bool TypeTests::floatTests() {
  bool ret = true;

  LumiverseFloat f(1, 0, 10, 0);

  // Basic functionality
  if (f.getMax() != 10) {
    cout << "LumiverseFloat maximum expected 1 but received " << f.getMax() << "\n";
    ret = false;
  }
  if (f.getMin() != 0) {
    cout << "LumiverseFloat minimum expected 0 but received " << f.getMin() << "\n";
    ret = false;
  }
  if (f.getDefault() != 0) {
    cout << "LumiverseFloat default expected 0 but received " << f.getDefault() << "\n";
    ret = false;
  }
  if (f.asPercent() != 0.1f) {
    cout << "LumiverseFloat asPercent expected 0.1 but received " << f.asPercent() << "\n";
    ret = false;
  }

  f += 100000000;
  if (f != 10.0f) {
    cout << "LumiverseFloat exceeded maximum value\n";
    ret = false;
  }

  f -= 100000000;
  if (f != 0) {
    cout << "LumiverseFloat exceeded minimum value\n";
    ret = false;
  }

  f += 5;
  if (f.isDefault()) {
    cout << "LumiverseFloat is not at default value but it thinks it is\n";
    ret = false;
  }

  f.reset();
  if (!f.isDefault()) {
    cout << "LumiverseFloat was reset but is not at default values\n";
    ret = false;
  }

  LumiverseFloat copy(f);
  if (f != copy) {
    cout << "Error copying LumiverseFloat\n";
    ret = false;
  }

  return ret;
}

bool TypeTests::enumTests() {
  bool ret = true;

  map<string, int> keys;
  keys["OPTION1"] = 0;
  keys["OPTION2"] = 127;
  keys["OPTION3"] = 180;
  keys["OPTION4"] = 210;

  LumiverseEnum e(keys);

  e.setDefault("OPTION1");
  e.reset();

  if (e.getVal() != "OPTION1") {
    cout << "LumiverseEnum not properly initialized to default\n";
    ret = false;
  }

  e.setVal("OPTION3");
  if (e.getVal() != "OPTION3") {
    cout << "LumiverseEnum set error\n";
    ret = false;
  }

  e.setVal(127);
  if (e.getVal() != "OPTION2") {
    cout << "Error setting value numerically. Expected: OPTION2. Received: " << e.getVal() << "\n";
    ret = false;
  }

  e.setMode(LumiverseEnum::FIRST);
  e.setVal("OPTION1");
  if (e.getRangeVal() != 0) {
    cout << "LumiverseEnum mode FIRST not working. Expected 0. Received: " << e.getRangeVal() << "\n";
    ret = false;
  }

  e.setMode(LumiverseEnum::LAST);
  e.setVal("OPTION1");
  if (e.getRangeVal() != 126) {
    cout << "LumiverseEnum mode LAST not working. Expected 126. Received: " << e.getRangeVal() << "\n";
    ret = false;
  }

  e.setMode(LumiverseEnum::CENTER);
  e.setVal("OPTION1");
  if (e.getRangeVal() != 63) {
    cout << "LumiverseEnum mode CENTER not working. Expected 63. Received: " << e.getRangeVal() << "\n";
    ret = false;
  }

  if (e.setVal("NOTANOPTION", 0)) {
    cout << "LumiverseEnum set value to invalid option\n";
    ret = false;
  }

  e.setTweak(1000);
  if (e.getTweak() != 1) {
    cout << "LumiverseEnum tweak value out of maximum bound\n";
    ret = false;
  }

  e.setTweak(-1);
  if (e.getTweak() != 0) {
    cout << "LumiverseEnum tweak value out of minimum bound\n";
    ret = false;
  }

  e.setVal(10000);
  if (e.getRangeVal() != 255) {
    cout << "LumiverseEnum numeric value out of range\n";
    ret = false;
  }

  return ret;
}

bool TypeTests::colorTests() {
  bool ret = true;

  map<string, Eigen::Vector3d> basis;
  basis["Blue"] = Eigen::Vector3d(4.30497, 3.859103, 29.365243);
  basis["Green"] = Eigen::Vector3d(5.59857, 25.901501, 4.084567);
  basis["Red"] = Eigen::Vector3d(13.16544, 5.868346, 0.000025);
  basis["White"] = Eigen::Vector3d(81.33195, 79.590576, 47.302138);

  LumiverseColor c(basis);

  c["Blue"] = 0.5;
  if (c.getColorChannel("Blue") != 0.5) {
    cout << "LumiverseColor error setting color channel. Expected: 0.5. Received: " << c["Blue"] << "\n";
    ret = false;
  }

  c["White"] = c["Green"] = c["Red"] = 0.5;
  auto calcXYZ = c["White"] * basis["White"] + c["Red"] * basis["Red"] + c["Blue"] * basis["Blue"] + c["Green"] * basis["Green"];
  if (c.getXYZ() != calcXYZ) {
    cout << "LumiverseColor error calculating XYZ coordinates.\n";
    ret = false;
  }

  c.reset();
  if (!c.isDefault()) {
    cout << "LumiverseColor error resetting to default color.\n";
    ret = false;
  }

  return ret;
}

bool TypeTests::oriTests() {
  bool ret = true;

  LumiverseOrientation o(0, DEGREE, 180, 360, 0);

  if (o.getVal() != 0) {
    cout << "Error initializing orientation\n";
    ret = false;
  }

  o.setVal(270);
  if (o.getVal() != 270) {
    cout << "Error setting new value for orientation\n";
    ret = false;
  }

  o.setVal(M_PI, RADIAN);
  if (o.getVal() != 180) {
    cout << "Error setting values of different units. Expected: 180. Received: " << o.getVal() << "\n";
    ret = false;
  }

  LumiverseOrientation o2(90);
  o += o2;
  if (o.getVal() != 270) {
    cout << "+= op error. Expected: 270. Received: " << o.getVal() << "\n";
    ret = false;
  }

  o.setVal(0);
  LumiverseOrientation o3(M_PI, RADIAN);
  o += o3;
  if (o.getVal() != 180) {
    cout << "+= op error for differnt units. Expected: 180. Received: " << o.getVal() << "\n";
    ret = false;
  }

  o -= o3;
  if (o.getVal() != 0) {
    cout << "-= op error for different units. Expected: 0. Received: " << o.getVal() << "\n";
    ret = false;
  }

  o.setVal(90);
  o *= 2;
  if (o.getVal() != 180) {
    cout << "*= op error. Expected: 180. Received: " << o.getVal() << "\n";
    ret = false;
  }

  o /= 2;
  if (o.getVal() != 90) {
    cout << "/= op error. Expected: 90. Received: " << o.getVal() << "\n";
    ret = false;
  }

  LumiverseOrientation t(M_PI, RADIAN);
  if (!(o < t)) {
    cout << "< comparison op error. Claimed 90deg > PI rad\n";
    ret = false;
  }

  LumiverseOrientation t2(M_PI_2, RADIAN);
  if (!(o == t2 )) {
    cout << "== equality op error. Claimed 90deg != PI/2 rad\n";
    ret = false;
  }

  o.setVal(310941423);
  if (o.getVal() != 360) {
    cout << "Orientation out of maximum bound. Expected: 360. Recevied: " << o.getVal() << "\n";
    ret = false;
  }

  o.setVal(-31434);
  if (o.getVal() != 0) {
    cout << "Orientation out of minimal bound. Expected: 0. Received: " << o.getVal() << "\n";
    ret = false;
  }

  o.setVal(180);
  o.setUnit(RADIAN);
  if (abs(o.getVal() - M_PI) > 1e-6) {
    cout << "Orientation unit conversion value failure. Expected: " << M_PI << ". Received: " << o.getVal() << "\n";
    ret = false;
  }

  if (abs(o.getMax() - (M_PI * 2)) > 1e-6) {
    cout << "Orientation unit conversion maximum bound faliure. Expected: " << M_PI * 2 << ". Received: " << o.getMax() << "\n";
    ret = false;
  }

  o.setVal(0);
  o.reset();
  if (abs(o.getVal() - M_PI) > 1e-6) {
    cout << "Orientation didn't reset to default. Expected: " << M_PI << ". Received: " << o.getVal() << "\n";
    ret = false;
  }

  return ret;
}
