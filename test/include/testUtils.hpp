#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "AssertFail.hpp"
#include "TcpSocket.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"

#define SERVER_PROCESS_TIME_MS 10
#define SERVER_SEND_WAIT_MS    15
#define SERVER_START_WAIT_MS   50
#define SERVER_STOP_WAIT_MS    20
#define TEST_PORT              4343

class Server;
typedef struct Sresults
{
    int launchedTests;
    int passedTests;
    int failedTests;
}   t_results;

/**
 * @brief uses std::forward to preserve category of argument
 * by casting it to an rvalue.
 * Doesnt pass by value.
 * Allows to pass biggers lambdas, Functors, std::function
 * @tparam Func
 * @param f
 * @param name
 */
template <typename Func> void run_test(t_results* results, Func&& f, const char* name)
{
    try {
        results->launchedTests++;
        std::forward<Func>(f)();
        print_success(name);
        results->passedTests++;
    } catch (AssertFail& e) {
        print_error(name, e.what());
        results->failedTests++;
    }
}

/*
    to remain coherent we keep the same names for valid cases
    - username1 : 		roro
    - username2 : 		toto
    - username3         riri
    - username4         titi
    - username5         rara
    - opname1			op
    - opname2			op2
    - channel1 : 		#chan
    - channel2 : 		#chan2
    username will be same as <nick>
    Messages names are prefixed with valid if they are syntaxically correct and parseable
    Messages names are suffixed with `CommandnameMsg`
*/

static const std::string& userNick     = "roro";
static const std::string& user2Nick    = "toto";
static const std::string& user3Nick    = "riri";
static const std::string& user4Nick    = "titi";
static const std::string& user5Nick    = "rara";
static const std::string& user6Nick    = "tata";
static const std::string& opNick   = "op";
static const std::string& op2Nick  = "op2";
static const std::string& channelName  = "#chan";
static const std::string& channel2Name = "#chan2";

static const std::string& validPassMsg = std::string("PASS ") + DEFAULT_PASSWORD + "\r\n";

static const std::string& validUserMsg             = "USER roro 0 * :realroro\r\n";
static const std::string& validUser2Msg            = "USER toto 0 * :realtoto\r\n";
static const std::string& validUserOpMsg           = "USER op 0 * :realop\r\n";
static const std::string& validUserOp2Msg          = "USER op2 0 * :realop2\r\n";
static const std::string& invalidUserNoUsernameMsg = "USER 0 * :realop2\r\n";

static const std::string& validNickMsg             = "NICK roro\r\n";
static const std::string& validNick2Msg            = "NICK toto\r\n";
static const std::string& validNickOpMsg           = "NICK op\r\n";
static const std::string& validNickOp2Msg          = "NICK op2\r\n";
static const std::string& validNickSpecialMsg      = "NICK [roro]\r\n";
static const std::string& validNickChangeMsg       = "NICK rorotheboss\r\n";
static const std::string& invalidNickMissingArgMsg = "NICK \r\n";
static const std::string& invalidNick3oroMsg       = "NICK 3oro\r\n";

static const std::string& validJoinMsg           = "JOIN #chan\r\n";
static const std::string& validJoinWithKeyMsg    = "JOIN #chan key\r\n";
static const std::string& invalidJoinWithYekMsg  = "JOIN #chan yek\r\n";
static const std::string& invalidJoinWithKeyyMsg = "JOIN #chan keyy\r\n";
static const std::string& noparamsJoin           = "JOIN\r\n";
static const std::string& noSpecCharJoin         = "JOIN chan\r\n";
static const std::string& toobigJoin             = "JOIN #chanllllllllllllllllllllllllllllllllllllllllllllllllll\r\n";

static const std::string& validKickMsg                  = "KICK #chan roro\r\n";
static const std::string& validManyUsersKickMsg         = "KICK #chan roro,toto\r\n";
static const std::string& validManyChansUsersKickMsg    = "KICK #chan,#chan2 roro,toto\r\n";
static const std::string& invalidWrongChanKickMsg       = "KICK *chan roro\r\n";
static const std::string& invalidNoChanKickMsg          = "KICK roro\r\n";
static const std::string& invalidNoUserKickMsg          = "KICK #chan\r\n";
static const std::string& validInexistentChannelKickMsg = "KICK #chanel roro\r\n";

