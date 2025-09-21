/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/18 23:44:30 by npolack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "TcpSocket.hpp"
#include "consts.hpp"

#include "CmdFactory.hpp"
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h> // close

class Server
{
  public:
    Server(const unsigned short port, const std::string& psswd);
    ~Server();

    void				start();
	std::string 		getPassW() const;					// added for PASS
    Client*				findClientByNickname(std::string&);
    void				sendToClient(int, const std::string&);

  private:
    TcpSocket                      _serverSocket;
    std::vector<pollfd>            _fds;
    std::map<Socket, Client*>      _clients;
    std::map<std::string, Client*> _clientsByNick;
    std::string                    _psswd;
    std::string                    _name;

    Server();
    Server(const Server&);
    Server& operator=(const Server& inst);

    void 		handleNewConnection(int);
    void 		cleanupSocket(int);
    void 		removeClient(Socket);
    void 		handleClientDisconnection(int);
    void		handleClientData(int);
    void		handleClientOutput(int);
	void     	listenToSocket(Socket, uint32_t);
    ICommand*	parseCommand(Server&, Client&, std::string);
	void		handleCommand(Client&);
};

#endif
