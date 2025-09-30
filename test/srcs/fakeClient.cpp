#include "fakeClient.hpp"

#include "colors.hpp"
#include "consts.hpp"
#include "LogManager.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <cstddef>
#include <string>
#include <sys/socket.h>     // socket
#include <sys/types.h>      // for getaddrinfo
#include <sys/select.h>     // select
#include <netdb.h>          // for getaddrinfo
#include <unistd.h>         // close
#include <cstring>          // memset
#include <iostream>
#include <thread>
#include "LogManager.hpp"

Socket make_client_socket(int port)
{
	std::string portStr = std::to_string(port);
	LOG_TEST.debug(std::string("port is ") + portStr);

	struct addrinfo hints = {};
	struct addrinfo* result = nullptr;

    std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo("localhost", portStr.c_str(), &hints, &result);
	LOG_TEST.debug(std::string("status is ") + std::to_string(status));
	if (status != 0)
	{
		LOG_TEST.error("getaddrinfo failed: " + std::string(gai_strerror(status)));
		return -1;
	}

	int fd = -1;

	// trying to get a working connection socket for localhost
	for (struct addrinfo* test = result; test != nullptr; test = test->ai_next)
	{
		fd = socket(test->ai_family, test->ai_socktype, test->ai_protocol);
		if (fd == -1)
			continue;
		if (connect(fd, test->ai_addr, test->ai_addrlen) == 0)
			break;
		close(fd);
		fd = -1;
	}
	freeaddrinfo(result);
	return fd;
}

/**
 * @brief 
 * 
 * @param fd 
 * @param msg 
 * @throw if send error
 */
void send_line(int fd, const std::string& msg)
{
	ssize_t sentBytes = send(fd, msg.c_str(), msg.size(), 0);
	LOG_TEST.debug(std::string("sent bytes = " + TO_STRING(sentBytes)));
	if (sentBytes <= 0)
	{
		close(fd);
		throw std::runtime_error("can't send");
	}
	// Give time for server to process
	std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS));
}

std::string recv_line(int fd)
{
	char		buffer[RECEIVE_BUFFER];
	
	// Try to receive data with a short timeout
	fd_set readfds;
	struct timeval timeout = {};
	
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	
	timeout.tv_sec = 1;  // 1 second timeout
	timeout.tv_usec = 0;
	
	int selectResult = select(fd + 1, &readfds, NULL, NULL, &timeout);
	
	if (selectResult <= 0) {
		// no exception as all messages don't have to get a reply
		LOG_TEST.debug("recv_line: timeout or error in select");
		return {};
	}
	
	ssize_t		bytesReceived = recv(fd, static_cast<char *>(buffer), sizeof(buffer) - 1, 0);
	if (bytesReceived <= 0) {
		LOG_TEST.debug("recv_line: no data received");
		return {};
	}
	buffer[bytesReceived] = '\0';
	std::string result(static_cast<char *>(buffer));
	LOG_TEST.debug("recv_line: received " + std::to_string(bytesReceived) + " bytes: " + result);
	return result;
}
