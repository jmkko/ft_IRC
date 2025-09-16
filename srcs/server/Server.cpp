#include "Server.hpp"
#include "LogManager.hpp"
#include "utils.hpp"
#include <cstring>
#include <sstream>

/******************************************************************************
 * Initialize the Server IRC
 * assign a socket, a port and an adress
 * register the password
 * add the socket of the server to the pollfd vector
 ******************************************************************************/
Server::Server(const unsigned short port, const std::string& psswd) : _psswd(psswd)
{
    _name = "hazardous.irc.serv";
    try {
        _serverSocket.tcpBind(port);
        _serverSocket.tcpListen();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return; // need improve for exit program
    }
    LOG_SERVER.info("Server " + _name + "start at port :" + utils::toString(port));
    std::cout << std::endl;

    // Socket serveur : surveiller nouvelles connexions
    listenToSocket(_serverSocket.getSocket(), POLLIN);
}

Server::~Server()
{
    for (int i = 0; i < (int)_fds.size(); ++i) {
        cleanupSocket(i);
    }
}

/******************************************************************************
 * start the server
 * poll allow to know if there is activity on socket
 *
 ******************************************************************************/
void Server::start()
{
    while (true) {
        int pollResult = poll(_fds.data(), _fds.size(), 1000); // Timeout 1 second
        if (pollResult == -1) {
            LOG_ERR.error("Poll failed: " + TO_STRING(strerror(errno)));
            LOG_SERVER.error("Critical: Poll system failed");
            break;
        }
        if (pollResult == 0) {
            // std::cout << "Timeout - aucune activite" << std::endl;
            continue;
        }
        std::cout << std::endl;
        LOG_SERVER.debug(utils::toString(pollResult) + "event(s) detected");

        // review each client socket
        for (int i = 0; i < static_cast<int>(_fds.size()); i++) {
            // new connection
            if (i == 0 && (_fds[i].revents & POLLIN)) {
                handleNewConnection(i);
            }
            // Client socket : data or disconnection
            else if (i > 0) {
                std::map<Socket, Client*>::iterator clientIt = _clients.find(_fds[i].fd);
                // orphelan socket
                if (clientIt == _clients.end()) {
                    cleanupSocket(i);
                    --i;
                    continue;
                }
                if (_fds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
                    handleClientDisconnection(i);
                    --i;
                } else if (_fds[i].revents & POLLIN) {
                    handleClientData(i);
                } else if (_fds[i].revents & POLLOUT) {
                    handleClientOutput(i);
                }
            }
            _fds[i].revents = 0; // Reset events
        }
    }
}

/// @brief store new client socket, set it non blocking, and subscribe to new POLLIN events
/// @param i index of the monitored fds
void Server::handleNewConnection(int i)
{
    std::string pollEvent;
    if (_fds[i].revents & POLLIN)
        pollEvent.append("POLLIN ");
    if (_fds[i].revents & POLLOUT)
        pollEvent.append("POLLOUT ");
    if (_fds[i].revents & POLLHUP)
        pollEvent.append("POLLHUP ");
    if (_fds[i].revents & POLLERR)
        pollEvent.append("POLLERR ");
    LOG_SOCKET.debug("Socket " + utils::toString(_fds[i].fd) + " events: " + pollEvent);

    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t addrLen = sizeof(clientAddr);
    Socket    clientSocket = accept(_serverSocket.getSocket(), (sockaddr*)&clientAddr, &addrLen);

    if (clientSocket != -1) {
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
            LOG_ERR.error(std::string("Error while setting a non blocking client socket") + strerror(errno));
            LOG_SOCKET.error(std::string("Error while setting a non blocking client socket") + strerror(errno));
            close(clientSocket);
        } else {
            Client* newClient = new Client(clientSocket, clientAddr);
            LOG_CONN.info(std::string("New connection accepted on socket ") + utils::toString(clientSocket) + " => " +
                          utils::toString(*newClient));
            _clients[clientSocket] = newClient;
            listenToSocket(clientSocket, POLLIN);
        }
    }
}

/// @brief cleanup a socket and associated Client
/// close the socket fd
/// remove entry from _clients
/// remove entry from _clientsByNick
/// delete client instance
/// remove entry from _fds (the pollfd list)
/// @param i index of monitored fd
void Server::cleanupSocket(int i)
{
    Client* c = _clients[_fds[i].fd];
    close(_fds[i].fd);
    _clients.erase(_fds[i].fd);
    if (c) {
        if (!c->getNickName().empty())
            _clientsByNick.erase(c->getNickName());
        delete c;
    }
    _fds.erase(_fds.begin() + i);
}

/// @brief removes client
/// @param clientIndex
void Server::handleClientDisconnection(int clientIndex)
{
    Socket  clientSocket = _fds[clientIndex].fd;
    Client* client = _clients[clientSocket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }

    socklen_t err;
    socklen_t errsize = sizeof(err);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, (char*)&err, &errsize) == 0) {
        if (err == 0) {
            LOG_SERVER.debug("connection has been closed by client");
        } else {
            LOG_ERR.error(std::string("socket error : ") + strerror(err));
            LOG_SOCKET.error(std::string("socket error : ") + strerror(err));
        }
    }
    LOG_CONN.info(std::string("Client at ") + client->getAddress() + ":" + utils::toString(client->getPort()) +
                  " disconnected");
    cleanupSocket(clientIndex);
}

