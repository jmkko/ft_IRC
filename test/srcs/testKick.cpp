#include "AssertReply.hpp"
#include "LogManager.hpp"
#include "consts.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "fakeClient.hpp"
#include "Nick.hpp"
#include "Server.hpp"
#include "printUtils.hpp"
#include "reply_codes.hpp"
#include "testUtils.hpp"
#include "utils.hpp"
#include "ServerRunner.hpp"
#include "signal_handler.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>


/************************************************************
*		✅  VALID											*
************************************************************/

/**
 @brief integration test - normal case
*/
void op_existing_chan_valid_user_should_void_then_can_reenter(Server& s)
{
    ServerRunner runner(s);
    runner.start();

    Socket soOp = -1;
    soOp        = make_client_socket(TEST_PORT);
	Socket so	= -1;
	so			= make_client_socket(TEST_PORT);

    if (soOp == -1)
        throw std::runtime_error("Failed to connect to server");
    try {
        make_op(soOp);

		authenticate(so);
		send_line(so, validJoinMsg);

		send_line(soOp, validKickMsg);
		std::string reply = recv_line(so);
        AssertReply ar(reply);
		ar.is_empty();

		// kicked user can join again
		send_line(so, validJoinMsg);
		reply = recv_line(so);
		ar.handle_new_reply(reply);
		ar.has_code(RPL_TOPIC);

        close(soOp);
    } catch (const std::runtime_error& e) {
        LOG_TEST.error(e.what());
        close(soOp);
        close(so);
    }
    runner.stop();
}

/************************************************************
*		❌ ERRORS											*
************************************************************/



void	test_kick(Server& s)
{
	print_test_series("command KICK");
	run_test([&] {op_existing_chan_valid_user_should_void_then_can_reenter(s);}, "reenter");
}