#include "Join.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <bitset>

Join::Join() {}
Join::~Join() {}
Join::Join(const Join& other) : ICommand(), _chans(other._chans) {}
Join& Join::operator=(const Join& other)
{
    if (this != &other) {
        _chans = other._chans;
    }
    return *this;
}

/**
 * @brief recreate the channels list. Each name of channel are mapped with their corresponding key.
 *
 * @param params [TODO:parameter]
 */
Join::Join(const std::string& params)
{
    std::istringstream iss(params);
    std::string        channels;
    std::string        keys;

    iss >> channels;
    iss >> keys;

    std::string        currentChannel, currentKey;
    std::istringstream issChannels(channels), issKeys(keys);
    while (std::getline(issChannels, currentChannel, ',')) {
        std::getline(issKeys, currentKey, ',');
        if (!currentChannel.empty() && Channel::is_valid_channel_name(currentChannel)) {
            _chans[currentChannel] = currentKey;
        }
        currentChannel.clear();
        currentKey.clear();
    }
}

/**
 * @brief check and keep only true parameters [#chan1,#chan2,#chan3 key1,key2,key3]
 *	the key can be an empty string (means no key for the channel) like key1,,key2
 *	list of channels and list of keys (each value separated with a commas) are separated by a space.
 * @param server
 * @param client
 * @param params
 * @return formated string of true parameters: <#chan1>,<#chan2> <key1>,<key2>
 */
ReplyCode Join::check_args(Server& server, Client& client, std::string& params)
{
    ReplyHandler       rh = ReplyHandler::get_instance(&server);
    std::istringstream iss(params);
    std::string        channels, keys;
    std::string        channelsNames, channelsKeys;

    iss >> channels;
    iss >> keys;
    if (channels.empty()) {
        return (ERR_NEEDMOREPARAMS);
    }
    std::string        currentChannel, currentKey;
    std::istringstream issChannels(channels), issKeys(keys);
    while (std::getline(issChannels, currentChannel, ',')) {
        std::getline(issKeys, currentKey, ',');
        if (currentChannel.empty() || !Channel::is_valid_channel_name(currentChannel)) {
            rh.process_response(client, ERR_NOSUCHCHANNEL, currentChannel);
        } else if (Channel::is_valid_channel_key(currentKey)) {
            channelsNames += currentChannel + ",";
            channelsKeys += currentKey + ",";
        } else {
            rh.process_response(client, ERR_BADCHANNELKEY, client.get_nickname() + " " + currentChannel);
        }
        currentChannel.clear();
        currentKey.clear();
    }
    if (channelsNames.empty())
        return (PROCESSED_ERROR);
    params = channelsNames + " " + channelsKeys;
    return (CORRECT_FORMAT);
}

/**
 * @brief send the users's list of a channel to the client
 *
 * @param rh [TODO:parameter]
 * @param client [TODO:parameter]
 * @param channel [TODO:parameter]
 */
void Join::send_list_of_names(ReplyHandler& rh, Client& client, Channel& channel) {
	std::vector<std::string> users = channel.get_members_list();

	for (size_t i = 0; i < users.size(); ++i) {
		rh.process_response(client, RPL_NAMREPLY, "= " + channel.get_name(), NULL, users[i]);
	}
	rh.process_response(client, RPL_ENDOFNAMES, channel.get_name());
}

/**
 * @brief send the RPL_TOPIC or RPL_NOTOPIC of channel to the client *
 * @param rh [TODO:parameter]
 * @param client [TODO:parameter]
 * @param channel [TODO:parameter]
 */
void Join::display_topic(ReplyHandler& rh, Client& client, Channel& channel) {
	if (channel.get_topic().empty()) {
		rh.process_response(client, RPL_NOTOPIC, channel.get_name());
	} else {
		rh.process_response(client, RPL_TOPIC, channel.get_name(), NULL, channel.get_topic());
	}
}

/**
 * @brief Allows a client to join a channel or create it if it does not exist
 * after checking if the name of the channel is valid accordind to the RFC_2812
 *
 * @param server
 * @param client
 *
 * sending response sequence to client
 *	:user1!~username@host JOIN :#chan1 123
 *	:irc.example.com MODE #chan1 +o user1
 *	:irc.example.com MODE #chan1 +k 123
 *	:irc.example.com 331 user1 #chan1 :No topic is set
 *	:irc.example.com 353 user1 = #chan1 :@user1 user2 user3 user4 user5
 *	:irc.example.com 353 user1 = #chan1 :user6 @user1 user7 user8 user9
 *	:irc.example.com 366 user1 #chan1 :End of NAMES listV
 */
void Join::execute(Server& server, Client& client)
{
    ReplyHandler&                                rh        = ReplyHandler::get_instance(&server);
    std::map<std::string, std::string>::iterator it        = _chans.begin();
    ReplyCode                                    replyCode = CORRECT_FORMAT;

    LOG_CMD.debug("Join.cpp execute()");
    for (; it != _chans.end(); ++it) {
        std::string chanName = it->first;
        std::string chanKey  = it->second;
        Channel*    channel  = server.find_channel_by_name(chanName);
        if (!channel) { 								// if the channel doesnt exist
            channel = new Channel(chanName, chanKey);	// make a new one
            server.channels[chanName] = channel;		// add it to the server
            LOG_I_CMD("#️⃣ New channel", channel->get_name());
		} else if ((channel->get_mode() & CHANMODE_KEY) && (chanKey != channel->get_key())) { 	// if the channel exist but wrong key has been given
            rh.process_response(client, ERR_BADCHANNELKEY, channel->get_name());				// send error - no connexions
			continue ;																			// continue iteration
		}
		replyCode = channel->add_member(client); 											// try to add the members to the channel
		if (replyCode == CORRECT_FORMAT) {													// if right permissions ...
			LOG_CONN.info(client.get_nickname() + " joined channel: " + channel->get_name());
			rh.process_response(client, TRANSFER_JOIN, channel->get_name());				// send connection success message
			channel->broadcast(server, TRANSFER_JOIN, channel->get_name(), &client);		// + broad cast
			if (channel->get_nb_members() == 1) {											// if first and/or only user
				channel->make_operator(client);												// --> make the client operator
				rh.process_response(client, RPL_CHANNELMODEIS, channel->get_name() + " +o ");
			} else {																		// else if more than one user in the channel
				send_list_of_names(rh, client, *channel);									// send the list of names
			}
			display_topic(rh, client, *channel);											// in any case, display the topic

		} else {
			rh.process_response(client, replyCode, channel->get_name());					// else if not added to chan -> send permissions error
		}
	}
}
