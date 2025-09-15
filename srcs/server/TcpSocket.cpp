/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 14:12:49 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/15 19:17:08 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpSocket.hpp"
#include "LogManager.hpp"
#include <cstring>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <unistd.h>    // close
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include "utils.hpp"

/******************************************************************************
 * Create a socket
 * AF_INET IPv4 familly
 * SOCK_STREAM tcp type
 * IPPROTO_TCP tcp protocol
 ******************************************************************************/
TcpSocket::TcpSocket() {
    _sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_sckt == -1) {
        std::ostringstream error;
        LOG_SOCKET.error("init socket: [" + TO_STRING(errno) + "]");
        throw std::runtime_error(error.str());
    }
}

TcpSocket::TcpSocket(Socket socketFd) : _sckt(socketFd) {}

TcpSocket::~TcpSocket() { close(_sckt); }

Socket TcpSocket::getSocket() const { return _sckt; }

/******************************************************************************
 * Connect a socket to a server
 * ipaddress like "127.0.0.1"
 * AF_INET IPv4 familly
 ******************************************************************************/
bool TcpSocket::tcpConnect(const std::string &ipaddress, unsigned short port) {
    sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    return connect(_sckt, (const sockaddr *)&addr, sizeof(addr)) == 0;
}

/******************************************************************************
 * Assign a local address to a socket
 * INADDR_ANY all source allowed
 * AF_INET IPv4 familly
 * return 0 or SOKET_ERROR = -1
 ******************************************************************************/
void TcpSocket::tcpBind(unsigned short port) {
    sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int res = bind(_sckt, (const sockaddr *)&addr, sizeof(addr));
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
void TcpSocket::tcpListen() {

    int res = listen(_sckt, SOMAXCONN);
    std::string error = strerror(errno);
    if (res != 0) {
        LOG_SOCKET.error("Listen: " + error);
        throw std::runtime_error("Error listen:" + error + "\n");
    }
}

std::string TcpSocket::getAddress(const sockaddr_in &addr) {
    char buff[INET6_ADDRSTRLEN] = {0};
    return inet_ntop(addr.sin_family, (void *)&(addr.sin_addr), buff, INET6_ADDRSTRLEN);
}

int TcpSocket::setNonBlockingSocket() { return fcntl(_sckt, F_SETFL, O_NONBLOCK); }

/*****************************************************************************
 * first send data size
 * second send data
 *******************************************************************************/
int TcpSocket::Send(const unsigned char *data, unsigned short len) {
    unsigned short networkLen = htons(len);
    return send(_sckt, reinterpret_cast<const char *>(&networkLen), sizeof(networkLen), 0) == sizeof(networkLen) &&
           send(_sckt, reinterpret_cast<const char *>(data), len, 0) == len;
}

/*****************************************************************************
 * first read data size
 * second read data
 ******************************************************************************/
int TcpSocket::Receive(std::vector<unsigned char> &buffer) {
    unsigned short expectedSize;
    int pending = recv(_sckt, reinterpret_cast<char *>(&expectedSize), sizeof(expectedSize), 0);
    if (pending <= 0 || pending != sizeof(unsigned short)) {
        //!< Erreur
        return false;
    }

    expectedSize = ntohs(expectedSize);
    buffer.resize(expectedSize);
    int receivedSize = 0;
    do {
        int ret = recv(_sckt, reinterpret_cast<char *>(&buffer[receivedSize]),
                       (expectedSize - receivedSize) * sizeof(unsigned char), 0);
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
