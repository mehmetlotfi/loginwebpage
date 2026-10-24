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

#ifndef CONFIG_H
#define CONFIG_H

#define APP_NAME    "loginserver"
#define APP_VERSION "1.0.0"

#ifndef SERVER_PORT
#define SERVER_PORT 8080
#endif

#if SERVER_PORT < 1 || SERVER_PORT > 65535
#error "SERVER_PORT must be between 1 and 65535"
#endif

#define PRIVILEGED_PORT_LIMIT 1000

#define LISTEN_BACKLOG 512

#define NUM_WORKERS 64

#define QUEUE_CAPACITY 4096

#define WORKER_STACK_SIZE (256 * 1024)

#define IO_TIMEOUT_SEC 10

#define LINGER_TIMEOUT_MS 1000

#define MAX_HEADER_SIZE 8192
#define MAX_BODY_SIZE   4096
#define MAX_PATH_LEN    256

#define MAX_USERNAME_LEN 64
#define MAX_DB_LINE_LEN  1024

#endif
