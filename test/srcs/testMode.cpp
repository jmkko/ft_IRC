#include "AssertReply.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "TcpSocket.hpp"
#include "TestFixture.hpp"
#include "consts.hpp"
#include "fakeClient.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "signal_handler.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <cassert>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

/************************************************************
 *		WAITING FOR PART									*
 ************************************************************/

/**
 @brief integration test - normal case
 @todo implement PART to test it
*/
// void mode_plusk_should_allow_op_to_join_without_key(Server& s)
// {
//     try {
//         TEST_SETUP(test, s, 2);

// 		TcpSocket& sop = *sockets.at(0);
// 		TcpSocket& sop2 = *sockets.at(1);
// 		make_two_ops(sop, sop2);

// 		// test 1 - +k
// 		send_line(sop, validModePlusKMsg);
// 		std::string reply = recv_lines(sop);
//         AssertReply ar(reply);
//         ar.is_formatted_transfer(opNick, "MODE #chan +k key");

// 		// test 2 - part and re-join
// 		send_line(sop2, validPartMsg);
// 		join_assert(sop2);

//     } catch (const std::runtime_error& e) {
//         LOG_TEST.error(e.what());
//     }
// }

/**
 @brief integration test - normal case
*/
// void mode_plusi_should_allow_op_to_join_without_invite(Server& s)
// {
//     try {
//         TEST_SETUP(test, s, 2);
// 		TcpSocket& sop = *sockets.at(0);
// 		TcpSocket& sop2 = *sockets.at(1);
// 		make_two_ops(sop, sop2);

// 		// test 1 - +k
// 		send_line(sop, validModePlusIMsg);
// 		std::string reply = recv_lines(sop);
//         AssertReply ar(reply);
//         ar.is_formatted_transfer(opNick,"MODE #chan +i");

// 		// test 2 - part and re-join
// 		send_line(sop2, validPartMsg);
// 		join_assert(sop2);

//     } catch (const std::runtime_error& e) {
//         LOG_TEST.error(e.what());
//     }
// }

/**
 @brief integration test - normal case
*/
// void mode_plusl_should_allow_op_to_join_if_max_reached(Server& s)
// {
//     try {
//         TEST_SETUP(test, s, 2);
// 		TcpSocket& sop = *sockets.at(0);
// 		TcpSocket& sop2 = *sockets.at(1);
// 		make_two_ops(sop, sop2);

// 		// test 1 - +l
// 		send_line(sop, validModePlusLMsg);
// 		std::string reply = recv_lines(sop);
//         AssertReply ar(reply);
//         ar.is_formatted_transfer(opNick,"MODE #chan +l 1");

// 		// test 2 - part and re-join
// 		send_line(sop2, validPartMsg);
// 		join_assert(sop2);

//     } catch (const std::runtime_error& e) {
//         LOG_TEST.error(e.what());
//     }
// }

/************************************************************
 *		✅  VALID											*
 ************************************************************/


