#ifndef ASSERTUTILS_H
#define ASSERTUTILS_H

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

template<typename Condition>
void	assert_false(Condition cond) {
	if (cond) {
		std::ostringstream oss;
		oss << cond;
		throw std::runtime_error("Fail - should NOT meet condition " + oss.str());
	}
}

template<typename Condition>
void	assert_true(Condition cond) {
	if (!cond) {
		std::ostringstream oss;
		oss << cond;
		throw std::runtime_error("Fail - should meet condition " + oss.str());
	}
}

template<typename A, typename B>
void	assert_equal(const A& a, const B& b) {
	if (!(a == b))
	{
		std::ostringstream oss;
		oss << "Fail - " << a << " should be equal to " << b;
		throw std::runtime_error(oss.str());
	}
}

template<typename T>
void	assert_null(const T* t) {
	if (t)
	{
		std::ostringstream oss;
		oss << "Fail - value should be NULL";
		throw std::runtime_error(oss.str());
	}
}

template <typename Exception, typename Function>
bool	assert_throws(Function& f)
{	
	try {
		f();
	} catch (const Exception& e) {
		return true;
	} catch (const std::exception& e) {
		std::ostringstream oss;
		oss << "Fail - should throw " << typeid(Exception).name()
		<< ", but threw " << typeid(e).name() << ": " << e.what();
		throw std::runtime_error(oss.str());
	} catch (...)
	{
		std::ostringstream oss;
		oss << "Fail - should throw " << typeid(Exception).name()
		<< ", but an unknown exception was thrown";
		throw std::runtime_error(oss.str());
	}
	std::ostringstream oss;
	oss << "Fail - should throw " << typeid(Exception).name()
	<< ", but no exception was thrown";
	throw std::runtime_error(oss.str());
}

#endif