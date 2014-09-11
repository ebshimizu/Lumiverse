/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class ArnoldParameterVectorTests
{
public: 
  ArnoldParameterVectorTests() { };
  ~ArnoldParameterVectorTests() { };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  int m_numTests = 2;

  // Test functions
  bool parseTests();
  bool vectorTests();
};