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

int main(int argc, char**argv) {
  Logger::setLogFile("testLog.txt");

  DeviceTests dt;
  TypeTests tt;
  RigTests rt;
  PlaybackTests pt;

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

  cout << "========================================\n";
  cout << "Summary\n\n";
  cout << "[" << ttpassed << "/" << tt.numTests() << "]\tLumiverseType\n";
  cout << "[" << dtpassed << "/" << dt.numTests() << "]\tDevice\n";
  cout << "[" << rtpassed << "/" << rt.numTests() << "]\tRig and Device Set\n";
  cout << "[" << ptpassed << "/" << pt.numTests() << "]\tPlayback\n";

  getch();
}