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
#include "pager.h"

#include "signal.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

static const char * const PAGERS [] = { "less" , "more" };

static struct sigaction g_old_int , g_old_quit , g_old_pipe;
static int g_piped = 0;

static int in_path (const char * name)
{
	const char * path = getenv ("PATH");
	char candidate [1024];
	if (!path)
		return 0;
	while (*path)
	{
		size_t len = strcspn (path , ":");
		if (len > 0 && len + strlen (name) + 2 < sizeof candidate)
		{
			memcpy (candidate , path , len);
			candidate [len] = '/';
			strcpy (candidate + len + 1 , name);
			if (access (candidate , X_OK) == 0)
				return 1;
		}
		path += len;
		if (*path == ':')
			path++;
	}
	return 0;
}

static void ignore_signal (int sig , struct sigaction * old)
{
	struct sigaction sa;
	memset (&sa , 0 , sizeof sa);
	sa.sa_handler = SIG_IGN;
	sigemptyset (&sa.sa_mask);
	sigaction (sig , &sa , old);
}

FILE * pager_open (void)
{
	FILE * fp = NULL;
	if (!isatty (STDOUT_FILENO))
		return stdout;
	for (size_t i = 0; i < sizeof PAGERS / sizeof PAGERS [0] && !fp; i++)
	{
		if (!in_path (PAGERS [i]))
			continue;
		fflush (stdout);
		setenv ("LESS" , "-isR" , 0);
		fp = popen (PAGERS [i] , "w");
	}
	if (!fp)
		return stdout;
	g_piped = 1;
	ignore_signal (SIGINT , &g_old_int);
	ignore_signal (SIGQUIT , &g_old_quit);
	ignore_signal (SIGPIPE , &g_old_pipe);
	return fp;
}

void pager_close (FILE * fp)
{
	if (fp == stdout)
	{
		fflush (stdout);
		return;
	}
	pclose (fp);
	if (g_piped)
	{
		sigaction (SIGINT , &g_old_int , NULL);
		sigaction (SIGQUIT , &g_old_quit , NULL);
		sigaction (SIGPIPE , &g_old_pipe , NULL);
		g_piped = 0;
	}
}
