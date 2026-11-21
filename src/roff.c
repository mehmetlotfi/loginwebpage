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
#include "roff.h"

#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define FONT_R 0
#define FONT_B 1
#define FONT_I 2

#define NBSP   '\x01'

#define BODY_INDENT   7
#define SUBSECT_INDENT 3
#define MAX_ARGS      16

typedef struct
{
	unsigned char ch;
	unsigned char font;
} cell_t;

typedef struct
{
	cell_t * v;
	size_t len;
	size_t cap;
} cells_t;

typedef struct
{
	strbuf_t * out;
	int width;
	int styled;
	int oom;
	char title [64] , section [16] , date [64] , source [96] , manual [96];
	int rs;
	int left;
	int hang;
	int text_indent;
	int fill;
	int font , prev_font;
	int next_line_font;
	cells_t word;
	cells_t line;
	cells_t tag;
	int have_tag;
	int expect_tag;
	int printed;
	int after_heading;
	int last_blank;
} roff_t;

static void cells_push_font (roff_t * r , cells_t * c , unsigned char ch , int font)
{
	if (c -> len == c -> cap)
	{
		size_t ncap = c -> cap ? c -> cap * 2 : 128;
		cell_t * n = realloc (c -> v , ncap * sizeof *n);
		if (!n)
		{
			r -> oom = 1;
			return;
		}
		c -> v = n;
		c -> cap = ncap;
	}
	c -> v [c -> len].ch = ch;
	c -> v [c -> len].font = (unsigned char) font;
	c -> len++;
}

static void cells_push (roff_t * r , cells_t * c , unsigned char ch)
{
	cells_push_font (r , c , ch , r -> font);
}

static void emit (roff_t * r , const char * s , size_t n)
{
	if (sb_append (r -> out , s , n) < 0)
		r -> oom = 1;
}

static void put_spaces (roff_t * r , int n)
{
	for (; n > 0; n--)
		emit (r , " " , 1);
}

static void emit_cells (roff_t * r , const cells_t * c)
{
	for (size_t i = 0; i < c -> len; i++)
	{
		char ch = c -> v [i].ch == NBSP ? ' ' : (char) c -> v [i].ch;
		if (r -> styled && ch != ' ' && c -> v [i].font == FONT_B)
		{
			emit (r , &ch , 1);
			emit (r , "\b" , 1);
		}
		else if (r -> styled && ch != ' ' && c -> v [i].font == FONT_I)
		{
			emit (r , "_\b" , 2);
		}
		emit (r , &ch , 1);
	}
}

static void blank_line (roff_t * r)
{
	if (r -> printed && !r -> last_blank)
	{
		emit (r , "\n" , 1);
		r -> last_blank = 1;
	}
}

static void flush_line (roff_t * r)
{
	if (r -> line.len == 0 && !r -> have_tag)
		return;
	if (r -> have_tag)
	{
		put_spaces (r , r -> left);
		emit_cells (r , &r -> tag);
		if (r -> line.len > 0)
			put_spaces (r , r -> text_indent - r -> left - (int) r -> tag.len);
		r -> have_tag = 0;
	}
	else
	{
		put_spaces (r , r -> text_indent);
	}
	emit_cells (r , &r -> line);
	emit (r , "\n" , 1);
	r -> line.len = 0;
	r -> printed = 1;
	r -> last_blank = 0;
	r -> after_heading = 0;
}

static void emit_title (roff_t * r , int indent , const char * text)
{
	cells_t t = { NULL , 0 , 0 };
	int saved = r -> font;
	r -> font = FONT_B;
	for (; *text; text++)
		cells_push (r , &t , (unsigned char) *text);
	r -> font = saved;
	put_spaces (r , indent);
	emit_cells (r , &t);
	emit (r , "\n" , 1);
	free (t.v);
	r -> printed = 1;
	r -> last_blank = 0;
}

static void emit_triple (roff_t * r , const char * l , const char * m , const char * rt)
{
	size_t ll = strlen (l) , ml = strlen (m) , rl = strlen (rt);
	size_t w = (size_t) r -> width;
	size_t mid = w > ml ? (w - ml) / 2 : 0;
	size_t right = w > rl ? w - rl : 0;
	if (mid < ll + 1)
		mid = ll + 1;
	if (right < mid + ml + 1)
		right = mid + ml + 1;
	emit (r , l , ll);
	put_spaces (r , (int)(mid - ll));
	emit (r , m , ml);
	put_spaces (r , (int)(right - mid - ml));
	emit (r , rt , rl);
	emit (r , "\n" , 1);
	r -> printed = 1;
	r -> last_blank = 0;
}

