/*
  Runs tests to verify the functionality of the Rig and DeviceSet classes 
*/

#include <LumiverseCore.h>
#include <LumiverseShowControl.h>

using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

class PlaybackTests
{
public: 
  PlaybackTests() { };
  ~PlaybackTests() { delete m_pb; delete m_testRig; };

  // Runs all the tests and returns the number of tests that passed.
  int runTests();

  // Returns the total number of tests in this class.
  int numTests() { return m_numTests; }

private:
  bool runTest(std::function<bool()> t, string testName, int testNum);

  // Update when new tests are written.
  int m_numTests = 9;

  // Initialized in PlaybackStart()
  Rig* m_testRig;
  Playback* m_pb;

  // Test functions
  // Currently, these tests are setup to test the most commonly used
  // parts of the Playback library. As new features are added or
  // changed, more tests should be added.
  bool playbackSetup();
  bool createLayer();
  bool createCueList();
  bool createCue();
  bool checkCue();
  bool playCue();
  bool layerToggle();
  bool snapshot();
  bool groups();
};