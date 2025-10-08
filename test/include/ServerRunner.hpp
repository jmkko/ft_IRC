#ifndef SERVERRUNNER_HPP
#define SERVERRUNNER_HPP

#include "Server.hpp"
#include <string>
#include <thread>
#include <atomic>

class ServerRunner {
	public:
		ServerRunner(Server& s);
		virtual ~ServerRunner();

		void				start();
		void				stop();
		bool 				is_running() const;
		std::string			get_last_error() const;

	private:
		Server&						_server;
		std::thread 				_thread;
		std::atomic<bool>			_isRunning;
		std::string					_lastError;
};

#endif