static void place_word (roff_t * r)
{
	int room = r -> width - r -> text_indent;
	if (r -> word.len == 0)
		return;
	if (r -> line.len > 0 && (int)(r -> line.len + 1 + r -> word.len) > room)
		flush_line (r);
	if (r -> line.len > 0)
		cells_push_font (r , &r -> line , ' ' , FONT_R);
	for (size_t i = 0; i < r -> word.len; i++)
		cells_push_font (r , &r -> line , r -> word.v [i].ch , r -> word.v [i].font);
	r -> word.len = 0;
}

static void close_tag (roff_t * r)
{
	cells_t t;
	place_word (r);
	t = r -> tag;
	r -> tag = r -> line;
	r -> line = t;
	r -> line.len = 0;
	r -> text_indent = r -> left + r -> hang;
	if ((int) r -> tag.len < r -> hang)
	{
		r -> have_tag = 1;
	}
	else
	{
		put_spaces (r , r -> left);
		emit_cells (r , &r -> tag);
		emit (r , "\n" , 1);
		r -> printed = 1;
		r -> last_blank = 0;
	}
	r -> expect_tag = 0;
}

static const char * special_char (const char * name , size_t n)
{
	static const struct
	{
		const char * name;
		const char * text;
	} table [] = {
		{ "em" , "--" } , { "en" , "-" } , { "hy" , "-" } , { "bu" , "o" } ,
		{ "lq" , "\"" } , { "rq" , "\"" } , { "dq" , "\"" } , { "aq" , "'" } ,
		{ "oq" , "`" } , { "cq" , "'" } , { "co" , "(C)" } , { "rg" , "(R)" } ,
		{ "ha" , "^" } , { "ti" , "~" } , { "ga" , "`" } , { "ba" , "|" } ,
		{ "sl" , "/" } , { "rs" , "\\" } , { "Fo" , "<<" } , { "Fc" , ">>" }
	};
	for (size_t i = 0; i < sizeof table / sizeof table [0]; i++)
		if (strlen (table [i].name) == n && strncmp (table [i].name , name , n) == 0)
			return table [i].text;
	return "?";
}

static void put_char (roff_t * r , unsigned char ch)
{
	if (r -> fill)
	{
		if (ch == ' ' || ch == '\t')
			place_word (r);
		else
			cells_push (r , &r -> word , ch);
	}
	else
	{
		cells_push (r , &r -> line , ch);
	}
}

static void set_font (roff_t * r , int font)
{
	r -> prev_font = r -> font;
	r -> font = font;
}

static void add_text (roff_t * r , const char * s , size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		unsigned char c = (unsigned char) s [i];
		if (c != '\\' || i + 1 >= n)
		{
			put_char (r , c);
			continue;
		}
		c = (unsigned char) s [++i];
		switch (c)
		{
			case 'f':
				if (i + 1 < n)
				{
					char f = s [++i];
					if (f == 'B') set_font (r , FONT_B);
					else if (f == 'I') set_font (r , FONT_I);
					else if (f == 'P')
					{
						int t = r -> font;
						r -> font = r -> prev_font;
						r -> prev_font = t;
					}
					else set_font (r , FONT_R);
				}
				break;
			case '(':
				if (i + 2 < n)
				{
					for (const char * t = special_char (s + i + 1 , 2); *t; t++)
						put_char (r , (unsigned char) *t);
					i += 2;
				}
				break;
			case '[':
				{
					const char * close = memchr (s + i , ']' , n - i);
					if (close)
					{
						size_t len = (size_t)(close - (s + i + 1));
						for (const char * t = special_char (s + i + 1 , len); *t; t++)
							put_char (r , (unsigned char) *t);
						i = (size_t)(close - s);
					}
					break;
				}
			case '"':
				return;
			case 'e':
			case '\\':
				put_char (r , '\\');
				break;
			case '-':
				put_char (r , '-');
				break;
			case ' ':
			case '~':
				put_char (r , NBSP);
				break;
			case '&':
			case '|':
			case '^':
			case '/':
			case ',':
			case 'c':
				break;
			default:
				put_char (r , c);
		}
	}
}

