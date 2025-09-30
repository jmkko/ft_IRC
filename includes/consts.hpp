/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   consts.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fpetit <fpetit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:38:52 by npolack           #+#    #+#             */
/*   Updated: 2025/09/25 09:40:47 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTS_HPP
#define CONSTS_HPP

#define COMMANDS_NB		9
#define EXPECTED_ARGS_NB	3
#define MIN_PASSWORD_LEN	8
#define POLL_TIMEOUT		1000
#define PERM_755		755
#define EPOCH_TIME_START	1900
#define NO_LIMIT		(-1)
#define SERVER_NAME		"hazardous.irc"
#define SERVER_CONF_FILE	"irc.conf"
#define DEFAULT_PASSWORD	"password"
#define DEFAULT_PORT		6667
#define MAX_JOINED_CHANNELS	NO_LIMIT
#define CHAN_NAME_MAX_LEN	50
#define CLIENT_READ_BUFFER_SIZE 512
#define WELL_KNOWN_PORT_MAX	1024
#define DYNAMIC_PORT_MIN	49152
#define NICKNAME_MAX_LEN	9
#define NB_AVAILABLE_CMD	9

#define CHANMODE_INIT	0x00
#define CHANMODE_OP	0x01
#define CHANMODE_TOPIC	0x02
#define CHANMODE_KEY	0x04
#define CHANMODE_INVITE 0x08
#define CHANMODE_LIMIT	0x10

typedef int Socket;

#endif
