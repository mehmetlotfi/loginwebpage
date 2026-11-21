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
#include "cmd_user.h"
#include "config.h"
#include "prompt.h"
#include "secure.h"
#include "userdb_edit.h"
#include "userdb_name.h"

#include "stdio.h"
#include "string.h"
#include "unistd.h"

static int explain_prompt_failure (int rc)
{
	if (rc == PROMPT_INTERRUPTED)
		return 130;
	if (rc == PROMPT_TOO_LONG)
		fprintf (stderr , "%s: the password is too long (maximum %d characters)\n" ,
		APP_NAME , MAX_PASSWORD_LEN);
	else
		fprintf (stderr , "%s: no password was given\n" , APP_NAME);
	return 1;
}

static int read_new_password (char * pw , size_t cap , int * failed)
{
	char again [MAX_PASSWORD_LEN + 2];
	int n = prompt_password ("New password: " , pw , cap);
	*failed = 1;
	if (n < 0)
		return explain_prompt_failure (n);
	if (n == 0)
	{
		fprintf (stderr , "%s: an empty password is not allowed\n" , APP_NAME);
		return 1;
	}
	if (isatty (STDIN_FILENO))
	{
		int m = prompt_password ("Retype new password: " , again , sizeof again);
		if (m < 0)
		{
			secure_zero (again , sizeof again);
			return explain_prompt_failure (m);
		}
		int same = strcmp (pw , again) == 0;
		secure_zero (again , sizeof again);
		if (!same)
		{
			fprintf (stderr , "%s: the passwords do not match; nothing was changed\n" ,
			APP_NAME);
			return 1;
		}
	}
	*failed = 0;
	return 0;
}

int cmd_add_user (const char * file , const char * name)
{
	char password [MAX_PASSWORD_LEN + 2];
	name_problem_t problem;
	int failed , status;
	if (userdb_name_check (name , strlen (name) , &problem) != 0)
	{
		fprintf (stderr , "%s: invalid username '%s': %s\n" , APP_NAME , name , problem.why);
		return 1;
	}
	int exists = userdb_has_user (file , name);
	if (exists < 0)
		return 1;
	if (exists)
	{
		fprintf (stderr , "%s: user '%s' already exists in %s "
		"(remove it first with --remove-user)\n" , APP_NAME , name , file);
		return 1;
	}
	status = read_new_password (password , sizeof password , &failed);
	if (failed)
	{
		secure_zero (password , sizeof password);
		return status;
	}
	status = userdb_add_user (file , name , password) == 0 ? 0 : 1;
	secure_zero (password , sizeof password);
	return status;
}

int cmd_remove_user (const char * file , const char * name)
{
	return userdb_remove_user (file , name) == 0 ? 0 : 1;
}
