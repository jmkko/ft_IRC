/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   consts.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:38:52 by npolack           #+#    #+#             */
/*   Updated: 2025/10/07 18:48:48 by fpetit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTS_HPP
#define CONSTS_HPP

#define COMMANDS_NB               13
#define EXPECTED_ARGS_NB          3
#define MIN_PASSWORD_LEN          8
#define POLL_TIMEOUT              1000
#define PERM_755                  755
#define PERM_777                  777
#define EPOCH_TIME_START          1900
#define NO_LIMIT                  (-1)
#define SERVER_NAME               "hazardous.irc"
#define SERVER_CONF_FILE          "irc.conf"
#define SERVER_CONF_FILE_FOR_TEST "../irc.conf"
#define CODES_CONF_FILE           "codes.conf"
#define CODES_CONF_FILE_FOR_TEST  "../codes.conf"
#define DEFAULT_PASSWORD          "password"
#define DEFAULT_PORT              6667
#define MAX_JOINED_CHANNELS       NO_LIMIT
#define CHAN_NAME_MAX_LEN         50
#define CLIENT_READ_BUFFER_SIZE   512
#define WELL_KNOWN_PORT_MAX       1024
#define DYNAMIC_PORT_MIN          49152
#define NICKNAME_MAX_LEN          9
#define NB_AVAILABLE_CMD          14
#define USERS_PER_LINE            10
#define CHANMODE_INIT             0b00000
#define CHANMODE_OP               0b00001 // no use ?
#define CHANMODE_TOPIC            0b00010
#define CHANMODE_KEY              0b00100
#define CHANMODE_INVITE           0b01000
#define CHANMODE_LIMIT            0b10000
#define TARGET_LIMIT              5

#include <string>
#include <vector>

typedef int                      Socket;
typedef std::vector<std::string> t_params;

#endif
