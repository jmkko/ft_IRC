#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;

class Kick : public ICommand
{
  public:
    Kick(const std::vector<std::string>& args);
    ~Kick();

    void	     		execute(Server& server, Client& client);
    static ReplyCode	check_args(Server& server, Client& client, std::vector<std::string>& args);

  private:
  	std::vector<std::string> _args;
	// std::vector<std::string> _channelsNames;
	// std::vector<std::string> _userNames;

  	Kick();
    Kick(const Kick& other);
    Kick& operator=(const Kick& other);
};

#endif