# loginwebpage
[Click here to view the documentation in English](README.md)

<div dir="rtl" align="right">

سرور سبک و چندریسمانی صفحه ورود تحت وب که با زبان C نوشته شده است.

`loginwebpage` یک وب‌سرور کوچک است که صفحه ورود را نمایش می‌دهد و اطلاعات ارسال‌شده را با پایگاه داده محلی کاربران بررسی می‌کند. این پروژه برای توسعه، نمایش عملکرد، و آزمایش کنترل‌شده فرایند ورود و رفتار کلاینت‌های وب مناسب است.

## قابلیت‌ها

- صفحه‌های ورود، موفقیت و خطا به دو زبان فارسی و انگلیسی، با تم روشن و تیره.
- بارگذاری حساب‌ها از یک فایل متنی محلی و بررسی رمز با هش SHA-256.
- چندریسمانی: مجموعه‌ای از threadهای کارگر با صف محدود (در فشار زیاد پاسخ `503` می‌دهد و هرگز گیر نمی‌کند).
- خروج امن با `Ctrl+C` یا `SIGTERM`: پذیرش اتصال جدید متوقف می‌شود، درخواست‌های در حال انجام تا ۲ ثانیه فرصت پایان دارند، همه سوکت‌ها بسته، همه threadها join و همه حافظه آزاد می‌شود. `Ctrl+C` دوم فوراً خارج می‌شود.
- اگر فایل کاربران ایراد داشته باشد، روی `stderr` و به سبک `gcc` گزارش می‌شود (فایل:خط:ستون، خط معیوب، نشانگر `^~~~` و یک نمونه خط درست).
- سوییچ‌های `--add-user` و `--remove-user` برای مدیریت فایل کاربران؛ دیگر اسکریپت کمکی لازم نیست.
- `--help` راهنمای کامل را با قالب استاندارد man page داخل `less` نشان می‌دهد.
- `./configure` پیش‌نیازها را بررسی می‌کند، دستور نصب موارد ناقص را می‌گوید و `Makefile` را می‌نویسد.

## کاربرد موردنظر

این پروژه برای آزمایش فرم‌های ورود، فرایندهای احراز هویت، رفتار مرورگر و یکپارچه‌سازی کلاینت‌ها در محیط کنترل‌شده مناسب است. از حساب‌های آزمایشی و رمزهای غیرحساس استفاده کنید.

**نکته امنیتی:** این سرور از HTTP بدون رمزنگاری استفاده می‌کند و مقادیر بررسی رمز عبور را به‌صورت هش SHA-256 بدون salt نگه می‌دارد. بنابراین برای احراز هویت در محیط عملیاتی یا نگهداری اطلاعات واقعی کاربران طراحی نشده است. سرور به‌طور پیش‌فرض روی تمام رابط‌های شبکه گوش می‌دهد؛ دسترسی را با فایروال محدود کنید یا آن را فقط در شبکه آزمایشی مورداعتماد اجرا کنید. سرور را مستقیماً در معرض اینترنت عمومی قرار ندهید.

## پیش‌نیازها

- کامپایلر C مانند GCC یا Clang.
- ابزار `make` (GNU make یا make سیستم‌های BSD)، `sed` و یک پوسته POSIX برای `./configure`.
- سرآیندهای استاندارد POSIX و پشتیبانی از thread.
- اختیاری: `less` برای نمایش صفحه‌به‌صفحه `--help`.

سیستم‌های پشتیبانی‌شده: Linux، FreeBSD، OpenBSD، NetBSD، DragonFly BSD و Termux (اندروید). اگر چیزی نصب نباشد، `./configure` دستور دقیق نصب را بر اساس package manager سیستم شما چاپ می‌کند، مثلاً:

</div>

<div dir="ltr" align="left">

```sh
sudo apt install build-essential        # Debian/Ubuntu
sudo dnf install gcc make glibc-devel   # Fedora
sudo pacman -S --needed base-devel      # Arch
pkg install clang make                  # Termux
```

