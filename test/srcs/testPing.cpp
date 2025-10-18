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

/************************************************************
 *                      ✅ VALID                            *
 ************************************************************/

/**
 @brief integration test - normal case
*/
void valid_ping_should_pong(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate(so);
        std::string token = "token";
        send_line(so, validPingMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.matches_entirely(":" + ircConfigTest.get_name() + " PONG :" + token);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

/**
 @brief integration test - normal case
*/
void invalid_ping_should_err(Server& s)
{
    try {

        TEST_SETUP(test, s, 1);
        TcpSocket& so = *sockets.at(0);
        authenticate(so);
        send_line(so, invalidPingMsg);

        // test
        std::string reply = recv_lines(so);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NOORIGIN, userNick);

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void test_ping(Server& s, t_results* r)
{
    print_test_series("command PING");
    print_test_series_part("common cases");
    run_test(r, [&] { valid_ping_should_pong(s); }, "ping");
    print_test_series_part("error cases");
    run_test(r, [&] { invalid_ping_should_err(s); }, "no origin");
}
