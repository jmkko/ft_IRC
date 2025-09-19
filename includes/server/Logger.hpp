#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "colors.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

enum LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3 };

#define RESET NC

class Logger
{
  private:
    std::string   _name;
    std::ofstream _logFile;
    LogLevel      _minLevel;
    bool          _fileEnabled;

    Logger();
    Logger(const Logger& inst);

    Logger&     operator=(const Logger& inst);
    std::string getCurrentTime();
    std::string levelToString(LogLevel level);
    std::string getColorForLevel(LogLevel level);

  public:
    Logger(const std::string& logName, const std::string& filename = "irc_server.log", LogLevel level = INFO);
    ~Logger();

    void                       log(LogLevel level, const std::string& message);
    void                       debug(const std::string& message);
    template <typename T> void debug(const T& t, const std::string& message)
    {
        std::ostringstream oss;
        oss << message;
        oss << t;
        log(DEBUG, oss.str());
    }
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void setMinLevel(LogLevel level);
};

#endif
