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
#include "acceptor.h"
#include "log.h"
#include "sockopt.h"

#include "errno.h"
#include "string.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "time.h"
#include "unistd.h"

void acceptor_accept_ready (int lfd , pool_t * pool)
{
	static time_t last_busy_log = 0;
	static unsigned long busy_count = 0;
	for (int burst = 0; burst < 128; burst++)
	{
		conn_t c;
		memset (&c , 0 , sizeof c);
		c.peer_len = sizeof c.peer;
		int fd = accept (lfd , (struct sockaddr *)&c.peer , &c.peer_len);
		if (fd < 0)
		{
			if (errno == EINTR || errno == ECONNABORTED || errno == EPROTO)
				continue;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			log_warn ("accept: %s" , strerror (errno));
			if (errno == EMFILE || errno == ENFILE || errno == ENOBUFS ||
			errno == ENOMEM)
				usleep (20 * 1000);
			return;
		}
		sock_prepare_client (fd);
		c.fd = fd;
		if (pool_submit (pool , &c) != 0)
		{
			sock_reject_busy (fd);
			busy_count++;
			time_t now = time (NULL);
			if (now != last_busy_log)
			{
				log_warn ("server busy: %lu connection(s) rejected with 503" ,
				busy_count);
				busy_count = 0;
				last_busy_log = now;
			}
		}
	}
}
