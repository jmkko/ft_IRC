#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Part.hpp"
#include "Server.hpp"
#include "TcpSocket.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <iostream>
#include <set>

/************************************************************
 *        🥚 CONSTRUCTORS & DESTRUCTOR                        *
 ************************************************************/

Client::Client(Socket socket, sockaddr_in addr) :
    _socket(socket), _addr(addr), _addrStr(TcpSocket::get_address(_addr)), _status(UNAUTHENTICATED)
{
}

Client::~Client(void) {}

/************************************************************
 *        ➕ OPERATORS                                            *
 ************************************************************/

// clang-format off
std::ostream& operator<<(std::ostream& os, const Client& c)
{
    return os << "Client" << "["
        << "socket_fd = " << c.get_socket()
        << " address = " << c.get_address()
        << " status=" << (c.is_registered() ? "registered" : c.is_authenticated() ? "authenticated" : "unauthenticated")
        << " nick=" << c.get_nickname()
        << " to receive=" << c.get_read_buffer().size()
        << " to send=" << c.get_send_buffer().size()
        << " joined channels=" << c.get_nb_joined_channels()
        << "]";
}

/*************************************************************
 *        🛠️ FUNCTIONS                                            *
 *************************************************************/

void Client::append_to_send_buffer(const std::string& msg) { _sendBuffer += msg; }

void Client::append_to_read_buffer(const std::string& msg) { _readBuffer += msg; }

/*************************************************************
 *        👁️‍ GETTERS and SETTERS                                 *
 *************************************************************/

Socket             Client::get_socket() const { return _socket.get_socket(); }

TcpSocket&         Client::get_tcp_socket() { return _socket; }

const std::string& Client::get_address() const { return _addrStr; }

unsigned short     Client::get_port() const { return ntohs(_addr.sin_port); }

std::string        Client::get_nickname() const { return _nickName; }

std::string        Client::get_user_name() const { return _userName; }

std::string        Client::get_real_name() const { return _realName; }

std::string        Client::get_userhost() const { return _userName + "@" + ircConfig.get_name(); }

std::string        Client::get_full_userhost() const { return _nickName + "!" + _userName + "@" + ircConfig.get_name(); }

ClientStatus       Client::get_status() const { return _status; }

std::string        Client::get_send_buffer() const { return _sendBuffer; }
std::string&        Client::get_send_buffer() { return _sendBuffer; }

std::string        Client::get_read_buffer() const { return _readBuffer; }
std::string&       Client::get_read_buffer() { return _readBuffer; }

bool               Client::has_data_to_send() const { return _sendBuffer.empty(); }

bool               Client::is_registered() const { return _status & REGISTERED; }

bool               Client::is_authenticated () const { return _status & AUTHENTICATED; }

int                Client::get_nb_joined_channels() const { return static_cast<int>(_joinedChannels.size()); }

void               Client::set_nickname(const std::string& nick) { _nickName = nick; }

void               Client::set_user_name(const std::string& userName) { _userName = userName; }

void               Client::set_real_name(const std::string& realName) { _realName = realName; }

void               Client::set_status(ClientStatus status) { _status = _status | status; }

void               Client::add_joined_channel(Channel& channel) { _joinedChannels[channel.get_name()] = &channel; }

void               Client::remove_joined_channel(Channel& channel) {
    channel.remove_member(*this);
    _joinedChannels.erase(channel.get_name()); 
}

void                 Client::remove_from_all_channels()
{
    for (std::map<std::string, Channel*>::iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
    {
        it->second->remove_member(*this);
    }
    _joinedChannels.clear();
}
void                 Client::part_all_channels(Server& server, Client& client)
{
    ReplyHandler &rh = ReplyHandler::get_instance(&server);
    LOG_DT_CMD("nb joined channels", _joinedChannels.size());
    for (std::map<std::string, Channel*>::iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
    {
        Channel* channel = it->second;
        channel->broadcast(server, TRANSFER_PART, channel->get_name(), &client);
        rh.process_response(*this, TRANSFER_PART, channel->get_name());
        channel->remove_member(*this);
            if (channel->get_nb_members() == 0) {
                std::map<std::string, Channel *>::iterator it = server.channels.find(channel->get_name());
                if (it != server.channels.end()) {
                    server.channels.erase(it);
                    delete channel;
                }
            }
    }
    _joinedChannels.clear();
}

void      Client::set_send_buffer(const std::string& buffer) { _sendBuffer = buffer; }

Channel* Client::get_channel(const std::string& name) {

    std::map<std::string, Channel*>::iterator chan = _joinedChannels.find(name);
    if (chan != _joinedChannels.end()) {
        return chan->second;
    }
    return NULL;
};

void    Client::broadcast_to_all_channels(Server& server, ReplyCode code, const std::string& params, const std::string& trailing)
{
    LOG_DT_CMD("nb joined channels", _joinedChannels.size());
    std::set<Client*> target;
    for (std::map<std::string, Channel*>::iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); it++) {
        if (it->second) {
            LOG_DT_CMD("to",  it->second->get_name());
        Channel* channel = it->second;
        std::set<Client*> chanMembers = channel->get_members();
        for (std::set<Client*>::iterator itc = chanMembers.begin(); itc != chanMembers.end(); itc++){
          target.insert(*itc);
        }
        }
    }
    ReplyHandler &rh = ReplyHandler::get_instance(&server);
    LOG_DV_CMD(target.size());
    for (std::set<Client *>::iterator it = target.begin(); it != target.end(); ++it) {
        Client *recipient = *it;
        if (recipient == this)
            continue;
        LOG_D_SERVER(recipient->get_nickname() + " received a broadcast from " + _nickName, ircConfig.str(code));
        rh.process_response(*recipient, code, params, this, trailing);
    }
}
