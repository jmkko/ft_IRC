#include "TestFixture.hpp"

#include "LogManager.hpp"
#include "TcpSocket.hpp"
#include "fakeClient.hpp"
#include "testUtils.hpp"

#include <cstddef>

TestFixture::TestFixture(Server& server) : _sockets(), _runner(server), _hasStarted(false) {}

TestFixture::~TestFixture() { cleanup(); }

std::vector<TcpSocket*> TestFixture::setup(size_t nbSockets)
{
    if (!_hasStarted) {
        LOG_TEST.debug("Fixture setup - Starting test server ... ");
        _runner.start();
        _hasStarted = true;
    }
    std::vector<TcpSocket*> socketsPtrs;
    _sockets.reserve(_sockets.size() + nbSockets);
    for (size_t i = 0; i < nbSockets; ++i) {
        _sockets.push_back(make_client_socket(TEST_PORT));
        socketsPtrs.push_back(_sockets.back().get());
    }
    return socketsPtrs;
}

void TestFixture::cleanup()
{
    if (_hasStarted) {
        LOG_TEST.debug("Fixture cleanup - Stopping test server ... ");
        for (auto& socket : _sockets)
            close(socket->get_socket());
        _sockets.clear();
        _runner.stop();
        _hasStarted = false;
    }
}
