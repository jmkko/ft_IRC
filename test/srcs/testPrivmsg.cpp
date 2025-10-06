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

void no_params_should_err()
{
    try {
        TEST_SETUP(test, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, noparamsPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_NEEDMOREPARAMS);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_nickname_should_err()
{
    try {
        TEST_SETUP(test, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, invalidnicknamePrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_NOSUCHNICK);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void too_many_target_should_notice()
{
    try {
        TEST_SETUP(test, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, toomanytargetPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_TOOMANYTARGETS);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void no_text_should_err()
{
    try {
        TEST_SETUP(test, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        send_line(soOp, notextPrivmsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
		ar.has_code(ERR_NOTEXTTOSEND);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_privmsg()
{
    print_test_series("command PRIMSG");
    run_test([&] { no_params_should_err(); }, "PRIVMSG with no params");
    run_test([&] { invalid_nickname_should_err(); }, "PRIVMSG to non existant nickname");
    run_test([&] { too_many_target_should_notice(); }, "PRIVMSG with too many target");
    run_test([&] { no_text_should_err(); }, "PRIVMSG with no message");
}
