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
#include "page_login.h"
#include "page_common.h"

#include "string.h"

static const char LOGIN_A [] =
"<main class='card";

static const char LOGIN_A2 [] =
"'>\n";

static const char LOGIN_B [] =
"<h1><span class='fa'>ورود به حساب</span><span class='en'>Sign in</span></h1>\n"
"<p class='sub'><span class='fa'>نام کاربری و رمز عبورتان را وارد کنید.</span><span class='en'>Enter your username and password.</span></p>\n";

static const char ALERT [] =
"<div class='alert' role='alert'><svg viewBox='0 0 24 24'><circle cx='12' cy='12' r='10'/><path d='M12 7v6M12 16.5v.01'/></svg><div><span class='fa'>نام کاربری یا رمز عبور اشتباه است.</span><span class='en'>Incorrect username or password.</span></div></div>\n";

static const char LOGIN_C [] =
"<form method='post' action='/login' id='f'>\n"
"<label class='lbl' for='u'><span class='fa'>نام کاربری</span><span class='en'>Username</span></label>\n"
"<div class='field'><input id='u' name='username' type='text' value='";

static const char LOGIN_D [] =
"' maxlength='64' required autocomplete='username' autocapitalize='none' autocorrect='off' spellcheck='false'></div>\n"
"<label class='lbl' for='p'><span class='fa'>رمز عبور</span><span class='en'>Password</span></label>\n"
"<div class='field pw'><input id='p' name='password' type='password' required autocomplete='current-password'>\n"
"<button type='button' class='eye' id='eye' hidden aria-pressed='false' aria-label='نمایش رمز عبور' data-fa='نمایش رمز عبور' data-en='Show password'>\n"
"<svg class='show' viewBox='0 0 24 24'><path d='M2 12s3.6-7 10-7 10 7 10 7-3.6 7-10 7S2 12 2 12z'/><circle cx='12' cy='12' r='3'/></svg>\n"
"<svg class='hide' viewBox='0 0 24 24'><path d='M3 3l18 18'/><path d='M10.6 6.1A10.5 10.5 0 0 1 12 6c6.4 0 10 6 10 6a17 17 0 0 1-3.2 3.9M6.6 7.6A16.5 16.5 0 0 0 2 12s3.6 6 10 6a10 10 0 0 0 4.4-1'/><path d='M9.9 9.9a3 3 0 0 0 4.2 4.2'/></svg>\n"
"</button></div>\n"
"<button class='btn' type='submit' id='go'><span class='spin'></span><span class='fa'>ورود</span><span class='en'>Sign in</span></button>\n"
"</form>\n"
"</main>\n";

static const char JS_LOGIN [] =
"(function(){var d=document,u=d.getElementById('u'),p=d.getElementById('p'),e=d.getElementById('eye'),f=d.getElementById('f'),g=d.getElementById('go');\n"
"function reset(){g.disabled=false;g.classList.remove('busy');p.type='password';e.classList.remove('on');e.setAttribute('aria-pressed','false')}\n"
"e.hidden=false;\n"
"e.addEventListener('mousedown',function(x){x.preventDefault()});\n"
"e.addEventListener('click',function(){var s=p.type==='password';p.type=s?'text':'password';e.classList.toggle('on',s);e.setAttribute('aria-pressed',s?'true':'false')});\n"
"f.addEventListener('submit',function(){g.disabled=true;g.classList.add('busy')});\n"
"window.addEventListener('pageshow',reset);\n"
"if(window.matchMedia&&window.matchMedia('(hover:hover)').matches){(d.querySelector('.alert')?p:u).focus()}\n"
"})();\n";

#define APPEND(sb, lit) sb_append((sb), (lit), sizeof(lit) - 1)

int page_login_render (strbuf_t * sb , const char * username , int show_error)
{
	page_begin (sb , "\xd9\x88\xd8\xb1\xd9\x88\xd8\xaf \xc2\xb7 Sign in");
	APPEND (sb , LOGIN_A);
	if (show_error)
		sb_append_str (sb , " err");
	APPEND (sb , LOGIN_A2);
	page_emit_top (sb , PAGE_MARK_KEYHOLE);
	APPEND (sb , LOGIN_B);
	if (show_error)
		APPEND (sb , ALERT);
	APPEND (sb , LOGIN_C);
	if (username)
		sb_append_html_escaped (sb , username , strlen (username));
	APPEND (sb , LOGIN_D);
	return page_end (sb , JS_LOGIN);
}
