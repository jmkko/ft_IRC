#include "fakeClient.hpp"

#include "LogManager.hpp"
#include "TcpSocket.hpp"
#include "colors.hpp"
#include "consts.hpp"
#include "testUtils.hpp"
#include "utils.hpp"

#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring> // memset
#include <iostream>
#include <memory>
#include <netdb.h> // for getaddrinfo
#include <string>
#include <sys/select.h> // select
#include <sys/socket.h> // socket
#include <sys/types.h>  // for getaddrinfo
#include <thread>
#include <unistd.h> // close

#include <sys/poll.h>

std::unique_ptr<TcpSocket> make_client_socket(int port)
{
    std::string portStr = std::to_string(port);
    LOG_TEST.debug(std::string("port is ") + portStr);

	auto so = std::make_unique<TcpSocket>();
	if (so->set_non_blocking_socket() == -1)
	{
		LOG_TEST.debug("make socket: set non blocking error", strerror(errno));
	}
	int yes = 1;
    setsockopt(so->get_socket(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	if (so->tcp_connect("127.0.0.1", port) == false && errno != EINPROGRESS)
	{
		LOG_TEST.debug("make socket: tcp connect error", strerror(errno));
	}
	pollfd pfd = {
		.fd = so->get_socket(),
		.events = POLLOUT | POLLIN,
		.revents = 0
	};
	while (true)
	{
		int ret = poll(&pfd, 1, POLL_TIMEOUT);
		if (ret == -1)
		{
			LOG_TEST.debug("make_socket: error poll", strerror(errno) );
			break;
		}
		if (pfd.revents & POLLOUT)
		{
			LOG_TEST.debug(" make socket : connected");
			break;
		}
		else if (pfd.revents & (POLLHUP | POLLNVAL)) {
			LOG_TEST.debug(" make socket : disconnected");
			break;
        } else if (pfd.revents & POLLERR) {
			socklen_t err = 0;
			socklen_t errsize = sizeof(err);
			if (getsockopt(so->get_socket(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&err), &errsize) != 0)
			{
				LOG_TEST.debug(" make socket : unknown error", strerror(errno));
			} else if (err != 0)
				LOG_TEST.debug(" make socket : error", strerror(errno));
			break;
        }
	}
    return so;
}

/**
 * @brief
 *
 * @param fd
 * @param msg
 * @throw if send error
 */
void send_line(const TcpSocket& so, const std::string& msg)
{
	if (!so.is_valid())
		throw std::runtime_error("send_line: attempted to send on invalid socket");

	pollfd pfd = {
		.fd = so.get_socket(),
		.events = POLLOUT,
		.revents = 0
	};

	int ret = poll(&pfd, 1, POLL_TIMEOUT);
	if (ret == -1)
	{
		LOG_TEST.debug("send_lines: error poll", strerror(errno) );
		return ;
	}

	if (ret == 0) {
        LOG_TEST.debug("send_lines: poll timeout waiting for data");
        return ;
    }

	if (pfd.revents & POLLOUT)
	{
		ssize_t sentBytes = send(so.get_socket(), msg.c_str(), msg.size(), 0);
		LOG_TEST.debug(std::string("send_line: sent bytes = ") + TO_STRING(sentBytes) + ":" + msg);
		if (sentBytes <= 0) {
			throw std::runtime_error("send_line: can't send");
		}
	}
    // Give time for server to process
    std::this_thread::sleep_for(std::chrono::milliseconds(SERVER_PROCESS_TIME_MS * 10));
}

std::string recv_lines(const TcpSocket& so)
{
    char buffer[RECEIVE_BUFFER];

	std::string result = {};
	if (!so.is_valid())
		throw std::runtime_error("recv_line: attempted to send on invalid socket");

	for (int attempt = 0; attempt < MAX_RECV_ATTEMPTS; ++attempt)
	{
		LOG_TEST.debug("attempt" + TO_STRING(attempt + 1) + "/" + TO_STRING(MAX_RECV_ATTEMPTS));
		pollfd pfd = {
			.fd = so.get_socket(),
			.events = POLLIN,
			.revents = 0
		};

		int ret = poll(&pfd, 1, POLL_TEST_TIMEOUT);
		if (ret == -1)
		{
			LOG_TEST.debug("recv_lines: error poll", strerror(errno) );
			return {};
		}

		if (ret == 0) {
			LOG_TEST.debug("recv_lines: poll timeout waiting for data");
		}

		if (pfd.revents & POLLIN)
		{
			ssize_t bytesReceived = recv(so.get_socket(), static_cast<char*>(buffer), sizeof(buffer) - 1, 0);
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				LOG_TEST.debug("recv_line: would block - no data received", strerror(errno));
			}
			if (bytesReceived < 0)
			{
				LOG_TEST.debug("recv_line: error", strerror(errno));
			}
			else if (bytesReceived == 0) {
				LOG_TEST.debug("recv_line: recv nothing to receive", strerror(errno));
			}
			else {			
				buffer[bytesReceived] = '\0';
				result = std::string(static_cast<char *>(buffer));
				LOG_TEST.debug("recv_line: received " + std::to_string(bytesReceived) + " bytes: " + result);
				if (result.find("\r\n") != std::string::npos)
					break;
			}
		}
	}
	return result;
}
