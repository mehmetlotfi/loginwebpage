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
#include "log.h"

#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

static void vlog (FILE * fp , const char * level , const char * fmt , va_list ap)
{
	char line [1200];
	size_t n = 0;
	time_t now = time (NULL);
	struct tm tm;
	if (localtime_r (&now , &tm) != NULL)
		n = strftime (line , sizeof line , "%Y-%m-%d %H:%M:%S " , &tm);
	int w = snprintf (line + n , sizeof line - n , "[%s] " , level);
	if (w > 0) n += (size_t) w;
	if (n >= sizeof line - 2) n = sizeof line - 3;
	w = vsnprintf (line + n , sizeof line - n - 1 , fmt , ap);
	if (w > 0)
	{
		n += (size_t) w;
		if (n > sizeof line - 2) n = sizeof line - 2;
	}
	line [n++] = '\n';
	line [n] = '\0';
	fputs (line , fp);
	fflush (fp);
}

#define DEFINE_LOGGER(name, fp, level)       \
    void name(const char *fmt, ...)          \
    {                                        \
        va_list ap;                          \
        va_start(ap, fmt);                   \
        vlog(fp, level, fmt, ap);            \
        va_end(ap);                          \
    }

DEFINE_LOGGER (log_info , stdout , "INFO")
DEFINE_LOGGER (log_warn , stderr , "WARN")
DEFINE_LOGGER (log_error , stderr , "ERROR")
