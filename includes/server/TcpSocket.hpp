#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "consts.hpp"

#include <errno.h>
#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <string>
#include <vector>

class TcpSocket
{
  public:
    TcpSocket();
    TcpSocket(Socket socketFd);
    ~TcpSocket();

    static std::string getAddress(const sockaddr_in& addr);
    Socket             getSocket(void) const;

    int                setNonBlockingSocket(void);

    bool               tcpConnect(const std::string& ipaddress, unsigned short port);
    void               tcpBind(unsigned short port);
    void               tcpListen();
    int                Send(const unsigned char* data, unsigned short len);
    int                Receive(std::vector<unsigned char>& buffer);

  private:
    Socket _sckt;
};

#endif