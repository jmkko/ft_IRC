#include "Server.hpp"
#include "LogManager.hpp"
#include "utils.hpp"
#include <cstring>
#include <sstream>
#include <sys/poll.h>
#include <poll.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <thread>
// In your while loop
void printWhile(int& condition, std::string& msg) {
	int dots = 0;
	while (condition == 0) {
		    std::cout << "\r" + msg;
		for (int i = 0; i < (dots % 4); ++i) {
				std::cout << ".";
			}
			std::cout << "   "; // clear extra characters
			std::cout.flush();
			
			dots++;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/**
 * @brief initialize server, assign a socket binding a port with an address, and adds it to the
 * monitored pollfd
 * @pre the `psswd` argument should have been checked with `utils::checkPassword()`
 * @pre the `port`argument should have been checked with `utils::checkPort()`
 * @param port valid port number
 * @param psswd valid password
 */
Server::Server(const unsigned short port, const std::string& psswd) :
    _psswd(psswd), _name(SERVER_NAME)
{
    try {
        _serverSocket.tcpBind(port);
        _serverSocket.tcpListen();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return; // need improve for exit program
    }
    LOG_SERVER.info("Server " + _name + "start at port :" + utils::toString(port));
    std::cout << std::endl;

	pollfd serverPfd = {.fd = _serverSocket.getSocket(), .events = POLLIN, .revents = 0};
	_pfds.push_back(serverPfd);
    // Socket serveur : surveiller nouvelles connexions
    //listenToSocket(_serverSocket.getSocket(), POLLIN);
}

Server::~Server()
{
	_clients.clear();
	//for (cl_iterator client = _clients.begin(); client != _clients.end(); client++) {
	//	_clients.erase(++client);
	//}
    //for (int i = 0; i < (int)_fds.size(); ++i) {
     //   cleanupSocket(i);
    //}
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief runs the server loop, regularly checking activity through `poll()`
 */
							
int Server::findInPfds(Socket socket_fd) {
	if (_pfds.empty()) {
		return -1;
	}
	
	for (size_t i = 0; i < _pfds.size(); ++i) {
		if (_pfds[i].fd == socket_fd) {
			return static_cast<int>(i);
		}
	}
   return -1;  // Not found
}

void Server::start()
{
	LOG_SERVER.info("looking for events");
    while (true) {
        int pollResult = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT); // Timeout 1 second
        if (pollResult == -1) {
            LOG_ERR.error("Poll failed: " + TO_STRING(strerror(errno)));
            LOG_SERVER.error("Critical: Poll system failed");
            break;
        }
        if (pollResult == 0) {
            continue;
        } else if (_pfds[0].revents & POLLIN) {
			LOG_SERVER.info("New connection");
			std::cout << "New connection" << std::endl;
			handleNewConnection();
		}
        // review each client socket
		for (it_client current = _clients.begin() ; current != _clients.end();) {
			Client *current_client = *current;
			if (!current_client) {
					cleanupSocket(&current);
					continue ;
			}
			std::cout << "current: " << *current << std::endl;
			Socket current_socket = current_client->getSocket();
			int pfds_index = findInPfds(current_socket);
			if (pfds_index == -1) {									// Orphan
					handleClientDisconnection(&current);
					continue ;
			}
			short cl_revents = _pfds[pfds_index].revents;
			if (cl_revents & (POLLHUP | POLLNVAL | POLLERR)) {
					handleClientDisconnection(&current);
			} else if (cl_revents & POLLIN) {
				LOG_CMD.debug("client data ");
				handleClientData(&current);
				_pfds[pfds_index].revents = 0; // Reset events
			} else if (cl_revents & POLLOUT) {
				LOG_CMD.debug("client output ");
				handleClientOutput(&current);
				_pfds[pfds_index].revents = 0; // Reset events
			} else {
				++current;
			}
		}
	}
}

/**
 @brief store new client socket, set it non blocking, and subscribe to new POLLIN events
 @param i index of the monitored fds
 */
void Server::handleNewConnection()
{
	LOG_SERVER.info("New Connection !");
    std::string pollEvent;
    if (_pfds[0].revents & POLLIN)
        pollEvent.append("POLLIN ");
    if (_pfds[0].revents & POLLOUT)
        pollEvent.append("POLLOUT ");
    if (_pfds[0].revents & POLLERR)
        pollEvent.append("POLLERR ");
    if (_pfds[0].revents & POLLHUP)
        pollEvent.append("POLLHUP ");
    LOG_SOCKET.debug("Socket " + utils::toString(_pfds[0].fd) + " events: " + pollEvent);

    sockaddr_in clientAddr = {};
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t addrLen = sizeof(clientAddr);
    Socket    newSocket = accept(_pfds[0].fd, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

    if (newSocket != -1) {
        if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1) {
            LOG_ERR.error(std::string("Error while setting a non blocking client socket") +
                          strerror(errno));
            LOG_SOCKET.error(std::string("Error while setting a non blocking client socket") +
                             strerror(errno));
            close(newSocket);
        } else {
            Client* newClient = new Client(newSocket, clientAddr);
			_clients.push_back(newClient);
			listenToSocket(newSocket, POLLIN);
        }
    }
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
void Server::cleanupSocket(it_client* current)
{
	LOG_SERVER.info("Erase Client from list");
	Client* client = **current;
	it_client next = *current;
	std::advance(next, 1);
	if (**current) {
		int pfds_index = findInPfds(client->getSocket());
		_pfds.erase(_pfds.begin() + pfds_index);
		delete **current;
	}
	_clients.erase(*current);
	*current = next;
}

/**
 @brief removes client
 @param clientIndex
*/
void Server::handleClientDisconnection(it_client* current)
{
    Client* client = **current;
    if (!client) {
        LOG_SERVER.error("handleDicsconnect: client not found");
        return;
    }
    socklen_t err = 0;
    socklen_t errsize = sizeof(err);
	Socket socket = client->getSocket();
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &errsize) == 0) {
        if (err == 0) {
            LOG_SERVER.debug("Connection has been closed by client");
        } else {
            LOG_ERR.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
            LOG_SOCKET.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
        }
    }
    LOG_CONN.info(std::string("Client at ") + client->getAddress() + ":" +
                  utils::toString(client->getPort()) + " disconnected");
	cleanupSocket(current);
}

/// @brief attempt receiving bytes from client, parse into a command and execute it
/// enable write notification on client socket if a response has to be sent
/// in case of partial reception (message not ending with \r\n), add to receive buffer
/// @param clientIndex index of monitored fd
void Server::handleClientData(it_client *current)
{
	Client *client = **current;
    Socket  socket = client->getSocket();
	int pdfs_i = findInPfds(socket);

    char buffer[CLIENT_READ_BUFFER_SIZE];
    memset(static_cast<char*>(buffer), 0, CLIENT_READ_BUFFER_SIZE);
    ssize_t bytes = recv(socket, static_cast<char*>(buffer), CLIENT_READ_BUFFER_SIZE - 1, 0);

    if (bytes == 0) {
        LOG_CONN.warning("Connection closed properly");
        handleClientDisconnection(current);
        return;
    } else if (bytes == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERR.error("Reception failed:" + TO_STRING(strerror(errno)));
            handleClientDisconnection(current);
        }
        return;
    } else if (bytes > 0) {
        if (utils::safeAt(buffer, bytes)) {
            utils::safeAt(buffer, bytes) = '\0';
		}
		client->appendToReceiveBuffer(std::string(buffer));
		this->handleCommand(current);
		std::string response = "sECHO: ";
		_pfds[pdfs_i].events |= POLLOUT;
		++(*current);
    }
}

