/**
 * @file Parser.hpp
 * @brief Parser class
 * @version 0.1
 * @date 2025-10-25
 *
 * @copyright Copyright (c) 2025
 *
 */
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

/**
 * @brief Utility class to simplify command parsing
 * @class Parser
 *
 */
class Parser
{
  public:
    Parser(void);
    ~Parser();
    Parser(Server& server, Client& client);

    /**
     * @brief Get the server object
     *
     * @return Server*
     */
    Server* get_server();
    /**
     * @brief Get the client associated with Parser
     *
     * @return Client*
     */
    Client* get_client();

    typedef bool (Parser::*Checker)(std::string&);
    /**
     * @brief utility method to parse a message
     * @details can be called multiple times to return different parts of a message
     * @param params
     * @param function
     * @return std::string
     */
    std::string format_parameter(std::string& params, Checker function);
    /**
     * @brief get an argument
     *
     * @param params
     * @return std::string
     */
    std::string from_arg(std::string& params);
    /**
     * @brief get trailing part of a message
     *
     * @param params
     * @return std::string
     */
    std::string from_trailing(std::string& params);

    /**
     * @brief get remaining args as a string
     *
     * @param params
     * @return std::string
     */
    std::string from_remaining_args(std::string& params);

    /**
     * @brief converts args to a vector of string
     *
     * @param params
     * @return std::vector<std::string>
     */
    std::vector<std::string> to_vector(std::string& params);
    /**
     * @brief convert matching key and values arguments to a map
     * @details `#chan1,#chan2 key1,key2` will be mapped to key values `#chan1,key1` ..
     * @param key
     * @param value
     * @return std::map<std::string, std::string>
     */
    std::map<std::string, std::string> to_map(std::string& key, std::string& value);

    /**
     * @brief check password validity
     *
     * @param password
     * @return true if password is correct
     * @return false
     */
    bool correct_password(std::string& password);
    /**
     * @brief check nickname validity
     *
     * @param nickname
     * @return true
     * @return false
     */
    bool correct_nickname(std::string& nickname);
    /**
     * @brief check target validity
     *
     * @param target
     * @return true
     * @return false
     */
    bool correct_target(std::string& target);
    /**
     * @brief check channel validity
     *
     * @param name
     * @return true
     * @return false
     */
    bool correct_channel(std::string& name);
    /**
     * @brief check key validity
     *
     * @param key
     * @return true
     * @return false
     */
    bool correct_key(std::string& key);

    /**
     * @brief check whether nickname is registered on server
     *
     * @param nickname
     * @param failCommandIfFalse
     * @warning send ERR_NOSUCHNICK in case of error
     * @return Parser&
     */
    Parser& is_such_nick(std::string& nickname, bool failCommandIfFalse = false);
    /**
     * @brief check whether channel is registered
     *
     * @param channelName
     * @param failCommandIfFalse
     * @warning send ERR_NOSUCHCHANNEL in case of error
     * @return Parser&
     */
    Parser& is_such_channel(std::string& channelName, bool failCommandIfFalse = false);
    /**
     * @brief check that array of args has no more than provided max
     *
     * @param vector
     * @param max
     * @param failCommandIfFalse
     * @return Parser&
     */
    Parser& has_no_more_than(std::vector<std::string>& vector, std::size_t max, bool failCommandIfFalse = true);
    /**
     * @brief check if sender is on channel
     *
     * @param channelName
     * @param nickname
     * @param failCommandIfFalse
     * @warning send ERR_NOTONCHANNEL in case of error
     * @return Parser&
     */
    Parser& is_channel_member(std::string& channelName, const std::string& nickname, bool failCommandIfFalse = true);
    /**
     * @brief check whether prompt is valid
     *
     * @param prompt
     * @param commandName
     * @param failCommandIfFalse
     * @return Parser&
     */
    Parser& is_valid_bot_prompt(const std::string& prompt, const std::string& commandName, bool failCommandIfFalse = true);
    /**
     * @brief check whether subcommand is handled by server
     *
     * @param subcommand
     * @param cmdName
     * @param failCommandIfFalse
     * @return Parser&
     */
    Parser& is_valid_bot_subcommand(const std::string& subcommand, const std::string& cmdName, bool failCommandIfFalse = true);
    /**
     * @brief check if arg is not empty
     *
     * @param arg
     * @param commandName
     * @param failCommandIfFalse
     * @return Parser&
     */
    Parser& is_not_empty_arg(const std::string& arg, const std::string& commandName, bool failCommandIfFalse = true);

    /**
     * @brief count number of params
     *
     * @param argument
     * @return size_t
     */
    size_t count_params(const std::string& argument);
    /**
     * @brief send response through #ReplyHandler
     *
     * @param code
     * @param params
     * @param trailing
     * @return true
     * @return false
     */
    bool response(ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    /**
     * @brief send response through #ReplyHandler
     *
     * @param dest
     * @param code
     * @param params
     * @param trailing
     * @return true
     * @return false
     */
    bool response(Client* dest, ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    /**
     * @brief send response through #ReplyHandler
     *
     * @param dest
     * @param author
     * @param code
     * @param params
     * @param trailing
     * @return true
     * @return false
     */
    bool response(Client* dest, Client* author, ReplyCode code, const std::string& params = "", const std::string& trailing = "");
    ReplyHandler* rh;

    /**
     * @brief check whether consecutive checks have passed
     *
     * @return true
     * @return false
     */
    bool has_passed_checks() const;

  private:
    Parser& operator=(const Parser& other);
    Parser(const Parser& other);

    Server* _server;
    Client* _client;
    bool    _isValidCommand;
};

#endif
