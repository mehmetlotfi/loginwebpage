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
#include "netif.h"
#include "config.h"
#include "listener.h"
#include "log.h"
#include "netaddr.h"

#include "sys/types.h"
#include "sys/socket.h"
#include "net/if.h"
#include "ifaddrs.h"

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

int netif_open_listeners (listener_set_t * set , uint16_t port)
{
	struct ifaddrs * ifap , *ifa;
	bind_addr_t * seen = NULL;
	size_t nseen = 0 , cap = 0;
	set -> items = NULL;
	set -> count = 0;
	set -> last_errno = 0;
	if (getifaddrs (&ifap) < 0)
	{
		log_error ("getifaddrs: %s" , strerror (errno));
		return -1;
	}
	for (ifa = ifap; ifa; ifa = ifa -> ifa_next)
	{
		bind_addr_t ba;
		if (!ifa -> ifa_addr || ! (ifa -> ifa_flags & IFF_UP))
			continue;
		if (ifa -> ifa_addr -> sa_family != AF_INET &&
		ifa -> ifa_addr -> sa_family != AF_INET6)
			continue;
		if (netaddr_from_ifaddr (ifa , port , &ba) < 0)
			continue;
		int dup = 0;
		for (size_t i = 0; i < nseen; i++)
			if (netaddr_same (&seen [i].ss , &ba.ss))
			{
				dup = 1;
				break;
			}
		if (dup)
			continue;
		if (nseen == cap)
		{
			size_t ncap = cap ? cap * 2 : 16;
			bind_addr_t * n = realloc (seen , ncap * sizeof *n);
			listener_t * l = realloc (set -> items , ncap * sizeof *l);
			if (n) seen = n;
			if (l) set -> items = l;
			if (!n || !l)
			{
				log_error ("out of memory");
				break;
			}
			cap = ncap;
		}
		seen [nseen++] = ba;
		char label [80];
		netaddr_format (&ba , ifa -> ifa_name , label , sizeof label);
		int err = 0;
		int fd = listener_open (&ba , &err);
		if (fd < 0)
		{
			set -> last_errno = err;
			log_warn ("cannot listen on %s (%s): %s" , label , ifa -> ifa_name ,
			strerror (err));
			continue;
		}
		listener_t * l = &set -> items [set -> count++];
		l -> fd = fd;
		snprintf (l -> ifname , sizeof l -> ifname , "%s" , ifa -> ifa_name);
		snprintf (l -> label , sizeof l -> label , "%s" , label);
	}
	freeifaddrs (ifap);
	free (seen);
	return (int) set -> count;
}

void netif_close_listeners (listener_set_t * set)
{
	for (size_t i = 0; i < set -> count; i++)
		close (set -> items [i].fd);
	free (set -> items);
	set -> items = NULL;
	set -> count = 0;
}
