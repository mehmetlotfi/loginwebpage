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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "stddef.h"
#include "config.h"

typedef enum
{
	HTTP_RR_OK = 0 ,
	HTTP_RR_CLOSED ,
	HTTP_RR_IO_ERROR ,
	HTTP_RR_TIMEOUT ,
	HTTP_RR_BAD_REQUEST ,
	HTTP_RR_LENGTH_REQUIRED ,
	HTTP_RR_BODY_TOO_LARGE ,
	HTTP_RR_HEADERS_TOO_LARGE ,
	HTTP_RR_NOT_IMPLEMENTED
} http_rr_t;

typedef struct
{
	char method [16];
	char path [MAX_PATH_LEN];
	const char * body;
	size_t body_len;
	char buf [MAX_HEADER_SIZE + MAX_BODY_SIZE + 1];
} http_request_t;

http_rr_t http_read_request (int fd , http_request_t * req);

#endif
