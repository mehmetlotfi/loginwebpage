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
#include "netaddr.h"

#include "sys/types.h"
#include "sys/socket.h"
#include "net/if.h"
#include "ifaddrs.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#include "stdio.h"
#include "string.h"

int netaddr_same (const struct sockaddr_storage * a ,
const struct sockaddr_storage * b)
{
	if (a -> ss_family != b -> ss_family)
		return 0;
	if (a -> ss_family == AF_INET)
	{
		struct sockaddr_in x , y;
		memcpy (&x , a , sizeof x);
		memcpy (&y , b , sizeof y);
		return x.sin_addr.s_addr == y.sin_addr.s_addr;
	}
	else
	{
		struct sockaddr_in6 x , y;
		memcpy (&x , a , sizeof x);
		memcpy (&y , b , sizeof y);
		return memcmp (&x.sin6_addr , &y.sin6_addr , sizeof x.sin6_addr) == 0 &&
		x.sin6_scope_id == y.sin6_scope_id;
	}
}

int netaddr_from_ifaddr (const struct ifaddrs * ifa , uint16_t port ,
bind_addr_t * out)
{
	memset (out , 0 , sizeof *out);
	if (ifa -> ifa_addr -> sa_family == AF_INET)
	{
		struct sockaddr_in sin;
		memcpy (&sin , ifa -> ifa_addr , sizeof sin);
		sin.sin_port = htons (port);
		memcpy (&out -> ss , &sin , sizeof sin);
		out -> len = sizeof sin;
		return 0;
	}
	if (ifa -> ifa_addr -> sa_family == AF_INET6)
	{
		struct sockaddr_in6 sin6;
		memcpy (&sin6 , ifa -> ifa_addr , sizeof sin6);
		sin6.sin6_port = htons (port);
		sin6.sin6_flowinfo = 0;
		if (IN6_IS_ADDR_LINKLOCAL (&sin6.sin6_addr))
		{
			if (sin6.sin6_scope_id == 0 &&
			(sin6.sin6_addr.s6_addr [2] || sin6.sin6_addr.s6_addr [3]))
				sin6.sin6_scope_id = (uint32_t)((sin6.sin6_addr.s6_addr [2] << 8) |
				sin6.sin6_addr.s6_addr [3]);
			sin6.sin6_addr.s6_addr [2] = 0;
			sin6.sin6_addr.s6_addr [3] = 0;
			if (sin6.sin6_scope_id == 0)
				sin6.sin6_scope_id = if_nametoindex (ifa -> ifa_name);
			if (sin6.sin6_scope_id == 0)
				return -1;
		}
		memcpy (&out -> ss , &sin6 , sizeof sin6);
		out -> len = sizeof sin6;
		return 0;
	}
	return -1;
}

void netaddr_format (const bind_addr_t * ba , const char * ifname ,
char * buf , size_t cap)
{
	char ip [INET6_ADDRSTRLEN];
	if (ba -> ss.ss_family == AF_INET)
	{
		struct sockaddr_in sin;
		memcpy (&sin , &ba -> ss , sizeof sin);
		inet_ntop (AF_INET , &sin.sin_addr , ip , sizeof ip);
		snprintf (buf , cap , "%s:%u" , ip , (unsigned) ntohs (sin.sin_port));
	}
	else
	{
		struct sockaddr_in6 sin6;
		memcpy (&sin6 , &ba -> ss , sizeof sin6);
		inet_ntop (AF_INET6 , &sin6.sin6_addr , ip , sizeof ip);
		if (IN6_IS_ADDR_LINKLOCAL (&sin6.sin6_addr))
			snprintf (buf , cap , "[%s%%%s]:%u" , ip , ifname ,
			(unsigned) ntohs (sin6.sin6_port));
		else
			snprintf (buf , cap , "[%s]:%u" , ip , (unsigned) ntohs (sin6.sin6_port));
	}
}
