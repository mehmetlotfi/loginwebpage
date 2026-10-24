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
#include "login.h"
#include "config.h"
#include "http_form.h"
#include "page_login.h"
#include "page_success.h"
#include "sanitize.h"
#include "secure.h"

#include "stdio.h"

void login_process (const userdb_t * db , const http_request_t * req ,
strbuf_t * body , reply_t * rep)
{
	char user [MAX_USERNAME_LEN + 1];
	char pass [MAX_BODY_SIZE + 1];
	size_t ulen = 0 , plen = 0;
	int ok = 0;
	int have_user = 0;
	if (http_form_get (req -> body , req -> body_len , "username" , user ,
	sizeof user , &ulen) == 0)
		have_user = 1;
	else
		user [0] = '\0';
	if (have_user &&
	http_form_get (req -> body , req -> body_len , "password" , pass ,
	sizeof pass , &plen) == 0)
		ok = userdb_check (db , user , ulen , pass , plen);
	secure_zero (pass , sizeof pass);
	char shown [MAX_USERNAME_LEN + 1];
	sanitize_for_log (user , ulen , shown , sizeof shown);
	if (ok)
	{
		rep -> status = 200;
		snprintf (rep -> note , sizeof rep -> note , "login OK user=%s" , shown);
		if (page_success_render (body , user) < 0)
			rep -> status = 500;
	}
	else
	{
		rep -> status = 403;
		snprintf (rep -> note , sizeof rep -> note , "login FAILED user=%s" , shown);
		if (page_login_render (body , have_user ? user : "" , 1) < 0)
			rep -> status = 500;
	}
}
