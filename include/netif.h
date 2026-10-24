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

#ifndef NETIF_H
#define NETIF_H

#include "stddef.h"
#include "stdint.h"

typedef struct
{
	int fd;
	char ifname [32];
	char label [80];
} listener_t;

typedef struct
{
	listener_t * items;
	size_t count;
	int last_errno;
} listener_set_t;

int netif_open_listeners (listener_set_t * set , uint16_t port);

void netif_close_listeners (listener_set_t * set);

#endif
