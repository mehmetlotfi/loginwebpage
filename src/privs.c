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
#include "privs.h"
#include "log.h"

#include "errno.h"
#include "grp.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "unistd.h"

static int parse_id (const char * s , unsigned long * out)
{
	char * end;
	if (!s || !*s)
		return -1;
	errno = 0;
	*out = strtoul (s , &end , 10);
	return (errno || *end != '\0') ? -1 : 0;
}

int privs_drop (void)
{
	unsigned long uid , gid;
	if (geteuid () != 0)
		return 0;
	if (parse_id (getenv ("SUDO_UID") , &uid) < 0 ||
	parse_id (getenv ("SUDO_GID") , &gid) < 0 || uid == 0)
	{
		log_warn ("running as root and SUDO_UID is not set: "
		"privileges are NOT dropped");
		return 0;
	}
	if (setgroups (0 , NULL) < 0 || setgid ((gid_t) gid) < 0 ||
	setuid ((uid_t) uid) < 0)
	{
		log_error ("cannot drop privileges: %s" , strerror (errno));
		return -1;
	}
	if (setuid (0) == 0)
	{
		log_error ("privileges could be regained after dropping them");
		return -1;
	}
	log_info ("dropped root privileges (now uid=%lu gid=%lu)" , uid , gid);
	return 1;
}
