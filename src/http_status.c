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
#include "http_status.h"

const char * http_status_text (int status)
{
	switch (status)
	{
		case 200: return "OK";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 411: return "Length Required";
		case 413: return "Payload Too Large";
		case 431: return "Request Header Fields Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 503: return "Service Unavailable";
		default: return "Unknown";
	}
}

int http_rr_status (http_rr_t rr)
{
	switch (rr)
	{
		case HTTP_RR_TIMEOUT: return 408;
		case HTTP_RR_BAD_REQUEST: return 400;
		case HTTP_RR_LENGTH_REQUIRED: return 411;
		case HTTP_RR_BODY_TOO_LARGE: return 413;
		case HTTP_RR_HEADERS_TOO_LARGE: return 431;
		case HTTP_RR_NOT_IMPLEMENTED: return 501;
		default: return 0;
	}
}
