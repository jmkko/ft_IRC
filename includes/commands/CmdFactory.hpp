#ifndef CMDFACTORY_HPP
#define CMDFACTORY_HPP
#include "Join.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Pass.hpp"
#include "User.hpp"
#include "consts.hpp"
#include <iostream>
#include <sstream>

class Server;
class Client;

class CmdFactory
{
  public:
	CmdFactory(void);
	CmdFactory(Server& server, Client& client);
	CmdFactory(const CmdFactory& other);
	CmdFactory& operator=(const CmdFactory& other);
	~CmdFactory();

	ICommand* make_command(Server& server, Client& client, std::string& params);
	ICommand* user_cmd(Server& server, Client& client, std::string& params);
	ICommand* pass_cmd(Server& server, Client& client, std::string& params);
	ICommand* nick_cmd(Server& server, Client& client, std::string& params);
	ICommand* quit_cmd(Server& server, Client& client, std::string& params);
	ICommand* join_cmd(Server& server, Client& client, std::string& params);
	ICommand* part_cmd(Server& server, Client& client, std::string& params);
	ICommand* mode_cmd(Server& server, Client& client, std::string& params);
	ICommand* oper_cmd(Server& server, Client& client, std::string& params);
	ICommand* invite_cmd(Server& server, Client& client, std::string& params);
};

#endif
