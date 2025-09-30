#include "AssertReply.hpp"

#include "AssertFail.hpp"
#include "consts.hpp"
#include <algorithm>
#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/// @brief parses a reply string into tokens :
/// prefix (server name when sent by the server)
/// target : numeric code (or command name)
/// arguments
/// trailing message
/// @param reply
AssertReply::AssertReply(const std::string& reply) : _reply(reply), _messages()
{
	_process_reply();
}

AssertReply::~AssertReply(void) {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

bool	AssertReply::_has_message_code(const Message& msg, const std::string& expected, std::string* actual)
{
	
    if (msg.cmdOrCode != expected)
	{
		if (msg.cmdOrCode.empty())
			*actual = "nothing";
		else
			*actual = msg.cmdOrCode;
		return false;
	}
	return true;
}

bool	AssertReply::_is_message_ending_with(const Message& msg, const std::string& trailing, std::string* actual)
{
	if (msg.trailing != trailing)
	{
		*actual = msg.trailing;
		return false;
	}
	return true;
}

bool	AssertReply::_is_message_empty(const Message& msg, std::string* actual)
{
	if (!msg.raw.empty())
	{
		*actual = msg.raw;
		return false;
	}
	return true;
}

bool	AssertReply::_is_message_matching_entirely(const Message& msg, const std::string& message, std::string* actual)
{
	if (msg.raw != message)
	{
		*actual = msg.raw;
		return false;
	}
	return true;
}

bool	AssertReply::_is_message_containing(const Message& msg, const std::string& token)
{
	return msg.raw.find(token);
}

AssertReply& AssertReply::has_code(ReplyCode code)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(3) << code;
	std::string actual = "";
	bool isMatching = false;
	for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
		if (_has_message_code(*it, oss.str(), &actual))
		{
			isMatching = true;
			break;
		}
	}
	if (!isMatching)
        throw AssertFail("code", oss.str(), actual);
    return *this;
}

AssertReply& AssertReply::ends_with(const std::string& trailing)
{
	std::string actual = "";
	bool isMatching = false;
	for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
		if (_is_message_ending_with(*it, trailing, &actual))
		{
			isMatching = true;
			break;
		}
	}
    if (!isMatching) {
        throw AssertFail("trailing message ", trailing, actual);
    }
    return *this;
}

AssertReply& AssertReply::contains(const std::string& token)
{
	bool isMatching = false;
	for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
		if (_is_message_containing(*it, token))
		{
			isMatching = true;
			break;
		}
	}
    if (!isMatching) {
        throw AssertFail("message ", token, "no occurence");
    }
    return *this;
}

AssertReply& AssertReply::matches_entirely(const std::string& message)
{
	std::string actual = "";
	bool isMatching = false;
	for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
		if (_is_message_matching_entirely(*it, message, &actual))
		{
			isMatching = true;
			break;
		}
	}
    if (!isMatching) {
        throw AssertFail("message ", message, actual);
    }
    return *this;
}

AssertReply& AssertReply::is_empty()
{
	std::string actual = "";
	bool isMatching = false;
	for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
		if (_is_message_empty(*it, &actual))
		{
			isMatching = true;
			break;
		}
	}
    if (!isMatching) {
        throw AssertFail("message ", "empty", actual);
    }
    return *this;
}

AssertReply& AssertReply::handle_new_reply(const std::string& reply)
{
    _reply = reply;
	_process_reply();
    return *this;
}

void	AssertReply::_process_reply()
{
    std::istringstream iss(_reply);

	std::string rawMsg;
	std::string msgPrefix;
	std::string msgCmdOrCode;
	std::string msgTrailing;
	t_params	msgArgs;
	while (std::getline(iss, rawMsg, '\n'))
	{
		std::istringstream issMsg(rawMsg);
		if (rawMsg[0] == ':')
        	issMsg >> msgPrefix;
		issMsg >> msgCmdOrCode;
		std::string token;
		while (issMsg >> token) {
			if (token[0] == ':') {
				msgTrailing = token;
				break;
			}
			msgArgs.push_back(token);
    	}
		Message msg = {
			.args = msgArgs,
			.cmdOrCode = msgCmdOrCode,
			.prefix = msgPrefix,
			.trailing = msgTrailing,
			.raw = rawMsg
		};
		_messages.push_back(msg);
	}
}