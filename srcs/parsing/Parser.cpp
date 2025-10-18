#include "Parser.hpp"
# include "ReplyHandler.hpp"
# include "Server.hpp"
# include "utils.hpp"
# include "Config.hpp"
// Default constructor
Parser::Parser(void): rh(NULL), _server(NULL), _client(NULL) {}

Parser::Parser(Server& server, Client& client): rh(&ReplyHandler::get_instance(&server)), _server(&server), _client(&client) {}

// Copy constructor
Parser::Parser(const Parser &other): rh(other.rh), _server(other._server), _client(other._client) {}

// Assignment operator overload
Parser &Parser::operator=(const Parser &other)
{
	if (this != &other) {
		rh = other.rh;
		_client = other._client;
		_server = other._server;
	}
    return (*this);
}

// Destructor
Parser::~Parser(void) {}

bool Parser::response(ReplyCode code, const std::string& params, const std::string& trailing) {
	if (code != CORRECT_FORMAT) {
		if (rh && _client) {
			rh->process_response(*_client, code, params, NULL, trailing);
		}
		return (false);
	}
	return (true);
}

bool Parser::correct_channel(std::string& name)
{
	ReplyCode code = CORRECT_FORMAT;
    if (name.empty()) {
		return (response(ERR_BADCHANMASK));
    }
    for (std::string::const_iterator it = name.begin(); it != name.end(); ++it) {
        unsigned char c = *it;
        if (c > 0xFF || Utils::is_char_of(c, std::string(FORBIDEN_CHAR_CHAN_NAME, 7))) { // NOLINT
			code = ERR_BADCHANMASK;
            break ;
        }
    }
    if (!Utils::is_char_of(static_cast<unsigned char>(name[0]), "#&+!")) {
		code = ERR_BADCHANMASK;
	}
	if (name.length() <= 1 && name.length() >= ircConfig.get_chan_name_max_len()) {
		code = ERR_BADCHANMASK;
	}

	return response(code, name);
}

bool Parser::correct_target(std::string& target)
{
	Parser parser;

	bool validChannel = parser.correct_channel(target);
	bool validNickname = parser.correct_nickname(target);

	if (!validChannel && !validNickname)
		return response(ERR_NOTONCHANNEL);
	return CORRECT_FORMAT;
}

/**
 * @brief check if the key is valid -- no commas or spaces allowed
 *
 * @param key
 * @return true or false
 */
bool Parser::correct_key(std::string& key)
{
    if (key.empty()) {
        return true;
    } else if (key.size() > CHAN_KEY_MAX_LEN) {
        return false;
    }
    for (std::string::const_iterator it = key.begin(); it != key.end(); ++it) {
        unsigned char c = *it;
        if (c > 0x07F || Utils::is_char_of(c, std::string(FORBIDEN_CHAR_CHAN_KEY, 7))) { // NOLINT
            return false;
        }
    }
    return true;
}

bool Parser::correct_nickname(std::string& nickname)
{
	ReplyCode 		code = CORRECT_FORMAT;
    bool 			invalidChar = std::count_if(nickname.begin(), nickname.end(), Utils::is_invalid_char_nick);

    if (nickname.empty()) {
		code = ERR_NONICKNAMEGIVEN;	
	} else if (invalidChar || std::isdigit(nickname[0])) {
        code = ERR_ERRONEUSNICKNAME;
    } else if (nickname.length() > ircConfig.get_nickname_max_len()) {
        nickname = nickname.substr(0, ircConfig.get_nickname_max_len());
    } 
	return response(code, nickname);
}

std::vector<std::string> Parser::convert_to_vector(std::string& params)
{
	std::istringstream iss(params);
	std::string 		str;
	std::vector<std::string> result;

	while (std::getline(iss, str, ',')) {
		result.push_back(str);
	}
	return (result);
}
// return an formated string of parameters args = param1,param2,param3
std::string Parser::format_parameter(std::string& params, Checker function)
{
    std::string        arguments, list, trailing;
    std::string        currentParam, comma;
    std::istringstream iss(params);

	// std::cout << "format_parameter: " << params << std::endl;;
    iss >> arguments;
	if (!arguments.empty() && arguments[0] == ':') {
		arguments.erase(0, 1);
		std::getline(iss, trailing);
		return (arguments + trailing);
	}
    std::istringstream issParams(arguments);
    while (std::getline(issParams, currentParam, ','))
	{
		if (function && (this->*function)(currentParam) == false) {
			continue ;
		}
		list += comma + currentParam;
		comma = ",";
    }
	if (!std::getline(iss, params)) {
		params.clear();
	};
	return list;
}
