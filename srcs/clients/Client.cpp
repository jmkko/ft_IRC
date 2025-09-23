#include "Client.hpp"

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Client::Client(Socket socks, sockaddr_in addr) :
    _socket(socks), _addr(addr), _addrStr(TcpSocket::getAddress(_addr)), _status(UNAUTHENTICATED)
{
	//_socket.setEvents(POLLIN);
}

Client::Client(const Client& inst) :
    _socket(inst._socket),
    _addr(inst._addr),
    _addrStr(inst._addrStr),
    _nickName(inst._nickName),
    _userName(inst._userName),
    _realName(inst._realName),
    _status(inst._status),
    _sendBuffer(inst._sendBuffer),
    _receiveBuffer(inst._receiveBuffer)
{
	//+socket.setEvents(inst.getEvents());
	//_socket.setRevents(inst.getRevent());
}

Client::~Client(void) {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Client& Client::operator=(const Client& inst)
{
    if (this != &inst) {
        _socket = inst._socket;
        _addr = inst._addr;
        _addrStr = inst._addrStr;
        _status = inst._status;
        _nickName = inst._nickName;
        _userName = inst._userName;
        _realName = inst._realName;
        _sendBuffer = inst._sendBuffer;
        _receiveBuffer = inst._receiveBuffer;
    }
    return (*this);
}

// clang-format off
std::ostream& operator<<(std::ostream& os, const Client& c)
{
    return os << "Client" << "["
		<< "socket_fd = " << c.getSocket()
		<< " address = " << c.getAddress()
    	<< " status=" << (c.getStatus() == REGISTERED ? "registered" : "unauthenticated")
		<< " nick=" << c.getNickName()
		<< " to receive=" << c.getReceiveBuffer().size()
		<< " to send=" << c.getSendBuffer().size()
		<< " joined channels=" << c.getNbJoinedChannels()
		<< "]";
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Client::appendToSendBuffer(const std::string& msg) { _sendBuffer += msg; }

void Client::appendToReceiveBuffer(const std::string& msg) { _receiveBuffer += msg; }

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

Socket             Client::getSocket() const { return _socket.getSocket(); }

const std::string& Client::getAddress() const { return _addrStr; }

unsigned short     Client::getPort() const { return ntohs(_addr.sin_port); }

std::string        Client::getNickName() const { return _nickName; }

std::string        Client::getUserName() const { return _userName; }

std::string        Client::getRealName() const { return _realName; }

ClientStatus       Client::getStatus() const { return _status; }

std::string        Client::getSendBuffer() const { return _sendBuffer; }

std::string        Client::getReceiveBuffer() const { return _receiveBuffer; }
std::string&        Client::getReceiveBuffer() { return _receiveBuffer; }

bool               Client::hasDataToSend() const { return _sendBuffer.empty(); }

bool               Client::isRegistered() const { return _status == REGISTERED; }

int                Client::getNbJoinedChannels() const { return static_cast<int>(_joinedChannels.size()); }

void               Client::setNickName(const std::string& nick) { _nickName = nick; }

void               Client::setUserName(const std::string& userName) { _userName = userName; }

void               Client::setRealName(const std::string& realName) { _realName = realName; }

void               Client::setStatus(ClientStatus status) { _status = status; }

void               Client::addJoinedChannel(Channel& channel) { _joinedChannels[channel.getName()] = &channel; }

void               Client::removeJoinedChannel(Channel& channel) { _joinedChannels.erase(channel.getName()); }

void               Client::setSendBuffer(const std::string& buffer) { _sendBuffer = buffer; }

// NIL //
//short		Client::getRevents() const {_socket.getRevents();};
//void		Client::setRevents(short event) {_socket.setRevents(event);};
//
//short		Client::getEvents() const {_socket.getEvents();};
//void		Client::setEvents(short event) {_socket.setEvents(event);};

