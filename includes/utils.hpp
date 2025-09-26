#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

bool check_args(int ac, char** av, int* port);
bool check_port(const std::string& s, int* port);
bool check_password(const std::string& s);

namespace utils
{
template <typename T> std::string to_string(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

long string_to_ulong(const std::string& str);

template <size_t N> char& safe_at(char (&arr)[N], size_t index)
{
	if (index >= N)
		throw std::out_of_range(("out of array range"));
	return arr[index];
}
} // namespace utils

#define TO_STRING utils::to_string

#endif
