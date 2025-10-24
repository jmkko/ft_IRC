/**
 * @file Nick.hpp
 * @author your name (you@domain.com)
 * @brief Implements IRC NICK command
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

/**
 * @class Nick
 * @brief Handler for IRC NICK
 * @details this command is used to set or change a client's nickname
 */
class Nick : public ICommand
{
  public:
    /**
     * @brief Construct a new Nick object
     * @details uses #Parser
     * @param params
     */
    explicit Nick(std::string& params);

    /**
     * @brief Destroy the Nick object
     *
     */
    ~Nick();

    /**
     * @brief set nickname or change nickname
     *
     * @remark If previous PASS and USER have been successfull, the server will sends Replies 001 to 004 to a user upon
     * successful registration, although standard order is PASS NICK USER
     * @section ExampleNick reply sequence
     * @code{.unparsed}
     *  - :irc.example.com 001 :Welcome to the Internet Relay Network <nick>!<user>@<host>
     *  - :irc.example.com 002 :Your host is <servername>, running version <ver>
     *  - :irc.example.com 003 :This server was created <date>
     *  - :irc.example.com 004 :<servername> <version> <available user modes>
     * @endcode
     * @param server not used
     * @param client
     */
    void execute(Server& server, Client& client);

    /**
     * @brief check the args of command NICK
     * @details
     * cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.1.2)
     * @param server
     * @param client not used
     * @param params should match grammar rule
     * nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
     * @return @ref ReplyCode
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    std::string _nickname;

    Nick();
    Nick(const Nick& other);

    Nick& operator=(const Nick& other);
};

#endif
