/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhervoch <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 09:51:44 by jhervoch          #+#    #+#             */
/*   Updated: 2025/09/30 10:18:28 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JOIN_HPP
#define JOIN_HPP

#include "Client.hpp"
#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;
class ReplyHandler;

class Join : public ICommand
{
  public:
    Join(std::string& params);
    ~Join();

    void             execute(Server& s, Client& c);
    void             send_list_of_names(ReplyHandler& rh, Client& client, Channel& channel);
    void             display_topic(ReplyHandler& rh, Client& client, Channel& channel);

  private:
    std::map<std::string, std::string> _chans;
    Join();
    Join(const Join& other);
    Join& operator=(const Join& other);
};

#endif
