#include "User.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Motd.hpp"
//#include "ReplyHandler.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <algorithm>
#include <iostream>

// Default constructor
User::User(void) {}

User::User(std::string& params) {
	Parser parser;

	_username = parser.format_parameter(params, NULL);
	_mode = parser.format_parameter(params, NULL);
	_unused = parser.format_parameter(params, NULL);
	_realname = parser.format_parameter(params, NULL);
}
// Copy constructor
User::User(const User& other) : ICommand() { (void)other; }

// Assignment operator overload
User& User::operator=(const User& other)
{
    (void)other;
    return (*this);
}

// Destructor
User::~User(void) {}

/**
 * @brief set _username and _realname
 *
 * @if this command is done after NICK send welcome message to client
 * 001    RPL_WELCOME
 *          "Welcome to the Internet Relay Network
 *          <nick>!<user>@<host>"
 * 002    RPL_YOURHOST
 *          "Your host is <servername>, running version <ver>"
 * 003    RPL_CREATED
 *          "This server was created <date>"
 * 004    RPL_MYINFO
 *          "<servername> <version> <available user modes>
 *          <available channel modes>"
 *        - The server sends Replies 001 to 004 to a user upon
 *          successful registration.
 *
 * @param server not used
 * @param client
 *
 */
void User::execute(Server& server, Client& client)
{
	Parser p(server, client);

	if (_realname.empty()) {
		p.response(ERR_NEEDMOREPARAMS, "USER");
        return;
	}
	if (std::count_if(_username.begin(), _username.end(), Utils::is_invalid_char_user)) {
		p.response(ERR_NEEDMOREPARAMS, "USER");
		return ;
	}
    // trim white space around realname; return 461 if only space in realname
    size_t start = _realname.find_first_not_of(WHITE_SPACE);
    if (start == std::string::npos) {
		p.response(ERR_NEEDMOREPARAMS, "USER");
        return ;
    }
    size_t end = _realname.find_last_not_of(WHITE_SPACE);
    _realname   = _realname.substr(start, end - start + 1);
    if (!client.get_user_name().empty() && !client.get_nickname().empty() && client.is_registered()) {
		p.response(ERR_ALREADYREGISTRED);
        return ;
    }
	
    client.set_user_name(_username);
    client.set_real_name(_realname);
    LOG_DV_CMD(_realname);
    if (!client.get_nickname().empty()) {
        client.set_status(REGISTERED);
        p.rh->process_welcome(server, client);
#ifndef TEST
        Motd motd("");
        motd.execute(server, client);
#endif
    } else {
        LOG_CMD.info("202 RPL_USER");
    }
}

/**
 * @brief Check User commands args
 *
 * @param server not used
 * @param client
 * @param params should mathc `<user> <mode> <unused> <realname>`
 * @remark mode and unused are always 0 * for this project
 * @remark grammar rule
 * user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
 *                 ; any octet except NUL, CR, LF, " " and "@"
 *
 * @return @ref ReplyCode
 */
ReplyCode User::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    std::istringstream iss(params);
    std::string        username, modearg, unusedarg, realname;
    size_t             invalidChar = 0;

    iss >> username;
    invalidChar = std::count_if(username.begin(), username.end(), Utils::is_invalid_char_user);
    if (username.empty() || invalidChar) {
        return (ERR_NEEDMOREPARAMS);
    }
    iss >> modearg;
    if (modearg.empty()) {
        return (ERR_NEEDMOREPARAMS);
    }
    iss >> unusedarg;
    if (unusedarg.empty()) {
        return (ERR_NEEDMOREPARAMS);
    }
    iss >> realname;
    if (realname.empty() || realname[0] != ':' || realname.length() < 2) {
        return (ERR_NEEDMOREPARAMS);
    }

    // trim white space around realname; return 461 if only space in realname
    size_t start = realname.find_first_not_of(WHITE_SPACE);
    if (start == std::string::npos) {
        return (ERR_NEEDMOREPARAMS);
    }
    size_t end = realname.find_last_not_of(WHITE_SPACE);
    realname   = realname.substr(start, end - start + 1);

    if (!client.get_user_name().empty() && !client.get_nickname().empty() && client.is_registered()) {
        params = "";
        return (ERR_ALREADYREGISTRED);
    }
    params = username + " " + realname;
    return (CORRECT_FORMAT);
}
