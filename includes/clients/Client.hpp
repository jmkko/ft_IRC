/**
 * @file Client.hpp
 * @brief Client class
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "TcpSocket.hpp"
#include "reply_codes.hpp"

#include <map>
#include <string>

class Server;
class Channel;

typedef unsigned short ClientStatus;

/**
 * @def UNAUTHENTICATED
 * @brief Represents the unauthenticated state of a client.
 *
 * This macro is used as a bitmask flag to indicate that a client
 * has not yet been authenticated on the server.
 * - Value: `0b00000` (binary 00000, decimal 0)
 */
#define UNAUTHENTICATED 0b00000 // NOLINT

/**
 * @def AUTHENTICATED
 * @brief Represents the authenticated state of a client.
 *
 * This macro is used as a bitmask flag to indicate that a client
 * has been authenticated on the server after sending a successful PASS.
 * @see #Pass
 * - Value: `0b00000` (binary 00001, decimal 1)
 */
#define AUTHENTICATED 0b00001 // NOLINT

/**
 * @def REGISTERED
 * @brief Represents the registered state of a client.
 *
 * This macro is used as a bitmask flag to indicate that a client
 * has been authenticated on the server after sending successfull PASS, NICK, USER.
 * @see #User #Nick
 * - Value: `0b00000` (binary 00010, decimal 3)
 */
#define REGISTERED 0b00010 // NOLINT

/**
 * @class Client
 * @brief represents a Client interacting with the server
 * @details responsabilities
 * - handle connection buffers
 * - manage user related information
 */
class Client
{
  public:
    /**
     * @brief Construct a new Client object
     *
     * @param socket
     * @param addr
     */
    Client(Socket socket, sockaddr_in addr);

    /**
     * @brief Destroy the Client object
     *
     */
    virtual ~Client();

    /**
     * @brief Get the fd of socket associated with the client
     *
     * @return Socket (an int reprenting socket file descriptor)
     */
    Socket get_socket() const;

    /**
     * @brief Get the tcp socket object associated with the client
     * 
     * @return TcpSocket& 
     */
    TcpSocket& get_tcp_socket();

    /**
     * @brief Get the address object
     *
     * @return const std::string&
     */
    const std::string& get_address() const;

    /**
     * @brief Get the port object
     *
     * @return unsigned short
     */
    unsigned short get_port() const;

    /**
     * @brief Get the send buffer object
     *
     * @return std::string
     */
    std::string get_send_buffer() const;

    /**
     * @brief Get the send buffer object
     *
     * @return std::string&
     */
    std::string& get_send_buffer();

    /**
     * @brief Get the read buffer object as a reference
     *
     * @return std::string&
     */
    std::string& get_read_buffer();

    /**
     * @brief Get the read buffer object
     *
     * @return std::string
     */
    std::string get_read_buffer() const;

    /**
     * @brief
     *
     * @return true if send buffer has content
     * @return false otherwise
     */
    bool has_data_to_send() const;

    /**
     * @brief Get the nickname, created with #Nick command
     *
     * @return std::string
     */
    std::string get_nickname() const;

    /**
     * @brief Get the user name, created with #User command
     *
     * @return std::string
     */
    std::string get_user_name() const;

    /**
     * @brief Get the real name, created with #User command
     *
     * @return std::string
     */
    std::string get_real_name() const;

    /**
     * @brief Get the userhost formatted like `<user>@<host>`
     *
     * @return std::string
     */
    std::string get_userhost() const;

    /**
     * @brief Get the full userhost formatted like `<nick>!<user>@<host>`
     *
     * @return std::string
     */
    std::string get_full_userhost() const;

    /**
     * @brief Get the authentication and registration status
     *
     * @return ClientStatus
     */
    ClientStatus get_status() const;

    /**
     * @brief
     * @see REGISTERED
     * @return true if client is registered
     * @return false otherwise
     */
    bool is_registered() const;

    /**
     * @brief
     * @see AUTHENTICATED
     * @return true if client is authenticated
     * @return false otherwise
     */
    bool is_authenticated() const;

    /**
     * @brief Get the number of joined channels
     *
     * @return int
     */
    int get_nb_joined_channels() const;

    /**
     * @brief Set the nickname
     *
     * @param nick
     */
    void set_nickname(const std::string& nick);

    /**
     * @brief Set the user name
     *
     * @param userName
     */
    void set_user_name(const std::string& userName);

    /**
     * @brief Set the real name
     *
     * @param realName
     */
    void set_real_name(const std::string& realName);

    /**
     * @brief Set the status
     *
     * @param status
     */
    void set_status(ClientStatus status);

    /**
     * @brief store a trace of new joined channel
     *
     * @param channel
     */
    void add_joined_channel(Channel& channel);

    /**
     * @brief remove a channel from joined channels
     *
     * @param channel
     */
    void remove_joined_channel(Channel& channel);

    /**
     * @brief remove all joined channels
     *
     */
    void remove_from_all_channels();

    /**
     * @brief Set the send buffer object to a value
     *
     * @param buffer
     */
    void set_send_buffer(const std::string& buffer);

    /**
     * @brief add a message to the sending buffer
     *
     * @param msg
     */
    void append_to_send_buffer(const std::string& msg);

    /**
     * @brief add a message to the reading buffer
     *
     * @param msg
     */
    void append_to_read_buffer(const std::string& msg);

    /**
     * @brief sends a message to all channels members the client is member of
     *
     * @param server
     * @param code
     * @param params
     * @param trailing
     */
    void broadcast_to_all_channels(Server& server, ReplyCode code, const std::string& params, const std::string& trailing = "");

    /**
     * @brief Get the joined channel matching exactly the name
     *
     * @param name
     * @return Channel* or NULL if not found
     */
    Channel* get_channel(const std::string& name);

    /**
     * @brief Client leave all channels
     * @remark utils for JOIN 0
     *
     * @param server
     * @param client
     */
    void part_all_channels(Server& server, Client& client);

  private:
    Client(const Client& other);
    Client&                         operator=(const Client& other);
    TcpSocket                       _socket;
    sockaddr_in                     _addr;
    std::string                     _addrStr;
    std::string                     _nickName;
    std::string                     _userName;
    std::string                     _realName;
    ClientStatus                    _status;
    std::string                     _sendBuffer;
    std::string                     _readBuffer;
    std::map<std::string, Channel*> _joinedChannels;
};

/**
 * @brief displays Client info
 * @details overload of output operator
 * 
 * @param os 
 * @param c 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream& os, const Client& c);

#endif
