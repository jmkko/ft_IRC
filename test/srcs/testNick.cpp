#include "AssertReply.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
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
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);
    LOG_TEST.debug(std::string("so is ") + std::to_string(so));

    if (so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        send_valid_password_assert(so);
        send_valid_nick_assert(so);
        close(so);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
    }
    runner.stop();
}

/**
 @brief integration test - normal case
*/
void valid_nick_special_should_void(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);
    LOG_TEST.debug(std::string("so is ") + std::to_string(so));

    if (so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        send_valid_password_assert(so);
        send_line(so, validNickSpecialMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_empty();

        close(so);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void valid_change_should_void(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);

    if (so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        authenticate(so);

        send_line(so, validNickChangeMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_empty();

        close(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
    }
    runner.stop();
}

/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

/**
 @brief integration test - error case
*/
void no_arg_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);

    if (so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        send_valid_password_assert(so);

        send_line(so, invalidNickMissingArgMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(ERR_NONICKNAMEGIVEN);

        close(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void starting_with_number_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);

    if (so == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        send_valid_password_assert(so);

        send_line(so, invalidNick3oroMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(ERR_ERRONEUSNICKNAME);

        close(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
    }
    runner.stop();
}

/**
 @brief integration test - error case
*/
void taken_should_err(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket so = -1;
    so        = make_client_socket(TEST_PORT);

    Socket so2 = -1;
    so2        = make_client_socket(TEST_PORT);

    if (so == -1 || so2 == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        authenticate(so);

        send_valid_password_assert(so2);
        send_line(so2, validNickMsg);
        std::string reply = recv_lines(so2);
        AssertReply ar(reply);
        ar.has_code(ERR_NICKNAMEINUSE);

        close(so);
        close(so2);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(so);
        close(so2);
    }
    runner.stop();
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