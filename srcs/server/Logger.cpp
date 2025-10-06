#include "Logger.hpp"

#include "colors.hpp"
#include "consts.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

Logger::Logger(const std::string& logName, const std::string& filename, LogLevel level) :
    _name(logName), _minLevel(level), _fileEnabled(false)
{
    _logFile.open(filename.c_str(), std::ios::app);
    if (_logFile.is_open()) {
        _fileEnabled = true;
        log(INFO, _name + " Logger initialized - file: " + filename);
    } else {
        std::cerr << "Warning: Could not open log file " << filename << '\n';
    }
}

Logger::~Logger()
{
    log(INFO, _name + " Logger shutting down");
    if (_fileEnabled) {
        _logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message)
{
    if (level < _minLevel)
        return;

    std::string timestamp = _get_current_time();
    std::string levelStr  = _level_to_string(level);
    std::string color     = _get_color_for_level(level);
    if (_name == "TEST")
        color = PURPLEBACK;
    std::string terminalMessage = timestamp + " " + color + "[" + levelStr + "]" + RESET + " " + _name + "\t  " + message;
    std::string fileMessage     = timestamp + " [" + levelStr + "] " + message;
    if (level == ERROR)
        std::cerr << terminalMessage << '\n';
    else
        std::cout << terminalMessage << '\n';
    if (_fileEnabled) {
        _logFile << fileMessage << '\n';
        _logFile.flush();
    }
}
void Logger::receiving(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client) 
{

	log_full(INFO, file, function, std::string("") + "⬅️  receiving from " + BWHITE + client->get_nickname() + RESET, rawMessage);
}

void Logger::sending(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client) 
{
	std::string context = "";
	if (rawMessage.find("ERR") != std::string::npos)
		context = "⚠️  ";
	log_full(INFO, file, function, std::string(context) + "➡️  sending to " + BWHITE + client->get_nickname() + RESET, rawMessage);
}

void Logger::debug(const std::string& desc, const std::string& message) { log(DEBUG, desc + " -> " + message); }
void Logger::debug(const std::string& message) { log(DEBUG, message); }
void Logger::info(const std::string& message) { log(INFO, message); }
void Logger::warning(const std::string& message) { log(WARN, message); }
void Logger::error(const std::string& message) { log(ERROR, message); }

std::string	Logger::get_name() const
{
	return _name;
}

void Logger::set_min_level(LogLevel level) { _minLevel = level; }

std::string Logger::_level_to_string(LogLevel level)
{
    switch (level) {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO ";
    case WARN:
        return "WARN ";
    case ERROR:
        return "ERROR";
    default:
        return "UNKN ";
    }
}

std::string Logger::_get_color_for_level(LogLevel level)
{
    switch (level) {
    case DEBUG:
        return BLUE;
    case INFO:
        return GREEN;
    case WARN:
        return YELLOW;
    case ERROR:
        return RED;
    default:
        return RESET;
    }
}

std::string Logger::_get_current_time()
{
    std::time_t        now   = std::time(0);
    struct std::tm*    tmPtr = std::localtime(&now);
    std::ostringstream oss;
    oss << "[" << (tmPtr->tm_year + EPOCH_TIME_START) << "-" << std::setfill('0') << std::setw(2) << (tmPtr->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << tmPtr->tm_mday << " " << std::setfill('0') << std::setw(2) << tmPtr->tm_hour
        << ":" << std::setfill('0') << std::setw(2) << tmPtr->tm_min << ":" << std::setfill('0') << std::setw(2) << tmPtr->tm_sec
        << "]";
    return oss.str();
}
