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
#include "userdb_edit.h"
#include "config.h"
#include "hex.h"
#include "secure.h"
#include "sha256.h"
#include "userdb.h"
#include "userdb_name.h"

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"

static int rewrite_file (const char * path , size_t skip_line , const char * extra_line)
{
	int rc = -1;
	char * target = realpath (path , NULL);
	const char * dest = target ? target : path;
	char * tmp = NULL;
	FILE * in = NULL , *out = NULL;
	char * line = NULL;
	size_t cap = 0 , lineno = 0;
	ssize_t got;
	int ends_with_newline = 1;
	struct stat st;
	int have_stat = 0;
	in = fopen (dest , "r");
	if (in)
		have_stat = fstat (fileno (in) , &st) == 0;
	else if (errno != ENOENT)
	{
		fprintf (stderr , "%s: cannot read %s: %s\n" , APP_NAME , path , strerror (errno));
		goto done;
	}
	size_t dirlen = 0;
	const char * slash = strrchr (dest , '/');
	if (slash)
		dirlen = (size_t)(slash - dest) + 1;
	tmp = malloc (dirlen + sizeof ".loginwebpage.XXXXXX");
	if (!tmp)
		goto done;
	memcpy (tmp , dest , dirlen);
	strcpy (tmp + dirlen , ".loginwebpage.XXXXXX");
	int fd = mkstemp (tmp);
	if (fd < 0)
	{
		fprintf (stderr , "%s: cannot create a temporary file next to %s: %s\n" ,
		APP_NAME , path , strerror (errno));
		free (tmp);
		tmp = NULL;
		goto done;
	}
	out = fdopen (fd , "w");
	if (!out)
	{
		close (fd);
		goto fail;
	}
	while (in && (got = getline (&line , &cap , in)) != -1)
	{
		lineno++;
		ends_with_newline = line [got - 1] == '\n';
		if (lineno == skip_line)
			continue;
		if (fwrite (line , 1 , (size_t) got , out) != (size_t) got)
			goto fail;
	}
	if (extra_line)
	{
		if (lineno > 0 && !ends_with_newline)
			fputc ('\n' , out);
		fprintf (out , "%s\n" , extra_line);
	}
	if (fchmod (fileno (out) , have_stat ? (st.st_mode & 07777) : 0600) != 0)
		goto fail;
	if (have_stat)
		if (fchown (fileno (out) , st.st_uid , st.st_gid) != 0)
			errno = 0;
	if (fflush (out) != 0 || fsync (fileno (out)) != 0)
		goto fail;
	if (fclose (out) != 0)
	{
		out = NULL;
		goto fail;
	}
	out = NULL;
	if (rename (tmp , dest) != 0)
		goto fail;
	free (tmp);
	tmp = NULL;
	rc = 0;
	goto done;
	fail:
	fprintf (stderr , "%s: cannot write %s: %s\n" , APP_NAME , path , strerror (errno));
	done:
	if (out)
		fclose (out);
	if (tmp)
	{
		unlink (tmp);
		free (tmp);
	}
	if (in)
		fclose (in);
	free (line);
	free (target);
	return rc;
}

static int file_exists (const char * path)
{
	return access (path , F_OK) == 0;
}

int userdb_has_user (const char * path , const char * name)
{
	userdb_t db;
	int found;
	if (!file_exists (path))
		return 0;
	if (userdb_load (&db , path , stderr) != 0)
	{
		fprintf (stderr , "%s: %s has errors - fix them first; nothing was changed.\n" ,
		APP_NAME , path);
		return -1;
	}
	found = userdb_find (&db , name , strlen (name)) != NULL;
	userdb_free (&db);
	return found;
}

int userdb_add_user (const char * path , const char * name , const char * password)
{
	name_problem_t problem;
	uint8_t digest [SHA256_DIGEST_SIZE];
	char hex [2 * SHA256_DIGEST_SIZE + 1];
	char record [MAX_USERNAME_LEN + sizeof hex + 4];
	int exists , rc;
	if (userdb_name_check (name , strlen (name) , &problem) != 0)
	{
		fprintf (stderr , "%s: invalid username '%s': %s\n" , APP_NAME , name , problem.why);
		return -1;
	}
	exists = userdb_has_user (path , name);
	if (exists < 0)
		return -1;
	if (exists)
	{
		fprintf (stderr , "%s: user '%s' already exists in %s "
		"(remove it first with --remove-user)\n" , APP_NAME , name , path);
		return -1;
	}
	sha256 (password , strlen (password) , digest);
	hex_encode (digest , sizeof digest , hex);
	snprintf (record , sizeof record , "%s:%s;" , name , hex);
	secure_zero (digest , sizeof digest);
	rc = rewrite_file (path , 0 , record);
	if (rc == 0)
		printf ("added user '%s' to %s\n" , name , path);
	return rc;
}

int userdb_remove_user (const char * path , const char * name)
{
	userdb_t db;
	const user_entry_t * e;
	size_t line , remaining;
	int rc;
	if (!file_exists (path))
	{
		fprintf (stderr , "%s: cannot open %s: %s\n" , APP_NAME , path , strerror (ENOENT));
		return -1;
	}
	if (userdb_load (&db , path , stderr) != 0)
	{
		fprintf (stderr , "%s: %s has errors - fix them first; nothing was changed.\n" ,
		APP_NAME , path);
		return -1;
	}
	e = userdb_find (&db , name , strlen (name));
	if (!e)
	{
		fprintf (stderr , "%s: no user '%s' in %s\n" , APP_NAME , name , path);
		userdb_free (&db);
		return -1;
	}
	line = e -> line;
	remaining = db.count - 1;
	userdb_free (&db);
	rc = rewrite_file (path , line , NULL);
	if (rc == 0)
	{
		printf ("removed user '%s' from %s\n" , name , path);
		if (remaining == 0)
			fprintf (stderr , "%s: warning: %s now defines no users; the server will "
			"not start until you add one (--add-user)\n" , APP_NAME , path);
	}
	return rc;
}
