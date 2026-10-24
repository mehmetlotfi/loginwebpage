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
#include "userdb_parse.h"
#include "config.h"

#include "stdarg.h"
#include "string.h"
#include "stdlib.h"

void userdb_report (report_t * r , size_t line , const char * fmt , ...)
{
	va_list ap;
	r -> errors++;
	if (r -> errors > MAX_REPORTED_ERRORS)
		return;
	if (line)
		fprintf (r -> out , "%s:%zu: error: " , r -> path , line);
	else
		fprintf (r -> out , "%s: error: " , r -> path);
	va_start (ap , fmt);
	vfprintf (r -> out , fmt , ap);
	va_end (ap);
	fputc ('\n' , r -> out);
}

static int hex_val (unsigned char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

int userdb_parse_line (report_t * r , char * line , size_t len , size_t lineno ,
user_entry_t * out)
{
	if (len && line [len - 1] == '\n') len--;
	if (len && line [len - 1] == '\r') len--;
	line [len] = '\0';
	if (len == 0)
		return 1;
	if (len > MAX_DB_LINE_LEN)
	{
		userdb_report (r , lineno , "line is too long (max %d characters)" , MAX_DB_LINE_LEN);
		return -1;
	}
	if (strlen (line) != len)
	{
		userdb_report (r , lineno , "line contains a NUL byte");
		return -1;
	}
	if (line [len - 1] != ';')
	{
		userdb_report (r , lineno , "line must end with ';' (expected  username:sha256hex;)");
		return -1;
	}
	char * colon = strchr (line , ':');
	if (!colon)
	{
		userdb_report (r , lineno , "missing ':' between username and password hash");
		return -1;
	}
	size_t ulen = (size_t)(colon - line);
	if (ulen == 0)
	{
		userdb_report (r , lineno , "empty username");
		return -1;
	}
	if (ulen > MAX_USERNAME_LEN)
	{
		userdb_report (r , lineno , "username is too long (max %d characters)" , MAX_USERNAME_LEN);
		return -1;
	}
	for (size_t i = 0; i < ulen; i++)
	{
		unsigned char c = (unsigned char) line [i];
		if (c <= 0x20 || c == 0x7f || c == ';')
		{
			userdb_report (r , lineno , "username contains a forbidden character "
			"(spaces, control characters and ';' are not allowed)");
			return -1;
		}
	}
	const char * hash = colon + 1;
	size_t hlen = (size_t)((line + len - 1) - hash);
	if (hlen != 64)
	{
		userdb_report (r , lineno , "password hash must be exactly 64 hex digits "
		"(SHA-256), found %zu characters" , hlen);
		return -1;
	}
	for (size_t i = 0; i < 32; i++)
	{
		int hi = hex_val ((unsigned char) hash [2 * i]);
		int lo = hex_val ((unsigned char) hash [2 * i + 1]);
		if (hi < 0 || lo < 0)
		{
			userdb_report (r , lineno , "password hash contains a non-hex character "
			"at position %zu" , 2 * i + (hi < 0 ? 1 : 2));
			return -1;
		}
		out -> hash [i] = (uint8_t)(hi << 4 | lo);
	}
	out -> name = malloc (ulen + 1);
	if (!out -> name)
	{
		userdb_report (r , lineno , "out of memory");
		return -1;
	}
	memcpy (out -> name , line , ulen);
	out -> name [ulen] = '\0';
	out -> line = lineno;
	return 0;
}
