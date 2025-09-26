/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/26 11:31:40 by fpetit           ###   ########.fr       */
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
	Server(const unsigned short port, const std::string& password);

	std::string get_password() const; // added for PASS
	void		start();
	void		stop();
	Client*		find_client_by_nickname(std::string& nick);
	int			index_of(Client& client);
	void		add_events_of(Client& client, int event);

  public:
	std::map<std::string, Channel*> channels;

  private:
	TcpSocket					   _serverSocket;
	std::vector<pollfd>			   _pfds;
	std::map<Socket, Client*>	   _clients;
	std::map<std::string, Client*> _clientsByNick;
	std::string					   _psswd;
	std::string					   _name;

	Server();
	Server(const Server&);

	Server& operator=(const Server& inst);

	void	  _handle_new_connection(int pfdIndex);
	void	  _cleanup_socket_and_clients(int pfdIndex);
	void	  _remove_client(Socket s);
	void	  _handle_client_disconnection(int pfdIndex);
	void	  _handle_client_input(int pfdIndex);
	void	  _handle_client_output(int pfdIndex);
	void	  _listen_to_socket(Socket toListen, uint32_t flags);
	ICommand* _parse_command(Client& c, std::string line);
	void	  _handle_command(int pfdIndex);
};

#endif
