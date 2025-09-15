#include "Server.hpp"
#include <exception>
#include "LogManager.hpp"

int main() {

    Server newServer(9999, "passwd");
    LOG_ERR.setMinLevel(ERROR);  // Seulement les erreurs dans ce log
    try {
        newServer.start();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        LOG_ERR.error("Server error: " + std::string(e.what()));
    }
    return 0;
}