/**
 @brief integration test - normal case
*/
void mode_no_option_should_send_modes_list(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);
        send_line(sop, validModePlusIMsg);
        recv_lines(sop);

        // test 1
        send_line(sop, validModeMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(RPL_CHANNELMODEIS, opNick, "#chan +i");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_pluso_should_grant_op_and_kick(Server& s)
{
    try {
        TEST_SETUP(test, s, 3);
        TcpSocket& sop  = *sockets.at(0);
        TcpSocket& sop2 = *sockets.at(1);
        TcpSocket& so   = *sockets.at(2);
        make_op(sop);
        authenticate_and_join_op2(sop2);
        authenticate_and_join(so);
        skip_lines(sop, 2);  // skipping MSG JOIN sop2 so
        skip_lines(sop2, 1); // skipping MSG JOIN so

        // test 1
        send_line(sop, validModePlusOMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +o " + op2Nick);

        // members don't receive broadcast TOFIX
        reply = recv_lines(sop2);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +o " + op2Nick);

        // test 2
        send_line(sop2, validKickMsg);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(op2Nick, "KICK #chan " + userNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_minuso_should_remove_op_and_kick(Server& s)
{
    try {
        TEST_SETUP(test, s, 3);
        TcpSocket& sop  = *sockets.at(0);
        TcpSocket& sop2 = *sockets.at(1);
        TcpSocket& so   = *sockets.at(2);
        make_op(sop);
        authenticate_and_join_op2(sop2);
        authenticate_and_join(so);
        send_line(sop, validModePlusOMsg);
        recv_lines(sop);

        // test 1
        send_line(sop, validModeMinusOMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("-o");
        ar.is_formatted_transfer(opNick, "MODE #chan -o " + op2Nick);

        // test 2
        send_line(sop2, validKickMsg);
        reply = recv_lines(sop2);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_CHANOPRIVSNEEDED, op2Nick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void many_modes_work(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, validModePlusKLMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +kl key 10");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
 *		❔ EDGE CASES										*
 ************************************************************/

/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

/**
 @brief integration test - error case
*/
void no_chan_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModeNoChanMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "MODE");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void no_keyarg_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModePlusKNoArgMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "MODE");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void no_limitarg_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModePlusLNoArgMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "MODE");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void no_oparg_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop  = *sockets.at(0);
        TcpSocket& sop2 = *sockets.at(1);
        make_op(sop);
        authenticate_and_join_op2(sop2);

        // test 1
        send_line(sop, invalidModePlusONoArgMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "MODE");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void key_already_set_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);
        send_line(sop, validModePlusKMsg);
        recv_lines(sop);

        // test 1
        send_line(sop, validModePlusKMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_KEYSET, opNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 *@brief integration test - error case
 */
void unknown_mode_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModeUnknownModeMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_UNKNOWNMODE, opNick, "z");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void unknown_chan_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModeUnknownChanMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHCHANNEL, opNick, "#ch");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void mode_l_negativearg_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModePlusLNegativeMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(CUSTOMERR_WRONG_FORMAT, opNick, "#chan +l -1");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void mode_o_unknown_user_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, validModeUnknownUserMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHNICK, opNick, "pouic");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void mode_minuso_noarg_user_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
        make_op(sop);

        // test 1
        send_line(sop, invalidModeMinusONoUserMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "MODE");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_mode(Server& s, t_results* r)
{
    print_test_series("command MODE");
    // Waiting for implementations
    // run_test([&] { mode_plusk_should_allow_op_to_join_without_key(s); }, "+k <key> (op)");
    // run_test([&] { mode_plusi_should_allow_op_to_join_without_invite(s); }, "+i (op)");
    // run_test([&] { mode_plusl_should_allow_op_to_join_if_max_reached(s); }, "+l <limit> (op)");

    run_test(r, [&] { mode_no_option_should_send_modes_list(s); }, "list");
    run_test(r, [&] { mode_pluso_should_grant_op_and_kick(s); }, "+o <user>");
    run_test(r, [&] { mode_minuso_should_remove_op_and_kick(s); }, "-o <user>");
    run_test(r, [&] { many_modes_work(s); }, "+kl <key> <limit>");

    run_test(r, [&] { no_chan_should_err(s); }, "no chan");
    run_test(r, [&] { no_keyarg_should_err(s); }, "+k no arg");
    run_test(r, [&] { no_limitarg_should_err(s); }, "+l no arg");
    run_test(r, [&] { no_oparg_should_err(s); }, "+o no arg");
    run_test(r, [&] { key_already_set_should_err(s); }, "+k already set");
    run_test(r, [&] { unknown_mode_should_err(s); }, "+z");
    run_test(r, [&] { unknown_chan_should_err(s); }, "unknown chan");
    run_test(r, [&] { mode_l_negativearg_should_err(s); }, "+l -1");
    run_test(r, [&] { mode_o_unknown_user_should_err(s); }, "+o unknown");
    run_test(r, [&] { mode_minuso_noarg_user_should_err(s); }, "-o no user");
}
