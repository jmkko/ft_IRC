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

/************************************************************
*		✅  VALID											*
************************************************************/

/**
 @brief integration test - normal case
 */
void valid_topic_info_when_empty_should_rpl(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        std::string reply = get_rpl_for(soOp, validTopicEmpty);
        AssertReply ar(reply);
        ar.is_formatted(RPL_NOTOPIC, opNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
 */
void valid_topic_change_should_rpl(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        std::string reply = get_rpl_for(soOp, validTopic);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "TOPIC #chan",  "new topic");

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
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate_and_join(so);

        // test 1
        send_line(sop, validModePlusTMsg); // 1
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan +t");

        // test 2
        send_line(so, validTopic);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted(ERR_CHANOPRIVSNEEDED, userNick, "#chan");

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
        TcpSocket& so  = *sockets.at(1);
        make_op(sop);
        authenticate_and_join(so);
        send_line(sop, validModePlusTMsg);
        recv_lines(sop);

        // test 1
        send_line(sop, validModeMinusTMsg);
        std::string reply = recv_lines(sop);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "MODE #chan -t");

        // test 2
        send_line(so, validTopic);
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(userNick, "TOPIC #chan", "new topic");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
*      ❔ EDGE CASES 								    	*
************************************************************/

/**
 @brief integration test - normal case
 */
void topic_in_trailing_should_rpl(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        std::string reply = get_rpl_for(soOp, edgeTopicTrailing);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "TOPIC #chan ::trailing");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - normal case
 */
void concurrent_changes_should_rpl(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& soOp2 = *sockets.at(1);
        make_two_ops(soOp, soOp2);

        // op should have received both notifications
        send_line(soOp, validTopic);
        send_line(soOp2, validTopic2);
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
        std::string reply = recv_lines(soOp, "op");
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "TOPIC #chan :new topic");
        ar.is_formatted_transfer(op2Nick, "TOPIC #chan :new topic2");

        // op2 should have received both notifications
        reply = recv_lines(soOp2, "op2");
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "TOPIC #chan :new topic");
        ar.is_formatted_transfer(op2Nick, "TOPIC #chan :new topic2");

        // topic should be updated with last change
        reply = get_rpl_for(soOp, validTopicEmpty);
        ar.handle_new_reply(reply);
        ar.is_formatted(RPL_TOPIC, opNick, "#chan", "new topic2");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
*		❌ ERRORS											*
************************************************************/

void no_params_should_err_topic(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, invalidTopicNoParam);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "TOPIC");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_channel_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, validTopicWrongChannel);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHCHANNEL, opNick, "$notvalid");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void not_a_channel_member_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate(so);

        // test
        send_line(so, validTopic);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTONCHANNEL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_topic(Server& s, t_results* r)
{
    print_test_series("command TOPIC");
    print_test_series_part("common cases");
    run_test(r, [&] { valid_topic_info_when_empty_should_rpl(s); }, "'TOPIC #chan' for RPL_TOPIC");
    run_test(r, [&] { valid_topic_change_should_rpl(s); }, "'TOPIC #chan new topic'");
    run_test(r, [&] { mode_plust_should_reserve_topic_to_op(s); }, "TOPIC after MODE +t");
    run_test(r, [&] { mode_minust_should_grant_topic_to_all(s); }, "TOPIC after MODE -t");

    print_test_series_part("edge cases");
    run_test(r, [&] { topic_in_trailing_should_rpl(s); }, "'TOPIC #chan :new topic'");
    run_test(r, [&] { concurrent_changes_should_rpl(s); }, "TOPIC quasi concurrent changes");

    print_test_series_part("error cases");
    run_test(r, [&] { no_params_should_err_topic(s); }, "'TOPIC'");
    run_test(r, [&] { invalid_channel_should_err(s); }, "TOPIC on non existant channel");
    run_test(r, [&] { not_a_channel_member_should_err(s); }, "TOPIC user is not a channel member");
}
