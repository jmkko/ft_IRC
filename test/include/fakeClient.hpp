#ifndef FAKE_CLIENT_HPP
#define FAKE_CLIENT_HPP

#include "TcpSocket.hpp"
#include <string>

#define RECEIVE_BUFFER 512

Socket		make_client_socket(int port);
void		send_line(int fd, const std::string& msg);
std::string recv_line(int fd);

#endif