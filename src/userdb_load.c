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
#include "userdb_fix.h"
#include "userdb_parse.h"

#include "errno.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"

#define MAX_DB_FILE_SIZE (64L * 1024 * 1024)

static const char UTF8_BOM [] = "\xEF\xBB\xBF";

static int cmp_entries (const void * pa , const void * pb)
{
	const user_entry_t * a = pa , *b = pb;
	int c = strcmp (a -> name , b -> name);
	if (c)
		return c;
	return (a -> line > b -> line) - (a -> line < b -> line);
}

static void free_entries (user_entry_t * e , size_t n)
{
	for (size_t i = 0; i < n; i++)
		free (e [i].name);
	free (e);
}

static void skip_bom (const char ** p , size_t * len , size_t lineno)
{
	if (lineno == 1 && *len >= 3 && memcmp (*p , UTF8_BOM , 3) == 0)
	{
		*p += 3;
		*len -= 3;
	}
}

static char * fetch_line (const char * path , size_t wanted)
{
	FILE * fp = fopen (path , "r");
	char * line = NULL , *copy = NULL;
	size_t cap = 0 , lineno = 0;
	ssize_t got;
	if (!fp)
		return NULL;
	while ((got = getline (&line , &cap , fp)) != -1)
	{
		if (++lineno != wanted)
			continue;
		const char * p = line;
		size_t len = (size_t) got;
		skip_bom (&p , &len , lineno);
		while (len > 0 && (p [len - 1] == '\n' || p [len - 1] == '\r'))
			len--;
		copy = malloc (len + 1);
		if (copy)
		{
			memcpy (copy , p , len);
			copy [len] = '\0';
		}
		break;
	}
	free (line);
	fclose (fp);
	return copy;
}

typedef struct
{
	const user_entry_t * entry;
	size_t first_line;
} duplicate_t;

static int cmp_duplicates (const void * pa , const void * pb)
{
	const duplicate_t * a = pa , *b = pb;
	return (a -> entry -> line > b -> entry -> line) - (a -> entry -> line < b -> entry -> line);
}

static void report_duplicate (report_t * r , const duplicate_t * d)
{
	const user_entry_t * e = d -> entry;
	size_t nlen = strlen (e -> name);
	char * text = fetch_line (r -> path , e -> line);
	char * first = fetch_line (r -> path , d -> first_line);
	r -> errors++;
	r -> bad_lines++;
	if (r -> errors > MAX_REPORTED_ERRORS)
		goto done;
	diag_report (&r -> diag , DIAG_ERROR , r -> path , e -> line , text ,
	text ? strlen (text) : 0 , 0 , nlen , "duplicate username '%s'" , e -> name);
	diag_report (&r -> diag , DIAG_NOTE , r -> path , d -> first_line , first ,
	first ? strlen (first) : 0 , 0 , nlen , "'%s' was first defined here" , e -> name);
	if (text && nlen + 2 <= MAX_USERNAME_LEN)
	{
		char fixed [USERDB_FIX_MAX];
		const char * colon = strchr (text , ':');
		if (colon)
		{
			snprintf (fixed , sizeof fixed , "%s_2%s" , e -> name , colon);
			diag_suggest (&r -> diag , r -> path , e -> line , fixed ,
			"delete this line, or give it a different username, for example:");
		}
	}
	done:
	free (text);
	free (first);
}

static void report_duplicates (report_t * r , const user_entry_t * sorted , size_t count)
{
	duplicate_t * dups = malloc (count * sizeof *dups);
	size_t ndups = 0 , first_line = 0;
	if (!dups)
		return;
	for (size_t i = 0; i < count; i++)
	{
		if (i > 0 && strcmp (sorted [i - 1].name , sorted [i].name) == 0)
		{
			dups [ndups].entry = &sorted [i];
			dups [ndups].first_line = first_line;
			ndups++;
		}
		else
		{
			first_line = sorted [i].line;
		}
	}
	qsort (dups , ndups , sizeof *dups , cmp_duplicates);
	for (size_t i = 0; i < ndups; i++)
		report_duplicate (r , &dups [i]);
	free (dups);
}

static int append_entry (user_entry_t ** entries , size_t * count , size_t * cap ,
const user_entry_t * e)
{
	if (*count == *cap)
	{
		size_t ncap = *cap ? *cap * 2 : 64;
		user_entry_t * n = realloc (*entries , ncap * sizeof *n);
		if (!n)
			return -1;
		*entries = n;
		*cap = ncap;
	}
	(*entries) [(*count)++] = *e;
	return 0;
}

int userdb_load (userdb_t * db , const char * path , FILE * errout)
{
	report_t rep;
	user_entry_t * entries = NULL;
	size_t count = 0 , cap = 0 , lineno = 0 , linecap = 0;
	char * line = NULL;
	ssize_t got;
	struct stat st;
	report_init (&rep , path , errout);
	db -> entries = NULL;
	db -> count = 0;
	FILE * fp = fopen (path , "r");
	if (!fp)
	{
		report_file_error (&rep , "cannot open file: %s" , strerror (errno));
		return -1;
	}
	if (fstat (fileno (fp) , &st) == 0 && (!S_ISREG (st.st_mode) || st.st_size > MAX_DB_FILE_SIZE))
	{
		report_file_error (&rep , S_ISREG (st.st_mode) ?
		"file is too large to be a users file" : "not a regular file");
		fclose (fp);
		return -1;
	}
	while ((got = getline (&line , &linecap , fp)) != -1)
	{
		const char * p = line;
		size_t len = (size_t) got;
		user_entry_t e;
		lineno++;
		skip_bom (&p , &len , lineno);
		memset (&e , 0 , sizeof e);
		if (userdb_parse_line (&rep , p , len , lineno , &e) != 0)
			continue;
		if (append_entry (&entries , &count , &cap , &e) < 0)
		{
			free (e.name);
			report_file_error (&rep , "line %zu: out of memory" , lineno);
			break;
		}
	}
	free (line);
	if (ferror (fp))
		report_file_error (&rep , "read error: %s" , strerror (errno));
	fclose (fp);
	if (count > 1)
	{
		qsort (entries , count , sizeof *entries , cmp_entries);
		report_duplicates (&rep , entries , count);
	}
	if (rep.errors == 0 && count == 0)
		report_file_error (&rep , "the file defines no users");
	if (rep.errors > 0)
	{
		if (rep.errors > MAX_REPORTED_ERRORS)
			fprintf (errout , "%s: ... and %zu more error(s) not shown\n" , path ,
			rep.errors - MAX_REPORTED_ERRORS);
		if (rep.bad_lines)
			fprintf (errout , "%s: %zu error(s) in %zu line(s)\n" , path ,
			rep.errors , rep.bad_lines);
		else
			fprintf (errout , "%s: %zu error(s)\n" , path , rep.errors);
		if (rep.examples)
			fprintf (errout , "hint: create valid entries with:  %s --add-user NAME %s\n" ,
			APP_NAME , path);
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
