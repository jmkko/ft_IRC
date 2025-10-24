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
 * @details responses should match [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1) 
 *  * grammar of message is  `<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>`
 * meaning that a message is made of
 * - for numerical responses
 *   - prefix, usually the server nama
 *   - numeric code
 *   - args
 *   - trailing message (optional)
 * - for other responses
 *   - prefix, usually the full userhost of the sender
 *   - command name
 *   - args
 *   - trailing message (optional)
 *  
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
