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
#include "page_css.h"

const char PAGE_CSS [] =

":root{color-scheme:light;"
"--paper:#f4f6fb;--surface:#fff;--ink:#111b3d;--muted:#566083;--line:#d6dcee;"
"--field:#fff;--brand:#1d3a99;--brand-hover:#162e7d;--on-brand:#fff;"
"--ring:rgba(29,58,153,.25);--err:#a31d0d;--err-bg:#fdf0ee;--err-line:#f1b4ab;"
"--ok:#0f7a54;--art-a:#0c1744;--art-b:#1b3d9b;--art-line:#4fd8c8;--art-key:#f6b93b;"
"--art-text:#eaf0ff}\n"

"@media(prefers-color-scheme:dark){:root{color-scheme:dark;"
"--paper:#080d1f;--surface:#0e1530;--ink:#e8edfb;--muted:#9aa6cb;--line:#25305a;"
"--field:#0a1129;--brand:#6c8cff;--brand-hover:#8aa4ff;--on-brand:#0a1230;"
"--ring:rgba(108,140,255,.35);--err:#ffb3a7;--err-bg:#3a1713;--err-line:#7a3128;"
"--ok:#46d6a0;--art-a:#060b1f;--art-b:#13287a;}}\n"

"*{box-sizing:border-box}\n"
"html,body{margin:0;min-height:100%}\n"
"body{min-height:100vh;min-height:100dvh;background:var(--paper);color:var(--ink);"
"font-family:'Vazirmatn','Estedad','Noto Sans Arabic','Segoe UI',Tahoma,system-ui,-apple-system,sans-serif;"
"font-size:16px;line-height:1.6;-webkit-font-smoothing:antialiased}\n"
".en{display:none}\n"
"html[lang=en] .fa{display:none}\n"
"html[lang=en] .en{display:inline}\n"
":focus-visible{outline:3px solid var(--ring);outline-offset:2px}\n"

".stage{display:grid;grid-template-columns:minmax(0,1.05fr) minmax(0,1fr);min-height:100vh;min-height:100dvh}\n"
".art{position:relative;overflow:hidden;display:flex;flex-direction:column;justify-content:space-between;"
"padding:clamp(28px,5vw,64px);color:var(--art-text);"
"background:radial-gradient(120% 90% at 20% 10%,var(--art-b),var(--art-a) 70%)}\n"
".art::before{content:'';position:absolute;inset:0;opacity:.5;pointer-events:none;"
"background-image:url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='96' height='96' fill='none' stroke='%234fd8c8' stroke-opacity='.18' stroke-width='1'%3E%3Crect x='24' y='24' width='48' height='48'/%3E%3Crect x='24' y='24' width='48' height='48' transform='rotate(45 48 48)'/%3E%3C/svg%3E\");"
"-webkit-mask-image:linear-gradient(160deg,#000,transparent 75%);mask-image:linear-gradient(160deg,#000,transparent 75%)}\n"
".art>*{position:relative}\n"
".brand{margin:0;font-size:1.05rem;font-weight:700;letter-spacing:.02em;unicode-bidi:isolate;text-align:start;opacity:.9}\n"
".emblem{align-self:center;width:min(66%,360px);aspect-ratio:1;margin:24px 0}\n"
".emblem svg{display:block;width:100%;height:100%;overflow:visible}\n"
".emblem *{fill:none;stroke-linecap:round;stroke-linejoin:round}\n"
".emblem .star{stroke:var(--art-line);stroke-width:1.6}\n"
".emblem .ring{stroke:var(--art-text);stroke-opacity:.55;stroke-width:1}\n"
".emblem .key{stroke:var(--art-key);stroke-width:4}\n"
".emblem .key-dot{fill:var(--art-key);stroke:none}\n"
".emblem .spin{transform-origin:100px 100px;animation:turn 90s linear infinite}\n"
".emblem .spin2{animation-direction:reverse;animation-duration:140s}\n"
".emblem .d{stroke-dasharray:1;stroke-dashoffset:0;animation:draw 1.6s cubic-bezier(.65,0,.35,1) both}\n"
".emblem .d2{animation-delay:.2s}\n"
".emblem .d3{animation-delay:.6s}\n"
"html.busy .emblem .spin{animation-duration:2.4s}\n"
"html.busy .emblem .spin2{animation-duration:3.6s}\n"
".art-text h2{margin:0 0 8px;font-size:clamp(1.6rem,3vw,2.4rem);line-height:1.25;font-weight:800;text-align:start}\n"
".art-text p{margin:0;max-width:34ch;color:var(--art-text);opacity:.8;text-align:start}\n"

