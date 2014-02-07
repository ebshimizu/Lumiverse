#include "Logger.h"

namespace Logger {
  void setLogFile(string name) {
    logFile.open(name, ios::out | ios::app);
  }

  // Sticks the current time and date into a string.
  string printTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
  }

  // Translates the log level to a string.
  string printLevel(LOG_LEVEL level) {
    switch (level) {
      case (DEBUG) : return "DEBUG";
      case (INFO) : return "INFO";
      case (WARN) : return "WARN";
      case (ERR) : return "ERROR";
      case (CRITICAL) : return "CRITICAL";
      case (FATAL) : return "FATAL";
      default: return "";
    }
  }

  void log(LOG_LEVEL level, string message) {
    if ((unsigned int)level >= logLevel) {

      // TODO: Change to configurable file output or something
      if (logFile.is_open()) {
        logFile << "[" << printLevel(level) << "]\t" << printTime() << " " << message << "\n";
      }
      else {
        cout << "[" << printLevel(level) << "]\t" << printTime() << " " << message << "\n";
      }
    }
  }
};