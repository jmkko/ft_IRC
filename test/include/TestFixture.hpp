#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "TcpSocket.hpp"
#include <cstddef>
#include <vector>
#include <memory>

class Server;

class TestFixture 
{
	public:
		TestFixture();
		~TestFixture();
		
		std::vector<TcpSocket*>	setup(size_t nbSockets = 1);
		void					cleanup();
		
	private:
		std::vector<std::unique_ptr<TcpSocket>> 	_sockets;
		// Server*										_server;

};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_SETUP(test, nbSockets) \
	TestFixture test; \
	auto sockets = (test).setup(nbSockets)
#endif