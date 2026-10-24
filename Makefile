# Makefile for loginwebpage
#
#  loginwebpage - Login Page on web
#  Copyright (C) 2026  Mehmet Lotfi
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#
# Written in the portable subset of make syntax: it works unchanged with GNU
# make (Linux, gmake on the BSDs) and with BSD make (FreeBSD, OpenBSD,
# NetBSD, DragonFly).
#
# Layout:
#   src/           C sources              include/   header files
#   build/         every file produced by compiling (object files) - created on
#                  demand, removed by "make clean"
#   loginwebpage   the final binary (also removed by "make clean")
#
# Incremental builds: each object file depends on its own .c file and on
# exactly the headers it includes (directly or indirectly), so after a change
# only the affected files are recompiled and the binary is relinked.
# If you add or remove an #include, update that object's dependency line below.
#
# Targets:
#   make              build ./loginwebpage
#   make install      install to $(PREFIX)/bin   (default PREFIX=/usr/local)
#   make uninstall
#   make clean        delete build/ and the binary
#
# Change the TCP port (default 8080) with either of:
#   - editing SERVER_PORT in include/config.h
#   - make clean && make CPPFLAGS=-DSERVER_PORT=80

TARGET   = loginwebpage
BUILDDIR = build
PREFIX   = /usr/local

CFLAGS   = -O2 -std=gnu99 -Wall -Wextra
CPPFLAGS =
LDFLAGS  =
LIBS     =
THREADS  = -pthread
INCLUDES = -I include

OBJS = \
    build/main.o \
    build/acceptor.o \
    build/connection.o \
    build/http_form.o \
    build/http_request.o \
    build/http_response.o \
    build/http_status.o \
    build/listener.o \
    build/log.o \
    build/login.o \
    build/netaddr.o \
    build/netif.o \
    build/page_common.o \
    build/page_error.o \
    build/page_login.o \
    build/page_success.o \
    build/peer.o \
    build/pool.o \
    build/privs.o \
    build/rootcheck.o \
    build/route.o \
    build/sanitize.o \
    build/secure.o \
    build/server.o \
    build/sha256.o \
    build/signals.o \
    build/sockopt.o \
    build/strbuf.o \
    build/usage.o \
    build/userdb_load.o \
    build/userdb_lookup.o \
    build/userdb_parse.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(THREADS) -o $(TARGET) $(OBJS) $(LIBS)

build/main.o: src/main.c \
    include/config.h \
    include/log.h \
    include/platform.h \
    include/rootcheck.h \
    include/server.h \
    include/usage.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/main.c -o build/main.o

build/acceptor.o: src/acceptor.c \
    include/acceptor.h \
    include/log.h \
    include/platform.h \
    include/pool.h \
    include/sockopt.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/acceptor.c -o build/acceptor.o

build/connection.o: src/connection.c \
    include/config.h \
    include/connection.h \
    include/http_request.h \
    include/http_response.h \
    include/http_status.h \
    include/log.h \
    include/page_error.h \
    include/peer.h \
    include/platform.h \
    include/pool.h \
    include/reply.h \
    include/route.h \
    include/sanitize.h \
    include/sockopt.h \
    include/strbuf.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/connection.c -o build/connection.o

build/http_form.o: src/http_form.c \
    include/http_form.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/http_form.c -o build/http_form.o

build/http_request.o: src/http_request.c \
    include/config.h \
    include/http_request.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/http_request.c -o build/http_request.o

build/http_response.o: src/http_response.c \
    include/config.h \
    include/http_request.h \
    include/http_response.h \
    include/http_status.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/http_response.c -o build/http_response.o

build/http_status.o: src/http_status.c \
    include/config.h \
    include/http_request.h \
    include/http_status.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/http_status.c -o build/http_status.o

build/listener.o: src/listener.c \
    include/config.h \
    include/listener.h \
    include/netaddr.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/listener.c -o build/listener.o

build/log.o: src/log.c \
    include/log.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/log.c -o build/log.o

