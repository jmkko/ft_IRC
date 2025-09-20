/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   consts.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npolack <npolack@student.42angouleme.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:38:52 by npolack           #+#    #+#             */
/*   Updated: 2025/09/18 23:39:18 by npolack          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTS_HPP
#define CONSTS_HPP

#define EXPECTED_ARGS_NB				3
#define MIN_PASSWORD_LEN				8
#define NO_LIMIT                (-1)
#define SERVER_NAME             "hazardous"
#define MAX_JOINED_CHANNELS     NO_LIMIT
#define CHAN_NAME_MAX_LEN       200
#define CLIENT_READ_BUFFER_SIZE 512
#define WELL_KNOWN_PORT_MAX     1024
#define DYNAMIC_PORT_MIN        49152
#define NICKNAME_MAX_LEN		9
#define	NB_AVAILABLE_CMD		9
typedef int Socket;

#endif
