#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <ctime>
#include <iomanip>
#include <sstream>

enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"

class Logger {
private:
    std::string _name;
    std::ofstream _logFile;
    LogLevel _minLevel;
    bool _fileEnabled;	
    
    std::string getCurrentTime();
    std::string levelToString(LogLevel level);
    std::string getColorForLevel(LogLevel level);
    
public:
    Logger(const std::string& logName, const std::string& filename= "irc_server.log", LogLevel level = INFO);
    ~Logger();
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void setMinLevel(LogLevel level);
};


#endif

