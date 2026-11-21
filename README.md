<div align="center">
  <h1>loginwebpage</h1>
  <p>A lightweight, multithreaded HTTP login page server written in C.</p>
</div>

[برای دیدن توضیحات به زبان فارسی اینجا را کلیک کنید](README-fa.md)

`loginwebpage` is a small web server that serves a login page and checks submitted credentials against a local user database. It is intended for development, demonstrations, and controlled testing of login flows and related web-client behavior.

## Features

- Bilingual (Persian/English) login, success and error pages with light and dark themes.
- User records loaded from a local text file; passwords verified with SHA-256 hashes.
- Multithreaded: a pool of worker threads fed by a bounded queue (excess load gets `503`, never a stall).
- Safe shutdown on `Ctrl+C` / `SIGTERM`: stops accepting, lets running requests finish (2 s grace), closes every socket, joins all threads and frees all memory. A second `Ctrl+C` exits at once.
- A broken users file is reported on `stderr` in the style of `gcc` (file:line:column, the faulty line, a `^~~~` marker, and a corrected example line).
- `--add-user` / `--remove-user` manage the users file; no helper scripts needed.
- `--help` shows a full manual page (standard `man` layout) in `less`.
- `./configure` checks the build requirements, tells you how to install what is missing, and writes the `Makefile`.

## Intended use

This project is suitable for testing login forms, authentication flows, browser behavior, and client-side integrations in a controlled environment. Use test accounts and non-sensitive passwords.

**Security notice:** This server uses plain HTTP and stores password verifiers as unsalted SHA-256 hashes. It is not intended for production authentication or for handling real user credentials. It listens on every network interface by default, so restrict access with a firewall or run it only on a trusted test network. Do not expose it to the public Internet.

## Requirements

- A C compiler such as GCC or Clang.
- `make` (GNU make or a BSD make), `sed`, and a POSIX shell for `./configure`.
- POSIX system headers and threading support.
- Optional: `less`, for the paged `--help`.

Supported systems: Linux, FreeBSD, OpenBSD, NetBSD, DragonFly BSD and Termux (Android). `./configure` prints the exact install command for your package manager when something is missing, for example:

```sh
sudo apt install build-essential        # Debian/Ubuntu
sudo dnf install gcc make glibc-devel   # Fedora
sudo pacman -S --needed base-devel      # Arch
pkg install clang make                  # Termux
```
## Clone Repository

```bash
git clone https://github.com/mehmetlotfi/loginwebpage.git

cd loginwebpage/
```

## Build

```sh
./configure
make
```

`./configure` writes the `Makefile` **only if every check passes**; if something is missing it explains how to install it and writes nothing. The text of the Makefile lives inside the `configure` script (there is no `Makefile.in`).

- Object files go to the `build/` directory, never into `src/` or `include/`.
- After you change a file, `make` recompiles only what depends on it.
- `make clean` deletes `build/` and the `loginwebpage` binary; `make distclean` also deletes the `Makefile`.
- If you add a `.c` file or an `#include` line, run `./configure` again so the dependency rules are regenerated.
- `sudo make install` installs the program and its manual page under `/usr/local` (change with `./configure --prefix=/usr`); `sudo make uninstall` removes them.

## Manage users

The users file has one entry per line:

```text
username:sha256-hex-of-password;
```

Create and edit it with the program itself (the password is asked twice and not shown):

```sh
./loginwebpage --add-user Ali users.db      # short form: -a Ali users.db
./loginwebpage --remove-user Ali users.db   # short form: -r Ali users.db
```

The file is created if it does not exist (mode `0600`). The commands refuse to touch a file that contains errors. For scripts, the password is read from standard input when it is not a terminal:

```sh
printf '%s\n' 'test-password' | ./loginwebpage -a testuser users.db
```

Restart the server after changing the file. An example file is provided as `users.db.example` (user `Ali`, password `1234`).

### Errors in the users file

When the server starts it checks the whole file and reports every problem, then refuses to start:

```text
users.db:2:5: error: password hash has 6 characters, expected 64 hex digits (SHA-256)
    2 | Bob:03ac67;
      |     ^~~~~~
users.db:2:1: note: a valid line looks like this (the hash is only an example):
    2 | Bob:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4;
users.db:3:70: error: missing ';' at the end of the line
    3 | Dave:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4
      |                                                                      ^
users.db:3:1: note: suggested fix for this line:
    3 | Dave:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4;
```

A *suggested fix* keeps your own name and hash and repairs only the format; when the hash itself is damaged, the note shows a valid line with an *example* hash.

## Run the server

```sh
./loginwebpage users.db
```

By default the server listens on TCP port `8080` on all network interfaces. Open `http://127.0.0.1:8080/` on the same machine, or `http://<SERVER_LAN_IP>:8080/` from another device on your trusted network. The application does not provide HTTPS.

Stop it with `Ctrl+C`.

## Help

```sh
./loginwebpage --help      # or -h: the full manual page, opened in less
./loginwebpage --version  # or -V
```

If standard output is not a terminal (for example `./loginwebpage --help | grep port`), the manual is printed as plain text.

## Change the listening port

The default port is `SERVER_PORT` in `include/config.h`. Edit it and rebuild, or pass a compiler definition:

```sh
make clean
make CPPFLAGS=-DSERVER_PORT=8081
```

Ports below 1000 need root: run the server with `sudo`; it gives up root rights as soon as the sockets are open.

## Repository layout

```text
loginwebpage/
├── include/          C header files (one per module)
├── src/              C source files (one job per file; manpage.c holds the manual)
├── users.db.example  Example user database
├── configure         Requirement check + Makefile generator
└── LICENSE           License text
```

Generated by `./configure` and `make` (removed by `make distclean`): `Makefile`, `build/`, `loginwebpage`.

## Troubleshooting

- **The port is already in use:** choose another `SERVER_PORT` and rebuild.
- **`./configure` fails:** run the install command it prints, then run it again.
- **The server rejects the user database:** read the `file:line:column` messages on `stderr`; each shows the faulty line and how it should look.
- **The browser cannot connect from another device:** confirm the server is running, use the correct LAN IP and port, and check firewall rules.

## License

This project is distributed under the GNU General Public License, version 3 or (at your option) any later version. See [`LICENSE`](LICENSE).

## Supporting the Project

If you found this content useful, I would appreciate your support for the project. You can contribute by **writing new guides, reporting bugs, offering suggestions, or simply sharing this repository with others.** If you would like to make a financial contribution, you can do so via [this link](https://github.com/mehmetlotfi). Every bit of support—no matter how small—motivates me to continue this work and maintain a high standard of quality.

Thank you for your support!
