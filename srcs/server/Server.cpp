#include "Server.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "CmdFactory.hpp"
#include "Config.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "consts.hpp"
#include "signal_handler.hpp"
#include "utils.hpp"

#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <csignal>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h> // close

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR	            *
 ************************************************************/

/**
 * @brief initialize server, assign a socket binding a port with an address, and adds it to the
 * monitored pollfd
 * @pre the `psswd` argument should have been checked with `utils::check_password()`
 * @pre the `port`argument should have been checked with `utils::check_port()`
 * @param port valid port number
 * @param psswd valid password
 */
Server::Server(const unsigned short port, const std::string& password) :
	_psswd(password), _name(ircConfig.get_name())
{
	try {
		_serverSocket.tcp_bind(port);
		_serverSocket.tcp_listen();
	} catch (std::exception& e) {
		std::cout << e.what();
		exit(1); // need improve for exit program
	}
	LOG_SERVER.info("Server " + _name + " start at port :" + utils::to_string(port));
	std::cout << "\n";

	// Socket serveur : surveiller nouvelles connexions
	_listen_to_socket(_serverSocket.get_socket(), POLLIN);
}

Server::~Server()
{
	LOG_SERVER.debug("Server dtor");
	_clean();
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief runs the server loop, regularly checking activity through `poll()`
 */
void Server::start()
{
	while (globalSignal != SIGINT && globalSignal != SIGABRT) {
		int pollResult = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT); // Timeout 1 second
		if (pollResult == -1) {
			LOG_SERVER.error("Poll failed: " + TO_STRING(strerror(errno)));
			break;
		}
		if (pollResult == 0) {
			continue;
		}

		// review each client socket
		LOG_SERVER.debug(utils::to_string(pollResult) + "event(s) detected");

		for (int i = 0; i < static_cast<int>(_pfds.size()); i++) {
			// new connection
			if (i == 0 && (_pfds[i].revents & POLLIN)) {
				_handle_new_connection(i);
			}
			// Client socket : data or disconnection
			else if (i > 0) {
				std::map<Socket, Client*>::iterator clientIt = _clients.find(_pfds[i].fd);
				// orphelan socket
				if (clientIt == _clients.end()) {
					_cleanup_socket_and_clients(i--);
					continue;
				}
				if (_pfds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
					_handle_client_disconnection(i--);
				} else if (_pfds[i].revents & POLLIN) {
					_handle_client_input(i);
				} else if (_pfds[i].revents & POLLOUT) {
					_handle_client_output(i);
				}
			}
			_pfds[i].revents = 0; // Reset events
		}
	}
	_clean();
}

/**
 @brief store new client socket, set it non blocking, and subscribe to new POLLIN events
 @param i index of the monitored fds
 */
void Server::_handle_new_connection(int pfdIndex)
{
	std::string pollEvent;
	if (_pfds[pfdIndex].revents & POLLIN)
		pollEvent.append("POLLIN ");
	if (_pfds[pfdIndex].revents & POLLOUT)
		pollEvent.append("POLLOUT ");
	if (_pfds[pfdIndex].revents & POLLHUP)
		pollEvent.append("POLLHUP ");
	if (_pfds[pfdIndex].revents & POLLERR)
		pollEvent.append("POLLERR ");
	LOG_SOCKET.debug("Socket " + utils::to_string(_pfds[pfdIndex].fd) + " events: " + pollEvent);

	sockaddr_in clientAddr = {};
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t addrLen = sizeof(clientAddr);
	Socket	  socket = accept(_serverSocket.get_socket(), reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

	if (socket != -1) {
		if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
			LOG_ERR.error(std::string("Error while setting a non blocking client socket") + strerror(errno));
			LOG_SOCKET.error(std::string("Error while setting a non blocking client socket")
							 + strerror(errno));
			close(socket);
		} else {
			Client* newClient = new Client(socket, clientAddr);
			LOG_CONN.info(std::string("New connection accepted on socket ") + utils::to_string(socket)
						  + " => " + utils::to_string(*newClient));
			_clients[socket] = newClient;

			_listen_to_socket(socket, POLLIN);
		}
	}
}

