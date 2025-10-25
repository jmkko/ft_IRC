#include "LogManager.hpp"

#include "consts.hpp"

#include <cstring>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>

LogManager& LogManager::get_instance()
{
    static LogManager instance;
    return instance;
}

/************************************************************
 *            🥚 CONSTRUCTORS & DESTRUCTOR                  *
 ************************************************************/

LogManager::LogManager() : _globalLevel(DEBUG) {}

LogManager::~LogManager()
{
    std::map<std::string, Logger*>::iterator it;
    for (it = _loggers.begin(); it != _loggers.end(); ++it) {
        delete it->second;
    }
}

/*************************************************************
 *                      🛠️ FUNCTIONS                         *
 *************************************************************/

Logger& LogManager::get_logger(const std::string& name, const std::string& filename, LogLevel level)
{
    if (_loggers.find(name) == _loggers.end()) {
        std::string logFile = filename.empty() ? ("logs/" + name + ".log") : filename;

        if (filename.empty()) {
            struct stat st = {};
            if (stat("logs", &st) == -1) {
                if (mkdir("logs", PERM_777) == -1) {
                    std::cerr << "Warning: Could not create logs directory: " << strerror(errno) << '\n';
                }
            }
        }
        if (level < _globalLevel)
            level = _globalLevel;
        _loggers[name] = new Logger(name, logFile, level);
    }
    return *_loggers[name];
}

void LogManager::set_global_level(LogLevel level)
{
    _globalLevel = level;
    std::map<std::string, Logger*>::iterator it;
    for (it = _loggers.begin(); it != _loggers.end(); ++it) {
        it->second->set_min_level(level);
    }
}
