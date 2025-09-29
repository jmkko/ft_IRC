#ifndef PRINT_UTILS_HPP
#define PRINT_UTILS_HPP

#include <string>

void	print_test_series(const std::string& seriesName);
void	print_test_name(const std::string& testName);
void	print_success();
void	print_error(const std::string& test_name);

#endif