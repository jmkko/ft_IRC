#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "printUtils.hpp"
#include "AssertFail.hpp"

#define SERVER_PROCESS_TIME_MS 10
#define TEST_PORT 6668

/**
 * @brief uses std::forward to preserve category of argument
 * by casting it to an rvalue. 
 * Doesnt pass by value. 
 * Allows to pass biggers lambdas, Functors, std::function
 * @tparam Func 
 * @param f 
 * @param name 
 */
template <typename Func>
void run_test(Func&& f, const char* name)
{
	try {
		std::forward<Func>(f)();
		print_success(name);
	} catch (AssertFail& e) {
		print_error(name, e.what());
	}
}

#endif