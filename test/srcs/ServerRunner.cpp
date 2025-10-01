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
    if (_isRunning) {
        stop();
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
    _thread = std::thread([this]() {
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
    if (!_isRunning)
        return;
        
    LOG_TEST.debug("Stopping server...");
    
    // Signal the server to stop
    globalSignal = SIGINT;
    
    // Wait for server to process the signal and stop
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_STOP_WAIT_MS));
    
    if (_thread.joinable()) {
        _thread.join();
    }
    
    // Reset signal for next test
    globalSignal = 0;
    _isRunning = false;
    
    LOG_TEST.debug("Server stopped");
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

bool ServerRunner::is_running() const { return _isRunning; }

std::string ServerRunner::get_last_error() const { return _lastError; }