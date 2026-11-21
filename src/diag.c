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
#include "diag.h"
#include "config.h"

#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#define SGR_RESET   "\033[0m"
#define SGR_BOLD    "\033[1m"
#define SGR_RED     "\033[1;31m"
#define SGR_MAGENTA "\033[1;35m"
#define SGR_CYAN    "\033[1;36m"
#define SGR_GREEN   "\033[1;32m"

void diag_init (diag_t * d , FILE * out)
{
	const char * term = getenv ("TERM");
	d -> out = out;
	d -> color = isatty (fileno (out)) && getenv ("NO_COLOR") == NULL &&
	(term == NULL || strcmp (term , "dumb") != 0);
}

static const char * paint (const diag_t * d , const char * code)
{
	return d -> color ? code : "";
}

static size_t sequence_bytes (unsigned char c , size_t remaining)
{
	size_t n = 1;
	if ((c >> 5) == 0x06) n = 2;
	else if ((c >> 4) == 0x0e) n = 3;
	else if ((c >> 3) == 0x1e) n = 4;
	return n <= remaining ? n : 1;
}

static size_t columns_in (const char * text , size_t nbytes)
{
	size_t cols = 0;
	for (size_t i = 0; i < nbytes; i += sequence_bytes ((unsigned char) text [i] , nbytes - i))
		cols++;
	return cols;
}

static void put_cell (FILE * out , const char * p , size_t n)
{
	unsigned char c = (unsigned char) p [0];
	if (n == 1 && (c < 0x20 || c == 0x7f))
		fputc (c == '\t' ? ' ' : '?' , out);
	else
		fwrite (p , 1 , n , out);
}

static void print_source (const diag_t * d , size_t lineno , const char * text ,
size_t len , size_t off , size_t span , int green)
{
	FILE * out = d -> out;
	size_t total = columns_in (text , len);
	size_t col = columns_in (text , off < len ? off : len);
	size_t width = 5;
	char digits [32];
	int nd = snprintf (digits , sizeof digits , "%zu" , lineno);
	size_t ncols = off < len ? columns_in (text + off , (off + span <= len ? span : len - off)) : 0;
	if (ncols == 0)
		ncols = 1;
	size_t from = 0 , to = total;
	if (total > DIAG_WINDOW)
	{
		from = col > DIAG_WINDOW / 2 ? col - DIAG_WINDOW / 2 : 0;
		to = from + DIAG_WINDOW < total ? from + DIAG_WINDOW : total;
	}
	if ((size_t) nd > width)
		width = (size_t) nd;
	fprintf (out , "%*zu | " , (int) width , lineno);
	if (green)
		fputs (paint (d , SGR_GREEN) , out);
	if (from > 0)
		fputs ("..." , out);
	size_t column = 0;
	for (size_t i = 0; i < len; column++)
	{
		size_t n = sequence_bytes ((unsigned char) text [i] , len - i);
		if (column >= from && column < to)
			put_cell (out , text + i , n);
		i += n;
	}
	if (to < total)
		fputs ("..." , out);
	fputs (paint (d , SGR_RESET) , out);
	fputc ('\n' , out);
	if (span == 0)
		return;
	fprintf (out , "%*s | " , (int) width , "");
	size_t pad = (col - from) + (from > 0 ? 3 : 0);
	for (size_t i = 0; i < pad; i++)
		fputc (' ' , out);
	fputs (paint (d , SGR_GREEN) , out);
	fputc ('^' , out);
	size_t room = to > col ? to - col : 1;
	for (size_t i = 1; i < ncols && i < room; i++)
		fputc ('~' , out);
	fputs (paint (d , SGR_RESET) , out);
	fputc ('\n' , out);
}

static void vreport (diag_t * d , diag_level_t level , const char * path ,
size_t lineno , const char * text , size_t len , size_t off , size_t span ,
int green , const char * fmt , va_list ap)
{
	FILE * out = d -> out;
	const char * name = level == DIAG_ERROR ? "error" :
	level == DIAG_WARNING ? "warning" : "note";
	const char * color = level == DIAG_ERROR ? SGR_RED :
	level == DIAG_WARNING ? SGR_MAGENTA : SGR_CYAN;
	fprintf (out , "%s%s" , paint (d , SGR_BOLD) , path);
	if (lineno)
	{
		fprintf (out , ":%zu" , lineno);
		if (text)
			fprintf (out , ":%zu" , columns_in (text , off < len ? off : len) + 1);
	}
	fprintf (out , ": %s%s:%s " , paint (d , color) , name , paint (d , SGR_RESET));
	vfprintf (out , fmt , ap);
	fputc ('\n' , out);
	if (lineno && text)
		print_source (d , lineno , text , len , off , span , green);
}

void diag_report (diag_t * d , diag_level_t level , const char * path ,
size_t lineno , const char * text , size_t len , size_t off , size_t span ,
const char * fmt , ...)
{
	va_list ap;
	va_start (ap , fmt);
	vreport (d , level , path , lineno , text , len , off , span , 0 , fmt , ap);
	va_end (ap);
}

void diag_suggest (diag_t * d , const char * path , size_t lineno ,
const char * fixed , const char * fmt , ...)
{
	va_list ap;
	va_start (ap , fmt);
	vreport (d , DIAG_NOTE , path , lineno , fixed , strlen (fixed) , 0 , 0 , 1 , fmt , ap);
	va_end (ap);
}
