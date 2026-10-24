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
#include "rootcheck.h"
#include "config.h"

#include "stdio.h"
#include "unistd.h"

int rootcheck_needs_sudo (int port , int argc , char ** argv)
{
	if (port >= PRIVILEGED_PORT_LIMIT || geteuid () == 0)
		return 0;
	fprintf (stderr ,
	"Port %d is below %d, so this program needs root privileges.\n"
	"Please run me with sudo:\n"
	"\n"
	"    sudo %s %s\n" ,
	port , PRIVILEGED_PORT_LIMIT , argc > 0 ? argv [0] : APP_NAME ,
	argc > 1 ? argv [1] : "<users-file>");
	return 1;
}
