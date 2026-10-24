<div dir="rtl" align="right">

# loginwebpage

سرور سبک و چندریسمانی صفحه ورود تحت وب که با زبان C نوشته شده است.

`loginwebpage` یک وب‌سرور کوچک است که صفحه ورود را نمایش می‌دهد و اطلاعات ارسال‌شده را با پایگاه داده محلی کاربران بررسی می‌کند. این پروژه برای توسعه، نمایش عملکرد، و آزمایش کنترل‌شده فرایند ورود و رفتار کلاینت‌های وب مناسب است.

## قابلیت‌ها

- نمایش صفحه ورود و پاسخ‌های موفقیت یا خطا.
- بارگذاری حساب‌های کاربری از یک فایل متنی محلی.
- بررسی رمز عبور با استفاده از هش SHA-256.
- پردازش چندریسمانی اتصال‌ها.
- تجزیه درخواست‌های HTTP و اعتبارسنجی ورودی‌ها.
- امکان تغییر پورت شنود هنگام کامپایل؛ پورت پیش‌فرض `8080` است.
- اسکریپت `configure` برای بررسی پیش‌نیازهای ساخت.
- هدف‌های `Makefile` برای کامپایل، نصب، حذف نصب و پاک‌سازی فایل‌های تولیدشده.

## کاربرد موردنظر

این پروژه برای آزمایش فرم‌های ورود، فرایندهای احراز هویت، رفتار مرورگر و یکپارچه‌سازی کلاینت‌ها در محیط کنترل‌شده مناسب است. از حساب‌های آزمایشی و رمزهای غیرحساس استفاده کنید.

**نکته امنیتی:** این سرور از HTTP بدون رمزنگاری استفاده می‌کند و مقادیر بررسی رمز عبور را به‌صورت هش SHA-256 بدون salt نگه می‌دارد. بنابراین برای احراز هویت در محیط عملیاتی یا نگهداری اطلاعات واقعی کاربران طراحی نشده است. سرور به‌طور پیش‌فرض روی تمام رابط‌های شبکه گوش می‌دهد؛ دسترسی را با فایروال محدود کنید یا آن را فقط در شبکه آزمایشی مورداعتماد اجرا کنید. سرور را مستقیماً در معرض اینترنت عمومی قرار ندهید.

## پیش‌نیازها

- کامپایلر C مانند GCC یا Clang.
- ابزار `make`؛ مانند GNU make یا نسخه سازگار BSD.
- سرآیندهای استاندارد سیستم‌های سازگار با POSIX و پشتیبانی از threadها.
- پوسته POSIX برای اجرای اسکریپت‌های `configure` و ساخت کاربر.

این پروژه برای Linux و خانواده سیستم‌عامل‌های BSD طراحی شده است. در Debian و Ubuntu می‌توانید ابزارهای لازم را با دستورهای زیر نصب کنید:

</div>

<div dir="ltr" align="left">

```sh
sudo apt update
sudo apt install build-essential
```

</div>

<div dir="rtl" align="right">

در Fedora:

</div>

<div dir="ltr" align="left">

```sh
sudo dnf install gcc make glibc-devel
```

</div>

<div dir="rtl" align="right">

در Arch Linux:

</div>

<div dir="ltr" align="left">

```sh
sudo pacman -S --needed base-devel
```

</div>

<div dir="rtl" align="right">

## دریافت پروژه

</div>

<div dir="ltr" align="left">

```sh
git clone https://github.com/mehmetlotfi/loginwebpage.git
cd loginwebpage
```

</div>

<div dir="rtl" align="right">

## بررسی پیش‌نیازها و کامپایل

ابتدا پیش‌نیازهای ساخت را بررسی کنید:

</div>

<div dir="ltr" align="left">

```sh
./configure
```

</div>

<div dir="rtl" align="right">

اگر بررسی با موفقیت انجام شد، برنامه را کامپایل کنید:

</div>

<div dir="ltr" align="left">

```sh
make
```

</div>

<div dir="rtl" align="right">

فایل اجرایی با نام `loginwebpage` ساخته می‌شود و فایل‌های میانی کامپایل در پوشه `build/` قرار می‌گیرند.

برای حذف فایل اجرایی و فایل‌های تولیدشده هنگام کامپایل:

</div>

<div dir="ltr" align="left">

```sh
make clean
```

</div>

<div dir="rtl" align="right">

برای نصب فایل اجرایی در مسیر `/usr/local/bin`:

</div>

<div dir="ltr" align="left">

```sh
sudo make install
```

</div>

<div dir="rtl" align="right">

برای حذف نصب:

</div>

<div dir="ltr" align="left">

```sh
sudo make uninstall
```

</div>

<div dir="rtl" align="right">

در صورت نیاز می‌توانید مسیر نصب را تغییر دهید؛ برای نمونه:

</div>

<div dir="ltr" align="left">

```sh
sudo make install PREFIX=/usr
```

</div>

<div dir="rtl" align="right">

## ساخت پایگاه داده کاربران آزمایشی

سرور فایلی می‌پذیرد که در هر خط آن یک کاربر با قالب زیر تعریف شده باشد:

</div>

<div dir="ltr" align="left">

```text
username:sha256-hex-of-password;
```

</div>

<div dir="rtl" align="right">

هش رمز عبور باید دقیقاً ۶۴ رقم هگزادسیمال داشته باشد. یک فایل نمونه با نام `users.db.example` در پروژه قرار دارد. آن را به فایل کاری جداگانه‌ای کپی کنید:

</div>

<div dir="ltr" align="left">

```sh
cp users.db.example users.db
```

</div>

<div dir="rtl" align="right">

