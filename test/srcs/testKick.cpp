#include "AssertReply.hpp"
#include "LogManager.hpp"
#include "consts.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "fakeClient.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "testUtils.hpp"
#include "utils.hpp"
#include "ServerRunner.hpp"
#include "signal_handler.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>


/************************************************************
*		✅  VALID											*
************************************************************/

/**
 @brief integration test - normal case
*/
void op_existing_chan_valid_user_should_notice(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket soOp = -1;
    soOp        = make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        make_op(soOp);
		authenticate(so);
		send_line(so, validJoinMsg);
		recv_lines(so);

		send_line(soOp, validKickMsg);
		// as a member, operator receives the notice
		std::string reply = recv_lines(soOp);
		LOG_TEST.debug("test_valid1: reply op", reply);
        AssertReply ar(reply);
		// ar.contains("KICK").contains(channelName).contains(userNick);

		// kicked user gets a notice
		reply = recv_lines(so);
        ar.handle_new_reply(reply);
		LOG_TEST.debug("test_valid1: reply user", reply);
		ar.contains("KICK").contains(channelName).contains(userNick);

		// kicked user can join again
		send_line(so, validJoinMsg);
		reply = recv_lines(so);
		ar.handle_new_reply(reply);
		ar.has_code(RPL_NOTOPIC);

        close(soOp);
        close(so);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(soOp);
        close(so);
    }
    runner.stop();
}

/**
 @brief integration test - normal case - many users
*/
void op_existing_chan_valid_users_should_notice(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket soOp = -1;
    soOp        = make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);
	Socket so2	= -1;
	so2			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1 || so2 == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        make_op(soOp);

		authenticate(so);
		send_line(so, validJoinMsg);
		recv_lines(so);
		
		authenticate_second_user(so2);
		send_line(so2, validJoinMsg);
		recv_lines(so2);

		send_line(soOp, validManyUsersKickMsg);
		// as a member, operator receives the notice
		std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
		// ar.contains("KICK").contains(channelName).contains(userNick);

		// kicked user1 gets a notice
		reply = recv_lines(so);
        ar.handle_new_reply(reply);
		ar.contains("KICK").contains(channelName).contains(userNick);

		// kicked user2 gets a notice
		reply = recv_lines(so);
        ar.handle_new_reply(reply);
		ar.contains("KICK").contains(channelName).contains(user2Nick);

        close(soOp);
        close(so);
        close(so2);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(soOp);
        close(so);
        close(so2);
    }
    runner.stop();
}

/************************************************************
*		❌ ERRORS											*
************************************************************/

/**
 @brief integration test - error case
*/
void no_op_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);
    Socket so2	= -1;
    so2       	= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1 || so2 == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		authenticate_second_user(so2);

		send_line(so, validJoinMsg);
		send_line(so2, validJoinMsg);
		send_line(so2, validKickMsg);
		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_CHANOPRIVSNEEDED);

        close(so);
        close(so2);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(so2);
        close(soOp);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void op_missing_chan_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		send_line(so, validJoinMsg);
		send_line(soOp, invalidNoChanKickMsg);

		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_NEEDMOREPARAMS);

        close(so);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(soOp);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void op_missing_user_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		send_line(so, validJoinMsg);
		send_line(soOp, invalidNoUserKickMsg);

		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_NEEDMOREPARAMS);

        close(so);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(soOp);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void op_user_not_in_channel_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		send_line(soOp, validKickMsg);

		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_USERNOTINCHANNEL);

        close(so);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(soOp);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void op_invalid_channel_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		send_line(so, validJoinMsg);
		send_line(soOp, invalidWrongChanKickMsg);

		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_BADCHANMASK);

        close(so);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(soOp);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void op_valid_inexistent_channel_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

	Socket soOp	= -1;
	soOp		= make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1 || so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
		make_op(soOp);
		authenticate(so);
		send_line(so, validJoinMsg);
		send_line(soOp, validInexistentKickMsg);

		std::string reply = recv_lines(so);
        AssertReply ar(reply);
		ar.has_code(ERR_NOSUCHCHANNEL);

        close(so);
        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(soOp);
    }
    runner.stop();
}

void	test_kick(Server& s)
{
	print_test_series("command KICK");
	run_test([&] {op_existing_chan_valid_user_should_notice(s);}, "single kick");
	run_test([&] {op_existing_chan_valid_users_should_notice(s);}, "combo");
	run_test([&] {no_op_should_err(s);}, "no op");
	run_test([&] {op_missing_chan_should_err(s);}, "no chan");
	run_test([&] {op_missing_user_should_err(s);}, "no user");
	run_test([&] {op_user_not_in_channel_should_err(s);}, "not in chan");
	run_test([&] {op_invalid_channel_should_err(s);}, "invalid chan");
	run_test([&] {op_valid_inexistent_channel_should_err(s);}, "inexisting chan");
}