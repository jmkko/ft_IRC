#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "consts.hpp"

#include <errno.h>
#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <string>
#include <vector>
#include <poll.h>

class TcpSocket
{
  public:
    TcpSocket();
    TcpSocket(Socket);
    TcpSocket(const TcpSocket& inst);

    ~TcpSocket();

    TcpSocket& operator=(const TcpSocket& inst);

    static std::string getAddress(const sockaddr_in& addr);
    Socket             getSocket(void) const;

    int setNonBlockingSocket(void);

    bool tcpConnect(const std::string& ipaddress, unsigned short port);
    void tcpBind(unsigned short port);
    void tcpListen();
    int  Send(const unsigned char* data, unsigned short len);
    int  Receive(std::vector<unsigned char>& buffer);

    short  getRevents() const;
    short  setRevents(short);

    short  getEvents() const;
    short  setEvents(short);
  private:
    Socket _sckt;
//	pollfd							_pfd;
};

#endif
