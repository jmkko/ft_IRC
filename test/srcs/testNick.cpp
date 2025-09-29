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
#include "signal_handler.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

/** 
 @brief integration test - normal case
*/
void	no_previous_nick_and_valid_nick_should_send_nothing()
{
	// Reset global signal
	globalSignal = 0;
	
	std::unique_ptr<Server> s;
	std::thread serverThread;
	Socket so = -1;
	
	try {
		s = std::make_unique<Server>(TEST_PORT, DEFAULT_PASSWORD);
		
		// Start server in a separate thread
		serverThread = std::thread([&s]() {
			try {
				s->start();
			} catch (const std::exception& e) {
				LOG_TEST.error("Server thread error: " + std::string(e.what()));
			}
		});
		
		// Give the server time to start
		std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS * 2));

		const std::string& validPassMsg = std::string("PASS ") + DEFAULT_PASSWORD + "\r\n";
		const std::string& validNickMsg = "NICK toto\r\n";

		so = make_client_socket(TEST_PORT);
		std::cout << "so is " << so << '\n';
		LOG_TEST.debug(std::string("so is ") + std::to_string(so));
		
		if (so != -1) {
			
			if (!send_line(so, validPassMsg))
				LOG_TEST.error("can't send");
			// Give time for server to process
			std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
			std::string reply = recv_line(so);
			LOG_SERVER.debug("Reply: " + reply);
			AssertReply ar(reply);
			ar.has_code(RPL_PASS);

			if (!send_line(so, validNickMsg))
				LOG_TEST.error("can't send");
			std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
			reply = recv_line(so);
			LOG_TEST.debug("Reply: " + reply);
			ar = AssertReply(reply);
			ar.is_empty();
			
		} else {
			LOG_TEST.error("Failed to connect to server");
			throw std::runtime_error("Failed to connect to server");
		}
		
	} catch (const std::exception& e) {
		LOG_TEST.error("Test error: " + std::string(e.what()));
		// Clean up socket if it was opened
		if (so != -1) {
			close(so);
		}
		// Signal the server to stop
		globalSignal = SIGINT;
		// Wait a bit for server to process the signal
		std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
		if (serverThread.joinable()) {
			serverThread.join();
		}
		throw; // Re-throw to be caught by run_test
	}
	
	// Clean up socket
	if (so != -1) {
		close(so);
	}
	
	// Signal the server to stop
	globalSignal = SIGINT;
	
	// Wait a bit for server to process the signal
	std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
	
	// Wait for server thread to finish with timeout
	if (serverThread.joinable()) {
		serverThread.join();
	}
}

void	test_nick()
{
	print_test_series("command NICK");
	run_test(
		[&] {
			no_previous_nick_and_valid_nick_should_send_nothing();
		 }, "first nick" );
}