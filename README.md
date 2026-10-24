<div align="center">
  <h1>loginwebpage</h1>
  <p>A lightweight, multithreaded HTTP login page server written in C.</p>
</div>

[برای دیدن توضیحات به زبان فارسی اینجا را کلیک کنید](README-fa.md)

`loginwebpage` is a small web server that serves a login page and checks submitted credentials against a local user database. It is intended for development, demonstrations, and controlled testing of login flows and related web-client behavior.

## Features

- Login page and success/error responses.
- User records loaded from a local text file.
- Password verification using SHA-256 hashes.
- Multithreaded connection handling.
- HTTP request parsing and input validation.
- Configurable listening port at compile time (default: `8080`).
- `configure` script to check build requirements.
- `Makefile` targets for building, installing, uninstalling, and cleaning.

## Intended use

This project is suitable for testing login forms, authentication flows, browser behavior, and client-side integrations in a controlled environment. Use test accounts and non-sensitive passwords.

**Security notice:** This server uses plain HTTP and stores password verifiers as unsalted SHA-256 hashes. It is not intended for production authentication or for handling real user credentials. It listens on every network interface by default, so restrict access with a firewall or run it only on a trusted test network. Do not expose it to the public Internet.

## Requirements

- A C compiler such as GCC or Clang.
- `make` (GNU make or a compatible BSD make).
- POSIX-compatible system headers and threading support.
- A POSIX shell for the `configure` and user-creation scripts.

The project is designed for Linux and BSD-family systems. On Debian/Ubuntu, install the usual build tools with:

```sh
sudo apt update
sudo apt install build-essential
```

On Fedora:

```sh
sudo dnf install gcc make glibc-devel
```

On Arch Linux:

```sh
sudo pacman -S --needed base-devel
```

## Download

```sh
git clone https://github.com/mehmetlotfi/loginwebpage.git
cd loginwebpage
```

## Check requirements and compile

Run the configuration check:

```sh
./configure
```

If the check succeeds, build the executable:

```sh
make
```

The resulting executable is named `loginwebpage`. Build object files are placed in `build/`.

To remove the executable and generated object files:

```sh
make clean
```

To install the executable under `/usr/local/bin`:

```sh
sudo make install
```

To uninstall it:

```sh
sudo make uninstall
```

The install prefix can be changed, for example:

```sh
sudo make install PREFIX=/usr
```

## Create a test user database

The server expects a users file in this format, with one entry per line:

```text
username:sha256-hex-of-password;
```

Each password hash must be exactly 64 hexadecimal characters. An example file is provided as `users.db.example`. Copy it to a separate working file:

```sh
cp users.db.example users.db
```

To add a test user, use the helper script. It prompts for the password without displaying it:

```sh
sh tools/mkuser.sh testuser >> users.db
```

You can also append a record while passing the password as a command-line argument, but this is discouraged because the password may be exposed through shell history or process listings:

```sh
sh tools/mkuser.sh testuser 'test-password' >> users.db
```

Do not commit a personal or sensitive user database to the repository. Keep `users.db` local and use only disposable test credentials. The supplied `users.db.example` is an example file, not a recommendation to use its accounts for anything sensitive.

## Run the server

From the project directory, pass the user database path as the only required argument:

```sh
./loginwebpage users.db
```

By default, the server listens on TCP port `8080` on all network interfaces. Open the following address on the same machine:

```text
http://127.0.0.1:8080/
```

To test it from another device on your trusted local network, use the server machine's LAN IP address:

```text
http://<SERVER_LAN_IP>:8080/
```

Only do this on a network where you control access. The application does not provide HTTPS.

To display command-line usage:

```sh
./loginwebpage --help
```

Stop the server with `Ctrl+C`.

## Change the listening port

The default port is defined by `SERVER_PORT` in `include/config.h`. You can change it there and rebuild, or pass a compiler definition:

```sh
make clean
make CPPFLAGS=-DSERVER_PORT=8081
```

Then visit `http://127.0.0.1:8081/`.

## Repository layout

```text
loginwebpage/
├── include/          C header files
├── src/              C source files
├── tools/mkuser.sh   Helper to create user database records
├── users.db.example  Example user database
├── configure         Build-requirements check
├── Makefile          Build and installation targets
└── LICENSE           License text
```

## Troubleshooting

- **The port is already in use:** choose another `SERVER_PORT` and rebuild.
- **The build check fails:** install the missing compiler, `make`, or C development headers reported by `./configure`, then run it again.
- **The server rejects the user database:** verify that every non-empty record follows `username:64-hex-character-SHA256-hash;`.
- **The browser cannot connect from another device:** confirm the server is running, use the correct LAN IP and port, and check firewall rules. Remember that the server binds to all interfaces by default.

## License

This project is distributed under the GNU General Public License, version 3 or (at your option) any later version. See [`LICENSE`](LICENSE).

## Supporting the Project

If you found this content useful, I would appreciate your support for the project. You can contribute by **writing new guides, reporting bugs, offering suggestions, or simply sharing this repository with others.** If you would like to make a financial contribution, you can do so via [this link](https://github.com/mehmetlotfi). Every bit of support—no matter how small—motivates me to continue this work and maintain a high standard of quality.

Thank you for your support!
