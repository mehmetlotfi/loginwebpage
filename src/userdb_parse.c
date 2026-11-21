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
#include "userdb_parse.h"
#include "config.h"
#include "hex.h"
#include "userdb_fix.h"
#include "userdb_name.h"

#include "stdarg.h"
#include "stdlib.h"
#include "string.h"

void report_init (report_t * r , const char * path , FILE * out)
{
	r -> path = path;
	r -> errors = 0;
	r -> bad_lines = 0;
	r -> examples = 0;
	diag_init (&r -> diag , out);
}

void report_file_error (report_t * r , const char * fmt , ...)
{
	char msg [256];
	va_list ap;
	r -> errors++;
	if (r -> errors > MAX_REPORTED_ERRORS)
		return;
	va_start (ap , fmt);
	vsnprintf (msg , sizeof msg , fmt , ap);
	va_end (ap);
	diag_report (&r -> diag , DIAG_ERROR , r -> path , 0 , NULL , 0 , 0 , 0 , "%s" , msg);
}

typedef struct
{
	const char * text;
	size_t len;
	size_t lineno;
} line_t;

static void error_at (report_t * r , const line_t * ln , size_t off , size_t span ,
const char * fmt , ...) __attribute__ ((format (printf , 5 , 6)));

static void error_at (report_t * r , const line_t * ln , size_t off , size_t span ,
const char * fmt , ...)
{
	char msg [256];
	va_list ap;
	r -> errors++;
	if (r -> errors > MAX_REPORTED_ERRORS)
		return;
	va_start (ap , fmt);
	vsnprintf (msg , sizeof msg , fmt , ap);
	va_end (ap);
	diag_report (&r -> diag , DIAG_ERROR , r -> path , ln -> lineno , ln -> text ,
	ln -> len , off , span , "%s" , msg);
}

static int is_blank_line (const line_t * ln)
{
	for (size_t i = 0; i < ln -> len; i++)
		if (ln -> text [i] != ' ' && ln -> text [i] != '\t')
			return 0;
	return 1;
}

static void check_length (report_t * r , const line_t * ln)
{
	if (ln -> len > MAX_DB_LINE_LEN)
		error_at (r , ln , MAX_DB_LINE_LEN , ln -> len - MAX_DB_LINE_LEN ,
		"line is %zu bytes long (maximum is %d)" , ln -> len , MAX_DB_LINE_LEN);
}

static void check_username (report_t * r , const line_t * ln , size_t colon)
{
	name_problem_t p;
	if (userdb_name_check (ln -> text , colon , &p) == 0)
		return;
	error_at (r , ln , p.off , p.span , "%s" , p.why);
}

static void check_hash (report_t * r , const line_t * ln , const char * hash ,
size_t hlen , size_t hoff)
{
	if (hlen == 0)
		error_at (r , ln , hoff , 1 ,
		"password hash is missing (expected 64 hex digits)");
	else if (hlen != 64)
		error_at (r , ln , hoff , hlen ,
		"password hash has %zu characters, expected 64 hex digits (SHA-256)" , hlen);
	for (size_t i = 0; i < hlen; i++)
	{
		if (hex_digit_value ((unsigned char) hash [i]) < 0)
		{
			char what [48];
			userdb_describe_char ((unsigned char) hash [i] , what , sizeof what);
			error_at (r , ln , hoff + i , 1 ,
			"%s is not a hex digit (use 0-9 and a-f)" , what);
			break;
		}
	}
}

static void check_terminator (report_t * r , const line_t * ln , const char * semi)
{
	if (!semi)
	{
		error_at (r , ln , ln -> len , 1 , "missing ';' at the end of the line");
		return;
	}
	size_t after = (size_t)(semi - ln -> text) + 1;
	if (after >= ln -> len)
		return;
	line_t tail = { ln -> text + after , ln -> len - after , ln -> lineno };
	if (is_blank_line (&tail))
		error_at (r , ln , after , tail.len , "trailing whitespace after ';'");
	else
		error_at (r , ln , after , tail.len , "unexpected text after ';'");
}

static void suggest_fix (report_t * r , const line_t * ln)
{
	char fixed [USERDB_FIX_MAX];
	int exact;
	if (r -> errors > MAX_REPORTED_ERRORS)
		return;
	userdb_suggest_line (ln -> text , ln -> len , fixed , sizeof fixed , &exact);
	if (exact)
		diag_suggest (&r -> diag , r -> path , ln -> lineno , fixed ,
		"suggested fix for this line:");
	else
	{
		r -> examples++;
		diag_suggest (&r -> diag , r -> path , ln -> lineno , fixed ,
		"a valid line looks like this (the hash is only an example):");
	}
}

int userdb_parse_line (report_t * r , const char * raw , size_t rawlen ,
size_t lineno , user_entry_t * out)
{
	line_t ln = { raw , rawlen , lineno };
	size_t errors_before = r -> errors;
	if (ln.len > 0 && ln.text [ln.len - 1] == '\n')
		ln.len--;
	if (ln.len > 0 && ln.text [ln.len - 1] == '\r')
		ln.len--;
	if (is_blank_line (&ln))
		return 1;
	check_length (r , &ln);
	const char * colon = memchr (ln.text , ':' , ln.len);
	if (!colon)
	{
		error_at (r , &ln , ln.len , 1 ,
		"missing ':' between the username and the password hash");
	}
	else
	{
		size_t c = (size_t)(colon - ln.text);
		const char * hash = colon + 1;
		const char * semi = memchr (hash , ';' , ln.len - c - 1);
		size_t hlen = semi ? (size_t)(semi - hash) : ln.len - c - 1;
		check_username (r , &ln , c);
		check_hash (r , &ln , hash , hlen , c + 1);
		check_terminator (r , &ln , semi);
	}
	if (r -> errors != errors_before)
	{
		r -> bad_lines++;
		suggest_fix (r , &ln);
		return -1;
	}
	const char * hash = colon + 1;
	for (size_t i = 0; i < 32; i++)
		out -> hash [i] = (uint8_t)(hex_digit_value ((unsigned char) hash [2 * i]) << 4 |
		hex_digit_value ((unsigned char) hash [2 * i + 1]));
	size_t ulen = (size_t)(colon - ln.text);
	out -> name = malloc (ulen + 1);
	if (!out -> name)
	{
		report_file_error (r , "line %zu: out of memory" , lineno);
		return -1;
	}
	memcpy (out -> name , ln.text , ulen);
	out -> name [ulen] = '\0';
	out -> line = lineno;
	return 0;
}
