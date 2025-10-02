#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "ServerRunner.hpp"
#include "TcpSocket.hpp"
#include <cstddef>
class Server;

class TestFixture 
{
	public:
		TestFixture(Server& server);
		~TestFixture();
		
		std::vector<TcpSocket*>	setup(size_t nbSockets = 1);
		void					cleanup();
		
	private:
		std::vector<std::unique_ptr<TcpSocket>> 	_sockets;
		ServerRunner								_runner;
		bool										_hasStarted;

};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_SETUP(test, s, nbSockets) \
	TestFixture test(s); \
	auto sockets = (test).setup(nbSockets)

#endif