#ifndef UTILS_HPP
#define UTILS_HPP

#include "consts.hpp"
#include "LogManager.hpp"

#include <string>
#include <sstream>
#include <iostream>

bool	checkArgs(int ac, char** av, int* port);
bool	checkPort(const std::string& s, int* port);
bool	checkPassword(const std::string& s);

namespace utils {
    template<typename T>
    std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

	long stringToULong(const std::string& str);
}

#define TO_STRING utils::toString

#endif
