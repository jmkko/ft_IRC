#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <set>
#include <string>
#include <vector>

class Client;
class Server;

// static const std::string& authorizedModes   = "kilot";
// static const std::string& modesRequiringArg = "klo";
// static const std::string& authorizedOps     = "+-";
// static const std::string& digits            = "0123456789";

class Mode : public ICommand
{
  public:
    Mode(const std::string& params);
    ~Mode();

    void             execute(Server& server, Client& client);
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    std::string _params;

    Mode();
    Mode(const Mode& other);
    Mode& operator=(const Mode& other);
};

#endif
