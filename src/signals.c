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

#include "signal.h"
#include "string.h"

static volatile sig_atomic_t g_stop = 0;

static void on_signal (int sig)
{
	(void) sig;
	g_stop = 1;
}

void signals_install (void)
{
	struct sigaction sa;
	memset (&sa , 0 , sizeof sa);
	sa.sa_handler = on_signal;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction (SIGINT , &sa , NULL);
	sigaction (SIGTERM , &sa , NULL);
	signal (SIGPIPE , SIG_IGN);
}

int signals_stop_requested (void)
{
	return g_stop != 0;
}
