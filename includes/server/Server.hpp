/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 08:55:19 by jhervoch          #+#    #+#             */
/*   Updated: 2025/10/01 22:05:22 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "TcpSocket.hpp"

#include <map>
#include <poll.h>
#include <string>
#include <vector>

class ICommand;
class TcpSocket;
class Client;
class Channel;
class Server
{
  public:
    ~Server();
    Server(const unsigned short port, const std::string& password);

    std::string          get_password() const; // added for PASS
    std::string          get_name() const;
    void                 start();
    void                 stop();
    Client*              find_client_by_nickname(std::string& nick);
    int                  index_of(Client& client);
    void                 add_events_of(Client& client, int event);
    std::vector<Client*> find_clients_by_pattern(const std::string& pattern) const;

  public:
    std::map<std::string, Channel*> channels;

  private:
    TcpSocket                      _serverSocket;
    std::vector<pollfd>            _pfds;
    std::map<Socket, Client*>      _clients;
    std::map<std::string, Client*> _clientsByNick;
    std::string                    _psswd;
    std::string                    _name;

    Server();
    Server(const Server&);

    Server& operator=(const Server& inst);

    void      _clean();
    void      _handle_new_connection(int pfdIndex);
    void      _cleanup_socket_and_client(int pfdIndex);
    void      _remove_client(Socket s);
    void      _handle_client_disconnection(int pfdIndex);
    void      _handle_client_input(int pfdIndex);
    void      _handle_client_output(int pfdIndex);
    void      _listen_to_socket(Socket toListen, uint32_t flags);
    ICommand* _parse_command(Client& c, std::string line);
    void      _handle_command(int pfdIndex);
};

#endif
