/**
 * @file CmdFactory.hpp
 * @brief Factory for classes implementing #ICommand
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CMDFACTORY_HPP
#define CMDFACTORY_HPP

#include <string>

class Server;
class Client;
class ICommand;

/**
 * @brief Generates a new Command among those implemented by the server
 * @details
 * - perform checks on Client status
 * - delegate to a series of helper functions arguments checking and instanciation
 */
class CmdFactory
{
  public:
    /**
     * @brief Construct a new Cmd Factory object
     * @details is instanciated in #Server
     */
    CmdFactory(void);
    /**
     * @brief Destroy the Cmd Factory object
     *
     */
    ~CmdFactory();

    /**
     * @brief checks client status against detected command
     * @details when client is ..
     - #UNAUTHENTICATED only #Pass and #Quitcan be handled
     - #AUTHENTICATED only #Pass, #Quit, #Nick and #User can be handled
     - #REGISTERED all commands can be handled
     * @param client
     * @param command
     * @return true if command is usable in client state
     * @return false otherwise
     */
    bool check_in(Client& client, std::string& command);
    /**
     * @brief main command in charge of
     * - checking command availability
     * - calling adequate helper command
     * @param server
     * @param client
     * @param params
     * @return ICommand*
     * @warning can send ERR_NOTREGISTERED, ERR_UNKNOWNCOMMAND
     */
    ICommand* make_command(Server& server, Client& client, std::string& params);
    ICommand* user_cmd(std::string& params);
    ICommand* pass_cmd(std::string& params);
    ICommand* nick_cmd(std::string& params);
    ICommand* kick_cmd(std::string& params);
    ICommand* quit_cmd(std::string& params);
    ICommand* join_cmd(std::string& params);
    ICommand* part_cmd(std::string& params);
    ICommand* mode_cmd(std::string& params);
    ICommand* topic_cmd(std::string& params);
    ICommand* privmsg_cmd(std::string& params);
    ICommand* invite_cmd(std::string& params);
    ICommand* who_cmd(std::string& params);
    ICommand* motd_cmd(std::string& params);
    ICommand* bot_cmd(std::string& params);
    ICommand* ping_cmd(std::string& params);

  private:

};

#endif
