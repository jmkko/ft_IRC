#include "ServerRunner.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "consts.hpp"
#include "signal_handler.hpp"
#include "testUtils.hpp"

#include <exception>
#include <stdexcept>
#include <thread>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

ServerRunner::ServerRunner(Server& s) : _server(s), _isRunning(false) {}

ServerRunner::~ServerRunner()
{
    if (_thread.joinable()) {
        try {
            stop();
        } catch (std::exception e) {
            LOG_W_TEST("Runner", e.what());
        }
    }
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void ServerRunner::start()
{
    // Reset global signal
    // globalSignal = 0;

    if (_thread.joinable()) {
        throw std::runtime_error("Server already running");
    }

    _isRunning = true;
    _thread    = std::thread([this]() {
        try {
            _server.start();
        } catch (const std::exception& e) {
            _lastError = e.what();
            LOG_TEST.error("Server thread error: " + _lastError);
        }
        _isRunning = false;
    });

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_START_WAIT_MS));
}

void ServerRunner::stop()
{
    LOG_d_TEST("Stopping server...");

    _server.stop();

    // Wait for server to process the signal and stop
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_STOP_WAIT_MS));

    if (_thread.joinable()) {
        LOG_d_TEST("joining server");
        _thread.join();
        LOG_d_TEST("joined server");
    }
    
    _isRunning   = false;

    LOG_TEST.debug("Server stopped");
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

bool ServerRunner::is_running() const { return _isRunning; }

std::string ServerRunner::get_last_error() const { return _lastError; }