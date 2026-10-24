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

#ifndef USERDB_PARSE_H
#define USERDB_PARSE_H

#include "stddef.h"
#include "stdio.h"
#include "userdb.h"

#define MAX_REPORTED_ERRORS 50

typedef struct
{
	const char * path;
	FILE * out;
	size_t errors;
} report_t;

void userdb_report (report_t * r , size_t line , const char * fmt , ...)
__attribute__ ((format (printf , 3 , 4)));

int userdb_parse_line (report_t * r , char * line , size_t len , size_t lineno ,
user_entry_t * out);

#endif
