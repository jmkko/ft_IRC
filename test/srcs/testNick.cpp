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

const std::string& validUserMsg = "USER roro 0 * :Ronnie Reagan\r\n";
const std::string& validNickMsg = "NICK roro\r\n";
const std::string& validNickSpecialMsg = "NICK [roro]\r\n";
const std::string& validNickChangeMsg = "NICK rorotheboss\r\n";
const std::string& invalidNickMissingArgMsg = "NICK \r\n";
const std::string& invalidNick3oroMsg = "NICK 3oro\r\n";
const std::string& validPassMsg = std::string("PASS ") + DEFAULT_PASSWORD + "\r\n";

static void send_valid_password(Socket so)
{
	send_line(so, validPassMsg);		
	std::string reply = recv_line(so);
	AssertReply ar(reply);
	ar.is_empty();
}

static void send_valid_nick(Socket so)
{
	send_line(so, validNickMsg);
	std::string reply = recv_line(so);
	AssertReply ar(reply);
	ar.is_empty();
}

static void authenticate(Socket so)
{
	send_line(so, validPassMsg);		
	send_line(so, validNickMsg);
	send_line(so, validUserMsg);
}

/************************************************************
*		✅  VALID											*
************************************************************/

/** 
 @brief integration test - normal case
*/
void	valid_nick_should_void(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);
	LOG_TEST.debug(std::string("so is ") + std::to_string(so));

	if (so == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		send_valid_password(so);
		send_valid_nick(so);
		close(so);
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

/** 
 @brief integration test - normal case
*/
void	valid_nick_special_should_void(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);
	LOG_TEST.debug(std::string("so is ") + std::to_string(so));

	if (so == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		send_valid_password(so);
		send_line(so, validNickSpecialMsg);
		std::string reply = recv_line(so);
		AssertReply ar(reply);
		ar.is_empty();

		close(so);
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

/** 
 @brief integration test - error case
*/
void	valid_change_should_void(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);

	if (so == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		authenticate(so);

		send_line(so, validNickChangeMsg);
		std::string reply = recv_line(so);
		AssertReply ar(reply);
		ar.is_empty();

		close(so);
			
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

/************************************************************
*		❌ ERRORS											*
************************************************************/

/** 
 @brief integration test - error case
*/
void	no_arg_should_err(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);

	if (so == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		send_valid_password(so);

		send_line(so, invalidNickMissingArgMsg);
		std::string reply = recv_line(so);
		AssertReply ar(reply);
		ar.has_code(ERR_NONICKNAMEGIVEN);

		close(so);
			
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

/** 
 @brief integration test - error case
*/
void	starting_with_number_should_err(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);

	if (so == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		send_valid_password(so);

		send_line(so, invalidNick3oroMsg);
		std::string reply = recv_line(so);
		AssertReply ar(reply);
		ar.has_code(ERR_ERRONEUSNICKNAME);

		close(so);
			
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

/** 
 @brief integration test - error case
*/
void	taken_should_err(Server& s)
{	
	ServerRunner runner(s);
	runner.start();

	Socket so = -1;
	so = make_client_socket(TEST_PORT);

	Socket so2 = -1;
	so2 = make_client_socket(TEST_PORT);

	if (so == -1 || so2 == -1)
		throw std::runtime_error("Failed to connect to server");
	try {
		authenticate(so);

		send_valid_password(so2);
		send_line(so, validNickMsg);
		std::string reply = recv_line(so);
		AssertReply ar(reply);
		ar.has_code(ERR_NICKNAMEINUSE);

		close(so);
			
	} catch (const std::runtime_error& e) {
		LOG_TEST.error(e.what());
		close(so);
	}
	runner.stop();
}

void	test_nick(Server& s)
{
	print_test_series("command NICK");
	run_test([&] {valid_nick_should_void(s);}, "roro" );
	run_test([&] {valid_nick_special_should_void(s);}, "[roro]" );
	run_test([&] {no_arg_should_err(s);}, "no arg" );
	run_test([&] {starting_with_number_should_err(s);}, "3oro" );
	run_test([&] {taken_should_err(s);}, "taken" );
}