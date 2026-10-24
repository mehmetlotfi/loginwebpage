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
#include "config.h"
#include "log.h"
#include "rootcheck.h"
#include "server.h"
#include "usage.h"
#include "userdb.h"

#include "stdio.h"
#include "string.h"

int main (int argc , char ** argv)
{
	const char * prog = argc > 0 ? argv [0] : APP_NAME;
	if (argc == 2 && (strcmp (argv [1] , "-h") == 0 || strcmp (argv [1] , "--help") == 0))
	{
		usage_print (stdout , prog);
		return 0;
	}
	if (rootcheck_needs_sudo (SERVER_PORT , argc , argv))
		return 1;
	if (argc != 2)
	{
		usage_print (stderr , prog);
		return 2;
	}
	userdb_t db;
	if (userdb_load (&db , argv [1] , stderr) != 0)
	{
		fprintf (stderr , "%s: the users file is not valid - nothing was started.\n" ,
		prog);
		return 1;
	}
	log_info ("%s %s: loaded %zu user(s) from %s" , APP_NAME , APP_VERSION ,
	db.count , argv [1]);
	int rc = server_run (&db , (uint16_t) SERVER_PORT);
	userdb_free (&db);
	return rc;
}
