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
#include "userdb_parse.h"

#include "errno.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"

static int cmp_entries (const void * pa , const void * pb)
{
	const user_entry_t * a = pa , *b = pb;
	int c = strcmp (a -> name , b -> name);
	if (c) return c;
	return (a -> line > b -> line) - (a -> line < b -> line);
}

static void free_entries (user_entry_t * e , size_t n)
{
	for (size_t i = 0; i < n; i++)
		free (e [i].name);
	free (e);
}

int userdb_load (userdb_t * db , const char * path , FILE * errout)
{
	report_t rep = { path , errout , 0 };
	user_entry_t * entries = NULL;
	size_t count = 0 , cap = 0;
	char * line = NULL;
	size_t linecap = 0;
	size_t lineno = 0;
	ssize_t got;
	db -> entries = NULL;
	db -> count = 0;
	FILE * fp = fopen (path , "r");
	if (!fp)
	{
		userdb_report (&rep , 0 , "cannot open file: %s" , strerror (errno));
		return -1;
	}
	while ((got = getline (&line , &linecap , fp)) != -1)
	{
		char * p = line;
		size_t len = (size_t) got;
		lineno++;
		if (lineno == 1 && len >= 3 && (unsigned char) p [0] == 0xEF &&
		(unsigned char) p [1] == 0xBB && (unsigned char) p [2] == 0xBF)
		{
			p += 3;
			len -= 3;
		}
		user_entry_t e;
		memset (&e , 0 , sizeof e);
		int rc = userdb_parse_line (&rep , p , len , lineno , &e);
		if (rc != 0)
			continue;
		if (count == cap)
		{
			size_t ncap = cap ? cap * 2 : 64;
			user_entry_t * n = realloc (entries , ncap * sizeof *n);
			if (!n)
			{
				free (e.name);
				userdb_report (&rep , lineno , "out of memory");
				break;
			}
			entries = n;
			cap = ncap;
		}
		entries [count++] = e;
	}
	free (line);
	if (ferror (fp))
	{
		userdb_report (&rep , 0 , "read error: %s" , strerror (errno));
	}
	fclose (fp);
	if (count > 1)
	{
		size_t first_line = 0;
		qsort (entries , count , sizeof *entries , cmp_entries);
		for (size_t i = 0; i < count; i++)
		{
			if (i > 0 && strcmp (entries [i - 1].name , entries [i].name) == 0)
			{
				userdb_report (&rep , entries [i].line ,
				"duplicate username '%s' (already defined on line %zu)" ,
				entries [i].name , first_line);
			}
			else
			{
				first_line = entries [i].line;
			}
		}
	}
	if (rep.errors == 0 && count == 0)
		userdb_report (&rep , 0 , "the file defines no users");
	if (rep.errors > 0)
	{
		if (rep.errors > MAX_REPORTED_ERRORS)
			fprintf (errout , "%s: ... and %zu more error(s) not shown\n" ,
			path , rep.errors - MAX_REPORTED_ERRORS);
		free_entries (entries , count);
		return -1;
	}
	db -> entries = entries;
	db -> count = count;
	return 0;
}

void userdb_free (userdb_t * db)
{
	free_entries (db -> entries , db -> count);
	db -> entries = NULL;
	db -> count = 0;
}
