/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhervoch <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/11 09:27:13 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

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
#include "TcpSocket.hpp"

#define SOCKET int

struct Client {
    SOCKET sckt;
    sockaddr_in addr;
    std::string messageQueue; // Messages sending queue
    bool hasDataToSend() const { return !messageQueue.empty(); }
};

class Server {
private:
    TcpSocket                   _serverSocket;
    std::vector<pollfd>         _fds;
    std::map<SOCKET, Client>    _clients;
    std::string                 _psswd;
    std::string                 _name;

    void handleNewConnection(int);
    void cleanupSocket(int);
    void handleClientDisconnection(int);

public:
    Server(const unsigned short port, const std::string &psswd);
    ~Server();
    void start();
};

#endif
