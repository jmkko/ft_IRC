/**
 * @file Join.hpp
 * @brief Implement IRC JOIN Command
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef JOIN_HPP
#define JOIN_HPP

#include "Client.hpp"
#include "ICommand.hpp"

#include <string>

class Client;
class Server;
class Parser;

/**
 * @class Join
 * @brief Handler for IRC JOIN
 * @details This command is used to make a client join a channel
 */
class Join : public ICommand
{
  public:
    /**
     * @brief Construct a new Join object
     * @details parses and recreate the channels list.
     * Each name of channel is mapped its corresponding key.
     *
     * @param params
     */
    Join(std::string& params);

    /**
     * @brief Destroy the Join object
     *
     */
    ~Join();

    /**
     * @brief Allows a client to join a channel or create it if it does not exist
     * @details check validity accordind to the RFC_2812
     * - should match pattern `( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"`
     * cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.1)
     * @section Example Reply Sequence
     * @code{.unparsed}
     * example of reply sequence
     *	:user1!~username@host JOIN :#chan1 123
     *	:irc.example.com MODE #chan1 +o user1
     *	:irc.example.com MODE #chan1 +k 123
     *	:irc.example.com 331 user1 #chan1 :No topic is set
     *	:irc.example.com 353 user1 = #chan1 :@user1 user2 user3 user4 user5
     *	:irc.example.com 353 user1 = #chan1 :user6 @user1 user7 user8 user9
     *	:irc.example.com 366 user1 #chan1 :End of NAMES listV
     * @endcode
     * @param server
     * @param client
     * @warning can send ERR_NEEDMOREPARAMS, ERR_BADCHANMASK, ERR_BADCHANNELKEY
     */
    void execute(Server& server, Client& client);

  private:
    std::map<std::string, std::string> _chans;

    /**
     * @brief send user list
     * @details through #ReplyHandler included in #Parser
     * @param p
     * @param channel
     */
    void _send_list_of_names(Parser& p, Channel& channel);

    /**
     * @brief send RPL_TOPIC or RPL_NOTOPIC
     * @details through #ReplyHandler included in #Parser
     * @param p
     * @param channel
     */
    void _display_topic(Parser& p, Channel& channel);

    Join();
    Join(const Join& other);
    Join& operator=(const Join& other);
};

#endif
