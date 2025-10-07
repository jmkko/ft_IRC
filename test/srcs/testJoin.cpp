
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

void noparams_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, noparamsJoin);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_NEEDMOREPARAMS);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void no_spec_char_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, noSpecCharJoin);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_BADCHANMASK);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void toobigname_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, toobigJoin);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.has_code(ERR_BADCHANMASK);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void valid_join_should_notice(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, validJoinMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);

        // LOG_TEST.error(reply);
        ar.has_code(RPL_NAMREPLY).contains(channelName).contains(RPL_NOTOPIC_MSG).contains(userOp).has_code(RPL_NOTOPIC);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_join(Server& s)
{
    print_test_series("command JOIN");
    run_test([&] { noparams_should_err(s); }, "JOIN with no params");
    run_test([&] { no_spec_char_should_err(s); }, "JOIN chan");
    run_test([&] { toobigname_should_err(s); }, "JOIN more 50 char channel name");
    run_test([&] { valid_join_should_notice(s); }, "JOIN #chan (op user)"); // only check the last code endof names
}
