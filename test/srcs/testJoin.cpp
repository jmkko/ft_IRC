
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
        ar.is_formatted(RPL_NAMREPLY, userNick, "= #chan", "@" + opNick + " " + userNick);
        ar.is_formatted(RPL_ENDOFNAMES, userNick, "#chan");
        ar.is_formatted(RPL_NOTOPIC, userNick, "#chan");

        // test 2
        reply = recv_lines(soOp);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(userNick, "JOIN #chan", "");

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

void mode_plusi_with_invite_should_broadcast(Server& s)
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
        ar.has_code(RPL_NAMREPLY).contains("= #chan").contains("@" + opNick).contains(userNick);

        ar.is_formatted(RPL_ENDOFNAMES, userNick, "#chan");

        ar.is_formatted(RPL_NOTOPIC, userNick, "#chan");
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
void mode_plusl_when_max_reached_should_err(Server& s)
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
void mode_plusl_zeroarg_should_err(Server& s)
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
void mode_minusk_should_transfer(Server& s)
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
void mode_minusi_should_transfer(Server& s)
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
void mode_minusl_should_transfer(Server& s)
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

void creation_of_multiple_chan_with_key_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate(so);

        // test 1 - check that user is in channels
        send_line(sop, "JOIN #chan1,#chan2 key1,key2\r\n");
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "JOIN #chan1");
        ar.is_formatted_transfer(opNick, "JOIN #chan2");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void join_0_multiple_chan_should_rpl_and_broadcast(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        TcpSocket& so  = *sockets.at(1);
        authenticate(sop);
        do_cmd(sop, "JOIN #chan1,#chan2\r\n");
        authenticate_second_user(so);
        do_cmd(so, "JOIN #chan1,#chan2\r\n");

        // test 1 - check that user is in channels
        send_line(so, "JOIN 0\r\n");
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(user2Nick, "PART #chan1");
        ar.is_formatted_transfer(user2Nick, "PART #chan2");
        std::string reply2 = recv_lines(so);
        AssertReply ar2(reply2);
        ar2.is_formatted_transfer(user2Nick, "PART #chan1");
        ar2.is_formatted_transfer(user2Nick, "PART #chan2");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_join(Server& s, t_results* r)
{
    print_test_series("command JOIN");
    print_test_series_part("common cases");
    run_test(r, [&] { valid_join_should_send_rpl_and_broadcast(s); }, "JOIN #chan");
    run_test(r, [&] { join_0_multiple_chan_should_rpl_and_broadcast(s); }, "JOIN 0");
    print_test_series_part("common cases - modes");
    run_test(r, [&] { mode_plusi_with_invite_should_broadcast(s); }, "JOIN after MODE +i and being invited.");
    run_test(r, [&] { creation_of_multiple_chan_with_key_should_transfer(s); }, "JOIN multiple creation of channels with keys");
    run_test(r, [&] { mode_minusk_should_transfer(s); }, "JOIN without key after MODE -k");
    run_test(r, [&] { mode_minusi_should_transfer(s); }, "JOIN without invite after MODE -i");
    run_test(r, [&] { mode_minusl_should_transfer(s); }, "JOIN full channel after MODE -l");
    print_test_series_part("edge cases");
    run_test(r, [&] { mode_plusl_zeroarg_should_err(s); }, "JOIN after MODE +l 0");
    print_test_series_part("error cases");
    run_test(r, [&] { noparams_should_err(s); }, "JOIN with no params");
    run_test(r, [&] { mode_plusl_when_max_reached_should_err(s); }, "JOIN full channel after MODE +l <limit>");
    run_test(r, [&] { name_no_prefix_should_err(s); }, "JOIN chan");
    run_test(r, [&] { name_too_big_should_err(s); }, "JOIN more 50 char channel name");
    run_test(r, [&] { mode_plusk_wrong_yek_should_err(s); }, "A user try to join with wrong yek");
    run_test(r, [&] { mode_plusk_wrong_keyy_should_err(s); }, "A user try to join with wrong keyy");
    run_test(r, [&] { mode_plusk_wrong_yek_should_err(s); }, "A user try to join with wrong yek");
    run_test(r, [&] { mode_plusk_wrong_keyy_should_err(s); }, "A user try to join with wrong keyy");
}
