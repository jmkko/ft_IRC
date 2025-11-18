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

void nick_as_receiver_should_transfer(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);

        // msg is received by dest
        send_line(soOp, validPrivmsgSingleUser);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG roro", "hi");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void channel_as_receiver_should_broadcast(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);

        // msg is broadcasted = not received on channel by sender
        send_line(soOp, validPrivmsgSingleChan);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_empty();

        // .. but received by channel member
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan", "hi");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void multiple_channels_as_receiver_should_broadcast(Server& s)
{
    try {
        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        make_op(soOp);
        authenticate_and_join(so);
        authenticate_second_user(so2);
        do_cmd(soOp, "JOIN #chan2\r\n");
        do_cmd(so2, "JOIN #chan2\r\n");
        recv_lines(soOp);

        // sender does not receive
        send_line(soOp, validPrivMsgTwoChans);
        std::string reply = recv_lines(soOp, "op");
        AssertReply ar(reply);
        ar.is_empty();

        // other chan members receive
        reply = recv_lines(so, "roro");
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan", "hi");

        reply = recv_lines(so2, "toto");
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan2", "hi");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void nick_and_channel_as_receiver_should_transfer_and_broadcast(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate(so);

        // msg is broadcasted and received by members except sender
        send_line(soOp, validPrivmsgChanUser);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_empty();

        // .. and as dm
        reply = recv_lines(so);
        ar.handle_new_reply(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG roro", "hi");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
 *      ❔ EDGE CASES 								    	*
 ************************************************************/

void empty_msg_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test is received
        send_line(soOp, invalidPrivmsgEmptyTrailing);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTEXTTOSEND, opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void blank_msg_should_do_broadcast(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate_and_join(so);

        // msg is received by other member
        send_line(soOp, validPrivmsgBlank);
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted_transfer(opNick, "PRIVMSG #chan", " ");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void msg_in_arg_instead_of_trailing_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 3);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        make_op(soOp);
        authenticate_and_join(so);
        recv_lines(soOp);
        authenticate_and_join_second_user(so2);
        recv_lines(soOp);

        // test is received
        send_line(soOp, invalidPrivMsgInArg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTEXTTOSEND, opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void msg_in_arg_instead_of_trailing_should_notice(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so = *sockets.at(1);
        make_op(soOp);

		authenticate(so, "user");
        // test is received
        send_line(soOp, "PRIVMSG user many words message\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.matches_entirely(":op!op@hazardous.irc.serv PRIVMSG user :many words message");
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}
/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

void not_channel_member_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 2);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so   = *sockets.at(1);
        make_op(soOp);
        authenticate(so);

        // test
        std::string reply = get_rpl_for(so, validPrivmsgSingleChan);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTONCHANNEL, userNick, "#chan");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void no_params_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, invalidPrivmsgNoParams);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, opNick, "PRIVMSG");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_nickname_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, validPrivmsgWrongUser);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHNICK, opNick, "nonexistent");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void invalid_privmsg_channel_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        // test
        send_line(soOp, validPrivmsgWrongChannel);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOSUCHCHANNEL, opNick, "#nonexistent");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void too_many_target_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 7);
        TcpSocket& soOp = *sockets.at(0);
        TcpSocket& so1  = *sockets.at(1);
        TcpSocket& so2  = *sockets.at(2);
        TcpSocket& so3  = *sockets.at(3);
        TcpSocket& so4  = *sockets.at(4);
        TcpSocket& so5  = *sockets.at(5); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        TcpSocket& so6  = *sockets.at(6); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        make_op(soOp);
        authenticate(so1, userNick);
        authenticate(so2, user2Nick);
        authenticate(so3, user3Nick);
        authenticate(so4, user4Nick);
        authenticate(so5, user5Nick);
        authenticate(so6, user6Nick);

        // test - message should mention only the last offending target
        send_line(soOp, validPrivMsgToomanytarget);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_TOOMANYTARGETS, opNick, "tata");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void no_text_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);
        make_op(soOp);

        send_line(soOp, invalidPrivmsgNoText);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOTEXTTOSEND, opNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_privmsg(Server& s, t_results* r)
{
    print_test_series("command PRIVMSG");
    print_test_series_part("common cases");
    run_test(r, [&] { nick_as_receiver_should_transfer(s); }, "PRIVMSG to roro");
    run_test(r, [&] { channel_as_receiver_should_broadcast(s); }, "PRIVMSG to #chan");
    run_test(r, [&] { multiple_channels_as_receiver_should_broadcast(s); }, "PRIVMSG to #chan,#chan2");
    run_test(r, [&] { nick_and_channel_as_receiver_should_transfer_and_broadcast(s); }, "PRIVMSG to roro,#chan");

    print_test_series_part("edge cases");
    run_test(r, [&] { empty_msg_should_err(s); }, "'PRIVMSG #chan :'");
    run_test(r, [&] { blank_msg_should_do_broadcast(s); }, "'PRIVMSG #chan : '");
    run_test(r, [&] { msg_in_arg_instead_of_trailing_should_notice(s); }, "'PRIVMSG #chan msg'"); // <-- add opposite test 
    
    print_test_series_part("error cases");
    run_test(r, [&] { not_channel_member_should_err(s); }, "PRIVMSG to #chan without being a member");
    run_test(r, [&] { no_params_should_err(s); }, "PRIVMSG with no params");
    run_test(r, [&] { invalid_nickname_should_err(s); }, "PRIVMSG to non existent nickname");
    run_test(r, [&] { invalid_privmsg_channel_should_err(s); }, "PRIVMSG to non existent channel");
    run_test(r, [&] { too_many_target_should_err(s); }, "PRIVMSG with too many targets");
    run_test(r, [&] { no_text_should_err(s); }, "PRIVMSG with no message");
}
