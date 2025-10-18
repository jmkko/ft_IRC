#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

class Nick : public ICommand
{
  public:
    explicit Nick(const std::string& nickname);
    ~Nick(); // NOLINT

    /**
     * @brief set nickname or change nickname
     *
     * @details this command is done after USER and it is the first use
     * - send welcome message sequence to client
     *  - 001    RPL_WELCOME
     *          "Welcome to the Internet Relay Network
     *          <nick>!<user>@<host>"
     *  - 002    RPL_YOURHOST
     *          "Your host is <servername>, running version <ver>"
     *  - 003    RPL_CREATED
     *          "This server was created <date>"
     *  - 004    RPL_MYINFO
     *          "<servername> <version> <available user modes>
     *          <available channel modes>"
     *        - The server sends Replies 001 to 004 to a user upon
     *          successful registration.
     *
     * @param server not used
     * @param client
     */
    void execute(Server& s, Client& c); // NOLINT
    /**
     * @brief check the args of command NICK
     *
     * @param server
     * @param client not used
     * @param params should match `<nickname>`
     * @remark grammar rule
     * nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
     *
     *
     * @return @ref ReplyCode
     */
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _nickname;

    Nick();
    Nick(const Nick& other);

    Nick& operator=(const Nick& other);
};

#endif
