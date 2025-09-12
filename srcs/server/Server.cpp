#include "Server.hpp"

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
    std::cout << "Server start at port :" << port << std::endl;
    std::cout << std::endl;

    // Socket serveur : surveiller nouvelles connexions
    pollfd tfd;
    tfd.fd = _serverSocket.getSocket();
    tfd.events = POLLIN;
    tfd.revents = 0;
    _fds.push_back(tfd);
}

Server::~Server() {
    for (int i = 0; i < _fds.size(); ++i){
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
        std::cout << pollResult << " evenement(s) detecte(s)" << std::endl;

        // Examiner chaque socket
        for (int i = 0; i < _fds.size(); i++) {
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
                }
            }
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

