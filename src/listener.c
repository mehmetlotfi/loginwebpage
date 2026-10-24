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
#include "listener.h"
#include "config.h"

#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"

#include "errno.h"
#include "fcntl.h"
#include "unistd.h"

int listener_open (const bind_addr_t * ba , int * err)
{
	int one = 1;
	int fd = socket (ba -> ss.ss_family , SOCK_STREAM , 0);
	if (fd < 0)
	{
		*err = errno;
		return -1;
	}
	setsockopt (fd , SOL_SOCKET , SO_REUSEADDR , &one , sizeof one);
#ifdef IPV6_V6ONLY
	if (ba -> ss.ss_family == AF_INET6)
		setsockopt (fd , IPPROTO_IPV6 , IPV6_V6ONLY , &one , sizeof one);
#endif
	if (bind (fd , (const struct sockaddr *)&ba -> ss , ba -> len) < 0 ||
	listen (fd , LISTEN_BACKLOG) < 0)
	{
		*err = errno;
		close (fd);
		return -1;
	}
	int fl = fcntl (fd , F_GETFL , 0);
	if (fl >= 0)
		fcntl (fd , F_SETFL , fl | O_NONBLOCK);
	fcntl (fd , F_SETFD , FD_CLOEXEC);
	return fd;
}
