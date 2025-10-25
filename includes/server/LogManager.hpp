/**
 * @file LogManager.hpp
 * @brief LogManager class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef LOGMANAGER
#define LOGMANAGER

#include "Logger.hpp"

class LogManager
{
  public:
    ~LogManager();

    /**
     * @brief Singleton pattern, this function allows you to have
     * only one instance of this class
     *
     * @return the only one instance of LogManager
     */
    static LogManager& get_instance();
    /**
     * @brief return a logger with specified name and level
     * @details if the Logger doesn't exist it is instanciated
     *
     * @param name of Logger
     * @param filename to store the Logger messages by Default LoggerName.log
     * @param level minlevel
     * @return the Logger
     */
    Logger& get_logger(const std::string& name, const std::string& filename, LogLevel level);
    /**
     * @brief set the minlevel allowed
     *
     * @param level (DETAIL, DEBUG, INFO, WARN, ERROR)
     */
    void set_global_level(LogLevel level);

  private:
    LogManager();
    LogManager(const LogManager& inst);

    LogManager& operator=(const LogManager& inst);

    std::map<std::string, Logger*> _loggers;
    LogLevel                       _globalLevel;
};

/**
* @section Macros macros naming
* @subsection Original original macros
* original macro names follow the pattern `LOG_<logger>`
* they can be called this way `LOG_CMD.debug("message")`
* @subsection Extra extra macros
* other macro names follow the pattern `LOG_<level><extra>_<logger>`
* - those macro make a call to 
* - level being the first letter of DEBUG, INFO, WARNING, ERROR - and DT for DETAIL. If letter is lowercase, then it will log a single message.
* - extra V is for variable display : accepts one arg and prints variable name and its value
* 
* they generate file name and function.
* they can be called this way `LOG_d_CMD("message")`
*/
#define LOG_SOCKET LogManager::get_instance().get_logger("SOCKET", "", DEBUG)
#define LOG_SERVER LogManager::get_instance().get_logger("SERVER", "", DEBUG)
#define LOG_CONN   LogManager::get_instance().get_logger("CONNECTION", "", DEBUG)
#define LOG_CMD    LogManager::get_instance().get_logger("COMMAND", "", DEBUG)
#define LOG_ERR    LogManager::get_instance().get_logger("ERROR", "", ERROR)
#define LOG_TEST   LogManager::get_instance().get_logger("TEST", "", DEBUG)

#define LOG_DT_SERVER(message, data)                                                                                             \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_DTV_SERVER(data)                                                                                                     \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_dt_SERVER(message)                                                                                                   \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message)
#define LOG_D_SERVER(message, data)                                                                                              \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_d_SERVER(message)                                                                                                    \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message)
#define LOG_DV_SERVER(data)                                                                                                      \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_I_SERVER(message, data)                                                                                              \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(INFO, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_W_SERVER(message, data)                                                                                              \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_w_SERVER(message)                                                                                                    \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message)
#define LOG_E_SERVER(message, data)                                                                                              \
    LogManager::get_instance().get_logger("SERVER", "", DEBUG).log(ERROR, __FILE_NAME__, __FUNCTION__, message, data)

#define LOG_DT_CMD(message, data)                                                                                                \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_DTV_CMD(data)                                                                                                        \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_dt_CMD(message)                                                                                                      \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message)
#define LOG_D_CMD(message, data)                                                                                                 \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_d_CMD(message)                                                                                                       \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message)
#define LOG_DV_CMD(data)                                                                                                         \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_I_CMD(message, data)                                                                                                 \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(INFO, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_W_CMD(message, data)                                                                                                 \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_w_CMD(message)                                                                                                       \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message)
#define LOG_E_CMD(message, data)                                                                                                 \
    LogManager::get_instance().get_logger("COMMAND", "", DEBUG).log(ERROR, __FILE_NAME__, __FUNCTION__, message, data)

#define LOG_DT_TEST(message, data)                                                                                               \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_DTV_TEST(data)                                                                                                       \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_dt_TEST(message)                                                                                                     \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DETAIL, __FILE_NAME__, __FUNCTION__, message)
#define LOG_D_TEST(message, data)                                                                                                \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_d_TEST(message)                                                                                                      \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, message)
#define LOG_DV_TEST(data)                                                                                                        \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(DEBUG, __FILE_NAME__, __FUNCTION__, #data, data)
#define LOG_I_TEST(message, data)                                                                                                \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(INFO, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_W_TEST(message, data)                                                                                                \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message, data)
#define LOG_w_TEST(message)                                                                                                      \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(WARN, __FILE_NAME__, __FUNCTION__, message)
#define LOG_E_TEST(message, data)                                                                                                \
    LogManager::get_instance().get_logger("TEST", "", DEBUG).log(ERROR, __FILE_NAME__, __FUNCTION__, message, data)

#endif
