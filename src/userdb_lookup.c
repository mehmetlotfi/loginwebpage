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
#include "userdb.h"
#include "config.h"
#include "secure.h"
#include "sha256.h"

#include "string.h"

const user_entry_t * userdb_find (const userdb_t * db , const char * name , size_t len)
{
	size_t lo = 0 , hi = db -> count;
	if (len == 0 || len > MAX_USERNAME_LEN || memchr (name , '\0' , len))
		return NULL;
	while (lo < hi)
	{
		size_t mid = lo + (hi - lo) / 2;
		const char * en = db -> entries [mid].name;
		int c = strncmp (en , name , len);
		if (c == 0 && en [len] != '\0')
			c = 1;
		if (c == 0)
			return &db -> entries [mid];
		if (c < 0)
			lo = mid + 1;
		else
			hi = mid;
	}
	return NULL;
}

int userdb_check (const userdb_t * db , const char * username , size_t ulen ,
const char * password , size_t plen)
{
	static const uint8_t dummy [32] = { 0 };
	uint8_t digest [SHA256_DIGEST_SIZE];
	sha256 (password , plen , digest);
	const user_entry_t * e = userdb_find (db , username , ulen);
	const uint8_t * expected = e ? e -> hash : dummy;
	uint8_t diff = 0;
	for (size_t i = 0; i < sizeof digest; i++)
		diff |= (uint8_t)(digest [i] ^ expected [i]);
	secure_zero (digest , sizeof digest);
	return e != NULL && diff == 0;
}
