#pragma once

#include <string>
#include <vector>
#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP

#define SOCKET int
class TcpSocket {
public:
    TcpSocket();
    ~TcpSocket();

    SOCKET getSocket(void);

    bool tcpConnect(const std::string &ipaddress, unsigned short port);
    void tcpBind(unsigned short port);
    void tcpListen();
    static std::string getAddress(const sockaddr_in &addr);
    int setNonBlockingSocket(void);
    int Send(const unsigned char *data, unsigned short len);
    int Receive(std::vector<unsigned char> &buffer);

private:
    SOCKET _sckt;
};
