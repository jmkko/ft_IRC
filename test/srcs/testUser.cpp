
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testNick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhervoch <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 16:00:12 by jhervoch          #+#    #+#             */
/*   Updated: 2025/10/17 16:09:27 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AssertReply.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "TestFixture.hpp"
#include "User.hpp"
#include "consts.hpp"
#include "fakeClient.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>

/************************************************************
 *                      ✅  VALID                           *
 ************************************************************/

void valid_user_should_void(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);

        // test
        send_line(so, "USER Marco 0 * :Marco Polo\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_empty();

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

// Failing on GITHUB Actions if assert on whole message - can't get codes.conf ?
void valid_user_after_nick_should_rpl(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, "NICK mcpolo\r\n");

        // test
        send_line(so, "USER Marco 0 * :Marco Polo\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_MOTD_WAIT_MS));
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.has_code(RPL_WELCOME).contains("mcpolo");
        ar.has_code(RPL_YOURHOST).contains("mcpolo");
        ar.has_code(RPL_CREATED).contains("mcpolo");
        ar.has_code(RPL_MYINFO).contains("mcpolo");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}
/************************************************************
 *                      ❌ ERROR                            *
 ************************************************************/
void invalid_user_char_arobase_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, "NICK mcpolo\r\n");

        // test
        send_line(so, "USER Mar@co 0 * :Marco Polo\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, "mcpolo", "USER");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_user(Server& s, t_results* r)
{
    print_test_series("command USER");
    print_test_series_part("valid cases");
    run_test(r, [&] { valid_user_should_void(s); }, "USER Marco should void himself");
    run_test(r, [&] { valid_user_after_nick_should_rpl(s); }, "USER Marco should receive the welcome msg");
    print_test_series_part("invalid cases");
    run_test(r, [&] { invalid_user_char_arobase_should_err(s); }, "USER Mar@co should err");
}
