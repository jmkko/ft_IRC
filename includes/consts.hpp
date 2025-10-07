/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   consts.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:38:52 by npolack           #+#    #+#             */
/*   Updated: 2025/10/01 16:15:41 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTS_HPP
#define CONSTS_HPP

#define COMMANDS_NB             11
#define EXPECTED_ARGS_NB        3
#define MIN_PASSWORD_LEN        8
#define POLL_TIMEOUT            1000
#define PERM_755                755
#define PERM_777                777
#define EPOCH_TIME_START        1900
#define NO_LIMIT                (-1)
#define SERVER_NAME             "hazardous.irc"
#define SERVER_CONF_FILE        "irc.conf"
#define DEFAULT_PASSWORD        "password"
#define DEFAULT_PORT            6667
#define MAX_JOINED_CHANNELS     NO_LIMIT
#define CHAN_NAME_MAX_LEN       50
#define CLIENT_READ_BUFFER_SIZE 512
#define WELL_KNOWN_PORT_MAX     1024
#define DYNAMIC_PORT_MIN        49152
#define NICKNAME_MAX_LEN        9
#define NB_AVAILABLE_CMD        11
#define USERS_PER_LINE          10
#define TARGET_LIMIT            5
#define CHANMODE_INIT           0x00
#define CHANMODE_OP             0x01
#define CHANMODE_TOPIC          0x02
#define CHANMODE_KEY            0x04
#define CHANMODE_INVITE         0x08
#define CHANMODE_LIMIT          0x10

#include <string>
#include <vector>

typedef int                      Socket;
typedef std::vector<std::string> t_params;

#endif
