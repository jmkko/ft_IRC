#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "TcpSocket.hpp"
#include "reply_codes.hpp"

#include <map>
#include <string>

class Server;
class Channel;

// NOLINTNEXTLINE(performance-enum-size)
//  enum ClientStatus {
//      UNAUTHENTICATED, // before anything
//      AUTHENTICATED,	 // after PASS
//      REGISTERED,      // after PASS + NICK + USER
//  };
typedef unsigned short ClientStatus;

#define UNAUTHENTICATED 0b00000 // NOLINT
#define AUTHENTICATED   0b00001 // NOLINT
#define REGISTERED      0b00010 // NOLINT

class Client
{
  public:
    Client(Socket socket, sockaddr_in addr);
    virtual ~Client();

    Socket             get_socket() const;
    const std::string& get_address() const;
    unsigned short     get_port() const;
    std::string        get_send_buffer() const;
    std::string&       get_send_buffer();
    std::string&       get_read_buffer(); // added for cleaning command that has executed
    std::string        get_read_buffer() const;
    bool               has_data_to_send() const;

    std::string  get_nickname() const;
    std::string  get_user_name() const;
    std::string  get_real_name() const;
    std::string  get_userhost() const;
    std::string  get_full_userhost() const;
    ClientStatus get_status() const;

    bool is_registered() const;
    bool is_authenticated() const;
    int  get_nb_joined_channels() const;

    void set_nickname(const std::string& nick);
    void set_user_name(const std::string& userName);
    void set_real_name(const std::string& realName);
    void set_status(ClientStatus status);
    void add_joined_channel(Channel& channel);
    void remove_joined_channel(Channel& channel);
    void remove_from_all_channels();
    void set_send_buffer(const std::string& buffer);

    void append_to_send_buffer(const std::string& msg);
    void append_to_read_buffer(const std::string& msg);

    void broadcast_to_all_channels(Server& server, ReplyCode code, const std::string& params, const std::string& trailing = "");
    Channel* get_channel(const std::string& name);

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

std::ostream& operator<<(std::ostream& os, const Client& c);

#endif
