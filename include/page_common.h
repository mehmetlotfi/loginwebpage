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

#ifndef PAGE_COMMON_H
#define PAGE_COMMON_H

#include "strbuf.h"

typedef enum
{
	PAGE_MARK_KEYHOLE ,
	PAGE_MARK_CHECK
} page_mark_t;

void page_begin (strbuf_t * sb , const char * title);

void page_emit_top (strbuf_t * sb , page_mark_t mark);

int page_end (strbuf_t * sb , const char * page_js);

#endif
