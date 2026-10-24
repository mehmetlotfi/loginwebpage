#!/bin/sh

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

# mkuser.sh - print one users-file line for loginserver.
#
#   tools/mkuser.sh Ali >> users.db        (asks for the password, hidden)
#   tools/mkuser.sh Ali 1234               (password on the command line)
#
# Output format:   username:sha256-hex-of-password;

sha256_hex() {
    if command -v sha256sum >/dev/null 2>&1; then sha256sum | cut -d' ' -f1
    elif command -v shasum >/dev/null 2>&1;  then shasum -a 256 | cut -d' ' -f1
    elif command -v sha256 >/dev/null 2>&1;  then sha256 -q
    elif command -v openssl >/dev/null 2>&1; then openssl dgst -sha256 | sed 's/^.*= *//'
    else echo "no SHA-256 tool found (sha256sum, shasum, sha256, openssl)" >&2; return 1
    fi
}

[ $# -ge 1 ] && [ $# -le 2 ] || { echo "usage: $0 USERNAME [PASSWORD]" >&2; exit 2; }
user=$1

case $user in
    ''|*[!A-Za-z0-9._@+=-]*)
        echo "username may only contain letters, digits and . _ @ + = -" >&2
        exit 2 ;;
esac

if [ $# -eq 2 ]; then
    pass=$2
else
    printf 'Password for %s: ' "$user" >&2
    stty -echo 2>/dev/null; IFS= read -r pass; stty echo 2>/dev/null; echo >&2
fi

hash=$(printf '%s' "$pass" | sha256_hex) || exit 1
printf '%s:%s;\n' "$user" "$hash"
