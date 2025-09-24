#include "LogManager.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <exception>
#include <signal.h>

void handleSignal(int signal) {  globalSignal = signal; }

int main(int ac, char** av)
{
    int port = 0;

    LOG_ERR.setMinLevel(ERROR); // Seulement les erreurs dans ce log
    if (!checkArgs(ac, av, &port))
        return 1;
    Server newServer(port, av[2]);
    signal(SIGINT, handleSignal);
    signal(SIGABRT, handleSignal);
    try {
        newServer.start();
    } catch (std::exception& e) {
        std::cout << e.what() << "\n";
        LOG_ERR.error("Server error: " + std::string(e.what()));
    }
    return 0;
}
