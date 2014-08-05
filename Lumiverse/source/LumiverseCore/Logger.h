/*! \file Logger.h
* \brief Contains functions for logging information to console or file.
*/
#ifndef _LOGGER_H_
#define _LOGGER_H_

#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <time.h>

using namespace std;

namespace Lumiverse {
  /*! \brief Log detail level */
  enum LOG_LEVEL {
    LDEBUG = 0,      /*!< Debugging info */
    INFO = 1,        /*!< Status messages */
    WARN = 2,        /*!< Warnings */
    ERR = 3,         /*!< Recoverable Errors */
    CRITICAL = 4,    /*!< Potentially fatal errors / more severe errors */
    FATAL = 5        /*!< Program's gonna crash now / something really bad happened. */
  };

  /*!
  * \namespace Lumiverse::Logger
  * \brief Logging interface.
  *
  * Contains functions to help control and rout Lumiverse 
  * error and status messages.
  */
  namespace Logger {
    /*! \brief Sets the minimum logging level
    *
    * If a message has a LOG_LEVEL less than the logLevel,
    * it will not be output.
    */
    static unsigned int logLevel = 0;

    /*!
    * \brief Log file
    *
    * If not specified, this logs to stdout.
    */
    static ofstream logFile;

    /*!
    * \brief Open a log file for writing to instead of writing to stdout
    *
    * This will append to an existing file.
    * \param name Path to the file. Can be existing file or new file.
    */
    void setLogFile(string name);

    /*!
    * \brief Sticks the current time and date into a string.
    *
    * \return String containing date and time.
    */
    string printTime();

    /*!
    * \brief Translates the log level to a string.
    *
    * \param level Log message level
    * \return String containing the text version of the message level
    */
    string printLevel(LOG_LEVEL level);

    /*!
    * \brief Logs a meesage to the output file.
    *
    * Messages are formatted as: [LOG_LEVEL] Date Message
    * \param level Log message level
    * \param message The message to put in the log
    */
    void log(LOG_LEVEL level, string message);

    /*!
    * \brief Sets the logLevel
    *
    * You could just set logLevel=[level] in plain code, but if you
    * like functions better, here it is.
    */
    void setLogLevel(LOG_LEVEL level);
  };
}
#endif
