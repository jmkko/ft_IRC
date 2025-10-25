/**
 * @file TcpSocket.hpp
 * @brief TcpSocket class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "consts.hpp"

#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <string>

/**
 * @brief Encapsulate socket actions
 * @details handle bind, listen, connect
 * @class TcpSocket
 */
class TcpSocket
{
  public:
    /**
     * @brief create a socket
     * AF_INET IPv4 familly
     * SOCK_STREAM tcp type
     * IPPROTO_TCP tcp protocol
     */
    TcpSocket();

    /**
     * @brief create a socket
     * @param socketFd
     */
    TcpSocket(Socket socketFd);
    ~TcpSocket();

    /**
     * @brief Connect a socket to a server
     * AF_INET IPv4 familly
     *
     * @param ipaddress like "127.0.0.1"
     * @param port
     * @return true
     */
    bool tcp_connect(const std::string& ipaddress, unsigned short port);

    /**
     * @brief assign a local address to a socket
     * INADDR_ANY = all sources
     * AF_INET = IPV4
     * @param port port to bind
     * @throw exception if bind error
     */
    void tcp_bind(unsigned short port);

    /**
     * @brief puts socket in a passive state
     * @details prepare a socket that will be used to accept incoming connection requests using accept(2)
     *  SOMAXCONN system manage the value
     *  AF_INET IPv4 familly
     */
    void               tcp_listen();
    bool               is_valid() const;
    static std::string get_address(const sockaddr_in& addr);
    Socket             get_socket() const;
    int                set_non_blocking_socket();

    /**
     * @brief first send data size
     *        second send the data@
     *
     * @param data
     * @param len
     * @return
     */
    int do_send(const unsigned char* data, unsigned short len);

  private:
    Socket     _sckt;
    TcpSocket& operator=(const TcpSocket& inst);
    TcpSocket(const TcpSocket& inst);
};

#endif