void Server::sendTo(Client *client, const std::string&response) // RRTOUR ERREUR
{
	if (!client) {
		LOG_SERVER.error("didn fint any client to send meeage to");
		return ;
	}
    Socket  socket = client->getSocket();
    LOG_SOCKET.debug(std::string("sending response : " + response));

    size_t bytes = send(socket, response.c_str(), response.length(), 0);

    if (bytes == static_cast<size_t>(-1)) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            LOG_SOCKET.warning("Socket not ready, adding to queue");
            client->appendToSendBuffer(response);
			_pfds[findInPfds(client->getSocket())].events |= POLLOUT;
        } else {
            LOG_ERR.error("Sending failed: " + TO_STRING(errno));
			_pfds[findInPfds(socket)].fd = -1;
        }
    } else if (bytes < response.length()) {
        // send partial
        LOG_SOCKET.warning("Partial sending (" + TO_STRING(bytes) + "/" + TO_STRING(response.length()) + ")");
        client->appendToSendBuffer(response.substr(bytes));
		_pfds[findInPfds(socket)].events |= POLLOUT;
    } else {
        LOG_SERVER.info("Message sent completely");
		_pfds[findInPfds(socket)].events &= ~POLLOUT;
    }
}
/**
 @brief attempt sending the queued messages
 if a message is partially sent, updates send buffer accordingly
 if a message is completely sent, disable write notification for the client fd
 in case of send error, either retry or disconnect the client
 @param clientIndex monitored fd for client
 */
