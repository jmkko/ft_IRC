/**
 * @file ICommand.hpp
 * @brief command interface
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP
#include <vector>

class Server;
class Client;
// interface of each commands
/**
 * @class ICommand
 * @brief Interface providing an `execute` behavior for all commands
 *
 */
class ICommand
{
  public:
    /**
     * @brief Destroy the ICommand object
     *
     */
    virtual ~ICommand() {}
    /**
     * @brief abstract function
     *
     * @param server #Server
     * @param client the #Client having sent the command message
     */
    virtual void execute(Server& server, Client& client) = 0;

  protected:
    ICommand() {}

  private:
    ICommand(const ICommand& inst);
    ICommand& operator=(const ICommand& inst);
};

#endif