build/login.o: src/login.c \
    include/config.h \
    include/http_form.h \
    include/http_request.h \
    include/login.h \
    include/page_login.h \
    include/page_success.h \
    include/platform.h \
    include/reply.h \
    include/sanitize.h \
    include/secure.h \
    include/strbuf.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/login.c -o build/login.o

build/netaddr.o: src/netaddr.c \
    include/netaddr.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/netaddr.c -o build/netaddr.o

build/netif.o: src/netif.c \
    include/config.h \
    include/listener.h \
    include/log.h \
    include/netaddr.h \
    include/netif.h \
    include/platform.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/netif.c -o build/netif.o

build/page_common.o: src/page_common.c \
    include/page_common.h \
    include/platform.h \
    include/strbuf.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/page_common.c -o build/page_common.o

build/page_error.o: src/page_error.c \
    include/page_common.h \
    include/page_error.h \
    include/platform.h \
    include/strbuf.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/page_error.c -o build/page_error.o

build/page_login.o: src/page_login.c \
    include/page_common.h \
    include/page_login.h \
    include/platform.h \
    include/strbuf.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/page_login.c -o build/page_login.o

build/page_success.o: src/page_success.c \
    include/page_common.h \
    include/page_success.h \
    include/platform.h \
    include/strbuf.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/page_success.c -o build/page_success.o

build/peer.o: src/peer.c \
    include/peer.h \
    include/platform.h \
    include/pool.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/peer.c -o build/peer.o

build/pool.o: src/pool.c \
    include/config.h \
    include/log.h \
    include/platform.h \
    include/pool.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/pool.c -o build/pool.o

build/privs.o: src/privs.c \
    include/log.h \
    include/platform.h \
    include/privs.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/privs.c -o build/privs.o

build/rootcheck.o: src/rootcheck.c \
    include/config.h \
    include/platform.h \
    include/rootcheck.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/rootcheck.c -o build/rootcheck.o

build/route.o: src/route.c \
    include/config.h \
    include/http_request.h \
    include/login.h \
    include/page_error.h \
    include/page_login.h \
    include/platform.h \
    include/reply.h \
    include/route.h \
    include/strbuf.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/route.c -o build/route.o

build/sanitize.o: src/sanitize.c \
    include/platform.h \
    include/sanitize.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/sanitize.c -o build/sanitize.o

build/secure.o: src/secure.c \
    include/platform.h \
    include/secure.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/secure.c -o build/secure.o

build/server.o: src/server.c \
    include/acceptor.h \
    include/config.h \
    include/connection.h \
    include/log.h \
    include/netif.h \
    include/platform.h \
    include/pool.h \
    include/privs.h \
    include/server.h \
    include/signals.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/server.c -o build/server.o

build/sha256.o: src/sha256.c \
    include/platform.h \
    include/sha256.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/sha256.c -o build/sha256.o

build/signals.o: src/signals.c \
    include/platform.h \
    include/signals.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/signals.c -o build/signals.o

build/sockopt.o: src/sockopt.c \
    include/config.h \
    include/platform.h \
    include/sockopt.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/sockopt.c -o build/sockopt.o

build/strbuf.o: src/strbuf.c \
    include/platform.h \
    include/strbuf.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/strbuf.c -o build/strbuf.o

build/usage.o: src/usage.c \
    include/config.h \
    include/platform.h \
    include/usage.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/usage.c -o build/usage.o

build/userdb_load.o: src/userdb_load.c \
    include/platform.h \
    include/userdb.h \
    include/userdb_parse.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/userdb_load.c -o build/userdb_load.o

build/userdb_lookup.o: src/userdb_lookup.c \
    include/config.h \
    include/platform.h \
    include/secure.h \
    include/sha256.h \
    include/userdb.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/userdb_lookup.c -o build/userdb_lookup.o

build/userdb_parse.o: src/userdb_parse.c \
    include/config.h \
    include/platform.h \
    include/userdb.h \
    include/userdb_parse.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(THREADS) -c src/userdb_parse.c -o build/userdb_parse.o

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all install uninstall clean
