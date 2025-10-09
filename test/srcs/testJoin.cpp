
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
        ar.contains(":" + s.get_name() + " MODE #chan +o");
        ar.contains(":" + s.get_name() + " 331 op #chan :No topic is set");
        ar.contains(":" + s.get_name() + " 353 op = #chan :@op");
        ar.contains(":" + s.get_name() + " 366 op #chan :End of NAMES list");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_join_inv_only_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        send_line(soOp, validModePlusIMsg);
        recv_lines(soOp);
        authenticate(so);
        send_line(so, validJoinMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);

        ar.has_code(ERR_INVITEONLYCHAN);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_join_no_key_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        send_line(soOp, validModePlusKMsg);
        recv_lines(soOp);
        authenticate(so);
        send_line(so, validJoinMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);

        ar.has_code(ERR_BADCHANNELKEY);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_join_wrong_yek_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        send_line(soOp, validModePlusKMsg);
        recv_lines(soOp);
        authenticate(so);
        send_line(so, invalidJoinWithYekMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);

        ar.has_code(ERR_BADCHANNELKEY);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_join_wrong_keyy_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        send_line(soOp, validModePlusKMsg);
        recv_lines(soOp);
        authenticate(so);
        send_line(so, invalidJoinWithKeyyMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);

        ar.has_code(ERR_BADCHANNELKEY);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void valid_join_inv_only_should_notice(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        do_cmd(soOp, validModePlusIMsg);
        authenticate(so);
        do_cmd(soOp, validInvite);

        // test
        send_line(so, validJoinMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(RPL_NAMREPLY);

        std::string identity = userNick + "!" + userNick + "@" + s.get_name();
        ar.contains(":" + identity + " JOIN :#chan");
        ar.contains(":" + s.get_name() + " 331 " + userNick + " #chan :No topic is set");
        ar.contains(":" + s.get_name() + " 366 roro #chan :End of NAMES list");
        std::string reply2 = recv_lines(soOp);
        AssertReply ar2(reply2);
        ar2.contains(":" + identity + " JOIN :#chan");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void valid_join_key_should_notice(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        do_cmd(soOp, validModePlusKMsg);
        authenticate(so);

        // test
        send_line(so, validJoinWithKeyMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(RPL_NAMREPLY);
        std::string identity = userNick + "!" + userNick + "@" + s.get_name();
        ar.contains(":" + identity + " JOIN :#chan");
        ar.contains(":" + s.get_name() + " 331 " + userNick + " #chan :No topic is set");
        ar.contains(":" + s.get_name() + " 353 " + userNick + " = #chan :@op roro");
        ar.contains(":" + s.get_name() + " 366 roro #chan :End of NAMES list");
        std::string reply2 = recv_lines(soOp);
        AssertReply ar2(reply2);
        ar2.contains(":" + identity + " JOIN :#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_join_limit1_should_err(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        do_cmd(soOp, validModePlusLMsg);
        authenticate(so);

        // test
        send_line(so, validJoinMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(ERR_CHANNELISFULL);

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
    run_test([&] { valid_join_should_notice(s); }, "JOIN #chan (op user)");
    run_test([&] { invalid_join_inv_only_should_err(s); }, "A user try to join invite only chan");
    run_test([&] { invalid_join_no_key_should_err(s); }, "A user try to join with no key");
    run_test([&] { invalid_join_wrong_yek_should_err(s); }, "A user try to join with wrong yek");
    run_test([&] { invalid_join_wrong_keyy_should_err(s); }, "A user try to join with wrong keyy");
    run_test([&] { valid_join_inv_only_should_notice(s); }, "Joining A channel invite only after being invited.");
    run_test([&] { valid_join_key_should_notice(s); }, "Joining A channel with a valid key");
    run_test([&] { invalid_join_limit1_should_err(s); }, "Joining A channel wich is full");
}
