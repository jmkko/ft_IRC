#include "LogManager.hpp"
#include "Logger.hpp"
#include "TcpSocket.hpp"
#include "TestFixture.hpp"
#include "colors.hpp"
#include "fakeClient.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstddef>
#include <thread>
#include <unistd.h> // close

TestFixture::TestFixture(Server& s) : _sockets(), _server(s) {}

TestFixture::~TestFixture() { cleanup(); }

std::vector<TcpSocket*> TestFixture::setup(size_t nbSockets)
{
    LOG_TEST.debug("Fixture setup - creating " + Utils::to_string(nbSockets) + " socket(s)");

    std::vector<TcpSocket*> socketsPtrs;
    _sockets.reserve(_sockets.size() + nbSockets);

    for (size_t i = 0; i < nbSockets; ++i) {
        _sockets.push_back(make_client_socket(TEST_PORT));
        socketsPtrs.push_back(_sockets.back().get());
    }

    // Give server time to process new connections
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));

    return socketsPtrs;
}

void TestFixture::cleanup()
{
#ifdef DEB
    std::cout << BWHITE << "=====================END OF TEST=====================" << RESET << '\n';
#endif
    if (!_sockets.empty()) {
        LOG_dt_TEST("Fixture cleanup - closing " + Utils::to_string(_sockets.size()) + " socket(s)");
        for (auto& socket : _sockets) {
            if (socket && socket->get_socket() != -1) {
                // Send QUIT command to properly disconnect from server
                send_line(*socket.get(), validQuitMsg);

                // Give server time to process QUIT
                std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
                close(socket->get_socket());
            }
        }
        _sockets.clear();
    }
}
