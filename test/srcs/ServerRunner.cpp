#include "ServerRunner.hpp"

#include "LogManager.hpp"
#include "signal_handler.hpp"
#include "testUtils.hpp"

#include <exception>
#include <stdexcept>
#include <thread>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

ServerRunner::ServerRunner(Server& server) : _server(server), _isRunning(false) {}

ServerRunner::~ServerRunner()
{
    if (_thread.joinable()) {
        _server.stop();
        _thread.join();
    }
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void ServerRunner::start()
{
    // Reset global signal
    globalSignal = 0;

    if (_isRunning)
        throw std::runtime_error("Server already running");
    _isRunning = true;
    _thread    = std::thread([this]() {
        try {
            _server.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS * 4));
        } catch (const std::exception& e) {
            _lastError = e.what();
            LOG_TEST.error(_lastError);
        }
        _isRunning = false;
    });
}

void ServerRunner::stop()
{
    if (!_isRunning)
        return;
    _server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS * 4));
    LOG_TEST.debug("call stop for server");
    if (_thread.joinable()) {
        _thread.join();
    }
    _isRunning = false;
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

bool ServerRunner::is_running() const { return _isRunning; }

std::string ServerRunner::get_last_error() const { return _lastError; }