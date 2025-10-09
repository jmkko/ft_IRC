#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include "TcpSocket.hpp"
#include <cstddef>
#include <vector>
#include <memory>
#include <iostream>
#include "colors.hpp"
#include "Server.hpp"
#include <cstdlib>

class TestFixture 
{

    friend class Server;

	public:
		TestFixture(Server& s);
		~TestFixture();
		
		std::vector<TcpSocket*>	setup(size_t nbSockets = 1);
		void					cleanup();
		
	private:
		std::vector<std::unique_ptr<TcpSocket>> 	_sockets;
        //NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
        Server&                                     _server;

};

#ifdef DEB
    #define DEBUG_PRINT(test) \
        std::cout << BWHITE << "=====================START OF TEST " << __FUNCTION__ << "=====================" << RESET << '\n';
#else
    #define DEBUG_PRINT(test)
#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_SETUP(test, s, nbSockets) \
	TestFixture test(s); \
	auto sockets = (test).setup(nbSockets); \
    DEBUG_PRINT(test);
#endif