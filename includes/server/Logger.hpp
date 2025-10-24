#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Client.hpp"

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

enum LogLevel { DETAIL = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4 };

// NOLINTNEXTLINE(cppcoreguidelines-macro-to-enum)
#define PADDING_FILE_FUN 50
#define RESET            NC
class Logger
{
  public:
    Logger(const std::string& logName, const std::string& filename = "irc_server.log", LogLevel level = INFO);
    ~Logger();

    // custom level
    void                       log(LogLevel level, const std::string& message);
    template <typename T> void log(LogLevel level, const std::string& file, const std::string& function, const T& message)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << std::left << std::setw(PADDING_FILE_FUN) << (file + ": " + function + "() ") << message;
        log(level, oss.str());
    }

    template <typename T>
    void log(LogLevel level, const std::string& file, const std::string& function, const std::string& varName, const T& varVal)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << std::left << std::setw(PADDING_FILE_FUN) << (file + ": " + function + "() ") << varName << " -> " << varVal;
        log(level, oss.str());
    }

    template <typename T> void log(const std::string& context, const T& t)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << context << " -> " << t;
        log(DEBUG, oss.str());
    }

    template <typename T> void log(const T& t)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << t;
        log(DEBUG, oss.str());
    }

    // detail
    template <typename T> void detail(const std::string& file, const std::string& function, const T& t)
    {
        log(DETAIL, file, function, t);
    }

    // debug
    void                       debug(const std::string& message);
    void                       debug(const std::string& desc, const std::string& message);
    template <typename T> void debug(const std::string& file, const std::string& function, const T& t)
    {
        log(DEBUG, file, function, t);
    }

    // info
    void receiving(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);
    void sending(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);
    void info(const std::string& message);
    template <typename T> void info(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    void                       warning(const std::string& message);
    template <typename T> void warning(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    void                       error(const std::string& message);
    template <typename T> void error(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    void        set_min_level(LogLevel level);
    std::string get_name() const;

  private:
    std::string   _name;
    std::ofstream _logFile;
    LogLevel      _minLevel;
    bool          _fileEnabled;

    Logger();
    Logger(const Logger& inst);

    Logger& operator=(const Logger& inst);

    std::string _get_current_time();
    std::string _level_to_string(LogLevel level);
    std::string _get_color_for_level(LogLevel level);
};

#endif
