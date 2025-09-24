#include "Server.hpp"

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
    _psswd(psswd), _name(irc_config.get_name())
{
    try {
        _serverSocket.tcpBind(port);
        _serverSocket.tcpListen();
    } catch (std::exception& e) {
        std::cout << e.what();
        return; // need improve for exit program
    }
    LOG_SERVER.info("Server " + _name + " start at port :" + utils::toString(port));
    std::cout << "\n";

    // Socket serveur : surveiller nouvelles connexions
    listenToSocket(_serverSocket.getSocket(), POLLIN);
}

Server::~Server()
{
    LOG_SERVER.debug("Server dtor");
    stop();
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief runs the server loop, regularly checking activity through `poll()`
 */
void Server::start()
{
    while (true) {
        if (globalSignal == SIGINT || globalSignal == SIGABRT)
            stop();
        int pollResult = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT); // Timeout 1 second
        if (pollResult == -1) {
            LOG_SERVER.error("Poll failed: " + TO_STRING(strerror(errno)));
            break;
        }
        if (pollResult == 0) {
            continue;
        }

        // review each client socket
        LOG_SERVER.debug(utils::toString(pollResult) + "event(s) detected");

        for (int i = 0; i < static_cast<int>(_pfds.size()); i++) {
            // new connection
            if (i == 0 && (_pfds[i].revents & POLLIN)) {
                handleNewConnection(i);
            }
            // Client socket : data or disconnection
            else if (i > 0) {
                std::map<Socket, Client*>::iterator clientIt = _clients.find(_pfds[i].fd);
                // orphelan socket
                if (clientIt == _clients.end()) {
                    cleanupSocketAndClients(i--);
                    continue;
                }
                if (_pfds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
                    handleClientDisconnection(i--);
                } else if (_pfds[i].revents & POLLIN) {
                    handleClientData(i);
                } else if (_pfds[i].revents & POLLOUT) {
                    handleClientOutput(i);
                }
            }
            _pfds[i].revents = 0; // Reset events
        }
    }
}

/**
 @brief store new client socket, set it non blocking, and subscribe to new POLLIN events
 @param i index of the monitored fds
 */
void Server::handleNewConnection(int i)
{
    std::string pollEvent;
    if (_pfds[i].revents & POLLIN)
        pollEvent.append("POLLIN ");
    if (_pfds[i].revents & POLLOUT)
        pollEvent.append("POLLOUT ");
    if (_pfds[i].revents & POLLHUP)
        pollEvent.append("POLLHUP ");
    if (_pfds[i].revents & POLLERR)
        pollEvent.append("POLLERR ");
    LOG_SOCKET.debug("Socket " + utils::toString(_pfds[i].fd) + " events: " + pollEvent);

    sockaddr_in clientAddr = {};
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t addrLen = sizeof(clientAddr);
    Socket    socket =
        accept(_serverSocket.getSocket(), reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

    if (socket != -1) {
        if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
            LOG_ERR.error(std::string("Error while setting a non blocking client socket") +
                          strerror(errno));
            LOG_SOCKET.error(std::string("Error while setting a non blocking client socket") +
                             strerror(errno));
            close(socket);
        } else {
            Client* newClient = new Client(socket, clientAddr);
            LOG_CONN.info(std::string("New connection accepted on socket ") +
                          utils::toString(socket) + " => " + utils::toString(*newClient));
            _clients[socket] = newClient;

            listenToSocket(socket, POLLIN);
        }
    }
}


/**
 @brief removes client
 @param pfd_index
*/
void Server::handleClientDisconnection(int pfd_index)
{
    Socket  socket = _pfds[pfd_index].fd;
    Client* client = _clients[socket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }

    socklen_t err = 0;
    socklen_t errsize = sizeof(err);
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &errsize) ==
        0) {
        if (err == 0) {
            LOG_SERVER.debug("connection has been closed by client");
        } else {
            LOG_ERR.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
            LOG_SOCKET.error(std::string("socket error : ") + strerror(static_cast<int>(err)));
        }
    }
    LOG_CONN.info(std::string("Client at ") + client->getAddress() + ":" +
                  utils::toString(client->getPort()) + " disconnected");
    cleanupSocketAndClients(pfd_index);
}

/**
 * @brief attempt receiving bytes from client, parse into a command and execute it
 enable write notification on client socket if a response has to be sent
 in case of partial reception (message not ending with \r\n), add to receive buffer
 * @param pfd_index index of monitored fd
 */
void Server::handleClientData(int pfd_index)
{
    LOG_SERVER.debug("Server#handleClientData");
    Socket  socket = _pfds[pfd_index].fd;
    Client* client = _clients[socket];
    if (!client) {
        LOG_ERR.error("handle receive... client not found");
        return;
    }

    char buffer[CLIENT_READ_BUFFER_SIZE];
    memset(static_cast<char*>(buffer), 0, CLIENT_READ_BUFFER_SIZE);
    ssize_t bytesReceived =
        recv(socket, static_cast<char*>(buffer), CLIENT_READ_BUFFER_SIZE - 1, 0);

    if (bytesReceived == 0) {
        LOG_CONN.warning("Connection closed properly");
        handleClientDisconnection(pfd_index);
        return;
    } else if (bytesReceived == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERR.error("Reception failed:" + TO_STRING(strerror(errno)));
            handleClientDisconnection(pfd_index);
        }
        return;
    } else {
        if (utils::safeAt(buffer, bytesReceived))
            utils::safeAt(buffer, bytesReceived) = '\0';
        client->appendToReceiveBuffer(std::string(buffer));
        this->handleCommand(pfd_index);
    }
}

