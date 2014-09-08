/*
  Runs tests to verify the functionality of the Rig and DeviceSet classes 
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class RigTests
{
public: 
  RigTests() { };
  ~RigTests() { delete m_testRig; };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  int m_numTests = 11;

  // Initialized in rigStart()
  Rig* m_testRig;

  // Test functions
  bool rigStart();
  bool rigAddDeleteDevices();
  bool rigAddDeletePatch();
  bool deviceSetAdd();
  bool deviceSetRemove();
  bool queryID();
  bool queryChannel();
  bool queryMetadata();
  bool queryParameter();
  bool queryMixed();
  bool queryFilter();

  // Reserved for future use.
  bool queryComplex();
};