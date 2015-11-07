#include "DeviceTests.h"

int DeviceTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->deviceCreation(); }, "deviceCreation", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceAccessors(); }, "deviceAccessors", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceCopy(); }, "deviceCopy", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceSerialization(); }, "deviceSerialization", 4)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->devicePropertyManipulation(); }, "devicePropertyManipulation", 5)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceMetadataManipulation(); }, "deviceMetadataManipulation", 6)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->devicePropertyInfo(); }, "devicePropertyInfo", 7)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->deviceCallbacks(); }, "deviceCallbacks", 8)) ? numPassed++ : numPassed;

  return numPassed;
}

bool DeviceTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool DeviceTests::deviceCreation() {
  Device d("test", 1, "ETC Source 4 26deg");

  return (d.getId() == "test" &&
    d.getChannel() == 1 &&
    d.getType() == "ETC Source 4 26deg");
}

bool DeviceTests::deviceCopy(){
  Device d("test1", 1, "ETC Source 4 26deg");

  if (d.setParam("intensity", (LumiverseType*)(new LumiverseFloat(1, 0, 1, 0)))) {
    cout << "[ERROR] deviceCopy: Error creating intensity parameter\n";
    return false;
  }
  if (d.setMetadata("test", "myVal")) {
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

  return true;
}

bool DeviceTests::devicePropertyManipulation() {
  bool ret = true;
  Device d("test", 1, "TEST DEVICE");

  LumiverseFloat* f1 = new LumiverseFloat(1, 0, 1, 0);

  if (d.setParam("intensity", (LumiverseType*)f1))
  {
    cout << "Add new parameter failure.\n";
    ret = false;
  }

  if (!d.setParam("intensity", 0.5))
  {
    cout << "Set parameter failure.\n";
    ret = false;
  }

  if (((LumiverseFloat*)d.getParam("intensity"))->getVal() != 0.5)
  {
    cout << "Device parameter retrieval failure.\n";
    ret = false;
  }

  // test accsessors for inappropriate data retrieval next
  float val;
  if (d.getParam("does not exist", val))
  {
    cout << "Received data for non-existant parameter\n";
    ret = false;
  }

  LumiverseColor* c = new LumiverseColor(BASIC_RGB);

  c->setRGBRaw(1, 0, 0, 1);
  d.setParam("color", (LumiverseType*)c);

  // Should return false, type mismatch.
  if (d.getParam("color", val))
  {
    cout << "Was able to set data for wrong type\n";
    ret = false;
  }

  // If it somehow returns data for something that doesn't exist, that's wrong.
  if (d.getParam("does not exist") != nullptr) {
    cout << "Received data for parameter that doesn't exist.";
    ret = false;
  }

  // Test for manipulators.
  if (d.setParam("color", 5))
  {
    cout << "Set non-floating point parameter data as a floating point parameter.\n";
    ret = false;
  }

  if (!d.setColorRGBRaw("color", 1, 0, 0, 1))
  {
    cout << "Unable to set color parameter data\n";
    ret = false;
  }

  // Testing that reset is called on params. Don't have a default for the
  // basic enum, so we do it here.
  d.reset();
  if (!d.getParam("intensity")->isDefault() || !d.getParam("color")->isDefault()) {
    cout << "Parameters not reset to default\n";
    ret = false;
  }

  // Just testing type stuff here mostly.
  LumiverseEnum* blank = new LumiverseEnum();
  d.setParam("enum", (LumiverseType*)blank);

  if (d.setParam("enum", "option doesn't exist")) {
    cout << "Enumeration value set to invalid option\n";
    ret = false;
  }

  // Type mismatch
  if (d.setParam("enum", 5)) {
    cout << "Able to set enumeration as floating point\n";
    ret = false;
  }

  // Data tests
  d.setParam("intensity", 0.5f);
  d.getParam("intensity", val);
  if (val != 0.5f) {
    cout << "Expected result: 0.5. Received: " << val << "\n";
    ret = false;
  }

  d.setColorRGBRaw("color", 0, 1, 0, 1);
  LumiverseColor* deviceColor = (LumiverseColor*)d.getParam("color");
  if (deviceColor->getColorChannel("Green") != 1
    && deviceColor->getColorChannel("Red") != 0
    && deviceColor->getColorChannel("Blue") != 0)
  {
    cout << "Color data set incorrectly\n";
    ret = false;
  }

  return ret;
}

bool DeviceTests::deviceMetadataManipulation() {
  bool ret = true;

  Device d("test", 1, "Test Device");

  if (d.setMetadata("gel", "R27")) {
    cout << "Metadata key already exists in empty device\n";
    ret = false;
  }

  if (!d.setMetadata("gel", "R80")) {
    cout << "Metadata key doesn't exist in device when previously set\n";
    ret = false;
  }

  string val;
  if (!d.getMetadata("gel", val)) {
    cout << "Error retrieving metadata value\n";
    ret = false;
  }

  if (val != "R80") {
    cout << "Metadata value mismatch. Expected: R80. Received: " << val << "\n";
    ret = false;
  }

  d.setMetadata("angle", "top");
  d.setMetadata("system", "top cool");
  d.setMetadata("area", "5");

  if (d.numMetadataKeys() != 4) {
    cout << "Expected Metadata Keys: 4. Received: " << d.numMetadataKeys() << "\n";
    ret = false;
  }

  d.clearMetadataValues();
  if (d.numMetadataKeys() != 4) {
    cout << "Clear metadata value deleted keys\n";
    ret = false;
  }
  d.getMetadata("gel", val);
  if (val != "") {
    cout << "Clear metadata values didn't clear metadata values\n";
    ret = false;
  }

  d.clearAllMetadata();
  if (d.numMetadataKeys() != 0) {
    cout << "Clear all metadata didn't delete all of the metadata keys\n";
    ret = false;
  }

  return ret;
}

bool DeviceTests::devicePropertyInfo() {
  bool ret = true;

  Device d("test", 1, "test device");
  d.setParam("intensity", (LumiverseType*)new LumiverseFloat(1, 0, 1, 0));
  d.setParam("tilt", (LumiverseType*)new LumiverseOrientation(0, DEGREE, 0, 360, 0));

  if (d.numParams() != 2) {
    cout << "Expected 2 parameters. Counted: " << d.numParams() << "\n";
    ret = false;
  }

  return ret;
}

bool DeviceTests::deviceCallbacks() {
  bool ret = true;

  int* counter = new int(0);
  Device d("test", 1, "test device");
  d.setParam("intensity", (LumiverseType*)new LumiverseFloat(1, 0, 1, 0));

  int callbackID = d.addParameterChangedCallback([=](Device* d){ *counter += 1; });
  d.setParam("intensity", 0.5);

  if (*counter != 1) {
    cout << "Callback " << callbackID << " didn't execute\n";
    ret = false;
  }

  d.setParam("dne", 0.0f);
  if (*counter != 1) {
    cout << "Callback " << callbackID << " executed when a parameter didn't change\n";
    ret = false;
  }

  d.deleteParameterChangedCallback(callbackID);
  d.setParam("intensity", 0.5);
  if (*counter != 1) {
    cout << "Callback " << callbackID << " wasn't properly deleted\n";
    ret = false;
  }

  *counter = 0;
  d.setMetadata("thing1", "thing2");
  callbackID = d.addMetadataChangedCallback([=](Device* d){ *counter += 1; });
  d.setMetadata("thing1", "thing3");

  if (*counter != 1) {
    cout << "Callback " << callbackID << " didn't execute\n";
    ret = false;
  }

  d.clearMetadataValues();
  if (*counter != 2) {
    cout << "Callback " << callbackID << " didn't execute when all metadata values were cleared\n";
    ret = false;
  }

  d.setMetadata("newKey", "newVal");
  if (*counter != 3) {
    cout << "Callback " << callbackID << " didn't execute when a new key was added\n";
    ret = false;
  }

  d.clearAllMetadata();
  if (*counter != 4) {
    cout << "Callback " << callbackID << " didn't execute when all metadata was deleted\n";
    ret = false;
  }

  d.deleteMetadataChangedCallback(callbackID);
  d.setMetadata("newKey", "newVal");

  if (*counter != 4) {
    cout << "Callback " << callbackID << " wasn't deleted properly\n";
    ret = false;
  }

  return ret;
}
