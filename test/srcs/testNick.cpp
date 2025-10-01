#include "AssertReply.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "TestFixture.hpp"
#include "consts.hpp"
#include "fakeClient.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "signal_handler.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

/************************************************************
 *		✅  VALID											*
 ************************************************************/

/**
 @brief integration test - normal case
*/
void valid_nick_should_void(Server& s)
{
    try {

		TEST_SETUP(test, s, 1);
		TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

		// test
        send_valid_nick_assert(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void valid_nick_special_should_void(Server& s)
{
    try {
		
		TEST_SETUP(test, s, 1);
		TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

		// test
        send_line(so, validNickSpecialMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_empty();
	
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void valid_change_should_void(Server& s)
{
    try {
		TEST_SETUP(test, s, 1);
		TcpSocket& so = *sockets.at(0);
        authenticate(so);

        send_line(so, validNickChangeMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_empty();

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

/**
 @brief integration test - error case
*/
void no_arg_should_err(Server& s)
{

    try {
		TEST_SETUP(test, s, 1);
		TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

        send_line(so, invalidNickMissingArgMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(ERR_NONICKNAMEGIVEN);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void starting_with_number_should_err(Server& s)
{
    try {
		TEST_SETUP(test, s, 1);
		TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

        send_line(so, invalidNick3oroMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(ERR_ERRONEUSNICKNAME);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void taken_should_err(Server& s)
{
    try {
		TEST_SETUP(test, s, 2);
		TcpSocket& so = *sockets.at(0);
		TcpSocket& so2 = *sockets.at(1);
        authenticate(so);

        send_valid_password_assert(so2);
        send_line(so2, validNickMsg);
        std::string reply = recv_lines(so2);
        AssertReply ar(reply);
        ar.has_code(ERR_NICKNAMEINUSE);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_nick(Server& s)
{
    print_test_series("command NICK");
    run_test([&] { valid_nick_should_void(s); }, "roro");
    run_test([&] { valid_nick_special_should_void(s); }, "[roro]");
    run_test([&] { no_arg_should_err(s); }, "no arg");
    run_test([&] { starting_with_number_should_err(s); }, "3oro");
    run_test([&] { taken_should_err(s); }, "taken");
}