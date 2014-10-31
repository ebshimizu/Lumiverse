/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class TypeTests
{
public: 
  TypeTests() { };
  ~TypeTests() { };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  static const int m_numTests = 4;

  // Test functions
  bool floatTests();
  bool enumTests();
  bool colorTests();
  bool oriTests();
};