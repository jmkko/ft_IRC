#include "Join.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

/******************************************************************************
 *                  CONSTRUCTOR AND DESTRUCTOR                                *
 ******************************************************************************/

Join::Join() {}
Join::~Join() {}

Join::Join(std::string& params)
{
    Parser parser;

    std::string channels = parser.format_parameter(params, NULL);
    std::string keys     = parser.format_parameter(params, NULL);

    _chans = parser.to_map(channels, keys);
}

/******************************************************************************
 *                                 METHODS                                    *
 ******************************************************************************/

void Join::_send_list_of_names(Parser& p, Channel& channel)
{
    std::vector<std::string> users = channel.get_members_list();

    for (size_t i = 0; i < users.size(); ++i) {
        p.response(RPL_NAMREPLY, "= " + channel.get_name(), users[i]);
    }
    p.response(RPL_ENDOFNAMES, channel.get_name());
}

void Join::_display_topic(Parser& p, Channel& channel)
{
    if (channel.get_topic().empty()) {
        p.response(RPL_NOTOPIC, channel.get_name());
    } else {
        p.response(RPL_TOPIC, channel.get_name(), channel.get_topic());
    }
}

void Join::execute(Server& server, Client& client)
{
    Parser    p(server, client);
    ReplyCode replyCode = CORRECT_FORMAT;

    LOG_CMD.debug("Join.cpp execute()");
    if (_chans.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "JOIN");
    }
    for (std::map<std::string, std::string>::iterator it = _chans.begin(); it != _chans.end(); ++it) {
        std::string chanName = it->first;
        std::string chanKey  = it->second;
        if (!p.correct_channel(chanName) && chanName != "0") {
            continue;
        }
        if (chanName.size() == 1 && chanName[0] == '0') {
            client.part_all_channels(server, client);
            continue;
        }
        Channel* channel = server.find_channel_by_name(chanName);
        if (!channel) {
            if (!p.correct_key(chanKey))
                chanKey.clear();
            channel                   = new Channel(chanName, chanKey);
            server.channels[chanName] = channel;
            LOG_I_CMD("#️⃣ New channel", channel->get_name());
        } else if ((channel->get_mode() & CHANMODE_KEY) && (chanKey != channel->get_key())) {
            p.response(ERR_BADCHANNELKEY, chanName);
            continue;
        }
        replyCode = channel->add_member(client);
        if (replyCode == CORRECT_FORMAT) {
            LOG_CONN.info(client.get_nickname() + " joined channel: " + chanName);
            p.response(TRANSFER_JOIN, chanName);
            channel->broadcast(server, TRANSFER_JOIN, chanName, &client);
            channel->remove_from_invited_list(client);
            if (channel->get_nb_members() == 1) {
                channel->make_operator(client);
                p.response(RPL_CHANNELMODEIS, chanName + " +o ");
                // rh.process_response(client, TRANSFER_MODE, chanName + " +o " + client.get_nickname());  <-- merge changements
            }
            p.response(RPL_CHANNELMODEIS, chanName + " " + channel->get_modes_str(client));
            _send_list_of_names(p, *channel);
            _display_topic(p, *channel);
        } else {
            p.response(replyCode, chanName);
        }
    }
}
