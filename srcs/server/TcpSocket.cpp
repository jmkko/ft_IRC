/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:12:49 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/16 00:25:40 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpSocket.hpp"
#include "LogManager.hpp"
#include "utils.hpp"
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <unistd.h> // close

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/**
 * @brief create a socket
 * AF_INET IPv4 familly
 * SOCK_STREAM tcp type
 * IPPROTO_TCP tcp protocol
 */
TcpSocket::TcpSocket()
{
    _sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_sckt == -1) {
        std::ostringstream error;
        LOG_SOCKET.error("init socket: [" + TO_STRING(errno) + "]");
        throw std::runtime_error(error.str());
    }
}

/**
 * @brief create a socket
 * @param socketFd
 */
TcpSocket::TcpSocket(Socket socketFd) : _sckt(socketFd) {}

TcpSocket::TcpSocket(const TcpSocket& inst) : _sckt(inst._sckt) {}

TcpSocket::~TcpSocket() { close(_sckt); }

Socket TcpSocket::getSocket() const { return _sckt; }

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

TcpSocket& TcpSocket::operator=(const TcpSocket& inst)
{
    if (this != &inst) {
        _sckt = inst._sckt;
    }
    return *this;
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/******************************************************************************
 * Connect a socket to a server
 * ipaddress like "127.0.0.1"
 * AF_INET IPv4 familly
 ******************************************************************************/
bool TcpSocket::tcpConnect(const std::string& ipaddress, unsigned short port)
{
    sockaddr_in addr = {};
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    return connect(_sckt, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0;
}

/**
 * @brief assign a local address to a socket
 * INADDR_ANY = all sources
 * AF_INET = IPV4
 * @param port port to bind
 * @throw exception if bind error
 */
void TcpSocket::tcpBind(unsigned short port)
{
    sockaddr_in addr = {};
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int yes = 1;
    setsockopt(_sckt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    int res = 0;
    res = bind(_sckt, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    std::string error = strerror(errno);
    if (res != 0) {
        LOG_SOCKET.error("Bind:" + error);
        throw std::runtime_error("Error bind:" + error + "\n");
    }
}

/******************************************************************************
 * wait a socket connection a socket that will be used to accept incoming
 *      connection requests using accept(2)
 * SOMAXCONN system manage the value
 * AF_INET IPv4 familly
 * return 0 on success or -1 on error
 ******************************************************************************/
void TcpSocket::tcpListen()
{

    int         res = listen(_sckt, SOMAXCONN);
    std::string error = strerror(errno);
    if (res != 0) {
        LOG_SOCKET.error("Listen: " + error);
        throw std::runtime_error("Error listen:" + error + "\n");
    }
}

std::string TcpSocket::getAddress(const sockaddr_in& addr) { return inet_ntoa(addr.sin_addr); }

int TcpSocket::setNonBlockingSocket() { return fcntl(_sckt, F_SETFL, O_NONBLOCK); }

/*****************************************************************************
 * first send data size
 * second send data
 *******************************************************************************/
int TcpSocket::Send(const unsigned char* data, unsigned short len)
{
    unsigned short networkLen = htons(len);
    return send(_sckt, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) ==
               sizeof(networkLen) &&
           send(_sckt, reinterpret_cast<const char*>(data), len, 0) == len;
}

/*****************************************************************************
 * first read data size
 * second read data
 ******************************************************************************/
int TcpSocket::Receive(std::vector<unsigned char>& buffer)
{
    unsigned short expectedSize = 0;
    size_t pending = recv(_sckt, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
    if (pending <= 0 || pending != sizeof(unsigned short)) {
        //!< Erreur
        return false;
    }

    expectedSize = ntohs(expectedSize);
    buffer.resize(expectedSize);
    ssize_t receivedSize = 0;
    do {
        ssize_t ret = recv(_sckt,
                           reinterpret_cast<char*>(&buffer[receivedSize]),
                           (expectedSize - receivedSize) * sizeof(unsigned char),
                           0);
        if (ret <= 0) {
            //!< Erreur
            buffer.clear();
            return false;
        } else {
            receivedSize += ret;
        }
    } while (receivedSize < expectedSize);
    return true;
}
