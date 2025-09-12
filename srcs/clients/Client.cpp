#include "Client.hpp"

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

Client::Client(TcpSocket& socket) : _socket(socket), _status(UNAUTHENTICATED) {}

Client::Client(const Client& inst) : _socket(inst._socket), _status(inst._status) {}

Client::~Client(void) {}

/************************************************************
*		➕ OPERATORS											*
************************************************************/

Client& Client::operator=(const Client& inst) 
{
	if (this != &inst)
	{
		_socket = inst._socket;
		_status = inst._status;
		_nickName = inst._nickName;
		_userName = inst._userName;
		_realName = inst._realName;
		_sendBuffer = inst._sendBuffer;
		_receiveBuffer = inst._receiveBuffer;
	}
	return (*this);
}

std::ostream&	operator<<(std::ostream& os, const Client& c)
{
	return os << CYAN << "Client" << NC << "["
		<< BWHITE << "socket_fd = " << NC << c.getSocket().getSocket()
		<< BWHITE << " status=" << NC << (c.getStatus() == REGISTERED ? "registered" : "unauthenticated")
		<< BWHITE << " nick=" << NC << c.getNickName() 
		<< BWHITE << " to receive=" << NC << c.getReceiveBuffer().size()
		<< BWHITE << " to send=" << NC << c.getSendBuffer().size()
		<< BWHITE << " joined channels=" << NC << c.getNbJoinedChannels()
		<< "]"
		<< std::endl;
}

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

void		Client::appendToSendBuffer(const std::string& msg)
{
	_sendBuffer += msg;
}

/*************************************************************
*		👁️‍ GETTERS and SETTERS				 				*
*************************************************************/

TcpSocket&		Client::getSocket() const
{
	return _socket;
}

std::string		Client::getNickName() const
{
	return _nickName;
}

std::string		Client::getUserName() const
{
	return _userName;
}

std::string		Client::getRealName() const
{
	return _realName;
}

ClientStatus	Client::getStatus() const
{
	return _status;
}

std::string		Client::getSendBuffer() const
{
	return _sendBuffer;
}

std::string		Client::getReceiveBuffer() const
{
	return _receiveBuffer;
}

bool			Client::hasDataToSend() const
{
	return _sendBuffer.empty();
}

bool			Client::isRegistered() const
{
	return _status == REGISTERED;
}

int				Client::getNbJoinedChannels() const
{
	return _joinedChannels.size();
}

void			Client::setNickName(const std::string& nick)
{
	_nickName = nick;
}

void			Client::setUserName(const std::string& userName)
{
	_userName = userName;
}

void			Client::setRealName(const std::string& realName)
{
	_realName = realName;
}

void			Client::setStatus(ClientStatus status)
{
	_status = status;
}

void			Client::addJoinedChannel(Channel& channel)
{
	_joinedChannels[channel.getName()] = &channel;
}

void			Client::removeJoinedChannel(Channel& channel)
{
	_joinedChannels.erase(channel.getName());
}