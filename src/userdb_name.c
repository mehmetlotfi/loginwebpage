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
#include "userdb_name.h"
#include "config.h"

#include "stdio.h"

void userdb_describe_char (unsigned char c , char * out , size_t cap)
{
	if (c == ' ')
		snprintf (out , cap , "a space");
	else if (c == '\t')
		snprintf (out , cap , "a TAB");
	else if (c == '\0')
		snprintf (out , cap , "a NUL byte");
	else if (c < 0x20 || c == 0x7f)
		snprintf (out , cap , "control character 0x%02x" , c);
	else
		snprintf (out , cap , "'%c'" , c);
}

int userdb_name_char_allowed (unsigned char c)
{
	return ! (c <= 0x20 || c == 0x7f || c == ';' || c == ':');
}

int userdb_name_check (const char * name , size_t len , name_problem_t * problem)
{
	problem -> off = 0;
	problem -> span = 1;
	if (len == 0)
	{
		snprintf (problem -> why , sizeof problem -> why , "empty username");
		return -1;
	}
	if (len > MAX_USERNAME_LEN)
	{
		problem -> off = MAX_USERNAME_LEN;
		problem -> span = len - MAX_USERNAME_LEN;
		snprintf (problem -> why , sizeof problem -> why ,
		"username is %zu bytes long (maximum is %d)" , len , MAX_USERNAME_LEN);
		return -1;
	}
	for (size_t i = 0; i < len; i++)
	{
		if (!userdb_name_char_allowed ((unsigned char) name [i]))
		{
			char what [48];
			userdb_describe_char ((unsigned char) name [i] , what , sizeof what);
			problem -> off = i;
			snprintf (problem -> why , sizeof problem -> why ,
			"%s is not allowed in a username" , what);
			return -1;
		}
	}
	return 0;
}
