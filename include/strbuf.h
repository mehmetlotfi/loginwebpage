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

#ifndef STRBUF_H
#define STRBUF_H

#include "stddef.h"

typedef struct
{
	char * data;
	size_t len;
	size_t cap;
	int failed;
} strbuf_t;

void sb_init (strbuf_t * sb);
void sb_free (strbuf_t * sb);
int sb_append (strbuf_t * sb , const char * s , size_t n);
int sb_append_str (strbuf_t * sb , const char * s);
int sb_append_html_escaped (strbuf_t * sb , const char * s , size_t n);

#endif
