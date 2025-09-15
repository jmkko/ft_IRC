#include "LogManager.hpp"
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <iostream>

LogManager* LogManager::_instance = 0;

LogManager &LogManager::getInstance() {
    if (!_instance) {
        _instance = new LogManager();
    }
    return *_instance;
}

LogManager::~LogManager() {
    std::map<std::string, Logger *>::iterator it;
    for (it = _loggers.begin(); it != _loggers.end(); ++it) {
        delete it->second;
    }
}

Logger& LogManager::getLogger(const std::string& name, const std::string& filename, 
                      LogLevel level) {
        if (_loggers.find(name) == _loggers.end()) {
            std::string logFile = filename.empty() ? ("logs/" + name + ".log") : filename;
            
            if (filename.empty()) {
                struct stat st;
                if (stat("logs", &st) == -1) {
                    if (mkdir("logs", 0755) == -1) {
                        std::cerr << "Warning: Could not create logs directory: " << strerror(errno) << std::endl;
                    }
                }
            }
            
            _loggers[name] = new Logger(name, logFile, level);
        }
        return *_loggers[name];
    }

void LogManager::setGlobalLevel(LogLevel level) {
    std::map<std::string, Logger *>::iterator it;
    for (it = _loggers.begin(); it != _loggers.end(); ++it) {
        it->second->setMinLevel(level);
    }
}
