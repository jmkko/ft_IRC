/**
 * @file Bot.hpp
 * @brief Bot class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef BOT_HPP
#define BOT_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "TcpSocket.hpp"

#include <string>

/**
 * @def BOT_PROCESS_TIME_MS
 * @brief time to process API calls
 *
 */
#define BOT_PROCESS_TIME_MS 200

/**
 * @brief Implement non RFC Bot command
 * @details responsabilities
 * - syntax checking
 * - building headers and calling Ollama API
 * - reading response and sending reply through #Privmsg
 * @class Bot
 */
class Bot : public ICommand
{

  public:
    static const std::string& cmdName;

    /**
     * @brief Construct a new Bot object
     *
     * @param params
     */
    Bot(std::string& params);
    /**
     * @brief Destroy the Bot object
     *
     */
    ~Bot();

    /**
     * @brief get and transfer Ollama reply
     * @details steps are :
     * - build generic prompt (expected format, ...)
     * - adapt it to subcommand
     * - send it
     * - read reply and trim quotes
     * - connect and register bot, make it join a channel
     * - transfer original prompt to channel
     * - broadcast bot response to channel as a privmsg
     * @warning case of an invite-only channel is not handled yet
     * @param s
     * @param c
     */
    void execute(Server& s, Client& c);

  private:
    std::string              _params;
    std::vector<std::string> _targets;
    std::string              _targetChannelName;
    std::vector<Channel*>    _targetChannels;
    std::vector<Client*>     _targetClients;
    std::string              _subcommand;
    std::string              _prompt;
    TcpSocket                _socket;

    Bot();
    Bot(const Bot& other);
    Bot& operator=(const Bot& other);

    /**
     * @brief checks argument validity
     * @details make syntaxic checks using #Parser
     * - command should match pattern `BOT #chan !subcommand :prompt`
     * with subcommand being one of
     * - !reply : for a direct reply
     * - !check : fact checking a statement
     * and makes validity checks
     * - channel should exist
     * - sender should be a channel member
     * @param s
     * @param c
     * @return true if args are valid
     * @return false otherwise
     * @warning can send ERR_NEEDMOREPARAMS, ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL, CUSTOMERR_WRONG_FORMAT
     */
    bool _check_args(Server& s, Client& c);

    /**
     * @brief connects a socket to the server
     * @details make socket non blocking, reusable before connecting to the server
     * uses select to monitor the socket
     * @param s
     * @param so
     * @return true if successfull
     * @return false otherwise
     */
    bool _connect_to_server(Server& s, TcpSocket& so);

    /**
     * @brief register to IRC server
     * @details send sequence PASS NICK USER
     * @remarks it is necessary to register bot as a Client in order to have it join and appear as a Channel member
     */
    bool _register_bot(Server& s, TcpSocket& so);
};

#endif