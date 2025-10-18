#ifndef KICK_HPP
#define KICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;

class Kick : public ICommand
{
  public:
    Kick(std::string& params);
    ~Kick();

    void	     		execute(Server& server, Client& client);
    static ReplyCode	check_args(Server& server, Client& client, std::string& params);
    static bool				check_syntax(Server& server, Client& client, std::string& params);


  private:
	std::vector<std::string> _channelsNames;
	std::vector<std::string> _usersNames;
	std::string				_msg;


    Kick();
    Kick(const Kick& other);
    Kick& operator=(const Kick& other);
};

#endif
