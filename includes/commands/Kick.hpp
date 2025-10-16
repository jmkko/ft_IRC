/**
 * @file Kick.hpp
 * @brief Implements IRC KICK command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef KICK_HPP
#define KICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;

/**
 * @class Kick
 * @brief Handler for IRC KICK
 * @details This command is used to force a member to leave channel
 */
class Kick : public ICommand
{
  public:
    /**
     * @brief Construct a new Kick object
     *
     * @param args
     */
    Kick(const std::vector<std::string>& args);
    /**
     * @brief Destroy the Kick object
     *
     */
    ~Kick();

    /**
     * @brief Force part a channel
     * @details
     * - loop over channels :
     *   - check for invalid channel name -> ERR_BADCHANMASK
     *   - check for existing channel -> ERR_NOSUCHCHANNEL
     *   - check for op privilege -> ERR_CHANOPRIVSNEEDED
     *   - inner loop over channel members :
     *      - check for user presence -> ERR_USERNOTINCHANNEL
     *      - execute : remove - broadcast + direct notice
     * @param server
     * @param client
     * @warning can send ERR_BADCHANMASK, ERR_NOSUCHCHANNEL, ERR_CHANOPRIVSNEEDED, ERR_USERNOTINCHANNEL
     */
    void execute(Server& server, Client& client);
    /**
     * @brief checks syntaxic validity of args
     * @details cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.8)
     * - one or many channels, comma separated
     * - one or many users, comma separated
     * @param server
     * @param client
     * @param args should match `<channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]`
     * @return #ReplyCode to be used in #CommandFactory
     */
    static ReplyCode check_args(Server& server, Client& client, std::vector<std::string>& args);

  private:
    std::vector<std::string> _args;

    Kick();
    Kick(const Kick& other);
    Kick& operator=(const Kick& other);
};

#endif
