/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/15 23:35:11 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "Client.hpp"
#include "ICommand.hpp"
#include "TcpSocket.hpp"
#include "LogManager.hpp"
#include "consts.hpp"

#include <string>
#include <string.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <unistd.h>    // close
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <exception>

class Server {
private:
    TcpSocket                       _serverSocket;
    std::vector<pollfd>             _fds;
    std::map<Socket, Client*>        _clients;
    std::map<std::string, Client*>   _clientsByNick;
    std::string                     _psswd;
    std::string                     _name;

    void handleNewConnection(int);
    void cleanupSocket(int, Client* c);
	void removeClient(Socket socket);
    void handleClientDisconnection(int);
    void handleClientData(int);
    void sendToClient(int, const std::string &);
    void handleClientOutput(int);
    void listenToSocket(Socket toListen, uint32_t flags);
	Client*	getClientBySocket(Socket socket);
	Client*	getClientByNick(const std::string& nick);
	ICommand* parseCommand(char* buffer);

public:
    Server(const unsigned short port, const std::string &psswd);
    ~Server();
    void start();
};

#endif
