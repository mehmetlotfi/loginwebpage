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
#include "page_common.h"
#include "page_css.h"

static const char HEAD_A [] =
"<!DOCTYPE html>\n"
"<html lang='fa' dir='rtl'>\n"
"<head>\n"
"<meta charset='utf-8'>\n"
"<meta name='viewport' content='width=device-width,initial-scale=1,viewport-fit=cover'>\n"
"<meta name='color-scheme' content='light dark'>\n"
"<meta name='robots' content='noindex,nofollow'>\n"
"<link rel='icon' href=\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64' fill='none' stroke='%231d3a99' stroke-width='5' stroke-linejoin='round'%3E%3Crect x='14' y='14' width='36' height='36'/%3E%3Crect x='14' y='14' width='36' height='36' transform='rotate(45 32 32)'/%3E%3C/svg%3E\">\n"
"<title>";

static const char HEAD_B [] =
"</title>\n"
"<style>";

static const char HEAD_C [] =
"</style>\n"
"</head>\n"
"<body>\n";

static const char ART_A [] =
"<div class='stage'>\n"
"<aside class='art'>\n"
"<p class='brand'>loginwebpage</p>\n"
"<div class='emblem'><svg viewBox='0 0 200 200' aria-hidden='true'>"
"<g class='spin'>"
"<rect class='star d' pathLength='1' x='40' y='40' width='120' height='120' rx='3'/>"
"<rect class='star d d2' pathLength='1' x='40' y='40' width='120' height='120' rx='3' transform='rotate(45 100 100)'/>"
"</g>"
"<g class='spin spin2'>"
"<circle class='ring' cx='100' cy='100' r='93'/>"
"<rect class='ring d d2' pathLength='1' x='62' y='62' width='76' height='76' rx='2' transform='rotate(22.5 100 100)'/>"
"</g>";

static const char KEYHOLE [] =
"<circle class='key d d3' pathLength='1' cx='100' cy='94' r='9'/>"
"<path class='key d d3' pathLength='1' d='M100 103V124'/>"
"<circle class='key-dot' cx='100' cy='94' r='2.5'/>";

static const char CHECK [] =
"<path class='key d d3' pathLength='1' d='M78 102l16 16 30-34'/>";

static const char ART_B [] =
"</svg></div>\n"
"<div class='art-text'>"
"<h2><span class='fa'>خوش آمدید</span><span class='en'>Welcome</span></h2>"
"<p><span class='fa'>برای ادامه وارد حساب خود شوید.</span><span class='en'>Sign in to your account to continue.</span></p>"
"</div>\n"
"</aside>\n"
"<main class='panel'>\n"
"<div class='bar'><button type='button' class='lang' id='lang' aria-label='تغییر زبان' data-fa='تغییر زبان' data-en='Change language'><span class='fa'>English</span><span class='en'>فارسی</span></button></div>\n";

static const char END_BOX [] =
"</div>\n"
"</main>\n"
"</div>\n"
"<script>\n";

static const char JS_LANG [] =
"(function(){var h=document.documentElement;\n"
"function apply(l){h.lang=l;h.dir=l==='fa'?'rtl':'ltr';var e=document.querySelectorAll('[data-fa]');for(var i=0;i<e.length;i++){var t=e[i].getAttribute(l==='fa'?'data-fa':'data-en');e[i].setAttribute('aria-label',t);e[i].title=t}}\n"
"var s=null;try{s=localStorage.getItem('lang')}catch(x){}\n"
"if(s==='en')apply('en');\n"
"var b=document.getElementById('lang');\n"
"if(b)b.addEventListener('click',function(){var l=h.lang==='fa'?'en':'fa';apply(l);try{localStorage.setItem('lang',l)}catch(x){}});\n"
"})();\n";

static const char SCRIPT_CLOSE [] =
"</script>\n"
"</body>\n"
"</html>\n";

#define APPEND(sb, lit) sb_append((sb), (lit), sizeof(lit) - 1)

void page_begin (strbuf_t * sb , const char * title)
{
	APPEND (sb , HEAD_A);
	sb_append_str (sb , title);
	APPEND (sb , HEAD_B);
	sb_append_str (sb , PAGE_CSS);
	APPEND (sb , HEAD_C);
}

void page_emit_top (strbuf_t * sb , page_mark_t mark)
{
	APPEND (sb , ART_A);
	if (mark == PAGE_MARK_CHECK)
		APPEND (sb , CHECK);
	else
		APPEND (sb , KEYHOLE);
	APPEND (sb , ART_B);
}

int page_end (strbuf_t * sb , const char * page_js)
{
	APPEND (sb , END_BOX);
	APPEND (sb , JS_LANG);
	if (page_js)
		sb_append_str (sb , page_js);
	APPEND (sb , SCRIPT_CLOSE);
	return sb -> failed ? -1 : 0;
}
