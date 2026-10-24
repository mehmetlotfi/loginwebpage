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
#include "route.h"
#include "login.h"
#include "page_error.h"
#include "page_login.h"

#include "string.h"

static int is_login_path (const char * path)
{
	return strcmp (path , "/") == 0 || strcmp (path , "/login") == 0;
}

void route_request (const userdb_t * db , const http_request_t * req ,
strbuf_t * body , reply_t * rep)
{
	rep -> status = 200;
	rep -> content_type = "text/html; charset=utf-8";
	rep -> extra_headers = NULL;
	rep -> note [0] = '\0';
	int is_get = strcmp (req -> method , "GET") == 0 || strcmp (req -> method , "HEAD") == 0;
	int is_post = strcmp (req -> method , "POST") == 0;
	if (!is_login_path (req -> path))
	{
		rep -> status = 404;
	}
	else if (is_get)
	{
		if (page_login_render (body , NULL , 0) < 0)
			rep -> status = 500;
	}
	else if (is_post)
	{
		login_process (db , req , body , rep);
	}
	else
	{
		rep -> status = 405;
		rep -> extra_headers = "Allow: GET, HEAD, POST\r\n";
	}
	if (rep -> status >= 400 && rep -> status != 403)
	{
		sb_free (body);
		sb_init (body);
		page_error_render (body , rep -> status);
	}
}
