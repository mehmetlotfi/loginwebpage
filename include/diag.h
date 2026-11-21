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

#ifndef DIAG_H
#define DIAG_H

#include "stddef.h"
#include "stdio.h"

typedef enum
{
	DIAG_ERROR ,
	DIAG_WARNING ,
	DIAG_NOTE
} diag_level_t;

typedef struct
{
	FILE * out;
	int color;
} diag_t;

void diag_init (diag_t * d , FILE * out);

void diag_report (diag_t * d , diag_level_t level , const char * path ,
size_t lineno , const char * text , size_t len , size_t off , size_t span ,
const char * fmt , ...) __attribute__ ((format (printf , 9 , 10)));

void diag_suggest (diag_t * d , const char * path , size_t lineno ,
const char * fixed , const char * fmt , ...)
__attribute__ ((format (printf , 5 , 6)));

#endif
