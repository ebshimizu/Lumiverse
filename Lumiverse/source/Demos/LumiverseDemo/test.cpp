#include <string>
#include "LumiverseDemoConfig.h"
#include "LumiverseCore.h"

using namespace std;
using namespace Lumiverse;

int main(int argc, char**argv) {
  // Testing colors.

  LumiverseColor c1;
  c1.setRGB(1, 0, 0);
  c1.getRGB();
  
  _getch();

  // Logger::setLogFile("OLLlog.txt");
  
  // Rig rig("E:/Users/falindrith/Documents/Programming/Lumiverse/Core/Lumiverse/data/PauschBridge.json");

  // TODO: (roughly in order of importance)
  // -Sample command line control
  // -Color Mixing and other types
  // -Saving rigs
  // -Robust file reads

  // In OpenLL FX
  // -Presets
  // -Named Groups
  // -Cues / timeline

  //rig.save("E:/Users/falindrith/Documents/Programming/Lumiverse/Core/Lumiverse/data/PauschBridge2.json");

  // Init rig
  //rig.init();

  // Test extra funcs
  // rig.addFunction([]() { cout << "Testing additional functions\n"; });

  //rig.run();

  // Loop for stuff
  //cout << "Lumiverse Test Command Line\n";
  //cout << "Available commands: select [query], set [parameter]=[value], reset, info [device id], info selected\n";
  //cout << "Only floating point parameters are currently supported.\n";
  //DeviceSet current;
  //
  //while (1) {
  //  cout << ">> ";

  //  string input;
  //  getline(cin, input);

  //  if (input.substr(0, 7) == "select ") {
  //    current = rig.query(input.substr(7));

  //    cout << "Query returned " << current.size() << " devices.\n";
  //  }
  //  else if (input.substr(0, 5) == "reset") {
  //    current.reset();
  //  }
  //  else if (input.substr(0, 13) == "info selected") {
  //    cout << current.info() << "\n";
  //  }
  //  else if (input.substr(0, 5) == "info ") {
  //    string id = input.substr(5);

  //    cout << rig.getDevice(id)->toString() << "\n";
  //  }
  //  else if (input.substr(0, 4) == "set ") {
  //    // Set float
  //    regex paramRegex("(\\w+)=(\\d*\\.\?\\d*)([f])");
  //    string param = input.substr(4);
  //    smatch matches;

  //    regex_match(param, matches, paramRegex);

  //    if (matches.size() != 4) {
  //      // Try to set an enumeration
  //      regex paramRegex("(\\w+)=([\\w_\\s\\b]+),\?(\\d*\\.\?\\d*)");
  //      string param = input.substr(4);
  //      smatch matches;

  //      regex_match(param, matches, paramRegex);

  //      if (matches.size() != 4) {
  //        cout << "Invalid set command\n";
  //      }

  //      string key = matches[1];
  //      string val = matches[2].str();

  //      float val2 = -1.0f;
  //      if (matches[3].length() > 0) {
  //        stringstream(matches[3]) >> val2;
  //      }

  //      current.setParam(key, val, val2);
  //    }

  //    string key = matches[1];
  //    float val;
  //    stringstream(matches[2]) >> val;

  //    current.setParam(key, val);
  //  }
  //}
}