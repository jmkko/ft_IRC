/**
 * @file utils.hpp
 * @brief utils class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include "Client.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

#define TO_STRING    Utils::to_string
#define EVENT_TO_STR Utils::event_to_str

class Utils
{
  public:
    /**
     * @brief check is the char is a special according RFC 2812
     *
     * @details special    =  %x5B-60 / %x7B-7D
     *  - ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
     *
     * @param c the char
     * @return true on success
     */
    static bool is_special_abnf(char c);

    /**
     * @brief the argument when launching the IRC SERVER
     *
     * @param ac number of arguments
     * @param av the arguments
     * @param port
     * @return true on success
     */
    static bool check_args(int ac, char** av, int* port);

    /**
     * @brief sub function of check_args check if the port is in a range
     *
     * @param s string value of the port to check
     * @param port pointer to int take the port value on success
     * @return true on success
     */
    static bool check_port(const std::string& s, int* port);

    /**
     * @brief sub function of check_args check if the password respect the rules
     *
     * @param s string value of the password to check
     * @return true on success
     */
    static bool check_password(const std::string& s);

    /**
     * @brief check if a char is in a set
     *
     * @param c the char
     * @param set
     * @return true on success
     */
    static bool is_char_of(unsigned char c, const std::string& set);

    /**
     * @brief convert a Template value to string
     *
     * @tparam T
     * @param value
     * @return the value converted in string
     */
    template <typename T> static std::string to_string(const T& value)
    {
        std::ostringstream oss;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        oss << value;
        return oss.str();
    }

    /**
     * @brief convert a ReplyCode int to 3 char string
     *
     * @param code @ref ReplyCode
     * @return the 3 char ReplyCode string
     */
    static const std::string code_to_str(ReplyCode code);

    static long        string_to_ulong(const std::string& str);
    static std::string event_to_str(int event);

    /**
     * @brief return a value of an array in a secure way avoid out of bound
     *
     * @param arr
     * @param N
     * @param index
     *
     * @return arr
     */
    template <size_t N> static char& safe_at(char (&arr)[N], size_t index)
    {
        if (index >= N)
            throw std::out_of_range(("out of array range"));
        return arr[index];
    }

    /**
     * @brief check according to the RFC 2812 if a string matching a wildcard
     * pattern like : str= user_toto pattern = *to return true
     *
     * @param pattern string like *abc or abc* or *abc**df** etc..
     * @param str string to test
     * @return true if the string match the wildcard pattern
     *
     * *key point*
     * wildcard pattern begin with *
     * wildcard pattern end with *
     * multiple * wildcard pattern
     * - `?` : exactly ONE char
     * - `*` : ZERO or more char
     *
     * As per RFC 2812, wildcards are used for masks in WHO, NAMES, LIST, ... commands
     * @return true on success
     */
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

    static bool is_not_alpha_or_specialbnf(char c);
    static bool is_invalid_char_nick(char c);
    static bool is_invalid_char_user(char c);
    static bool is_invalid_char_key(char c);
    static bool is_not_digit(char c);

}; // class utils

#endif
