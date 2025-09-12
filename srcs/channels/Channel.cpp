#include "Channel.hpp"
#include "consts.hpp"

/************************************************************
*		📁 CLASS METHODS									*
************************************************************/

/// @brief checks validity according to RFC
bool				_isValidChannelName(const std::string& name)
{
	return name.length() > 1 && name[0] == '#' && name.length() < CHAN_NAME_MAX_LEN;
}

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

/// @throw exception if name is invalid 
Channel::Channel(const std::string& name) : 
	_topic("this is the topic. please update it with TOPIC"), 
	_userLimit(NO_LIMIT), 
	_isInviteOnly(false), 
	_isTopicChangeRestricted(false), 
	_members(), 
	_operators(), 
	_invites()
{
	setName(name);
}

Channel::Channel(const Channel& inst) : 
	_name(inst._name), 
	_topic(inst._topic), 
	_userLimit(inst._userLimit), 
	_isInviteOnly(inst._isInviteOnly), 
	_isTopicChangeRestricted(inst._isTopicChangeRestricted), 
	_members(inst._members), 
	_operators(inst._operators), 
	_invites(inst._invites)
	{}

Channel::Channel(void) : 
	_name(""), 
	_topic("this is the topic. please update it with TOPIC"), 
	_userLimit(NO_LIMIT), 
	_isInviteOnly(false), 
	_isTopicChangeRestricted(false), 
	_members(), 
	_operators(), 
	_invites()
{}

Channel::~Channel() {}

/************************************************************
*		➕ OPERATORS											*
************************************************************/

Channel& Channel::operator=(const Channel& inst) 
{
	if (this != &inst)
	{
		_name = inst._name;
	 	_topic = inst._topic;
		_userLimit = inst._userLimit;
		_isInviteOnly = inst._isInviteOnly;
		_isTopicChangeRestricted = inst._isTopicChangeRestricted;
		_members = inst._members;
		_operators = inst._operators;
		_invites = inst._invites;
	}
	return (*this);
}

std::ostream&	operator<<(std::ostream& os, const Channel& c)
{
	return os << CYAN << "Channel" << NC << "["
	<< BWHITE << " name = " << NC << c.getName()
	<< BWHITE << " topic=" << NC << c.getTopic()
	<< BWHITE << " userLimit=" << NC << c.getUserLimit()
	<< BWHITE << " invite only=" << NC << (c.isInviteOnly() ? "true" : "false")
	<< BWHITE << " topic change restricted=" << NC << (c.isTopicChangeRestricted() ? "true" : "false")
	<< BWHITE << "]"
	<< std::endl;
}

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

void				Channel::broadcast(const std::string& message, Client* sender) const
{
	for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		Client* recipient = *it;
		if (sender && recipient == sender)
			continue;
		recipient->appendToSendBuffer(message);
	}
}

/*************************************************************
*		👁️‍ GETTERS and SETTERS				 				*
*************************************************************/

const std::string&			Channel::getName() const
{
	return _name;
}

const std::string&			Channel::getTopic() const
{
	return _topic;
}

bool				Channel::isMember(Client& client) const
{
	return _members.find(&client) != _members.end();
}

bool				Channel::isOperator(Client& client) const
{
	return _operators.find(&client) != _operators.end();
}

bool				Channel::isMember(Client& client) const
{
	return _invites.find(&client) != _invites.end();
}

int					Channel::getUserLimit() const
{
	return _userLimit;
}

bool				Channel::isInviteOnly() const
{
	return _isInviteOnly;
}

bool				Channel::isTopicChangeRestricted() const
{
	return _isTopicChangeRestricted;
}


void				Channel::setName(const std::string& name)
{
	if (_isValidChannelName(name))
		_name = name;
	else
		throw std::runtime_error("invalid channel name");
}

void				Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

void				Channel::setUserLimit(int limit)
{
	if (limit < -1)
		_userLimit = -1;
	else
		_userLimit = limit;
}

void				Channel::setIsInviteOnly(bool isInviteOnly)
{
	_isInviteOnly = isInviteOnly;
}

void				Channel::setIsTopicChangeRestricted(bool isRestricted)
{
	_isTopicChangeRestricted = isTopicChangeRestricted;
}

void				Channel::addMember(Client& client)
{
		_invites.insert(&client);
}

void				Channel::addMember(Client& client)
{
	if (_userLimit == NO_LIMIT || _members.size() < static_cast<size_t>(_userLimit))
		throw std::runtime_error("channel max capacity reached");
	if (_isInviteOnly)
	{
		if (isInvited(client))
			_invites.erase(&client);
		else	
			throw std::runtime_error("client was not invited");
	}
	if (MAX_JOINED_CHANNELS == NO_LIMIT || client.getNbJoinedChannels() < MAX_JOINED_CHANNELS)
		_members.insert(&client);
	else
		throw std::runtime_error("joined channels limit reached");
}

void				Channel::removeMember(Client& client)
{
	_members.erase(&client);
}

void				Channel::makeOperator(Client& client)
{
	if (isMember(client))
	{
		_operators.insert(&client);
	}
	else
		throw std::runtime_error("client can't be made operator as it is not a channel member");
}