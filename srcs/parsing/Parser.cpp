#include "Parser.hpp"

#include "Config.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "parsing/Parser.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstddef>

// Default constructor
Parser::Parser(void) : rh(NULL), _server(NULL), _client(NULL), _isValidCommand(true) {}

Parser::Parser(Server& server, Client& client) :
    rh(&ReplyHandler::get_instance(&server)), _server(&server), _client(&client), _isValidCommand(true)
{
}

// Copy constructor
Parser::Parser(const Parser& other) :
    rh(other.rh), _server(other._server), _client(other._client), _isValidCommand(other._isValidCommand)
{
}

// Assignment operator overload
Parser& Parser::operator=(const Parser& other)
{
    if (this != &other) {
        rh              = other.rh;
        _client         = other._client;
        _server         = other._server;
        _isValidCommand = other._isValidCommand;
    }
    return (*this);
}

// Destructor
Parser::~Parser(void) {}

bool Parser::response(ReplyCode code, const std::string& params, const std::string& trailing)
{
    return response(NULL, NULL, code, params, trailing);
}

bool Parser::response(Client* dest, ReplyCode code, const std::string& params, const std::string& trailing)
{
    return response(dest, NULL, code, params, trailing);
}

bool Parser::response(Client* dest, Client* author, ReplyCode code, const std::string& params, const std::string& trailing)
{
    if (code != CORRECT_FORMAT) {
        if (rh && _client) {
            if (dest && author) {
                rh->process_response(*dest, code, params, author, trailing);
            } else if (dest) {
                rh->process_response(*dest, code, params, NULL, trailing);
            } else {
                rh->process_response(*_client, code, params, NULL, trailing);
            }
        }
        return (false);
    }
    return (true);
}

// Not in use //
bool Parser::correct_password(std::string& password)
{

    if (_server->get_password() != password) {
        return response(ERR_PASSWDMISMATCH);
    } else if (_client->is_registered()) {
        return response(ERR_ALREADYREGISTRED);
    }
    return (true);
}

bool Parser::correct_channel(std::string& name)
{
    if (name.empty()) {
        return (response(ERR_BADCHANMASK));
    }
    for (std::string::const_iterator it = name.begin(); it != name.end(); ++it) {
        unsigned char c = *it;
        if (c > 0xFF || Utils::is_char_of(c, std::string(FORBIDEN_CHAR_CHAN_NAME, 7))) { // NOLINT
            return response(ERR_BADCHANMASK, name);
        }
    }
    if (!Utils::is_char_of(static_cast<unsigned char>(name[0]), "#&+!")) {
        return response(ERR_BADCHANMASK, name);
    }
    if (name.length() <= 1 || name.length() >= ircConfig.get_chan_name_max_len()) {
        return response(ERR_BADCHANMASK, name);
    }

    return true;
}

// not in use //
bool Parser::correct_target(std::string& target)
{
    Parser parser;

    bool validChannel  = parser.correct_channel(target);
    bool validNickname = parser.correct_nickname(target);

    if (!validChannel && !validNickname)
        return response(ERR_NOTONCHANNEL);

    return CORRECT_FORMAT;
}

/**
 * @brief check if the key is valid -- no commas or spaces allowed
 *
 * @param key
 * @return true or false
 */
bool Parser::correct_key(std::string& key)
{
    if (key.empty()) {
        return true;
    } else if (key.size() > CHAN_KEY_MAX_LEN) {
        return response(ERR_BADCHANNELKEY);
    }
    for (std::string::const_iterator it = key.begin(); it != key.end(); ++it) {
        unsigned char c = *it;
        if (c > 0x07F || Utils::is_char_of(c, std::string(FORBIDEN_CHAR_CHAN_KEY, 7))) { // NOLINT
            return response(ERR_BADCHANNELKEY);
        }
    }
    return true;
}

// for a new nickname
bool Parser::correct_nickname(std::string& nickname)
{
    bool 			invalidChar = false;

    if (nickname.empty()) {
        return response(ERR_NONICKNAMEGIVEN);
    }
    invalidChar = std::count_if(nickname.begin(), nickname.end(), Utils::is_invalid_char_nick);
    if (invalidChar == true || std::isdigit(nickname[0])) {
        return response(ERR_ERRONEUSNICKNAME, nickname);
    } else if (nickname.length() > ircConfig.get_nickname_max_len()) {
        nickname = nickname.substr(0, ircConfig.get_nickname_max_len());
    }
    if (_server && _server->find_client_by_nickname(nickname)) {
        return response(ERR_NICKNAMEINUSE, nickname);
    }
    return true;
}

// checking if nick exists
Parser& Parser::is_such_nick(std::string& nickname, bool failCommandIfFalse)
{
    if (!_server->find_client_by_nickname(nickname)) {
        response(ERR_NOSUCHNICK, nickname);
        if (failCommandIfFalse)
            _isValidCommand = false;
    }
    return *this;
}

// checking if channel exists
Parser& Parser::is_such_channel(std::string& channelName, bool failCommandIfFalse)
{
    if (_isValidCommand && !_server->find_channel_by_name(channelName)) {
        response(ERR_NOSUCHCHANNEL, channelName);
        if (failCommandIfFalse)
            _isValidCommand = false;
    }
    return *this;
}

