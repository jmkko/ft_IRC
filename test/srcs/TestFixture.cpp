#include "TestFixture.hpp"

#include "LogManager.hpp"
#include "Logger.hpp"
#include "TcpSocket.hpp"
#include "fakeClient.hpp"
#include "testUtils.hpp"
#include "utils.hpp"
#include "colors.hpp"

#include <cstddef>
#include <thread>
#include <chrono>

TestFixture::TestFixture() : _sockets() {}

TestFixture::~TestFixture() { cleanup(); }

std::vector<TcpSocket*> TestFixture::setup(size_t nbSockets)
{
    LOG_TEST.debug("Fixture setup - creating " + utils::to_string(nbSockets) + " socket(s)");
    
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
	// wait to process replies
	std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
    std::cout << BWHITE << "=====================END OF TEST=====================" << RESET << '\n';
    if (!_sockets.empty()) {
        LOG_TEST.debug("Fixture cleanup - closing " + utils::to_string(_sockets.size()) + " socket(s)");
        
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
        
        // Give server time to cleanup disconnected clients
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
    }
}
