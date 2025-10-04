#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>
#include <set>

class Client;
class Server;

static const std::string& authorizedModes = "kilot";

class Mode : public ICommand
{
  public:
    Mode(const std::vector<std::string>& args);
    ~Mode();

    void	     		execute(Server& server, Client& client);
    static ReplyCode	check_args(Server& server, Client& client, std::vector<std::string>& args);

  private:
  	std::vector<std::string> _args;
	std::string				_channel;
	char					_oper;


  	Mode();
    Mode(const Mode& other);
    Mode& operator=(const Mode& other);
};

#endif