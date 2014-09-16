#include "ArnoldInterfaceTests.h"

int ArnoldInterfaceTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->initTests(); }, "initTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->renderTests(); }, "renderTests", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->closeTests(); }, "closeTests", 3)) ? numPassed++ : numPassed;

  return numPassed;
}

bool ArnoldInterfaceTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool ArnoldInterfaceTests::initTests() {
  std::string file = "J:/Lumiverse/Lumiverse/data/cornell.ass";
  std::string plugins = "J:/LumiverseInstall/lib/arnold/plugin;I:/solidangle/mtoadeploy/2014/shaders/";
  m_test_interface = new ArnoldInterface();

  m_test_interface->setAssFile(file);
  if (m_test_interface->getAssFile() != file) {
	  std::cout << "Error setting ass file." << endl;
	  return false;
  }

  m_test_interface->setPluginDirectory(plugins);
  if (m_test_interface->getPluginDirectory() != plugins) {
	  std::cout << "Error setting plugin directory." << endl;
	  return false;
  }

  m_test_interface->setSamples(-3);
  if (m_test_interface->getSamples() != -3) {
	  std::cout << "Error setting samples." << endl;
	  return false;
  }
	  
  m_test_interface->init();

  if (m_test_interface->render() != AI_SUCCESS) {
	  std::cout << "Error rendering. Initialization failed." << endl;
	  return false;
  }

  return true;
}

bool ArnoldInterfaceTests::renderTests() {
  AtNode *light_ptr = AiNode("point_light");
  AiNodeSetStr(light_ptr, "name", "test");
  AiNodeSetPnt(light_ptr, "position", 0, 2, 0);
  AiNodeSetFlt(light_ptr, "intensity", 10);

  if (AiNodeLookUpByName("test") == NULL) {
	  std::cout << "Error creating node." << endl;
	  return false;
  }

  if (m_test_interface->render() != AI_SUCCESS) {
	  std::cout << "Error rendering." << endl;
	  return false;
  }

  return true;
}

bool ArnoldInterfaceTests::closeTests() {
  m_test_interface->close();

  if (m_test_interface->render() != AI_ERROR) {
	  std::cout << "Error closing." << endl;
	  return false;
  }

  return true;
}
