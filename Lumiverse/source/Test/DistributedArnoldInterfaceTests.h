/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
#include <Windows.h>

using namespace Lumiverse;

class DistributedArnoldInterfaceTests
{
public: 
  DistributedArnoldInterfaceTests() { }
  ~DistributedArnoldInterfaceTests() { delete m_test_interface; };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  int m_numTests = 3;

  // Test functions
  bool initTests();
  bool renderTests();
  bool closeTests();

  // Helper functions for spinning up test server
  bool spinUpTestServer();
  void tearDownTestServer();
  bool m_server_spun_up = false;
  PROCESS_INFORMATION m_test_server_info;

  DistributedArnoldInterface *m_test_interface;
};