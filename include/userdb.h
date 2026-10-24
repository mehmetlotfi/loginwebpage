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

#ifndef USERDB_H
#define USERDB_H

#include "stddef.h"
#include "stdint.h"
#include "stdio.h"

typedef struct
{
	char * name;
	uint8_t hash [32];
	size_t line;
} user_entry_t;

typedef struct
{
	user_entry_t * entries;
	size_t count;
} userdb_t;

int userdb_load (userdb_t * db , const char * path , FILE * errout);

void userdb_free (userdb_t * db);

const user_entry_t * userdb_find (const userdb_t * db , const char * name , size_t len);

int userdb_check (const userdb_t * db , const char * username , size_t ulen ,
const char * password , size_t plen);

#endif
