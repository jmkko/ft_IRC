/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:12:49 by jhervoch          #+#    #+#             */
/*   Updated: 2025/10/16 14:53:14 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpSocket.hpp"

#include "LogManager.hpp"
#include "utils.hpp"

#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <unistd.h> // close

/************************************************************
 *             🥚 CONSTRUCTORS & DESTRUCTOR                 *
 ************************************************************/

TcpSocket::TcpSocket() : _sckt(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    if (_sckt == -1) {
        std::ostringstream error;
        LOG_SOCKET.error("init socket: [" + TO_STRING(errno) + "]");
        throw std::runtime_error(error.str());
    }
}

TcpSocket::TcpSocket(Socket socketFd) : _sckt(socketFd) {}

TcpSocket::TcpSocket(const TcpSocket& inst) : _sckt(inst._sckt) {}

TcpSocket::~TcpSocket() { close(_sckt); }

Socket TcpSocket::get_socket() const { return _sckt; }

/************************************************************
 *                    ➕ OPERATORS                          *
 ************************************************************/

TcpSocket& TcpSocket::operator=(const TcpSocket& inst)
{
    if (this != &inst) {
        _sckt = inst._sckt;
    }
    return *this;
}

/*************************************************************
 *                      🛠️ FUNCTIONS                         *
 *************************************************************/

bool TcpSocket::tcp_connect(const std::string& ipaddress, unsigned short port)
{
    sockaddr_in addr = {};
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    return connect(_sckt, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0;
}

void TcpSocket::tcp_bind(unsigned short port)
{
    sockaddr_in addr = {};
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);

    int yes = 1;
    setsockopt(_sckt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    int res           = 0;
    res               = bind(_sckt, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    std::string error = strerror(errno);
    if (res != 0) {
        LOG_SOCKET.error("Bind:" + error);
        throw std::runtime_error("Error bind:" + error + "\n");
    }
}

void TcpSocket::tcp_listen()
{

    int         res   = listen(_sckt, SOMAXCONN);
    std::string error = strerror(errno);
    if (res != 0) {
        LOG_SOCKET.error("Listen: " + error);
        throw std::runtime_error("Error listen:" + error + "\n");
    }
}

bool TcpSocket::is_valid() const { return _sckt != -1; }

std::string TcpSocket::get_address(const sockaddr_in& addr) { return inet_ntoa(addr.sin_addr); }

int TcpSocket::set_non_blocking_socket() { return fcntl(_sckt, F_SETFL, O_NONBLOCK); }

int TcpSocket::do_send(const unsigned char* data, unsigned short len)
{
    unsigned short networkLen = htons(len);
    return send(_sckt, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) == sizeof(networkLen)
           && send(_sckt, reinterpret_cast<const char*>(data), len, 0) == len;
}
