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
#include "time.h"
#include "unistd.h"

struct pool;

typedef struct
{
	struct pool * pool;
	size_t index;
} worker_arg_t;

struct pool
{
	pthread_mutex_t mu;
	pthread_cond_t not_empty;
	pthread_cond_t idle;
	int stopping;
	conn_t * queue;
	size_t capacity;
	size_t head;
	size_t count;
	pthread_t * threads;
	worker_arg_t * args;
	int * active_fd;
	size_t nthreads;
	size_t busy;
	conn_handler_fn handler;
	void * ctx;
};

static void * worker_main (void * arg_)
{
	worker_arg_t * arg = arg_;
	pool_t * p = arg -> pool;
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
		p -> active_fd [arg -> index] = c.fd;
		p -> busy++;
		pthread_mutex_unlock (&p -> mu);
		p -> handler (p -> ctx , &c);
		pthread_mutex_lock (&p -> mu);
		p -> active_fd [arg -> index] = -1;
		p -> busy--;
		if (p -> busy == 0)
			pthread_cond_broadcast (&p -> idle);
		pthread_mutex_unlock (&p -> mu);
		close (c.fd);
	}
	return NULL;
}

static void pool_free (pool_t * p)
{
	free (p -> queue);
	free (p -> threads);
	free (p -> args);
	free (p -> active_fd);
	free (p);
}

pool_t * pool_create (size_t workers , size_t queue_capacity ,
conn_handler_fn handler , void * ctx)
{
	pool_t * p = calloc (1 , sizeof *p);
	pthread_attr_t attr;
	sigset_t all , old;
	if (!p)
		return NULL;
	p -> queue = calloc (queue_capacity , sizeof *p -> queue);
	p -> threads = calloc (workers , sizeof *p -> threads);
	p -> args = calloc (workers , sizeof *p -> args);
	p -> active_fd = calloc (workers , sizeof *p -> active_fd);
	if (!p -> queue || !p -> threads || !p -> args || !p -> active_fd)
	{
		pool_free (p);
		return NULL;
	}
	for (size_t i = 0; i < workers; i++)
		p -> active_fd [i] = -1;
	p -> capacity = queue_capacity;
	p -> handler = handler;
	p -> ctx = ctx;
	pthread_mutex_init (&p -> mu , NULL);
	pthread_cond_init (&p -> not_empty , NULL);
	pthread_cond_init (&p -> idle , NULL);
	pthread_attr_init (&attr);
	pthread_attr_setstacksize (&attr , WORKER_STACK_SIZE);
	sigfillset (&all);
	pthread_sigmask (SIG_BLOCK , &all , &old);
	for (size_t i = 0; i < workers; i++)
	{
		p -> args [i].pool = p;
		p -> args [i].index = i;
		int rc = pthread_create (&p -> threads [i] , &attr , worker_main , &p -> args [i]);
		if (rc != 0)
		{
			log_warn ("pthread_create failed after %zu threads: %s" , p -> nthreads ,
			strerror (rc));
			break;
		}
		p -> nthreads++;
	}
	pthread_sigmask (SIG_SETMASK , &old , NULL);
	pthread_attr_destroy (&attr);
	if (p -> nthreads == 0)
	{
		pthread_cond_destroy (&p -> idle);
		pthread_cond_destroy (&p -> not_empty);
		pthread_mutex_destroy (&p -> mu);
		pool_free (p);
		return NULL;
	}
	return p;
}

int pool_submit (pool_t * p , const conn_t * conn)
{
	int rc = -1;
	pthread_mutex_lock (&p -> mu);
	if (!p -> stopping && p -> count < p -> capacity)
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

static struct timespec deadline_in (long ms)
{
	struct timespec t;
	clock_gettime (CLOCK_REALTIME , &t);
	t.tv_sec += ms / 1000;
	t.tv_nsec += (ms % 1000) * 1000000L;
	if (t.tv_nsec >= 1000000000L)
	{
		t.tv_sec++;
		t.tv_nsec -= 1000000000L;
	}
	return t;
}

void pool_destroy (pool_t * p)
{
	struct timespec limit;
	if (!p)
		return;
	pthread_mutex_lock (&p -> mu);
	p -> stopping = 1;
	pthread_cond_broadcast (&p -> not_empty);
	limit = deadline_in (SHUTDOWN_GRACE_MS);
	while (p -> busy > 0)
		if (pthread_cond_timedwait (&p -> idle , &p -> mu , &limit) == ETIMEDOUT)
			break;
	if (p -> busy > 0)
	{
		log_warn ("%zu connection(s) still busy - closing them" , p -> busy);
		for (size_t i = 0; i < p -> nthreads; i++)
			if (p -> active_fd [i] >= 0)
				shutdown (p -> active_fd [i] , SHUT_RDWR);
	}
	pthread_mutex_unlock (&p -> mu);
	for (size_t i = 0; i < p -> nthreads; i++)
		pthread_join (p -> threads [i] , NULL);
	while (p -> count > 0)
	{
		close (p -> queue [p -> head].fd);
		p -> head = (p -> head + 1) % p -> capacity;
		p -> count--;
	}
	pthread_cond_destroy (&p -> idle);
	pthread_cond_destroy (&p -> not_empty);
	pthread_mutex_destroy (&p -> mu);
	pool_free (p);
}
