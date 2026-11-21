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
#include "cmd_serve.h"
#include "config.h"
#include "log.h"
#include "rootcheck.h"
#include "server.h"
#include "userdb.h"

#include "stdio.h"

int cmd_serve (const char * prog , const char * file)
{
	userdb_t db;
	int rc;
	if (rootcheck_needs_sudo (SERVER_PORT , prog , file))
		return 1;
	if (userdb_load (&db , file , stderr) != 0)
	{
		fprintf (stderr , "%s: the users file is not valid - nothing was started.\n" , prog);
		return 1;
	}
	log_info ("%s %s: loaded %zu user(s) from %s" , APP_NAME , APP_VERSION ,
	db.count , file);
	rc = server_run (&db , (uint16_t) SERVER_PORT);
	userdb_free (&db);
	return rc;
}
