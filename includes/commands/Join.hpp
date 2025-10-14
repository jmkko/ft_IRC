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

class Join : public ICommand
{
  public:
    Join(const std::vector<std::string>& channelsLst);
    Join(const std::string& params);
    ~Join();
    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::vector<std::string>& params);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
	std::map<std::string, std::string> 	_chans;
	std::string 			_channelkey;
    std::vector<std::string> _channelsLst;
    Join();
    Join(const Join& other);
    Join& operator=(const Join& other);
};

#endif
