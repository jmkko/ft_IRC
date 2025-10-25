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
     * @param params should match should match `<msgtarget> <text to be sent>`
     */
    Privmsg(std::string& params);

    /**
     * @brief Destroy the Privmsg:: Privmsg object
     *
     */
    ~Privmsg();

    /**
     * @brief check validity and transfer message
     * @details should match [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.3.1)
     * - make syntaxic checks
     *   - trailing message (after `:`) should not be empty
     *   - at least one target (#Channel or #Client sender)
     *   - at max MAX_TARGET (defined in config)
     * - make other checks
     *   - targets should be existing nicks or channels
     *   - presence of sender on target channel
     * - message is transferred to target #Client or broadcasted to #Channel for other members (sender excepted)
     *
     * @param server @ref Server
     * @param client sender @ref Client
     *
     * @warning can send ERR_NEEDMOREPARAMS, ERR_NORECIPIENT, ERR_NOTEXTTOSEND, ERR_TOOMANYTARGETS,
     * ERR_NOSUCHCHANNEL, ERR_NOSUCHNICK
     */
    void execute(Server& server, Client& client);

  private:
    std::vector<std::string> _targets;
    std::string              _message;
    std::string              _params;
    std::string              _msg;
    std::vector<Channel*>    _chans;
    std::vector<Client*>     _dests;

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
