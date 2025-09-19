#include "utils.hpp"

bool checkPort(const std::string& s, int* port)
{
    long n = utils::stringToULong(s.c_str());
    if (!(n > WELL_KNOWN_PORT_MAX && n < DYNAMIC_PORT_MIN))
        return false;
    *port = static_cast<int>(n);
    return true;
}

bool checkPassword(const std::string& s)
{
    if (s.length() < MIN_PASSWORD_LEN) {
        LOG_SERVER.warning(std::string("password must be ") + TO_STRING(MIN_PASSWORD_LEN) + " long at least");
        return false;
    }
    return true;
}

bool checkArgs(int ac, char** av, int* port)
{
    if (ac != EXPECTED_ARGS_NB) {
        LOG_SERVER.warning("usage: ./ircserv <port> <password>");
        return false;
    }
    if (!checkPort(av[1], port))
        return false;
    if (!checkPassword(av[2]))
        return false;
    return true;
}

namespace utils
{
long stringToULong(const std::string& str)
{
    std::stringstream ss(str);
    long              result = 0;
    ss >> result;
    if (ss.fail()) {
        return -1;
    }
    return result;
}
}
