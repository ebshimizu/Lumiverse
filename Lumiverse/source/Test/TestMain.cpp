/*
  Test program for Lumiverse. Run when changing parts of the core library
  to make sure you didn't break thigs, or just run to prove to yourself that
  this thing works.

  The various categories of tests are broken out into classes for easy editing.
  See the individual files for details.
*/

#include <LumiverseCore.h>
#include "TypeTests.h"
#include "DeviceTests.h"
#include "RigTests.h"
#include "PlaybackTests.h"

#ifdef USE_ARNOLD
#include "ArnoldInterfaceTests.h"
#include "ArnoldFrameManagerTests.h"
#include "ArnoldParameterVectorTests.h"
#include "RegressiveTest.h"
#endif

int main(int argc, char**argv) {
  Logger::setLogFile("testLog.txt");

  DeviceTests dt;
  TypeTests tt;
  RigTests rt;
  PlaybackTests pt;

#ifdef USE_ARNOLD
  ArnoldInterfaceTests ait;
  ArnoldFrameManagerTests afmt;
  ArnoldParameterVectorTests apvt;
  RegressiveTest rgt;
#endif

  cout << "Starting Lumiverse Test Suite for version " << LumiverseCore_VERSION_MAJOR << "." << LumiverseCore_VERSION_MINOR << "\n";
  cout << "========================================\n";

  cout << "Running Tests for LumiverseType...\n";
  int ttpassed = tt.runTests();
  cout << "\n";

  cout << "Running Tests for Device...\n";
  int dtpassed = dt.runTests();
  cout << "\n";

  cout << "Running Tests for Rig and DeviceSet...\n";
  int rtpassed = rt.runTests();
  cout << "\n";

  cout << "Running Tests for Playback...\n";
  int ptpassed = pt.runTests();
  cout << "\n";

#ifdef USE_ARNOLD
  cout << "Running Tests for ArnoldInterface...\n";
  int aitpassed = ait.runTests();
  cout << "\n";

  cout << "Running Tests for ArnoldFrameManager...\n";
  int afmtpassed = afmt.runTests();
  cout << "\n";

  cout << "Running Tests for ArnoldParameterVector...\n";
  int apvtpassed = apvt.runTests();
  cout << "\n";
#endif

  cout << "========================================\n";
  cout << "Summary\n\n";
  cout << "[" << ttpassed << "/" << tt.numTests() << "]\tLumiverseType\n";
  cout << "[" << dtpassed << "/" << dt.numTests() << "]\tDevice\n";
  cout << "[" << rtpassed << "/" << rt.numTests() << "]\tRig and Device Set\n";
  cout << "[" << ptpassed << "/" << pt.numTests() << "]\tPlayback\n";

#ifdef USE_ARNOLD
  cout << "[" << aitpassed << "/" << ait.numTests() << "]\tArnoldInterface\n";
  cout << "[" << afmtpassed << "/" << afmt.numTests() << "]\tArnoldFrameManager\n";
  cout << "[" << apvtpassed << "/" << apvt.numTests() << "]\tArnoldParameterVector\n";

  cout << "\nPress a key to start the test show.\n";
  getchar();

  cout << "Running Test show with Arnold...\n";
  rgt.runTest();
#endif

  getchar();
}
