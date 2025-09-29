#include "AssertReply.hpp"

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

/// @brief parses a reply string into tokens : 
/// prefix (server name when sent by the server)
/// target : numeric code (or command name)
/// arguments
/// trailing message
/// @param reply 
AssertReply::AssertReply(const std::string& reply) : _reply(reply), _args() 
{
	std::stringstream ss(_reply);
	if (_reply[0] == ':')
		ss >> _prefix;
	ss >> _cmdOrCode;
	std::string token;
	while (ss >> token)
	{
		if (token[0] == ':')
		{
			_trailing = token;
			break ;
		}
		_args.push_back(token);
	}
}

AssertReply::~AssertReply(void) {}

/************************************************************
*		➕ OPERATORS											*
************************************************************/

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

AssertReply&	AssertReply::has_code(ReplyCode code) 
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(3) << code;

	if (_cmdOrCode != oss.str())
	{
		throw std::runtime_error("Expected code " + oss.str() + " not found ");
	}
	return *this;
}

AssertReply&	AssertReply::ends_with(const std::string& trailing) 
{
	if (_trailing != trailing)
	{
		throw std::runtime_error("Expected trailing message " + trailing + " not found ");
	}
	return *this;
}

AssertReply&	AssertReply::contains(const std::string& token) 
{
	if (_reply.find(token) == std::string::npos)
	{
		throw std::runtime_error("Expected token " + token + " not found ");
	}
	return *this;
}

AssertReply&	AssertReply::is_empty() 
{
	if (!_reply.empty())
	{
		throw std::runtime_error("Expected no reply ");
	}
	return *this;
}
