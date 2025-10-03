#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "printUtils.hpp"
#include "AssertFail.hpp"
#include "TcpSocket.hpp"

#define SERVER_PROCESS_TIME_MS 10
#define SERVER_SEND_WAIT_MS 50
#define SERVER_START_WAIT_MS 1000
#define SERVER_STOP_WAIT_MS 2000
#define TEST_PORT 4343

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
	- opname1			op
	- opname2			op2
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
static const std::string& validUserOp2Msg 				= "USER op2 0 * :Channel_Op2\r\n";

static const std::string& validNickMsg 					= "NICK roro\r\n";
static const std::string& validNick2Msg 				= "NICK toto\r\n";
static const std::string& validNickOpMsg 				= "NICK op\r\n";
static const std::string& validNickOp2Msg 				= "NICK op2\r\n";
static const std::string& validNickSpecialMsg 			= "NICK [roro]\r\n";
static const std::string& validNickChangeMsg 			= "NICK rorotheboss\r\n";
static const std::string& invalidNickMissingArgMsg 		= "NICK \r\n";
static const std::string& invalidNick3oroMsg 			= "NICK 3oro\r\n";

static const std::string& validJoinMsg 					= "JOIN #chan\r\n";
static const std::string& validJoinWithKeyMsg 			= "JOIN #chan key\r\n";

static const std::string& validKickMsg 					= "KICK #chan roro\r\n";
static const std::string& validManyUsersKickMsg 		= "KICK #chan roro,toto\r\n";
static const std::string& validManyChansUsersKickMsg 	= "KICK #chan,#chan2 roro,toto\r\n";
static const std::string& invalidWrongChanKickMsg 		= "KICK *chan roro\r\n";
static const std::string& invalidNoChanKickMsg 			= "KICK roro\r\n";
static const std::string& invalidNoUserKickMsg 			= "KICK #chan\r\n";
static const std::string& validInexistentChannelKickMsg = "KICK #chanel roro\r\n";

static const std::string& validModePlusKMsg 			= "MODE #chan +k key\r\n";
static const std::string& validModeMinusKMsg 			= "MODE #chan -k key\r\n";
static const std::string& invalidModePlusKNoArgMsg 		= "MODE #chan +k\r\n";
static const std::string& validModePlusIMsg 			= "MODE #chan +i\r\n";
static const std::string& validModeMinusIMsg 			= "MODE #chan -i\r\n";
static const std::string& validModePlusLMsg 			= "MODE #chan +l 1\r\n";
static const std::string& validModePlusLZeroMsg 		= "MODE #chan +l 0\r\n";
static const std::string& validModeMinusLMsg 			= "MODE #chan -l\r\n";
static const std::string& invalidModePlusLNegativeMsg 	= "MODE #chan +l -1\r\n";
static const std::string& invalidModePlusLNoArgMsg 		= "MODE #chan +l\r\n";
static const std::string& validModePlusOMsg 			= "MODE #chan +o op2\r\n";
static const std::string& validModeMinusOMsg 			= "MODE #chan -o op2\r\n";
static const std::string& validModePlusOWildcardMsg 	= "MODE #chan -o *2\r\n";
static const std::string& invalidModePlusONoArgMsg 		= "MODE #chan +o\r\n";
static const std::string& validModePlusTMsg 			= "MODE #chan +t\r\n";
static const std::string& validModeMinusTMsg 			= "MODE #chan -t\r\n";
static const std::string& validModePlusKLMsg 			= "MODE #chan +kl key 10\r\n";
static const std::string& invalidModeNoModeMsg 			= "MODE #chan\r\n";
static const std::string& invalidModeUnknownModeMsg 	= "MODE #chan +z\r\n";
static const std::string& invalidModeUnknownUserMsg 	= "MODE #chan +o pouic\r\n";
static const std::string& invalidModeNoChanMsg 			= "MODE +l 4\r\n";

static const std::string& validPartMsg 					= "PART #chan\r\n";

static const std::string& validTopicMsg 				= "TOPIC #chan :New topic\r\n";

// with assertions
void send_valid_password_assert(const TcpSocket& so);
void send_valid_nick_assert(const TcpSocket& so);
void join_assert(const TcpSocket& so);

// with receive
void authenticate(const TcpSocket& so);
void authenticate_and_join(const TcpSocket& so);
void authenticate_and_join_second_user(const TcpSocket& so);
void authenticate_and_join_op2(const TcpSocket& so);
void authenticate_second_user(const TcpSocket& so);
void make_op(const TcpSocket& so);
void make_two_ops(const TcpSocket& so, const TcpSocket& so2);

// simple

#endif