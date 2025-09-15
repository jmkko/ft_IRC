#include "Logger.hpp"


Logger::Logger(const std::string& logName, const std::string &filename, LogLevel level)
    : _name(logName), _minLevel(level), _fileEnabled(false) {

    _logFile.open(filename.c_str(), std::ios::app);
    if (_logFile.is_open()) {
        _fileEnabled = true;
        log(INFO, "Logger initialized - file: " + filename);
    } else {
        std::cerr << "Warning: Could not open log file " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (_fileEnabled) {
        log(INFO, "Logger shutting down");
        _logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string &message) {
    if (level < _minLevel)
        return;

    std::string timestamp = getCurrentTime();
    std::string levelStr = levelToString(level);
    std::string color = getColorForLevel(level);
    std::string terminalMessage = timestamp + " " + color + "[" + levelStr + "]" + RESET + " " + message;
    std::string fileMessage = timestamp + " [" + levelStr + "] " + message;
    std::cout << terminalMessage << std::endl;
    if (_fileEnabled) {
        _logFile << fileMessage << std::endl;
        _logFile.flush();
    }
}

void Logger::debug(const std::string &message) { log(DEBUG, message); }
void Logger::info(const std::string &message) { log(INFO, message); }
void Logger::warning(const std::string &message) { log(WARN, message); }
void Logger::error(const std::string &message) { log(ERROR, message); }

void Logger::setMinLevel(LogLevel level) { _minLevel = level; }

std::string Logger::levelToString(LogLevel level) {
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

std::string Logger::getColorForLevel(LogLevel level) {
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

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(0);
    struct std::tm* tm_ptr = std::localtime(&now);
    std::ostringstream oss;
    oss << "[" << (tm_ptr->tm_year + 1900) << "-"
        << std::setfill('0') << std::setw(2) << (tm_ptr->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << tm_ptr->tm_mday << " "
        << std::setfill('0') << std::setw(2) << tm_ptr->tm_hour << ":"
        << std::setfill('0') << std::setw(2) << tm_ptr->tm_min << ":"
        << std::setfill('0') << std::setw(2) << tm_ptr->tm_sec << "]";
    return oss.str();
}
