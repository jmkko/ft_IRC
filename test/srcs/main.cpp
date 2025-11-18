#include "AssertUtils.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "consts.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <cstring>
#include <exception>
#include <functional>
#include <memory>
#include <sys/stat.h>
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
            functions["PASS"]    = &test_pass;
            functions["PRIVMSG"] = &test_privmsg;
            functions["PING"]    = &test_ping;
            functions["TOPIC"]   = &test_topic;
            functions["MOTD"]    = &test_motd;
            functions["USER"]    = &test_user;
            functions["BOT"]     = &test_bot;

            // functions[av[1]](*s, &results);
            std::string                                                    key(av[1]);
            std::map<std::string, void (*)(Server&, t_results*)>::iterator it = functions.find(key);
            if (it != functions.end())
                it->second(*s, &results);
            else
                std::cerr << "Commande inconnue : " << key << "\n";
        } else {
            test_nick(*s, &results);
            test_user(*s, &results);
            test_who(*s, &results);
            test_mode(*s, &results);
            test_join(*s, &results);
            test_pass(*s, &results);
            test_privmsg(*s, &results);
            test_kick(*s, &results);
            test_ping(*s, &results);
            test_topic(*s, &results);
            test_motd(*s, &results);
            test_bot(*s, &results);
        }
        LOG_TEST.info("End of first test suite...");
        s->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_RERUN_WAIT_MS));
        delete s; // NOLINT(cppcoreguidelines-owning-memory)
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_RERUN_WAIT_MS));

        // if (chmod(SERVER_CONF_FILE_FOR_TEST, 000) != 0) {
        //     throw std::runtime_error(TO_STRING("error changing rights ") + strerror(errno));
        // }
        
        // Server*      s2 = new Server(TEST_PORT, DEFAULT_PASSWORD);
        // ServerRunner runner2(*s2);
        // runner2.start();

        // // Run second command tests
        // LOG_TEST.info("Running test suite without conf file...");

        // if (ac != 1) {
        //     std::map<std::string, void (*)(Server&, t_results*)> functions;

        //     functions["MODE"]    = &test_mode;
        //     functions["NICK"]    = &test_nick;
        //     functions["KICK"]    = &test_kick;
        //     functions["WHO"]     = &test_who;
        //     functions["JOIN"]    = &test_join;
        //     functions["PASS"]    = &test_pass;
        //     functions["PRIVMSG"] = &test_privmsg;
        //     functions["PING"]    = &test_ping;
        //     functions["TOPIC"]   = &test_topic;
        //     functions["MOTD"]    = &test_motd;
        //     functions["USER"]    = &test_user;
        //     functions["BOT"]     = &test_bot;

        //     // functions[av[1]](*s, &results);
        //     std::string                                                    key(av[1]);
        //     std::map<std::string, void (*)(Server&, t_results*)>::iterator it = functions.find(key);
        //     if (it != functions.end())
        //         it->second(*s2, &results);
        //     else
        //         std::cerr << "Commande inconnue : " << key << "\n";
        // } else {
        //     test_nick(*s2, &results);
        //     test_user(*s2, &results);
        //     test_who(*s2, &results);
        //     test_mode(*s2, &results);
        //     test_join(*s2, &results);
        //     test_pass(*s2, &results);
        //     test_privmsg(*s2, &results);
        //     test_kick(*s2, &results);
        //     test_ping(*s2, &results);
        //     test_topic(*s2, &results);
        //     test_motd(*s2, &results);
        //     test_bot(*s2, &results);
        // }
        // if (chmod(SERVER_CONF_FILE_FOR_TEST, PERM_644) != 0) {
        //     throw std::runtime_error(TO_STRING("error changing rights ") + strerror(errno));
        // }
        // runner2.stop();
        // delete s2; // NOLINT(cppcoreguidelines-owning-memory)

        LOG_TEST.info("All tests completed, stopping server...");

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
