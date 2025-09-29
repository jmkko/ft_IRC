#include "printUtils.hpp"

#include "colors.hpp"

#include <iostream>

void	print_test_series(const std::string& seriesName)
{
	std::cout << YELLOW << seriesName << NC << '\n';
}

void	print_test_name(const std::string& testName)
{
	std::cout << BCYAN << testName << NC << '\n';
}

void	print_success()
{
	std::cout << " ✅ "; 
}

void	print_error(const std::string& test_name)
{
	std::cout << " ❌ [" << test_name << "] ";
}