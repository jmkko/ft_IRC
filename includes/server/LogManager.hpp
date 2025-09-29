#ifndef LOGMANAGER
#define LOGMANAGER

#include "Logger.hpp"

class LogManager
{
  public:
	~LogManager();

	static LogManager& get_instance();
	Logger&			   get_logger(const std::string& name, const std::string& filename, LogLevel level);
	void			   set_global_level(LogLevel level);

  private:
	LogManager();
	LogManager(const LogManager& inst);

	LogManager& operator=(const LogManager& inst);

	std::map<std::string, Logger*>	_loggers;
	LogLevel						_globalLevel;
};

// Macros for easy usage
#define LOG_SOCKET LogManager::get_instance().get_logger("SOCKET", "", DEBUG)
#define LOG_SERVER LogManager::get_instance().get_logger("SERVER", "", DEBUG)
#define LOG_CONN   LogManager::get_instance().get_logger("CONNECTION", "", DEBUG)
#define LOG_CMD	   LogManager::get_instance().get_logger("COMMAND", "", DEBUG)
#define LOG_ERR	   LogManager::get_instance().get_logger("ERROR", "", ERROR)
#define LOG_TEST   LogManager::get_instance().get_logger("TEST", "", DEBUG)

#endif
