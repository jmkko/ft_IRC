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
	return os << "Client [socket_fd = " << c.getSocket().getSocket()
		<< " status=" << (c.getStatus() == REGISTERED ? "registered" : "unauthenticated")
		<< " nick=" << c.getNickName() 
		<< " to receive=" << c.getReceiveBuffer().size()
		<< " to send=" << c.getSendBuffer().size()
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
