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
#include "http_response.h"
#include "http_status.h"

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "time.h"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#define CSP_HEADER \
    "Content-Security-Policy: default-src 'none'; style-src 'unsafe-inline'; " \
    "script-src 'unsafe-inline'; img-src data:; form-action 'self'; " \
    "base-uri 'none'; frame-ancestors 'none'\r\n"

int http_send_all (int fd , const char * data , size_t len)
{
	while (len > 0)
	{
		ssize_t n = send (fd , data , len , MSG_NOSIGNAL);
		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		data += n;
		len -= (size_t) n;
	}
	return 0;
}

int http_send_response (int fd , int status , const char * content_type ,
const char * extra_headers , const char * body ,
size_t body_len , int head_only)
{
	char date [64];
	char head [1536];
	time_t now = time (NULL);
	struct tm tm;
	if (gmtime_r (&now , &tm) == NULL ||
	strftime (date , sizeof date , "%a, %d %b %Y %H:%M:%S GMT" , &tm) == 0)
		strcpy (date , "Thu, 01 Jan 1970 00:00:00 GMT");
	int hl = snprintf (head , sizeof head ,
	"HTTP/1.1 %d %s\r\n"
	"Date: %s\r\n"
	"Content-Type: %s\r\n"
	"Content-Length: %zu\r\n"
	"Connection: close\r\n"
	"Cache-Control: no-store\r\n"
	"X-Content-Type-Options: nosniff\r\n"
	"X-Frame-Options: DENY\r\n"
	"Referrer-Policy: no-referrer\r\n"
	CSP_HEADER
	"%s"
	"\r\n" ,
	status , http_status_text (status) , date , content_type ,
	body_len , extra_headers ? extra_headers : "");
	if (hl < 0 || (size_t) hl >= sizeof head)
		return -1;
	size_t send_body = head_only ? 0 : body_len;
	char * msg = malloc ((size_t) hl + send_body);
	if (!msg)
		return -1;
	memcpy (msg , head , (size_t) hl);
	if (send_body)
		memcpy (msg + hl , body , send_body);
	int rc = http_send_all (fd , msg , (size_t) hl + send_body);
	free (msg);
	return rc;
}
