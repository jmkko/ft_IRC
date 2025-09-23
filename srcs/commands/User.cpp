#include "User.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

// Default constructor
User::User(void) {}

User::User(const std::string& username, const std::string& realname): _username(username), _realname(realname) {} // NOLINT(bugprone-easily-swappable-parameters)
// Copy constructor
User::User(const User &other) {(void) other;}

// Assignment operator overload
User &User::operator=(const User &other) { (void) other; return (*this);}

// Destructor
User::~User(void) {}

void User::execute(Server& server, Client& client) {
	(void)server;
	client.setUserName(_username);
	client.setRealName(_realname);
	if (!client.getNickName().empty() && client.getStatus() == REGISTERED) {
		LOG_CMD.info("001 RPL_WELCOME");
	} else {
		LOG_CMD.info("??? RPL_USER");
	}

}

int User::checkArgs(Server& server, Client& client, std::string& params)
{
	(void)server;
	std::istringstream iss(params);
	std::string username, hostname, servername, realname;

	iss >> username;
	if (username.empty()) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}
	iss >> hostname;
	if (hostname.empty()) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}
	iss >> servername;
	if (servername.empty()) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}
	iss >> realname;
	if (realname.empty() || realname[0] != ':' || realname.length() < 2) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}

	// trim white space around realname; return 461 if only space in realname
	size_t start = realname.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);  
	}
    size_t end = realname.find_last_not_of(" \t\r\n");
    realname = realname.substr(start, end - start + 1);

	if (!client.getUserName().empty() && !client.getNickName().empty()) {
		LOG_CMD.error("462 ERR_ALREADYREGISTRED");
		return (ERR_ALREADYREGISTERED);
	}
	params = username + " " + realname;
	return (RPL_SUCCESS);
}