</div>

## کلون کردن مخزن

<div dir="ltr" align="left">

```bash
git clone https://github.com/mehmetlotfi/loginwebpage.git

cd loginwebpage/
```
</div>

<div dir="rtl" align="right">

## کامپایل

</div>

<div dir="ltr" align="left">

```sh
./configure
make
```

</div>

<div dir="rtl" align="right">

`./configure` فقط **وقتی همه بررسی‌ها موفق باشند** فایل `Makefile` را می‌سازد؛ اگر مورد ناقصی باشد روش نصب آن را توضیح می‌دهد و هیچ فایلی نمی‌سازد. متن Makefile داخل خود اسکریپت `configure` نگه‌داری می‌شود (فایل `Makefile.in` وجود ندارد).

- فایل‌های object در پوشه `build/` ساخته می‌شوند و هرگز `src/` و `include/` را آلوده نمی‌کنند.
- پس از تغییر یک فایل، `make` فقط بخش‌های وابسته به آن را دوباره کامپایل می‌کند.
- `make clean` پوشه `build/` و فایل اجرایی `loginwebpage` را حذف می‌کند؛ `make distclean` علاوه بر آن `Makefile` را هم حذف می‌کند.
- اگر فایل `.c` اضافه کردید یا خط `#include` را تغییر دادید، `./configure` را دوباره اجرا کنید تا قواعد وابستگی بازسازی شوند.
- `sudo make install` برنامه و man page را در `/usr/local` نصب می‌کند (با `./configure --prefix=/usr` قابل تغییر است)؛ `sudo make uninstall` آن‌ها را حذف می‌کند.

## مدیریت کاربران

فایل کاربران در هر خط یک حساب دارد:

</div>

<div dir="ltr" align="left">

```text
username:sha256-hex-of-password;
```

</div>

<div dir="rtl" align="right">

فایل را با خود برنامه بسازید و ویرایش کنید (رمز دو بار پرسیده می‌شود و نمایش داده نمی‌شود):

</div>

<div dir="ltr" align="left">

```sh
./loginwebpage --add-user Ali users.db      # فرم کوتاه: -a Ali users.db
./loginwebpage --remove-user Ali users.db   # فرم کوتاه: -r Ali users.db
```

</div>

<div dir="rtl" align="right">

اگر فایل وجود نداشته باشد ساخته می‌شود (با دسترسی `0600`). اگر فایل ایراد داشته باشد این دستورها به آن دست نمی‌زنند. برای اسکریپت‌ها، وقتی ورودی استاندارد ترمینال نباشد رمز از آن خوانده می‌شود:

</div>

<div dir="ltr" align="left">

```sh
printf '%s\n' 'test-password' | ./loginwebpage -a testuser users.db
```

</div>

<div dir="rtl" align="right">

بعد از تغییر فایل، سرور را دوباره اجرا کنید. فایل نمونه `users.db.example` همراه پروژه است (کاربر `Ali` با رمز `1234`).

### ایراد در فایل کاربران

هنگام اجرا، کل فایل بررسی می‌شود و هر ایرادی تفسیر و گزارش می‌شود؛ سپس سرور اجرا نمی‌شود:

</div>

<div dir="ltr" align="left">

```text
users.db:2:5: error: password hash has 6 characters, expected 64 hex digits (SHA-256)
    2 | Bob:03ac67;
      |     ^~~~~~
users.db:2:1: note: a valid line looks like this (the hash is only an example):
    2 | Bob:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4;
users.db:3:70: error: missing ';' at the end of the line
    3 | Dave:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4
      |                                                                      ^
users.db:3:1: note: suggested fix for this line:
    3 | Dave:03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4;
```

</div>

<div dir="rtl" align="right">

«suggested fix» نام و هش خود شما را نگه می‌دارد و فقط قالب را درست می‌کند. اگر خود هش خراب باشد، یادداشت یک خط درست با هش «نمونه» نشان می‌دهد.

