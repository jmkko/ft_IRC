/**
 * @file User.hpp
 * @brief Implementats IRC USER command
 * @version 0.1
 * @date 2025-10-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef USER_HPP
#define USER_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

/**
 * @brief Handler for IRC USER command
 * @class User
 */
class User : public ICommand
{
  public:
    User(std::string& params);
    ~User();

    /**
     * @brief register a username and realname
     * @details trim whitespaces around realname
     * @remark If previous PASS and NICK have been successfull, the server will sends Replies 001 to 004 to a user upon
     * successful registration
     * @section ExampleUser Example of reply sequence
     * @code{.unparsed}
     *  - :irc.example.com 001 :Welcome to the Internet Relay Network <nick>!<user>@<host>
     *  - :irc.example.com 002 :Your host is <servername>, running version <ver>
     *  - :irc.example.com 003 :This server was created <date>
     *  - :irc.example.com 004 :<servername> <version> <available user modes>
     * @endcode
     * @param server not used
     * @param client
     * @warning can send ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED
     *
     */
    void execute(Server& server, Client& client);

    /**
     * @brief Check User commands args
     * @details
     * cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.1.3)
     * @param server not used
     * @param client
     * @param params should mathc `<user> <mode> <unused> <realname>`
     * @remark mode and unused are always 0 * for this project
     * @remark grammar rule
     * user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
     *                 ; any octet except NUL, CR, LF, " " and "@"
     * @return @ref ReplyCode
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    std::string _username;
    std::string _mode;
    std::string _unused;
    std::string _realname;
    User(const User& other);
    User(void);
    User& operator=(const User& other);
};

#endif
