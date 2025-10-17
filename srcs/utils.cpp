#include "LogManager.hpp"
#include "consts.hpp"
#include "utils.hpp"

#include <cctype>
#include <poll.h>
#include <string>

bool Utils::is_char_of(unsigned char c, const std::string& set)
{
    for (std::string::const_iterator it = set.begin(); it != set.end(); ++it) {
        if (c == static_cast<unsigned char>(*it))
            return true;
    }
    return false;
}

bool Utils::is_special_abnf(char c)
{
    unsigned char uc = (unsigned char)c;
    return (uc >= '[' && uc <= '`') || (uc >= '{' && uc <= '}');
}

bool Utils::check_port(const std::string& s, int* port)
{
    long n = string_to_ulong(s.c_str());
    if (!(n > WELL_KNOWN_PORT_MAX && n < DYNAMIC_PORT_MIN))
        return false;
    *port = static_cast<int>(n);
    return true;
}

bool Utils::check_password(const std::string& s)
{
    if (s.length() < MIN_PASSWORD_LEN) {
        LOG_SERVER.warning(std::string("password must be ") + TO_STRING(MIN_PASSWORD_LEN) + " long at least");
        return false;
    }
    return true;
}

bool Utils::check_args(int ac, char** av, int* port)
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

const std::string Utils::code_to_str(ReplyCode code)
{
    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << code;
    return ss.str();
}

long Utils::string_to_ulong(const std::string& str)
{
    std::stringstream ss(str);
    long              result = 0;
    ss >> result;
    if (ss.fail()) {
        return -1;
    }
    return result;
}

std::string Utils::event_to_str(int event)
{
    switch (event) {
    case POLLIN:
        return ("POLLIN");
    case POLLOUT:
        return ("POLLOUT");
    case POLLERR:
        return ("POLLERR");
    case POLLNVAL:
        return ("POLLNVAL");
    default:
        return ("undefined");
    }
}
/**
 * @brief test if the string matching a wildcard with * pattern
 *
 * @param pattern string like *abc or abc* or *abc**df** etc..
 * @param str string to test
 * @return true if the string match the wildcard pattern
 *
 * key point
 * wildcard pattern begin with *
 * wildcard pattern end with *
 * multiple * wildcard pattern
 * - `?` : exactly ONE char
 * - `*` : ZERO or more char
 *
 * Conformément à la RFC 2812, les wildcards servent pour les masks
 * dans les commandes WHO, NAMES, LIST, etc.
 */
bool Utils::is_matching_pattern(const std::string& pattern, const std::string& str)
{
    if (pattern.empty() || str.empty())
        return false;
    size_t p = 0, s = 0, star = std::string::npos, match = 0;

    while (s < str.size()) {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == str[s])) {
            // char is equal or ?
            p++;
            s++;
        } else if (p < pattern.size() && pattern[p] == '*') {
            // save * pos and move forward in pattern
            // but not in str
            star  = p++;
            match = s;
        } else if (star != std::string::npos) {
            // pas de match direct : on revient au dernier char apres *
            p = star + 1;
            s = ++match;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        p++;
    return (p == pattern.size());
}
Utils::MatchPattern::MatchPattern(const std::string& p) : pattern(p) {}

bool Utils::MatchPattern::operator()(const Client* c) const
{
    return is_matching_pattern(pattern, c->get_nickname()) || is_matching_pattern(pattern, c->get_user_name())
           || is_matching_pattern(pattern, c->get_userhost()) || is_matching_pattern(pattern, c->get_real_name());
}

bool Utils::is_invalid_char_nick(char c) { return (!std::isalnum(c) && !is_special_abnf(c)); }
bool Utils::is_invalid_char_user(char c) { return (is_char_of(c, std::string(FORBIDEN_CHAR_USER, 5))); }
