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
#include "sockopt.h"
#include "config.h"

#include "fcntl.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#include "poll.h"
#include "sys/socket.h"
#include "sys/time.h"
#include "sys/types.h"
#include "unistd.h"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

void sock_prepare_client (int fd)
{
	struct timeval tv;
	int one = 1;
	int fl = fcntl (fd , F_GETFL , 0);
	if (fl >= 0 && (fl & O_NONBLOCK))
		fcntl (fd , F_SETFL , fl & ~O_NONBLOCK);
	fcntl (fd , F_SETFD , FD_CLOEXEC);
	tv.tv_sec = IO_TIMEOUT_SEC;
	tv.tv_usec = 0;
	setsockopt (fd , SOL_SOCKET , SO_RCVTIMEO , &tv , sizeof tv);
	setsockopt (fd , SOL_SOCKET , SO_SNDTIMEO , &tv , sizeof tv);
#ifdef SO_NOSIGPIPE
	setsockopt (fd , SOL_SOCKET , SO_NOSIGPIPE , &one , sizeof one);
#endif
#ifdef TCP_NODELAY
	setsockopt (fd , IPPROTO_TCP , TCP_NODELAY , &one , sizeof one);
#endif
	(void) one;
}

void sock_reject_busy (int fd)
{
	static const char msg [] =
	"HTTP/1.1 503 Service Unavailable\r\n"
	"Content-Length: 0\r\n"
	"Retry-After: 2\r\n"
	"Connection: close\r\n\r\n";
	send (fd , msg , sizeof msg - 1 , MSG_NOSIGNAL | MSG_DONTWAIT);
	close (fd);
}

void sock_lingering_close (int fd)
{
	char sink [1024];
	size_t drained = 0;
	shutdown (fd , SHUT_WR);
	for (;;)
	{
		struct pollfd p = { fd , POLLIN , 0 };
		int r = poll (&p , 1 , LINGER_TIMEOUT_MS);
		if (r <= 0)
			break;
		ssize_t n = recv (fd , sink , sizeof sink , 0);
		if (n <= 0)
			break;
		drained += (size_t) n;
		if (drained > 64 * 1024)
			break;
	}
	close (fd);
}
