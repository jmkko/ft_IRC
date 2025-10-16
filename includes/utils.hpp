#ifndef UTILS_HPP
#define UTILS_HPP

#include "Client.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

bool is_special_abnf(char c);
bool check_args(int ac, char** av, int* port);
bool check_port(const std::string& s, int* port);
bool check_password(const std::string& s);

namespace utils
{
template <typename T> std::string to_string(const T& value)
{
    std::ostringstream oss;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    oss << value;
    return oss.str();
}

const std::string code_to_str(ReplyCode code);

long        string_to_ulong(const std::string& str);
std::string event_to_str(int event);

template <size_t N> char& safe_at(char (&arr)[N], size_t index)
{
    if (index >= N)
        throw std::out_of_range(("out of array range"));
    return arr[index];
}

bool is_matching_pattern(const std::string& pattern, const std::string& str);

/**
 * @struct MatchPattern
 *
 * @brief This is a Functor for finding Client by searching in each
 * field if his infotching the pattern
 *
 */
struct MatchPattern {
    std::string pattern;
    MatchPattern(const std::string& p);
    bool operator()(const Client* c) const;
};

} // namespace utils

#define TO_STRING    utils::to_string
#define EVENT_TO_STR utils::event_to_str

#endif
