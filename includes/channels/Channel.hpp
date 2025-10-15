#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <set>
#include <string>
#include <vector>

#define MODE_LEN 5

class Client;

class Channel
{

  public:
    Channel(const std::string& name, const std::string& key = "");
    Channel(const Channel& other);

    virtual ~Channel();
    Channel& operator=(const Channel& other);

    const std::string& get_name() const;
    const std::string& get_topic() const;
    const std::string& get_key() const;
    bool               is_member(Client& client) const;
    bool               is_operator(Client& client) const;
    bool               is_invited(Client& client) const;
    int                get_user_limit() const;
    bool               is_invite_only() const;
    bool               is_topic_change_restricted() const;
    bool               is_banned(Client& client) const;

    ReplyCode set_name(const std::string& name);
    ReplyCode set_topic(Client& client, const std::string& topic);
    ReplyCode set_key(const std::string& key);
    void      set_user_limit(int limit);
    void      invite_client(Client& client);
    bool      remove_from_invited_list(Client& client);
    ReplyCode add_member(Client& client);
    void      remove_member(Client& client);
    ReplyCode ban_member(Client& client);
    ReplyCode make_operator(Client& client);
    void      remove_operator(Client& client);

    void                     add_mode(unsigned short mode);
    void                     remove_mode(unsigned short mode);
    unsigned short           get_mode() const;
    std::vector<std::string> get_members_list() const;
    size_t                   get_nb_members() const;
    std::set<Client*>        get_members() const;

    void        broadcast(Server&            server,
                          ReplyCode          replyCode,
                          const std::string& params,
                          Client*            sender   = NULL,
                          const std::string& trailing = "") const;
    static bool is_valid_channel_name(const std::string& name);
    static bool is_valid_channel_key(const std::string& key);

  private:
    std::string       _name;
    std::string       _topic;
    std::string       _key;
    unsigned short    _mode;
    int               _userLimit; // -1 if -l not set
    std::set<Client*> _members;
    std::set<Client*> _invites;
    std::set<Client*> _operators;
    std::set<Client*> _banList;

    Channel();
};

std::ostream& operator<<(std::ostream& os, const Channel& c);

#endif
