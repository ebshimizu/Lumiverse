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
  (runTest([=]{ return this->deviceSetRemove(); }, "deviceSetRemove", 5)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryID(); }, "queryID", 6)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryChannel(); }, "queryChannel", 7)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryMetadata(); }, "queryMetadata", 8)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryParameter(); }, "queryParameter", 9)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryMixed(); }, "queryMixed", 10)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->queryFilter(); }, "queryFilter", 11)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->dynamicQuery(); }, "dynamicQuery", 12)) ? numPassed++ : numPassed;

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

  // If adding an ID fails, the expected set won't have the right values so
  // we don't necessarily have to test it.
  test = test.add(1);
  expected = expected.add("s41");
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet channel add failed\n";
    ret = false;
  }

  test.clear();
  expected.clear();

  test = test.add(1, 5);
  expected = expected.add("s41").add("par1").add("par2").add("s42").add("s43");
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet channel range add failed\n";
    ret = false;
  }

  test.clear();
  expected.clear();

  DeviceSet set2(m_testRig);
  set2 = set2.add("s41");
  test = test.add("s42");
  expected = expected.add("s41").add("s42");
  test = test.add(set2);

  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet add other DeviceSet failed.\n";
    ret = false;
  }

  // Most of the other complex add functions are called from the queries,
  // so we'll test them over in that section.

  return ret;
}

bool RigTests::deviceSetRemove() {
  bool ret = true;

  DeviceSet expected(m_testRig);
  DeviceSet test(m_testRig);

  test = test.add("s41").add("s42");
  test = test.remove("s42");
  expected = expected.add("s41");
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet failed to remove device\n";
    ret = false;
  }

  test.clear();
  test = test.add(1, 5);
  test = test.remove(2, 5);

  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet failed to remove range of channels\n";
    ret = false;
  }

  test.clear();
  expected.clear();
  expected = expected.add(1, 5);
  test = test.add(1, 6).remove(6);
  if (!test.hasSameDevices(expected)) {
    cout << "DeviceSet failed to remove single channel\n";
    ret = false;
  }

  // Like add, most of the other complex remove functions are called from the queries.
  return ret;
}

bool RigTests::queryID() {
  bool ret = true;

  DeviceSet expected(m_testRig);
  expected = expected.add("s41");

  // single id
  if (!m_testRig->select("s41").hasSameDevices(expected)) {
    cout << "Failed to select single device from query\n";
    ret = false;
  }

  // multiple ids
  expected = expected.add("s42").add("s43");

  if (!m_testRig->select("s41,s42,s43").hasSameDevices(expected)) {
    cout << "Failed to select multiple devices by id from query\n";
    ret = false;
  }

  // Negation for IDs is a known unimeplemented feature of the library.
  // These tests will cause the current version of Lumiverse to fail the tests,
  // and should go away once this feature is implemented.
  // Negation
  if (m_testRig->select("!s41").size() != 49) {
    cout << "[UNIMPLEMENTED] Failed to select devices by id negation\n";
    ret = false;
  }

  // Multiple negation
  if (m_testRig->select("![s41, s42, s43]").size() != 47) {
    cout << "[UNIMPLEMENTED] Failed to select devices by multiple id negation\n";
    ret = false;
  }

  // IDs and channels
  expected.clear();
  expected = expected.select("s41,par1");
  if (!m_testRig->select("s41,#2").hasSameDevices(expected)) {
    cout << "Unable to combine id selector with other selectors\n";
    ret = false;
  }
  
  return ret;
}
 
bool RigTests::queryChannel() {
  bool ret = true;

  DeviceSet expected(m_testRig);
  expected = expected.add("s41");

  // Single channel
  if (!m_testRig->select("#1").hasSameDevices(expected)) {
    cout << "Failed to select single channel with query\n";
    ret = false;
  }

  // Channel Range
  expected.clear();
  expected = expected.select("s41, par1");
  if (!m_testRig->select("#1-2").hasSameDevices(expected)) {
    cout << "Failed to select multiple channels with query\n";
    ret = false;
  }

  // Multiple channels
  expected.clear();
  expected = expected.select("s41, s42");
  if (!m_testRig->select("#1,#3").hasSameDevices(expected)) {
    cout << "Failed to select multiple non-continuous channels\n";
    ret = false;
  }

  // Channel negation
  if (m_testRig->select("!#1-2").size() != 48) {
    cout << "Failed to select channels by negation\n";
    ret = false;
  }

  return ret;
}

