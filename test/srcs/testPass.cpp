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

void correct_pass_should_void(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);

        send_line(soOp, validPassMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_empty();

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

/************************************************************
 *		❌ ERRORS											*
 ************************************************************/

void no_pass_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);

        send_line(soOp, invalidPassNoParamMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_NEEDMOREPARAMS, "*", "PASS");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

void wrong_pass_should_err(Server& s)
{
    try {
        TEST_SETUP(test, s, 1);
        TcpSocket& soOp = *sockets.at(0);

        send_line(soOp, validPassWrongMsg);
        std::string reply = recv_lines(soOp);
        AssertReply ar(reply);
        ar.is_formatted(ERR_PASSWDMISMATCH, "*");

    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
    }
}

// void already_registered_should_err(Server& s)
// {
//     try {
//         TEST_SETUP(test, s, 1);
//         TcpSocket& so = *sockets.at(0);
//         authenticate(so);
        
//         std::string reply = get_rpl_for(so, validPassMsg);
//         AssertReply ar(reply);
//         ar.is_formatted(ERR_ALREADYREGISTRED, userNick);

//     } catch (const std::runtime_error& e) {
//         LOG_TEST.error(e.what());
//     }
// }

 void test_pass(Server& s, t_results* r)
{
    print_test_series("command PASS");
    print_test_series_part("common cases");
    run_test(r, [&] { correct_pass_should_void(s); }, "PASS correct");
    print_test_series_part("error cases");
    run_test(r, [&] { no_pass_should_err(s); }, "PASS with no arg");
    run_test(r, [&] { wrong_pass_should_err(s); }, "PASS wrong");
    // run_test(r, [&] { already_registered_should_err(s); }, "PASS already registered");
}