".panel{display:flex;flex-direction:column;padding:clamp(20px,4vw,40px);"
"padding-block-start:max(20px,env(safe-area-inset-top));padding-block-end:max(20px,env(safe-area-inset-bottom))}\n"
".bar{display:flex;justify-content:flex-end}\n"
".box{width:100%;max-width:400px;margin:auto;padding:24px 0}\n"
".box.err{animation:shake .5s}\n"
".lang{-webkit-appearance:none;appearance:none;border:1px solid var(--line);background:var(--surface);color:var(--muted);"
"border-radius:999px;height:36px;padding:0 16px;font:inherit;font-size:.86rem;cursor:pointer;transition:border-color .15s,color .15s}\n"
".lang:hover{border-color:var(--brand);color:var(--brand)}\n"

"h1{margin:0 0 6px;font-size:2.1rem;line-height:1.25;font-weight:800;letter-spacing:-.01em;text-align:start}\n"
".sub{margin:0 0 28px;color:var(--muted);text-align:start}\n"
".code{margin:0 0 4px;font-size:5rem;line-height:1;font-weight:800;color:var(--brand);font-variant-numeric:tabular-nums;unicode-bidi:isolate;text-align:start}\n"
".badge{display:inline-block;padding:0 10px;border:1px solid var(--line);border-radius:6px;background:var(--surface);"
"font-weight:700;color:var(--ink);direction:ltr;unicode-bidi:isolate}\n"

".lbl{display:block;margin:0 0 6px;font-size:.9rem;font-weight:600}\n"
".field{position:relative;margin-bottom:18px}\n"
".field .ico{position:absolute;inset-inline-start:14px;top:50%;width:20px;height:20px;margin-top:-10px;"
"fill:none;stroke:var(--muted);stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round;pointer-events:none;transition:stroke .15s}\n"
".field:focus-within .ico{stroke:var(--brand)}\n"
".field input{width:100%;height:54px;padding:0 14px;padding-inline-start:46px;border:1.5px solid var(--line);border-radius:12px;"
"background:var(--field);color:var(--ink);font:inherit;font-size:16px;outline:none;transition:border-color .15s,box-shadow .15s}\n"
".field.pw input{padding-inline-end:54px}\n"
".field input:focus{border-color:var(--brand);box-shadow:0 0 0 4px var(--ring)}\n"
".field input:-webkit-autofill{-webkit-text-fill-color:var(--ink);box-shadow:0 0 0 100px var(--field) inset}\n"
".eye{position:absolute;inset-inline-end:7px;top:7px;width:40px;height:40px;display:grid;place-items:center;padding:0;border:0;"
"border-radius:9px;background:transparent;color:var(--muted);cursor:pointer}\n"
".eye[hidden]{display:none}\n"
".eye:hover{color:var(--brand)}\n"
".eye svg{width:22px;height:22px;fill:none;stroke:currentColor;stroke-width:1.8;stroke-linecap:round;stroke-linejoin:round}\n"
".eye .hide{display:none}\n"
".eye.on .hide{display:block}\n"
".eye.on .show{display:none}\n"
".btn{display:flex;align-items:center;justify-content:center;gap:10px;width:100%;height:54px;margin-top:10px;border:0;border-radius:12px;"
"background:var(--brand);color:var(--on-brand);font:inherit;font-size:1.05rem;font-weight:700;text-decoration:none;cursor:pointer;"
"transition:background .15s,transform .1s}\n"
".btn:hover{background:var(--brand-hover)}\n"
".btn:active{transform:translateY(1px)}\n"
".btn:disabled{opacity:.75;cursor:progress}\n"
".spin-dot{display:none;width:18px;height:18px;border:2.5px solid currentColor;border-top-color:transparent;border-radius:50%;animation:turn .7s linear infinite}\n"
".busy .spin-dot{display:inline-block}\n"
".alert{display:flex;gap:10px;align-items:flex-start;margin:0 0 20px;padding:12px 14px;background:var(--err-bg);color:var(--err);"
"border:1px solid var(--err-line);border-inline-start:4px solid var(--err);border-radius:10px;font-size:.94rem;font-weight:600;line-height:1.55}\n"
".alert svg{flex:none;width:20px;height:20px;margin-top:2px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round}\n"
".ok-mark{color:var(--ok)}\n"

"@media(max-width:860px){"
".stage{grid-template-columns:1fr;grid-template-rows:auto 1fr}"
".art{flex-direction:row;align-items:center;justify-content:flex-start;gap:18px;padding:18px 20px}"
".emblem{order:-1;width:64px;margin:0;align-self:center;flex:none}"
".art-text{display:none}"
".brand{font-size:1.1rem}"
".box{padding:12px 0 24px}"
"h1{font-size:1.8rem}"
".code{font-size:4rem}}\n"

"@keyframes draw{from{stroke-dashoffset:1}to{stroke-dashoffset:0}}\n"
"@keyframes turn{to{transform:rotate(360deg)}}\n"
"@keyframes shake{0%,100%{transform:none}20%{transform:translateX(-9px)}40%{transform:translateX(8px)}60%{transform:translateX(-5px)}80%{transform:translateX(3px)}}\n"
"@media(prefers-reduced-motion:reduce){*,*::before,*::after{animation:none!important;transition:none!important}}\n";
