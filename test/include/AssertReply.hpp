#pragma once

#include "reply_codes.hpp"

#include <stdexcept>
#include <string>
#include <vector>
#include <iomanip>

class AssertReply {

	public:

		AssertReply(const std::string& reply);
		virtual ~AssertReply();

		AssertReply&				has_code(ReplyCode code);
		AssertReply&				ends_with(const std::string& trailing);
		AssertReply&				is_empty();
		AssertReply&				matches_entirely(const std::string& reply);
		AssertReply&				contains(const std::string& token);

		AssertReply&				handle_new_reply(const std::string& reply);
		
	private:
		std::string					_reply;
		std::string					_prefix;
		std::string					_cmdOrCode;
		std::vector<std::string>	_args;
		std::string					_trailing;
	
};