/**
 @brief removes client
 @param pfdIndex
*/
void Server::_handle_client_disconnection(int pfdIndex)
{
	Socket	socket = _pfds[pfdIndex].fd;
	Client* client = _clients[socket];
	if (!client) {
		LOG_SERVER.error("client not found");
		return;
	}

	socklen_t err = 0;
	socklen_t errsize = sizeof(err);
	if (getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &errsize) == 0) {
		if (err == 0) {
			LOG_SERVER.debug("connection has been closed by client");
		} else {
			LOG_ERR.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
			LOG_SOCKET.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
		}
	}
	LOG_CONN.info(std::string("Client at ") + client->get_address() + ":"
				  + utils::to_string(client->get_port()) + " disconnected");
	_cleanup_socket_and_clients(pfdIndex);
}

/**
 * @brief attempt receiving bytes from client, parse into a command and execute it
 enable write notification on client socket if a response has to be sent
 in case of partial reception (message not ending with \r\n), add to receive buffer
 * @param pfdIndex index of monitored fd
 */
void Server::_handle_client_input(int pfdIndex)
{
	LOG_SERVER.debug("Server #_handle_client_input");
	Socket	socket = _pfds[pfdIndex].fd;
	Client* client = _clients[socket];
	if (!client) {
		LOG_ERR.error("handle receive... client not found");
		return;
	}

	char buffer[CLIENT_READ_BUFFER_SIZE];
	memset(static_cast<char*>(buffer), 0, CLIENT_READ_BUFFER_SIZE);
	ssize_t bytesRead = recv(socket, static_cast<char*>(buffer), CLIENT_READ_BUFFER_SIZE - 1, 0);

	if (bytesRead == 0) {
		LOG_CONN.warning("Connection closed properly");
		_handle_client_disconnection(pfdIndex);
		return;
	} else if (bytesRead == -1) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			LOG_ERR.error("Reception failed:" + TO_STRING(strerror(errno)));
			_handle_client_disconnection(pfdIndex);
		}
		return;
	} else {
		if (utils::safe_at(buffer, bytesRead))
			utils::safe_at(buffer, bytesRead) = '\0';
		client->append_to_read_buffer(std::string(static_cast<char*>(buffer)));
		this->_handle_command(pfdIndex);
	}
}

/**
 *@brief attempt sending the queued messages
 if a message is partially sent, updates send buffer accordingly
 if a message is completely sent, disable write notification for the client fd
 in case of send error, either retry or disconnect the client
 @param pfdIndex monitored fd for client
*/
void Server::_handle_client_output(int pfdIndex)
{
	LOG_SERVER.info("Server#_handle_client_output");
	Socket	socket = _pfds[pfdIndex].fd;
	Client* client = _clients[socket];
	if (!client) {
		LOG_SERVER.error("client not found");
		return;
	}

	std::string sendBuffer = client->get_send_buffer();
	if (!sendBuffer.empty()) {

		LOG_SERVER.info("Client.Buffer: " + sendBuffer);
		ssize_t bytesSent = send(socket, sendBuffer.c_str(), sendBuffer.length(), 0);

		if (bytesSent == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				LOG_SERVER.warning(std::string("Socket is not ready for sending ... trying later"));

			} else {
				LOG_SERVER.error(std::string("Error sending to client"));
				_handle_client_disconnection(pfdIndex);
			}
		} else if (static_cast<size_t>(bytesSent) < sendBuffer.length()) {
			LOG_SERVER.warning(std::string("Queue has been partially sent (") + utils::to_string(bytesSent)
							   + "/" + utils::to_string(sendBuffer.length()) + ")");
			client->set_send_buffer(sendBuffer.substr(bytesSent));
		} else {
			LOG_SERVER.debug(std::string("Message sent normally ... unsubscribing from POLLOUT"));
			client->get_send_buffer().clear();
			_pfds[pfdIndex].events &= ~POLLOUT;
		}
	} else {
		LOG_SERVER.debug(std::string("No data to send ... unsubscribing from POLLOUT"));
		_pfds[pfdIndex].events &= ~POLLOUT;
	}
}

