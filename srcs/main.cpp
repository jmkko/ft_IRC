#include "Server.hpp"

int main() {


    
    while (true) {
        // std::cout << "\nPoll en cours sur " << nfds << " socket(s)..." << std::endl;
        int pollResult = poll(fds.data(), nfds, 1000);  // Timeout 1 seconde
        //int pollResult = poll(fds, nfds, 1000);  // Timeout 1 seconde
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
        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0) continue;  // Pas d'événement sur ce socket
            
            // begin ---------------------HandleNewConnextion
            std::cout << "Socket " << fds[i].fd << " events: ";
            if (fds[i].revents & POLLIN) std::cout << "POLLIN ";
            if (fds[i].revents & POLLOUT) std::cout << "POLLOUT ";
            if (fds[i].revents & POLLHUP) std::cout << "POLLHUP ";
            if (fds[i].revents & POLLERR) std::cout << "POLLERR ";
            std::cout << std::endl;
            
            // Socket serveur : nouvelle connexiona
            if (i == 0 && (fds[i].revents & POLLIN)) {
                sockaddr_in clientAddr;
                memset(&clientAddr, 0, sizeof(clientAddr));
                socklen_t addrLen = sizeof(clientAddr);
                SOCKET clientSocket = accept(serverSocket.getSocket(), (sockaddr*)&clientAddr, &addrLen);
                
                if (clientSocket != -1) {
                    // Configurer en non-bloquant
                    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
                        std::cout << "Erreur configuration non-bloquant : " << strerror(errno) << std::endl;
                        close(clientSocket);
                    } else {
                        std::cout << "Nouvelle connexion acceptee: socket " << clientSocket << std::endl;
                        
                        // Ajouter le client
                        Client newClient;
                        newClient.sckt = clientSocket;
                        newClient.addr = clientAddr;
                        clients[clientSocket] = newClient;
                        
                        // Ajouter à la surveillance poll
                        fds[nfds].fd = clientSocket;
                        fds[nfds].events = POLLIN;  // Seulement POLLIN au début
                        fds[nfds].revents = 0;
                        nfds++;
                    }
                }
            }
            // end ---------------------HandleNewConnextion
            
            //
            // Socket client : données ou déconnexion
            else if (i > 0) {
                std::map<SOCKET, Client>::iterator clientIt = clients.find(fds[i].fd);
                // Remove socket si deconnexion
                if (clientIt == clients.end()) {
                    // Socket orphelin socket existe dans fsd[] mais pas dans clients[] " deconnexion brutales ou bug"
                    close(fds[i].fd);
                    // Supprimer de la liste
                    for (int j = i; j < nfds - 1; j++) {
                        fds[j] = fds[j + 1];
                    }
                    nfds--;
                    i--; // Réexaminer cette position
                    continue;
                }
                
                Client& client = clientIt->second;
                const std::string clientAddress = TcpSocket::getAddress(client.addr);
                const unsigned short clientPort = ntohs(client.addr.sin_port);
                bool disconnect = false;
                
                // Gérer les erreurs
                if (fds[i].revents & POLLERR) {
                    socklen_t err;
                    socklen_t errsize = sizeof(err);
                    if (getsockopt(client.sckt, SOL_SOCKET, SO_ERROR, (char*)&err, &errsize) == 0) {
                        std::cout << "Erreur socket : " << strerror(err) << std::endl;
                    }
                    disconnect = true;
                }
                // Client fermé la connexion POLLHUP le client ferme sa connection POLLNVAL le client perd sa connexion ex: plus de wifi
                else if (fds[i].revents & (POLLHUP | POLLNVAL)) {
                    std::cout << "Client ferme la connexion" << std::endl;
                    disconnect = true;
                }
                // Données reçues
                // begin -----------------------handle ClientData
                else if (fds[i].revents & POLLIN) {
                    char buffer[512];
                    memset(buffer, 0, sizeof(buffer));
                    int bytesReceived = recv(client.sckt, buffer, sizeof(buffer)-1, 0);
                    
                    if (bytesReceived == 0) {
                        std::cout << "Connexion fermee proprement" << std::endl;
                        disconnect = true;
                    } else if (bytesReceived == -1) {
                        if (errno != EAGAIN && errno != EWOULDBLOCK) {
                            std::cout << "Erreur reception : " << strerror(errno) << std::endl;
                            disconnect = true;
                        }
                    } else {
                        if (buffer[bytesReceived]==EOF)
                            std::cout << " ctrl + D ";
                        buffer[bytesReceived] = '\0';
                        std::cout << "[" << clientAddress << ":" << clientPort << "] " << buffer << std::endl;

                        // if (strncmp(buffer,"HELLO\n",6)==0)
                        //     std::cout << "VALID" << std::endl;
                        // std::cout << strncmp(buffer,"HELLO\n",6) << std::endl;
                        // Préparer la réponse
                        std::string response = "sECHO: ";
                        response += buffer;
                        std::cout << response << std::endl;
                        

                        // Tentative d'envoi immédiat
                        int bytesSent = send(client.sckt, response.c_str(), response.length(), 0);
                        
                        if (bytesSent == -1) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                // Socket pas prêt, mettre en queue
                                std::cout << "Socket pas pret, ajout a la queue" << std::endl;
                                client.messageQueue += response;
                                fds[i].events |= POLLOUT;
                            } else {
                                std::cout << "Erreur envoi : " << strerror(errno) << std::endl;
                                disconnect = true;
                            }
                        } else if (bytesSent < (int)response.length()) {
                            // Envoi partiel
                            std::cout << "Envoi partiel (" << bytesSent << "/" << response.length() << ")" << std::endl;
                            client.messageQueue += response.substr(bytesSent);
                            fds[i].events |= POLLOUT;
                        } else {
                            std::cout << "Message envoye completement" << std::endl;
                        }
                    }
                }
                // Prêt à envoyer
                else if (fds[i].revents & POLLOUT) {
                    if (client.hasDataToSend()) {
                        int bytesSent = send(client.sckt, client.messageQueue.c_str(), client.messageQueue.length(), 0);
                        
                        if (bytesSent == -1) {
                            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                                std::cout << "Erreur envoi queue : " << strerror(errno) << std::endl;
                                disconnect = true;
                            }
                        } else {
                            std::cout << "Envoye de la queue : " << bytesSent << " bytes" << std::endl;
                            client.messageQueue.erase(0, bytesSent);
                            
                            // Queue vide ? Désactiver POLLOUT
                            if (!client.hasDataToSend()) {
                                fds[i].events &= ~POLLOUT;
                                std::cout << "Queue videe, POLLOUT desactive" << std::endl;
                            }
                        }
                    } else {
                        // Pas de données mais POLLOUT activé
                        fds[i].events &= ~POLLOUT;
                        std::cout << "POLLOUT desactive (pas de donnees)" << std::endl;
                    }
                }
                // end-----------------------hqndle ClientData
                
                // Nettoyer si déconnexion
                if (disconnect) {
                    std::cout << "Deconnexion de [" << clientAddress << ":" << clientPort << "]" << std::endl;
                    close(fds[i].fd);
                    clients.erase(clientIt);
                    
                    // Supprimer de la liste poll
                    for (int j = i; j < nfds - 1; j++) {
                        fds[j] = fds[j + 1];
                    }
                    nfds--;
                    i--; // Ajuster l'index
                }
            }
            
            // Réinitialiser les événements
            fds[i].revents = 0;
        }
    }

    //////////////////////////////////////////////////
    //close(serverSocket);
    return 0;
}
