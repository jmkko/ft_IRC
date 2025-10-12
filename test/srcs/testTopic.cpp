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

void no_params_should_err_topic(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, noparamsTopic);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "TOPIC");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_channel_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, invalidChannelTopic);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHCHANNEL, opNick, "$notvalid");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void not_a_channel_member_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate(so);

        // test
        send_line(so, notAChannelMemberTopic);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTONCHANNEL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_topic(Server& s)
{
    print_test_series("command TOPIC");
    run_test([&] { no_params_should_err_topic(s); }, "TOPIC with no params");
    run_test([&] { invalid_channel_should_err(s); }, "TOPIC on non existant channel");
    run_test([&] { not_a_channel_member_should_err(s); }, "TOPIC user is not a channel member");
}
