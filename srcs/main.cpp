#include "LogManager.hpp"
#include "Server.hpp"
#include "signal_handler.hpp"
#include "utils.hpp"

#include <exception>

int main(int ac, char** av)
{
    int port = DEFAULT_PORT;

    LOG_ERR.setMinLevel(ERROR); // Seulement les erreurs dans ce log
    if (!checkArgs(ac, av, &port))
        return 1;
    Server newServer(port, av[2]);
    setupSignalHandlers();
    try {
        newServer.start();
    } catch (std::exception& e) {
        std::cout << e.what() << "\n";
        LOG_ERR.error("Server error: " + std::string(e.what()));
    }
    return 0;
}
