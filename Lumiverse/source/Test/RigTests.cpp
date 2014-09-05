#include "RigTests.h"

int RigTests::runTests() {
  int numPassed = 0;

  if (runTest([=]{ return this->rigStart(); }, "rigStart", 1)) {
    numPassed++;
  }
  else {
    return 0;
  }
  (runTest([=]{ return this->rigAddDeleteDevices(); }, "rigAddDeleteDevices", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->rigAddDeletePatch(); }, "rigAddDeletePatch", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceSetAdd(); }, "deviceSetAdd", 4)) ? numPassed++ : numPassed;

  return numPassed;
}

bool RigTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool RigTests::rigStart() {
  bool ret = true;
  m_testRig = new Rig("../../source/Test/testRig.json");

  if (m_testRig->getNumDevices() != 50) {
    cout << "Failed to load rig file\n";
    ret = false;
  }
  return ret;
}

bool RigTests::rigAddDeleteDevices() {
  bool ret = true;
  Device* newDevice = new Device("test", 100, "test");

  m_testRig->addDevice(newDevice);
  if (m_testRig->getDevice("test") == nullptr) {
    cout << "Could not add new device to rig\n";
    ret = false;
  }

  m_testRig->addDevice(newDevice);
  if (m_testRig->getNumDevices() != 51) {
    cout << "Added duplicate device to rig\n";
    ret = false;
  }

  m_testRig->deleteDevice("test");
  if (m_testRig->getDevice("test") != nullptr) {
    cout << "Device not deleted from rig\n";
    ret = false;
  }

  return ret;
}

bool RigTests::rigAddDeletePatch() {
  bool ret = true;
  Patch* newPatch = (Patch*)new DMXPatch();

  m_testRig->addPatch("test", newPatch);
  if (m_testRig->getPatch("test") == nullptr) {
    cout << "Patch not added to rig\n";
    ret = false;
  }

  m_testRig->deletePatch("test");
  if (m_testRig->getPatch("test") != nullptr) {
    cout << "Patch not deleted from rig\n";
    ret = false;
  }

  return ret;
}

bool RigTests::deviceSetAdd() {
  bool ret = true;

  DeviceSet expected(m_testRig);

  DeviceSet test(m_testRig);

  test.add(1);
  expected.add("s41");
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet channel add failed\n";
    ret = false;
  }

  test.clear();
  expected.clear();

  test.add(1, 5);
  expected.add("s41"); expected.add("par1"); expected.add("par2");
  expected.add("s42"); expected.add("s43");
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet channel range add failed\n";
    ret = false;
  }

  // more to come...

  return ret;
}