static const std::string& validModeMsg                = "MODE #chan\r\n";
static const std::string& validModePlusKMsg           = "MODE #chan +k key\r\n";
static const std::string& validModeMinusKMsg          = "MODE #chan -k key\r\n";
static const std::string& invalidModePlusKNoArgMsg    = "MODE #chan +k\r\n";
static const std::string& validModePlusIMsg           = "MODE #chan +i\r\n";
static const std::string& validModeMinusIMsg          = "MODE #chan -i\r\n";
static const std::string& validModePlusLMsg           = "MODE #chan +l 1\r\n";
static const std::string& validModePlusLZeroMsg       = "MODE #chan +l 0\r\n";
static const std::string& validModeMinusLMsg          = "MODE #chan -l\r\n";
static const std::string& invalidModePlusLNegativeMsg = "MODE #chan +l -1\r\n";
static const std::string& invalidModePlusLNoArgMsg    = "MODE #chan +l\r\n";
static const std::string& validModePlusOMsg           = "MODE #chan +o op2\r\n";
static const std::string& validModeMinusOMsg          = "MODE #chan -o op2\r\n";
static const std::string& invalidModePlusONoArgMsg    = "MODE #chan +o\r\n";
static const std::string& validModePlusTMsg           = "MODE #chan +t\r\n";
static const std::string& validModeMinusTMsg          = "MODE #chan -t\r\n";
static const std::string& validModePlusKLMsg          = "MODE #chan +kl key 10\r\n";
static const std::string& invalidModeUnknownModeMsg   = "MODE #chan +z\r\n";
static const std::string& invalidModeUnknownChanMsg   = "MODE #ch +i\r\n";
static const std::string& validModeUnknownUserMsg     = "MODE #chan +o pouic\r\n";
static const std::string& invalidModeMinusONoUserMsg  = "MODE #chan -o\r\n";
static const std::string& invalidModeNoChanMsg        = "MODE +l 4\r\n";

static const std::string& validPartMsg = "PART #chan\r\n";

static const std::string& validTopicMsg  = "TOPIC #chan :New topic\r\n";
static const std::string& validPingMsg   = "PING token\r\n";
static const std::string& invalidPingMsg = "PING\r\n";

static const std::string& validQuitMsg = "QUIT\r\n";

static const std::string& noparamsWho    = "WHO \r\n";
static const std::string& chan1Who       = "WHO #chan\r\n";
static const std::string& chan1OpWho     = "WHO #chan o\r\n";
static const std::string& goodPatternWho = "WHO ro*\r\n";
static const std::string& badUserWho     = "WHO resu\r\n";
static const std::string& badPatternWho  = "WHO *x*\r\n";
static const std::string& allUserWho     = "WHO *\r\n";

static const std::string& noparamsPrivmsg        = "PRIVMSG\r\n";
static const std::string& invalidnicknamePrivmsg = "PRIVMSG nonexistent :message\r\n";
static const std::string& toomanytargetPrivmsg   = "PRIVMSG roro,toto,riri,titi,rara,tata :message\r\n";
static const std::string& notextPrivmsg          = "PRIVMSG #chan\r\n";

static const std::string& noparamsTopic				= "TOPIC\r\n";
static const std::string& invalidChannelTopic		= "TOPIC $notvalid\r\n";
static const std::string& notAChannelMemberTopic	= "TOPIC #chan\r\n";

static const std::string& validInvite = "INVITE roro #chan\r\n";

// with assertions
void send_valid_password_assert(const TcpSocket& so);
void send_valid_nick_assert(const TcpSocket& so);
void join_assert(const TcpSocket& so);

// with receive
void send_pass_nick(const TcpSocket& so);
void authenticate(const TcpSocket& so);
void authenticate(const TcpSocket& so, const std::string& nick);
void authenticate_and_join(const TcpSocket& so, const std::string& nick, const std::string& chan);
void authenticate_and_join(const TcpSocket& so);
void authenticate_and_join_second_user(const TcpSocket& so);
void authenticate_and_join_op2(const TcpSocket& so);
void authenticate_second_user(const TcpSocket& so);
void make_op(const TcpSocket& so);
void make_two_ops(const TcpSocket& so, const TcpSocket& so2);
void skip_lines(const TcpSocket& so, int nb);

void test_join(Server& s, t_results* r);
void test_kick(Server& s, t_results* r);
void test_mode(Server& s, t_results* r);
void test_nick(Server& s, t_results* r);
void test_who(Server& s, t_results* r);
void test_privmsg(Server& s, t_results* r);
void test_ping(Server& s, t_results* r);
void test_topic(Server& s, t_results* r);

#endif
