#include "Channel.hpp"

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

/// @brief checks validity according to RFC
bool Channel::is_valid_channel_name(const std::string& name)
{
	size_t posColon = name.find(':');
	size_t posBell = name.find('\a');
	if (posColon != std::string::npos && posBell != std::string::npos)
		return false;
	if (name[0] != '#' && name[0] != '&' && name[0] != '+' && name[0] != '!')
		return false;
	return name.length() > 1 && name.length() <= ircConfig.get_chan_name_max_len();
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/// @throw exception if name is invalid
Channel::Channel(const std::string& name) :
	_topic("No topic is set"),
	_mode(CHANMODE_INIT),
	_userLimit(NO_LIMIT),
	_isInviteOnly(false),
	_isTopicChangeRestricted(false),
	_members(),
	_invites(),
	_operators()
{
	set_name(name);
}

Channel::Channel(const Channel& other) :
	_name(other._name),
	_topic(other._topic),
	_mode(other._mode),
	_userLimit(other._userLimit),
	_isInviteOnly(other._isInviteOnly),
	_isTopicChangeRestricted(other._isTopicChangeRestricted),
	_members(other._members),
	_invites(other._invites),
	_operators(other._operators)
{
}

Channel::Channel(void) :
	_name(""),
	_topic("No topic is set"),
	_mode(CHANMODE_INIT),
	_userLimit(NO_LIMIT),
	_isInviteOnly(false),
	_isTopicChangeRestricted(false),
	_members(),
	_invites(),
	_operators()
{
}

Channel::~Channel() {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other) {
		_name = other._name;
		_topic = other._topic;
		_userLimit = other._userLimit;
		_isInviteOnly = other._isInviteOnly;
		_isTopicChangeRestricted = other._isTopicChangeRestricted;
		_members = other._members;
		_operators = other._operators;
		_invites = other._invites;
	}
	return (*this);
}

// clang-format off
std::ostream&	operator<<(std::ostream& os, const Channel& c)
{
	return os << "Channel" << "["
	<< " name = " << c.get_name()
	<< " topic=" << c.get_topic()
	<< " userLimit=" << c.get_user_limit()
	<< " invite only=" << (c.is_invite_only() ? "true" : "false")
	<< " topic change restricted=" << (c.is_topic_change_restricted() ? "true" : "false")
	<< "]";
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Channel::broadcast(const std::string& message, Client* sender) const
{
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        Client* recipient = *it;
        if (sender && recipient == sender)
            continue;
        recipient->append_to_send_buffer(message);
    }
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

const std::string& Channel::get_name() const { return _name; }

const std::string& Channel::get_topic() const { return _topic; }

bool               Channel::is_member(Client& client) const { return _members.find(&client) != _members.end(); }

bool               Channel::is_operator(Client& client) const { return _operators.find(&client) != _operators.end(); }

bool               Channel::is_invited(Client& client) const { return _invites.find(&client) != _invites.end(); }

int                Channel::get_user_limit() const { return _userLimit; }

bool               Channel::is_invite_only() const { return _isInviteOnly; }

bool               Channel::is_topic_change_restricted() const { return _isTopicChangeRestricted; }

void               Channel::set_name(const std::string& name)
{
    if (Channel::is_valid_channel_name(name))
        _name = name;
    else
        throw std::runtime_error("invalid channel name");
}

void Channel::set_topic(const std::string& topic) { _topic = topic; }

void Channel::set_user_limit(int limit)
{
    if (limit < -1)
        _userLimit = -1;
    else
        _userLimit = limit;
}

void Channel::set_is_invite_only(bool is_invite_only) { _isInviteOnly = is_invite_only; }

void Channel::set_is_topic_change_restricted(bool isRestricted) { _isTopicChangeRestricted = isRestricted; }

void Channel::invite_client(Client& client) { _invites.insert(&client); }

void Channel::add_member(Client& client)
{
    if (is_member(client))
        return;
    if (_userLimit != NO_LIMIT && _members.size() >= static_cast<size_t>(_userLimit))
        throw std::runtime_error("channel max capacity reached");
    if (_isInviteOnly) {
        if (is_invited(client))
            _invites.erase(&client);
        else
            throw std::runtime_error("client was not invited");
    }
    if (ircConfig.get_max_joined_channels() != NO_LIMIT
		&& client.get_nb_joined_channels() >= ircConfig.get_max_joined_channels())
        throw std::runtime_error("joined channels limit reached");
    _members.insert(&client);
}

void Channel::remove_member(Client& client) { _members.erase(&client); }

void Channel::make_operator(Client& client)
{
    if (is_member(client)) {
        _operators.insert(&client);
    } else
        throw std::runtime_error("client can't be made operator as it is not a channel member");
}
