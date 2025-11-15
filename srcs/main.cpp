#include "Config.hpp"
#include "LogManager.hpp"
#include "Server.hpp"
#include "signal_handler.hpp"
#include "utils.hpp"

#include <exception>
#include <iostream>

int main(int ac, char** av)
{
    int port = DEFAULT_PORT;

    LOG_ERR.set_min_level(ERROR);
    if (!Utils::check_args(ac, av, &port) || !ircConfig.exist)
        return 1;
    try {
        Server newServer(port, av[2]);
        setup_signal_handlers();
        newServer.start();
    } catch (std::exception& e) {
        LOG_ERR.error("Server error: " + std::string(e.what()));
    }
    return 0;
}