static int split_args (char * s , char * argv [])
{
	int n = 0;
	while (*s && n < MAX_ARGS)
	{
		while (*s == ' ' || *s == '\t')
			s++;
		if (!*s)
			break;
		if (*s == '"')
		{
			char * w = ++s;
			argv [n++] = s;
			while (*s)
			{
				if (*s == '"' && s [1] == '"')
				{
					*w++ = '"';
					s += 2;
				}
				else if (*s == '"')
				{
					s++;
					break;
				}
				else
					*w++ = *s++;
			}
			*w = '\0';
		}
		else
		{
			argv [n++] = s;
			while (*s && *s != ' ' && *s != '\t')
				s++;
			if (*s)
				*s++ = '\0';
		}
	}
	return n;
}

static void break_line (roff_t * r)
{
	place_word (r);
	flush_line (r);
}

static void new_paragraph (roff_t * r)
{
	break_line (r);
	if (!r -> after_heading)
		blank_line (r);
	r -> after_heading = 0;
	r -> font = r -> prev_font = FONT_R;
	r -> left = BODY_INDENT + r -> rs;
	r -> text_indent = r -> left;
	r -> fill = 1;
}

static void req_th (roff_t * r , int argc , char ** argv)
{
	size_t i;
	if (argc > 0)
	{
		snprintf (r -> title , sizeof r -> title , "%s" , argv [0]);
		for (i = 0; r -> title [i]; i++)
			r -> title [i] = (char) toupper ((unsigned char) r -> title [i]);
	}
	if (argc > 1) snprintf (r -> section , sizeof r -> section , "%s" , argv [1]);
	if (argc > 2) snprintf (r -> date , sizeof r -> date , "%s" , argv [2]);
	if (argc > 3) snprintf (r -> source , sizeof r -> source , "%s" , argv [3]);
	if (argc > 4) snprintf (r -> manual , sizeof r -> manual , "%s" , argv [4]);
	{
		char who [96];
		snprintf (who , sizeof who , "%s(%s)" , r -> title , r -> section);
		emit_triple (r , who , r -> manual , who);
	}
}

static void req_heading (roff_t * r , int argc , char ** argv , int sub)
{
	char title [256] = "";
	for (int i = 0; i < argc; i++)
	{
		if (i)
			strncat (title , " " , sizeof title - strlen (title) - 1);
		strncat (title , argv [i] , sizeof title - strlen (title) - 1);
	}
	break_line (r);
	blank_line (r);
	if (!sub)
		r -> rs = 0;
	r -> hang = BODY_INDENT;
	emit_title (r , sub ? SUBSECT_INDENT : 0 , title);
	r -> after_heading = 1;
	r -> font = r -> prev_font = FONT_R;
	r -> left = BODY_INDENT + r -> rs;
	r -> text_indent = r -> left;
	r -> fill = 1;
}

static void req_fonts (roff_t * r , const char * name , int argc , char ** argv)
{
	static const char letters [] = { 'R' , 'B' , 'I' };
	int a = 0 , b = 0 , single = strlen (name) == 1;
	for (int i = 0; i < 3; i++)
	{
		if (name [0] == letters [i]) a = i;
		if (!single && name [1] == letters [i]) b = i;
	}
	if (single)
		b = a;
	if (argc == 0)
	{
		r -> next_line_font = a;
		return;
	}
	for (int i = 0; i < argc; i++)
	{
		int f = i % 2 ? b : a;
		char esc [3] = { '\\' , 'f' , letters [f] };
		add_text (r , esc , 3);
		add_text (r , argv [i] , strlen (argv [i]));
		if (single && i + 1 < argc)
			add_text (r , " " , 1);
	}
	add_text (r , "\\fR" , 3);
}

static void req_tp (roff_t * r , int argc , char ** argv)
{
	new_paragraph (r);
	if (argc > 0)
		r -> hang = atoi (argv [0]);
	r -> expect_tag = 1;
}

static void req_ip (roff_t * r , int argc , char ** argv)
{
	new_paragraph (r);
	if (argc > 1)
		r -> hang = atoi (argv [1]);
	r -> text_indent = r -> left + r -> hang;
	if (argc > 0)
	{
		r -> text_indent = r -> left;
		add_text (r , argv [0] , strlen (argv [0]));
		close_tag (r);
	}
}

