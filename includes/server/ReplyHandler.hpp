/**
 * @file ReplyHandler.hpp
 * @brief ReplyHandler class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "reply_codes.hpp"

#include <string>

#define LOWER_CUSTOM_CODE              100
#define UPPER_CUSTOM_CODE              299
#define LOWER_CUSTOM_NONNUMERICAL_CODE 800

class Server;
class Client;

/*
 * @brief **ReplyHandler** class for To facilitate client responses
 *
 * @remark
 * <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
 * <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
 * <command>  ::= <letter> { <letter> } | <number> <number> <number>
 * <SPACE>    ::= ' ' { ' ' }
 * <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
 *
 * <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
 *                or NUL or CR or LF, the first of which may not be ':'>
 * <trailing> ::= <Any, possibly *empty*, sequence of octets not including
 *                  NUL or CR or LF>
 *
 * <crlf>     ::= CR LF
 *
 *   1.    <SPACE> is consists only of SPACE character(s) (0x20).
 *         Specially notice that TABULATION, and all other control
 *         characters are considered NON-WHITE-SPACE.
 *
 *   2.    After extracting the parameter list, all parameters are equal,
 *         whether matched by <middle> or <trailing>. <Trailing> is just
 *         a syntactic trick to allow SPACE within parameter.
 *
 *   3.    The fact that CR and LF cannot appear in parameter strings is
 *         just artifact of the message framing. This might change later.
 *
 *   4.    The NUL character is not special in message framing, and
 *         basically could end up inside a parameter, but as it would
 *         cause extra complexities in normal C string handling. Therefore
 *         NUL is not allowed within messages.
 *
 *   5.    The last parameter may be an empty string.
 *
 *   6.    Use of the extended prefix (['!' <user> ] ['@' <host> ]) must
 *         not be used in server to server communications and is only
 *         intended for server to client messages in order to provide
 *         clients with more useful information about who a message is
 *         from without the need for additional queries.
 */
class ReplyHandler
{
  public:
    /**
     * @brief return client identifier formatted as nick!username@hostname
     *
     * @param client
     * @param nickname
     * @return formatted identifier
     */
    std::string get_user_id_of(Client& client);

    /**
     * @brief directs the message to the correct response method,
     * either to a response with ReplyCode or a Transfer response
     *
     * @param client who made the command
     * @param code Replycode
     * @param parameters of the command
     * @param sender if the message need a sender
     * @param trailing message of the Replycode
     * @return @see ReplyCode or Internal code
     */
    int process_response(Client&            client,
                         ReplyCode          code,
                         const std::string& parameters = "",
                         Client*            sender     = NULL,
                         const std::string& trailing   = "");

    /**
     * @brief message sequence to send to the client
     * when they have just registered
     *
     * @param server
     * @param client
     */
    void process_welcome(Server& server, Client& client);

    /**
     * @brief Singleton pattern, this function allows you to have
     * only one instance of this class
     *
     * @param s the Server
     * @return the only one instance of ReplyHandler
     */
    static ReplyHandler& get_instance(Server* s);

  private:
    Server* _server;

    ReplyHandler(Server* s);

    /**
     * @brief append the correct message to the Client buffer
     * and add POLLOUT event in the pollfd of the Client
     *
     * @param c the Client
     * @param msg string to add to the buffer
     */
    void _send_reply(Client& c, const std::string& msg);
};

#endif