void Server::handleClientOutput(it_client* current)
{
	Client *client = **current;
    if (!client) {
        LOG_SERVER.error(" Client OUtpout: client not found");
         return;
	}
    Socket  clientSocket = client->getSocket();
    std::string sendBuffer = client->getSendBuffer();

    if (client->hasDataToSend()) {

        size_t bytesSent = send(clientSocket, sendBuffer.c_str(), sendBuffer.length(), 0);

        if (bytesSent == static_cast<size_t>(-1)) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_SERVER.warning(std::string("Socket is not ready for sending ... trying later"));

            } else {
                LOG_SERVER.error(std::string("Error sending to client"));
                handleClientDisconnection(current);
            }
        } else if (bytesSent < sendBuffer.length()) {
            LOG_SERVER.warning(std::string("Queue has been partially sent (") +
                               utils::toString(bytesSent) + "/" +
                               utils::toString(sendBuffer.length()) + ")");
            client->setSendBuffer(sendBuffer.substr(bytesSent));
        } else {
            LOG_SERVER.debug(std::string("Message sent normally ... unsubscribing from POLLOUT"));
            client->setSendBuffer("");
			_pfds[findInPfds(clientSocket)].events &= ~POLLOUT;
			++(*current);
        }
    } else {
        LOG_SERVER.debug(std::string("No data to send ... unsubscribing from POLLOUT"));
		_pfds[findInPfds(clientSocket)].events &= ~POLLOUT;
		++(*current);
    }
}

void Server::listenToSocket(Socket toListen, uint32_t flags)
{
    pollfd newPollFd = {.fd = toListen, .events = static_cast<short>(flags), .revents = 0};
    _pfds.push_back(newPollFd);
}

void	Server::handleCommand(it_client* current)
{
		Client* client = **current;
		LOG_SERVER.info("handleCommand start");
		if (!client) {
			LOG_SERVER.error("Client handle Command: client not found");
			 return;
		}
		size_t pos = std::string::npos;
		// tant qu'il y a un \r\n dans le readbuffer du client, executer les commandes
		while ((pos = client->getReceiveBuffer().find("\r\n")) != std::string::npos) {
			//extract the first command from the readBuffer
			std::string line = client->getReceiveBuffer().substr(0, pos);
			//delete the command that has been extracted from the client readbuffer
			client->getReceiveBuffer().erase(0, pos + 2); 
			// parse and create the appropriate command, NULL is returned if a faillure has happen
			ICommand* cmd = parseCommand(*client, line);
			if (cmd) {
				cmd->execute(*this, *client);
				delete cmd;
			}
		}
		LOG_SERVER.info("handleCommand end");
}

// Make and return a command object from the command line if valid command;
// return NULL if command has failed amd print
ICommand* Server::parseCommand(Client& client, std::string line)
{
	LOG_CMD.debug("Parsing of the command: " + line);
	//CmdFactory command_builder(server); TO DO
	CmdFactory command_builder;
	ICommand *cmd = command_builder.makeCommand(*this, client, line);

	return cmd;
}

Client* Server::findClientByNickname(std::string& nickname)
{
    for (it_client client = _clients.begin(); client != _clients.end(); client++) {
        if (*client && nickname == (*client)->getNickName())
            return (*client);
    }
    return NULL;
}

std::string Server::getPassW() const {return _psswd;}
