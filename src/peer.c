/*
 *  loginwebpage - Login Page on web
 *  Copyright (C) 2026  Mehmet Lotfi
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "platform.h"
#include "peer.h"

#include "netdb.h"
#include "stdio.h"
#include "sys/socket.h"
#include "sys/types.h"

void peer_to_string (const conn_t * c , char * out , size_t cap)
{
	if (getnameinfo ((const struct sockaddr *)&c -> peer , c -> peer_len , out ,
	(socklen_t) cap , NULL , 0 , NI_NUMERICHOST) != 0)
		snprintf (out , cap , "?");
}
