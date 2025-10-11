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

/*************************************************************
 *                    ✅  VALID                              *
 ************************************************************/

void noparams_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, noparamsWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.contains(user2Nick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(userNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void chan1_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, chan1Who);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.contains(userNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(userNick).contains(channelName);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void chan1op_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, chan1OpWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.contains(opNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(userNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void good_pattern_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, goodPatternWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.contains(userNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(opNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void bad_user_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, badUserWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.do_not_contains(userNick).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(opNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void bad_pattern_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, badPatternWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.do_not_contains(userNick).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(opNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void all_user_should_notice(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        // test
        send_line(soOp, allUserWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.contains(userNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).contains(opNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_who(Server& s)
{
    print_test_series("command WHO");

    run_test([&] { noparams_should_notice(s); }, "No params WHO");
    run_test([&] { chan1_should_notice(s); }, "WHO #chan1");
    run_test([&] { chan1op_should_notice(s); }, "WHO #chan1 o");
    run_test([&] { good_pattern_should_notice(s); }, "WHO ro*");
    run_test([&] { bad_user_should_notice(s); }, "WHO resu");
    run_test([&] { bad_pattern_should_notice(s); }, "WHO *x*");
    run_test([&] { all_user_should_notice(s); }, "WHO *");
}
