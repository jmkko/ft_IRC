#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "consts.hpp"

#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <string>
#include <vector>

class TcpSocket
{
  public:
    TcpSocket();
    TcpSocket(Socket socketFd);

    ~TcpSocket();

    static std::string get_address(const sockaddr_in& addr);
    Socket             get_socket() const;

    int set_non_blocking_socket();

    bool is_valid() const;
    bool tcp_connect(const std::string& ipaddress, unsigned short port);
    void tcp_bind(unsigned short port);
    void tcp_listen();
    int  do_send(const unsigned char* data, unsigned short len);
    int  do_receive(std::vector<unsigned char>& buffer);

  private:
    Socket     _sckt;
    TcpSocket& operator=(const TcpSocket& inst);
    TcpSocket(const TcpSocket& inst);
};

#endif
