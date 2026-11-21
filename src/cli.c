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
#include "usage.h"

#include "getopt.h"
#include "stdio.h"

#define OPT_DUMP_MAN 1000

static const char SHORT_OPTS [] = "a:r:hV";

static const struct option LONG_OPTS [] = {
	{ "add-user" , required_argument , NULL , 'a' } ,
	{ "remove-user" , required_argument , NULL , 'r' } ,
	{ "help" , no_argument , NULL , 'h' } ,
	{ "version" , no_argument , NULL , 'V' } ,
	{ "dump-man" , no_argument , NULL , OPT_DUMP_MAN } ,
	{ NULL , 0 , NULL , 0 }
};

int cli_parse (int argc , char ** argv , cli_options_t * opt)
{
	const char * prog = argc > 0 ? argv [0] : "loginwebpage";
	int c;
	opt -> mode = CLI_SERVE;
	opt -> user = NULL;
	opt -> file = NULL;
	while ((c = getopt_long (argc , argv , SHORT_OPTS , LONG_OPTS , NULL)) != -1)
	{
		switch (c)
		{
			case 'h':
				opt -> mode = CLI_HELP;
				return 0;
			case OPT_DUMP_MAN:
				opt -> mode = CLI_DUMP_MAN;
				return 0;
			case 'V':
				opt -> mode = CLI_VERSION;
				return 0;
			case 'a':
			case 'r':
				if (opt -> mode != CLI_SERVE)
				{
					fprintf (stderr , "%s: --add-user and --remove-user cannot be "
					"combined, and each may be given only once\n" , prog);
					usage_print (stderr , prog);
					return -1;
				}
				opt -> mode = c == 'a' ? CLI_ADD_USER : CLI_REMOVE_USER;
				opt -> user = optarg;
				break;
			default:
				usage_print (stderr , prog);
				return -1;
		}
	}
	if (optind >= argc)
	{
		fprintf (stderr , "%s: missing users file\n" , prog);
		usage_print (stderr , prog);
		return -1;
	}
	if (optind + 1 < argc)
	{
		fprintf (stderr , "%s: too many arguments ('%s' ...)\n" , prog , argv [optind + 1]);
		usage_print (stderr , prog);
		return -1;
	}
	opt -> file = argv [optind];
	return 0;
}
