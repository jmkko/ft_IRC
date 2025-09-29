#include "AssertUtils.hpp"
#include "testList.hpp"

#include <exception>

int main()
{
	try {
		test_nick();
	} catch (const std::exception e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}