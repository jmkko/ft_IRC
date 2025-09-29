#ifndef ASSERT_FAIL_HPP
#define ASSERT_FAIL_HPP

#include "AssertFail.hpp"

AssertFail::AssertFail(
		const std::string& context,
		const std::string& expected,
		const std::string& actual) : _message(std::string("expected " + expected + " in " + context + " but found " + actual)), _expected(expected), _actual(actual) {}

const char* AssertFail::what() const noexcept
{
	return _message.c_str();
}

#endif