## اجرای سرور

</div>

<div dir="ltr" align="left">

```sh
./loginwebpage users.db
```

</div>

<div dir="rtl" align="right">

به‌طور پیش‌فرض سرور روی پورت TCP شماره `8080` و روی همه رابط‌های شبکه گوش می‌دهد. روی همان دستگاه `http://127.0.0.1:8080/` و از دستگاه دیگر در شبکه مورداعتمادتان `http://<SERVER_LAN_IP>:8080/` را باز کنید. برنامه HTTPS ارائه نمی‌دهد.

سرور را با `Ctrl+C` متوقف کنید.

## راهنما

</div>

<div dir="ltr" align="left">

```sh
./loginwebpage --help     # یا -h : راهنمای کامل (man page) داخل less
./loginwebpage --version  # یا -V
```

</div>

<div dir="rtl" align="right">

اگر خروجی استاندارد ترمینال نباشد (مثلاً `./loginwebpage --help | grep port`) راهنما به‌صورت متن ساده چاپ می‌شود.

## تغییر پورت شنود

پورت پیش‌فرض با ماکروی `SERVER_PORT` در `include/config.h` مشخص شده است. آن را تغییر دهید و دوباره کامپایل کنید یا هنگام کامپایل تعریف دیگری بدهید:

</div>

<div dir="ltr" align="left">

```sh
make clean
make CPPFLAGS=-DSERVER_PORT=8081
```

</div>

<div dir="rtl" align="right">

پورت‌های کمتر از ۱۰۰۰ به root نیاز دارند: سرور را با `sudo` اجرا کنید؛ به‌محض باز شدن سوکت‌ها دسترسی root را رها می‌کند.

## ساختار مخزن

</div>

<div dir="ltr" align="left">

```text
loginwebpage/
├── include/          C header files (one per module)
├── src/              C source files (one job per file; manpage.c holds the manual)
├── users.db.example  Example user database
├── configure         Requirement check + Makefile generator
└── LICENSE           License text
```

</div>

<div dir="rtl" align="right">

فایل‌هایی که `./configure` و `make` می‌سازند (با `make distclean` پاک می‌شوند): `Makefile`، `build/` و `loginwebpage`.

## عیب‌یابی

- **پورت قبلاً در حال استفاده است:** مقدار `SERVER_PORT` را تغییر دهید و دوباره کامپایل کنید.
- **`./configure` ناموفق است:** دستور نصبی را که چاپ می‌کند اجرا کنید و دوباره آن را اجرا کنید.
- **فایل کاربران پذیرفته نمی‌شود:** پیام‌های `file:line:column` روی `stderr` را بخوانید؛ هر کدام خط معیوب و شکل درست آن را نشان می‌دهد.
- **اتصال از دستگاه دیگری برقرار نمی‌شود:** روشن بودن سرور، IP محلی و پورت صحیح و قوانین فایروال را بررسی کنید.

## مجوز

این پروژه تحت مجوز GNU General Public License نسخه ۳ یا، بنا به انتخاب شما، هر نسخه جدیدتر منتشرشده از آن توزیع می‌شود. متن مجوز در فایل `LICENSE` قرار دارد.

---

## حمایت از پروژه

اگر این مطالب برایتان مفید بود، خوشحال می‌شویم که از پروژه حمایت کنید. حمایت شما می‌تواند به‌صورت **مشارکت در نوشتن راهنماهای جدید، گزارش اشکالات، پیشنهاد بهبود، یا حتی معرفی این مخزن به دیگران** باشد. همچنین در صورت تمایل به دونیت، می‌توانید از طریق [این لینک](https://github.com/mehmetlotfi) اقدام فرمایید. هرگونه حمایت، هرچند کوچک، به ما انگیزه می‌دهد تا این مسیر را با کیفیت بیشتری ادامه دهم.

از همراهی شما سپاسگزاریم.

</div>

