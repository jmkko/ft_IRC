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
class Parser;


class Join : public ICommand
{
  public:
    Join(std::string& params);
    ~Join();

    void execute(Server& s, Client& c);

  private:
    std::map<std::string, std::string> _chans;

    void _send_list_of_names(Parser& p, Channel& channel);
    void _display_topic(Parser& p, Channel& channel);
    Join();
    Join(const Join& other);
    Join& operator=(const Join& other);
};

#endif
