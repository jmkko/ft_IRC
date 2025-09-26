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

LogManager::~LogManager()
{
	std::map<std::string, Logger*>::iterator it;
	for (it = _loggers.begin(); it != _loggers.end(); ++it) {
		delete it->second;
	}
}

Logger& LogManager::get_logger(const std::string& name, const std::string& filename, LogLevel level)
{
	if (_loggers.find(name) == _loggers.end()) {
		std::string logFile = filename.empty() ? ("logs/" + name + ".log") : filename;

		if (filename.empty()) {
			struct stat st = {};
			if (stat("logs", &st) == -1) {
				if (mkdir("logs", PERM_755) == -1) {
					std::cerr << "Warning: Could not create logs directory: " << strerror(errno) << std::endl;
				}
			}
		}

		_loggers[name] = new Logger(name, logFile, level);
	}
	return *_loggers[name];
}

void LogManager::set_global_level(LogLevel level)
{
	std::map<std::string, Logger*>::iterator it;
	for (it = _loggers.begin(); it != _loggers.end(); ++it) {
		it->second->set_min_level(level);
	}
}
