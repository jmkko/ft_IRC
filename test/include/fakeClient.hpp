#ifndef FAKE_CLIENT_HPP
#define FAKE_CLIENT_HPP

#include "TcpSocket.hpp"
#include <memory>
#include <string>

#define RECEIVE_BUFFER 512
#define POLL_TEST_TIMEOUT 50
#define MAX_RECV_ATTEMPTS 1

std::unique_ptr<TcpSocket> make_client_socket(int port);
void						send_line(const TcpSocket& so, const std::string& msg);
std::string 				recv_lines(const TcpSocket& so);

#endif