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
void op_existing_chan_valid_user_should_broadcast(Server& s)
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
        // as a member, operator receives the transferred message
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "KICK #chan roro");
        ar.is_formatted_transfer(opNick, "KICK #chan toto");

        // kicked user1 gets an individual noticd
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
        ar.is_formatted(ERR_USERNOTINCHANNEL, opNick, "#chan");

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

void test_kick(Server& s, t_results* r)
{
    print_test_series("command KICK");
    run_test(r, [&] { op_existing_chan_valid_user_should_broadcast(s); }, "single kick");
    run_test(r, [&] { op_existing_chan_valid_users_should_notice(s); }, "combo double kick");
    run_test(r, [&] { no_op_should_err(s); }, "no op");
    run_test(r, [&] { op_missing_chan_should_err(s); }, "no chan");
    run_test(r, [&] { op_missing_user_should_err(s); }, "no user");
    run_test(r, [&] { op_user_not_in_channel_should_err(s); }, "not in chan");
    run_test(r, [&] { op_invalid_channel_should_err(s); }, "invalid chan");
    run_test(r, [&] { op_valid_inexistent_channel_should_err(s); }, "inexisting chan");
}
