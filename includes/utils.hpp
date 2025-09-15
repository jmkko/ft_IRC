#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

namespace utils {
    template<typename T>
    std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

#define TO_STRING utils::toString

#endif
