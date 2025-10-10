#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "colors.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <algorithm>
#include <bitset>
#include <iostream>

/************************************************************
 *		📁 CLASS METHODS
 **
 ************************************************************/

/// @brief checks validity according to RFC
bool Channel::is_valid_channel_name(const std::string& name)
{
    size_t posColon = name.find(':');
    size_t posBell  = name.find('\a');
    if (posColon != std::string::npos && posBell != std::string::npos)
        return false;
    if (name[0] != '#' && name[0] != '&' && name[0] != '+' && name[0] != '!')
        return false;
    return name.length() > 1 && name.length() <= ircConfig.get_chan_name_max_len();
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR
 **
 ************************************************************/

/// @throw exception if name is invalid
Channel::Channel(const std::string& name) :
    _topic(""), _key(""), _mode(CHANMODE_INIT), _userLimit(NO_LIMIT), _members(), _invites(), _operators()
{
    set_name(name);
}

Channel::Channel(const Channel& other) :
    _name(other._name),
    _topic(other._topic),
    _key(other._key),
    _mode(other._mode),
    _userLimit(other._userLimit),
    _members(other._members),
    _invites(other._invites),
    _operators(other._operators)
{
}

Channel::Channel(void) :
    _name(""), _topic(""), _key(""), _mode(CHANMODE_INIT), _userLimit(NO_LIMIT), _members(), _invites(), _operators()
{
}

Channel::~Channel() {}

/************************************************************
 *		➕ OPERATORS 	*
 ************************************************************/

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other) {
        _name      = other._name;
        _topic     = other._topic;
        _key	   = other._key;
        _userLimit = other._userLimit;
        _members   = other._members;
        _operators = other._operators;
        _invites   = other._invites;
    }
    return (*this);
}

// clang-format off
std::ostream&	operator<<(std::ostream& os, const Channel& c)
{
    std::bitset<MODE_LEN>modes(c.get_mode());
    return os << "Channel" << "["
    << " name = " << c.get_name()
    << " modes=" << modes
    << " topic=" << c.get_topic()
    << " userLimit=" << c.get_user_limit()
    << " nb of members=" << c.get_members().size()
    << "]";
}
// clang-format on

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Channel::broadcast(Server& server, ReplyCode replyCode, const std::string& message, Client* sender) const
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        Client* recipient = *it;
        if (sender && recipient == sender)
           continue;
        LOG_DT_SERVER(recipient->get_nickname() + " received a broadcast from " + get_name(), "");
        rh.process_response(*recipient, replyCode, message, sender);
    }
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

const std::string& Channel::get_name() const { return _name; }

const std::string& Channel::get_topic() const { return _topic; }

const std::string& Channel::get_key() const { return _key; }

bool Channel::is_member(Client& client) const { return _members.find(&client) != _members.end(); }

bool Channel::is_operator(Client& client) const { return _operators.find(&client) != _operators.end(); }

bool Channel::is_invited(Client& client) const { return _invites.find(&client) != _invites.end(); }

int Channel::get_user_limit() const { return _userLimit; }

bool Channel::is_invite_only() const { return _mode & CHANMODE_INVITE; }

bool Channel::is_topic_change_restricted() const { return _mode & CHANMODE_TOPIC; }

ReplyCode Channel::set_name(const std::string& name)
{
    if (Channel::is_valid_channel_name(name))
        _name = name;
    else
        return ERR_BADCHANMASK;
    return CORRECT_FORMAT;
}

ReplyCode Channel::set_topic(Client& client, const std::string& topic)
{
    if ((_mode & CHANMODE_TOPIC && is_operator(client)) || (!(_mode & CHANMODE_TOPIC)))
        _topic = topic;
    else
        return ERR_CHANOPRIVSNEEDED;
    return CORRECT_FORMAT;
}

ReplyCode Channel::set_key(const std::string& key)
{
    _key = key;
    return CORRECT_FORMAT;
}

void Channel::set_user_limit(int limit)
{
    if (limit < -1)
        _userLimit = -1;
    else
        _userLimit = limit;
}

void Channel::invite_client(Client& client) { _invites.insert(&client); }
// Can we invite a banned client ?
// Or when we invite a banned client it's remove it from banned list ?

ReplyCode Channel::add_member(Client& client)
{
    if (is_member(client))
        return CORRECT_FORMAT;
    if (_userLimit != NO_LIMIT && _members.size() >= static_cast<size_t>(_userLimit))
        return ERR_CHANNELISFULL;
    if (is_invite_only() && !is_invited(client)) {
        return ERR_INVITEONLYCHAN;
    }
    _invites.erase(&client);
    if (ircConfig.get_max_joined_channels() != NO_LIMIT && client.get_nb_joined_channels() >= ircConfig.get_max_joined_channels())
        return ERR_CHANNELISFULL;
    if (is_banned(client))
        return ERR_BANNEDFROMCHAN;
    _members.insert(&client);
    client.add_joined_channel(*this);
    return CORRECT_FORMAT;
}

void Channel::remove_member(Client& client) { _members.erase(&client); }

void Channel::remove_operator(Client& client) { _operators.erase(&client); }

ReplyCode Channel::ban_member(Client& client)
{
    if (is_member(client)) {
        _banList.insert(&client);
        return CORRECT_FORMAT;
    }
    return ERR_USERNOTINCHANNEL;
}

bool Channel::is_banned(Client& client) const { return _banList.find(&client) != _banList.end(); }

ReplyCode Channel::make_operator(Client& client)
{
    if (is_member(client)) {
        _operators.insert(&client);
        return CORRECT_FORMAT;
    }
    return ERR_USERNOTINCHANNEL;
}

void           Channel::add_mode(unsigned short mode) { _mode = _mode | mode; }
void           Channel::remove_mode(unsigned short mode) { _mode = _mode & ~mode; }
unsigned short Channel::get_mode() const { return _mode; }

size_t            Channel::get_nb_members() const { return _members.size(); }
std::set<Client*> Channel::get_members() const { return _members; }

std::vector<std::string> Channel::get_members_list() const
{
    std::vector<std::string>          list;
    std::set<Client*>::const_iterator it = _members.begin();
    std::string                       users;
    int                               nbUserPerLine = USERS_PER_LINE;
    int                               count         = 0;

    while (it != _members.end()) {
        Client* c = *it;
        if (is_operator(*c))
            users.append("@");
        users.append(c->get_nickname() + " ");
        ++it;
        ++count;
        if (count % nbUserPerLine == 0) {
            list.push_back(users);
            count = 0;
            users.clear();
        }
    }
    if (!users.empty())
        list.push_back(users);
    return (list);
}
