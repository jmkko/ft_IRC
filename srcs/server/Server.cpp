#include "Server.hpp"

Server::Server(const unsigned short port, const std::string & psswd):_nfds(1), _psswd(psswd) {
    _name = "hazardous.irc.serv";
    try{
    _serverSocket.tcpBind(port);
    _serverSocket.tcpListen();
    }
    catch (std::exception& e){
        std::cout<< e.what() << std::endl;
        return ; // need improve for exit program
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

Server::~Server(){}

void Server::start(){
    while (true) {
        int pollResult = poll(_fds.data(), _nfds, 1000);  // Timeout 1 seconde
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
        for (int i = 0; i < _nfds; i++) {
            //new connection 
            if (i == 0 && (_fds[i].revents & POLLIN)) {
                handleNewConnection();
            }
            // Client socket : data or disconnection
            else if (i > 0) {
                std::map<SOCKET, Client>::iterator clientIt = _clients.find(_fds[i].fd);
                //orphelan socket 
                if (clientIt == _clients.end()) {
                    cleanupSocket(i);
                    i--;
                    continue;
                }
                if (_fds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
                handleClientDisconnection();
             }


    
}

void Server::handleNewConnection(){
            std::cout << "Socket " << _fds[i].fd << " events: ";
            if (_fds[i].revents & POLLIN) std::cout << "POLLIN ";
            if (_fds[i].revents & POLLOUT) std::cout << "POLLOUT ";
            if (_fds[i].revents & POLLHUP) std::cout << "POLLHUP ";
            if (_fds[i].revents & POLLERR) std::cout << "POLLERR ";
            std::cout << std::endl;
            
                sockaddr_in clientAddr;
                memset(&clientAddr, 0, sizeof(clientAddr));
                socklen_t addrLen = sizeof(clientAddr);
                SOCKET clientSocket = accept(serverSocket.getSocket(), (sockaddr*)&clientAddr, &addrLen);
                
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
                        clients[clientSocket] = newClient;
                        
                        // add the new  client to  the watching fd list ti poll
                        // only POLLIN for first time
                        _fds[_nfds].fd = clientSocket;
                        _fds[_nfds].events = POLLIN;
                        _fds[_nfds].revents = 0;
                        _nfds++;
                    }
                }
}

void Server::cleanupSocket(int i){
    close(_fds[i].fd);
    for (int j = i; j < _nfds - 1; j++) {
        _fds[j] = _fds[j + 1];
    }
    _nfds--;
}
