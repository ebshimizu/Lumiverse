#ifndef _LOGGER_H_
#define _LOGGER_H_

#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <chrono>
#include <time.h>

using namespace std;

namespace Lumiverse {
  // Log levels.
  enum LOG_LEVEL {
    LDEBUG = 0,      // Debugging info
    INFO = 1,       // Status messages
    WARN = 2,       // Warnings
    ERR = 3,      // Recoverable Errors
    CRITICAL = 4,   // Potentially fatal errors / more severe errors
    FATAL = 5       // Program's gonna crash now / something really bad happened.
  };

  // Logging interface. Just some functions to help control and rout Lumiverse 
  // error and status messages
  // Currently this logs to std out. Eventually it will be to a file.
  namespace Logger {
    // Set the logging level
    static unsigned int logLevel = 0;

    // Log file, if specified.
    static ofstream logFile;

    // Opens a log file for writing to instead of writing to stdout
    void setLogFile(string name);

    // Sticks the current time and date into a string.
    string printTime();

    // Translates the log level to a string.
    string printLevel(LOG_LEVEL level);

    // Logs a meesage to the output file.
    void log(LOG_LEVEL level, string message);
  };
}
#endif
