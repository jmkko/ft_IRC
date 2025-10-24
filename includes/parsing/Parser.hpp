#ifndef PARSER_HPP
#define PARSER_HPP
#include "Channel.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <vector>

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
    std::string from_arg(std::string& params);
    std::string from_trailing(std::string& params);

    std::vector<std::string>           to_vector(std::string& params);
    std::map<std::string, std::string> to_map(std::string& key, std::string& value);

    bool correct_password(std::string& password);
    bool correct_nickname(std::string& nickname);
    bool correct_target(std::string& target);
    bool correct_channel(std::string& name);
    bool correct_key(std::string& key);

    Parser& is_such_nick(std::string& nickname, bool failCommandIfFalse = false);
    Parser& is_such_channel(std::string& channelName, bool failCommandIfFalse = false);
    Parser& has_no_more_than(std::vector<std::string>& vector, std::size_t max, bool failCommandIfFalse = true);
    Parser& is_channel_member(std::string& channelName, const std::string& nickname, bool failCommandIfFalse = true);
    Parser& is_valid_bot_prompt(const std::string& prompt, const std::string& commandName, bool failCommandIfFalse = true);
    Parser& is_valid_bot_subcommand(const std::string& subcommand, const std::string& cmdName, bool failCommandIfFalse = true);
    Parser& is_not_empty_arg(const std::string& arg, const std::string& commandName, bool failCommandIfFalse = true);

    size_t count_params(const std::string& argument);
    bool   response(ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    bool   response(Client* dest, ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    bool response(Client* dest, Client* author, ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    ReplyHandler* rh;

    bool has_passed_checks() const;

  private:
    Parser& operator=(const Parser& other);
    Parser(const Parser& other);

    Server* _server;
    Client* _client;
    bool    _isValidCommand;
};

#endif
