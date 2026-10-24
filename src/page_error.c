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
#include "page_error.h"
#include "page_common.h"

static const char ERR_A [] =
"<main class='card'>\n";

static const char ERR_B [] =
"<div class='code'>";

static const char ERR_C [] =
"</div>\n"
"<h1>";

static const char ERR_D [] =
"</h1>\n"
"<p class='sub'>";

static const char ERR_E [] =
"</p>\n"
"<a class='btn' href='/'><span class='fa'>صفحه ورود</span><span class='en'>Sign-in page</span></a>\n"
"</main>\n";

#define APPEND(sb, lit) sb_append((sb), (lit), sizeof(lit) - 1)

typedef struct
{
	int status;
	const char * fa_title;
	const char * en_title;
	const char * fa_msg;
	const char * en_msg;
} error_info_t;

static const error_info_t ERRORS [] = {
	{ 400 , "درخواست نامعتبر" , "Bad request" , "درخواست شما قابل فهم نبود." , "The request could not be understood." } ,
	{ 404 , "صفحه پیدا نشد" , "Page not found" , "صفحه‌ای که دنبالش بودید وجود ندارد." , "The page you asked for does not exist." } ,
	{ 405 , "روش مجاز نیست" , "Method not allowed" , "این روش درخواست پشتیبانی نمی‌شود." , "That request method is not allowed here." } ,
	{ 408 , "پایان زمان" , "Request timeout" , "ارسال درخواست بیش از حد طول کشید." , "The request took too long to arrive." } ,
	{ 411 , "طول مشخص نیست" , "Length required" , "هدر Content-Length الزامی است." , "A Content-Length header is required." } ,
	{ 413 , "حجم بیش از حد" , "Payload too large" , "حجم درخواست بیش از حد مجاز است." , "The request is larger than allowed." } ,
	{ 431 , "سرآیند بزرگ" , "Headers too large" , "سرآیندهای درخواست بیش از حد بزرگ‌اند." , "The request headers are too large." } ,
	{ 500 , "خطای سرور" , "Server error" , "مشکلی در سرور رخ داد." , "Something went wrong on the server." } ,
	{ 501 , "پشتیبانی نمی‌شود" , "Not implemented" , "این نوع درخواست پشتیبانی نمی‌شود." , "This kind of request is not supported." } ,
	{ 503 , "سرور شلوغ است" , "Service unavailable" , "سرور شلوغ است؛ کمی بعد دوباره تلاش کنید." , "The server is busy. Please try again shortly." } ,
};

static void emit_bilingual (strbuf_t * sb , const char * fa , const char * en)
{
	APPEND (sb , "<span class='fa'>");
	sb_append_str (sb , fa);
	APPEND (sb , "</span><span class='en'>");
	sb_append_str (sb , en);
	APPEND (sb , "</span>");
}

int page_error_render (strbuf_t * sb , int status)
{
	const error_info_t * info = NULL;
	size_t i;
	char num [4];
	for (i = 0; i < sizeof ERRORS / sizeof ERRORS [0]; i++)
	{
		if (ERRORS [i].status == status)
			info = &ERRORS [i];
		else if (info == NULL && ERRORS [i].status == 500)
			info = &ERRORS [i];
	}
	num [0] = (char)('0' + (info -> status / 100) % 10);
	num [1] = (char)('0' + (info -> status / 10) % 10);
	num [2] = (char)('0' + info -> status % 10);
	num [3] = '\0';
	page_begin (sb , "Error");
	APPEND (sb , ERR_A);
	page_emit_top (sb , PAGE_MARK_KEYHOLE);
	APPEND (sb , ERR_B);
	sb_append_str (sb , num);
	APPEND (sb , ERR_C);
	emit_bilingual (sb , info -> fa_title , info -> en_title);
	APPEND (sb , ERR_D);
	emit_bilingual (sb , info -> fa_msg , info -> en_msg);
	APPEND (sb , ERR_E);
	return page_end (sb , NULL);
}
