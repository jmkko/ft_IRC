#ifndef WHO_HPP
#define WHO_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <sstream>
#include <string>
#include <vector>

class Client;
class Server;

class Who : public ICommand
{
  public:
    Who(const std::string& params);
    ~Who();
    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _params;
    Who();
    Who(const Who& other);
    Who&                 operator=(const Who& other);
    std::vector<Client*> _find_all_clients_by_pattern(const std::set<Client*>& members, const std::string& pat);
    std::string          _who_msg(Client* client, Channel* channel, Server& server);
};

#endif
