#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "reply_codes.hpp"

#include <string>

#define LOWER_CUSTOM_CODE 200
#define UPPER_CUSTOM_CODE 299
#define LOWER_CUSTOM_NONNUMERICAL_CODE 800

class Server;
class Client;

class ReplyHandler
{
  public:
    std::string get_user_id_of(Client& client);
    std::string select_response(Client& client, ReplyCode code, const std::string& parameter, Client* sender = NULL);
    int         process_response(Client& client, ReplyCode code, const std::string& parameters = "",  Client* sender = NULL, const std::string& trailing = "");

    // int         process_code_response(Client& client, ReplyCode code, const std::string& parameters);
    // int         process_nocode_response(Client& client, const std::string& parameters, Client* sender = NULL);

    static ReplyHandler& get_instance(Server* s);

  private:
    Server* _server;

    ReplyHandler(Server* s);

    void _send_reply(Client& c, const std::string& msg);
};

#endif
