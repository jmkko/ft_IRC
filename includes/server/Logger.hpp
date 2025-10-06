#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Client.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

enum LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3 };

#define RESET NC

class Logger
{
  public:
	Logger(const std::string& logName, const std::string& filename = "irc_server.log", LogLevel level = INFO);
	~Logger();

	void					   log(LogLevel level, const std::string& message);
	void					   debug(const std::string& message);
	void 						debug(const std::string& desc, const std::string& message);
	template <typename T> void debug(const std::string& context, const T& t)
	{
		std::ostringstream oss;
		//NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		oss << context << " -> " << t;
		log(DEBUG, oss.str());
	}

	template <typename T> void log_full(LogLevel level, const std::string& file, const std::string& function, const std::string& varName, const T& varVal)
	{
		std::ostringstream oss;
		//NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		oss << file << ": " << function << "() " << varName << " -> " << varVal;
		log(level, oss.str() );
	}

	void receiving(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);
	void sending(const std::string& file, const std::string& function, const std::string& rawMessage, Client* client);

	void info(const std::string& message);
	void warning(const std::string& message);
	void error(const std::string& message);
	void set_min_level(LogLevel level);
	std::string	get_name() const;

  private:
	std::string	  _name;
	std::ofstream _logFile;
	LogLevel	  _minLevel;
	bool		  _fileEnabled;

	Logger();
	Logger(const Logger& inst);

	Logger& operator=(const Logger& inst);

	std::string _get_current_time();
	std::string _level_to_string(LogLevel level);
	std::string _get_color_for_level(LogLevel level);
};

#endif
