/**
 * @file Server.hpp
 * @brief Server class
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include "TcpSocket.hpp"

#include <map>
#include <poll.h>
#include <string>
#include <vector>

class ICommand;
class TcpSocket;
class Client;
class Channel;

struct BotState {
    // Client* client;
    int         socketfd;
    Channel*    targetChannel;
    std::string subCommand;
    std::string pendingMsg;
    bool        readyToSend;
    BotState() : socketfd(-1), targetChannel(), readyToSend(false) {}
};

/**
 * @class Server
 * @brief Represents an IRC server
 * @details responsabilities are
 *
 * - initializing the server with provided arguments (port, password) + config file
 * - running the server loop, using `poll` to detect events on sockets, ie
 *   - registering a new Client connection
 *   - reading Client messages and processing them (through #CmdFactory and #ICommand implementations)
 *   - sending messages back
 * - gracefully handling client disconnection and server termination (through signals)
 *
 */
class Server
{
  public:
    /**
     *
     * @details
     * - assign a socket binding a port with an address
     * - put socket in listening state @see Server::_listen_to_socket
     * @pre the `psswd` argument should have been checked with @ref Utils::check_password()
     * @pre the `port`argument should have been checked with @ref Utils::check_port()
     * @param port valid port number
     * @param password valid password
     * @todo delegate initialization to helper function, as `exit` is not authorized
     */
    Server(const unsigned short port, const std::string& password);

    /**
     * @brief Destroy the Server object
     *
     */
    ~Server();

    /**
     * @brief Get the password object
     *
     * @return std::string
     * @remark required by #Pass - not very secure yet we couldn't implement a proper hash function
     */
    std::string get_password() const;

    /**
     * @brief Get the port
     *
     * @return int
     */
    int get_port() const;

    /**
     * @brief Get the socket fd object
     *
     * @return int
     */
    int get_socket_fd() const;

    /**
     * @brief Get the name object
     * @details also accessible through #Config
     * @return std::string
     */
    std::string get_name() const;

    /**
     * @brief runs the server loop, regularly checking activity through `poll()`
     * @details

     - update list of sockets (in struct pfd) having events the server subscribed at
     - loop through them and
       - if read event on server client socket -> @see Server::_handle_new_connection
       - if read event on client socket -> @see Server::_handle_client_input
       - if send event on client socket -> @see Server::_handle_client_output
       - if error events on client socket -> @see Server::_handle_client_disconnection
     - clean data (clients, channels, sockets) once loop is ended

       @remark throughout the loop, orphan sockets are cleaned and signal are checked through a global variable
     */
    void start();

    /**
     * @brief setting @ref globalSignal to SIGINT
     * @remark the main infinite loop STOP and -> @see do _clean()
     */
    void stop();
    /**
     * @brief find a client by his nickname and return a #Client
     *
     * @param nick
     * @return on succes the @ref Client
     */
    Client* find_client_by_nickname(const std::string& nick);

    /**
     * @brief find a channel by his name
     *
     * @param name
     * @return @ref Channel*
     */
    Channel* find_channel_by_name(const std::string& name);

    /**
     * @brief util function to get index of client in pfd array
     *
     * @param client
     * @return int
     */
    int index_of(Client& client);

    void update_bot_state(
        Socket socketfd, Channel* targetChannel, const std::string& subCommand, const std::string& botReply, bool readyToSend);

    /**
     * @brief update events on socket the server is subscribed to
     *
     * @param client
     * @param event
     */
    void                 add_events_of(Client& client, int event);
    std::vector<Client*> find_clients_by_pattern(const std::string& pattern) const;

    /**
     * @brief remove #TcpSocket and #Client a pfd is associated with
     * @details
     * - close the socket fd
     * - remove entry from _clients
     * - remove entry from _clientsByNick
     * - delete client instance
     * - remove entry from _fds (the pollfd list)
     * @param pfdIndex
     */
    void cleanup_socket_and_client(int pfdIndex);

    /**
     * @brief remove registered channels
     *
     */
    void cleanup_channels();
    void cleanup_bot(Socket so);

    std::map<std::string, Channel*> channels;

  private:
    TcpSocket                      _serverSocket;
    std::vector<pollfd>            _pfds;
    std::map<Socket, Client*>      _clients;
    std::map<Socket, BotState>     _bots;
    std::map<std::string, Client*> _clientsByNick;
    std::string                    _psswd;
    std::string                    _name;
    unsigned short                 _port;

    Server();
    Server(const Server&);

    Server& operator=(const Server& inst);

    /**
     * @brief global cleaning
     * @details need to clean in reverse order (cf #Server::_cleanup_socket_and_client)
     */
    void _clean();

    /**
     * @brief store new client socket, set it non blocking, and subscribe to new POLLIN events
     * @param i index of the monitored fds
     */
    void _handle_new_connection(int pfdIndex);

    /**
     * @brief removes client
     * @param pfdIndex
     */
    void _remove_client(Socket s);

    /**
     * @brief handles if a client is disconnected
     * @remark check if the client has closed the connection
     * and @see cleanup_socket_and_client
     *
     * @param pfdIndex socket fd of the Client
     */
    void _handle_client_disconnection(int pfdIndex);

    /**
     * @brief attempt receiving bytes from client, parse into a command and execute it
     * enable write notification on client socket if a response has to be sent
     * in case of partial reception (message not ending with \r\n), add to receive buffer
     * @param pfdIndex index of monitored fd
     */
    void _handle_client_input(int pfdIndex);

    void _handle_bot_input(int pfdIndex, Client* botClient, BotState& state);

    /**
     * @brief attempt sending the queued messages
     * if a message is partially sent, updates send buffer accordingly
     * if a message is completely sent, disable write notification for the client fd
     * in case of send error, either retry or disconnect the client
     * @param pfdIndex monitored fd for client
     */
    void _handle_client_output(int pfdIndex);
    /**
     * @brief listen a socket by adding him to the list of fd to listen
     * _pfds vector of pollfd
     *
     * @param toListen Socket to listen
     * @param flags like POLLIN
     */
    void _listen_to_socket(Socket toListen, uint32_t flags);
    /**
     * @brief return a command through #CmdFactory::make_command
     *
     * @param c
     * @param line
     * @return ICommand*
     * @todo integrate directlt into Server::_handle_commands
     */
    ICommand* _parse_command(Client& c, std::string line);
    /**
     * @brief
     * @details
      - loop through messages (terminated by `\r\n`) in read buffer
        - update read buffer
        - try parsing with @see Server::_parse_command
        - execute
        - stop iteration if command is #Quit
     * @param pfdIndex
     * @return true if command was QUIT
     * @return false if client is still connected
     */
    bool _handle_commands(int pfdIndex);
};

#endif
