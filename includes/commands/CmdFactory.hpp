#ifndef CMDFACTORY_HPP
#define CMDFACTORY_HPP

#include <string>

class Server;
class Client;
class ICommand;

class CmdFactory
{
  public:
    CmdFactory(void);
    CmdFactory(Server& server, Client& client);
    CmdFactory(const CmdFactory& other);
    CmdFactory& operator=(const CmdFactory& other);
    ~CmdFactory();

    bool      check_in(Client& client, std::string& command);
    ICommand* make_command(Server& server, Client& client, std::string& params);
    ICommand* user_cmd(Server& server, Client& client, std::string& params);
    ICommand* pass_cmd(Server& server, Client& client, std::string& params);
    ICommand* nick_cmd(Server& server, Client& client, std::string& params);
    ICommand* kick_cmd(Server& server, Client& client, std::string& params);
    ICommand* quit_cmd(Server& server, Client& client, std::string& params);
    ICommand* join_cmd(Server& server, Client& client, std::string& params);
    ICommand* part_cmd(Server& server, Client& client, std::string& params);
    ICommand* mode_cmd(Server& server, Client& client, std::string& params);
    ICommand* privmsg_cmd(Server& server, Client& client, std::string& params);
    ICommand* oper_cmd(Server& server, Client& client, std::string& params);
    ICommand* invite_cmd(Server& server, Client& client, std::string& params);
    ICommand* who_cmd(Server& server, Client& client, std::string& params);
    ICommand* motd_cmd(Server& server, Client& client, std::string& params);
    ICommand* ping_cmd(Server& server, Client& client, std::string& params);
};

#endif
