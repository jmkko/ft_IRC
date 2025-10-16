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
#include <cstddef>
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
 *		🥚 CONSTRUCTORS & DESTRUCTOR*
 ************************************************************/

Server::Server(const unsigned short port, const std::string& password) : _psswd(password), _name(ircConfig.get_name())
{
    try {
        _serverSocket.tcp_bind(port);
        _serverSocket.tcp_listen();
    } catch (std::exception& e) {
        std::cout << e.what();
        exit(1);
    }
    LOG_SERVER.info("Server " + _name + " start at port :" + utils::to_string(port));
    std::cout << "\n";

    _listen_to_socket(_serverSocket.get_socket(), POLLIN);
}

Server::~Server() { _clean(); }

/*************************************************************
 *		👁️‍ GETTERS and SETTERS	                     *
 *************************************************************/

std::string Server::get_password() const { return _psswd; }
std::string Server::get_name() const { return _name; }

/*************************************************************
 *		🛠️ FUNCTIONS                                 *
 *************************************************************/

void Server::start()
{
    while (globalSignal != SIGINT && globalSignal != SIGABRT) {
        int pollResult = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT); // Timeout 1 second
        if (pollResult == -1) {
            LOG_W_SERVER("Poll failed", strerror(errno));
            break;
        }
        if (pollResult == 0) {
            continue;
        }

        LOG_DT_SERVER("event detected", pollResult);
        for (int i = 0; i < static_cast<int>(_pfds.size()); i++) {
            if (globalSignal == SIGINT && globalSignal == SIGABRT)
                break;

            if (i >= static_cast<int>(_pfds.size()))
                break;

            if (i == 0 && (_pfds[i].revents & POLLIN)) {
                _pfds[i].revents = 0; // Reset events
                _handle_new_connection(i);
            } else if (i > 0) {
                std::map<Socket, Client*>::iterator clientIt = _clients.find(_pfds[i].fd);
                if (clientIt == _clients.end()) {
                    cleanup_socket_and_client(i--);
                    continue;
                }

                // Store revents before processing and reset it
                short events     = _pfds[i].revents;
                _pfds[i].revents = 0;

                if (events & (POLLHUP | POLLNVAL | POLLERR)) {
                    _handle_client_disconnection(i--);
                } else if (events & POLLIN) {
                    Socket originalSocket = _pfds[i].fd;
                    size_t originalSize   = _pfds.size();
                    _handle_client_input(i);
                    if (_pfds.size() < originalSize || i >= static_cast<int>(_pfds.size()) || _pfds[i].fd != originalSocket) {
                        i--; // Client was removed, adjust index
                    }
                } else if (events & POLLOUT) {
                    _handle_client_output(i);
                }
            }
        }
    }
    _clean();
}

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
    Socket    socket  = accept(_serverSocket.get_socket(), reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

    if (socket != -1) {
        if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
            LOG_ERR.error(std::string("Error while setting a non blocking client socket") + strerror(errno));
            LOG_SOCKET.error(std::string("Error while setting a non blocking client socket") + strerror(errno));
            close(socket);
        } else {
            Client* newClient = new Client(socket, clientAddr); // NOLINT
            LOG_CONN.info(std::string("New connection accepted on socket ") + utils::to_string(socket) + " => "
                          + utils::to_string(*newClient));
            _clients[socket] = newClient;

            _listen_to_socket(socket, POLLIN);
        }
    }
}

void Server::_handle_client_disconnection(int pfdIndex)
{
    Socket  socket = _pfds[pfdIndex].fd;
    Client* client = _clients[socket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }

    socklen_t err     = 0;
    socklen_t errsize = sizeof(err);
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &errsize) == 0) {
        if (err == 0) {
            LOG_SERVER.debug("connection has been closed by client");
        } else {
            LOG_CONN.warning(std::string("socket error : ") + strerror(static_cast<int>(err)));
            LOG_SOCKET.warning(std::string("socket error : ") + strerror(static_cast<int>(err)));
        }
    }
    LOG_CONN.info(std::string("Client at ") + client->get_address() + ":" + utils::to_string(client->get_port())
                  + " disconnected");
    cleanup_socket_and_client(pfdIndex);
}

void Server::_handle_client_input(int pfdIndex)
{
    LOG_dt_SERVER("");
    Socket  socket = _pfds[pfdIndex].fd;
    Client* client = _clients[socket];
    if (!client) {
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
        bool clientDisconnected = this->_handle_commands(pfdIndex);
        if (clientDisconnected) {
            return;
        }
    }
}

