#include "DeviceTests.h"

int DeviceTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->deviceCreation(); }, "deviceCreation", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceAccessors(); }, "deviceAccessors", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceCopy(); }, "deviceCopy", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceSerialization(); }, "deviceSerialization", 4)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->devicePropertyManipulation(); }, "devicePropertyManipulation", 5)) ? numPassed++ : numPassed;

  return numPassed;
}

bool DeviceTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool DeviceTests::deviceCreation() {
  Device d("test", 1, "ETC Source 4 26deg");

  return (d.getId() == "test" &&
    d.getChannel() == 1 &&
    d.getType() == "ETC Source 4 26deg");
}

bool DeviceTests::deviceCopy(){
  Device d("test1", 1, "ETC Source 4 26deg");

  if (d.setParam("intensity", 1.0f)) {
    cout << "[ERROR] deviceCopy: Empty device already had intensity parameter\n";
    return false;
  }
  if (!d.setMetadata("test", "myVal")) {
    cout << "[ERROR] deviceCopy: Empty device already had metadata parameter 'test'\n";
    return false;
  }

  Device copy(d);

  if (d.getId() != copy.getId()) {
    cout << "[ERROR] deviceCopy: IDs do not match\n";
    return false;
  }
  if (d.getChannel() != copy.getChannel()) {
    cout << "[ERROR] deviceCopy: Channels do not match\n";
    return false;
  }
  if (d.getType() != copy.getType()) {
    cout << "[ERROR] deviceCopy: Device types do not match\n";
    return false;
  }
  if (!LumiverseTypeUtils::equals(d.getParam("intensity"), copy.getParam("intensity"))) {
    cout << "[ERROR] deviceCopy: Intensity parameter not equal\n";
    return false;
  }
  
  string orig;
  string cpy;
  d.getMetadata("test", orig);
  
  if (!copy.getMetadata("test", cpy)) {
    cout << "[ERROR] Metadata key 'test' does not exist in copy.\n";
    return false;
  }
  if (orig != cpy) {
    cout << "[ERROR] Metadata values are not equal\n";
    return false;
  }

  return true;
}

bool DeviceTests::deviceSerialization() {
  Device d("test", 1, "TEST DEVICE");

  LumiverseFloat* intens = new LumiverseFloat(1, 0, 1, 0);
  LumiverseColor* color = new LumiverseColor(BASIC_RGB);

  d.setParam("intensity", (LumiverseType*)intens);
  d.setParam("color", (LumiverseType*)color);
  d.setMetadata("key", "val");
  d.setMetadata("val", "key");

  Device read("test", d.toJSON());
  return d.isIdentical(&read);
}

bool DeviceTests::deviceAccessors() {
  Device d("test", 1, "TEST DEVICE");

  d.setChannel(5789);
  if (d.getChannel() != 5789) {
    return false;
  }

  d.setType("Another Type");
  if (d.getType() != "Another Type") {
    return false;
  }

  if (d.getId() != "test") {
    return false;
  }
}

bool DeviceTests::devicePropertyManipulation() {
  Device d("test", 1, "TEST DEVICE");

  LumiverseFloat* f1 = new LumiverseFloat(1, 0, 1, 0);

  if (d.setParam("intensity", (LumiverseType*)f1))
    return false;

  if (!d.setParam("intensity", 0.5))
    return false;

  if (((LumiverseFloat*)d.getParam("intensity"))->getVal() != 0.5)
    return false;

  // test accsessors for inappropriate data retrieval next

  return true;
}