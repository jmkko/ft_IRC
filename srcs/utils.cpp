#include "utils.hpp"

bool check_port(const std::string& s, int* port)
{
	long n = utils::string_to_ulong(s.c_str());
	if (!(n > WELL_KNOWN_PORT_MAX && n < DYNAMIC_PORT_MIN))
		return false;
	*port = static_cast<int>(n);
	return true;
}

bool check_password(const std::string& s)
{
	if (s.length() < MIN_PASSWORD_LEN) {
		LOG_SERVER.warning(std::string("password must be ") + TO_STRING(MIN_PASSWORD_LEN) + " long at least");
		return false;
	}
	return true;
}

bool check_args(int ac, char** av, int* port)
{
	if (ac != EXPECTED_ARGS_NB) {
		LOG_SERVER.warning("usage: ./ircserv <port> <password>");
		return false;
	}
	if (!check_port(av[1], port))
		return false;
	if (!check_password(av[2]))
		return false;
	return true;
}

namespace utils
{
long string_to_ulong(const std::string& str)
{
	std::stringstream ss(str);
	long			  result = 0;
	ss >> result;
	if (ss.fail()) {
		return -1;
	}
	return result;
}
} // namespace utils
