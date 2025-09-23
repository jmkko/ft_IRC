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
#include <list>
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
	typedef std::map<Socket, Client*>::iterator clients_socket;
	typedef std::list<Client*>::iterator it_client;
    Server(const unsigned short port, const std::string& psswd);
    ~Server();

    void				start();
	std::string 		getPassW() const;					// added for PASS
    Client*				findClientByNickname(std::string&);
    void				sendTo(Client *, const std::string&);

  private:
	std::list<Client*>				_clients; // added
    TcpSocket                      _serverSocket;
    std::vector<pollfd>            _pfds;
	pollfd							_server_pfd; // added
    //std::map<Socket, Client*>      _clients;
    std::map<std::string, Client*> _clientsByNick;
    std::string                    _psswd;
    std::string                    _name;

    Server();
    Server(const Server&);
    Server& operator=(const Server& inst);

    void 		handleNewConnection();
	void 		cleanupSocket(it_client*);
    void 		removeClient(Socket);
    void 		handleClientDisconnection(it_client*);
    void		handleClientData(it_client*);
    void		handleClientOutput(it_client*);
	void     	listenToSocket(Socket, uint32_t);
    ICommand*	parseCommand(Client&, std::string);
	void		handleCommand(it_client *);
	int			findInPfds(Socket socket_fd);
};

#endif
