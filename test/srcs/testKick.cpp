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
void op_existing_chan_valid_user_should_notice(Server& s)
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
        std::string reply = recv_lines(soOp);
        LOG_TEST.debug("test_valid1: reply op", reply);
        AssertReply ar(reply);
        // ar.contains("KICK").contains(channelName).contains(userNick);

        // kicked user gets a notice
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.contains("KICK").contains(channelName).contains(userNick);

        // kicked user can join again
        send_line(so, validJoinMsg);
        std::string joinReplies = recv_lines(so);
        ar.handle_new_reply(joinReplies);
        ar.contains("JOIN");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case - many users
*/
void op_existing_chan_valid_users_should_notice(Server& s)
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
        // as a member, operator receives the notice
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        LOG_TEST.debug("test 2 - reply Op after KICK", reply);
        // ar.contains("KICK").contains(channelName).contains(userNick);

        // kicked user1 gets a notice
        reply = recv_lines(so);
        LOG_TEST.debug("test 2 - reply kicked user1 after KICK", reply);
        ar.handle_new_reply(reply);
        ar.contains("KICK").contains(channelName).contains(userNick);

        // kicked user2 gets a notice
        reply = recv_lines(so2);
        LOG_TEST.debug("test 2 - reply kicked user2 after KICK", reply);
        ar.handle_new_reply(reply);
        ar.contains("KICK").contains(channelName).contains(user2Nick);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

// /************************************************************
// *		❌ ERRORS											*
// ************************************************************/

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
        ar.has_code(ERR_CHANOPRIVSNEEDED);

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
        ar.has_code(ERR_NEEDMOREPARAMS);

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
        ar.has_code(ERR_NEEDMOREPARAMS);

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
        ar.has_code(ERR_USERNOTINCHANNEL);

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
        ar.has_code(ERR_BADCHANMASK);

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
        ar.has_code(ERR_NOSUCHCHANNEL);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_kick(Server& s)
{
    print_test_series("command KICK");
    run_test([&] { op_existing_chan_valid_user_should_notice(s); }, "single kick");
    run_test([&] { op_existing_chan_valid_users_should_notice(s); }, "combo double kick");
    run_test([&] { no_op_should_err(s); }, "no op");
    run_test([&] { op_missing_chan_should_err(s); }, "no chan");
    run_test([&] { op_missing_user_should_err(s); }, "no user");
    run_test([&] { op_user_not_in_channel_should_err(s); }, "not in chan");
    run_test([&] { op_invalid_channel_should_err(s); }, "invalid chan");
    run_test([&] { op_valid_inexistent_channel_should_err(s); }, "inexisting chan");
}