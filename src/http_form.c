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
#include "http_form.h"

#include "string.h"

static int hexv (unsigned char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

int http_form_get (const char * body , size_t body_len , const char * key ,
char * out , size_t cap , size_t * out_len)
{
	size_t klen = strlen (key);
	for (size_t i = 0; i <= body_len;)
	{
		size_t j = i;
		while (j < body_len && body [j] != '&')
			j++;
		const char * pair = body + i;
		size_t pair_len = j - i;
		const char * eq = memchr (pair , '=' , pair_len);
		size_t nlen = eq ? (size_t)(eq - pair) : pair_len;
		if (nlen == klen && memcmp (pair , key , klen) == 0)
		{
			const char * v = eq ? eq + 1 : pair + pair_len;
			size_t vlen = eq ? pair_len - nlen - 1 : 0;
			size_t o = 0;
			for (size_t k = 0; k < vlen; k++)
			{
				unsigned char c = (unsigned char) v [k];
				if (c == '+')
				{
					c = ' ';
				}
				else if (c == '%' && k + 2 < vlen)
				{
					int hi = hexv ((unsigned char) v [k + 1]);
					int lo = hexv ((unsigned char) v [k + 2]);
					if (hi >= 0 && lo >= 0)
					{
						c = (unsigned char)(hi << 4 | lo);
						k += 2;
					}
				}
				if (o + 1 >= cap)
					return -2;
				out [o++] = (char) c;
			}
			out [o] = '\0';
			if (out_len)
				*out_len = o;
			return 0;
		}
		i = j + 1;
	}
	return -1;
}
