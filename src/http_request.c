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
#include "http_request.h"

#include "ctype.h"
#include "errno.h"
#include "string.h"
#include "strings.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "time.h"

static size_t find_head_end (const char * buf , size_t from , size_t len)
{
	for (size_t i = from; i + 4 <= len; i++)
	{
		if (buf [i] == '\r' && buf [i + 1] == '\n' &&
		buf [i + 2] == '\r' && buf [i + 3] == '\n')
			return i;
	}
	return (size_t)-1;
}

static const char * find_crlf (const char * p , const char * end)
{
	for (; p + 1 < end; p++)
		if (p [0] == '\r' && p [1] == '\n')
			return p;
	return NULL;
}

static int is_ws (char c)
{
	return c == ' ' || c == '\t';
}

static http_rr_t parse_head (http_request_t * r , size_t head_len ,
size_t * content_length , int * has_cl)
{
	const char * p = r -> buf;
	const char * end = r -> buf + head_len;
	*content_length = 0;
	*has_cl = 0;
	if (memchr (r -> buf , '\0' , head_len))
		return HTTP_RR_BAD_REQUEST;
	const char * eol = find_crlf (p , end);
	const char * line_end = eol ? eol : end;
	const char * sp1 = memchr (p , ' ' , (size_t)(line_end - p));
	if (!sp1 || sp1 == p || (size_t)(sp1 - p) >= sizeof r -> method)
		return HTTP_RR_BAD_REQUEST;
	for (const char * c = p; c < sp1; c++)
		if (!isupper ((unsigned char)*c))
			return HTTP_RR_BAD_REQUEST;
	memcpy (r -> method , p , (size_t)(sp1 - p));
	r -> method [sp1 - p] = '\0';
	const char * target = sp1 + 1;
	const char * sp2 = memchr (target , ' ' , (size_t)(line_end - target));
	if (!sp2 || sp2 == target)
		return HTTP_RR_BAD_REQUEST;
	const char * ver = sp2 + 1;
	if ((size_t)(line_end - ver) != 8 ||
	(memcmp (ver , "HTTP/1.1" , 8) != 0 && memcmp (ver , "HTTP/1.0" , 8) != 0))
		return HTTP_RR_BAD_REQUEST;
	if (*target != '/')
		return HTTP_RR_BAD_REQUEST;
	size_t plen = 0;
	for (const char * c = target; c < sp2; c++)
	{
		unsigned char ch = (unsigned char)*c;
		if (ch < 0x21 || ch == 0x7f)
			return HTTP_RR_BAD_REQUEST;
		if (ch == '?' || ch == '#')
			break;
		if (plen + 1 >= sizeof r -> path)
			return HTTP_RR_BAD_REQUEST;
		r -> path [plen++] = (char) ch;
	}
	r -> path [plen] = '\0';
	p = eol ? eol + 2 : end;
	while (p < end)
	{
		const char * e = find_crlf (p , end);
		const char * le = e ? e : end;
		const char * colon = memchr (p , ':' , (size_t)(le - p));
		if (!colon || colon == p || is_ws (*p))
			return HTTP_RR_BAD_REQUEST;
		size_t nlen = (size_t)(colon - p);
		const char * v = colon + 1;
		while (v < le && is_ws (*v)) v++;
		const char * ve = le;
		while (ve > v && is_ws (ve [-1])) ve--;
		if (nlen == 14 && strncasecmp (p , "content-length" , 14) == 0)
		{
			size_t n = (size_t)(ve - v);
			size_t val = 0;
			if (n == 0 || n > 9)
				return HTTP_RR_BAD_REQUEST;
			for (size_t i = 0; i < n; i++)
			{
				if (v [i] < '0' || v [i] > '9')
					return HTTP_RR_BAD_REQUEST;
				val = val * 10 + (size_t)(v [i] - '0');
			}
			if (*has_cl && val != *content_length)
				return HTTP_RR_BAD_REQUEST;
			*has_cl = 1;
			*content_length = val;
		}
		else if (nlen == 17 && strncasecmp (p , "transfer-encoding" , 17) == 0)
		{
			return HTTP_RR_NOT_IMPLEMENTED;
		}
		p = e ? e + 2 : end;
	}
	return HTTP_RR_OK;
}

http_rr_t http_read_request (int fd , http_request_t * req)
{
	const size_t cap = sizeof req -> buf - 1;
	size_t len = 0;
	size_t head_end = (size_t)-1;
	size_t total = 0;
	int parsed = 0;
	time_t deadline = time (NULL) + IO_TIMEOUT_SEC;
	memset (req -> method , 0 , sizeof req -> method);
	req -> path [0] = '\0';
	req -> body = "";
	req -> body_len = 0;
	for (;;)
	{
		if (parsed && len >= total)
			break;
		if (len >= cap)
			return parsed ? HTTP_RR_BODY_TOO_LARGE : HTTP_RR_HEADERS_TOO_LARGE;
		if (time (NULL) > deadline)
			return HTTP_RR_TIMEOUT;
		ssize_t n = recv (fd , req -> buf + len , cap - len , 0);
		if (n == 0)
			return len == 0 ? HTTP_RR_CLOSED : HTTP_RR_BAD_REQUEST;
		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return len == 0 ? HTTP_RR_CLOSED : HTTP_RR_TIMEOUT;
			return HTTP_RR_IO_ERROR;
		}
		size_t prev = len;
		len += (size_t) n;
		if (!parsed)
		{
			size_t from = prev >= 3 ? prev - 3 : 0;
			head_end = find_head_end (req -> buf , from , len);
			if (head_end == (size_t)-1)
			{
				if (len > MAX_HEADER_SIZE)
					return HTTP_RR_HEADERS_TOO_LARGE;
				continue;
			}
			if (head_end > MAX_HEADER_SIZE)
				return HTTP_RR_HEADERS_TOO_LARGE;
			size_t clen;
			int has_cl;
			http_rr_t rr = parse_head (req , head_end , &clen , &has_cl);
			if (rr != HTTP_RR_OK)
				return rr;
			if (strcmp (req -> method , "POST") == 0)
			{
				if (!has_cl)
					return HTTP_RR_LENGTH_REQUIRED;
				if (clen > MAX_BODY_SIZE)
					return HTTP_RR_BODY_TOO_LARGE;
			}
			else
			{
				clen = 0;
			}
			req -> body_len = clen;
			total = head_end + 4 + clen;
			parsed = 1;
		}
	}
	req -> buf [total] = '\0';
	req -> body = req -> buf + head_end + 4;
	return HTTP_RR_OK;
}