bool RigTests::queryMetadata() {
  bool ret = true;

  DeviceSet expected(m_testRig);
  expected = expected.select("s41,s43,s413,s414,s415");

  // Equality (=)
  if (!m_testRig->select("$color=R02").hasSameDevices(expected)) {
    cout << "Failed to select devices by metadata equality\n";
    ret = false;
  }

  // *= (contains)
  expected.clear();
  expected = expected.select("#7-10,#27-30");
  if (!m_testRig->select("$color*=L1").hasSameDevices(expected)) {
    cout << "Failed to select devices by metadata contains\n";
    ret = false;
  }
  
  //$= (ends with)
  expected.clear();
  expected = expected.select("#11-12,#31-32");
  if (!m_testRig->select("$angle$=op").hasSameDevices(expected)) {
    cout << "Failed to select devices by metadata ends with\n";
    ret = false;
  }
  
  //^= (starts with)
  if (!m_testRig->select("$angle^=to").hasSameDevices(expected)) {
    cout << "Failed to select devices by metadata starts with\n";
    ret = false;
  }
  
  //!= (is not)
  if (m_testRig->select("$area!=2").size() != 24) {
    cout << "Failed to select devices by metadata not equal to\n";
    ret = false;
  }

  // ! *= (does not contain)
  if (m_testRig->select("!$color*=L1").size() != 36) {
    cout << "Failed to select devices by metadata does not contain\n";
    ret = false;
  }

  // ! $= (does not end with)
  if (m_testRig->select("!$angle^=to").size() != 40) {
    cout << "Failed to select devices by metadata does not end with\n";
    ret = false;
  }

  // ! ^= (does not start with)
  if (m_testRig->select("!$angle^=to").size() != 40) {
    cout << "Failed to select devices by metadata does not start with\n";
    ret = false;
  }

  return ret;
}

bool RigTests::queryParameter() {
  bool ret = true;

  // Only works for Float parameters at the moment.

  DeviceSet expected(m_testRig);
  // =
  expected = expected.add("s47");
  if (!m_testRig->select("@intensity=1.0f").hasSameDevices(expected)) {
    cout << "Failed to select devices by parameter equality\n";
    ret = false;
  }

  expected.clear();
  // >=
  expected = expected.select("s47,s46, par5, par6");
  if (!m_testRig->select("@intensity>=0.5f").hasSameDevices(expected)) {
    cout << "Failed to select devices by parameter >=\n";
    ret = false;
  }

  // <=
  if (m_testRig->select("@intensity<=0.5f").size() != 43) {
    cout << "Failed to select devices by parameter <=\n";
    ret = false;
  }

  return ret;

}

bool RigTests::queryMixed() {
  bool ret = true;

  DeviceSet expected(m_testRig);

  expected = expected.select("s48,s420,s41,par1,s42,s44");
  if (!m_testRig->select("$color=R80,#1-3,s44").hasSameDevices(expected)) {
    cout << "Failed to select devices with mixed query\n";
    ret = false;
  }

  return ret;
}

bool RigTests::queryFilter() {
  bool ret = true;

  DeviceSet expected(m_testRig);
  expected = expected.select("par10, s414");
  if (!m_testRig->select("$area=1[$angle=front]").hasSameDevices(expected)) {
    cout << "Failed to filter devices based on metadata\n";
    ret = false;
  }


  expected = expected.add("s4LED2").add("s413").add("par9").add("s415").add("par11");
  if (!m_testRig->select("$area=1[$angle*=front]").hasSameDevices(expected)) {
    cout << "Failed to filter devices based on metadata contains\n";
    ret = false;
  }

  expected.clear();
  expected = expected.add("s42");
  if (!m_testRig->select("#1-10[$color=L201]").hasSameDevices(expected)) {
    cout << "Failed to filter devices from channel selection with metadata equals\n";
    ret = false;
  }

  return ret;
}

bool RigTests::dynamicQuery() {
  m_testRig->resetDevices();

  bool ret = true;
  DynamicDeviceSet dynam(m_testRig, "@intensity>0.5f");
  
  DeviceSet expected(m_testRig);
  expected = expected.select("");
  
  if (dynam.size() != 0) {
    cout << "Dynamic DeviceSet shouldn't have any devices in it. (No devices with intensity > 0.5).\n";
    ret = false;
  }

  m_testRig->getDevice("s41")->setParam("intensity", 1.0f);
  expected = expected.select("s41");

  if (!dynam.getDeviceSet().hasSameDevices(expected)) {
    cout << "Dynamic DeviceSet should have one device in it (s41)\n";
    cout << "Info: " << dynam.info() << "\n";
    ret = false;
  }

  m_testRig->getDevice("s42")->setParam("intensity", 0.7f);
  expected = expected.add("s42");

  if (!dynam.getDeviceSet().hasSameDevices(expected)) {
    cout << "Dynamic DeviceSet should have two devices in it (s41, s42)\n";
    cout << "Info: " << dynam.info() << "\n";
    ret = false;
  }

  m_testRig->getDevice("s43")->setParam("intensity", 0.2f);

  if (!dynam.getDeviceSet().hasSameDevices(expected)) {
    cout << "Dynamic DeviceSet should have two devices in it (s41, s42)\n";
    cout << "Info: " << dynam.info() << "\n";
    ret = false;
  }
  return ret;
}