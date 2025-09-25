/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/24 14:15:26 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Config.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "TcpSocket.hpp"
#include "consts.hpp"
#include "utils.hpp"

#include "CmdFactory.hpp"
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h> // close

class Server
{
  public:
    ~Server();
    Server(const unsigned short port, const std::string& psswd);

    // static Server&		getInstance(const unsigned short port = DEFAULT_PORT, const std::string&
    // password = DEFAULT_PASSWORD)

    void        start();
    void        stop();
    std::string getPassW() const; // added for PASS
    Client*     findClientByNickname(std::string&);
    int         indexOf(Client& client);
    void        addEventsOf(Client&, int event);

  public:
    std::map<std::string, Channel*> channels;

  private:
    TcpSocket                       _serverSocket;
    std::vector<pollfd>             _pfds;
    std::map<Socket, Client*>       _clients;
    std::map<std::string, Client*>  _clientsByNick;
    std::string                     _psswd;
    std::string                     _name;
    // satic Server*                 _instance;

    Server();
    Server(const Server&);

    Server& operator=(const Server& inst);

    void      handleNewConnection(int);
    void      cleanupSocketAndClients(int);
    void      removeClient(Socket);
    void      handleClientDisconnection(int);
    void      handleClientData(int);
    void      handleClientOutput(int);
    void      listenToSocket(Socket, uint32_t);
    ICommand* parseCommand(Client&, std::string);
    void      handleCommand(int);
};

#endif
