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
 *                      ✅  VALID                           *
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

void valid_nick_after_user_should_notice(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, validUserMsg);

        // test
        send_line(so, "NICK roro\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.matches_entirely(":" + s.get_name() + " 001 roro :" + ircCodes.trailing(RPL_WELCOME) + " roro!roro@localhost");
        ar.matches_entirely(":" + s.get_name() + " 002 roro :" + ircCodes.trailing(RPL_YOURHOST) + " " + s.get_name());
        ar.matches_entirely(":" + s.get_name() + " 003 roro :" + ircCodes.trailing(RPL_CREATED));
        ar.matches_entirely(":" + s.get_name() + " 004 roro :" + s.get_name() + " 1.0  0 0");

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
        send_valid_nick_assert(so);

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
 @brief integration test - normal case
*/
void valid_nick_rename_special_should_void(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);
        send_valid_nick_assert(so);

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
/**
 @brief integration test - error case
*/
void valid_change_should_notice(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& so  = *sockets.at(0);
        TcpSocket& so2 = *sockets.at(1);
        make_op(so);
        authenticate_and_join_second_user(so2);
        recv_lines(so);
        send_line(so, validNickChangeMsg);
        std::string reply = recv_lines(so);
        LOG_SERVER.error(reply);
        AssertReply ar(reply);
        ar.is_formatted_transfer("roro", "NICK", "rorotheboss");
        std::string reply2 = recv_lines(so2);
        AssertReply ar2(reply2);
        LOG_TEST.error(reply2);
        ar2.is_empty();

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
        ar.is_formatted(ERR_NONICKNAMEGIVEN, "*");

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
        ar.is_formatted(ERR_ERRONEUSNICKNAME, "*", "3oro");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_char_star_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

        send_line(so, "NICK ro*ro\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_ERRONEUSNICKNAME, "*", "ro*ro");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_char_comma_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

        send_line(so, "NICK ro,ro\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_ERRONEUSNICKNAME, "*", "ro,ro");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_char_arobase_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_valid_password_assert(so);

        send_line(so, "NICK ro@ro\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_ERRONEUSNICKNAME, "*", "ro@ro");

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
        TcpSocket& so  = *sockets.at(0);
        TcpSocket& so2 = *sockets.at(1);
        authenticate(so);

        send_valid_password_assert(so2);
        send_line(so2, validNickMsg);
        std::string reply = recv_lines(so2);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NICKNAMEINUSE, "*", "roro");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_nick(Server& s, t_results* r)
{
    print_test_series("command NICK");
    run_test(r, [&] { valid_nick_should_void(s); }, "roro should void himself");
    run_test(r, [&] { valid_nick_after_user_should_notice(s); }, "roro should notice after USER");
    run_test(r, [&] { valid_nick_special_should_void(s); }, "[roro] should void himself");
    run_test(r, [&] { valid_nick_rename_special_should_void(s); }, "roro->[roro] should void himself");
    run_test(r, [&] { valid_change_should_void(s); }, "roro->\"roro the boss should\" void himself");
    run_test(r, [&] { valid_change_should_notice(s); }, "roro->\"roro the boss\" should tell is new nick to other");
    run_test(r, [&] { no_arg_should_err(s); }, "no arg");
    run_test(r, [&] { invalid_char_star_should_err(s); }, "ro*ro should err");
    run_test(r, [&] { invalid_char_comma_should_err(s); }, "ro,ro should err");
    run_test(r, [&] { invalid_char_arobase_should_err(s); }, "ro@ro should err");
    run_test(r, [&] { starting_with_number_should_err(s); }, "3oro");
    run_test(r, [&] { taken_should_err(s); }, "taken");
}
