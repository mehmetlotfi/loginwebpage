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
#include "connection.h"
#include "config.h"
#include "http_request.h"
#include "http_response.h"
#include "http_status.h"
#include "log.h"
#include "page_error.h"
#include "peer.h"
#include "reply.h"
#include "route.h"
#include "sanitize.h"
#include "sockopt.h"
#include "strbuf.h"

#include "stdio.h"
#include "string.h"
#include "unistd.h"

void connection_handle (void * ctx_ , conn_t * conn)
{
	const connection_ctx_t * ctx = ctx_;
	int fd = conn -> fd;
	http_request_t req;
	strbuf_t body;
	reply_t rep;
	char ip [128];
	sb_init (&body);
	memset (&rep , 0 , sizeof rep);
	rep.content_type = "text/html; charset=utf-8";
	peer_to_string (conn , ip , sizeof ip);
	http_rr_t rr = http_read_request (fd , &req);
	if (rr == HTTP_RR_CLOSED || rr == HTTP_RR_IO_ERROR)
	{
		close (fd);
		return;
	}
	int head_only = 0;
	if (rr == HTTP_RR_OK)
	{
		head_only = strcmp (req.method , "HEAD") == 0;
		route_request (ctx -> db , &req , &body , &rep);
	}
	else
	{
		rep.status = http_rr_status (rr);
		page_error_render (&body , rep.status);
		snprintf (rep.note , sizeof rep.note , "bad request");
	}
	if (body.len == 0)
	{
		static const char fallback [] = "Internal Server Error\n";
		rep.status = 500;
		http_send_response (fd , 500 , "text/plain; charset=utf-8" , NULL ,
		fallback , sizeof fallback - 1 , head_only);
	}
	else
	{
		http_send_response (fd , rep.status , rep.content_type , rep.extra_headers ,
		body.data , body.len , head_only);
	}
	char method [32] , path [MAX_PATH_LEN];
	const char * m = rr == HTTP_RR_OK ? req.method : "-";
	const char * p = rr == HTTP_RR_OK ? req.path : "-";
	sanitize_for_log (m , strlen (m) , method , sizeof method);
	sanitize_for_log (p , strlen (p) , path , sizeof path);
	log_info ("%s \"%s %s\" %d%s%s" , ip , method , path , rep.status ,
	rep.note [0] ? " " : "" , rep.note);
	sb_free (&body);
	sock_lingering_close (fd);
}
