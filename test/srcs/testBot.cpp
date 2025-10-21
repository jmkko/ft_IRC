#include "AssertReply.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Ping.hpp"
#include "Server.hpp"
#include "ServerRunner.hpp"
#include "TestFixture.hpp"
#include "consts.hpp"
#include "fakeClient.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "testUtils.hpp"
#include <chrono>
#include <thread>

/************************************************************
 *                      ✅ VALID                            *
 ************************************************************/

/**
 @brief integration test - normal case
*/
void bot_valid_target_channel_should_broadcast(Server& s)
{
    try {

        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);
  
        // operator receives a copy of his prompt and bot response
        send_line(soOp, "BOT #chan !reply :how are you?\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_BOT_WAIT_MS));
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan", "reply how are you?");
        ar.starts_with(":bot!bot@hazardous.irc.serv PRIVMSG #chan :");

        // other channel members receive a copy of the prompt and bot response
        reply = recv_lines(so);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan", "reply how are you?");
        ar.starts_with(":bot!bot@hazardous.irc.serv PRIVMSG #chan :");

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
void bot_no_target_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate_and_join(so);

        // test
        std::string reply = get_rpl_for(so, "BOT !reply :how are you?\r\n");
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, userNick, "BOT");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void bot_no_prompt_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate_and_join(so);

        // test
        std::string reply = get_rpl_for(so, "BOT #chan !reply\r\n");
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, userNick, "BOT");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void bot_no_subcommand_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate_and_join(so);

        // test
        std::string reply = get_rpl_for(so, "BOT #chan :hello\r\n");
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, userNick, "BOT");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}


/**
 @brief integration test - error case
*/
void bot_invalid_subcommand_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate_and_join(so);

        // test
        std::string reply = get_rpl_for(so, "BOT #chan !invalid :how are you?\r\n");
        AssertReply ar(reply);
        ar.is_formatted(CUSTOMERR_WRONG_FORMAT, userNick, "!invalid");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/**
 @brief integration test - error case
*/
void bot_not_member_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so2 = *sockets.at(1);
        make_op(soOp);
        authenticate(so2);

        // test
        std::string reply = get_rpl_for(so2, "BOT #chan !reply :how are you?\r\n");
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTONCHANNEL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_bot(Server& s, t_results* r)
{
    print_test_series("command BOT");
    print_test_series_part("common cases");
    run_test(r, [&] { bot_valid_target_channel_should_broadcast(s); }, "BOT #chan !reply :msg");
    print_test_series_part("error cases");
    run_test(r, [&] { bot_no_target_should_err(s); }, "BOT !reply :msg");
    run_test(r, [&] { bot_no_subcommand_should_err(s); }, "BOT #chan :msg");
    run_test(r, [&] { bot_no_prompt_should_err(s); }, "BOT #chan !reply");
    run_test(r, [&] { bot_invalid_subcommand_should_err(s); }, "BOT !reply :msg");
    run_test(r, [&] { bot_not_member_should_err(s); }, "BOT not member");
}
