/*
  Test program for Lumiverse. Run when changing parts of the core library
  to make sure you didn't break thigs, or just run to prove to yourself that
  this thing works.

  The various categories of tests are broken out into classes for easy editing.
  See the individual files for details.
*/

#include <LumiverseCore.h>
#include "DeviceTests.h"

int main(int argc, char**argv) {
  DeviceTests dt;

  cout << "Starting Lumiverse Test Suite for version " << LumiverseCore_VERSION_MAJOR << "." << LumiverseCore_VERSION_MINOR << "\n";
  cout << "========================================\n";

  cout << "Running Tests for LumiverseDevice...\n";
  int passed = dt.runTests();
  cout << "LumiverseDevice passed " << passed << "/" << dt.numTests() << " tests.\n";
  cout << "\n";

  getch();
}