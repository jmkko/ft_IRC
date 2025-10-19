#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include <map>

#include "reply_codes.hpp"
#include "ReplyHandler.hpp"
class Server;
class Client;
class ReplyHandler;

#define INVALID_PARAM "*"

class Parser
{
  public:
    Parser(void);
    ~Parser();
    Parser(Server& server, Client& client);

	Server* get_server();
	Client* get_client();

	typedef bool (Parser::*Checker)(std::string&);
    std::string format_parameter(std::string& params, Checker function);

	std::vector<std::string> to_vector(std::string& params);
	std::map<std::string, std::string> to_map(std::string& key, std::string& value);

	bool correct_password(std::string& password);
    bool correct_nickname(std::string& nickname);
	bool correct_target(std::string& target);
    bool correct_channel(std::string& name);
    bool correct_key(std::string& key);

	size_t count_params(const std::string& argument);
	bool response(ReplyCode code, const std::string& params  = "", const std::string& trailing = "");
	bool response(Client* dest, ReplyCode code, const std::string& params  = "", const std::string& trailing = "");
	bool response(Client* dest, Client* author, ReplyCode code, const std::string& params  = "", const std::string& trailing = "");
    ReplyHandler* rh;

  private:
    Parser& operator=(const Parser& other);
    Parser(const Parser& other);

    Server* _server;
    Client* _client;
};

#endif
