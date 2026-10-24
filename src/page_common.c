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

static const char HEAD_A [] =
"<!DOCTYPE html>\n"
"<html lang='fa' dir='rtl'>\n"
"<head>\n"
"<meta charset='utf-8'>\n"
"<meta name='viewport' content='width=device-width,initial-scale=1,viewport-fit=cover'>\n"
"<meta name='color-scheme' content='light dark'>\n"
"<meta name='robots' content='noindex,nofollow'>\n"
"<link rel='icon' href=\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64' fill='none' stroke='%230b6e78' stroke-width='5' stroke-linejoin='round'%3E%3Crect x='14' y='14' width='36' height='36'/%3E%3Crect x='14' y='14' width='36' height='36' transform='rotate(45 32 32)'/%3E%3C/svg%3E\">\n"
"<title>";

static const char HEAD_B [] =
"</title>\n"
"<style>";

static const char CSS_1 [] =
":root{color-scheme:light;--paper:#eef2f4;--card:#fff;--ink:#13203a;--muted:#59687a;--line:#cfd8df;--field:#fbfcfd;--brand:#0b6e78;--brandh:#095b64;--onbrand:#fff;--ring:rgba(11,110,120,.28);--err:#a8200f;--errbg:#fdf1ef;--errline:#f3b9b1;--ok:#15803d;--pat:url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='72' height='72' fill='none' stroke='%230b6e78' stroke-opacity='.16' stroke-width='1.2'%3E%3Crect x='16' y='16' width='40' height='40'/%3E%3Crect x='16' y='16' width='40' height='40' transform='rotate(45 36 36)'/%3E%3Ccircle cx='36' cy='36' r='6'/%3E%3C/svg%3E\")}\n"
"@media(prefers-color-scheme:dark){:root{color-scheme:dark;--paper:#0b1424;--card:#122036;--ink:#e7eef6;--muted:#9db0c4;--line:#2a3d57;--field:#0d1a2d;--brand:#35c3c0;--brandh:#5fd6d3;--onbrand:#06232a;--ring:rgba(53,195,192,.3);--err:#ffb4a8;--errbg:#3a1713;--errline:#7a3128;--ok:#4ade80;--pat:url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='72' height='72' fill='none' stroke='%2335c3c0' stroke-opacity='.14' stroke-width='1.2'%3E%3Crect x='16' y='16' width='40' height='40'/%3E%3Crect x='16' y='16' width='40' height='40' transform='rotate(45 36 36)'/%3E%3Ccircle cx='36' cy='36' r='6'/%3E%3C/svg%3E\")}}\n"
"*{box-sizing:border-box}\n"
"body{margin:0;min-height:100vh;min-height:100dvh;display:flex;align-items:center;justify-content:center;padding:max(20px,env(safe-area-inset-top)) max(16px,env(safe-area-inset-right)) max(20px,env(safe-area-inset-bottom)) max(16px,env(safe-area-inset-left));background:var(--paper);color:var(--ink);font-family:'Vazirmatn','Estedad','Noto Sans Arabic','Segoe UI',Tahoma,system-ui,-apple-system,sans-serif;font-size:16px;line-height:1.6;-webkit-text-size-adjust:100%}\n"
"body::before{content:'';position:fixed;inset:0;z-index:0;pointer-events:none;background:var(--pat);-webkit-mask-image:radial-gradient(ellipse at center,rgba(0,0,0,.28),#000 85%);mask-image:radial-gradient(ellipse at center,rgba(0,0,0,.28),#000 85%)}\n"
".en{display:none}\n"
"html[lang=en] .fa{display:none}\n"
"html[lang=en] .en{display:inline}\n"
".card{position:relative;z-index:1;width:100%;max-width:420px;background:var(--card);border:1px solid var(--line);border-radius:18px;padding:26px 28px 28px;box-shadow:0 1px 2px rgba(19,32,58,.06),0 28px 50px -26px rgba(19,32,58,.4)}\n"
".card.err{animation:shake .5s}\n"
".top{display:flex;align-items:center;justify-content:space-between;margin-bottom:20px}\n"
".mark{width:56px;height:56px;color:var(--brand)}\n"
".mark *{fill:none;stroke:currentColor;stroke-width:2.4;stroke-linecap:round;stroke-linejoin:round}\n"
".mark .d{animation:draw 1.1s cubic-bezier(.65,0,.35,1) both}\n"
".mark .d2{animation-delay:.15s}\n"
".mark .d3{animation-delay:.4s}\n"
".ok .mark{width:84px;height:84px;color:var(--ok)}\n"
".lang{-webkit-appearance:none;appearance:none;border:1px solid var(--line);background:transparent;color:var(--muted);border-radius:999px;height:34px;padding:0 14px;font:inherit;font-size:.85rem;cursor:pointer}\n"
".lang:hover{border-color:var(--brand);color:var(--brand)}\n"
"h1{margin:0 0 4px;font-size:2rem;line-height:1.3;font-weight:800}\n"
".sub{margin:0 0 22px;color:var(--muted);font-size:.96rem}\n"
".lbl{display:block;margin:0 0 6px;font-size:.88rem;font-weight:600}\n"
".field{position:relative;margin-bottom:16px}\n"
".field input{width:100%;height:52px;padding:0 14px;border:1.5px solid var(--line);border-radius:10px;background:var(--field);color:var(--ink);font:inherit;font-size:16px;outline:none;transition:border-color .15s,box-shadow .15s}\n"
".field.pw input{padding-inline-end:54px}\n"
".field input:focus{border-color:var(--brand);box-shadow:0 0 0 4px var(--ring)}\n"
".field input:-webkit-autofill{-webkit-text-fill-color:var(--ink);box-shadow:0 0 0 100px var(--field) inset}\n";