Parser& Parser::is_channel_member(std::string& channelName, const std::string& nickname, bool failCommandIfFalse)
{
    bool passedCheck = false;
    LOG_DV_CMD(_isValidCommand);

    if (_isValidCommand) {
        Client* client = _server->find_client_by_nickname(nickname);
        LOG_D_CMD("found nick", client->get_nickname());

        if (client) {
            Channel* c = _server->find_channel_by_name(channelName);
            LOG_DV_CMD(c->get_nb_members());
            if (c->is_member(*client) == false) {
                LOG_W_CMD("not member", client->get_nickname());
                response(ERR_NOTONCHANNEL, c->get_name());
            } else
                passedCheck = true;
        }
    }
    if (!passedCheck && failCommandIfFalse)
        _isValidCommand = false;
    return *this;
}

Parser& Parser::has_no_more_than(std::vector<std::string>& vector, size_t max, bool failCommandIfFalse)
{
    if (_isValidCommand && vector.size() <= max) {
        response(ERR_TOOMANYTARGETS, vector.back());
        if (failCommandIfFalse)
            _isValidCommand = false;
    }
    return *this;
}

Parser& Parser::is_not_empty_arg(const std::string& arg, const std::string& commandName, bool failCommandIfFalse)
{
    if (_isValidCommand && arg.empty()) {
        response(ERR_NEEDMOREPARAMS, commandName);
        if (failCommandIfFalse)
            _isValidCommand = false;
    }
    return *this;
}

Parser& Parser::is_valid_bot_subcommand(const std::string& subcommand, const std::string& cmdName, bool failCommandIfFalse)
{
    bool passedCheck = false;
    if (_isValidCommand) {
        this->is_not_empty_arg(subcommand, cmdName);
        std::string availableSubcommands[NB_AVAILABLE_BOT_SUBCMD] = {"!reply", "!check"};
        for (int i = 0; i < NB_AVAILABLE_BOT_SUBCMD; ++i) {
            if (availableSubcommands[i] == subcommand)
                passedCheck = true;
        }
        if (!passedCheck)
            response(CUSTOMERR_WRONG_FORMAT, subcommand);
    }
    if (failCommandIfFalse & !passedCheck)
        _isValidCommand = false;
    return *this;
}

Parser& Parser::is_valid_bot_prompt(const std::string& prompt, const std::string& commandName, bool failCommandIfFalse)
{
    bool passedCheck = true;
    if (_isValidCommand) {
        this->is_not_empty_arg(prompt, commandName);
        for (std::string::const_iterator it = prompt.begin(); it != prompt.end(); ++it) {
            unsigned char c = *it;
            if (Utils::is_char_of(c, std::string(FORBIDEN_CHAR_BOT_PROMPT))) {
                response(CUSTOMERR_WRONG_FORMAT, prompt);
                passedCheck = false;
            }
        }
    }
    if (failCommandIfFalse & !passedCheck)
        _isValidCommand = false;
    return *this;
}

std::vector<std::string> Parser::to_vector(std::string& params)
{
    std::istringstream       iss(params);
    std::string              str;
    std::vector<std::string> result;

    while (std::getline(iss, str, ',')) {
        result.push_back(str);
    }
    return (result);
}

std::map<std::string, std::string> Parser::to_map(std::string& keys, std::string& values)
{
    std::istringstream                 issKeys(keys);
    std::istringstream                 issValues(values);
    std::string                        key, value;
    std::map<std::string, std::string> result;

    while (std::getline(issKeys, key, ',')) {
        std::getline(issValues, value, ',');
        result[key] = value;
    }
    return (result);
}
// return an argument, as a formated string of parameters args = param1,param2,param3
// can apply a function on every param (but not in use for now)
// if arguments start by ':' takes the all line
std::string Parser::format_parameter(std::string& params, Checker function)
{
    std::string        arguments, list, trailing;
    std::string        currentParam, comma;
    std::istringstream iss(params);

    iss >> arguments;
    if (!arguments.empty() && arguments[0] == ':') {
        arguments.erase(0, 1);
        std::getline(iss, trailing);
        return (arguments + trailing);
    }
    std::istringstream issParams(arguments);
    while (std::getline(issParams, currentParam, ',')) {
        if (function && (this->*function)(currentParam) == false) {
            continue;
        }
        list += comma + currentParam;
        comma = ",";
    }
    if (!std::getline(iss, params)) {
        params.clear();
    };
    return list;
}

std::string Parser::from_arg(std::string& params)
{
    std::string        argument;
    std::istringstream iss(params);

    iss >> argument;
    params.erase(0, argument.size());

    return argument;
}

std::string Parser::from_trailing(std::string& params)
{
    std::string        trailing;
    std::istringstream iss(params);

    std::getline(iss, trailing);
    if (trailing.empty())
        return trailing;
    size_t posColon = trailing.find(" :");
    params.erase(0, trailing.size());

    if (posColon != std::string::npos && posColon + 1 < trailing.size()) {
        trailing = trailing.substr(posColon + 2);
    } else {
        return "";
    }
    return trailing;
}

bool Parser::has_passed_checks() const { return _isValidCommand; }