برای افزودن یک کاربر آزمایشی، از اسکریپت کمکی استفاده کنید. اسکریپت رمز عبور را بدون نمایش روی صفحه دریافت می‌کند و رکورد کاربر را به فایل اضافه می‌کند:

</div>

<div dir="ltr" align="left">

```sh
sh tools/mkuser.sh testuser >> users.db
```

</div>

<div dir="rtl" align="right">

امکان افزودن کاربر با وارد کردن رمز عبور به‌عنوان آرگومان هم وجود دارد، اما این روش توصیه نمی‌شود؛ زیرا ممکن است رمز در تاریخچه پوسته یا فهرست فرایندها قابل مشاهده باشد:

</div>

<div dir="ltr" align="left">

```sh
sh tools/mkuser.sh testuser 'test-password' >> users.db
```

</div>

<div dir="rtl" align="right">

**توجه:** پایگاه داده کاربران شخصی یا حساس را در مخزن Git قرار ندهید. فایل `users.db` را محلی نگه دارید و فقط از اطلاعات آزمایشی موقت استفاده کنید. فایل `users.db.example` صرفاً نمونه است و نباید برای حساب‌های حساس استفاده شود.

## اجرای سرور

از داخل پوشه پروژه، مسیر فایل پایگاه داده کاربران را به‌عنوان تنها آرگومان ضروری برنامه وارد کنید:

</div>

<div dir="ltr" align="left">

```sh
./loginwebpage users.db
```

</div>

<div dir="rtl" align="right">

سرور به‌طور پیش‌فرض روی پورت TCP شماره `8080` و روی تمام رابط‌های شبکه گوش می‌دهد. برای باز کردن صفحه روی همان دستگاه، نشانی زیر را در مرورگر وارد کنید:

</div>

<div dir="ltr" align="left">

```text
http://127.0.0.1:8080/
```

</div>

<div dir="rtl" align="right">

برای آزمایش از دستگاه دیگری در شبکه محلی مورداعتماد، به‌جای `<SERVER_LAN_IP>` آدرس IP محلی دستگاهی را قرار دهید که سرور روی آن اجرا شده است:

</div>

<div dir="ltr" align="left">

```text
http://<SERVER_LAN_IP>:8080/
```

</div>

<div dir="rtl" align="right">

این کار را فقط در شبکه‌ای انجام دهید که دسترسی به آن را کنترل می‌کنید؛ ارتباط برنامه HTTPS نیست.

برای نمایش راهنمای خط فرمان:

</div>

<div dir="ltr" align="left">

```sh
./loginwebpage --help
```

</div>

<div dir="rtl" align="right">

برای متوقف کردن سرور، در ترمینال کلیدهای `Ctrl+C` را فشار دهید.

## تغییر پورت شنود

پورت پیش‌فرض با ماکروی `SERVER_PORT` در فایل `include/config.h` مشخص شده است. می‌توانید مقدار آن را در فایل تغییر دهید و دوباره کامپایل کنید یا هنگام کامپایل تعریف دیگری به کامپایلر بدهید:

</div>

<div dir="ltr" align="left">

```sh
make clean
make CPPFLAGS=-DSERVER_PORT=8081
```

</div>

<div dir="rtl" align="right">

پس از آن صفحه در نشانی زیر در دسترس خواهد بود:

</div>

<div dir="ltr" align="left">

```text
http://127.0.0.1:8081/
```

</div>

<div dir="rtl" align="right">

## ساختار مخزن

</div>

<div dir="ltr" align="left">

```text
loginwebpage/
├── include/          C header files
├── src/              C source files
├── tools/mkuser.sh   Helper to create user database records
├── users.db.example  Example user database
├── configure         Build-requirements check
├── Makefile          Build and installation targets
└── LICENSE           License text
```

</div>

<div dir="rtl" align="right">

## عیب‌یابی

- **پورت قبلاً در حال استفاده است:** مقدار `SERVER_PORT` را تغییر دهید و پروژه را دوباره کامپایل کنید.
- **بررسی پیش‌نیازها ناموفق است:** کامپایلر، `make` یا سرآیندهای توسعه‌ای را که `./configure` گزارش می‌کند نصب کنید و بررسی را دوباره اجرا کنید.
- **فایل کاربران پذیرفته نمی‌شود:** مطمئن شوید هر رکورد غیرخالی قالب `username:64-hex-character-SHA256-hash;` را رعایت می‌کند.
- **اتصال از دستگاه دیگری برقرار نمی‌شود:** روشن بودن سرور، IP محلی و پورت صحیح و قوانین فایروال را بررسی کنید. سرور به‌طور پیش‌فرض روی همه رابط‌های شبکه گوش می‌دهد.

## مجوز

این پروژه تحت مجوز GNU General Public License نسخه ۳ یا، بنا به انتخاب شما، هر نسخه جدیدتر منتشرشده از آن توزیع می‌شود. متن مجوز در فایل `LICENSE` قرار دارد.

---

## حمایت از پروژه

اگر این مطالب برایتان مفید بود، خوشحال می‌شویم که از پروژه حمایت کنید. حمایت شما می‌تواند به‌صورت **مشارکت در نوشتن راهنماهای جدید، گزارش اشکالات، پیشنهاد بهبود، یا حتی معرفی این مخزن به دیگران** باشد. همچنین در صورت تمایل به دونیت، می‌توانید از طریق [این لینک](https://github.com/mehmetlotfi) اقدام فرمایید. هرگونه حمایت، هرچند کوچک، به ما انگیزه می‌دهد تا این مسیر را با کیفیت بیشتری ادامه دهم.

از همراهی شما سپاسگزاریم.

</div>
