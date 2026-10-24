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

#ifndef NETADDR_H
#define NETADDR_H

#include "stddef.h"
#include "stdint.h"
#include "sys/types.h"
#include "sys/socket.h"

struct ifaddrs;

typedef struct
{
	struct sockaddr_storage ss;
	socklen_t len;
} bind_addr_t;

int netaddr_same (const struct sockaddr_storage * a , const struct sockaddr_storage * b);

int netaddr_from_ifaddr (const struct ifaddrs * ifa , uint16_t port , bind_addr_t * out);

void netaddr_format (const bind_addr_t * ba , const char * ifname , char * buf , size_t cap);

#endif
