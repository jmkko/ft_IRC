
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

void valid_user_after_nick_should_rpl(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, "NICK mcpolo\r\n");

        // test
        send_line(so, "USER Marco 0 * :Marco Polo\r\n");
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.matches_entirely(":" + s.get_name() + " 001 mcpolo :" + ircCodes.trailing(RPL_WELCOME) + " mcpolo!Marco@localhost");
        ar.matches_entirely(":" + s.get_name() + " 002 mcpolo :" + ircCodes.trailing(RPL_YOURHOST) + " " + s.get_name());
        ar.matches_entirely(":" + s.get_name() + " 003 mcpolo :" + ircCodes.trailing(RPL_CREATED));
        ar.matches_entirely(":" + s.get_name() + " 004 mcpolo :" + s.get_name() + " 1.0  0 0");

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
    run_test(r, [&] { valid_user_should_void(s); }, "Marco should void himself");
    run_test(r, [&] { valid_user_after_nick_should_rpl(s); }, "Marco should received the welcome msg");
    print_test_series_part("invalid cases");
    run_test(r, [&] { invalid_user_char_arobase_should_err(s); }, "Mar@co should err");
}
