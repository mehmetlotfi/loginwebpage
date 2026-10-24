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
#include "pool.h"
#include "config.h"
#include "log.h"

#include "errno.h"
#include "pthread.h"
#include "signal.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

struct pool
{
	pthread_mutex_t mu;
	pthread_cond_t not_empty;
	int stopping;
	conn_t * queue;
	size_t capacity;
	size_t head;
	size_t count;
	pthread_t * threads;
	size_t nthreads;
	conn_handler_fn handler;
	void * ctx;
};

static void * worker_main (void * arg)
{
	pool_t * p = arg;
	for (;;)
	{
		conn_t c;
		pthread_mutex_lock (&p -> mu);
		while (p -> count == 0 && !p -> stopping)
			pthread_cond_wait (&p -> not_empty , &p -> mu);
		if (p -> stopping)
		{
			pthread_mutex_unlock (&p -> mu);
			break;
		}
		c = p -> queue [p -> head];
		p -> head = (p -> head + 1) % p -> capacity;
		p -> count--;
		pthread_mutex_unlock (&p -> mu);
		p -> handler (p -> ctx , &c);
	}
	return NULL;
}

pool_t * pool_create (size_t workers , size_t queue_capacity ,
conn_handler_fn handler , void * ctx)
{
	pool_t * p = calloc (1 , sizeof *p);
	if (!p)
		return NULL;
	p -> queue = calloc (queue_capacity , sizeof *p -> queue);
	p -> threads = calloc (workers , sizeof *p -> threads);
	if (!p -> queue || !p -> threads)
		goto fail;
	p -> capacity = queue_capacity;
	p -> handler = handler;
	p -> ctx = ctx;
	if (pthread_mutex_init (&p -> mu , NULL) != 0)
		goto fail;
	if (pthread_cond_init (&p -> not_empty , NULL) != 0)
	{
		pthread_mutex_destroy (&p -> mu);
		goto fail;
	}
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setstacksize (&attr , WORKER_STACK_SIZE);
	sigset_t all , old;
	sigfillset (&all);
	pthread_sigmask (SIG_BLOCK , &all , &old);
	for (size_t i = 0; i < workers; i++)
	{
		int rc = pthread_create (&p -> threads [p -> nthreads] , &attr , worker_main , p);
		if (rc != 0)
		{
			log_warn ("pthread_create failed after %zu threads: %s" ,
			p -> nthreads , strerror (rc));
			break;
		}
		p -> nthreads++;
	}
	pthread_sigmask (SIG_SETMASK , &old , NULL);
	pthread_attr_destroy (&attr);
	if (p -> nthreads == 0)
	{
		pthread_cond_destroy (&p -> not_empty);
		pthread_mutex_destroy (&p -> mu);
		goto fail;
	}
	return p;
	fail:
	free (p -> queue);
	free (p -> threads);
	free (p);
	return NULL;
}

int pool_submit (pool_t * p , const conn_t * conn)
{
	int rc = -1;
	pthread_mutex_lock (&p -> mu);
	if (p -> count < p -> capacity)
	{
		size_t tail = (p -> head + p -> count) % p -> capacity;
		p -> queue [tail] = *conn;
		p -> count++;
		rc = 0;
		pthread_cond_signal (&p -> not_empty);
	}
	pthread_mutex_unlock (&p -> mu);
	return rc;
}

size_t pool_workers (const pool_t * p)
{
	return p -> nthreads;
}

void pool_destroy (pool_t * p)
{
	if (!p)
		return;
	pthread_mutex_lock (&p -> mu);
	p -> stopping = 1;
	pthread_cond_broadcast (&p -> not_empty);
	pthread_mutex_unlock (&p -> mu);
	for (size_t i = 0; i < p -> nthreads; i++)
		pthread_join (p -> threads [i] , NULL);
	while (p -> count > 0)
	{
		close (p -> queue [p -> head].fd);
		p -> head = (p -> head + 1) % p -> capacity;
		p -> count--;
	}
	pthread_cond_destroy (&p -> not_empty);
	pthread_mutex_destroy (&p -> mu);
	free (p -> queue);
	free (p -> threads);
	free (p);
}
