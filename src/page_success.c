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
#include "page_success.h"
#include "page_common.h"

#include "string.h"

static const char OK_A [] =
"<main class='card ok'>\n";

static const char OK_B [] =
"<h1><span class='fa'>ورود شما درست بود</span><span class='en'>Login successful</span></h1>\n"
"<p class='sub'><span class='fa'>خوش آمدید،</span><span class='en'>Welcome,</span> <span class='badge'>";

static const char OK_C [] =
"</span></p>\n"
"<a class='btn' href='/'><span class='fa'>بازگشت به صفحه ورود</span><span class='en'>Back to sign in</span></a>\n"
"</main>\n";

#define APPEND(sb, lit) sb_append((sb), (lit), sizeof(lit) - 1)

int page_success_render (strbuf_t * sb , const char * username)
{
	page_begin (sb , "\xd9\x88\xd8\xb1\xd9\x88\xd8\xaf \xc2\xb7 Sign in");
	APPEND (sb , OK_A);
	page_emit_top (sb , PAGE_MARK_CHECK);
	APPEND (sb , OK_B);
	sb_append_html_escaped (sb , username , strlen (username));
	APPEND (sb , OK_C);
	return page_end (sb , NULL);
}
