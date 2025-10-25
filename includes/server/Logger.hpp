/**
 * @file Logger.hpp
 * @brief Logger class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Client.hpp"

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

/**
 * @brief severity levels for logging
 * @enum LogLevel
 */
enum LogLevel {
    DETAIL = 0, /** detail log level. For extra debugging */
    DEBUG  = 1, /** debug  log level. For dev purpose */
    INFO   = 2, /** info log level. For common behaviors that need to draw attention upon  */
    WARN   = 3, /** warning log level. For uncommon behaviors not threatening program execution */
    ERROR  = 4  /** error log level. For fatal behaviors */
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-to-enum)
#define PADDING_FILE_FUN 50
#define RESET            NC

/**
 * @brief implements a Logger
 * @details has a main @ref log function with overloads
 * reponsabilities :
 * - log with different levels
 * - handle optional file output
 * @class Logger
 */
class Logger
{
  public:
    /**
     * @brief Construct a new Logger object
     *
     * @param logName
     * @param filename
     * @param level
     */
    Logger(const std::string& logName, const std::string& filename = "irc_server.log", LogLevel level = INFO);

    /**
     * @brief Destroy the Logger object
     *
     */
    ~Logger();

    /**
     * @brief base logging function
     * @remark not to be used directly
     * @param level
     * @param message
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief simple logging : logs an object (string. IRC object...)
     * @pre object T should have an ouput operator `<<` overload
     * @tparam T object to log
     * @param level
     * @param file
     * @param function
     * @param message
     */
    template <typename T> void log(LogLevel level, const std::string& file, const std::string& function, const T& message)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << std::left << std::setw(PADDING_FILE_FUN) << (file + ": " + function + "() ") << message;
        log(level, oss.str());
    }

    /**
     * @brief variable value debugging : logs a variable name and its value
     * @pre object T should have an ouput operator `<<` overload
     * @tparam T
     * @param level
     * @param file
     * @param function
     * @param varName
     * @param varVal
     */
    template <typename T>
    void log(LogLevel level, const std::string& file, const std::string& function, const std::string& varName, const T& varVal)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << std::left << std::setw(PADDING_FILE_FUN) << (file + ": " + function + "() ") << varName << " -> " << varVal;
        log(level, oss.str());
    }

    /**
     * @brief context debugging : logs a context message -> and object value
     * @pre object T should have an ouput operator `<<` overload
     * @tparam T
     * @param context
     * @param t
     */
    template <typename T> void log(const std::string& context, const T& t)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << context << " -> " << t;
        log(DEBUG, oss.str());
    }

    /**
     * @brief simple logging : only one string message
     * @deprecated as it doesn't display file and function
     * @tparam T
     * @param t
     */
    template <typename T> void log(const T& t)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << t;
        log(DEBUG, oss.str());
    }

    /**
     * @brief helper function to log at detail level
     * 
     * @tparam T 
     * @param file 
     * @param function 
     * @param t 
     */
    template <typename T> void detail(const std::string& file, const std::string& function, const T& t)
    {
        log(DETAIL, file, function, t);
    }

    /**
     * @brief log at debug level
     * @deprecated as it does not log file and function
     * @param message
     */
    void debug(const std::string& message);

    /**
     * @brief log at debug level
     * @deprecated as it does not log file and function
     * @param desc description
     * @param message
     */
    void debug(const std::string& desc, const std::string& message);

    /**
     * @brief helper function to log at debug level
     * 
     * @tparam T 
     * @param file 
     * @param function 
     * @param t 
     */
    template <typename T> void debug(const std::string& file, const std::string& function, const T& t)
    {
        log(DEBUG, file, function, t);
    }

    /**
     * @brief helper function with specific format for logging received messages by server
     *
     * @param file
     * @param function
     * @param rawMessage
     * @param client
     */
    void receiving(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);

    /**
     * @brief helper function with specific format for logging sent messages by server
     *
     * @param file
     * @param function
     * @param rawMessage
     * @param client
     */
    void sending(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);

    /**
     * @brief log at info level
     * @deprecated as it does not log file and function
     *
     * @param message
     */
    void info(const std::string& message);

    /**
     * @brief helper function for logging at info level
     * 
     * @tparam T 
     * @param file 
     * @param function 
     * @param t 
     */
    template <typename T> void info(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    /**
     * @brief log at warning level
     * @deprecated as it does not log file and function
     * @param message
     */
    void warning(const std::string& message);

/**
 * @brief helper function to log at warning level
 * 
 * @tparam T 
 * @param file 
 * @param function 
 * @param t 
 */
    template <typename T> void warning(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    /**
     * @brief log at error level
     * @deprecated as it does not log file and function
     * @param message 
     */
    void error(const std::string& message);

    /**
     * @brief helper function to log at error level
     * 
     * @tparam T 
     * @param file 
     * @param function 
     * @param t 
     */
    template <typename T> void error(const std::string& file, const std::string& function, const T& t)
    {
        log(INFO, file, function, t);
    }

    /**
     * @brief Set the min level from which logs will be visible
     *
     * @param level
     */
    void set_min_level(LogLevel level);

    /**
     * @brief Get the logger name
     *
     * @return std::string
     */
    std::string get_name() const;

  private:
    std::string   _name;
    std::ofstream _logFile;
    LogLevel      _minLevel;
    bool          _fileEnabled;

    Logger();
    Logger(const Logger& inst);

    Logger& operator=(const Logger& inst);

    /**
     * @brief display current timestamp
     *
     * @return std::string
     */
    std::string _get_current_time();

    /**
     * @brief gets level of loving as a string
     *
     * @param level
     * @return std::string
     */
    std::string _level_to_string(LogLevel level);

    /**
     * @brief get adequqte color for level
     *
     * @param level
     * @return std::string
     */
    std::string _get_color_for_level(LogLevel level);
};

#endif
