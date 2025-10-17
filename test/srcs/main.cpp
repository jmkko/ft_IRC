#include "AssertUtils.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "consts.hpp"
#include "testUtils.hpp"

#include <cstring>
#include <exception>
#include <functional>
#include <memory>
#include <vector>

int main(int ac, char** av)
{
    LogManager& lm = LogManager::get_instance();

    t_results results;
    results.failedTests   = 0;
    results.passedTests   = 0;
    results.launchedTests = 0;

#ifdef DEB
    lm.set_global_level(DETAIL);
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
            std::map<std::string, void (*)(Server&, t_results*)> functions;

            functions["MODE"]    = &test_mode;
            functions["NICK"]    = &test_nick;
            functions["KICK"]    = &test_kick;
            functions["WHO"]     = &test_who;
            functions["JOIN"]    = &test_join;
            functions["PASS"] = &test_pass;
            functions["PRIVMSG"] = &test_privmsg;
            functions["PING"]    = &test_ping;
            functions["TOPIC"]   = &test_topic;
            functions["MOTD"]   = &test_motd;

            functions[av[1]](*s, &results);
        } else {
            test_mode(*s, &results);
            test_nick(*s, &results);
            test_kick(*s, &results);
            test_who(*s, &results);
            test_join(*s, &results);
            test_pass(*s, &results);
            test_privmsg(*s, &results);
            test_ping(*s, &results);
            test_topic(*s, &results);
            test_motd(*s, &results);
        }

        LOG_TEST.info("All tests completed, stopping server...");
        runner.stop();
        delete s; // NOLINT(cppcoreguidelines-owning-memory)

        std::cout << "Launched tests : " << results.launchedTests << '\n';
        std::cout << "Passed : " << results.passedTests << '\n';
        std::cout << "Failed : " << results.failedTests << '\n';

        if (results.failedTests > 0)
            return 1;

    } catch (const std::exception& e) {
        std::cerr << "Caught : " << e.what() << ": " << strerror(errno) << '\n';
        return 1;
    }

    LOG_TEST.info("Test suite finished successfully");
    return 0;
}
