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

#ifndef USERDB_NAME_H
#define USERDB_NAME_H

#include "stddef.h"

typedef struct
{
	size_t off;
	size_t span;
	char why [160];
} name_problem_t;

int userdb_name_check (const char * name , size_t len , name_problem_t * problem);

int userdb_name_char_allowed (unsigned char c);

void userdb_describe_char (unsigned char c , char * out , size_t cap);

#endif
