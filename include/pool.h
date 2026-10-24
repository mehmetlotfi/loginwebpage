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

#ifndef POOL_H
#define POOL_H

#include "stddef.h"
#include "sys/types.h"
#include "sys/socket.h"

typedef struct
{
	int fd;
	struct sockaddr_storage peer;
	socklen_t peer_len;
} conn_t;

typedef void (*conn_handler_fn)(void * ctx , conn_t * conn);

typedef struct pool pool_t;

pool_t * pool_create (size_t workers , size_t queue_capacity ,
conn_handler_fn handler , void * ctx);

int pool_submit (pool_t * p , const conn_t * conn);

size_t pool_workers (const pool_t * p);

void pool_destroy (pool_t * p);

#endif
