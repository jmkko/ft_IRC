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
	- username1 : 		roro
	- username1 : 		toto
	- channel1 : 		#chan
	- channel2 : 		#chan2
	Messages names are prefixed with valid if they are syntaxically correct and parseable
	Messages names are suffixed with `CommandnameMsg`
*/

static const std::string& userNick						= "roro";
static const std::string& user2Nick						= "toto";
static const std::string& channelName					= "#chan";
static const std::string& channel2Name					= "#chan2";

static const std::string& validPassMsg 					= std::string("PASS ") + DEFAULT_PASSWORD + "\r\n";

static const std::string& validUserMsg 					= "USER roro 0 * :Ronnie Reagan\r\n";
static const std::string& validUser2Msg 				= "USER toto 0 * :Tony Parker\r\n";
static const std::string& validUserOpMsg 				= "USER op 0 * :Channel_Op\r\n";

static const std::string& validNickMsg 					= "NICK roro\r\n";
static const std::string& validNick2Msg 				= "NICK toto\r\n";
static const std::string& validNickOpMsg 				= "NICK op\r\n";
static const std::string& validNickSpecialMsg 			= "NICK [roro]\r\n";
static const std::string& validNickChangeMsg 			= "NICK rorotheboss\r\n";
static const std::string& invalidNickMissingArgMsg 		= "NICK \r\n";
static const std::string& invalidNick3oroMsg 			= "NICK 3oro\r\n";

static const std::string& validJoinMsg 					= "JOIN #chan\r\n";

static const std::string& validKickMsg 					= "KICK #chan roro\r\n";
static const std::string& validManyUsersKickMsg 		= "KICK #chan roro,toto\r\n";
static const std::string& validManyChansUsersKickMsg 	= "KICK #chan,#chan2 roro,toto\r\n";
static const std::string& invalidWrongChanKickMsg 		= "KICK *chan roro\r\n";
static const std::string& invalidNoChanKickMsg 			= "KICK roro\r\n";
static const std::string& invalidNoUserKickMsg 			= "KICK #chan\r\n";
static const std::string& validInexistentKickMsg 		= "KICK #chanel\r\n";

void send_valid_password_assert(Socket so);
void send_valid_nick_assert(Socket so);
void authenticate(Socket so);
void authenticate_second_user(Socket so);
void make_op(Socket so);

#endif