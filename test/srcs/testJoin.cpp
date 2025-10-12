
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
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "JOIN");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void name_no_prefix_should_err(Server& s)
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
        ar.is_formatted(ERR_BADCHANMASK, opNick, "chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void name_too_big_should_err(Server& s)
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
        ar.is_formatted(ERR_BADCHANMASK, opNick, "#chanllllllllllllllllllllllllllllllllllllllllllllllllll");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void valid_join_should_send_rpl_and_broadcast(Server& s)
{
    try {
        // init test_join
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);
        TcpSocket& so = *sockets.at(1);
        authenticate(so);

        // test 1 new member receives messages
        send_line(so, validJoinMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted_transfer(userNick, "JOIN #chan", "");
        ar.is_formatted(RPL_NOTOPIC, userNick, "#chan");
        ar.is_formatted(RPL_NAMREPLY, userNick, "#chan", "@" + opNick + " " + userNick);
        ar.is_formatted(RPL_ENDOFNAMES, userNick, "#chan");

        // test 2
        reply = recv_lines(soOp);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(userNick, "JOIN #chan", "");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusk_no_key_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModePlusKMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +k key");

        // test 2
        send_line(so, validJoinMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_BADCHANNELKEY, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void mode_plusk_wrong_yek_should_err(Server& s)
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

        // test
        send_line(so, invalidJoinWithYekMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_BADCHANNELKEY, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void mode_plusk_wrong_keyy_should_err(Server& s)
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

        // test
        send_line(so, invalidJoinWithKeyyMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_BADCHANNELKEY, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void mode_plusi_with_invite_should_send_rpl_and_broadcast(Server& s)
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
        recv_lines(so);

        // test 1 new member gets replies
        send_line(so, validJoinMsg);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(RPL_NOTOPIC, userNick, "#chan");
        ar.is_formatted(RPL_NAMREPLY, userNick, "#chan", "@" + opNick + " " + userNick);
        ar.is_formatted(RPL_ENDOFNAMES, userNick, "#chan");

        // test 2 other members get broadcast message
        reply = recv_lines(soOp);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(userNick, "JOIN #chan", "");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusi_no_invite_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModePlusIMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +i");

        // test 2
        send_line(so, validJoinMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_INVITEONLYCHAN, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusl_should_block_join_if_max_reached(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModePlusLMsg); // 1
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +l 1");

        // test 2
        send_line(so, validJoinMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_CHANNELISFULL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void mode_plusl_zeroarg_should_block_join(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModePlusLZeroMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +l 0");

        // test 2
        send_line(so, validJoinMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_CHANNELISFULL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_minusk_should_lift_block(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModeMinusKMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan -k");

        // test 2
        send_line(so, validJoinMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(RPL_ENDOFNAMES, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_minusi_should_lift_block(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1 - -i
        send_line(sop, validModeMinusIMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan -i");

        // test 2 - user can join
        join_assert(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_minusl_should_lift_block(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1
        send_line(sop, validModeMinusLMsg); // 1
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan -l");

        // test 2
        join_assert(so);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_join(Server& s, t_results* r)
{
    print_test_series("command JOIN");
    run_test(r, [&] { valid_join_should_send_rpl_and_broadcast(s); }, "JOIN #chan (op user)");
    run_test(r, [&] { noparams_should_err(s); }, "JOIN with no params");
    run_test(r, [&] { name_no_prefix_should_err(s); }, "JOIN chan");
    run_test(r, [&] { name_too_big_should_err(s); }, "JOIN more 50 char channel name");
    run_test(r, [&] { mode_plusi_no_invite_should_err(s); }, "+i");
    run_test(r, [&] { mode_plusk_no_key_should_err(s); }, "+k <key>");
    run_test(r, [&] { mode_plusk_wrong_yek_should_err(s); }, "A user try to join with wrong yek");
    run_test(r, [&] { mode_plusk_wrong_keyy_should_err(s); }, "A user try to join with wrong keyy");
    run_test(r, 
        [&] { mode_plusi_with_invite_should_send_rpl_and_broadcast(s); }, "Joining A channel invite only after being invited.");
    run_test(r, [&] { mode_plusl_should_block_join_if_max_reached(s); }, "+l <limit>");
    run_test(r, [&] { mode_plusl_zeroarg_should_block_join(s); }, "+l 0");
    run_test(r, [&] { mode_minusk_should_lift_block(s); }, "-k <key>");
    run_test(r, [&] { mode_minusi_should_lift_block(s); }, "-i");
    run_test(r, [&] { mode_minusl_should_lift_block(s); }, "-l");
}