void Server::_handle_command(int pfdIndex)
{
	Socket	socket = _pfds[pfdIndex].fd;
	Client* client = _clients[socket];

	size_t pos = std::string::npos;
	// tant qu'il y a un \r\n dans le readbuffer du client, executer les commandes
	while ((pos = client->get_read_buffer().find("\r\n")) != std::string::npos) {
		// extract the first command from the readBuffer
		std::string line = client->get_read_buffer().substr(0, pos);
		// delete the command that has been extracted from the client readbuffer
		client->get_read_buffer().erase(0, pos + 2);
		// parse and create the appropriate command, NULL is returned if a faillure has happen
		ICommand* cmd = _parse_command(*client, line);
		if (cmd) {
			cmd->execute(*this, *client);
			delete cmd;
		}
	}
}

// Make and return a command object from the command line if valid command;
// return NULL if command has failed amd print
ICommand* Server::_parse_command(Client& client, std::string line)
{
	CmdFactory commandBuilder;
	ICommand*  cmd = commandBuilder.make_command(*this, client, line);

	return cmd;
}

Client* Server::find_client_by_nickname(std::string& nickname)
{
	for (std::map<Socket, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (nickname == it->second->get_nickname())
			return it->second;
	}
	return NULL;
}

std::string Server::get_password() const { return _psswd; }

/**
 * @brief [TODO:return index of client in _pfds[]]
 *
 * @param client [TODO:parameter]
 * @return [TODO:-1 if not found or bad socket]
 */
int Server::index_of(Client& client)
{
	Socket socket = client.get_socket();

	if (socket == -1)
		return -1;
	for (size_t i = 0; i < _pfds.size(); ++i) {
		if (_pfds[i].fd == socket)
			return static_cast<int>(i);
	}
	return -1;
}

void Server::add_events_of(Client& client, int event)
{
	int index = index_of(client);
	LOG_SERVER.debug(TO_STRING("index of client : ") + TO_STRING(index));
	if (index >= 0) {
		_pfds[index].events = static_cast<short>(_pfds[index].events | event); // ADD to existing events
	}
}

void Server::_listen_to_socket(Socket toListen, uint32_t flags)
{
	pollfd newPollFd = {.fd = toListen, .events = static_cast<short>(flags), .revents = 0};
	_pfds.push_back(newPollFd);
}

/**
 * @brief [TODO:description]
 */
void Server::_clean()
{
	if (globalSignal != SIGINT || globalSignal != SIGABRT)
		globalSignal = SIGINT;
	LOG_SERVER.debug(std::string("cleaning ") + TO_STRING(_pfds.size())
					 + " sockets and their associated clients");
	for (size_t i = 0; i < _pfds.size(); ++i)
		_cleanup_socket_and_clients(static_cast<int>(i));

	// Clean up channels
	LOG_SERVER.debug(std::string("cleaning ") + TO_STRING(channels.size()) + " channels");
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		delete it->second;
	}
	channels.clear();
}

void Server::stop()
{
	if (globalSignal != SIGINT || globalSignal != SIGABRT)
		globalSignal = SIGINT;
}

/**
 @brief cleanup a socket and associated Client
 close the socket fd
 remove entry from _clients
 remove entry from _clientsByNick
 delete client instance
 remove entry from _fds (the pollfd list)
 @param i index of monitored fd
*/
void Server::_cleanup_socket_and_clients(int pfdIndex)
{
	Client* c = _clients[_pfds[pfdIndex].fd];
	_clients.erase(_pfds[pfdIndex].fd);
	if (c) {
		if (!c->get_nickname().empty())
			_clientsByNick.erase(c->get_nickname());
		delete c;
	}
	_pfds.erase(_pfds.begin() + pfdIndex);
}
