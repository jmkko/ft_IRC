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
void op_existing_chan_valid_user_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);

        // test
        send_line(soOp, validKickMsg);
        // as a member, operator receives the notice
        std::string reply = recv_lines(soOp, opNick);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro");

        // kicked user gets a notice
        reply = recv_lines(so, userNick);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro");

        // kicked user can join again
        send_line(so, validJoinMsg);
        std::string joinReplies = recv_lines(so);
        ar.handle_new_reply(joinReplies);
        ar.is_formatted(RPL_NOTOPIC, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case - many users
*/
void op_one_channel_many_users_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        make_op(soOp);
        authenticate_and_join(so);
        authenticate_and_join_second_user(so2);

        // test
        send_line(soOp, validManyUsersKickMsg);
        // as a member, operator receives the transferred message
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro");
        ar.is_formatted_transfer(opNick, "KICK #chan toto");

        // kicked user1 gets an individual notice
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro");

        // kicked user2 gets an individual notice
        reply = recv_lines(so2);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan toto");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case - custom reason
*/
void kick_with_reason_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);

        make_op(soOp);
        authenticate_and_join(so);

        // test - operator receives broadcast with reason
        std::string reply = get_rpl_for(soOp, validKickReasonMsg);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro", "please behave roro");

        // kicked user1 gets an individual notice
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro", "please behave roro");

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
void no_op_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        make_op(soOp);
        authenticate_and_join(so);
        authenticate_and_join_second_user(so2);

        // test
        send_line(so2, validKickMsg);
        std::string reply = recv_lines(so2);
        AssertReply ar(reply);
        ar.is_formatted(ERR_CHANOPRIVSNEEDED, user2Nick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void op_missing_chan_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);

        // test
        send_line(soOp, invalidNoChanKickMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "KICK");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void op_missing_user_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);

        // test
        send_line(soOp, invalidNoUserKickMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "KICK");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void op_user_not_in_channel_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate(so);

        // test
        send_line(soOp, validKickMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_USERNOTINCHANNEL, opNick, "#chan roro");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void op_invalid_channel_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);

        // test
        send_line(soOp, invalidWrongChanKickMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_BADCHANMASK, opNick, "*chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void op_valid_inexistent_channel_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);

        // test
        send_line(soOp, validInexistentChannelKickMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHCHANNEL, opNick, "#chanel");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void op_matching_size_channel_list_and_user_list_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 6);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so1   = *sockets.at(1);
        TcpSocket& so2   = *sockets.at(2);
        TcpSocket& so3  = *sockets.at(3);
        TcpSocket& so4   = *sockets.at(4);

        make_op(soOp);
        authenticate(so1, "user1");
        authenticate(so2, "user2");
        authenticate(so3, "user3");
        authenticate(so4, "user4");

		send_line(soOp, "JOIN #chan1,#chan2,#chan3,#chan4,#chan5\r\n");
		send_line(so1, "JOIN #chan1\r\n");
		send_line(so2, "JOIN #chan2\r\n");
		recv_lines(so2);
		send_line(so3, "JOIN #chan3\r\n");
		recv_lines(so3);
		send_line(so4, "JOIN #chan4\r\n");
		recv_lines(so4);
        send_line(soOp, "KICK #chan1,#chan2,#chan3,#chan4 user1,user2,user3,user4 :you're kicked, badass\r\n");
        recv_lines(soOp);

        // user1 should get a transferred message
        std::string reply = recv_lines(so1);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan1 user1", "you're kicked, badass");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_kick(Server& s, t_results* r)
{
    print_test_series("command KICK");
    print_test_series_part("common cases");
    run_test(r, [&] { op_existing_chan_valid_user_should_transfer(s); }, "KICK single kick");
    run_test(r, [&] { op_one_channel_many_users_should_transfer(s); }, "KICK combo double kick");
    run_test(r, [&] { kick_with_reason_should_transfer(s); }, "KICK custom reason should appear in message");

    print_test_series_part("error cases");
    run_test(r, [&] { no_op_should_err(s); }, "KICK no op");
    run_test(r, [&] { op_missing_chan_should_err(s); }, "KICK no chan");
    run_test(r, [&] { op_missing_user_should_err(s); }, "KICK no user");
    run_test(r, [&] { op_user_not_in_channel_should_err(s); }, "KICK not in chan");
    run_test(r, [&] { op_invalid_channel_should_err(s); }, "KICK invalid chan");
    run_test(r, [&] { op_valid_inexistent_channel_should_err(s); }, "KICK inexisting chan");
    run_test(r, [&] { op_matching_size_channel_list_and_user_list_should_transfer(s); }, "KICK user list and channel list are same size");
}
