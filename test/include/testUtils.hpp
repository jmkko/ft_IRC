#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "printUtils.hpp"
#include "AssertFail.hpp"
#include "TcpSocket.hpp"

#define SERVER_PROCESS_TIME_MS 10
#define TEST_PORT 6668

/**
 * @brief uses std::forward to preserve category of argument
 * by casting it to an rvalue. 
 * Doesnt pass by value. 
 * Allows to pass biggers lambdas, Functors, std::function
 * @tparam Func 
 * @param f 
 * @param name 
 */
template <typename Func>
void run_test(Func&& f, const char* name)
{
	try {
		std::forward<Func>(f)();
		print_success(name);
	} catch (AssertFail& e) {
		print_error(name, e.what());
	}
}

/*
	to remain coherent we keep the same names for valid cases
	- username : 		roro
	- channel name : 	#chan
*/

static const std::string& validPassMsg 					= std::string("PASS ") + DEFAULT_PASSWORD + "\r\n";

static const std::string& validUserMsg 					= "USER roro 0 * :Ronnie Reagan\r\n";
static const std::string& validUserOpMsg 				= "USER op 0 * :Channel_Op\r\n";

static const std::string& validNickMsg 					= "NICK roro\r\n";
static const std::string& validNickOpMsg 				= "NICK op\r\n";
static const std::string& validNickSpecialMsg 			= "NICK [roro]\r\n";
static const std::string& validNickChangeMsg 			= "NICK rorotheboss\r\n";
static const std::string& invalidNickMissingArgMsg 		= "NICK \r\n";
static const std::string& invalidNick3oroMsg 			= "NICK 3oro\r\n";

static const std::string& validJoinMsg 					= "JOIN #chan\r\n";

static const std::string& validKickMsg 					= "KICK #chan roro\r\n";

void send_valid_password_assert(Socket so);
void send_valid_nick_assert(Socket so);
void authenticate(Socket so);
void make_op(Socket so);

#endif