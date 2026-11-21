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
#include "prompt.h"

#include "signal.h"
#include "stdio.h"
#include "string.h"
#include "termios.h"
#include "unistd.h"

static volatile sig_atomic_t g_interrupted = 0;

static void on_interrupt (int sig)
{
	(void) sig;
	g_interrupted = 1;
}

static const int WATCHED [] = { SIGINT , SIGTERM , SIGHUP , SIGQUIT };
#define NWATCHED (sizeof WATCHED / sizeof WATCHED [0])

static void catch_signals (struct sigaction old [NWATCHED])
{
	struct sigaction sa;
	memset (&sa , 0 , sizeof sa);
	sa.sa_handler = on_interrupt;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	for (size_t i = 0; i < NWATCHED; i++)
		sigaction (WATCHED [i] , &sa , &old [i]);
}

static void restore_signals (const struct sigaction old [NWATCHED])
{
	for (size_t i = 0; i < NWATCHED; i++)
		sigaction (WATCHED [i] , &old [i] , NULL);
}

int prompt_password (const char * prompt , char * buf , size_t cap)
{
	struct termios saved , quiet;
	struct sigaction old [NWATCHED];
	int tty = isatty (STDIN_FILENO);
	int result;
	g_interrupted = 0;
	if (tty)
	{
		if (tcgetattr (STDIN_FILENO , &saved) != 0)
			tty = 0;
	}
	if (tty)
	{
		catch_signals (old);
		quiet = saved;
		quiet.c_lflag &= ~ (tcflag_t) ECHO;
		tcsetattr (STDIN_FILENO , TCSAFLUSH , &quiet);
		fputs (prompt , stderr);
		fflush (stderr);
	}
	if (fgets (buf , (int) cap , stdin) == NULL)
	{
		result = g_interrupted ? PROMPT_INTERRUPTED : PROMPT_EOF;
	}
	else
	{
		size_t n = strlen (buf);
		if (n > 0 && buf [n - 1] == '\n')
		{
			buf [--n] = '\0';
			if (n > 0 && buf [n - 1] == '\r')
				buf [--n] = '\0';
			result = (int) n;
		}
		else if (feof (stdin))
			result = (int) n;
		else
			result = PROMPT_TOO_LONG;
	}
	if (tty)
	{
		tcsetattr (STDIN_FILENO , TCSAFLUSH , &saved);
		fputc ('\n' , stderr);
		restore_signals (old);
		if (g_interrupted)
			result = PROMPT_INTERRUPTED;
	}
	return result;
}
