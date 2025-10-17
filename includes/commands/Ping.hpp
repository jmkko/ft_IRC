/**
 * @file Ping.hpp
 * @brief Implements IRC PING command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef PING_H
#define PING_H

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

/**
 * @class Ping
 * @brief Handler for PING
 */
class Ping : public ICommand
{
  public:
    /**
     * @brief Construct a new Ping object
     *
     * @param params
     */
    Ping(const std::string& params);
    virtual ~Ping();
    /**
     * @brief sends a PONG message with token provided by PING sender
     *
     * @param s server
     * @param c client
     */
    void execute(Server& server, Client& client);
    /**
     * @brief checks that there is an origin token
     *
     * @details cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.7.2)
     * @param server
     * @param client
     * @param params
     * @return ReplyCode
     * @warning return ERR_NOORIGIN if no token provided
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    Ping();
    std::string _token;

    Ping(const Ping& other);
    Ping& operator=(const Ping& other);
};

#endif