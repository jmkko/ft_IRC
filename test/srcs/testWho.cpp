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

/**
 * @brief ensure params respect syntax
 * <channel> <username> <host> <server> <nick> <flags>"
 * channel 'is an arbitrary channel the client is joined to or a literal asterisk character ('*', 0x2A) if no channel is returned'
 * @return std::string
 */
static std::string build_who_params(const std::string& foundNick, const std::string& chan, const std::string& flag)
{
    return chan + " " + foundNick + " localhost " + ircConfigTest.get_name() + " " + foundNick + " " + flag;
}

/*************************************************************
 *                    ✅  VALID                              *
 ************************************************************/

void noparams_should_send_rpl(Server& s)
{
    try {
        // init test
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);

        // test - one RPL_WHOREPLY is sent for each user
        // sharing channel with sender (except sender him/herself) - but this is up to server's own implementation)
        send_line(soOp, noparamsWho);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(RPL_WHOREPLY, opNick, build_who_params(userNick, "*", "H"), "0 realroro");
        ar.is_formatted(RPL_ENDOFWHO, opNick, "*");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void chan1_should_send_rpl(Server& s)
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
        ar.is_formatted(RPL_WHOREPLY, opNick, build_who_params(userNick, "#chan", "H"), "0 realroro");
        ar.is_formatted(RPL_ENDOFWHO, opNick, "#chan");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void chan1op_should_send_rpl_with_flag(Server& s)
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
        ar.is_formatted(RPL_WHOREPLY, opNick, build_who_params(opNick, "#chan", "H@"), "0 realop");
        ar.is_formatted(RPL_ENDOFWHO, opNick, "#chan");
        // ar.contains(opNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(userNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void good_pattern_should_send_rpl(Server& s)
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
        ar.is_formatted(RPL_WHOREPLY, opNick, build_who_params(userNick, "*", "H"), "0 realroro");
        ar.is_formatted(RPL_ENDOFWHO, opNick, "ro*");

        // ar.contains(userNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(opNick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void bad_user_should_send_only_rplend(Server& s)
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
        ar.is_formatted(RPL_ENDOFWHO, opNick, "resu");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void bad_pattern_should_send_only_rplend(Server& s)
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
        ar.is_formatted(RPL_ENDOFWHO, opNick, "*x*");
        // ar.do_not_contains(userNick).has_code(RPL_ENDOFWHO).contains(channelName).do_not_contains(opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void pattern_wildcard_should_send_rpl(Server& s)
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
        ar.is_formatted(RPL_WHOREPLY, opNick, build_who_params(userNick, "*", "H"), "0 realroro");
        ar.is_formatted(RPL_ENDOFWHO, opNick, "*");
        // ar.contains(userNick).has_code(RPL_WHOREPLY).has_code(RPL_ENDOFWHO).contains(channelName).contains(opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_who(Server& s, t_results* r)
{
    print_test_series("command WHO");

    run_test(r, [&] { noparams_should_send_rpl(s); }, "No params WHO");
    run_test(r, [&] { chan1_should_send_rpl(s); }, "WHO #chan1");
    run_test(r, [&] { chan1op_should_send_rpl_with_flag(s); }, "WHO #chan1 o");
    run_test(r, [&] { good_pattern_should_send_rpl(s); }, "WHO ro*");
    run_test(r, [&] { bad_user_should_send_only_rplend(s); }, "WHO resu");
    run_test(r, [&] { bad_pattern_should_send_only_rplend(s); }, "WHO *x*");
    run_test(r, [&] { pattern_wildcard_should_send_rpl(s); }, "WHO *");
}
