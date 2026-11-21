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
#include "server.h"
#include "acceptor.h"
#include "config.h"
#include "connection.h"
#include "log.h"
#include "netif.h"
#include "pool.h"
#include "privs.h"
#include "signals.h"

#include "errno.h"
#include "poll.h"
#include "stdlib.h"
#include "string.h"

static struct pollfd * make_poll_table (const listener_set_t * ls)
{
	struct pollfd * pfds = calloc (ls -> count + 1 , sizeof *pfds);
	if (!pfds)
		return NULL;
	for (size_t i = 0; i < ls -> count; i++)
	{
		pfds [i].fd = ls -> items [i].fd;
		pfds [i].events = POLLIN;
	}
	pfds [ls -> count].fd = signals_wake_fd ();
	pfds [ls -> count].events = POLLIN;
	return pfds;
}

static int accept_loop (listener_set_t * ls , pool_t * pool)
{
	struct pollfd * pfds = make_poll_table (ls);
	int rc = 0;
	if (!pfds)
	{
		log_error ("out of memory");
		return 1;
	}
	while (!signals_stop_requested ())
	{
		int r = poll (pfds , (nfds_t)(ls -> count + 1) , -1);
		if (r < 0)
		{
			if (errno == EINTR)
				continue;
			log_error ("poll: %s" , strerror (errno));
			rc = 1;
			break;
		}
		for (size_t i = 0; r > 0 && i < ls -> count; i++)
		{
			if (pfds [i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				log_warn ("listener %s failed - disabled" , ls -> items [i].label);
				pfds [i].fd = -1;
			}
			else if (pfds [i].revents & POLLIN)
			{
				acceptor_accept_ready (pfds [i].fd , pool);
			}
		}
	}
	free (pfds);
	return rc;
}

static void explain_listen_failure (const listener_set_t * ls , uint16_t port)
{
	if (ls -> count == 0 && ls -> last_errno == EACCES)
		log_error ("permission denied on port %u - run with sudo" , port);
	else if (ls -> last_errno == EADDRINUSE)
		log_error ("port %u is already in use" , port);
	else
		log_error ("could not listen on any interface (port %u)" , port);
}

int server_run (const userdb_t * db , uint16_t port)
{
	listener_set_t ls;
	connection_ctx_t cctx = { db };
	pool_t * pool;
	int rc;
	if (signals_install () != 0)
	{
		log_error ("cannot set up signal handling: %s" , strerror (errno));
		return 1;
	}
	if (netif_open_listeners (&ls , port) <= 0)
	{
		explain_listen_failure (&ls , port);
		netif_close_listeners (&ls);
		signals_cleanup ();
		return 1;
	}
	log_info ("listening on %zu address(es), port %u:" , ls.count , port);
	for (size_t i = 0; i < ls.count; i++)
		log_info ("  %-12s %s" , ls.items [i].ifname , ls.items [i].label);
	if (privs_drop () < 0)
	{
		netif_close_listeners (&ls);
		signals_cleanup ();
		return 1;
	}
	pool = pool_create (NUM_WORKERS , QUEUE_CAPACITY , connection_handle , &cctx);
	if (!pool)
	{
		log_error ("cannot start worker threads");
		netif_close_listeners (&ls);
		signals_cleanup ();
		return 1;
	}
	log_info ("%zu worker threads ready - press Ctrl+C to stop" , pool_workers (pool));
	rc = accept_loop (&ls , pool);
	if (signals_received ())
		log_info ("signal %d (%s) received - shutting down..." , signals_received () ,
		strsignal (signals_received ()));
	netif_close_listeners (&ls);
	pool_destroy (pool);
	signals_cleanup ();
	log_info ("stopped");
	return rc;
}
