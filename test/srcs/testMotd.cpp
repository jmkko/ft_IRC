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
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

/************************************************************
*		✅  VALID											*
************************************************************/

/**
 @brief integration test - normal case
 */
void valid_motd_should_rpl(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        if (chmod(MOTD_FILE_FOR_TEST, PERM_644) != 0) {
            throw std::runtime_error(TO_STRING("error changing rights ") + strerror(errno));
        }
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, validNickMsg);
        send_line(so, validUserMsg);
        recv_lines(so);
        // std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_MOTD_WAIT_MS));
        // std::string registerReply;
        // int tries = 0;
        // while (registerReply.find(ircCodes.trailing(RPL_ENDOFMOTD)) == std::string::npos || tries < MAX_TRIES_MOTD)
        // {
        //     registerReply += recv_lines(so, "roro on registration");
        //     ++tries;
        // }
        // LOG_DV_TEST(registerReply);

        // test
        send_line(so, validMotd);
        std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_MOTD_WAIT_MS));
        std::string reply = "";
        int tries = 0;
        while (reply.find(ircCodes.trailing(RPL_ENDOFMOTD)) == std::string::npos || tries < MAX_TRIES_MOTD)
        {
            reply += recv_lines(so, "roro on motd");
            ++tries;
        }
        LOG_DV_TEST(reply);
        AssertReply ar(reply);
        ar.is_formatted(RPL_MOTDSTART, userNick, "", "- " + ircConfigTest.get_name() + " message of the day -");
        ar.is_formatted(RPL_MOTD, userNick, "", "|   Enjoy the trip through hazardous cyberspace!         |");
        ar.is_formatted(RPL_ENDOFMOTD, userNick, "");

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
void motd_not_opening_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        if (chmod(MOTD_FILE_FOR_TEST, 000) != 0) {
            throw std::runtime_error(TO_STRING("error changing rights ") + strerror(errno));
        }
        TcpSocket& so = *sockets.at(0);
        send_line(so, validPassMsg);
        send_line(so, validNickMsg);
        send_line(so, validUserMsg);
        recv_lines(so);

        // test
        std::string reply = get_rpl_for(so, validMotd);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOMOTD, userNick, "");

        if (chmod(MOTD_FILE_FOR_TEST, PERM_644) != 0) {
            throw std::runtime_error(TO_STRING("error changing rights back ") + strerror(errno));
        }
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_motd(Server& s, t_results* r)
{
    print_test_series("command MOTD");
    print_test_series_part("common cases");

    run_test(r, [&] { valid_motd_should_rpl(s); }, "'MOTD");
    
    print_test_series_part("error cases");
    run_test(r, [&] { motd_not_opening_should_err(s); }, "'MOTD file with no rights");
}
