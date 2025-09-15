#include "Server.hpp"
#include <sstream>
#include "utils.hpp"

/******************************************************************************
 * Initialize the Server
 * assign a socket, a port and an adress
 * register the password
 * add the socket of the server to the pollfd vector
 ******************************************************************************/
Server::Server(const unsigned short port, const std::string &psswd) : _psswd(psswd) {
    _name = "hazardous.irc.serv";
    try {
        _serverSocket.tcpBind(port);
        _serverSocket.tcpListen();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return; // need improve for exit program
    }
    LOG_SERVER.info("Server " + _name + "start at port :" + utils::toString(port));
    std::cout << std::endl;

    // Socket serveur : surveiller nouvelles connexions
    pollfd tfd;
    tfd.fd = _serverSocket.getSocket();
    tfd.events = POLLIN;
    tfd.revents = 0;
    _fds.push_back(tfd);
}

Server::~Server() {
    for (int i = 0; i < (int)_fds.size(); ++i) {
        cleanupSocket(i);
    }
}

/******************************************************************************
 * start the server
 * poll allow to know if there is activity on socket
 *
 ******************************************************************************/
void Server::start() {
    while (true) {
        int pollResult = poll(_fds.data(), _fds.size(), 1000); // Timeout 1 second
        if (pollResult == -1) {
            std::cout << "Erreur poll : " << strerror(errno) << std::endl;
            break;
        }
        if (pollResult == 0) {
            // std::cout << "Timeout - aucune activite" << std::endl;
            continue;
        }
        std::cout << std::endl;
        LOG_SERVER.debug(utils::toString(pollResult)+ "event(s) detected");

        // review each client socket
        for (int i = 0; i < (int)_fds.size(); i++) {
            // new connection
            if (i == 0 && (_fds[i].revents & POLLIN)) {
                handleNewConnection(i);
            }
            // Client socket : data or disconnection
            else if (i > 0) {
                std::map<SOCKET, Client>::iterator clientIt = _clients.find(_fds[i].fd);
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

void Server::handleNewConnection(int i) {
    std::cout << "Socket " << _fds[i].fd << " events: ";
    if (_fds[i].revents & POLLIN)
        std::cout << "POLLIN ";
    if (_fds[i].revents & POLLOUT)
        std::cout << "POLLOUT ";
    if (_fds[i].revents & POLLHUP)
        std::cout << "POLLHUP ";
    if (_fds[i].revents & POLLERR)
        std::cout << "POLLERR ";
    std::cout << std::endl;

    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t addrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(_serverSocket.getSocket(), (sockaddr *)&clientAddr, &addrLen);

    if (clientSocket != -1) {
        // non blocking socket
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
            std::cout << "Erreur configuration non-bloquant : " << strerror(errno) << std::endl;
            close(clientSocket);
        } else {
            std::cout << "Nouvelle connexion acceptee: socket " << clientSocket << std::endl;

            // Add new CLient
            Client newClient;
            newClient.sckt = clientSocket;
            newClient.addr = clientAddr;
            _clients[clientSocket] = newClient;

            // add the new  client to  the watching fd list ti poll
            // only POLLIN for first time
            pollfd newPollFd;
            newPollFd.fd = clientSocket;
            newPollFd.events = POLLIN;
            newPollFd.revents = 0;
            _fds.push_back(newPollFd);
        }
    }
}

/******************************************************************************
 * Cleanup a socket
 * close the socket fd
 * remove from client list : the _clients map
 * remove from pollfd list : the _fds vector
 *
 ******************************************************************************/
void Server::cleanupSocket(int i) {
    close(_fds[i].fd);
    _clients.erase(_fds[i].fd);
    _fds.erase(_fds.begin() + i);
}

void Server::handleClientDisconnection(int clientIndex) {
    SOCKET clientSocket = _fds[clientIndex].fd;
    const unsigned short clientPort = ntohs(_clients[clientSocket].addr.sin_port);
    const std::string clientAddress = TcpSocket::getAddress(_clients[clientSocket].addr);
    socklen_t err;
    socklen_t errsize = sizeof(err);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, (char *)&err, &errsize) == 0) {
        std::cout << "Erreur socket : " << strerror(err) << std::endl;
    } else {
        std::cout << "Client ferme la connexion" << std::endl;
    }
    cleanupSocket(clientIndex);
    std::cout << "Deconnexion de [" << clientAddress << ":" << clientPort << "]" << std::endl;
}

void Server::handleClientData(int clientIndex) {
    SOCKET clientSocket = _fds[clientIndex].fd;
    Client &client = _clients[clientSocket];
    const unsigned short clientPort = ntohs(client.addr.sin_port);
    const std::string clientAddress = TcpSocket::getAddress(client.addr);
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived == 0) {
        std::cout << "Connexion fermee proprement" << std::endl;
        handleClientDisconnection(clientIndex);
        return;
    } else if (bytesReceived == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cout << "Erreur reception : " << strerror(errno) << std::endl;
            handleClientDisconnection(clientIndex);
        }
        return;
    } else {
        buffer[bytesReceived] = '\0';
        std::cout << "[" << clientAddress << ":" << clientPort << "] " << buffer << std::endl;

        // if (strncmp(buffer,"HELLO\n",6)==0)
        //     std::cout << "VALID" << std::endl;
        // std::cout << strncmp(buffer,"HELLO\n",6) << std::endl;
        // Préparer la réponse
        std::string response = "sECHO: ";
        response += buffer;
        std::cout << response << std::endl;

        // Immediate sending attempt
        sendToClient(clientIndex, response);
    }
}

void Server::sendToClient(int clientIndex, const std::string &response) {
    SOCKET clientSocket = _fds[clientIndex].fd;
    Client &client = _clients[clientSocket];

    int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);

    if (bytesSent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Socket pas prêt, mettre en queue
            std::cout << "Socket pas pret, ajout a la queue" << std::endl;
            client.messageQueue += response;
            _fds[clientIndex].events |= POLLOUT;
        } else {
            std::cout << "Erreur envoi : " << strerror(errno) << std::endl;
            handleClientDisconnection(clientIndex);
        }
    } else if (bytesSent < (int)response.length()) {
        // send partial
        std::cout << "Envoi partiel (" << bytesSent << "/" << response.length() << ")" << std::endl;
        client.messageQueue += response.substr(bytesSent);
        _fds[clientIndex].events |= POLLOUT;
    } else {
        std::cout << "Message envoye completement" << std::endl;
    }
}

void Server::handleClientOutput(int clientIndex) {
    SOCKET clientSocket = _fds[clientIndex].fd;
    Client &client = _clients[clientSocket];
    if (client.hasDataToSend()) {
        int bytesSent = send(clientSocket, client.messageQueue.c_str(), client.messageQueue.length(), 0);

        if (bytesSent == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "Erreur envoi queue : " << strerror(errno) << std::endl;
                handleClientDisconnection(clientIndex);
            }
        } else {
            std::cout << "Envoye de la queue : " << bytesSent << " bytes" << std::endl;
            client.messageQueue.erase(0, bytesSent);

            // Queue vide ? Désactiver POLLOUT
            if (!client.hasDataToSend()) {
                _fds[clientIndex].events &= ~POLLOUT;
                std::cout << "Queue videe, POLLOUT desactive" << std::endl;
            }
        }
    } else {
        // no data but POLLOUT set
        _fds[clientIndex].events &= ~POLLOUT;
        std::cout << "POLLOUT desactive (pas de donnees)" << std::endl;
    }
}
