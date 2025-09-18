#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include	<sstream>
#include	<iostream>
#include	<string>
#include	<iomanip>

class Server;

class ReplyHandler
{
	public:
		~ReplyHandler();

		void									sendReply(int, ReplyCode, const std::string& target, const std::string& trailing = "");
		void									sendErrNeedMoreParams(int, const std::string& commandName);
		static ReplyHandler&	getInstance(Server*);

	private:
		Server*		_server;

		ReplyHandler(Server*);
		ReplyHandler(const ReplyHandler& inst);
};

#endif
