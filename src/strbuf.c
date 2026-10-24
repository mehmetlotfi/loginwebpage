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
#include "strbuf.h"

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

void sb_init (strbuf_t * sb)
{
	sb -> data = NULL;
	sb -> len = 0;
	sb -> cap = 0;
	sb -> failed = 0;
}

void sb_free (strbuf_t * sb)
{
	free (sb -> data);
	sb_init (sb);
}

static int sb_reserve (strbuf_t * sb , size_t extra)
{
	if (sb -> failed)
		return -1;
	if (extra > SIZE_MAX - sb -> len - 1)
	{
		sb -> failed = 1;
		return -1;
	}
	size_t need = sb -> len + extra + 1;
	if (need <= sb -> cap)
		return 0;
	size_t cap = sb -> cap ? sb -> cap : 4096;
	while (cap < need)
	{
		if (cap > SIZE_MAX / 2)
		{
			sb -> failed = 1;
			return -1;
		}
		cap *= 2;
	}
	char * p = realloc (sb -> data , cap);
	if (!p)
	{
		sb -> failed = 1;
		return -1;
	}
	sb -> data = p;
	sb -> cap = cap;
	return 0;
}

int sb_append (strbuf_t * sb , const char * s , size_t n)
{
	if (sb_reserve (sb , n) < 0)
		return -1;
	if (n)
		memcpy (sb -> data + sb -> len , s , n);
	sb -> len += n;
	sb -> data [sb -> len] = '\0';
	return 0;
}

int sb_append_str (strbuf_t * sb , const char * s)
{
	return sb_append (sb , s , strlen (s));
}

int sb_append_html_escaped (strbuf_t * sb , const char * s , size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		const char * rep;
		switch (s [i])
		{
			case '&': rep = "&amp;";
				break;
			case '<': rep = "&lt;";
				break;
			case '>': rep = "&gt;";
				break;
			case '"': rep = "&quot;";
				break;
			case '\'': rep = "&#39;";
				break;
			default: rep = NULL;
				break;
		}
		if (rep ? sb_append_str (sb , rep) < 0 : sb_append (sb , s + i , 1) < 0)
			return -1;
	}
	return 0;
}