/**
@brief attempt sending the queued messages
if a message is partially sent, updates send buffer accordingly
if a message is completely sent, disable write notification for the client fd
in case of send error, either retry or disconnect the client
@param pfd_index monitored fd for client
*/
void Server::handleClientOutput(int pfd_index)
{
    LOG_SERVER.info("Server#handleClientOutput");
    Socket  socket = _pfds[pfd_index].fd;
    Client* client = _clients[socket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }

    std::string sendBuffer = client->getSendBuffer();
    if (!sendBuffer.empty()) {

    	LOG_SERVER.info("Client.Buffer: " + sendBuffer);
        int bytesSent = send(socket, sendBuffer.c_str(), sendBuffer.length(), 0);

        if (bytesSent == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_SERVER.warning(std::string("Socket is not ready for sending ... trying later"));

            } else {
                LOG_SERVER.error(std::string("Error sending to client"));
                handleClientDisconnection(pfd_index);
            }
        } else if (static_cast<size_t>(bytesSent) < sendBuffer.length()) {
            LOG_SERVER.warning(std::string("Queue has been partially sent (") +
                               utils::toString(bytesSent) + "/" +
                               utils::toString(sendBuffer.length()) + ")");
            client->setSendBuffer(sendBuffer.substr(bytesSent));
        } else {
            LOG_SERVER.debug(std::string("Message sent normally ... unsubscribing from POLLOUT"));
            client->getSendBuffer().clear();
            _pfds[pfd_index].events &= ~POLLOUT;
        }
    } else {
        LOG_SERVER.debug(std::string("No data to send ... unsubscribing from POLLOUT"));
        _pfds[pfd_index].events &= ~POLLOUT;
    }
}

void Server::handleCommand(int pfd_index)
{
    Socket  socket = _pfds[pfd_index].fd;
    Client* client = _clients[socket];

    size_t pos = std::string::npos;
    // tant qu'il y a un \r\n dans le readbuffer du client, executer les commandes
    while ((pos = client->getReceiveBuffer().find("\r\n")) != std::string::npos) {
        // extract the first command from the readBuffer
        std::string line = client->getReceiveBuffer().substr(0, pos);
        // delete the command that has been extracted from the client readbuffer
        client->getReceiveBuffer().erase(0, pos + 2);
        // parse and create the appropriate command, NULL is returned if a faillure has happen
        ICommand* cmd = parseCommand(*client, line);
        if (cmd) {
            cmd->execute(*this, *client);
            delete cmd;
        }
    }
}

// Make and return a command object from the command line if valid command;
// return NULL if command has failed amd print
ICommand* Server::parseCommand(Client& client, std::string line)
{
    CmdFactory command_builder;
    ICommand*  cmd = command_builder.makeCommand(*this, client, line);

    return cmd;
}

Client* Server::findClientByNickname(std::string& nickname)
{
    for (std::map<Socket, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (nickname == it->second->getNickName())
            return it->second;
    }
    return NULL;
}

std::string Server::getPassW() const { return _psswd; }

/**
 * @brief [TODO:return index of client in _pfds[]]
 *
 * @param client [TODO:parameter]
 * @return [TODO:-1 if not found or bad socket]
 */
int	Server::indexOf(Client& client) {
	Socket socket = client.getSocket();

	if (socket == -1)
		return -1;
	for (size_t i = 0; i < _pfds.size(); ++i ) {
		if (_pfds[i].fd == socket)
			return static_cast<int>(i);
	}
	return -1;
}

void Server::addEventsOf(Client& client, short event) {
    int index = indexOf(client);
    if (index >= 0) {
        _pfds[index].events |= event;
    }
}

void Server::listenToSocket(Socket toListen, uint32_t flags)
{
    pollfd newPollFd = {.fd = toListen, .events = static_cast<short>(flags), .revents = 0};
    _pfds.push_back(newPollFd);
}

/**
 * @brief [TODO:description]
 */
void Server::stop()
{
    LOG_SERVER.debug(std::string("cleaning ") + TO_STRING(_pfds.size()) +
                     " sockets and their associated clients");
    for (size_t i = 0; i < _pfds.size(); ++i)
        cleanupSocketAndClients(i);
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
void Server::cleanupSocketAndClients(int i)
{
    Client* c = _clients[_pfds[i].fd];
    _clients.erase(_pfds[i].fd);
    if (c) {
        if (!c->getNickName().empty())
            _clientsByNick.erase(c->getNickName());
        delete c;
    }
    _pfds.erase(_pfds.begin() + i);
}