void Server::_handle_client_output(int pfdIndex)
{
    Socket  socket = _pfds[pfdIndex].fd;
    Client* client = _clients[socket];
    if (!client) {
        LOG_SERVER.error("client not found");
        return;
    }

    std::string sendBuffer = client->get_send_buffer();
    if (!sendBuffer.empty()) {

        ssize_t bytesSent = send(socket, sendBuffer.c_str(), sendBuffer.length(), 0);

        if (bytesSent == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_SERVER.warning(std::string("Socket is not ready for sending ... trying later"));

            } else {
                LOG_SERVER.error(std::string("Error sending to client"));
                _handle_client_disconnection(pfdIndex);
            }
        } else if (static_cast<size_t>(bytesSent) < sendBuffer.length()) {
            LOG_SERVER.warning(std::string("Queue has been partially sent (") + utils::to_string(bytesSent) + "/"
                               + utils::to_string(sendBuffer.length()) + ")");
            client->set_send_buffer(sendBuffer.substr(bytesSent));
        } else {
            client->get_send_buffer().clear();
            LOG_d_SERVER("Message sent, unsuscribe from POLLOUT");
            _pfds[pfdIndex].events &= ~POLLOUT;
        }
    } else {
        _pfds[pfdIndex].events &= ~POLLOUT;
    }
}

bool Server::_handle_commands(int pfdIndex)
{
    Socket      socket = _pfds[pfdIndex].fd;
    Client*     client = _clients[socket];
    size_t      pos    = std::string::npos;
    std::string cmdName;

    while ((pos = client->get_read_buffer().find("\r\n")) != std::string::npos) {
        std::string line = client->get_read_buffer().substr(0, pos);
        client->get_read_buffer().erase(0, pos + 2);
        ICommand* cmd = _parse_command(*client, line);
        if (cmd) {
            cmd->execute(*this, *client);
            delete cmd;
            std::istringstream iss(line);
            iss >> cmdName;
            if (cmdName == "QUIT")
                return true;
        }
    }
    return false;
}

ICommand* Server::_parse_command(Client& client, std::string line)
{
    LOG_CMD.receiving(__FILE_NAME__, __FUNCTION__, line, &client);
    CmdFactory commandBuilder;
    ICommand*  cmd = commandBuilder.make_command(*this, client, line);

    return cmd;
}

Client* Server::find_client_by_nickname(const std::string& nickname)
{
    for (std::map<Socket, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (nickname == it->second->get_nickname())
            return it->second;
    }
    return NULL;
}

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
    LOG_dt_SERVER("Server::add_envents_of --> " + EVENT_TO_STR(event) + " on index: " + TO_STRING(index));
    if (index >= 0) {
        _pfds[index].events = static_cast<short>(_pfds[index].events | event); // ADD to existing events
    }
}

void Server::_listen_to_socket(Socket toListen, uint32_t flags)
{
    pollfd newPollFd = {.fd = toListen, .events = static_cast<short>(flags), .revents = 0};
    _pfds.push_back(newPollFd);
}

void Server::_clean()
{
    LOG_SERVER.debug(std::string("cleaning ") + TO_STRING(_clients.size()) + " clients");

    for (int i = static_cast<int>(_pfds.size()) - 1; i >= 1; --i) {
        cleanup_socket_and_client(i);
    }

    cleanup_channels();

    globalSignal = 0;
    LOG_SERVER.debug("Server cleaned and ready for reuse");
}

void Server::stop()
{
    LOG_SERVER.debug("Server stop requested");
    globalSignal = SIGINT;
}

void Server::cleanup_socket_and_client(int pfdIndex)
{
    Client* c = _clients[_pfds[pfdIndex].fd];
    close(_pfds[pfdIndex].fd);
    _clients.erase(_pfds[pfdIndex].fd);
    if (c) {
        if (!c->get_nickname().empty())
            _clientsByNick.erase(c->get_nickname());
        LOG_SERVER.debug("cleanup: deleting client");
        c->remove_from_all_channels();
        delete c;
    }
    _pfds.erase(_pfds.begin() + pfdIndex);
}

void Server::cleanup_channels()
{
    LOG_dt_SERVER(std::string("cleaning ") + TO_STRING(channels.size()) + " channels");
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second; // NOLINT
    }
    channels.clear();
}

std::vector<Client*> Server::find_clients_by_pattern(const std::string& pattern) const
{
    std::vector<Client*> result;
    for (std::map<Socket, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (utils::MatchPattern(pattern)(it->second)) {
            LOG_D_CMD("pattern " + pattern + " matched", it->second->get_nickname());
            result.push_back(it->second);
        }
    }
    return result;
}

Channel* Server::find_channel_by_name(const std::string& name)
{
    std::map<std::string, Channel*>::iterator chan = channels.find(name);
    if (chan != channels.end()) {
        return chan->second;
    }
    return NULL;
}
