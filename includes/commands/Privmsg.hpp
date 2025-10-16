/**
 * @file Privmsg.hpp
 * @brief Implements IRC PRIVMSG command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP
#include "Channel.hpp"
#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <iostream>
#include <string>

/**
 * @class Privmsg
 * @brief Handler for IRC PRIVMSG
 * @details This command is used to send
 * - direct messages to #Client
 * - messages to channels the sender belongs to
 */
class Privmsg : public ICommand
{
  public:
    /**
     * @brief Construct a new Privmsg:: Privmsg object
     *
     * @param params
     */
    Privmsg(const std::string& params);

    /**
     * @brief Destroy the Privmsg:: Privmsg object
     *
     */
    ~Privmsg();

    /**
     * @brief check syntaxic validity of params
     * @details should match [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.3.1)
     * syntax rules:
     * - trailing message (after `:`) should not be empty
     * - at least one target (#Channel or #Client sender)
     * - at max MAX_TARGET (defined in @ref irc.conf)
     * other rules:
     * - targets should be existing nicks or channels
     * @param server @ref Server
     * @param client sender @ref Client
     * @param params string following the command name
     * @return @ref ReplyCode corresponding to RFC ERR replies or CORRECT_FORMAT if syntax is correct
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

    /**
     * @brief proceed to extra checks (presence of sender on target channel) and to message transfer
     * @details
     * - message is transferred to target #Client
     * - or broadcasted to #Channel for other members (sender excepted)
     * @pre Privmsg::check_args should have returned CORRECT_FORMAT
     * @param server @ref Server
     * @param client sender @ref Client
     */
    void execute(Server& server, Client& client);

  private:
    std::string           _params;
    std::string           _msg;
    std::vector<Channel*> _chans;
    std::vector<Client*>  _dests;

    Privmsg(void);
    Privmsg(const Privmsg& other);

    Privmsg& operator=(const Privmsg& other);

    /**
     * @brief util method to parse args into target clients, channels and message before execution
     *
     * @param server
     * @param params
     */
    void _build_args(Server& server, std::string& params);
    /**
     * @brief adds a client to the list of valid clients
     *
     * @param client
     */
    void _add_client(Client* client);
    /**
     * @brief adds a channel to the list of valid channels
     *
     * @param chan
     */
    void _add_channel(Channel* chan);
};

#endif
