#ifndef UTILS_HPP
#define UTILS_HPP

#include "LogManager.hpp"
#include "consts.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

bool checkArgs(int ac, char** av, int* port);
bool checkPort(const std::string& s, int* port);
bool checkPassword(const std::string& s);

namespace utils
{
template <typename T> std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

long stringToULong(const std::string& str);

template <size_t N> char& safeAt(char (&arr)[N], size_t index)
{
    if (index >= N)
        throw std::out_of_range(("out of array range"));
    return arr[index];
}

} // namespace utils

#define TO_STRING utils::toString

#endif
