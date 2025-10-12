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

void no_params_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, noparamsPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "PRIVMSG");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_nickname_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, invalidnicknamePrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHNICK, opNick, "nonexistent");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void too_many_target_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 7);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so1  = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        TcpSocket& so3  = *sockets.at(3);
        TcpSocket& so4  = *sockets.at(4);
        TcpSocket& so5  = *sockets.at(5); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        TcpSocket& so6  = *sockets.at(6); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        make_op(soOp);
        authenticate(so1, userNick);
        authenticate(so2, user2Nick);
        authenticate(so3, user3Nick);
        authenticate(so4, user4Nick);
        authenticate(so5, user5Nick);
        authenticate(so6, user6Nick);

        // test - message should mention only the last offending target
        send_line(soOp, toomanytargetPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_TOOMANYTARGETS, opNick, "tata");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void no_text_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        send_line(soOp, notextPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTEXTTOSEND, opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_privmsg(Server& s, t_results* r)
{
    print_test_series("command PRIMSG");
    run_test(r, [&] { no_params_should_err(s); }, "PRIVMSG with no params");
    run_test(r, [&] { invalid_nickname_should_err(s); }, "PRIVMSG to non existent nickname");
    run_test(r, [&] { too_many_target_should_err(s); }, "PRIVMSG with too many target");
    run_test(r, [&] { no_text_should_err(s); }, "PRIVMSG with no message");
}