static const char CSS_2 [] =
".eye{position:absolute;inset-inline-end:6px;top:6px;width:40px;height:40px;display:grid;place-items:center;padding:0;border:0;border-radius:8px;background:transparent;color:var(--muted);cursor:pointer}\n"
".eye[hidden]{display:none}\n"
".eye:hover{color:var(--brand)}\n"
".eye svg{width:22px;height:22px;fill:none;stroke:currentColor;stroke-width:1.8;stroke-linecap:round;stroke-linejoin:round}\n"
".eye .hide{display:none}\n"
".eye.on .hide{display:block}\n"
".eye.on .show{display:none}\n"
".btn{display:flex;align-items:center;justify-content:center;gap:10px;width:100%;height:52px;margin-top:8px;border:0;border-radius:10px;background:var(--brand);color:var(--onbrand);font:inherit;font-size:1.05rem;font-weight:700;text-decoration:none;cursor:pointer;transition:background .15s,transform .1s}\n"
".btn:hover{background:var(--brandh)}\n"
".btn:active{transform:translateY(1px)}\n"
".btn:disabled{opacity:.75;cursor:progress}\n"
".spin{display:none;width:18px;height:18px;border:2.5px solid currentColor;border-top-color:transparent;border-radius:50%;animation:spin .7s linear infinite}\n"
".busy .spin{display:inline-block}\n"
".alert{display:flex;gap:10px;align-items:flex-start;margin:0 0 18px;padding:12px 14px;background:var(--errbg);color:var(--err);border:1px solid var(--errline);border-inline-start:4px solid var(--err);border-radius:10px;font-size:.94rem;font-weight:600;line-height:1.55}\n"
".alert svg{flex:none;width:20px;height:20px;margin-top:2px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round}\n"
".badge{display:inline-block;padding:0 10px;border:1px solid var(--line);border-radius:6px;background:var(--paper);font-weight:700;color:var(--ink);direction:ltr;unicode-bidi:isolate}\n"
".code{margin:0 0 6px;font-size:4rem;line-height:1.1;font-weight:800;color:var(--brand);font-variant-numeric:tabular-nums;unicode-bidi:isolate}\n"
":focus-visible{outline:3px solid var(--ring);outline-offset:2px}\n"
".field input:focus-visible{outline:none}\n"
"@media(max-width:440px){.card{padding:22px 20px 24px}h1{font-size:1.75rem}}\n"
"@keyframes draw{from{stroke-dasharray:1;stroke-dashoffset:1}to{stroke-dasharray:1;stroke-dashoffset:0}}\n"
"@keyframes shake{0%,100%{transform:none}20%{transform:translateX(-9px)}40%{transform:translateX(8px)}60%{transform:translateX(-5px)}80%{transform:translateX(3px)}}\n"
"@keyframes spin{to{transform:rotate(360deg)}}\n"
"@media(prefers-reduced-motion:reduce){*,*::before,*::after{animation:none!important;transition:none!important}}\n";

static const char HEAD_C [] =
"</style>\n"
"</head>\n"
"<body>\n";

static const char TOP_A [] =
"<div class='top'><svg class='mark' viewBox='0 0 64 64' aria-hidden='true'><rect class='d' pathLength='1' x='14' y='14' width='36' height='36' rx='2'/><rect class='d d2' pathLength='1' x='14' y='14' width='36' height='36' rx='2' transform='rotate(45 32 32)'/>";

static const char KEYHOLE [] =
"<circle class='d d3' pathLength='1' cx='32' cy='29' r='4.5'/><path class='d d3' pathLength='1' d='M32 33.5V41'/>";

static const char CHECK [] =
"<path class='d d3' pathLength='1' d='M24 33l6 6 11-13'/>";

static const char TOP_B [] =
"</svg><button type='button' class='lang' id='lang' aria-label='تغییر زبان' data-fa='تغییر زبان' data-en='Change language'><span class='fa'>English</span><span class='en'>فارسی</span></button></div>\n";

static const char SCRIPT_OPEN [] =
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
	APPEND (sb , CSS_1);
	APPEND (sb , CSS_2);
	APPEND (sb , HEAD_C);
}

void page_emit_top (strbuf_t * sb , page_mark_t mark)
{
	APPEND (sb , TOP_A);
	if (mark == PAGE_MARK_CHECK)
		APPEND (sb , CHECK);
	else
		APPEND (sb , KEYHOLE);
	APPEND (sb , TOP_B);
}

int page_end (strbuf_t * sb , const char * page_js)
{
	APPEND (sb , SCRIPT_OPEN);
	APPEND (sb , JS_LANG);
	if (page_js)
		sb_append_str (sb , page_js);
	APPEND (sb , SCRIPT_CLOSE);
	return sb -> failed ? -1 : 0;
}
