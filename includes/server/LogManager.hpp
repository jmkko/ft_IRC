#ifndef LOGMANAGER
#define LOGMANAGER

#include "Logger.hpp"
#include "consts.hpp"

class LogManager
{
  private:
    std::map<std::string, Logger*> _loggers;

    LogManager(const LogManager& inst);

    LogManager& operator=(const LogManager& inst);
    LogManager(){};

  public:
    static LogManager& getInstance();
    Logger& getLogger(const std::string& name, const std::string& filename, LogLevel level);
    void    setGlobalLevel(LogLevel level);
    ~LogManager();
};

// Macros for easy usage
#define LOG_SOCKET LogManager::getInstance().getLogger("SOCKET", "", DEBUG)
#define LOG_SERVER LogManager::getInstance().getLogger("SERVER", "", INFO)
#define LOG_CONN   LogManager::getInstance().getLogger("CONNECTION", "", INFO)
#define LOG_CMD    LogManager::getInstance().getLogger("COMMAND", "", DEBUG)
#define LOG_ERR    LogManager::getInstance().getLogger("ERROR", "", ERROR)

#endif
