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
#include "userdb_fix.h"
#include "config.h"
#include "hex.h"
#include "userdb_name.h"

#include "stdio.h"
#include "string.h"

#define SAMPLE_HASH "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4"

#define HASH_HEX_LEN 64

static int is_blank (unsigned char c)
{
	return c == ' ' || c == '\t';
}

static size_t repair_username (const char * s , size_t n , char * out)
{
	size_t o = 0;
	while (n > 0 && is_blank ((unsigned char) s [0]))
	{
		s++;
		n--;
	}
	while (n > 0 && is_blank ((unsigned char) s [n - 1]))
		n--;
	if (n > MAX_USERNAME_LEN)
	{
		size_t cut = MAX_USERNAME_LEN;
		while (cut > 0 && ((unsigned char) s [cut] & 0xC0) == 0x80)
			cut--;
		n = cut;
	}
	for (; o < n; o++)
		out [o] = userdb_name_char_allowed ((unsigned char) s [o]) ? s [o] : '_';
	out [o] = '\0';
	return o;
}

static int collect_hash (const char * s , size_t n , char * hash)
{
	size_t k = 0;
	for (size_t i = 0; i < n; i++)
	{
		if (is_blank ((unsigned char) s [i]))
			continue;
		if (hex_digit_value ((unsigned char) s [i]) < 0 || k == HASH_HEX_LEN)
			return 0;
		hash [k++] = s [i];
	}
	hash [k] = '\0';
	return k == HASH_HEX_LEN;
}

void userdb_suggest_line (const char * line , size_t len , char * out ,
size_t cap , int * exact)
{
	char name [MAX_USERNAME_LEN + 1];
	char hash [HASH_HEX_LEN + 1];
	size_t nlen;
	int have_hash = 0;
	const char * colon = memchr (line , ':' , len);
	if (colon)
	{
		const char * rest = colon + 1;
		size_t rlen = len - (size_t)(rest - line);
		const char * semi = memchr (rest , ';' , rlen);
		if (semi)
			rlen = (size_t)(semi - rest);
		nlen = repair_username (line , (size_t)(colon - line) , name);
		have_hash = collect_hash (rest , rlen , hash);
	}
	else
	{
		size_t n = len;
		while (n > 0 && (is_blank ((unsigned char) line [n - 1]) || line [n - 1] == ';'))
			n--;
		if (n > HASH_HEX_LEN && collect_hash (line + n - HASH_HEX_LEN , HASH_HEX_LEN , hash))
		{
			have_hash = 1;
			n -= HASH_HEX_LEN;
		}
		nlen = repair_username (line , n , name);
	}
	if (nlen == 0)
	{
		snprintf (name , sizeof name , "username");
		have_hash = 0;
	}
	if (!have_hash)
		snprintf (hash , sizeof hash , "%s" , SAMPLE_HASH);
	*exact = have_hash;
	snprintf (out , cap , "%s:%s;" , name , hash);
}
