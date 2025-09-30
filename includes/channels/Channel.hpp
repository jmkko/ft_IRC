#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "reply_codes.hpp"

#include <set>
#include <string>
#include <vector>

class Client;

class Channel
{

  public:
    Channel(const std::string& name);
    Channel(const Channel& other);

    virtual ~Channel();
    Channel& operator=(const Channel& other);

    const std::string& get_name() const;
    const std::string& get_topic() const;
    bool               is_member(Client& client) const;
    bool               is_operator(Client& client) const;
    bool               is_invited(Client& client) const;
    int                get_user_limit() const;
    bool               is_invite_only() const;
    bool               is_topic_change_restricted() const;
    bool               is_banned(Client& client) const;

    ReplyCode set_name(const std::string& name);
    ReplyCode set_topic(Client& client, const std::string& topic);
    void      set_user_limit(int limit);
    // void set_is_invite_only(bool is_invite_only);
    // void set_is_topic_change_restricted(bool isRestricted);
    void      invite_client(Client& client);
    ReplyCode add_member(Client& client);
    void      remove_member(Client& client);
    ReplyCode ban_member(Client& client);
    ReplyCode make_operator(Client& client);

    void           set_mode(unsigned short mode);
    unsigned short get_mode() const;
    std::string    get_members_list() const;
    size_t         get_nb_members() const;

    void        broadcast(const std::string& message, Client* sender = NULL) const;
    static bool is_valid_channel_name(const std::string& name);

  private:
    std::string    _name;
    std::string    _topic;
    unsigned short _mode;
    int            _userLimit; // -1 if -l not set
    // bool	      _isInviteOnly;		// true if -i is set
    // bool	      _isTopicChangeRestricted; // true if -t is set
    std::set<Client*> _members;
    std::set<Client*> _invites;
    std::set<Client*> _operators;
    std::set<Client*> _banList;

    Channel();
};

std::ostream& operator<<(std::ostream& os, const Channel& c);

#endif
