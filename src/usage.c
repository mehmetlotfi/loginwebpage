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
#include "usage.h"

void usage_print (FILE * out , const char * prog)
{
	fprintf (out ,
	"Usage: %s USERS-FILE\n"
	"       %s -a USER USERS-FILE\n"
	"       %s -r USER USERS-FILE\n"
	"Try '%s --help' for the complete manual.\n" ,
	prog , prog , prog , prog);
}
