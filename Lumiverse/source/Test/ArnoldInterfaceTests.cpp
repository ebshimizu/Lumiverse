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

  m_test_interface = new ArnoldInterface();

  m_test_interface->setAssFile(file);
  if (m_test_interface->getAssFile() != file)
	  return false;

  m_test_interface->setSamples(3);
  if (m_test_interface->getSamples() != 3)
	  return false;

  m_test_interface->init();

  if (m_test_interface->render() != AI_SUCCESS)
	  return false;

  return true;
}

bool ArnoldInterfaceTests::renderTests() {
  AtNode *light_ptr = AiNode("point_light");
  AiNodeSetStr(light_ptr, "name", "test");
  AiNodeSetPnt(light_ptr, "position", 0, 2, 0);
  AiNodeSetFlt(light_ptr, "intensity", 10);

  if (AiNodeLookUpByName("test") == NULL)
	  return false;

  if (m_test_interface->render() != AI_SUCCESS)
	  return false;

  return true;
}

bool ArnoldInterfaceTests::closeTests() {
  m_test_interface->close();

  if (m_test_interface->render() != AI_ABORT)
	  return false;

  return true;
}
