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
#include <functional>
#include <memory>
#include <vector>

int main(int ac, char** av)
{
    LogManager& lm = LogManager::get_instance();
#ifdef DEB
    lm.set_global_level(DEBUG);
#else
    lm.set_global_level(ERROR);
#endif
    try {
        // Start server once for all tests
        Server*      s = new Server(TEST_PORT, DEFAULT_PASSWORD);
        ServerRunner runner(*s);
        runner.start();

        // Run command tests
        LOG_TEST.info("Running test suite...");

        if (ac != 1) {
            std::map<std::string, void (*)(Server&)> functions;

            functions["MODE"]    = &test_mode;
            functions["NICK"]    = &test_nick;
            functions["KICK"]    = &test_kick;
            functions["WHO"]     = &test_who;
            functions["JOIN"]    = &test_join;
            functions["PRIVMSG"] = &test_privmsg;

            functions[av[1]](*s);
        } else {
            test_mode(*s);
            test_nick(*s);
            test_kick(*s);
            test_who(*s);
            test_join(*s);
            test_privmsg(*s);
        }

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
