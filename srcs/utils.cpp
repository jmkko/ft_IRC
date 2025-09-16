#include "utils.hpp"

bool	checkPort(const std::string& s, int* port)
{
	long n = utils::stringToULong(s.c_str());
	if (!(n > WELL_KNOWN_PORT_MAX && n < DYNAMIC_PORT_MIN))
		return false;
	*port = n;
	return true;
}

bool	checkPassword(const std::string& s)
{
	if (s.length() < 8)
	{
		LOG_SERVER.warning("password must be 8 characters long at least");
		return false;
	}
	return true;
}

bool	checkArgs(int ac, char** av, int* port)
{
	if (ac != 3)
	{
		LOG_SERVER.warning("usage: ./ircserv <port> <password>");
		return false;
	}
	if (!checkPort(av[1], port))
		return false;
	if (!checkPassword(av[2]))
		return false;
	return true;
}

namespace utils {
	long stringToULong(const std::string& str) {
		std::stringstream ss(str);
		long result;
		ss >> result;
		if (ss.fail()) {
			return -1;
		}
		return result;
	}
}
