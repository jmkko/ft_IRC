#include "AssertUtils.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "testList.hpp"
#include "testUtils.hpp"

#include <exception>

int main()
{
    LogManager& lm = LogManager::get_instance();
    lm.set_global_level(ERROR);

    try {
        Server server(TEST_PORT, DEFAULT_PASSWORD);
        test_nick(server);
        test_kick(server);
        test_who(server);
        test_join(server);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
