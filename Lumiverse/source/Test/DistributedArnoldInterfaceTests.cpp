#include "DistributedArnoldInterfaceTests.h"

#ifdef USE_DUMIVERSE

int DistributedArnoldInterfaceTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->initTests(); }, "initTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->renderTests(); }, "renderTests", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->closeTests(); }, "closeTests", 3)) ? numPassed++ : numPassed;

  if (m_server_spun_up) {
	  tearDownTestServer();
  }

  return numPassed;
}

bool DistributedArnoldInterfaceTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool DistributedArnoldInterfaceTests::initTests() {
  std::string file = "J:/Lumiverse/Lumiverse/data/cornell.ass";
  std::string plugins = "J:/LumiverseInstall/lib/arnold/plugin;I:/solidangle/mtoadeploy/2014/shaders/";
  m_test_interface = new DistributedArnoldInterface();

  m_test_interface->setAssFile(file);
  m_test_interface->setHostPort(3000);

  if (m_test_interface->getAssFile() != file) {
	  std::cout << "Error setting ass file." << endl;
	  return false;
  }

  m_server_spun_up = spinUpTestServer();
  if (!m_server_spun_up) {
	  std::cout << "Error spinning up test node server." << endl;
	  return false;
  }

  // m_test_interface->init();

  /*
  if (m_test_interface->render() != AI_SUCCESS) {
	  std::cout << "Error rendering. Initialization failed." << endl;
	  return false;
  }
  */

  return true;
}

bool DistributedArnoldInterfaceTests::spinUpTestServer() {
	// Fork new process
	std::string plugins = "C:/Program Files/Lumiverse/lib/arnold/plugin;C:/solidangle/mtoadeploy/2016/shaders/";
	std::string path_to_dumiverse = "C:/Program Files/Lumiverse/include/Simulation/dumiverse.js";
	std::string executable = "C:/Program Files/nodejs/node.exe";
	std::string command_line = executable + " " + path_to_dumiverse + " " + plugins;

#ifdef _WIN32
	STARTUPINFO si;
	LPSTR process_arg = const_cast<char *>(command_line.c_str());
	
	return !CreateProcess(
		NULL,
		process_arg,
		NULL,
		NULL,
		false,
		NULL,
		NULL,
		NULL,
		&si,
		&m_test_server_info
		);
#endif

	return false;
}

void DistributedArnoldInterfaceTests::tearDownTestServer() {
	std::cout << "Killing test server" << std::endl;

	// Send termination signal
#ifdef _WIN32
	TerminateProcess(m_test_server_info.hProcess, 0);

	// Close handles
	CloseHandle(m_test_server_info.hProcess);
	CloseHandle(m_test_server_info.hThread);
#endif
}

bool DistributedArnoldInterfaceTests::renderTests() {
	/*
  if (m_test_interface->render() != AI_SUCCESS) {
	  std::cout << "Error rendering." << endl;
	  return false;
  }
  */

  return true;
}

bool DistributedArnoldInterfaceTests::closeTests() {
  m_test_interface->close();

  /*
  if (m_test_interface->render() != AI_ERROR) {
	  std::cout << "Error closing." << endl;
	  return false;
  }
  */

  return true;
}

#endif // USE_DUMIVERSE
