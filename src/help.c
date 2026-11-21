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
#include "help.h"
#include "manpage.h"
#include "pager.h"
#include "roff.h"
#include "strbuf.h"

#include "stdlib.h"
#include "sys/ioctl.h"
#include "unistd.h"

#define DEFAULT_COLUMNS 80
#define MAX_COLUMNS     80

static int text_width (void)
{
	int cols = DEFAULT_COLUMNS;
	const char * env = getenv ("MANWIDTH");
	struct winsize ws;
	if (env && atoi (env) > 0)
		cols = atoi (env);
	else if (isatty (STDOUT_FILENO) && ioctl (STDOUT_FILENO , TIOCGWINSZ , &ws) == 0 &&
	ws.ws_col > 0)
		cols = ws.ws_col;
	if (cols > MAX_COLUMNS)
		cols = MAX_COLUMNS;
	return cols - 2;
}

int help_show (void)
{
	strbuf_t text;
	FILE * out;
	int styled = isatty (STDOUT_FILENO);
	sb_init (&text);
	if (roff_render (manpage_roff , text_width () , styled , &text) != 0)
	{
		sb_free (&text);
		return 1;
	}
	out = pager_open ();
	fwrite (text.data , 1 , text.len , out);
	pager_close (out);
	sb_free (&text);
	return 0;
}

void help_dump_source (void)
{
	fputs (manpage_roff , stdout);
}
