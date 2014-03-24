#include <string>
#include "LumiverseDemoConfig.h"
#include "LumiverseCore.h"

using namespace std;

int main(int argc, char**argv) {
  // Logger::setLogFile("OLLlog.txt");
  
  Rig rig("/Users/Evan/Documents/Programming/Lumiverse/lumiverse/Lumiverse/data/movingLights.json");

  // TODO: (roughly in order of importance)
  // -Sample command line control
  // -Color Mixing and other types
  // -Saving rigs
  // -Robust file reads

  // In OpenLL FX
  // -Presets
  // -Named Groups
  // -Cues / timeline

  // Init rig

  rig.init();
  rig.run();

  // Loop for stuff
  cout << "Lumiverse Test Command Line\n";
  cout << "Available commands: select [query], set [parameter]=[value], info [device id], info selected\n";
  cout << "Only floating point parameters are currently supported.\n";
  DeviceSet current;
  
  while (1) {
    cout << ">> ";

    string input;
    getline(cin, input);

    if (input.substr(0, 7) == "select ") {
      current = rig.query(input.substr(7));

      cout << "Query returned " << current.size() << " devices.\n";
    }
    else if (input.substr(0, 13) == "info selected") {
      cout << current.info() << "\n";
    }
    else if (input.substr(0, 5) == "info ") {
      string id = input.substr(5);

      cout << rig.getDevice(id)->toString() << "\n";
    }
    else if (input.substr(0, 4) == "set ") {
      // Can only set float currently
      regex paramRegex("(\\w+)=(\\d*\\.\?\\d*)([f])");
      string param = input.substr(4);
      smatch matches;

      regex_match(param, matches, paramRegex);

      if (matches.size() != 4) {
        cout << "Invalid set command\n";
      }

      string key = matches[1];
      float val;
      stringstream(matches[2]) >> val;

      current.setParam(key, val);
    }
  }


  //DeviceSet channelRange = rig.getChannel(1, 10);
  //channelRange = channelRange.remove(5, 7);
  //channelRange.setParam("intensity", 1.0f);

  //DeviceSet query = rig.query("!$color=R80");

  //_getch();

  //// get all devices that aren't in a back angle
  //DeviceSet myDevices = rig.getAllDevices().remove("angle", "back", false);

  //// Do things. Remember that the update loops is only 40Hz and changes aren't instant
  //// in computer time
  //Sleep(100);
  //_getch();

  //rig.getDevices("angle", "front", true).add("angle", "front left", true).add("angle", "front right", true).setParam("intensity", 0.75f);

  //_getch();
}