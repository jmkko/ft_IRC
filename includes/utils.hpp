#ifndef UTILS_HPP
#define UTILS_HPP

#include "Client.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#define TO_STRING    Utils::to_string
#define EVENT_TO_STR Utils::event_to_str

class Utils
{
  public:
    static bool is_special_abnf(char c);
    static bool check_args(int ac, char** av, int* port);
    static bool check_port(const std::string& s, int* port);
    static bool check_password(const std::string& s);
    static bool is_char_of(unsigned char c, const std::string& set);

    template <typename T> static std::string to_string(const T& value)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << value;
        return oss.str();
    }

    static const std::string code_to_str(ReplyCode code);

    static long        string_to_ulong(const std::string& str);
    static std::string event_to_str(int event);

    template <size_t N> static char& safe_at(char (&arr)[N], size_t index)
    {
        if (index >= N)
            throw std::out_of_range(("out of array range"));
        return arr[index];
    }

    static bool is_matching_pattern(const std::string& pattern, const std::string& str);

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
    static bool is_invalid_nick(char c);

}; // class utils

#endif
