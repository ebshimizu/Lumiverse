#include "Logger.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Lumiverse {
namespace Logger {
  mutex logMutex;
    
  void setLogFile(string name) {
    logFile.open(name, ios::out | ios::app);
  }

  // Sticks the current time and date into a string.
  string printTime() {
    // C++11 chrono used for timestamp
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    stringstream buf;
    
#ifndef __linux__
    buf << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S");
#else
    char buffer[80];
    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",localtime(&now));
    std::string timebuf(buffer);
    buf << timebuf;
#endif

    return buf.str();
  }

  // Translates the log level to a string.
  string printLevel(LOG_LEVEL level) {
    switch (level) {
      case (LDEBUG) : return "DEBUG";
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

      logMutex.lock();
      // TODO: Change to configurable file output or something
      if (logFile.is_open()) {
        logFile << "[" << printLevel(level) << "]\t" << printTime() << " " << message << "\n";
      }
      else {
#ifndef _WIN32
        cout << "[" << printLevel(level) << "]\t" << printTime() << " " << message << "\n";
#else
		stringstream buf;
		buf << "[" << printLevel(level) << "]\t" << printTime() << " " << message << "\n";
		OutputDebugString(buf.str().c_str());
#endif
      }
      logMutex.unlock();
    }
  }

  void setLogLevel(LOG_LEVEL level) {
    logLevel = level;
  }
}
}