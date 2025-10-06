#include "AssertUtils.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "consts.hpp"
#include "testList.hpp"
#include "testUtils.hpp"

#include <cstring>
#include <exception>
#include <memory>

int main()
{
    LogManager& lm = LogManager::get_instance();
	#ifdef DEB
    lm.set_global_level(DEBUG);
	#else
	lm.set_global_level(ERROR);
	#endif
    try {
        // Start server once for all tests
        Server* s = new Server(TEST_PORT, DEFAULT_PASSWORD);
		ServerRunner runner(*s);
		runner.start();

        // Run command tests
        LOG_TEST.info("Running test suite...");
        // test_nick();
        // test_kick();
        test_mode();
        
        LOG_TEST.info("All tests completed, stopping server...");
		runner.stop();
		delete s; // NOLINT(cppcoreguidelines-owning-memory)
        
    } catch (const std::exception& e) {
        std::cerr << "Caught : " << e.what() << ": " << strerror(errno) << '\n';
        return 1;
    }
    
    LOG_TEST.info("Test suite finished successfully");
    return 0;
}