#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "ACommand.hpp"
#include "colors.hpp"
#include "consts.hpp"

#include <iostream>
#include <set>
#include <string>

class Client;

class Channel
{

  public:
    Channel(const std::string& name);
    Channel(const Channel& other);

    virtual ~Channel();
    Channel& operator=(const Channel& other);

    const std::string& getName() const;
    const std::string& getTopic() const;
    bool               isMember(Client& client) const;
    bool               isOperator(Client& client) const;
    bool               isInvited(Client& client) const;
    int                getUserLimit() const;
    bool               isInviteOnly() const;
    bool               isTopicChangeRestricted() const;

    void setName(const std::string& name);
    void setTopic(const std::string& topic);
    void setUserLimit(int limit);
    void setIsInviteOnly(bool isInviteOnly);
    void setIsTopicChangeRestricted(bool isRestricted);
    void inviteClient(Client& client);
    void addMember(Client& client); // should implement different logics whether it is or not invite-only
    void removeMember(Client& client);
    void makeOperator(Client& client);

    void broadcast(const std::string& message, Client* sender = NULL) const;

  private:
    std::string       _name;
    std::string       _topic;
    int               _userLimit;               // -1 if -l not set
    bool              _isInviteOnly;            // true if -i is set
    bool              _isTopicChangeRestricted; // true if -t is set
    std::set<Client*> _members;
    std::set<Client*> _invites;
    std::set<Client*> _operators;

    Channel();
    static bool _isValidChannelName(const std::string& name);
};

std::ostream& operator<<(std::ostream& os, const Channel& c);

#endif
