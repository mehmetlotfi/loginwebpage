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
#include "sanitize.h"

void sanitize_for_log (const char * in , size_t n , char * out , size_t cap)
{
	size_t o = 0;
	if (cap == 0)
		return;
	for (size_t i = 0; i < n && o + 1 < cap; i++)
	{
		unsigned char c = (unsigned char) in [i];
		out [o++] = (c >= 0x20 && c < 0x7f) ? (char) c : '?';
	}
	out [o] = '\0';
}
