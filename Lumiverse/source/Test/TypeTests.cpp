#include "TypeTests.h"

int TypeTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->floatTests(); }, "floatTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->enumTests(); }, "enumTests", 2)) ? numPassed++ : numPassed;
  //(runTest([=]{ return this->colorTests(); }, "colorTests", 3)) ? numPassed++ : numPassed;
  //(runTest([=]{ return this->oriTests(); }, "oriTests", 4)) ? numPassed++ : numPassed;
  //(runTest([=]{ return this->utilTests(); }, "utilTests", 5)) ? numPassed++ : numPassed;

  return numPassed;
}

bool TypeTests::runTest(std::function<bool()> t, string testName, int testNum) {
  bool pass;

  if (pass = t()) {
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