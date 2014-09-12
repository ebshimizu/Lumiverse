/*
  Runs tests to verify the functionality of a LumiverseDevice
*/

#include <LumiverseCore.h>
using namespace Lumiverse;

class ArnoldFrameManagerTests
{
public: 
  ArnoldFrameManagerTests() :
	  m_file_frame_manager(".") {};
  ~ArnoldFrameManagerTests() { };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  int m_numTests = 4;

  // Test functions
  bool dumpAndPlayTests(ArnoldFrameManager *fm);
  bool clearTests(ArnoldFrameManager *fm);

  bool memDumpAndPlayTests();
  bool memClearTests();
  bool fileDumpAndPlayTests();
  bool fileClearTests();

  ArnoldMemoryFrameManager m_mem_frame_manager;
  ArnoldFileFrameManager m_file_frame_manager;
};