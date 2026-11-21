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
#include "cli.h"
#include "cmd_serve.h"
#include "cmd_user.h"
#include "config.h"
#include "help.h"

#include "stdio.h"

int main (int argc , char ** argv)
{
	cli_options_t opt;
	const char * prog = argc > 0 ? argv [0] : APP_NAME;
	if (cli_parse (argc , argv , &opt) != 0)
		return 2;
	switch (opt.mode)
	{
		case CLI_HELP:
			return help_show ();
		case CLI_DUMP_MAN:
			help_dump_source ();
			return 0;
		case CLI_VERSION:
			printf ("%s %s\n" , APP_NAME , APP_VERSION);
			return 0;
		case CLI_ADD_USER:
			return cmd_add_user (opt.file , opt.user);
		case CLI_REMOVE_USER:
			return cmd_remove_user (opt.file , opt.user);
		case CLI_SERVE:
			break;
	}
	return cmd_serve (prog , opt.file);
}
