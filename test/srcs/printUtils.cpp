#include "printUtils.hpp"

#include "colors.hpp"

#include <iostream>

void print_test_series(const std::string& seriesName) { std::cout << YELLOW << seriesName << NC << '\n'; }

void print_test_series_part(const std::string& part) { std::cout << IWHITE << part << NC << '\n'; }

void print_test_name(const std::string& testName) { std::cout << BCYAN << testName << NC << '\n'; }

void print_success(const std::string& test_name) { std::cout << " ✅ " << test_name << "\n"; }

void print_error(const std::string& test_name, const std::string& cause)
{
    std::cout << " ❌ " << test_name << " (" << cause << ")\n";
}