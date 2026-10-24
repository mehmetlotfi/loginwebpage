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
#include "usage.h"
#include "config.h"

void usage_print (FILE * out , const char * prog)
{
	fprintf (out ,
	"%s %s - multi-threaded web login page server\n"
	"\n"
	"Usage: %s <users-file>\n"
	"\n"
	"  <users-file>   text file with one user per line:\n"
	"                     username:sha256-hex-of-password;\n"
	"                 e.g.  Ali:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4;\n"
	"                 (create hashes with:  printf %%s \"pass\" | sha256sum\n"
	"                  or use  tools/mkuser.sh USERNAME)\n"
	"\n"
	"Listens on TCP port %d (compile-time macro SERVER_PORT) on every\n"
	"network interface of this machine.\n" ,
	APP_NAME , APP_VERSION , prog , SERVER_PORT);
}
