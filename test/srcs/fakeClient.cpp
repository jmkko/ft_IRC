#include "fakeClient.hpp"

#include "colors.hpp"
#include "consts.hpp"
#include "LogManager.hpp"

#include <string>
#include <sys/socket.h>     // socket
#include <sys/types.h>      // for getaddrinfo
#include <netdb.h>          // for getaddrinfo
#include <unistd.h>         // close
#include <cstring>          // memset
#include <iostream>

Socket make_client_socket(int port)
{
	std::string portStr = std::to_string(port);
	std::cout << "portStr " << portStr << '\n';

	LOG_TEST.debug(portStr);

	struct addrinfo hints = {};
	struct addrinfo* result = nullptr;

    std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo("localhost", portStr.c_str(), &hints, &result);
	LOG_TEST.debug(std::to_string(status));
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

bool send_line(int fd, const std::string& msg)
{
	return send(fd, msg.c_str(), msg.size(), 0) >= 0;
}

std::string recv_line(int fd)
{
	char		buffer[RECEIVE_BUFFER];
	ssize_t		bytesReceived = recv(fd, static_cast<char *>(buffer), sizeof(buffer) - 1, 0);
	if (bytesReceived <= 0)
		return {};
	buffer[bytesReceived] = '\0';
	return std::string(static_cast<char *>(buffer));
}