static void request (roff_t * r , char * line)
{
	char * argv [MAX_ARGS];
	char name [16];
	size_t n = 0;
	int argc;
	line++;
	while (*line == ' ')
		line++;
	while (*line && *line != ' ' && *line != '\t' && n + 1 < sizeof name)
		name [n++] = *line++;
	name [n] = '\0';
	argc = split_args (line , argv);
	if (strcmp (name , "\\\"") == 0 || name [0] == '\\')
		return;
	else if (strcmp (name , "TH") == 0) req_th (r , argc , argv);
	else if (strcmp (name , "SH") == 0) req_heading (r , argc , argv , 0);
	else if (strcmp (name , "SS") == 0) req_heading (r , argc , argv , 1);
	else if (strcmp (name , "PP") == 0 || strcmp (name , "P") == 0 ||
	strcmp (name , "LP") == 0) new_paragraph (r);
	else if (strcmp (name , "TP") == 0) req_tp (r , argc , argv);
	else if (strcmp (name , "IP") == 0) req_ip (r , argc , argv);
	else if (strcmp (name , "RS") == 0)
	{
		break_line (r);
		r -> rs += argc > 0 ? atoi (argv [0]) : BODY_INDENT;
		r -> left = BODY_INDENT + r -> rs;
		r -> text_indent = r -> left;
	}
	else if (strcmp (name , "RE") == 0)
	{
		break_line (r);
		r -> rs -= BODY_INDENT;
		if (r -> rs < 0)
			r -> rs = 0;
		r -> left = BODY_INDENT + r -> rs;
		r -> text_indent = r -> left;
	}
	else if (strcmp (name , "nf") == 0 || strcmp (name , "EX") == 0)
	{
		break_line (r);
		r -> fill = 0;
	}
	else if (strcmp (name , "fi") == 0 || strcmp (name , "EE") == 0)
	{
		break_line (r);
		r -> fill = 1;
	}
	else if (strcmp (name , "br") == 0) break_line (r);
	else if (strcmp (name , "sp") == 0)
	{
		break_line (r);
		blank_line (r);
	}
	else if (name [0] && strspn (name , "BIR") == strlen (name) && strlen (name) <= 2)
		req_fonts (r , name , argc , argv);
}

static void end_input_line (roff_t * r)
{
	place_word (r);
	if (r -> expect_tag && r -> line.len > 0)
		close_tag (r);
	if (!r -> fill)
		flush_line (r);
}

static void text_line (roff_t * r , const char * s , size_t n)
{
	if (n == 0)
	{
		break_line (r);
		if (r -> fill)
			blank_line (r);
		else
		{
			emit (r , "\n" , 1);
			r -> last_blank = 1;
		}
		return;
	}
	if (r -> fill && (s [0] == ' ' || s [0] == '\t'))
		break_line (r);
	if (r -> next_line_font)
	{
		char esc [3] = { '\\' , 'f' , r -> next_line_font == FONT_B ? 'B' : 'I' };
		add_text (r , esc , 3);
		add_text (r , s , n);
		add_text (r , "\\fR" , 3);
		r -> next_line_font = 0;
	}
	else
		add_text (r , s , n);
}

int roff_render (const char * source , int width , int styled , strbuf_t * out)
{
	roff_t r;
	const char * p = source;
	memset (&r , 0 , sizeof r);
	r.out = out;
	r.width = width < 30 ? 30 : width;
	r.styled = styled;
	r.fill = 1;
	r.hang = BODY_INDENT;
	snprintf (r.title , sizeof r.title , "UNTITLED");
	snprintf (r.section , sizeof r.section , "1");
	while (*p && !r.oom)
	{
		const char * eol = strchr (p , '\n');
		size_t n = eol ? (size_t)(eol - p) : strlen (p);
		if (n > 0 && (p [0] == '.' || p [0] == '\''))
		{
			char * copy = malloc (n + 1);
			if (!copy)
			{
				r.oom = 1;
				break;
			}
			memcpy (copy , p , n);
			copy [n] = '\0';
			request (&r , copy);
			free (copy);
			end_input_line (&r);
		}
		else
		{
			text_line (&r , p , n);
			end_input_line (&r);
		}
		p = eol ? eol + 1 : p + n;
	}
	break_line (&r);
	blank_line (&r);
	{
		char who [96];
		snprintf (who , sizeof who , "%s(%s)" , r.title , r.section);
		emit_triple (&r , r.source , r.date , who);
	}
	free (r.word.v);
	free (r.line.v);
	free (r.tag.v);
	return r.oom ? -1 : 0;
}
