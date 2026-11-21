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
#include "signals.h"

#include "errno.h"
#include "fcntl.h"
#include "signal.h"
#include "string.h"
#include "unistd.h"

static const int STOP_SIGNALS [] = { SIGINT , SIGTERM , SIGHUP , SIGQUIT };
#define NSTOP (sizeof STOP_SIGNALS / sizeof STOP_SIGNALS [0])

static volatile sig_atomic_t g_count = 0;
static volatile sig_atomic_t g_signal = 0;
static int g_pipe [2] = { -1 , -1 };
static struct sigaction g_old [NSTOP];

static void on_signal (int sig)
{
	int saved_errno = errno;
	if (g_count++ > 0)
	{
		static const char msg [] = "\nsecond signal: leaving immediately\n";
		(void) !write (STDERR_FILENO , msg , sizeof msg - 1);
		_exit (130);
	}
	g_signal = sig;
	(void) !write (g_pipe [1] , "x" , 1);
	errno = saved_errno;
}

static void prepare_fd (int fd)
{
	int fl = fcntl (fd , F_GETFL , 0);
	if (fl >= 0)
		fcntl (fd , F_SETFL , fl | O_NONBLOCK);
	fcntl (fd , F_SETFD , FD_CLOEXEC);
}

int signals_install (void)
{
	struct sigaction sa;
	if (pipe (g_pipe) != 0)
		return -1;
	prepare_fd (g_pipe [0]);
	prepare_fd (g_pipe [1]);
	memset (&sa , 0 , sizeof sa);
	sa.sa_handler = on_signal;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	for (size_t i = 0; i < NSTOP; i++)
		sigaction (STOP_SIGNALS [i] , &sa , &g_old [i]);
	signal (SIGPIPE , SIG_IGN);
	return 0;
}

int signals_stop_requested (void)
{
	return g_count != 0;
}

int signals_received (void)
{
	return g_signal;
}

int signals_wake_fd (void)
{
	return g_pipe [0];
}

void signals_cleanup (void)
{
	for (size_t i = 0; i < NSTOP; i++)
		sigaction (STOP_SIGNALS [i] , &g_old [i] , NULL);
	if (g_pipe [0] >= 0)
		close (g_pipe [0]);
	if (g_pipe [1] >= 0)
		close (g_pipe [1]);
	g_pipe [0] = g_pipe [1] = -1;
}
