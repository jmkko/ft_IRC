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

#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

/************************************************************
 *		✅  VALID											*
 ************************************************************/

/**
 @brief integration test - normal case
*/
void mode_plusk_should_block_join_if_no_key(Server& s)
{
    try {

        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModePlusKMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+k");
		// test 2
		send_line(so, validJoinMsg);
		reply = recv_lines(sop);
		ar.handle_new_reply(reply);
		ar.has_code(ERR_BADCHANNELKEY);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusk_should_allow_op_to_join_without_key(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
       
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		make_two_ops(sop, sop2);

		// test 1 - +k
		send_line(sop, validModePlusKMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+k");
		// test 2 - part and re-join
		send_line(sop2, validPartMsg);
		join_assert(sop2);

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
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModeMinusKMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("-k");
		// test 2
		send_line(so, validJoinMsg);
		reply = recv_lines(sop);
		ar.handle_new_reply(reply);
		ar.has_code(RPL_ENDOFNAMES);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusi_should_send_rpl_and_block_join_if_no_invite(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& sop = *sockets.at(0);
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModePlusIMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+i");
		// test 2
		send_line(so, validJoinMsg);
		reply = recv_lines(sop);
		ar.handle_new_reply(reply);
		ar.has_code(ERR_INVITEONLYCHAN);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusi_should_allow_op_to_join_without_invite(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		make_two_ops(sop, sop2);

		// test 1 - +k
		send_line(sop, validModePlusIMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+i");
		// test 2 - part and re-join
		send_line(sop2, validPartMsg);
		join_assert(sop2);

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
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1 - -i
		send_line(sop, validModeMinusIMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("-i");
		// test 2 - user can join
		join_assert(so);

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
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModePlusLMsg); // 1
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+l");
		// test 2
		send_line(so, validJoinMsg);
		reply = recv_lines(so);
		ar.handle_new_reply(reply);
		ar.has_code(ERR_CHANNELISFULL);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plusl_should_allow_op_to_join_if_max_reached(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		make_two_ops(sop, sop2);

		// test 1 - +l
		send_line(sop, validModePlusLMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+l");
		// test 2 - part and re-join
		send_line(sop2, validPartMsg);
		join_assert(sop2);

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
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModeMinusLMsg); // 1
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("-l");
		// test 2
		join_assert(so);

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
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		TcpSocket& so = *sockets.at(2);
		make_op(sop);
		authenticate_and_join_op2(sop2);
		authenticate_and_join(so);

		// test 1
		send_line(sop, validModePlusOMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+o");
		// test 2
		send_line(sop2, validKickMsg);
		reply  = recv_lines(sop2);
		ar.handle_new_reply(reply);
		ar.contains("KICK");

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
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		TcpSocket& so = *sockets.at(2);
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

		// test 2
		send_line(sop2, validKickMsg);
		reply  = recv_lines(sop2);
		ar.handle_new_reply(reply);
		ar.has_code(ERR_CHANOPRIVSNEEDED);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_plust_should_reserve_topic_to_op(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModePlusTMsg); // 1
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+t");
		// test 2
		send_line(so, validTopicMsg);
		reply = recv_lines(so);
		ar.handle_new_reply(reply);
		ar.has_code(ERR_CHANOPRIVSNEEDED);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void mode_minust_should_grant_topic_to_all(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);
		send_line(sop, validModePlusTMsg); // 1
		recv_lines(sop);

		// test 1
		send_line(sop, validModeMinusTMsg); // 1
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("-t");
		// test 2
		send_line(so, validTopicMsg);
		reply = recv_lines(so);
		ar.handle_new_reply(reply);
		ar.has_code(RPL_TOPIC);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
*/
void user_with_wildcard_should_match(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(1);
		make_op(sop);
		authenticate_and_join_op2(sop2);

		// test 1
		send_line(sop, validModePlusOWildcardMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+o");
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
        ar.contains("MODE").contains("+l").contains("+o");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
*		❔ EDGE CASES										*
************************************************************/

/**
 @brief integration test - error case
*/
void mode_l_zeroarg_should_block_join(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& so = *sockets.at(1);
		make_op(sop);
		authenticate(so);

		// test 1
		send_line(sop, validModePlusLZeroMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.contains("MODE").contains("+l");
		// test 2
		send_line(so, validJoinMsg);
		reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.has_code(ERR_CHANNELISFULL);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

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
        ar.has_code(ERR_NEEDMOREPARAMS);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void no_mode_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
		TcpSocket& sop = *sockets.at(0);
		make_op(sop);

		// test 1
		send_line(sop, invalidModeNoModeMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.has_code(ERR_NEEDMOREPARAMS);
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
        ar.has_code(ERR_NEEDMOREPARAMS);

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
        ar.has_code(ERR_NEEDMOREPARAMS);

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
		TcpSocket& sop = *sockets.at(0);
		TcpSocket& sop2 = *sockets.at(0);
		make_op(sop);
		authenticate_and_join_op2(sop2);

		// test 1
		send_line(sop, invalidModePlusONoArgMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.has_code(ERR_NEEDMOREPARAMS);

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
        ar.has_code(ERR_KEYSET);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}


/**
 @brief integration test - error case
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
        ar.has_code(ERR_UNKNOWNMODE);

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
		send_line(sop, invalidModeUnknownModeMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.has_code(ERR_UNKNOWNMODE);

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
        ar.contains("wrong limit"); // custom ERR ?

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
		send_line(sop, invalidModeUnknownUserMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.has_code(ERR_NOSUCHNICK);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void wildcard_no_match_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
		TcpSocket& sop = *sockets.at(0);
		make_op(sop);

		// test 1
		send_line(sop, validModePlusOWildcardMsg);
		std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.has_code(ERR_NOSUCHNICK);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_mode(Server& s)
{
    print_test_series("command MODE");
    run_test([&] { mode_plusk_should_block_join_if_no_key(s); }, "+k <key>");
    run_test([&] { mode_plusk_should_allow_op_to_join_without_key(s); }, "+k <key> (op)");
    run_test([&] { mode_minusk_should_lift_block(s); }, "-k <key>");
    run_test([&] { mode_plusi_should_send_rpl_and_block_join_if_no_invite(s); }, "+i");
    run_test([&] { mode_plusi_should_allow_op_to_join_without_invite(s); }, "+i (op)");
    run_test([&] { mode_minusi_should_lift_block(s); }, "-i");
    run_test([&] { mode_plusl_should_block_join_if_max_reached(s); }, "+l <limit>");
    run_test([&] { mode_plusl_should_allow_op_to_join_if_max_reached(s); }, "+l <limit> (op)");
    run_test([&] { mode_minusl_should_lift_block(s); }, "-l");
    run_test([&] { mode_pluso_should_grant_op_and_kick(s); }, "+o <user>");
    run_test([&] { mode_minuso_should_remove_op_and_kick(s); }, "-o <user>");
    run_test([&] { mode_plust_should_reserve_topic_to_op(s); }, "+t");
    run_test([&] { mode_minust_should_grant_topic_to_all(s); }, "-t");
    run_test([&] { user_with_wildcard_should_match(s); }, "+o <us*r>");
    run_test([&] { many_modes_work(s); }, "+kl <key> <limit>");

    run_test([&] { mode_l_zeroarg_should_block_join(s); }, "+l 0");

    run_test([&] { no_chan_should_err(s); }, "no chan");
    run_test([&] { no_mode_should_err(s); }, "no mode");
	run_test([&] { no_keyarg_should_err(s); }, "+k no arg");
	run_test([&] { no_limitarg_should_err(s); }, "+l no arg");
	run_test([&] { no_oparg_should_err(s); }, "+o no arg");
	run_test([&] { key_already_set_should_err(s); }, "+k already set");
	run_test([&] { unknown_mode_should_err(s); }, "+z");
	run_test([&] { unknown_chan_should_err(s); }, "unknown chan");
	run_test([&] { mode_l_negativearg_should_err(s); }, "+l -1");
	run_test([&] { mode_o_unknown_user_should_err(s); }, "+o unknown");
	run_test([&] { wildcard_no_match_should_err(s); }, "<us*r> no match");

}