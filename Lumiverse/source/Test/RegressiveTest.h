/*
  Runs tests to verify the functionality of the Rig and DeviceSet classes 
*/

#include <LumiverseCore.h>
using namespace Lumiverse;
using namespace Lumiverse::ShowControl;

class RegressiveTest
{
public: 
  RegressiveTest() : m_testRig(NULL), m_playback(NULL) {}
  ~RegressiveTest() { delete m_testRig; }

  // Runs all the tests and returns the number of tests that passed.
  void runTest();

private:
	void initCues();

  Rig* m_testRig;
  Playback *m_playback;
};