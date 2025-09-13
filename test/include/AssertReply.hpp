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

		AssertReply&				hasCode(ReplyCode code);
		AssertReply&				endsWith(const std::string& trailing);
		AssertReply&				contains(const std::string& token);
		
	private:
		std::string					_reply;
		std::string					_prefix;
		std::string					_cmdOrCode;
		std::vector<std::string>	_args;
		std::string					_trailing;
	
};