// TODO change return true if we find a way to pass \r through netcat
static bool hasCommandEnding(char* msg)
{
    std::string s(msg);
    if (s.size() < 2)
        return false;
    std::string::reverse_iterator it = s.rbegin();
    char                          last = *it;
    char                          beforeLast = *(++it);
    if (beforeLast != '\r' || last != '\n')
        return false;
    return true;
}

/// @brief attempt receiving bytes from client, parse into a command and execute it
/// enable write notification on client socket if a response has to be sent
/// in case of partial reception (message not ending with \r\n), add to receive buffer
/// @param clientIndex index of monitored fd
void Server::handleClientData(int clientIndex)
{
    LOG_SERVER.debug("Server#handleClientData");
    Socket  clientSocket = _fds[clientIndex].fd;
    Client* client = _clients[clientSocket];
    if (!client) {
        LOG_ERR.error("handle receive... client not found");
        return;
    }

    char buffer[CLIENT_READ_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived == 0) {
        LOG_CONN.warning("Connection closed properly");
        handleClientDisconnection(clientIndex);
        return;
    } else if (bytesReceived == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERR.error("Reception failed:" + TO_STRING(strerror(errno)));
            handleClientDisconnection(clientIndex);
        }
        return;
    } else {
        if (buffer[bytesReceived])
            buffer[bytesReceived] = '\0';
        LOG_SERVER.debug(std::string("[") + client->getAddress() + ":" + utils::toString(client->getPort()) +
                         "] : " + buffer);

        // if the buffer ends with \r\n -> parse as command
        // otherwise it can be a partial message with CTRL+D -> we append and wait \r\n
        if (hasCommandEnding(buffer)) {
            // parse to command and execute
            // ICommand* cmd = parseCommand(buffer)
            // cmd->execute(*this, client)
            // delete cmd;
            std::string response = "sECHO: ";
            response += buffer;
            _fds[clientIndex].events |= POLLOUT;
            sendToClient(clientIndex, response);
        } else
            client->appendToReceiveBuffer(std::string(buffer));
    }
}

void Server::sendToClient(int clientIndex, const std::string& response)
{
    LOG_SOCKET.debug(std::string("sending response : " + response));
    Socket  clientSocket = _fds[clientIndex].fd;
    Client* client = _clients[clientSocket];

    int     bytesSent = send(clientSocket, response.c_str(), response.length(), 0);

    if (bytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            LOG_SOCKET.warning("Socket not ready, adding to queue");
            client->appendToSendBuffer(response);
            _fds[clientIndex].events |= POLLOUT;
        } else {
            LOG_ERR.error("Sending failed: " + TO_STRING(errno));
            handleClientDisconnection(clientIndex);
        }
    } else if (bytesSent < (int)response.length()) {
        // send partial
        LOG_SOCKET.warning("Partial sending (" + TO_STRING(bytesSent) + "/" + TO_STRING(response.length()) + ")");
        client->appendToSendBuffer(response.substr(bytesSent));
        // _fds[clientIndex].events |= POLLOUT;
    } else {
        LOG_SERVER.info("Message sent completely");
        _fds[clientIndex].events &= ~POLLOUT;
    }
}

/// @brief attempt sending the queued messages
/// if a message is partially sent, updates send buffer accordingly
/// if a message is completely sent, disable write notification for the client fd
/// in case of send error, either retry or disconnect the client
/// @param clientIndex monitored fd for client
void Server::handleClientOutput(int clientIndex)
{
    LOG_SERVER.debug("Server#handleClientOutput");
    Socket  clientSocket = _fds[clientIndex].fd;
    Client* client = _clients[clientSocket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }
    std::string sendBuffer = client->getSendBuffer();

    if (client->hasDataToSend()) {

        int bytesSent = send(clientSocket, sendBuffer.c_str(), sendBuffer.length(), 0);

        if (bytesSent == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_SERVER.warning(std::string("Socket is not ready for sending ... trying later"));

            } else {
                LOG_SERVER.error(std::string("Error sending to client"));
                handleClientDisconnection(clientIndex);
            }
        } else if (bytesSent < static_cast<int>(sendBuffer.length())) {
            LOG_SERVER.warning(std::string("Queue has been partially sent (") + utils::toString(bytesSent) + "/" +
                               utils::toString(sendBuffer.length()) + ")");
            client->setSendBuffer(sendBuffer.substr(bytesSent));
        } else {
            LOG_SERVER.debug(std::string("Message sent normally ... unsubscribing from POLLOUT"));
            client->setSendBuffer("");
            _fds[clientIndex].events &= ~POLLOUT;
        }
    } else {
        LOG_SERVER.debug(std::string("No data to send ... unsubscribing from POLLOUT"));
        _fds[clientIndex].events &= ~POLLOUT;
    }
}

void Server::listenToSocket(Socket toListen, uint32_t flags)
{
    pollfd newPollFd = {.fd = toListen, .events = flags, .revents = 0};
    _fds.push_back(newPollFd);
}

ICommand* parseCommand(char* buffer)
{
    (void)buffer;
    return NULL;
}
