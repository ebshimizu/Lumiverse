/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class ArnoldInterfaceTests
{
public: 
  ArnoldInterfaceTests() { }
  ~ArnoldInterfaceTests() { delete m_test_interface; };

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

  ArnoldInterface *m_test_interface;
};