/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class DeviceTests
{
public: 
  DeviceTests() { };
  ~DeviceTests() { };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  static const int m_numTests = 8;

  // Test functions
  bool deviceCreation();
  bool deviceCopy();
  bool deviceSerialization();
  bool deviceAccessors();
  bool devicePropertyManipulation();
  bool deviceMetadataManipulation();
  bool devicePropertyInfo();
  bool deviceCallbacks();
};