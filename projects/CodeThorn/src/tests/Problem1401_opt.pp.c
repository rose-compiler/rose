int input;

typedef long unsigned int size_t;
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;
typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef int __daddr_t;
typedef int __key_t;
typedef int __clockid_t;
typedef void * __timer_t;
typedef long int __blksize_t;
typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;
typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;
typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;
typedef long int __fsword_t;
typedef long int __ssize_t;
typedef long int __syscall_slong_t;
typedef unsigned long int __syscall_ulong_t;
typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;
typedef long int __intptr_t;
typedef unsigned int __socklen_t;
struct _IO_FILE;

typedef struct _IO_FILE FILE;


typedef struct _IO_FILE __FILE;
typedef struct
{
  int __count;
  union
  {
    unsigned int __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
typedef __builtin_va_list __gnuc_va_list;
struct _IO_jump_t; struct _IO_FILE;
typedef void _IO_lock_t;
struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};
enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
struct _IO_FILE {
  int _flags;
  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;
  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _flags2;
  __off_t _old_offset;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;
  __off64_t _offset;
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;
  int _mode;
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};
typedef struct _IO_FILE _IO_FILE;
struct _IO_FILE_plus;
extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);
typedef __ssize_t __io_write_fn (void *__cookie, const char *__buf,
     size_t __n);
typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);
typedef int __io_close_fn (void *__cookie);
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_peekc_locked (_IO_FILE *__fp);
extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);
extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);
extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
typedef __gnuc_va_list va_list;
typedef __off_t off_t;
typedef __ssize_t ssize_t;

typedef _G_fpos_t fpos_t;

extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;

extern int remove (const char *__filename) __attribute__ ((__nothrow__ , __leaf__));
extern int rename (const char *__old, const char *__new) __attribute__ ((__nothrow__ , __leaf__));

extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) __attribute__ ((__nothrow__ , __leaf__));

extern FILE *tmpfile (void) ;
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;

extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;
extern char *tempnam (const char *__dir, const char *__pfx)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;

extern int fclose (FILE *__stream);
extern int fflush (FILE *__stream);

extern int fflush_unlocked (FILE *__stream);

extern FILE *fopen (const char *__restrict __filename,
      const char *__restrict __modes) ;
extern FILE *freopen (const char *__restrict __filename,
        const char *__restrict __modes,
        FILE *__restrict __stream) ;

extern FILE *fdopen (int __fd, const char *__modes) __attribute__ ((__nothrow__ , __leaf__)) ;
extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  __attribute__ ((__nothrow__ , __leaf__)) ;
extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__ , __leaf__)) ;

extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));
extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__ , __leaf__));

extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__ , __leaf__));
extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));

extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);
extern int printf (const char *__restrict __format, ...);
extern int sprintf (char *__restrict __s,
      const char *__restrict __format, ...) __attribute__ ((__nothrow__));
extern int vfprintf (FILE *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg);
extern int vprintf (const char *__restrict __format, __gnuc_va_list __arg);
extern int vsprintf (char *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));


extern int snprintf (char *__restrict __s, size_t __maxlen,
       const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));
extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

extern int vdprintf (int __fd, const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));

extern int fscanf (FILE *__restrict __stream,
     const char *__restrict __format, ...) ;
extern int scanf (const char *__restrict __format, ...) ;
extern int sscanf (const char *__restrict __s,
     const char *__restrict __format, ...) __attribute__ ((__nothrow__ , __leaf__));
extern int fscanf (FILE *__restrict __stream, const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf") ;
extern int scanf (const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf") ;
extern int sscanf (const char *__restrict __s, const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__ , __leaf__));


extern int vfscanf (FILE *__restrict __s, const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (const char *__restrict __s,
      const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
extern int vfscanf (FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (const char *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__format__ (__scanf__, 2, 0)));


extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);
extern int getchar (void);

extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
extern int fgetc_unlocked (FILE *__stream);

extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);
extern int putchar (int __c);

extern int fputc_unlocked (int __c, FILE *__stream);
extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);
extern int getw (FILE *__stream);
extern int putw (int __w, FILE *__stream);

extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;
extern char *gets (char *__s) __attribute__ ((__deprecated__));

extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;
extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;

extern int fputs (const char *__restrict __s, FILE *__restrict __stream);
extern int puts (const char *__s);
extern int ungetc (int __c, FILE *__stream);
extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite (const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);

extern int fseek (FILE *__stream, long int __off, int __whence);
extern long int ftell (FILE *__stream) ;
extern void rewind (FILE *__stream);

extern int fseeko (FILE *__stream, __off_t __off, int __whence);
extern __off_t ftello (FILE *__stream) ;

extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);
extern int fsetpos (FILE *__stream, const fpos_t *__pos);


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int feof (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int ferror (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;

extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;

extern void perror (const char *__s);

extern int sys_nerr;
extern const char *const sys_errlist[];
extern int fileno (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern FILE *popen (const char *__command, const char *__modes) ;
extern int pclose (FILE *__stream);
extern char *ctermid (char *__s) __attribute__ ((__nothrow__ , __leaf__));
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));


extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


typedef float float_t;
typedef double double_t;

extern double acos (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __acos (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double asin (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __asin (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double atan (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atan (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double atan2 (double __y, double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atan2 (double __y, double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double cos (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cos (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double sin (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sin (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double tan (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __tan (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double cosh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cosh (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double sinh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sinh (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double tanh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __tanh (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double acosh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __acosh (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double asinh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __asinh (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double atanh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atanh (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double exp (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __exp (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern double __frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));
extern double ldexp (double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern double __ldexp (double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));
extern double log (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double log10 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log10 (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern double __modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern double expm1 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __expm1 (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double log1p (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log1p (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double logb (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __logb (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double exp2 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __exp2 (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double log2 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log2 (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double pow (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __pow (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));
extern double sqrt (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sqrt (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double hypot (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __hypot (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));


extern double cbrt (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cbrt (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double ceil (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __ceil (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double fabs (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fabs (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double floor (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __floor (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double fmod (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __fmod (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));
extern int __isinf (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __finite (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int isinf (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int finite (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double drem (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __drem (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));
extern double significand (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __significand (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double copysign (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __copysign (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double nan (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nan (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int __isnan (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int isnan (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double j0 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __j0 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double j1 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __j1 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double jn (int, double) __attribute__ ((__nothrow__ , __leaf__)); extern double __jn (int, double) __attribute__ ((__nothrow__ , __leaf__));
extern double y0 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __y0 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double y1 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __y1 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double yn (int, double) __attribute__ ((__nothrow__ , __leaf__)); extern double __yn (int, double) __attribute__ ((__nothrow__ , __leaf__));

extern double erf (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __erf (double) __attribute__ ((__nothrow__ , __leaf__));
extern double erfc (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __erfc (double) __attribute__ ((__nothrow__ , __leaf__));
extern double lgamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __lgamma (double) __attribute__ ((__nothrow__ , __leaf__));


extern double tgamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __tgamma (double) __attribute__ ((__nothrow__ , __leaf__));

extern double gamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __gamma (double) __attribute__ ((__nothrow__ , __leaf__));
extern double lgamma_r (double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern double __lgamma_r (double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));

extern double rint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __rint (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double nextafter (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nextafter (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double nexttoward (double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nexttoward (double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double remainder (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __remainder (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));
extern double scalbn (double __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalbn (double __x, int __n) __attribute__ ((__nothrow__ , __leaf__));
extern int ilogb (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogb (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double scalbln (double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalbln (double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));
extern double nearbyint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __nearbyint (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double round (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __round (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double trunc (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __trunc (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double remquo (double __x, double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern double __remquo (double __x, double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));
extern long int lrint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrint (double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrint (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long int lround (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lround (double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llround (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llround (double __x) __attribute__ ((__nothrow__ , __leaf__));
extern double fdim (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __fdim (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));
extern double fmax (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fmax (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern double fmin (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fmin (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __fpclassify (double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern int __signbit (double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern double fma (double __x, double __y, double __z) __attribute__ ((__nothrow__ , __leaf__)); extern double __fma (double __x, double __y, double __z) __attribute__ ((__nothrow__ , __leaf__));

extern double scalb (double __x, double __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalb (double __x, double __n) __attribute__ ((__nothrow__ , __leaf__));

extern float acosf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __acosf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float asinf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __asinf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float atanf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atanf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float atan2f (float __y, float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atan2f (float __y, float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float cosf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __cosf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float sinf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sinf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float tanf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __tanf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float coshf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __coshf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float sinhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sinhf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float tanhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __tanhf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float acoshf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __acoshf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float asinhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __asinhf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float atanhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atanhf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float expf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __expf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float frexpf (float __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern float __frexpf (float __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));
extern float ldexpf (float __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern float __ldexpf (float __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));
extern float logf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __logf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float log10f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log10f (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float modff (float __x, float *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern float __modff (float __x, float *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern float expm1f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __expm1f (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float log1pf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log1pf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float logbf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __logbf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float exp2f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __exp2f (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float log2f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log2f (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float powf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __powf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));
extern float sqrtf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sqrtf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float hypotf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __hypotf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));


extern float cbrtf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __cbrtf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float ceilf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __ceilf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float fabsf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fabsf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float floorf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __floorf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float fmodf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __fmodf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));
extern int __isinff (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __finitef (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int isinff (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int finitef (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float dremf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __dremf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));
extern float significandf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __significandf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float copysignf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __copysignf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float nanf (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nanf (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int __isnanf (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int isnanf (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float j0f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __j0f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float j1f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __j1f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float jnf (int, float) __attribute__ ((__nothrow__ , __leaf__)); extern float __jnf (int, float) __attribute__ ((__nothrow__ , __leaf__));
extern float y0f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __y0f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float y1f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __y1f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float ynf (int, float) __attribute__ ((__nothrow__ , __leaf__)); extern float __ynf (int, float) __attribute__ ((__nothrow__ , __leaf__));

extern float erff (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __erff (float) __attribute__ ((__nothrow__ , __leaf__));
extern float erfcf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __erfcf (float) __attribute__ ((__nothrow__ , __leaf__));
extern float lgammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __lgammaf (float) __attribute__ ((__nothrow__ , __leaf__));


extern float tgammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __tgammaf (float) __attribute__ ((__nothrow__ , __leaf__));

extern float gammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __gammaf (float) __attribute__ ((__nothrow__ , __leaf__));
extern float lgammaf_r (float, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern float __lgammaf_r (float, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));

extern float rintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __rintf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float nextafterf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nextafterf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float nexttowardf (float __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nexttowardf (float __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float remainderf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __remainderf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));
extern float scalbnf (float __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalbnf (float __x, int __n) __attribute__ ((__nothrow__ , __leaf__));
extern int ilogbf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogbf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float scalblnf (float __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalblnf (float __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));
extern float nearbyintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __nearbyintf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float roundf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __roundf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float truncf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __truncf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float remquof (float __x, float __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern float __remquof (float __x, float __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));
extern long int lrintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrintf (float __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrintf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern long int lroundf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lroundf (float __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llroundf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llroundf (float __x) __attribute__ ((__nothrow__ , __leaf__));
extern float fdimf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __fdimf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));
extern float fmaxf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fmaxf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern float fminf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fminf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __fpclassifyf (float __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern int __signbitf (float __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern float fmaf (float __x, float __y, float __z) __attribute__ ((__nothrow__ , __leaf__)); extern float __fmaf (float __x, float __y, float __z) __attribute__ ((__nothrow__ , __leaf__));

extern float scalbf (float __x, float __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalbf (float __x, float __n) __attribute__ ((__nothrow__ , __leaf__));

extern long double acosl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __acosl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double asinl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __asinl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double atanl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atanl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double atan2l (long double __y, long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atan2l (long double __y, long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double cosl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __cosl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double sinl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sinl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double tanl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tanl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double coshl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __coshl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double sinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double tanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double acoshl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __acoshl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double asinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __asinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double atanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double expl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __expl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double frexpl (long double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern long double __frexpl (long double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));
extern long double ldexpl (long double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern long double __ldexpl (long double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));
extern long double logl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __logl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double log10l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log10l (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double modfl (long double __x, long double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern long double __modfl (long double __x, long double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern long double expm1l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __expm1l (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double log1pl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log1pl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double logbl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __logbl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double exp2l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __exp2l (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double log2l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log2l (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double powl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __powl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));
extern long double sqrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sqrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double hypotl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __hypotl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));


extern long double cbrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __cbrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double ceill (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __ceill (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double fabsl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fabsl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double floorl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __floorl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double fmodl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fmodl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));
extern int __isinfl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __finitel (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int isinfl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int finitel (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double dreml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __dreml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));
extern long double significandl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __significandl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double copysignl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __copysignl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double nanl (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nanl (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern int __isnanl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int isnanl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double j0l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __j0l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double j1l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __j1l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double jnl (int, long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __jnl (int, long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double y0l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __y0l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double y1l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __y1l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double ynl (int, long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __ynl (int, long double) __attribute__ ((__nothrow__ , __leaf__));

extern long double erfl (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __erfl (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double erfcl (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __erfcl (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double lgammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __lgammal (long double) __attribute__ ((__nothrow__ , __leaf__));


extern long double tgammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tgammal (long double) __attribute__ ((__nothrow__ , __leaf__));

extern long double gammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __gammal (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double lgammal_r (long double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern long double __lgammal_r (long double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));

extern long double rintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __rintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double nextafterl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nextafterl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double nexttowardl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nexttowardl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double remainderl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __remainderl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));
extern long double scalbnl (long double __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalbnl (long double __x, int __n) __attribute__ ((__nothrow__ , __leaf__));
extern int ilogbl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogbl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double scalblnl (long double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalblnl (long double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));
extern long double nearbyintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __nearbyintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double roundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __roundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double truncl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __truncl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double remquol (long double __x, long double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern long double __remquol (long double __x, long double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));
extern long int lrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long int lroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
extern long double fdiml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fdiml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));
extern long double fmaxl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fmaxl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern long double fminl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fminl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern int __fpclassifyl (long double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern int __signbitl (long double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));
extern long double fmal (long double __x, long double __y, long double __z) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fmal (long double __x, long double __y, long double __z) __attribute__ ((__nothrow__ , __leaf__));

extern long double scalbl (long double __x, long double __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalbl (long double __x, long double __n) __attribute__ ((__nothrow__ , __leaf__));
extern int signgam;
enum
  {
    FP_NAN =
      0,
    FP_INFINITE =
      1,
    FP_ZERO =
      2,
    FP_SUBNORMAL =
      3,
    FP_NORMAL =
      4
  };
typedef enum
{
  _IEEE_ = -1,
  _SVID_,
  _XOPEN_,
  _POSIX_,
  _ISOC_
} _LIB_VERSION_TYPE;
extern _LIB_VERSION_TYPE _LIB_VERSION;
struct exception
  {
    int type;
    char *name;
    double arg1;
    double arg2;
    double retval;
  };
extern int matherr (struct exception *__exc);

typedef int wchar_t;

typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __builtin_bswap32 (__bsx);
}
static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return __builtin_bswap64 (__bsx);
}
union wait
  {
    int w_status;
    struct
      {
 unsigned int __w_termsig:7;
 unsigned int __w_coredump:1;
 unsigned int __w_retcode:8;
 unsigned int:16;
      } __wait_terminated;
    struct
      {
 unsigned int __w_stopval:8;
 unsigned int __w_stopsig:8;
 unsigned int:16;
      } __wait_stopped;
  };
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));

typedef struct
  {
    int quot;
    int rem;
  } div_t;
typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;


__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;

extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) ;

extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;


__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;


extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern char *l64a (long int __n) __attribute__ ((__nothrow__ , __leaf__)) ;
extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
typedef __loff_t loff_t;
typedef __ino_t ino_t;
typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __uid_t uid_t;
typedef __pid_t pid_t;
typedef __id_t id_t;
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
typedef __key_t key_t;

typedef __clock_t clock_t;



typedef __time_t time_t;


typedef __clockid_t clockid_t;
typedef __timer_t timer_t;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));
typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));
typedef int register_t __attribute__ ((__mode__ (__word__)));
typedef int __sig_atomic_t;
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
typedef __sigset_t sigset_t;
struct timespec
  {
    __time_t tv_sec;
    __syscall_slong_t tv_nsec;
  };
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
typedef __suseconds_t suseconds_t;
typedef long int __fd_mask;
typedef struct
  {
    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];
  } fd_set;
typedef __fd_mask fd_mask;

extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);


__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

typedef __blksize_t blksize_t;
typedef __blkcnt_t blkcnt_t;
typedef __fsblkcnt_t fsblkcnt_t;
typedef __fsfilcnt_t fsfilcnt_t;
typedef unsigned long int pthread_t;
union pthread_attr_t
{
  char __size[56];
  long int __align;
};
typedef union pthread_attr_t pthread_attr_t;
typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    unsigned int __nusers;
    int __kind;
    short __spins;
    short __elision;
    __pthread_list_t __list;
  } __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;
typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
typedef union
{
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;
    unsigned int __flags;
  } __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;
typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;
typedef volatile int pthread_spinlock_t;
typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;

extern long int random (void) __attribute__ ((__nothrow__ , __leaf__));
extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));
extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };
extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));
extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));
extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));

extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ , __leaf__));
extern double drand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern long int lrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern long int mrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ , __leaf__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;
  };
extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;
extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;


extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));
extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));

extern void cfree (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));

extern void *alloca (size_t __size) __attribute__ ((__nothrow__ , __leaf__));

extern void *valloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;
extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern int putenv (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern int clearenv (void) __attribute__ ((__nothrow__ , __leaf__));
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern int system (const char *__command) ;

extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ , __leaf__)) ;
typedef int (*__compar_fn_t) (const void *, const void *);

extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;
extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
extern int abs (int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;

__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;

extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;


__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;

extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;
extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;
extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));
extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));
extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ , __leaf__));
extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));
extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__));

extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

int output;
int inputs[] = {(5), (1), (2), (3), (4)};
int a188 = 1;
int cf = 1;
int a70 = 9;
int a131 = 10;
int a168 = 9;
int a130 = 6;
int a57 = 11;
int a13 = 4;
int a104 = 1;
int a51 = 11;
int a133 = 3;
int a100 = 5;
int a182 = 7;
int a147 = 15;
int a24 = 7;
int a110 = 11;
int a198 = 4;
int a43 = 5;
int a183 = 5;
int a32 = 2;
int a112 = 7;
int a192 = 9;
int a59 = 2;
int a86 = 8;
int a143 = 7;
int a177 = 10;
int a158 = 6;
int a129 = 8;
int a138 = 9;
int a48 = 4;
int a162 = 8;
int a118 = 9;
int a76 = 3;
int a200 = 8;
int a153 = 11;
int a146 = 4;
int a35 = 4;
int main()
{
  while(1){
    output = - 1;
    scanf("%d",&input);
{
      cf = 1;
      if (cf == 1 && a162 == 7) {
        if (a32 == 4 && cf == 1) {{
            if (cf == 1 && a70 == 3) {{
                if (cf == 1 && input == 4) {
                  cf = 0;
                  a133 = 7;
                  a177 = 12;
                  a162 = 10;
                  output = 21;
                }
              }
            }
          }
        }
        if (cf == 1 && a32 == 7) {{
            if (a100 == 6 && cf == 1) {{
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a51 = 4;
                  a86 = 5;
                  a162 = 12;
                  output = 21;
                }
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a147 = 15;
                  a86 = 11;
                  a162 = 12;
                  output = 22;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a198 = 9;
                  a57 = 6;
                  a162 = 9;
                  output = 23;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a100 = 10;
                  output = 23;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  output = 25;
                }
              }
            }
          }
        }
      }
      if (a162 == 8 && cf == 1) {
        if (cf == 1 && a59 == 2) {{
            if (cf == 1 && a48 == 4) {{
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a200 = 7;
                  a86 = 9;
                  a162 = 12;
                  output = 21;
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a162 == 9) {
        if (cf == 1 && a57 == 6) {{
            if (a198 == 9 && cf == 1) {{
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a129 = 4;
                  a86 = 4;
                  a162 = 12;
                  output = 25;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a133 = 7;
                  a177 = 12;
                  a162 = 10;
                  output = 21;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a133 = 5;
                  a177 = 12;
                  a162 = 10;
                  output = 25;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a158 = 9;
                  a76 = 5;
                  a162 = 11;
                  output = 25;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a104 = 8;
                  a59 = 3;
                  a162 = 8;
                  output = 22;
                }
              }
            }
          }
        }
        if (a57 == 9 && cf == 1) {{
            if (a146 == 4 && cf == 1) {{
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a112 = 9;
                  a86 = 6;
                  a162 = 12;
                  output = 19;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a133 = 7;
                  a177 = 12;
                  a162 = 10;
                  output = 21;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a130 = 5;
                  a138 = 3;
                  a162 = 13;
                  output = 21;
                }
                if (cf == 1 && input == 4) {
                  cf = 0;
                  a35 = 8;
                  a183 = 3;
                  a162 = 14;
                  output = 22;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a112 = 9;
                  a32 = 6;
                  a162 = 7;
                  output = 20;
                }
              }
            }
            if (a146 == 6 && cf == 1) {{
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a32 = 1;
                  a177 = 7;
                  a162 = 10;
                  output = 21;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a32 = 6;
                  a177 = 7;
                  a162 = 10;
                  output = 20;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a32 = 1;
                  a177 = 7;
                  a162 = 10;
                  output = 21;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a198 = 9;
                  a57 = 6;
                  output = 20;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a138 = 5;
                  a59 = 6;
                  a162 = 8;
                  output = 22;
                }
              }
            }
          }
        }
      }
      if (a162 == 10 && cf == 1) {
        if (cf == 1 && a177 == 6) {{
            if (cf == 1 && a182 == 3) {{
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a182 = 7;
                  output = 19;
                }
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a32 = 2;
                  a177 = 7;
                  output = 19;
                }
              }
            }
            if (cf == 1 && a182 == 7) {{
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a153 = 9;
                  a138 = 7;
                  a162 = 13;
                  output = 23;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a112 = 7;
                  a32 = 6;
                  a162 = 7;
                  output = 19;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a138 = 4;
                  a59 = 6;
                  a162 = 8;
                  output = 21;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a133 = 7;
                  a86 = 7;
                  a162 = 12;
                  output = 21;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a100 = 6;
                  a32 = 7;
                  a162 = 7;
                  output = 20;
                }
              }
            }
          }
        }
        if (a177 == 7 && cf == 1) {{
            if (a32 == 1 && cf == 1) {{
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a182 = 3;
                  a177 = 6;
                  output = 20;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a200 = 3;
                  a86 = 9;
                  a162 = 12;
                  output = 21;
                }
              }
            }
            if (cf == 1 && a32 == 2) {{
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a143 = 14;
                  a76 = 7;
                  a162 = 11;
                  output = 25;
                }
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a118 = 3;
                  a138 = 9;
                  a162 = 13;
                  output = 20;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a35 = 7;
                  a76 = 0;
                  a162 = 11;
                  output = 20;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a146 = 6;
                  a57 = 9;
                  a162 = 9;
                  output = 19;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a112 = 6;
                  a183 = 2;
                  a162 = 14;
                  output = 20;
                }
              }
            }
          }
        }
        if (cf == 1 && a177 == 12) {{
            if (a133 == 7 && cf == 1) {{
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a158 = 9;
                  a76 = 5;
                  a162 = 11;
                  output = 25;
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a162 == 11) {
        if (cf == 1 && a76 == 5) {{
            if (cf == 1 && a158 == 9) {{
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a198 = 9;
                  a57 = 6;
                  a162 = 9;
                  output = 25;
                }
                if (cf == 1 && input == 4) {
                  cf = 0;
                  a146 = 4;
                  a57 = 9;
                  a162 = 9;
                  output = 21;
                }
              }
            }
          }
        }
      }
      if (a162 == 12 && cf == 1) {
        if (a86 == 6 && cf == 1) {{
            if (cf == 1 && a112 == 11) {{
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a146 = 5;
                  a86 = 10;
                  output = 20;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a110 = 10;
                  a183 = 6;
                  a162 = 14;
                  output = 22;
                }
              }
            }
          }
        }
        if (a86 == 8 && cf == 1) {{
            if (a131 == 8 && cf == 1) {{
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a188 = 3;
                  a177 = 5;
                  a162 = 10;
                  output = 19;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a198 = 9;
                  a57 = 6;
                  a162 = 9;
                  output = 22;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a200 = 8;
                  a59 = 4;
                  a162 = 8;
                  output = 21;
                }
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a110 = 6;
                  a183 = 6;
                  a162 = 14;
                  output = 20;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a198 = 9;
                  a57 = 6;
                  a162 = 9;
                  output = 20;
                }
              }
            }
          }
        }
        if (cf == 1 && a86 == 9) {{
            if (cf == 1 && a200 == 3) {{
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a110 = 6;
                  a183 = 6;
                  a162 = 14;
                  output = 20;
                }
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a13 = 2;
                  a183 = 5;
                  a162 = 14;
                  output = 21;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a112 = 11;
                  a86 = 6;
                  output = 19;
                }
              }
            }
            if (a200 == 7 && cf == 1) {{
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a146 = 2;
                  a86 = 10;
                  output = 25;
                }
              }
            }
          }
        }
        if (cf == 1 && a86 == 10) {{
            if (cf == 1 && a146 == 2) {{
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a70 = 3;
                  a32 = 4;
                  a162 = 7;
                  output = 25;
                }
                if (cf == 1 && input == 4) {
                  cf = 0;
                  a32 = 1;
                  a177 = 7;
                  a162 = 10;
                  output = 23;
                }
              }
            }
            if (a146 == 5 && cf == 1) {{
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a200 = 3;
                  a86 = 9;
                  output = 21;
                }
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a112 = 6;
                  a86 = 6;
                  output = 20;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a200 = 3;
                  a86 = 9;
                  output = 21;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a147 = 14;
                  a86 = 11;
                  output = 25;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a168 = 7;
                  a183 = 0;
                  a162 = 14;
                  output = 21;
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a162 == 13) {
        if (cf == 1 && a138 == 8) {{
            if (cf == 1 && a43 == 3) {{
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a188 = 1;
                  a177 = 5;
                  a162 = 10;
                  output = 22;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a146 = 2;
                  a86 = 10;
                  a162 = 12;
                  output = 25;
                }
                if (input == 4 && cf == 1) {
                  cf = 0;
                  a192 = 4;
                  a59 = 7;
                  a162 = 8;
                  output = 22;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a168 = 13;
                  a183 = 0;
                  a162 = 14;
                  output = 21;
                }
                if (cf == 1 && input == 2) {
                  cf = 0;
                  a133 = 3;
                  a177 = 12;
                  a162 = 10;
                  output = 21;
                }
              }
            }
          }
        }
      }
      if (a162 == 14 && cf == 1) {
        if (a183 == 5 && cf == 1) {{
            if (cf == 1 && a13 == 2) {{
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a43 = 3;
                  a138 = 8;
                  a162 = 13;
                  output = 22;
                }
                if (cf == 1 && input == 3) {
                  cf = 0;
                  a133 = 2;
                  a177 = 12;
                  a162 = 10;
                  output = 21;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a48 = 2;
                  a59 = 2;
                  a162 = 8;
                  output = 20;
                }
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a24 = 1;
                  a177 = 8;
                  a162 = 10;
                  output = 21;
                }
              }
            }
          }
        }
        if (cf == 1 && a183 == 6) {{
            if (a110 == 6 && cf == 1) {{
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a168 = 11;
                  a183 = 0;
                  output = 20;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a146 = 9;
                  a86 = 10;
                  a162 = 12;
                  output = 23;
                }
                if (cf == 1 && input == 5) {
                  cf = 0;
                  a131 = 8;
                  a86 = 8;
                  a162 = 12;
                  output = 19;
                }
                if (input == 1 && cf == 1) {
                  cf = 0;
                  a138 = 4;
                  a32 = 2;
                  a162 = 7;
                  output = 20;
                }
                if (cf == 1 && input == 4) {
                  cf = 0;
                  a104 = 5;
                  a59 = 3;
                  a162 = 8;
                  output = 21;
                }
              }
            }
            if (a110 == 10 && cf == 1) {{
                if (cf == 1 && input == 4) {
                  cf = 0;
                  output = 22;
                }
                if (input == 5 && cf == 1) {
                  cf = 0;
                  a112 = 11;
                  a86 = 6;
                  a162 = 12;
                  output = 19;
                }
                if (input == 3 && cf == 1) {
                  cf = 0;
                  a158 = 4;
                  a76 = 5;
                  a162 = 11;
                  output = 19;
                }
                if (input == 2 && cf == 1) {
                  cf = 0;
                  a200 = 6;
                  a177 = 10;
                  a162 = 10;
                  output = 22;
                }
                if (cf == 1 && input == 1) {
                  cf = 0;
                  a112 = 12;
                  a183 = 2;
                  output = 22;
                }
              }
            }
          }
        }
      }
{
        if (a112 == 13 && a86 == 6 && a162 == 12) {
          cf = 0;
          error_0:
          !1?((void )0) : __assert_fail("!error_0","Problem1401_mod_global_in_out.c",169,__PRETTY_FUNCTION__);
        }
        if (11 == 9 && a57 == 7 && a162 == 9) {
          cf = 0;
          error_1:
          !1?((void )0) : __assert_fail("!error_1","Problem1401_mod_global_in_out.c",173,__PRETTY_FUNCTION__);
        }
        if (a146 == 3 && a86 == 10 && a162 == 12) {
          cf = 0;
          error_2:
          !1?((void )0) : __assert_fail("!error_2","Problem1401_mod_global_in_out.c",177,__PRETTY_FUNCTION__);
        }
        if (a32 == 6 && a177 == 7 && a162 == 10) {
          cf = 0;
          error_3:
          !1?((void )0) : __assert_fail("!error_3","Problem1401_mod_global_in_out.c",181,__PRETTY_FUNCTION__);
        }
        if (a188 == 1 && a177 == 5 && a162 == 10) {
          cf = 0;
          error_4:
          !1?((void )0) : __assert_fail("!error_4","Problem1401_mod_global_in_out.c",185,__PRETTY_FUNCTION__);
        }
        if (2 == 5 && a32 == 3 && a162 == 7) {
          cf = 0;
          error_5:
          !1?((void )0) : __assert_fail("!error_5","Problem1401_mod_global_in_out.c",189,__PRETTY_FUNCTION__);
        }
        if (a112 == 9 && a32 == 6 && a162 == 7) {
          cf = 0;
          error_6:
          !1?((void )0) : __assert_fail("!error_6","Problem1401_mod_global_in_out.c",193,__PRETTY_FUNCTION__);
        }
        if (a13 == 4 && a32 == 5 && a162 == 7) {
          cf = 0;
          error_7:
          !1?((void )0) : __assert_fail("!error_7","Problem1401_mod_global_in_out.c",197,__PRETTY_FUNCTION__);
        }
        if (a153 == 12 && a138 == 7 && a162 == 13) {
          cf = 0;
          error_8:
          !1?((void )0) : __assert_fail("!error_8","Problem1401_mod_global_in_out.c",201,__PRETTY_FUNCTION__);
        }
        if (a35 == 8 && a183 == 3 && a162 == 14) {
          cf = 0;
          error_9:
          !1?((void )0) : __assert_fail("!error_9","Problem1401_mod_global_in_out.c",205,__PRETTY_FUNCTION__);
        }
        if (2 == 4 && a59 == 9 && a162 == 8) {
          cf = 0;
          error_10:
          !1?((void )0) : __assert_fail("!error_10","Problem1401_mod_global_in_out.c",209,__PRETTY_FUNCTION__);
        }
        if (a146 == 9 && a86 == 10 && a162 == 12) {
          cf = 0;
          error_11:
          !1?((void )0) : __assert_fail("!error_11","Problem1401_mod_global_in_out.c",213,__PRETTY_FUNCTION__);
        }
        if (14 == 11 && a138 == 5 && a162 == 13) {
          cf = 0;
          error_12:
          !1?((void )0) : __assert_fail("!error_12","Problem1401_mod_global_in_out.c",217,__PRETTY_FUNCTION__);
        }
        if (a146 == 5 && a57 == 11 && a162 == 9) {
          cf = 0;
          error_13:
          !1?((void )0) : __assert_fail("!error_13","Problem1401_mod_global_in_out.c",221,__PRETTY_FUNCTION__);
        }
        if (a188 == 3 && a177 == 5 && a162 == 10) {
          cf = 0;
          error_14:
          !1?((void )0) : __assert_fail("!error_14","Problem1401_mod_global_in_out.c",225,__PRETTY_FUNCTION__);
        }
        if (a112 == 7 && a32 == 6 && a162 == 7) {
          cf = 0;
          error_15:
          !1?((void )0) : __assert_fail("!error_15","Problem1401_mod_global_in_out.c",229,__PRETTY_FUNCTION__);
        }
        if (11 == 10 && a57 == 7 && a162 == 9) {
          cf = 0;
          error_16:
          !1?((void )0) : __assert_fail("!error_16","Problem1401_mod_global_in_out.c",233,__PRETTY_FUNCTION__);
        }
        if (a133 == 5 && a177 == 12 && a162 == 10) {
          cf = 0;
          error_17:
          !1?((void )0) : __assert_fail("!error_17","Problem1401_mod_global_in_out.c",237,__PRETTY_FUNCTION__);
        }
        if (2 == 3 && a59 == 9 && a162 == 8) {
          cf = 0;
          error_18:
          !1?((void )0) : __assert_fail("!error_18","Problem1401_mod_global_in_out.c",241,__PRETTY_FUNCTION__);
        }
        if (a188 == 2 && a177 == 5 && a162 == 10) {
          cf = 0;
          error_19:
          !1?((void )0) : __assert_fail("!error_19","Problem1401_mod_global_in_out.c",245,__PRETTY_FUNCTION__);
        }
        if (a112 == 12 && a183 == 2 && a162 == 14) {
          cf = 0;
          error_20:
          !1?((void )0) : __assert_fail("!error_20","Problem1401_mod_global_in_out.c",249,__PRETTY_FUNCTION__);
        }
        if (a153 == 9 && a138 == 7 && a162 == 13) {
          cf = 0;
          error_21:
          !1?((void )0) : __assert_fail("!error_21","Problem1401_mod_global_in_out.c",253,__PRETTY_FUNCTION__);
        }
        if (a147 == 13 && a86 == 11 && a162 == 12) {
          cf = 0;
          error_22:
          !1?((void )0) : __assert_fail("!error_22","Problem1401_mod_global_in_out.c",257,__PRETTY_FUNCTION__);
        }
        if (a48 == 1 && a59 == 2 && a162 == 8) {
          cf = 0;
          error_23:
          !1?((void )0) : __assert_fail("!error_23","Problem1401_mod_global_in_out.c",261,__PRETTY_FUNCTION__);
        }
        if (a138 == 4 && a32 == 2 && a162 == 7) {
          cf = 0;
          error_24:
          !1?((void )0) : __assert_fail("!error_24","Problem1401_mod_global_in_out.c",265,__PRETTY_FUNCTION__);
        }
        if (a13 == 1 && a32 == 5 && a162 == 7) {
          cf = 0;
          error_25:
          !1?((void )0) : __assert_fail("!error_25","Problem1401_mod_global_in_out.c",269,__PRETTY_FUNCTION__);
        }
        if (a129 == 4 && a86 == 4 && a162 == 12) {
          cf = 0;
          error_26:
          !1?((void )0) : __assert_fail("!error_26","Problem1401_mod_global_in_out.c",273,__PRETTY_FUNCTION__);
        }
        if (a158 == 7 && a76 == 5 && a162 == 11) {
          cf = 0;
          error_27:
          !1?((void )0) : __assert_fail("!error_27","Problem1401_mod_global_in_out.c",277,__PRETTY_FUNCTION__);
        }
        if (a35 == 7 && a76 == 0 && a162 == 11) {
          cf = 0;
          error_28:
          !1?((void )0) : __assert_fail("!error_28","Problem1401_mod_global_in_out.c",281,__PRETTY_FUNCTION__);
        }
        if (a158 == 2 && a183 == 1 && a162 == 14) {
          cf = 0;
          error_29:
          !1?((void )0) : __assert_fail("!error_29","Problem1401_mod_global_in_out.c",285,__PRETTY_FUNCTION__);
        }
        if (a182 == 5 && a177 == 6 && a162 == 10) {
          cf = 0;
          error_30:
          !1?((void )0) : __assert_fail("!error_30","Problem1401_mod_global_in_out.c",289,__PRETTY_FUNCTION__);
        }
        if (a51 == 5 && a76 == 6 && a162 == 11) {
          cf = 0;
          error_31:
          !1?((void )0) : __assert_fail("!error_31","Problem1401_mod_global_in_out.c",293,__PRETTY_FUNCTION__);
        }
        if (a100 == 10 && a32 == 7 && a162 == 7) {
          cf = 0;
          error_32:
          !1?((void )0) : __assert_fail("!error_32","Problem1401_mod_global_in_out.c",297,__PRETTY_FUNCTION__);
        }
        if (a43 == 1 && a138 == 8 && a162 == 13) {
          cf = 0;
          error_33:
          !1?((void )0) : __assert_fail("!error_33","Problem1401_mod_global_in_out.c",301,__PRETTY_FUNCTION__);
        }
        if (a35 == 8 && a76 == 0 && a162 == 11) {
          cf = 0;
          error_34:
          !1?((void )0) : __assert_fail("!error_34","Problem1401_mod_global_in_out.c",305,__PRETTY_FUNCTION__);
        }
        if (a131 == 9 && a86 == 8 && a162 == 12) {
          cf = 0;
          error_35:
          !1?((void )0) : __assert_fail("!error_35","Problem1401_mod_global_in_out.c",309,__PRETTY_FUNCTION__);
        }
        if (2 == 8 && a32 == 3 && a162 == 7) {
          cf = 0;
          error_36:
          !1?((void )0) : __assert_fail("!error_36","Problem1401_mod_global_in_out.c",313,__PRETTY_FUNCTION__);
        }
        if (a138 == 3 && a59 == 6 && a162 == 8) {
          cf = 0;
          error_37:
          !1?((void )0) : __assert_fail("!error_37","Problem1401_mod_global_in_out.c",317,__PRETTY_FUNCTION__);
        }
        if (a43 == 4 && a138 == 8 && a162 == 13) {
          cf = 0;
          error_38:
          !1?((void )0) : __assert_fail("!error_38","Problem1401_mod_global_in_out.c",321,__PRETTY_FUNCTION__);
        }
        if (a133 == 2 && a177 == 12 && a162 == 10) {
          cf = 0;
          error_39:
          !1?((void )0) : __assert_fail("!error_39","Problem1401_mod_global_in_out.c",325,__PRETTY_FUNCTION__);
        }
        if (a131 == 9 && a59 == 5 && a162 == 8) {
          cf = 0;
          error_40:
          !1?((void )0) : __assert_fail("!error_40","Problem1401_mod_global_in_out.c",329,__PRETTY_FUNCTION__);
        }
        if (a104 == 8 && a59 == 3 && a162 == 8) {
          cf = 0;
          error_41:
          !1?((void )0) : __assert_fail("!error_41","Problem1401_mod_global_in_out.c",333,__PRETTY_FUNCTION__);
        }
        if (a48 == 2 && a59 == 2 && a162 == 8) {
          cf = 0;
          error_42:
          !1?((void )0) : __assert_fail("!error_42","Problem1401_mod_global_in_out.c",337,__PRETTY_FUNCTION__);
        }
        if (a200 == 6 && a177 == 10 && a162 == 10) {
          cf = 0;
          error_43:
          !1?((void )0) : __assert_fail("!error_43","Problem1401_mod_global_in_out.c",341,__PRETTY_FUNCTION__);
        }
        if (a110 == 11 && a183 == 6 && a162 == 14) {
          cf = 0;
          error_44:
          !1?((void )0) : __assert_fail("!error_44","Problem1401_mod_global_in_out.c",345,__PRETTY_FUNCTION__);
        }
        if (a24 == 4 && a177 == 8 && a162 == 10) {
          cf = 0;
          error_45:
          !1?((void )0) : __assert_fail("!error_45","Problem1401_mod_global_in_out.c",349,__PRETTY_FUNCTION__);
        }
        if (a200 == 7 && a59 == 4 && a162 == 8) {
          cf = 0;
          error_46:
          !1?((void )0) : __assert_fail("!error_46","Problem1401_mod_global_in_out.c",353,__PRETTY_FUNCTION__);
        }
        if (11 == 7 && a177 == 9 && a162 == 10) {
          cf = 0;
          error_47:
          !1?((void )0) : __assert_fail("!error_47","Problem1401_mod_global_in_out.c",357,__PRETTY_FUNCTION__);
        }
        if (a133 == 7 && a86 == 7 && a162 == 12) {
          cf = 0;
          error_48:
          !1?((void )0) : __assert_fail("!error_48","Problem1401_mod_global_in_out.c",361,__PRETTY_FUNCTION__);
        }
        if (a133 == 3 && a177 == 12 && a162 == 10) {
          cf = 0;
          error_49:
          !1?((void )0) : __assert_fail("!error_49","Problem1401_mod_global_in_out.c",365,__PRETTY_FUNCTION__);
        }
        if (a48 == 0 && a59 == 2 && a162 == 8) {
          cf = 0;
          error_50:
          !1?((void )0) : __assert_fail("!error_50","Problem1401_mod_global_in_out.c",369,__PRETTY_FUNCTION__);
        }
        if (a147 == 14 && a86 == 11 && a162 == 12) {
          cf = 0;
          error_51:
          !1?((void )0) : __assert_fail("!error_51","Problem1401_mod_global_in_out.c",373,__PRETTY_FUNCTION__);
        }
        if (a104 == 5 && a59 == 3 && a162 == 8) {
          cf = 0;
          error_52:
          !1?((void )0) : __assert_fail("!error_52","Problem1401_mod_global_in_out.c",377,__PRETTY_FUNCTION__);
        }
        if (a198 == 7 && a57 == 6 && a162 == 9) {
          cf = 0;
          error_53:
          !1?((void )0) : __assert_fail("!error_53","Problem1401_mod_global_in_out.c",381,__PRETTY_FUNCTION__);
        }
        if (a118 == 7 && a138 == 9 && a162 == 13) {
          cf = 0;
          error_54:
          !1?((void )0) : __assert_fail("!error_54","Problem1401_mod_global_in_out.c",385,__PRETTY_FUNCTION__);
        }
        if (a131 == 6 && a59 == 5 && a162 == 8) {
          cf = 0;
          error_55:
          !1?((void )0) : __assert_fail("!error_55","Problem1401_mod_global_in_out.c",389,__PRETTY_FUNCTION__);
        }
        if (a118 == 3 && a138 == 9 && a162 == 13) {
          cf = 0;
          error_56:
          !1?((void )0) : __assert_fail("!error_56","Problem1401_mod_global_in_out.c",393,__PRETTY_FUNCTION__);
        }
        if (a24 == 1 && a177 == 8 && a162 == 10) {
          cf = 0;
          error_57:
          !1?((void )0) : __assert_fail("!error_57","Problem1401_mod_global_in_out.c",397,__PRETTY_FUNCTION__);
        }
        if (1 == 5 && a32 == 8 && a162 == 7) {
          cf = 0;
          error_58:
          !1?((void )0) : __assert_fail("!error_58","Problem1401_mod_global_in_out.c",401,__PRETTY_FUNCTION__);
        }
        if (a200 == 9 && a177 == 10 && a162 == 10) {
          cf = 0;
          error_59:
          !1?((void )0) : __assert_fail("!error_59","Problem1401_mod_global_in_out.c",405,__PRETTY_FUNCTION__);
        }
        if (a138 == 5 && a59 == 6 && a162 == 8) {
          cf = 0;
          error_60:
          !1?((void )0) : __assert_fail("!error_60","Problem1401_mod_global_in_out.c",409,__PRETTY_FUNCTION__);
        }
        if (a147 == 15 && a86 == 11 && a162 == 12) {
          cf = 0;
          error_61:
          !1?((void )0) : __assert_fail("!error_61","Problem1401_mod_global_in_out.c",413,__PRETTY_FUNCTION__);
        }
        if (a112 == 6 && a86 == 6 && a162 == 12) {
          cf = 0;
          error_62:
          !1?((void )0) : __assert_fail("!error_62","Problem1401_mod_global_in_out.c",417,__PRETTY_FUNCTION__);
        }
        if (2 == 6 && a32 == 3 && a162 == 7) {
          cf = 0;
          error_63:
          !1?((void )0) : __assert_fail("!error_63","Problem1401_mod_global_in_out.c",421,__PRETTY_FUNCTION__);
        }
        if (a112 == 6 && a183 == 2 && a162 == 14) {
          cf = 0;
          error_64:
          !1?((void )0) : __assert_fail("!error_64","Problem1401_mod_global_in_out.c",425,__PRETTY_FUNCTION__);
        }
        if (11 == 6 && a57 == 7 && a162 == 9) {
          cf = 0;
          error_65:
          !1?((void )0) : __assert_fail("!error_65","Problem1401_mod_global_in_out.c",429,__PRETTY_FUNCTION__);
        }
        if (a192 == 4 && a59 == 7 && a162 == 8) {
          cf = 0;
          error_66:
          !1?((void )0) : __assert_fail("!error_66","Problem1401_mod_global_in_out.c",433,__PRETTY_FUNCTION__);
        }
        if (a51 == 4 && a86 == 5 && a162 == 12) {
          cf = 0;
          error_67:
          !1?((void )0) : __assert_fail("!error_67","Problem1401_mod_global_in_out.c",437,__PRETTY_FUNCTION__);
        }
        if (a200 == 2 && a59 == 4 && a162 == 8) {
          cf = 0;
          error_68:
          !1?((void )0) : __assert_fail("!error_68","Problem1401_mod_global_in_out.c",441,__PRETTY_FUNCTION__);
        }
        if (a138 == 7 && a59 == 6 && a162 == 8) {
          cf = 0;
          error_69:
          !1?((void )0) : __assert_fail("!error_69","Problem1401_mod_global_in_out.c",445,__PRETTY_FUNCTION__);
        }
        if (11 == 11 && a57 == 7 && a162 == 9) {
          cf = 0;
          error_70:
          !1?((void )0) : __assert_fail("!error_70","Problem1401_mod_global_in_out.c",449,__PRETTY_FUNCTION__);
        }
        if (7 == 4 && a138 == 6 && a162 == 13) {
          cf = 0;
          error_71:
          !1?((void )0) : __assert_fail("!error_71","Problem1401_mod_global_in_out.c",453,__PRETTY_FUNCTION__);
        }
        if (10 == 12 && a138 == 4 && a162 == 13) {
          cf = 0;
          error_72:
          !1?((void )0) : __assert_fail("!error_72","Problem1401_mod_global_in_out.c",457,__PRETTY_FUNCTION__);
        }
        if (11 == 6 && a76 == 4 && a162 == 11) {
          cf = 0;
          error_73:
          !1?((void )0) : __assert_fail("!error_73","Problem1401_mod_global_in_out.c",461,__PRETTY_FUNCTION__);
        }
        if (a168 == 11 && a183 == 0 && a162 == 14) {
          cf = 0;
          error_74:
          !1?((void )0) : __assert_fail("!error_74","Problem1401_mod_global_in_out.c",465,__PRETTY_FUNCTION__);
        }
        if (a32 == 4 && a177 == 7 && a162 == 10) {
          cf = 0;
          error_75:
          !1?((void )0) : __assert_fail("!error_75","Problem1401_mod_global_in_out.c",469,__PRETTY_FUNCTION__);
        }
        if (a112 == 8 && a183 == 2 && a162 == 14) {
          cf = 0;
          error_76:
          !1?((void )0) : __assert_fail("!error_76","Problem1401_mod_global_in_out.c",473,__PRETTY_FUNCTION__);
        }
        if (a146 == 4 && a86 == 10 && a162 == 12) {
          cf = 0;
          error_77:
          !1?((void )0) : __assert_fail("!error_77","Problem1401_mod_global_in_out.c",477,__PRETTY_FUNCTION__);
        }
        if (11 == 9 && a177 == 9 && a162 == 10) {
          cf = 0;
          error_78:
          !1?((void )0) : __assert_fail("!error_78","Problem1401_mod_global_in_out.c",481,__PRETTY_FUNCTION__);
        }
        if (a200 == 8 && a59 == 4 && a162 == 8) {
          cf = 0;
          error_79:
          !1?((void )0) : __assert_fail("!error_79","Problem1401_mod_global_in_out.c",485,__PRETTY_FUNCTION__);
        }
        if (a138 == 8 && a59 == 6 && a162 == 8) {
          cf = 0;
          error_80:
          !1?((void )0) : __assert_fail("!error_80","Problem1401_mod_global_in_out.c",489,__PRETTY_FUNCTION__);
        }
        if (9 == 11 && a57 == 12 && a162 == 9) {
          cf = 0;
          error_81:
          !1?((void )0) : __assert_fail("!error_81","Problem1401_mod_global_in_out.c",493,__PRETTY_FUNCTION__);
        }
        if (a188 == 4 && a177 == 5 && a162 == 10) {
          cf = 0;
          error_82:
          !1?((void )0) : __assert_fail("!error_82","Problem1401_mod_global_in_out.c",497,__PRETTY_FUNCTION__);
        }
        if (1 == 1 && a57 == 13 && a162 == 9) {
          cf = 0;
          error_83:
          !1?((void )0) : __assert_fail("!error_83","Problem1401_mod_global_in_out.c",501,__PRETTY_FUNCTION__);
        }
        if (a143 == 14 && a76 == 7 && a162 == 11) {
          cf = 0;
          error_84:
          !1?((void )0) : __assert_fail("!error_84","Problem1401_mod_global_in_out.c",505,__PRETTY_FUNCTION__);
        }
        if (a168 == 13 && a183 == 0 && a162 == 14) {
          cf = 0;
          error_85:
          !1?((void )0) : __assert_fail("!error_85","Problem1401_mod_global_in_out.c",509,__PRETTY_FUNCTION__);
        }
        if (8 == 5 && a138 == 10 && a162 == 13) {
          cf = 0;
          error_86:
          !1?((void )0) : __assert_fail("!error_86","Problem1401_mod_global_in_out.c",513,__PRETTY_FUNCTION__);
        }
        if (a200 == 8 && a177 == 10 && a162 == 10) {
          cf = 0;
          error_87:
          !1?((void )0) : __assert_fail("!error_87","Problem1401_mod_global_in_out.c",517,__PRETTY_FUNCTION__);
        }
        if (a51 == 9 && a76 == 6 && a162 == 11) {
          cf = 0;
          error_88:
          !1?((void )0) : __assert_fail("!error_88","Problem1401_mod_global_in_out.c",521,__PRETTY_FUNCTION__);
        }
        if (a158 == 4 && a76 == 5 && a162 == 11) {
          cf = 0;
          error_89:
          !1?((void )0) : __assert_fail("!error_89","Problem1401_mod_global_in_out.c",525,__PRETTY_FUNCTION__);
        }
        if (a200 == 2 && a86 == 9 && a162 == 12) {
          cf = 0;
          error_90:
          !1?((void )0) : __assert_fail("!error_90","Problem1401_mod_global_in_out.c",529,__PRETTY_FUNCTION__);
        }
        if (a146 == 3 && a57 == 9 && a162 == 9) {
          cf = 0;
          error_91:
          !1?((void )0) : __assert_fail("!error_91","Problem1401_mod_global_in_out.c",533,__PRETTY_FUNCTION__);
        }
        if (a130 == 5 && a138 == 3 && a162 == 13) {
          cf = 0;
          error_92:
          !1?((void )0) : __assert_fail("!error_92","Problem1401_mod_global_in_out.c",537,__PRETTY_FUNCTION__);
        }
        if (a112 == 9 && a86 == 6 && a162 == 12) {
          cf = 0;
          error_93:
          !1?((void )0) : __assert_fail("!error_93","Problem1401_mod_global_in_out.c",541,__PRETTY_FUNCTION__);
        }
        if (a138 == 4 && a59 == 6 && a162 == 8) {
          cf = 0;
          error_94:
          !1?((void )0) : __assert_fail("!error_94","Problem1401_mod_global_in_out.c",545,__PRETTY_FUNCTION__);
        }
        if (a168 == 7 && a183 == 0 && a162 == 14) {
          cf = 0;
          error_95:
          !1?((void )0) : __assert_fail("!error_95","Problem1401_mod_global_in_out.c",549,__PRETTY_FUNCTION__);
        }
        if (a100 == 7 && a32 == 7 && a162 == 7) {
          cf = 0;
          error_96:
          !1?((void )0) : __assert_fail("!error_96","Problem1401_mod_global_in_out.c",553,__PRETTY_FUNCTION__);
        }
        if (a158 == 6 && a76 == 5 && a162 == 11) {
          cf = 0;
          error_97:
          !1?((void )0) : __assert_fail("!error_97","Problem1401_mod_global_in_out.c",557,__PRETTY_FUNCTION__);
        }
        if (a118 == 7 && a59 == 8 && a162 == 8) {
          cf = 0;
          error_98:
          !1?((void )0) : __assert_fail("!error_98","Problem1401_mod_global_in_out.c",561,__PRETTY_FUNCTION__);
        }
        if (13 == 12 && a57 == 10 && a162 == 9) {
          cf = 0;
          error_99:
          !1?((void )0) : __assert_fail("!error_99","Problem1401_mod_global_in_out.c",565,__PRETTY_FUNCTION__);
        }
      }
      if (cf == 1) {
        output = - 2;
      }
    }
    if (output == - 2) {
      fprintf(stderr,"Invalid input: %d\n",input);
    }
    else {
      if (output != - 1) {
        printf("%d\n",output);
      }
    }
  }
}
