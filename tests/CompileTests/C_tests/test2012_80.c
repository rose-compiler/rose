// # 1 "enter.c"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "enter.c"
// # 20 "enter.c"
// # 1 "mutt.h" 1
// # 19 "mutt.h"
// # 1 "config.h" 1
// # 20 "mutt.h" 2

// # 1 "/usr/include/stdio.h" 1 3 4
// # 28 "/usr/include/stdio.h" 3 4
// # 1 "/usr/include/features.h" 1 3 4
// # 329 "/usr/include/features.h" 3 4
// # 1 "/usr/include/sys/cdefs.h" 1 3 4
// # 313 "/usr/include/sys/cdefs.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 314 "/usr/include/sys/cdefs.h" 2 3 4
// # 330 "/usr/include/features.h" 2 3 4
// # 352 "/usr/include/features.h" 3 4
// # 1 "/usr/include/gnu/stubs.h" 1 3 4



// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 5 "/usr/include/gnu/stubs.h" 2 3 4




// # 1 "/usr/include/gnu/stubs-64.h" 1 3 4
// # 10 "/usr/include/gnu/stubs.h" 2 3 4
// # 353 "/usr/include/features.h" 2 3 4
// # 29 "/usr/include/stdio.h" 2 3 4





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 214 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long unsigned int size_t;
// # 35 "/usr/include/stdio.h" 2 3 4

// # 1 "/usr/include/bits/types.h" 1 3 4
// # 28 "/usr/include/bits/types.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 29 "/usr/include/bits/types.h" 2 3 4


// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 32 "/usr/include/bits/types.h" 2 3 4


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
// # 134 "/usr/include/bits/types.h" 3 4
// # 1 "/usr/include/bits/typesizes.h" 1 3 4
// # 135 "/usr/include/bits/types.h" 2 3 4


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
typedef long int __swblk_t;
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

typedef long int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;
// # 37 "/usr/include/stdio.h" 2 3 4









typedef struct _IO_FILE FILE;





// # 62 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
// # 72 "/usr/include/stdio.h" 3 4
// # 1 "/usr/include/libio.h" 1 3 4
// # 32 "/usr/include/libio.h" 3 4
// # 1 "/usr/include/_G_config.h" 1 3 4
// # 14 "/usr/include/_G_config.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 326 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef int wchar_t;
// # 355 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef unsigned int wint_t;
// # 15 "/usr/include/_G_config.h" 2 3 4
// # 24 "/usr/include/_G_config.h" 3 4
// # 1 "/usr/include/wchar.h" 1 3 4
// # 48 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 49 "/usr/include/wchar.h" 2 3 4

// # 1 "/usr/include/bits/wchar.h" 1 3 4
// # 51 "/usr/include/wchar.h" 2 3 4
// # 76 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
// # 25 "/usr/include/_G_config.h" 2 3 4

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
// # 44 "/usr/include/_G_config.h" 3 4
// # 1 "/usr/include/gconv.h" 1 3 4
// # 28 "/usr/include/gconv.h" 3 4
// # 1 "/usr/include/wchar.h" 1 3 4
// # 48 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 49 "/usr/include/wchar.h" 2 3 4
// # 29 "/usr/include/gconv.h" 2 3 4


// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 32 "/usr/include/gconv.h" 2 3 4





enum
{
  __GCONV_OK = 0,
  __GCONV_NOCONV,
  __GCONV_NODB,
  __GCONV_NOMEM,

  __GCONV_EMPTY_INPUT,
  __GCONV_FULL_OUTPUT,
  __GCONV_ILLEGAL_INPUT,
  __GCONV_INCOMPLETE_INPUT,

  __GCONV_ILLEGAL_DESCRIPTOR,
  __GCONV_INTERNAL_ERROR
};



enum
{
  __GCONV_IS_LAST = 0x0001,
  __GCONV_IGNORE_ERRORS = 0x0002
};



struct __gconv_step;
struct __gconv_step_data;
struct __gconv_loaded_object;
struct __gconv_trans_data;



typedef int (*__gconv_fct) (struct __gconv_step *, struct __gconv_step_data *,
       __const unsigned char **, __const unsigned char *,
       unsigned char **, size_t *, int, int);


typedef wint_t (*__gconv_btowc_fct) (struct __gconv_step *, unsigned char);


typedef int (*__gconv_init_fct) (struct __gconv_step *);
typedef void (*__gconv_end_fct) (struct __gconv_step *);



typedef int (*__gconv_trans_fct) (struct __gconv_step *,
      struct __gconv_step_data *, void *,
      __const unsigned char *,
      __const unsigned char **,
      __const unsigned char *, unsigned char **,
      size_t *);


typedef int (*__gconv_trans_context_fct) (void *, __const unsigned char *,
       __const unsigned char *,
       unsigned char *, unsigned char *);


typedef int (*__gconv_trans_query_fct) (__const char *, __const char ***,
     size_t *);


typedef int (*__gconv_trans_init_fct) (void **, const char *);
typedef void (*__gconv_trans_end_fct) (void *);

struct __gconv_trans_data
{

  __gconv_trans_fct __trans_fct;
  __gconv_trans_context_fct __trans_context_fct;
  __gconv_trans_end_fct __trans_end_fct;
  void *__data;
  struct __gconv_trans_data *__next;
};



struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;

  int __counter;

  char *__from_name;
  char *__to_name;

  __gconv_fct __fct;
  __gconv_btowc_fct __btowc_fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;



  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;


  int __stateful;

  void *__data;
};



struct __gconv_step_data
{
  unsigned char *__outbuf;
  unsigned char *__outbufend;



  int __flags;



  int __invocation_counter;



  int __internal_use;

  __mbstate_t *__statep;
  __mbstate_t __state;



  struct __gconv_trans_data *__trans;
};



typedef struct __gconv_info
{
  size_t __nsteps;
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data [];
} *__gconv_t;
// # 45 "/usr/include/_G_config.h" 2 3 4
typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;

typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
// # 33 "/usr/include/libio.h" 2 3 4
// # 53 "/usr/include/libio.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 43 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
// # 54 "/usr/include/libio.h" 2 3 4
// # 167 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
// # 177 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
// # 200 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
// # 268 "/usr/include/libio.h" 3 4
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
// # 316 "/usr/include/libio.h" 3 4
  __off64_t _offset;
// # 325 "/usr/include/libio.h" 3 4
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
// # 361 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
// # 413 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
extern wint_t __wunderflow (_IO_FILE *);
extern wint_t __wuflow (_IO_FILE *);
extern wint_t __woverflow (_IO_FILE *, wint_t);
// # 451 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
// # 481 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
// # 73 "/usr/include/stdio.h" 2 3 4
// # 86 "/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;




// # 138 "/usr/include/stdio.h" 3 4
// # 1 "/usr/include/bits/stdio_lim.h" 1 3 4
// # 139 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;









extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));














extern FILE *tmpfile (void);
// # 185 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__));





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__));
// # 203 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__));








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

// # 228 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
// # 242 "/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes);




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream);
// # 269 "/usr/include/stdio.h" 3 4

// # 280 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__));
// # 300 "/usr/include/stdio.h" 3 4



extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__));








extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

// # 394 "/usr/include/stdio.h" 3 4





extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));

// # 436 "/usr/include/stdio.h" 3 4





extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

// # 460 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
// # 471 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

// # 504 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

// # 585 "/usr/include/stdio.h" 3 4





extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s) ;

// # 638 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream) ;








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

// # 674 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
// # 693 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
// # 716 "/usr/include/stdio.h" 3 4

// # 725 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;








extern void perror (__const char *__s);






// # 1 "/usr/include/bits/sys_errlist.h" 1 3 4
// # 27 "/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];
// # 755 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
// # 774 "/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
// # 814 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
// # 844 "/usr/include/stdio.h" 3 4

// # 22 "mutt.h" 2
// # 1 "/usr/include/stdlib.h" 1 3 4
// # 33 "/usr/include/stdlib.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "/usr/include/stdlib.h" 2 3 4


// # 96 "/usr/include/stdlib.h" 3 4


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



// # 140 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__)) ;




extern double atof (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (__const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

// # 182 "/usr/include/stdlib.h" 3 4


extern long int strtol (__const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern unsigned long int strtoul (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int strtoq (__const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtouq (__const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





__extension__
extern long long int strtoll (__const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

// # 279 "/usr/include/stdlib.h" 3 4
extern double __strtod_internal (__const char *__restrict __nptr,
     char **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern float __strtof_internal (__const char *__restrict __nptr,
    char **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern long double __strtold_internal (__const char *__restrict __nptr,
           char **__restrict __endptr,
           int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern long int __strtol_internal (__const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern unsigned long int __strtoul_internal (__const char *__restrict __nptr,
          char **__restrict __endptr,
          int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int __strtoll_internal (__const char *__restrict __nptr,
      char **__restrict __endptr,
      int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



__extension__
extern unsigned long long int __strtoull_internal (__const char *
         __restrict __nptr,
         char **__restrict __endptr,
         int __base, int __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 429 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__)) ;


extern long int a64l (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




// # 1 "/usr/include/sys/types.h" 1 3 4
// # 29 "/usr/include/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
// # 62 "/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
// # 100 "/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;




typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
// # 133 "/usr/include/sys/types.h" 3 4
// # 1 "/usr/include/time.h" 1 3 4
// # 75 "/usr/include/time.h" 3 4


typedef __time_t time_t;



// # 93 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
// # 105 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
// # 134 "/usr/include/sys/types.h" 2 3 4
// # 147 "/usr/include/sys/types.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 148 "/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
// # 195 "/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
// # 217 "/usr/include/sys/types.h" 3 4
// # 1 "/usr/include/endian.h" 1 3 4
// # 37 "/usr/include/endian.h" 3 4
// # 1 "/usr/include/bits/endian.h" 1 3 4
// # 38 "/usr/include/endian.h" 2 3 4
// # 218 "/usr/include/sys/types.h" 2 3 4


// # 1 "/usr/include/sys/select.h" 1 3 4
// # 31 "/usr/include/sys/select.h" 3 4
// # 1 "/usr/include/bits/select.h" 1 3 4
// # 32 "/usr/include/sys/select.h" 2 3 4


// # 1 "/usr/include/bits/sigset.h" 1 3 4
// # 23 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
// # 35 "/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





// # 1 "/usr/include/time.h" 1 3 4
// # 121 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
// # 45 "/usr/include/sys/select.h" 2 3 4

// # 1 "/usr/include/bits/time.h" 1 3 4
// # 69 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
// # 47 "/usr/include/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
// # 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
// # 99 "/usr/include/sys/select.h" 3 4

// # 109 "/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
// # 121 "/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



// # 221 "/usr/include/sys/types.h" 2 3 4


// # 1 "/usr/include/sys/sysmacros.h" 1 3 4
// # 29 "/usr/include/sys/sysmacros.h" 3 4
__extension__
extern __inline unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern __inline unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern __inline unsigned long long int gnu_dev_makedev (unsigned int __major,
       unsigned int __minor)
     __attribute__ ((__nothrow__));


__extension__ extern __inline unsigned int
__attribute__ ((__nothrow__)) gnu_dev_major (unsigned long long int __dev)
{
  return ((__dev >> 8) & 0xfff) | ((unsigned int) (__dev >> 32) & ~0xfff);
}

__extension__ extern __inline unsigned int
__attribute__ ((__nothrow__)) gnu_dev_minor (unsigned long long int __dev)
{
  return (__dev & 0xff) | ((unsigned int) (__dev >> 12) & ~0xff);
}

__extension__ extern __inline unsigned long long int
__attribute__ ((__nothrow__)) gnu_dev_makedev (unsigned int __major, unsigned int __minor)
{
  return ((__minor & 0xff) | ((__major & 0xfff) << 8)
   | (((unsigned long long int) (__minor & ~0xff)) << 12)
   | (((unsigned long long int) (__major & ~0xfff)) << 32));
}
// # 224 "/usr/include/sys/types.h" 2 3 4
// # 235 "/usr/include/sys/types.h" 3 4
typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
// # 270 "/usr/include/sys/types.h" 3 4
// # 1 "/usr/include/bits/pthreadtypes.h" 1 3 4
// # 23 "/usr/include/bits/pthreadtypes.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 24 "/usr/include/bits/pthreadtypes.h" 2 3 4
// # 50 "/usr/include/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


typedef union
{
  char __size[56];
  long int __align;
} pthread_attr_t;



typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
// # 76 "/usr/include/bits/pthreadtypes.h" 3 4
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;

    unsigned int __nusers;



    int __kind;

    int __spins;
    __pthread_list_t __list;
// # 101 "/usr/include/bits/pthreadtypes.h" 3 4
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
// # 187 "/usr/include/bits/pthreadtypes.h" 3 4
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
// # 271 "/usr/include/sys/types.h" 2 3 4



// # 439 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







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
       int32_t *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__));







extern double drand48 (void) __attribute__ ((__nothrow__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;







extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__));



// # 1 "/usr/include/alloca.h" 1 3 4
// # 25 "/usr/include/alloca.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 26 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__));






// # 613 "/usr/include/stdlib.h" 2 3 4




extern void *valloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));

// # 658 "/usr/include/stdlib.h" 3 4


extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern char *__secure_getenv (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int putenv (char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (__const char *__name, __const char *__value, int __replace)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (__const char *__name) __attribute__ ((__nothrow__));






extern int clearenv (void) __attribute__ ((__nothrow__));
// # 698 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 709 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
// # 729 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;








extern int system (__const char *__command) ;

// # 756 "/usr/include/stdlib.h" 3 4
extern char *realpath (__const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__)) ;






typedef int (*__compar_fn_t) (__const void *, __const void *);









extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));



extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;












extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;

// # 821 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));







extern int mblen (__const char *__s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int mbtowc (wchar_t *__restrict __pwc,
     __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__)) ;



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__));

extern size_t wcstombs (char *__restrict __s,
   __const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__));








extern int rpmatch (__const char *__response) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 926 "/usr/include/stdlib.h" 3 4
extern int posix_openpt (int __oflag) ;
// # 961 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 977 "/usr/include/stdlib.h" 3 4

// # 23 "mutt.h" 2

// # 1 "/usr/include/unistd.h" 1 3 4
// # 28 "/usr/include/unistd.h" 3 4

// # 173 "/usr/include/unistd.h" 3 4
// # 1 "/usr/include/bits/posix_opt.h" 1 3 4
// # 174 "/usr/include/unistd.h" 2 3 4
// # 197 "/usr/include/unistd.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 198 "/usr/include/unistd.h" 2 3 4
// # 226 "/usr/include/unistd.h" 3 4
typedef __useconds_t useconds_t;
// # 238 "/usr/include/unistd.h" 3 4
typedef __intptr_t intptr_t;






typedef __socklen_t socklen_t;
// # 258 "/usr/include/unistd.h" 3 4
extern int access (__const char *__name, int __type) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 301 "/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__));
// # 320 "/usr/include/unistd.h" 3 4
extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, __const void *__buf, size_t __n) ;
// # 384 "/usr/include/unistd.h" 3 4
extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__)) ;
// # 393 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__));
// # 405 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);






extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__));






extern int usleep (__useconds_t __useconds);
// # 429 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__)) ;




extern int lchown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 457 "/usr/include/unistd.h" 3 4
extern int chdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__)) ;
// # 471 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__)) ;
// # 484 "/usr/include/unistd.h" 3 4
extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__));


extern char **__environ;







extern int execve (__const char *__path, char *__const __argv[],
     char *__const __envp[]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 516 "/usr/include/unistd.h" 3 4
extern int execv (__const char *__path, char *__const __argv[])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execle (__const char *__path, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execl (__const char *__path, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int execvp (__const char *__file, char *__const __argv[])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int execlp (__const char *__file, __const char *__arg, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int nice (int __inc) __attribute__ ((__nothrow__)) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));





// # 1 "/usr/include/bits/confname.h" 1 3 4
// # 26 "/usr/include/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,


    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS

  };
// # 555 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (__const char *__path, int __name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__));




extern __pid_t getpid (void) __attribute__ ((__nothrow__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__));




extern __pid_t getpgrp (void) __attribute__ ((__nothrow__));
// # 591 "/usr/include/unistd.h" 3 4
extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__));
// # 600 "/usr/include/unistd.h" 3 4
extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__));
// # 617 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__));
// # 634 "/usr/include/unistd.h" 3 4
extern __pid_t setsid (void) __attribute__ ((__nothrow__));







extern __uid_t getuid (void) __attribute__ ((__nothrow__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__)) ;
// # 667 "/usr/include/unistd.h" 3 4
extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__));




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__));




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__));






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__));




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__));




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__));
// # 723 "/usr/include/unistd.h" 3 4
extern __pid_t fork (void) __attribute__ ((__nothrow__));






extern __pid_t vfork (void) __attribute__ ((__nothrow__));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__));





extern int ttyslot (void) __attribute__ ((__nothrow__));




extern int link (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;
// # 769 "/usr/include/unistd.h" 3 4
extern int symlink (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (__const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;
// # 792 "/usr/include/unistd.h" 3 4
extern int unlink (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 801 "/usr/include/unistd.h" 3 4
extern int rmdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 837 "/usr/include/unistd.h" 3 4
// # 1 "/usr/include/getopt.h" 1 3 4
// # 59 "/usr/include/getopt.h" 3 4
extern char *optarg;
// # 73 "/usr/include/getopt.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
// # 152 "/usr/include/getopt.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__));
// # 838 "/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int sethostname (__const char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int sethostid (long int __id) __attribute__ ((__nothrow__)) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (__const char *__name, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int vhangup (void) __attribute__ ((__nothrow__));


extern int revoke (__const char *__file) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int acct (__const char *__name) __attribute__ ((__nothrow__));



extern char *getusershell (void) __attribute__ ((__nothrow__));
extern void endusershell (void) __attribute__ ((__nothrow__));
extern void setusershell (void) __attribute__ ((__nothrow__));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__)) ;






extern int chroot (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (__const char *__prompt) __attribute__ ((__nonnull__ (1)));
// # 923 "/usr/include/unistd.h" 3 4
extern int fsync (int __fd);






extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__));




extern int getpagesize (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__));




extern int truncate (__const char *__file, __off_t __length)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 970 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__)) ;
// # 990 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__));
// # 1011 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__));
// # 1034 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
// # 1065 "/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes);
// # 1103 "/usr/include/unistd.h" 3 4

// # 25 "mutt.h" 2





// # 1 "/usr/include/sys/stat.h" 1 3 4
// # 37 "/usr/include/sys/stat.h" 3 4
// # 1 "/usr/include/time.h" 1 3 4
// # 38 "/usr/include/sys/stat.h" 2 3 4
// # 103 "/usr/include/sys/stat.h" 3 4


// # 1 "/usr/include/bits/stat.h" 1 3 4
// # 43 "/usr/include/bits/stat.h" 3 4
struct stat
  {
    __dev_t st_dev;




    __ino_t st_ino;







    __nlink_t st_nlink;
    __mode_t st_mode;

    __uid_t st_uid;
    __gid_t st_gid;

    int pad0;

    __dev_t st_rdev;




    __off_t st_size;



    __blksize_t st_blksize;

    __blkcnt_t st_blocks;
// # 88 "/usr/include/bits/stat.h" 3 4
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
// # 103 "/usr/include/bits/stat.h" 3 4
    long int __unused[3];
// # 112 "/usr/include/bits/stat.h" 3 4
  };
// # 106 "/usr/include/sys/stat.h" 2 3 4
// # 207 "/usr/include/sys/stat.h" 3 4
extern int stat (__const char *__restrict __file,
   struct stat *__restrict __buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern int fstat (int __fd, struct stat *__buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
// # 259 "/usr/include/sys/stat.h" 3 4
extern int lstat (__const char *__restrict __file,
    struct stat *__restrict __buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
// # 280 "/usr/include/sys/stat.h" 3 4
extern int chmod (__const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int lchmod (__const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int fchmod (int __fd, __mode_t __mode) __attribute__ ((__nothrow__));
// # 307 "/usr/include/sys/stat.h" 3 4
extern __mode_t umask (__mode_t __mask) __attribute__ ((__nothrow__));
// # 316 "/usr/include/sys/stat.h" 3 4
extern int mkdir (__const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 331 "/usr/include/sys/stat.h" 3 4
extern int mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 345 "/usr/include/sys/stat.h" 3 4
extern int mkfifo (__const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 380 "/usr/include/sys/stat.h" 3 4
extern int __fxstat (int __ver, int __fildes, struct stat *__stat_buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3)));
extern int __xstat (int __ver, __const char *__filename,
      struct stat *__stat_buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat (int __ver, __const char *__filename,
       struct stat *__stat_buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat (int __ver, int __fildes, __const char *__filename,
         struct stat *__stat_buf, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4)));
// # 423 "/usr/include/sys/stat.h" 3 4
extern int __xmknod (int __ver, __const char *__path, __mode_t __mode,
       __dev_t *__dev) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));

extern int __xmknodat (int __ver, int __fd, __const char *__path,
         __mode_t __mode, __dev_t *__dev)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 5)));




extern __inline__ int
__attribute__ ((__nothrow__)) stat (__const char *__path, struct stat *__statbuf)
{
  return __xstat (1, __path, __statbuf);
}


extern __inline__ int
__attribute__ ((__nothrow__)) lstat (__const char *__path, struct stat *__statbuf)
{
  return __lxstat (1, __path, __statbuf);
}


extern __inline__ int
__attribute__ ((__nothrow__)) fstat (int __fd, struct stat *__statbuf)
{
  return __fxstat (1, __fd, __statbuf);
}
// # 463 "/usr/include/sys/stat.h" 3 4
extern __inline__ int
__attribute__ ((__nothrow__)) mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
{
  return __xmknod (0, __path, __mode, &__dev);
}
// # 515 "/usr/include/sys/stat.h" 3 4

// # 31 "mutt.h" 2
// # 1 "/usr/include/time.h" 1 3 4
// # 31 "/usr/include/time.h" 3 4








// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 40 "/usr/include/time.h" 2 3 4



// # 1 "/usr/include/bits/time.h" 1 3 4
// # 44 "/usr/include/time.h" 2 3 4
// # 59 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



// # 132 "/usr/include/time.h" 3 4


struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;


  long int tm_gmtoff;
  __const char *tm_zone;




};








struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };


struct sigevent;
// # 181 "/usr/include/time.h" 3 4



extern clock_t clock (void) __attribute__ ((__nothrow__));


extern time_t time (time_t *__timer) __attribute__ ((__nothrow__));


extern double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));


extern time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__));





extern size_t strftime (char *__restrict __s, size_t __maxsize,
   __const char *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

// # 229 "/usr/include/time.h" 3 4



extern struct tm *gmtime (__const time_t *__timer) __attribute__ ((__nothrow__));



extern struct tm *localtime (__const time_t *__timer) __attribute__ ((__nothrow__));





extern struct tm *gmtime_r (__const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__));



extern struct tm *localtime_r (__const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__));





extern char *asctime (__const struct tm *__tp) __attribute__ ((__nothrow__));


extern char *ctime (__const time_t *__timer) __attribute__ ((__nothrow__));







extern char *asctime_r (__const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__));


extern char *ctime_r (__const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__));




extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;




extern char *tzname[2];



extern void tzset (void) __attribute__ ((__nothrow__));



extern int daylight;
extern long int timezone;





extern int stime (__const time_t *__when) __attribute__ ((__nothrow__));
// # 312 "/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__));


extern int dysize (int __year) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
// # 327 "/usr/include/time.h" 3 4
extern int nanosleep (__const struct timespec *__requested_time,
        struct timespec *__remaining);



extern int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__));


extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__));


extern int clock_settime (clockid_t __clock_id, __const struct timespec *__tp)
     __attribute__ ((__nothrow__));






extern int clock_nanosleep (clockid_t __clock_id, int __flags,
       __const struct timespec *__req,
       struct timespec *__rem);


extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__));




extern int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__));


extern int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__));


extern int timer_settime (timer_t __timerid, int __flags,
     __const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__));


extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__));


extern int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__));
// # 416 "/usr/include/time.h" 3 4

// # 32 "mutt.h" 2
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 11 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/syslimits.h" 1 3 4






// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 122 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 3 4
// # 1 "/usr/include/limits.h" 1 3 4
// # 145 "/usr/include/limits.h" 3 4
// # 1 "/usr/include/bits/posix1_lim.h" 1 3 4
// # 153 "/usr/include/bits/posix1_lim.h" 3 4
// # 1 "/usr/include/bits/local_lim.h" 1 3 4
// # 36 "/usr/include/bits/local_lim.h" 3 4
// # 1 "/usr/include/linux/limits.h" 1 3 4
// # 37 "/usr/include/bits/local_lim.h" 2 3 4
// # 154 "/usr/include/bits/posix1_lim.h" 2 3 4
// # 146 "/usr/include/limits.h" 2 3 4



// # 1 "/usr/include/bits/posix2_lim.h" 1 3 4
// # 150 "/usr/include/limits.h" 2 3 4
// # 123 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 2 3 4
// # 8 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/syslimits.h" 2 3 4
// # 12 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 2 3 4
// # 33 "mutt.h" 2
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 105 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
// # 34 "mutt.h" 2
// # 1 "/usr/include/signal.h" 1 3 4
// # 31 "/usr/include/signal.h" 3 4


// # 1 "/usr/include/bits/sigset.h" 1 3 4
// # 103 "/usr/include/bits/sigset.h" 3 4
extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
// # 34 "/usr/include/signal.h" 2 3 4







typedef __sig_atomic_t sig_atomic_t;

// # 58 "/usr/include/signal.h" 3 4
// # 1 "/usr/include/bits/signum.h" 1 3 4
// # 59 "/usr/include/signal.h" 2 3 4
// # 75 "/usr/include/signal.h" 3 4
typedef void (*__sighandler_t) (int);




extern __sighandler_t __sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
// # 90 "/usr/include/signal.h" 3 4


extern __sighandler_t signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
// # 104 "/usr/include/signal.h" 3 4

// # 117 "/usr/include/signal.h" 3 4
extern int kill (__pid_t __pid, int __sig) __attribute__ ((__nothrow__));






extern int killpg (__pid_t __pgrp, int __sig) __attribute__ ((__nothrow__));




extern int raise (int __sig) __attribute__ ((__nothrow__));




extern __sighandler_t ssignal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
extern int gsignal (int __sig) __attribute__ ((__nothrow__));




extern void psignal (int __sig, __const char *__s);
// # 153 "/usr/include/signal.h" 3 4
extern int __sigpause (int __sig_or_mask, int __is_sig);
// # 181 "/usr/include/signal.h" 3 4
extern int sigblock (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int sigsetmask (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int siggetmask (void) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
// # 201 "/usr/include/signal.h" 3 4
typedef __sighandler_t sig_t;
// # 212 "/usr/include/signal.h" 3 4
// # 1 "/usr/include/bits/siginfo.h" 1 3 4
// # 25 "/usr/include/bits/siginfo.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 26 "/usr/include/bits/siginfo.h" 2 3 4







typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
// # 51 "/usr/include/bits/siginfo.h" 3 4
typedef struct siginfo
  {
    int si_signo;
    int si_errno;

    int si_code;

    union
      {
 int _pad[((128 / sizeof (int)) - 4)];


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
   } _kill;


 struct
   {
     int si_tid;
     int si_overrun;
     sigval_t si_sigval;
   } _timer;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     sigval_t si_sigval;
   } _rt;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     int si_status;
     __clock_t si_utime;
     __clock_t si_stime;
   } _sigchld;


 struct
   {
     void *si_addr;
   } _sigfault;


 struct
   {
     long int si_band;
     int si_fd;
   } _sigpoll;
      } _sifields;
  } siginfo_t;
// # 129 "/usr/include/bits/siginfo.h" 3 4
enum
{
  SI_ASYNCNL = -60,

  SI_TKILL = -6,

  SI_SIGIO,

  SI_ASYNCIO,

  SI_MESGQ,

  SI_TIMER,

  SI_QUEUE,

  SI_USER,

  SI_KERNEL = 0x80

};



enum
{
  ILL_ILLOPC = 1,

  ILL_ILLOPN,

  ILL_ILLADR,

  ILL_ILLTRP,

  ILL_PRVOPC,

  ILL_PRVREG,

  ILL_COPROC,

  ILL_BADSTK

};


enum
{
  FPE_INTDIV = 1,

  FPE_INTOVF,

  FPE_FLTDIV,

  FPE_FLTOVF,

  FPE_FLTUND,

  FPE_FLTRES,

  FPE_FLTINV,

  FPE_FLTSUB

};


enum
{
  SEGV_MAPERR = 1,

  SEGV_ACCERR

};


enum
{
  BUS_ADRALN = 1,

  BUS_ADRERR,

  BUS_OBJERR

};


enum
{
  TRAP_BRKPT = 1,

  TRAP_TRACE

};


enum
{
  CLD_EXITED = 1,

  CLD_KILLED,

  CLD_DUMPED,

  CLD_TRAPPED,

  CLD_STOPPED,

  CLD_CONTINUED

};


enum
{
  POLL_IN = 1,

  POLL_OUT,

  POLL_MSG,

  POLL_ERR,

  POLL_PRI,

  POLL_HUP

};
// # 273 "/usr/include/bits/siginfo.h" 3 4
typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;

    union
      {
 int _pad[((64 / sizeof (int)) - 4)];



 __pid_t _tid;

 struct
   {
     void (*_function) (sigval_t);
     void *_attribute;
   } _sigev_thread;
      } _sigev_un;
  } sigevent_t;






enum
{
  SIGEV_SIGNAL = 0,

  SIGEV_NONE,

  SIGEV_THREAD,


  SIGEV_THREAD_ID = 4

};
// # 213 "/usr/include/signal.h" 2 3 4



extern int sigemptyset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigfillset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigaddset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigdelset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigismember (__const sigset_t *__set, int __signo)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 246 "/usr/include/signal.h" 3 4
// # 1 "/usr/include/bits/sigaction.h" 1 3 4
// # 25 "/usr/include/bits/sigaction.h" 3 4
struct sigaction
  {


    union
      {

 __sighandler_t sa_handler;

 void (*sa_sigaction) (int, siginfo_t *, void *);
      }
    __sigaction_handler;







    __sigset_t sa_mask;


    int sa_flags;


    void (*sa_restorer) (void);
  };
// # 247 "/usr/include/signal.h" 2 3 4


extern int sigprocmask (int __how, __const sigset_t *__restrict __set,
   sigset_t *__restrict __oset) __attribute__ ((__nothrow__));






extern int sigsuspend (__const sigset_t *__set) __attribute__ ((__nonnull__ (1)));


extern int sigaction (int __sig, __const struct sigaction *__restrict __act,
        struct sigaction *__restrict __oact) __attribute__ ((__nothrow__));


extern int sigpending (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int sigwait (__const sigset_t *__restrict __set, int *__restrict __sig)
     __attribute__ ((__nonnull__ (1, 2)));






extern int sigwaitinfo (__const sigset_t *__restrict __set,
   siginfo_t *__restrict __info) __attribute__ ((__nonnull__ (1)));






extern int sigtimedwait (__const sigset_t *__restrict __set,
    siginfo_t *__restrict __info,
    __const struct timespec *__restrict __timeout)
     __attribute__ ((__nonnull__ (1)));



extern int sigqueue (__pid_t __pid, int __sig, __const union sigval __val)
     __attribute__ ((__nothrow__));
// # 304 "/usr/include/signal.h" 3 4
extern __const char *__const _sys_siglist[65];
extern __const char *__const sys_siglist[65];


struct sigvec
  {
    __sighandler_t sv_handler;
    int sv_mask;

    int sv_flags;

  };
// # 328 "/usr/include/signal.h" 3 4
extern int sigvec (int __sig, __const struct sigvec *__vec,
     struct sigvec *__ovec) __attribute__ ((__nothrow__));



// # 1 "/usr/include/bits/sigcontext.h" 1 3 4
// # 26 "/usr/include/bits/sigcontext.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 27 "/usr/include/bits/sigcontext.h" 2 3 4

struct _fpreg
{
  unsigned short significand[4];
  unsigned short exponent;
};

struct _fpxreg
{
  unsigned short significand[4];
  unsigned short exponent;
  unsigned short padding[3];
};

struct _xmmreg
{
  __uint32_t element[4];
};
// # 109 "/usr/include/bits/sigcontext.h" 3 4
struct _fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _fpxreg _st[8];
  struct _xmmreg _xmm[16];
  __uint32_t padding[24];
};

struct sigcontext
{
  unsigned long r8;
  unsigned long r9;
  unsigned long r10;
  unsigned long r11;
  unsigned long r12;
  unsigned long r13;
  unsigned long r14;
  unsigned long r15;
  unsigned long rdi;
  unsigned long rsi;
  unsigned long rbp;
  unsigned long rbx;
  unsigned long rdx;
  unsigned long rax;
  unsigned long rcx;
  unsigned long rsp;
  unsigned long rip;
  unsigned long eflags;
  unsigned short cs;
  unsigned short gs;
  unsigned short fs;
  unsigned short __pad0;
  unsigned long err;
  unsigned long trapno;
  unsigned long oldmask;
  unsigned long cr2;
  struct _fpstate * fpstate;
  unsigned long __reserved1 [8];
};
// # 334 "/usr/include/signal.h" 2 3 4


extern int sigreturn (struct sigcontext *__scp) __attribute__ ((__nothrow__));
// # 346 "/usr/include/signal.h" 3 4
extern int siginterrupt (int __sig, int __interrupt) __attribute__ ((__nothrow__));

// # 1 "/usr/include/bits/sigstack.h" 1 3 4
// # 26 "/usr/include/bits/sigstack.h" 3 4
struct sigstack
  {
    void *ss_sp;
    int ss_onstack;
  };



enum
{
  SS_ONSTACK = 1,

  SS_DISABLE

};
// # 50 "/usr/include/bits/sigstack.h" 3 4
typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
// # 349 "/usr/include/signal.h" 2 3 4
// # 357 "/usr/include/signal.h" 3 4
extern int sigstack (struct sigstack *__ss, struct sigstack *__oss)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));



extern int sigaltstack (__const struct sigaltstack *__restrict __ss,
   struct sigaltstack *__restrict __oss) __attribute__ ((__nothrow__));
// # 387 "/usr/include/signal.h" 3 4
// # 1 "/usr/include/bits/sigthread.h" 1 3 4
// # 31 "/usr/include/bits/sigthread.h" 3 4
extern int pthread_sigmask (int __how,
       __const __sigset_t *__restrict __newmask,
       __sigset_t *__restrict __oldmask)__attribute__ ((__nothrow__));


extern int pthread_kill (pthread_t __threadid, int __signo) __attribute__ ((__nothrow__));
// # 388 "/usr/include/signal.h" 2 3 4






extern int __libc_current_sigrtmin (void) __attribute__ ((__nothrow__));

extern int __libc_current_sigrtmax (void) __attribute__ ((__nothrow__));




// # 35 "mutt.h" 2

// # 1 "/usr/include/wchar.h" 1 3 4
// # 48 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 49 "/usr/include/wchar.h" 2 3 4
// # 93 "/usr/include/wchar.h" 3 4


typedef __mbstate_t mbstate_t;

// # 118 "/usr/include/wchar.h" 3 4





struct tm;









extern wchar_t *wcscpy (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncpy (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wcscat (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncat (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern int wcscmp (__const wchar_t *__s1, __const wchar_t *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern int wcsncmp (__const wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

// # 175 "/usr/include/wchar.h" 3 4



extern int wcscoll (__const wchar_t *__s1, __const wchar_t *__s2) __attribute__ ((__nothrow__));



extern size_t wcsxfrm (wchar_t *__restrict __s1,
         __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));

// # 205 "/usr/include/wchar.h" 3 4


extern wchar_t *wcschr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcsrchr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

// # 221 "/usr/include/wchar.h" 3 4



extern size_t wcscspn (__const wchar_t *__wcs, __const wchar_t *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern size_t wcsspn (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcspbrk (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcsstr (__const wchar_t *__haystack, __const wchar_t *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern wchar_t *wcstok (wchar_t *__restrict __s,
   __const wchar_t *__restrict __delim,
   wchar_t **__restrict __ptr) __attribute__ ((__nothrow__));


extern size_t wcslen (__const wchar_t *__s) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

// # 259 "/usr/include/wchar.h" 3 4


extern wchar_t *wmemchr (__const wchar_t *__s, wchar_t __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern int wmemcmp (__const wchar_t *__restrict __s1,
      __const wchar_t *__restrict __s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern wchar_t *wmemcpy (wchar_t *__restrict __s1,
    __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));



extern wchar_t *wmemmove (wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wmemset (wchar_t *__s, wchar_t __c, size_t __n) __attribute__ ((__nothrow__));

// # 291 "/usr/include/wchar.h" 3 4



extern wint_t btowc (int __c) __attribute__ ((__nothrow__));



extern int wctob (wint_t __c) __attribute__ ((__nothrow__));



extern int mbsinit (__const mbstate_t *__ps) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));



extern size_t mbrtowc (wchar_t *__restrict __pwc,
         __const char *__restrict __s, size_t __n,
         mbstate_t *__p) __attribute__ ((__nothrow__));


extern size_t wcrtomb (char *__restrict __s, wchar_t __wc,
         mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));


extern size_t __mbrlen (__const char *__restrict __s, size_t __n,
   mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));
extern size_t mbrlen (__const char *__restrict __s, size_t __n,
        mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

// # 348 "/usr/include/wchar.h" 3 4



extern size_t mbsrtowcs (wchar_t *__restrict __dst,
    __const char **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));



extern size_t wcsrtombs (char *__restrict __dst,
    __const wchar_t **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

// # 390 "/usr/include/wchar.h" 3 4



extern double wcstod (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));
// # 407 "/usr/include/wchar.h" 3 4
extern long int wcstol (__const wchar_t *__restrict __nptr,
   wchar_t **__restrict __endptr, int __base) __attribute__ ((__nothrow__));



extern unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
      wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));
// # 431 "/usr/include/wchar.h" 3 4

// # 502 "/usr/include/wchar.h" 3 4
extern double __wcstod_internal (__const wchar_t *__restrict __nptr,
     wchar_t **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__));
extern float __wcstof_internal (__const wchar_t *__restrict __nptr,
    wchar_t **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__));
extern long double __wcstold_internal (__const wchar_t *__restrict __nptr,
           wchar_t **__restrict __endptr,
           int __group) __attribute__ ((__nothrow__));
// # 707 "/usr/include/wchar.h" 3 4





extern wint_t fgetwc (__FILE *__stream);
extern wint_t getwc (__FILE *__stream);





extern wint_t getwchar (void);






extern wint_t fputwc (wchar_t __wc, __FILE *__stream);
extern wint_t putwc (wchar_t __wc, __FILE *__stream);





extern wint_t putwchar (wchar_t __wc);







extern wchar_t *fgetws (wchar_t *__restrict __ws, int __n,
   __FILE *__restrict __stream);





extern int fputws (__const wchar_t *__restrict __ws,
     __FILE *__restrict __stream);






extern wint_t ungetwc (wint_t __wc, __FILE *__stream);

// # 818 "/usr/include/wchar.h" 3 4




extern size_t wcsftime (wchar_t *__restrict __s, size_t __maxsize,
   __const wchar_t *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

// # 858 "/usr/include/wchar.h" 3 4

// # 37 "mutt.h" 2


// # 1 "/usr/include/wctype.h" 1 3 4
// # 35 "/usr/include/wctype.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 36 "/usr/include/wctype.h" 2 3 4
// # 63 "/usr/include/wctype.h" 3 4



typedef unsigned long int wctype_t;

// # 85 "/usr/include/wctype.h" 3 4
enum
{
  __ISwupper = 0,
  __ISwlower = 1,
  __ISwalpha = 2,
  __ISwdigit = 3,
  __ISwxdigit = 4,
  __ISwspace = 5,
  __ISwprint = 6,
  __ISwgraph = 7,
  __ISwblank = 8,
  __ISwcntrl = 9,
  __ISwpunct = 10,
  __ISwalnum = 11,

  _ISwupper = ((__ISwupper) < 8 ? (int) ((1UL << (__ISwupper)) << 24) : ((__ISwupper) < 16 ? (int) ((1UL << (__ISwupper)) << 8) : ((__ISwupper) < 24 ? (int) ((1UL << (__ISwupper)) >> 8) : (int) ((1UL << (__ISwupper)) >> 24)))),
  _ISwlower = ((__ISwlower) < 8 ? (int) ((1UL << (__ISwlower)) << 24) : ((__ISwlower) < 16 ? (int) ((1UL << (__ISwlower)) << 8) : ((__ISwlower) < 24 ? (int) ((1UL << (__ISwlower)) >> 8) : (int) ((1UL << (__ISwlower)) >> 24)))),
  _ISwalpha = ((__ISwalpha) < 8 ? (int) ((1UL << (__ISwalpha)) << 24) : ((__ISwalpha) < 16 ? (int) ((1UL << (__ISwalpha)) << 8) : ((__ISwalpha) < 24 ? (int) ((1UL << (__ISwalpha)) >> 8) : (int) ((1UL << (__ISwalpha)) >> 24)))),
  _ISwdigit = ((__ISwdigit) < 8 ? (int) ((1UL << (__ISwdigit)) << 24) : ((__ISwdigit) < 16 ? (int) ((1UL << (__ISwdigit)) << 8) : ((__ISwdigit) < 24 ? (int) ((1UL << (__ISwdigit)) >> 8) : (int) ((1UL << (__ISwdigit)) >> 24)))),
  _ISwxdigit = ((__ISwxdigit) < 8 ? (int) ((1UL << (__ISwxdigit)) << 24) : ((__ISwxdigit) < 16 ? (int) ((1UL << (__ISwxdigit)) << 8) : ((__ISwxdigit) < 24 ? (int) ((1UL << (__ISwxdigit)) >> 8) : (int) ((1UL << (__ISwxdigit)) >> 24)))),
  _ISwspace = ((__ISwspace) < 8 ? (int) ((1UL << (__ISwspace)) << 24) : ((__ISwspace) < 16 ? (int) ((1UL << (__ISwspace)) << 8) : ((__ISwspace) < 24 ? (int) ((1UL << (__ISwspace)) >> 8) : (int) ((1UL << (__ISwspace)) >> 24)))),
  _ISwprint = ((__ISwprint) < 8 ? (int) ((1UL << (__ISwprint)) << 24) : ((__ISwprint) < 16 ? (int) ((1UL << (__ISwprint)) << 8) : ((__ISwprint) < 24 ? (int) ((1UL << (__ISwprint)) >> 8) : (int) ((1UL << (__ISwprint)) >> 24)))),
  _ISwgraph = ((__ISwgraph) < 8 ? (int) ((1UL << (__ISwgraph)) << 24) : ((__ISwgraph) < 16 ? (int) ((1UL << (__ISwgraph)) << 8) : ((__ISwgraph) < 24 ? (int) ((1UL << (__ISwgraph)) >> 8) : (int) ((1UL << (__ISwgraph)) >> 24)))),
  _ISwblank = ((__ISwblank) < 8 ? (int) ((1UL << (__ISwblank)) << 24) : ((__ISwblank) < 16 ? (int) ((1UL << (__ISwblank)) << 8) : ((__ISwblank) < 24 ? (int) ((1UL << (__ISwblank)) >> 8) : (int) ((1UL << (__ISwblank)) >> 24)))),
  _ISwcntrl = ((__ISwcntrl) < 8 ? (int) ((1UL << (__ISwcntrl)) << 24) : ((__ISwcntrl) < 16 ? (int) ((1UL << (__ISwcntrl)) << 8) : ((__ISwcntrl) < 24 ? (int) ((1UL << (__ISwcntrl)) >> 8) : (int) ((1UL << (__ISwcntrl)) >> 24)))),
  _ISwpunct = ((__ISwpunct) < 8 ? (int) ((1UL << (__ISwpunct)) << 24) : ((__ISwpunct) < 16 ? (int) ((1UL << (__ISwpunct)) << 8) : ((__ISwpunct) < 24 ? (int) ((1UL << (__ISwpunct)) >> 8) : (int) ((1UL << (__ISwpunct)) >> 24)))),
  _ISwalnum = ((__ISwalnum) < 8 ? (int) ((1UL << (__ISwalnum)) << 24) : ((__ISwalnum) < 16 ? (int) ((1UL << (__ISwalnum)) << 8) : ((__ISwalnum) < 24 ? (int) ((1UL << (__ISwalnum)) >> 8) : (int) ((1UL << (__ISwalnum)) >> 24))))
};












extern int iswalnum (wint_t __wc) __attribute__ ((__nothrow__));





extern int iswalpha (wint_t __wc) __attribute__ ((__nothrow__));


extern int iswcntrl (wint_t __wc) __attribute__ ((__nothrow__));



extern int iswdigit (wint_t __wc) __attribute__ ((__nothrow__));



extern int iswgraph (wint_t __wc) __attribute__ ((__nothrow__));




extern int iswlower (wint_t __wc) __attribute__ ((__nothrow__));


extern int iswprint (wint_t __wc) __attribute__ ((__nothrow__));




extern int iswpunct (wint_t __wc) __attribute__ ((__nothrow__));




extern int iswspace (wint_t __wc) __attribute__ ((__nothrow__));




extern int iswupper (wint_t __wc) __attribute__ ((__nothrow__));




extern int iswxdigit (wint_t __wc) __attribute__ ((__nothrow__));
// # 185 "/usr/include/wctype.h" 3 4
extern wctype_t wctype (__const char *__property) __attribute__ ((__nothrow__));



extern int iswctype (wint_t __wc, wctype_t __desc) __attribute__ ((__nothrow__));










typedef __const __int32_t *wctrans_t;







extern wint_t towlower (wint_t __wc) __attribute__ ((__nothrow__));


extern wint_t towupper (wint_t __wc) __attribute__ ((__nothrow__));



// # 227 "/usr/include/wctype.h" 3 4





extern wctrans_t wctrans (__const char *__property) __attribute__ ((__nothrow__));


extern wint_t towctrans (wint_t __wc, wctrans_t __desc) __attribute__ ((__nothrow__));

// # 328 "/usr/include/wctype.h" 3 4

// # 40 "mutt.h" 2






// # 1 "/usr/include/pwd.h" 1 3 4
// # 28 "/usr/include/pwd.h" 3 4





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "/usr/include/pwd.h" 2 3 4
// # 50 "/usr/include/pwd.h" 3 4
struct passwd
{
  char *pw_name;
  char *pw_passwd;
  __uid_t pw_uid;
  __gid_t pw_gid;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};
// # 73 "/usr/include/pwd.h" 3 4
extern void setpwent (void);





extern void endpwent (void);





extern struct passwd *getpwent (void);
// # 95 "/usr/include/pwd.h" 3 4
extern struct passwd *fgetpwent (FILE *__stream);







extern int putpwent (__const struct passwd *__restrict __p,
       FILE *__restrict __f);






extern struct passwd *getpwuid (__uid_t __uid);





extern struct passwd *getpwnam (__const char *__name);
// # 140 "/usr/include/pwd.h" 3 4
extern int getpwent_r (struct passwd *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct passwd **__restrict __result);


extern int getpwuid_r (__uid_t __uid,
         struct passwd *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct passwd **__restrict __result);

extern int getpwnam_r (__const char *__restrict __name,
         struct passwd *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct passwd **__restrict __result);
// # 164 "/usr/include/pwd.h" 3 4
extern int fgetpwent_r (FILE *__restrict __stream,
   struct passwd *__restrict __resultbuf,
   char *__restrict __buffer, size_t __buflen,
   struct passwd **__restrict __result);
// # 184 "/usr/include/pwd.h" 3 4

// # 47 "mutt.h" 2
// # 1 "/usr/include/grp.h" 1 3 4
// # 28 "/usr/include/grp.h" 3 4





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "/usr/include/grp.h" 2 3 4
// # 43 "/usr/include/grp.h" 3 4
struct group
  {
    char *gr_name;
    char *gr_passwd;
    __gid_t gr_gid;
    char **gr_mem;
  };
// # 63 "/usr/include/grp.h" 3 4
extern void setgrent (void);





extern void endgrent (void);





extern struct group *getgrent (void);
// # 85 "/usr/include/grp.h" 3 4
extern struct group *fgetgrent (FILE *__stream);
// # 103 "/usr/include/grp.h" 3 4
extern struct group *getgrgid (__gid_t __gid);





extern struct group *getgrnam (__const char *__name);
// # 142 "/usr/include/grp.h" 3 4
extern int getgrgid_r (__gid_t __gid, struct group *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct group **__restrict __result);





extern int getgrnam_r (__const char *__restrict __name,
         struct group *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct group **__restrict __result);
// # 163 "/usr/include/grp.h" 3 4
extern int fgetgrent_r (FILE *__restrict __stream,
   struct group *__restrict __resultbuf,
   char *__restrict __buffer, size_t __buflen,
   struct group **__restrict __result);
// # 175 "/usr/include/grp.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 176 "/usr/include/grp.h" 2 3 4


extern int setgroups (size_t __n, __const __gid_t *__groups) __attribute__ ((__nothrow__));
// # 188 "/usr/include/grp.h" 3 4
extern int getgrouplist (__const char *__user, __gid_t __group,
    __gid_t *__groups, int *__ngroups);
// # 199 "/usr/include/grp.h" 3 4
extern int initgroups (__const char *__user, __gid_t __group);




// # 48 "mutt.h" 2

// # 1 "rfc822.h" 1
// # 22 "rfc822.h"
// # 1 "config.h" 1
// # 23 "rfc822.h" 2


enum
{
  ERR_MEMORY = 1,
  ERR_MISMATCH_PAREN,
  ERR_MISMATCH_QUOTE,
  ERR_BAD_ROUTE,
  ERR_BAD_ROUTE_ADDR,
  ERR_BAD_ADDR_SPEC
};

typedef struct address_t
{



  char *personal;
  char *mailbox;
  int group;
  struct address_t *next;
}
ADDRESS;

void rfc822_free_address (ADDRESS **);
void rfc822_qualify (ADDRESS *, const char *);
ADDRESS *rfc822_parse_adrlist (ADDRESS *, const char *s);
ADDRESS *rfc822_cpy_adr (ADDRESS *addr);
ADDRESS *rfc822_cpy_adr_real (ADDRESS *addr);
ADDRESS *rfc822_append (ADDRESS **a, ADDRESS *b);
void rfc822_write_address (char *, size_t, ADDRESS *);
void rfc822_write_address_single (char *, size_t, ADDRESS *);
void rfc822_write_list (char *, size_t, ADDRESS *);
void rfc822_free_address (ADDRESS **addr);
void rfc822_cat (char *, size_t, const char *, const char *);
int rfc822_valid_msgid (const char *msgid);

extern int RFC822Error;
extern const char *RFC822Errors[];
// # 50 "mutt.h" 2
// # 1 "hash.h" 1
// # 22 "hash.h"
struct hash_elem
{
  const char *key;
  void *data;
  struct hash_elem *next;
};

typedef struct
{
  int nelem;
  struct hash_elem **table;
}
HASH;





HASH *hash_create (int nelem);
int hash_string (const unsigned char *s, int n);
int hash_insert (HASH * table, const char *key, void *data, int allow_dup);
void *hash_find_hash (const HASH * table, int hash, const char *key);
void hash_delete_hash (HASH * table, int hash, const char *key, const void *data,
         void (*destroy) (void *));
void hash_destroy (HASH ** hash, void (*destroy) (void *));
// # 51 "mutt.h" 2
// # 1 "charset.h" 1
// # 23 "charset.h"
// # 1 "/usr/include/iconv.h" 1 3 4
// # 24 "/usr/include/iconv.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 25 "/usr/include/iconv.h" 2 3 4





typedef void *iconv_t;







extern iconv_t iconv_open (__const char *__tocode, __const char *__fromcode);




extern size_t iconv (iconv_t __cd, char **__restrict __inbuf,
       size_t *__restrict __inbytesleft,
       char **__restrict __outbuf,
       size_t *__restrict __outbytesleft);





extern int iconv_close (iconv_t __cd);


// # 24 "charset.h" 2
// # 37 "charset.h"
int mutt_convert_string (char **, const char *, const char *, int);

iconv_t mutt_iconv_open (const char *, const char *, int);
size_t mutt_iconv (iconv_t, char **, size_t *, char **, size_t *, char **, const char *);

typedef void * FGETCONV;

FGETCONV *fgetconv_open (FILE *, const char *, const char *, int);
int fgetconv (FGETCONV *);
void fgetconv_close (FGETCONV **);

void mutt_set_langinfo_charset (void);
// # 52 "mutt.h" 2
// # 111 "mutt.h"
typedef struct
{
  char *data;
  char *dptr;
  size_t dsize;
  int destroy;
} BUFFER;

typedef struct
{
  int ch;
  int op;
} event_t;





typedef enum
{
  M_FORMAT_FORCESUBJ = (1<<0),
  M_FORMAT_TREE = (1<<1),
  M_FORMAT_MAKEPRINT = (1<<2),
  M_FORMAT_OPTIONAL = (1<<3),
  M_FORMAT_STAT_FILE = (1<<4),
  M_FORMAT_ARROWCURSOR = (1<<5),
  M_FORMAT_INDEX = (1<<6)
} format_flag;
// # 176 "mutt.h"
enum
{

  M_REGULAR = 1,
  M_MAILCAP,
  M_AS_TEXT,


  M_ALL,
  M_NONE,
  M_NEW,
  M_OLD,
  M_REPLIED,
  M_READ,
  M_UNREAD,
  M_DELETE,
  M_UNDELETE,
  M_DELETED,
  M_FLAG,
  M_TAG,
  M_UNTAG,
  M_LIMIT,
  M_EXPIRED,
  M_SUPERSEDED,


  M_AND,
  M_OR,
  M_TO,
  M_CC,
  M_COLLAPSED,
  M_SUBJECT,
  M_FROM,
  M_DATE,
  M_DATE_RECEIVED,
  M_DUPLICATED,
  M_ID,
  M_BODY,
  M_HEADER,
  M_WHOLE_MSG,
  M_SENDER,
  M_MESSAGE,
  M_SCORE,
  M_SIZE,
  M_REFERENCE,
  M_RECIPIENT,
  M_LIST,
  M_PERSONAL_RECIP,
  M_PERSONAL_FROM,
  M_ADDRESS,

  M_PGP_SIGN,
  M_PGP_ENCRYPT,
  M_PGP_KEY,

  M_XLABEL,


  M_EDIT,
  M_COMPOSE,
  M_PRINT,
  M_AUTOVIEW,


  M_NEW_SOCKET,





  M_SAVE_APPEND,
  M_SAVE_OVERWRITE
};


enum
{
  M_NO,
  M_YES,
  M_ASKNO,
  M_ASKYES
};


enum
{


  OPT_VERIFYSIG,
  OPT_PGPTRADITIONAL,






  OPT_PRINT,
  OPT_INCLUDE,
  OPT_DELETE,
  OPT_MFUPTO,
  OPT_MIMEFWD,
  OPT_MOVE,
  OPT_COPY,




  OPT_POSTPONE,
  OPT_QUIT,
  OPT_REPLYTO,
  OPT_ABORT,
  OPT_RECALL,
  OPT_SUBJECT,
  OPT_MIMEFWDREST,
  OPT_MAX
};
// # 311 "mutt.h"
enum
{
  OPTALLOW8BIT,
  OPTALLOWANSI,
  OPTARROWCURSOR,
  OPTASCIICHARS,
  OPTASKBCC,
  OPTASKCC,
  OPTATTACHSPLIT,
  OPTAUTOEDIT,
  OPTAUTOTAG,
  OPTBEEP,
  OPTBEEPNEW,
  OPTBOUNCEDELIVERED,
  OPTCHECKNEW,
  OPTCOLLAPSEUNREAD,
  OPTCONFIRMAPPEND,
  OPTCONFIRMCREATE,
  OPTDELETEUNTAG,
  OPTDIGESTCOLLAPSE,
  OPTDUPTHREADS,
  OPTEDITHDRS,
  OPTENCODEFROM,
  OPTENVFROM,
  OPTFASTREPLY,
  OPTFCCATTACH,
  OPTFCCCLEAR,
  OPTFOLLOWUPTO,
  OPTFORCENAME,
  OPTFORWDECODE,
  OPTFORWQUOTE,
  OPTHDRS,
  OPTHEADER,
  OPTHELP,
  OPTHIDDENHOST,
  OPTHIDELIMITED,
  OPTHIDEMISSING,
  OPTHIDETOPLIMITED,
  OPTHIDETOPMISSING,
  OPTIGNORELISTREPLYTO,
// # 366 "mutt.h"
  OPTIMPLICITAUTOVIEW,
  OPTKEEPFLAGGED,
  OPTMAILCAPSANITIZE,
  OPTMAILDIRTRASH,
  OPTMARKERS,
  OPTMARKOLD,
  OPTMENUSCROLL,
  OPTMETAKEY,
  OPTMETOO,
  OPTMHPURGE,
  OPTMIMEFORWDECODE,
  OPTPAGERSTOP,
  OPTPIPEDECODE,
  OPTPIPESPLIT,




  OPTPRINTDECODE,
  OPTPRINTSPLIT,
  OPTPROMPTAFTER,
  OPTREADONLY,
  OPTREPLYSELF,
  OPTRESOLVE,
  OPTREVALIAS,
  OPTREVNAME,
  OPTREVREAL,
  OPTRFC2047PARAMS,
  OPTSAVEADDRESS,
  OPTSAVEEMPTY,
  OPTSAVENAME,
  OPTSCORE,
  OPTSIGDASHES,
  OPTSIGONTOP,
  OPTSORTRE,
  OPTSTATUSONTOP,
  OPTSTRICTTHREADS,
  OPTSUSPEND,
  OPTTEXTFLOWED,
  OPTTHOROUGHSRC,
  OPTTHREADRECEIVED,
  OPTTILDE,
  OPTUNCOLLAPSEJUMP,
  OPTUSE8BITMIME,
  OPTUSEDOMAIN,
  OPTUSEFROM,



  OPTWAITKEY,
  OPTWEED,
  OPTWRAP,
  OPTWRAPSEARCH,
  OPTWRITEBCC,
  OPTXMAILER,




  OPTPGPAUTOSIGN,
  OPTPGPAUTOENCRYPT,
  OPTPGPIGNORESUB,
  OPTPGPLONGIDS,
  OPTPGPREPLYENCRYPT,
  OPTPGPREPLYSIGN,
  OPTPGPREPLYSIGNENCRYPTED,



  OPTPGPRETAINABLESIG,
  OPTPGPSTRICTENC,
  OPTFORWDECRYPT,
  OPTPGPSHOWUNUSABLE,




  OPTAUXSORT,
  OPTFORCEREFRESH,
  OPTLOCALES,
  OPTNOCURSES,
  OPTNEEDREDRAW,
  OPTSEARCHREVERSE,
  OPTMSGERR,
  OPTSEARCHINVALID,
  OPTSIGNALSBLOCKED,
  OPTSYSSIGNALSBLOCKED,
  OPTNEEDRESORT,
  OPTRESORTINIT,
  OPTVIEWATTACH,
  OPTFORCEREDRAWINDEX,
  OPTFORCEREDRAWPAGER,
  OPTSORTSUBTHREADS,
  OPTNEEDRESCORE,
  OPTATTACHMSG,
  OPTKEEPQUIET,



  OPTMENUCALLER,

  OPTPGPCHECKTRUST,
  OPTDONTHANDLEPGPKEYS,





  OPTMAX
};
// # 492 "mutt.h"
typedef struct list_t
{
  char *data;
  struct list_t *next;
} LIST;


void mutt_free_list (LIST **);
int mutt_matches_ignore (const char *, LIST *);


LIST *mutt_add_list (LIST *, const char *);

void mutt_init (int, LIST *);

typedef struct alias
{
  struct alias *self;
  char *name;
  ADDRESS *addr;
  struct alias *next;
  short tagged;
  short del;
  short num;
} ALIAS;

typedef struct envelope
{
  ADDRESS *return_path;
  ADDRESS *from;
  ADDRESS *to;
  ADDRESS *cc;
  ADDRESS *bcc;
  ADDRESS *sender;
  ADDRESS *reply_to;
  ADDRESS *mail_followup_to;
  char *subject;
  char *real_subj;
  char *message_id;
  char *supersedes;
  char *date;
  char *x_label;
  LIST *references;
  LIST *in_reply_to;
  LIST *userhdrs;
} ENVELOPE;

typedef struct parameter
{
  char *attribute;
  char *value;
  struct parameter *next;
} PARAMETER;


typedef struct content
{
  long hibin;
  long lobin;
  long crlf;
  long ascii;
  long linemax;
  unsigned int space : 1;
  unsigned int binary : 1;
  unsigned int from : 1;
  unsigned int dot : 1;
  unsigned int cr : 1;
} CONTENT;

typedef struct body
{
  char *xtype;
  char *subtype;
  PARAMETER *parameter;
  char *description;
  char *form_name;
  long hdr_offset;




  long offset;
  long length;
  char *filename;


  char *d_filename;




  CONTENT *content;




  struct body *next;
  struct body *parts;
  struct header *hdr;

  time_t stamp;



  unsigned int type : 4;
  unsigned int encoding : 3;
  unsigned int disposition : 2;
  unsigned int use_disp : 1;
  unsigned int unlink : 1;



  unsigned int tagged : 1;
  unsigned int deleted : 1;

  unsigned int noconv : 1;
  unsigned int force_charset : 1;





  unsigned int goodsig : 1;


  unsigned int collapsed : 1;

} BODY;

typedef struct header
{

  unsigned int pgp : 4;


  unsigned int mime : 1;
  unsigned int flagged : 1;
  unsigned int tagged : 1;
  unsigned int deleted : 1;
  unsigned int changed : 1;
  unsigned int attach_del : 1;
  unsigned int old : 1;
  unsigned int read : 1;
  unsigned int expired : 1;
  unsigned int superseded : 1;
  unsigned int replied : 1;
  unsigned int subject_changed : 1;
  unsigned int threaded : 1;
  unsigned int display_subject : 1;
  unsigned int recip_valid : 1;
  unsigned int active : 1;
  unsigned int trash : 1;





  unsigned int zhours : 5;
  unsigned int zminutes : 6;
  unsigned int zoccident : 1;


  unsigned int searched : 1;
  unsigned int matched : 1;


  unsigned int collapsed : 1;
  unsigned int limited : 1;
  size_t num_hidden;

  short recipient;

  int pair;

  time_t date_sent;
  time_t received;
  long offset;
  int lines;
  int index;
  int msgno;
  int virtual;
  int score;
  ENVELOPE *env;
  BODY *content;
  char *path;

  char *tree;
  struct thread *thread;
// # 692 "mutt.h"
} HEADER;

typedef struct thread
{
  unsigned int fake_thread : 1;
  unsigned int duplicate_thread : 1;
  unsigned int sort_children : 1;
  unsigned int check_subject : 1;
  unsigned int visible : 1;
  unsigned int deep : 1;
  unsigned int subtree_visible : 2;
  unsigned int next_subtree_visible : 1;
  struct thread *parent;
  struct thread *child;
  struct thread *next;
  struct thread *prev;
  HEADER *message;
  HEADER *sort_key;
} THREAD;

// # 1 "mutt_regex.h" 1
// # 29 "mutt_regex.h"
// # 1 "/usr/include/regex.h" 1 3 4
// # 36 "/usr/include/regex.h" 3 4
typedef long int s_reg_t;
typedef unsigned long int active_reg_t;






typedef unsigned long int reg_syntax_t;
// # 181 "/usr/include/regex.h" 3 4
extern reg_syntax_t re_syntax_options;
// # 303 "/usr/include/regex.h" 3 4
typedef enum
{




  REG_NOERROR = 0,
  REG_NOMATCH,



  REG_BADPAT,
  REG_ECOLLATE,
  REG_ECTYPE,
  REG_EESCAPE,
  REG_ESUBREG,
  REG_EBRACK,
  REG_EPAREN,
  REG_EBRACE,
  REG_BADBR,
  REG_ERANGE,
  REG_ESPACE,
  REG_BADRPT,


  REG_EEND,
  REG_ESIZE,
  REG_ERPAREN
} reg_errcode_t;
// # 343 "/usr/include/regex.h" 3 4
struct re_pattern_buffer
{



  unsigned char *buffer;


  unsigned long int allocated;


  unsigned long int used;


  reg_syntax_t syntax;




  char *fastmap;





  unsigned char * translate;


  size_t re_nsub;





  unsigned can_be_null : 1;
// # 386 "/usr/include/regex.h" 3 4
  unsigned regs_allocated : 2;



  unsigned fastmap_accurate : 1;



  unsigned no_sub : 1;



  unsigned not_bol : 1;


  unsigned not_eol : 1;


  unsigned newline_anchor : 1;
};

typedef struct re_pattern_buffer regex_t;


typedef int regoff_t;




struct re_registers
{
  unsigned num_regs;
  regoff_t *start;
  regoff_t *end;
};
// # 434 "/usr/include/regex.h" 3 4
typedef struct
{
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;





extern reg_syntax_t re_set_syntax (reg_syntax_t __syntax);




extern const char *re_compile_pattern (const char *__pattern, size_t __length,
           struct re_pattern_buffer *__buffer);





extern int re_compile_fastmap (struct re_pattern_buffer *__buffer);







extern int re_search (struct re_pattern_buffer *__buffer, const char *__string,
        int __length, int __start, int __range,
        struct re_registers *__regs);




extern int re_search_2 (struct re_pattern_buffer *__buffer,
   const char *__string1, int __length1,
   const char *__string2, int __length2, int __start,
   int __range, struct re_registers *__regs, int __stop);




extern int re_match (struct re_pattern_buffer *__buffer, const char *__string,
       int __length, int __start, struct re_registers *__regs);



extern int re_match_2 (struct re_pattern_buffer *__buffer,
         const char *__string1, int __length1,
         const char *__string2, int __length2, int __start,
         struct re_registers *__regs, int __stop);
// # 502 "/usr/include/regex.h" 3 4
extern void re_set_registers (struct re_pattern_buffer *__buffer,
         struct re_registers *__regs,
         unsigned int __num_regs,
         regoff_t *__starts, regoff_t *__ends);
// # 537 "/usr/include/regex.h" 3 4
extern int regcomp (regex_t *__restrict __preg,
      const char *__restrict __pattern,
      int __cflags);

extern int regexec (const regex_t *__restrict __preg,
      const char *__restrict __string, size_t __nmatch,
      regmatch_t __pmatch[__restrict],
      int __eflags);

extern size_t regerror (int __errcode, const regex_t *__restrict __preg,
   char *__restrict __errbuf, size_t __errbuf_size);

extern void regfree (regex_t *__preg);
// # 30 "mutt_regex.h" 2
// # 42 "mutt_regex.h"
typedef struct
{
  char *pattern;
  regex_t *rx;
  int not;
} REGEXP;

extern REGEXP Alternates;
extern REGEXP Mask;
extern REGEXP QuoteRegexp;
extern REGEXP ReplyRegexp;
extern REGEXP Smileys;
extern REGEXP GecosMask;
// # 713 "mutt.h" 2




typedef enum {
  M_MATCH_FULL_ADDRESS = 1
} pattern_exec_flag;

typedef struct pattern_t
{
  short op;
  short not;
  short alladdr;
  int min;
  int max;
  struct pattern_t *next;
  struct pattern_t *child;
  regex_t *rx;
} pattern_t;

typedef struct
{
  char *path;
  FILE *fp;
  time_t mtime;
  time_t mtime_cur;
  off_t size;
  off_t vsize;
  char *pattern;
  pattern_t *limit_pattern;
  HEADER **hdrs;
  THREAD *tree;
  HASH *id_hash;
  HASH *subj_hash;
  HASH *thread_hash;
  int *v2r;
  int hdrmax;
  int msgcount;
  int vcount;
  int tagged;
  int new;
  int unread;
  int deleted;
  int flagged;
  int msgnotreadyet;




  short magic;

  unsigned int locked : 1;
  unsigned int changed : 1;
  unsigned int readonly : 1;
  unsigned int dontwrite : 1;
  unsigned int append : 1;
  unsigned int quiet : 1;
  unsigned int collapsed : 1;
  unsigned int closing : 1;
} CONTEXT;

typedef struct attachptr
{
  BODY *content;
  int parent_type;
  char *tree;
  int level;
  int num;
} ATTACHPTR;

typedef struct
{
  FILE *fpin;
  FILE *fpout;
  char *prefix;
  int flags;
} STATE;



typedef struct
{
  wchar_t *wbuf;
  size_t wbuflen;
  size_t lastchar;
  size_t curpos;
  size_t begin;
  int tabs;
} ENTER_STATE;
// # 824 "mutt.h"
void state_mark_attach (STATE *);
void state_attach_puts (const char *, STATE *);
void state_prefix_putc (char, STATE *);
int state_printf(STATE *, const char *, ...);

// # 1 "ascii.h" 1
// # 32 "ascii.h"
int ascii_isupper (int c);
int ascii_islower (int c);
int ascii_toupper (int c);
int ascii_tolower (int c);
int ascii_strcasecmp (const char *a, const char *b);
int ascii_strncasecmp (const char *a, const char *b, int n);
// # 830 "mutt.h" 2
// # 1 "protos.h" 1
// # 20 "protos.h"
// # 1 "mbyte.h" 1



// # 1 "config.h" 1
// # 5 "mbyte.h" 2
// # 27 "mbyte.h"
void mutt_set_charset (char *charset);
extern int Charset_is_utf8;
size_t utf8rtowc (wchar_t *pwc, const char *s, size_t n, mbstate_t *_ps);
wchar_t replacement_char (void);
// # 21 "protos.h" 2
// # 31 "protos.h"
void _mutt_make_string (char *, size_t, const char *, CONTEXT *,
 HEADER *, format_flag);

int mutt_extract_token (BUFFER *, BUFFER *, int);
void mutt_buffer_add (BUFFER*, const char*, size_t);
void mutt_buffer_addstr (BUFFER*, const char*);
void mutt_buffer_addch (BUFFER*, char);


int _mutt_system (const char *, int);





int _mutt_aside_thread (HEADER *, short, short);






int _mutt_traverse_thread (CONTEXT *ctx, HEADER *hdr, int flag);







typedef const char * format_t (char *, size_t, char, const char *, const char *, const char *, const char *, unsigned long, format_flag);

void mutt_FormatString (char *, size_t, const char *, format_t *, unsigned long, format_flag);
void mutt_parse_content_type (char *, BODY *);
void mutt_generate_boundary (PARAMETER **);
void mutt_delete_parameter (const char *attribute, PARAMETER **p);
void mutt_set_parameter (const char *, const char *, PARAMETER **);


FILE *mutt_open_read (const char *, pid_t *);

void set_quadoption (int, int);
int query_quadoption (int, const char *);
int quadoption (int);

ADDRESS *mutt_default_from (void);
ADDRESS *mutt_get_address (ENVELOPE *, char **);
ADDRESS *mutt_lookup_alias (const char *s);
ADDRESS *mutt_remove_duplicates (ADDRESS *);
ADDRESS *mutt_expand_aliases (ADDRESS *);
ADDRESS *mutt_parse_adrlist (ADDRESS *, const char *);

BODY *mutt_make_file_attach (const char *);
BODY *mutt_make_message_attach (CONTEXT *, HEADER *, int);
BODY *mutt_remove_multipart (BODY *);
BODY *mutt_make_multipart (BODY *);
BODY *mutt_new_body (void);
BODY *mutt_parse_multipart (FILE *, const char *, long, int);
BODY *mutt_parse_messageRFC822 (FILE *, BODY *);
BODY *mutt_read_mime_header (FILE *, int);

CONTENT *mutt_get_content_info (const char *fname, BODY *b);

HASH *mutt_make_id_hash (CONTEXT *);
HASH *mutt_make_subj_hash (CONTEXT *);

LIST *mutt_make_references(ENVELOPE *e);

ENVELOPE *mutt_read_rfc822_header (FILE *, HEADER *, short, short);
HEADER *mutt_dup_header (HEADER *);

ATTACHPTR **mutt_gen_attach_list (BODY *, int, ATTACHPTR **, short *, short *, int, int);

time_t mutt_decrease_mtime (const char *, struct stat *);
time_t mutt_local_tz (time_t);
time_t mutt_mktime (struct tm *, int);
time_t mutt_parse_date (const char *, HEADER *);
int is_from (const char *, char *, size_t, time_t *);

const char *mutt_attach_fmt (
 char *dest,
 size_t destlen,
 char op,
 const char *src,
 const char *prefix,
 const char *ifstring,
 const char *elsestring,
 unsigned long data,
 format_flag flags);


char *mutt_charset_hook (const char *);
char *mutt_iconv_hook (const char *);
char *mutt_expand_path (char *, size_t);
char *_mutt_expand_path (char *, size_t, int);
char *mutt_find_hook (int, const char *);
char *mutt_gecos_name (char *, size_t, struct passwd *);
char *mutt_gen_msgid (void);
char *mutt_get_body_charset (char *, size_t, BODY *);
char *mutt_get_name (ADDRESS *);
char *mutt_get_parameter (const char *, PARAMETER *);

char *mutt_pgp_hook (ADDRESS *);

char *mutt_make_date (char *, size_t);

const char *mutt_make_version (void);

const char *mutt_fqdn(short);

void mutt_account_hook (const char* url);
void mutt_add_to_reference_headers (ENVELOPE *env, ENVELOPE *curenv, LIST ***pp, LIST ***qq);
void mutt_adv_mktemp (char *, size_t);
void mutt_alias_menu (char *, size_t, ALIAS *);
void mutt_allow_interrupt (int);
void mutt_attach_init (BODY *);
void mutt_block_signals (void);
void mutt_block_signals_system (void);
void mutt_body_handler (BODY *, STATE *);
void mutt_bounce_message (FILE *fp, HEADER *, ADDRESS *);
void mutt_buffy (char *, size_t);
void mutt_canonical_charset (char *, size_t, const char *);
void mutt_check_rescore (CONTEXT *);
void mutt_clear_error (void);
void mutt_create_alias (ENVELOPE *, ADDRESS *);
void mutt_decode_attachment (BODY *, STATE *);
void mutt_decode_base64 (STATE *s, long len, int istext, iconv_t cd);
void mutt_default_save (char *, size_t, HEADER *);
void mutt_display_address (ENVELOPE *);
void mutt_display_sanitize (char *);
void mutt_edit_content_type (HEADER *, BODY *, FILE *);
void mutt_edit_file (const char *, const char *);
void mutt_edit_headers (const char *, const char *, HEADER *, char *, size_t);
void mutt_curses_error (const char *, ...);
void mutt_curses_message (const char *, ...);
void mutt_enter_command (void);
void mutt_expand_aliases_env (ENVELOPE *);
void mutt_expand_file_fmt (char *, size_t, const char *, const char *);
void mutt_expand_fmt (char *, size_t, const char *, const char *);
void mutt_expand_link (char *, const char *, const char *);
void mutt_fix_reply_recipients (ENVELOPE *env);
void mutt_folder_hook (char *);
void mutt_format_string (char *, size_t, int, int, int, char, const char *, size_t, int);
void mutt_format_s (char *, size_t, const char *, const char *);
void mutt_format_s_tree (char *, size_t, const char *, const char *);
void mutt_forward_intro (FILE *fp, HEADER *cur);
void mutt_forward_trailer (FILE *fp);
void mutt_free_alias (ALIAS **);
void mutt_free_body (BODY **);
void mutt_free_color (int fg, int bg);
void mutt_free_enter_state (ENTER_STATE **);
void mutt_free_envelope (ENVELOPE **);
void mutt_free_header (HEADER **);
void mutt_free_parameter (PARAMETER **);
void mutt_generate_header (char *, size_t, HEADER *, int);
void mutt_help (int);
void mutt_draw_tree (CONTEXT *);
void mutt_make_attribution (CONTEXT *ctx, HEADER *cur, FILE *out);
void mutt_make_forward_subject (ENVELOPE *env, CONTEXT *ctx, HEADER *cur);
void mutt_make_help (char *, size_t, char *, int, int);
void mutt_make_misc_reply_headers (ENVELOPE *env, CONTEXT *ctx, HEADER *cur, ENVELOPE *curenv);
void mutt_make_post_indent (CONTEXT *ctx, HEADER *cur, FILE *out);
void mutt_message_to_7bit (BODY *, FILE *);
void mutt_mktemp (char *);
void mutt_normalize_time (struct tm *);
void mutt_paddstr (int, const char *);
void mutt_parse_mime_message (CONTEXT *ctx, HEADER *);
void mutt_parse_part (FILE *, BODY *);
void mutt_perror (const char *);
void mutt_prepare_envelope (ENVELOPE *, int);
void mutt_unprepare_envelope (ENVELOPE *);
void mutt_pretty_mailbox (char *);
void mutt_pretty_size (char *, size_t, long);
void mutt_pipe_message (HEADER *);
void mutt_print_message (HEADER *);
void mutt_print_patchlist (void);
void mutt_query_exit (void);
void mutt_query_menu (char *, size_t);
void mutt_safe_path (char *s, size_t l, ADDRESS *a);
void mutt_save_path (char *s, size_t l, ADDRESS *a);
void mutt_score_message (CONTEXT *, HEADER *, int);
void mutt_select_fcc (char *, size_t, HEADER *);

void _mutt_select_file (char *, size_t, int, char ***, int *);
void mutt_message_hook (CONTEXT *, HEADER *, int);
void _mutt_set_flag (CONTEXT *, HEADER *, int, int, int);

void mutt_set_followup_to (ENVELOPE *);
void mutt_shell_escape (void);
void mutt_show_error (void);
void mutt_signal_init (void);
void mutt_stamp_attachment (BODY *a);
void mutt_tabs_to_spaces (char *);
void mutt_tag_set_flag (int, int);
void mutt_unblock_signals (void);
void mutt_unblock_signals_system (int);
void mutt_update_encoding (BODY *a);
void mutt_update_tree (ATTACHPTR **, short);
void mutt_version (void);
void mutt_view_attachments (HEADER *);
void mutt_set_virtual (CONTEXT *);

int mutt_addr_is_user (ADDRESS *);
int mutt_addwch (wchar_t);
int mutt_alias_complete (char *, size_t);
int mutt_alloc_color (int fg, int bg);
int mutt_any_key_to_continue (const char *);
int mutt_buffy_check (int);
int mutt_buffy_notify (void);
int mutt_builtin_editor (const char *, HEADER *, HEADER *);
int mutt_can_decode (BODY *);
int mutt_change_flag (HEADER *, int);
int mutt_check_encoding (const char *);
int mutt_check_key (const char *);
int mutt_check_menu (const char *);
int mutt_check_mime_type (const char *);
int mutt_check_month (const char *);
int mutt_check_overwrite (const char *, const char *, char *, size_t, int *);
int mutt_check_traditional_pgp (HEADER *, int *);
int mutt_command_complete (char *, size_t, int, int);
int mutt_var_value_complete (char *, size_t, int);
int mutt_complete (char *, size_t);
int mutt_compose_attachment (BODY *a);
int mutt_copy_body (FILE *, BODY **, BODY *);
int mutt_decode_save_attachment (FILE *, BODY *, char *, int, int);
int mutt_display_message (HEADER *h);
int mutt_edit_attachment(BODY *);
int mutt_edit_message (CONTEXT *, HEADER *);
int mutt_fetch_recips (ENVELOPE *out, ENVELOPE *in, int flags);
int mutt_chscmp (const char *s, const char *chs);


int mutt_parent_message (CONTEXT *, HEADER *);
int mutt_prepare_template(FILE*, CONTEXT *, HEADER *, HEADER *, short);
int mutt_resend_message (FILE *, CONTEXT *, HEADER *);

int _mutt_enter_fname (const char *, char *, size_t, int *, int, int, char ***, int *);
int mutt_enter_string (char *buf, size_t buflen, int y, int x, int flags);
int _mutt_enter_string (char *, size_t, int, int, int, int, char ***, int *, ENTER_STATE *);

int _mutt_get_field (char *, char *, size_t, int, int, char ***, int *);
int mutt_get_hook_type (const char *);
int mutt_get_password (char *, char *, size_t);
int mutt_get_postponed (CONTEXT *, HEADER *, HEADER **, char *, size_t);
int mutt_get_tmp_attachment (BODY *);
int mutt_index_menu (void);
int mutt_invoke_sendmail (ADDRESS *, ADDRESS *, ADDRESS *, ADDRESS *, const char *, int);
int mutt_is_autoview (BODY *, const char *);
int mutt_is_mail_list (ADDRESS *);
int mutt_is_message_type(int, const char *);
int mutt_is_list_cc (int, ADDRESS *, ADDRESS *);
int mutt_is_list_recipient (int, ADDRESS *, ADDRESS *);
int mutt_is_subscribed_list (ADDRESS *);
int mutt_is_text_type (int, char *);
int mutt_is_valid_mailbox (const char *);
int mutt_messages_in_thread (CONTEXT *, HEADER *, int);
int mutt_multi_choice (char *prompt, char *letters);
int mutt_needs_mailcap (BODY *);
int mutt_num_postponed (int);
int mutt_parse_bind (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_exec (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_color (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_uncolor (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_hook (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_macro (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_mailboxes (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_mono (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_unmono (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_push (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_rc_line ( char *, BUFFER *, BUFFER *);
int mutt_parse_rfc822_line (ENVELOPE *e, HEADER *hdr, char *line, char *p,
  short user_hdrs, short weed, short do_2047, LIST **lastp);
int mutt_parse_score (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_unscore (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_parse_unhook (BUFFER *, BUFFER *, unsigned long, BUFFER *);
int mutt_pattern_func (int, char *);
int mutt_pipe_attachment (FILE *, BODY *, const char *, char *);
int mutt_print_attachment (FILE *, BODY *);
int mutt_query_complete (char *, size_t);
int mutt_save_attachment (FILE *, BODY *, char *, int, HEADER *);
void _mutt_save_message (HEADER *, CONTEXT *, int, int, int);
int mutt_save_message (HEADER *, int, int, int, int *);
int mutt_search_command (int, int);
int mutt_compose_menu (HEADER *, char *, size_t, HEADER *);
int mutt_thread_set_flag (HEADER *, int, int, int);
int mutt_user_is_recipient (HEADER *);
void mutt_update_num_postponed (void);
int mutt_view_attachment (FILE*, BODY *, int, HEADER *, ATTACHPTR **, short);
int mutt_wait_filter (pid_t);
int mutt_which_case (const char *);
int mutt_write_fcc (const char *path, HEADER *hdr, const char *msgid, int, char *);
int mutt_write_mime_body (BODY *, FILE *);
int mutt_write_mime_header (BODY *, FILE *);
int mutt_write_rfc822_header (FILE *, ENVELOPE *, BODY *, int, int);
int mutt_yesorno (const char *, int);
void mutt_set_header_color(CONTEXT *, HEADER *);
void mutt_sleep (short);
int mutt_save_confirm (const char *, struct stat *);

int mh_valid_message (const char *);

pid_t mutt_create_filter (const char *, FILE **, FILE **, FILE **);
pid_t mutt_create_filter_fd (const char *, FILE **, FILE **, FILE **, int, int, int);

ADDRESS *alias_reverse_lookup (ADDRESS *);


void mutt_to_base64 (unsigned char*, const unsigned char*, size_t, size_t);
int mutt_from_base64 (char*, const char*);


int mutt_wctoutf8 (char *s, unsigned int c);
// # 358 "protos.h"
int mutt_pattern_exec (struct pattern_t *pat, pattern_exec_flag flags, CONTEXT *ctx, HEADER *h);
pattern_t *mutt_pattern_comp ( char *s, int flags, BUFFER *err);
void mutt_check_simple (char *s, size_t len, const char *simple);
void mutt_pattern_free (pattern_t **pat);
// # 382 "protos.h"
int getdnsdomainname (char *, size_t);
// # 416 "protos.h"
int strcasecmp (const char *, const char *);
int strncasecmp (const char *, const char *, size_t);
// # 474 "protos.h"
void ci_bounce_message (HEADER *, int *);
int ci_send_message (int, HEADER *, char *, CONTEXT *, HEADER *);
// # 831 "mutt.h" 2
// # 1 "lib.h" 1
// # 28 "lib.h"
// # 1 "config.h" 1
// # 29 "lib.h" 2


// # 1 "/usr/include/string.h" 1 3 4
// # 28 "/usr/include/string.h" 3 4





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "/usr/include/string.h" 2 3 4




extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

// # 82 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

// # 130 "/usr/include/string.h" 3 4
extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
// # 165 "/usr/include/string.h" 3 4


extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

// # 181 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
// # 240 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

// # 254 "/usr/include/string.h" 3 4


extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

// # 270 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
// # 288 "/usr/include/string.h" 3 4
extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));



extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
// # 325 "/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
// # 348 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
// # 426 "/usr/include/string.h" 3 4

// # 32 "lib.h" 2






// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 39 "lib.h" 2
// # 47 "lib.h"
// # 1 "/usr/include/libintl.h" 1 3 4
// # 35 "/usr/include/libintl.h" 3 4





extern char *gettext (__const char *__msgid)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (1)));



extern char *dgettext (__const char *__domainname, __const char *__msgid)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2)));
extern char *__dgettext (__const char *__domainname, __const char *__msgid)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2)));



extern char *dcgettext (__const char *__domainname,
   __const char *__msgid, int __category)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2)));
extern char *__dcgettext (__const char *__domainname,
     __const char *__msgid, int __category)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2)));




extern char *ngettext (__const char *__msgid1, __const char *__msgid2,
         unsigned long int __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (1))) __attribute__ ((__format_arg__ (2)));



extern char *dngettext (__const char *__domainname, __const char *__msgid1,
   __const char *__msgid2, unsigned long int __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2))) __attribute__ ((__format_arg__ (3)));



extern char *dcngettext (__const char *__domainname, __const char *__msgid1,
    __const char *__msgid2, unsigned long int __n,
    int __category)
     __attribute__ ((__nothrow__)) __attribute__ ((__format_arg__ (2))) __attribute__ ((__format_arg__ (3)));





extern char *textdomain (__const char *__domainname) __attribute__ ((__nothrow__));



extern char *bindtextdomain (__const char *__domainname,
        __const char *__dirname) __attribute__ ((__nothrow__));



extern char *bind_textdomain_codeset (__const char *__domainname,
          __const char *__codeset) __attribute__ ((__nothrow__));
// # 122 "/usr/include/libintl.h" 3 4

// # 48 "lib.h" 2
// # 93 "lib.h"
extern void (*mutt_error) (const char *, ...);

void mutt_exit (int);



FILE *safe_fopen (const char *, const char *);

char *mutt_read_line (char *, size_t *, FILE *, int *);
char *mutt_skip_whitespace (char *);
char *mutt_strlower (char *);
char *mutt_substrcpy (char *, const char *, const char *, size_t);
char *mutt_substrdup (const char *, const char *);
char *safe_strdup (const char *);

const char *mutt_stristr (const char *, const char *);

int mutt_copy_stream (FILE *, FILE *);
int mutt_copy_bytes (FILE *, FILE *, size_t);
int mutt_rx_sanitize_string (char *, size_t, const char *);
int mutt_strcasecmp (const char *, const char *);
int mutt_strcmp (const char *, const char *);
int mutt_strncasecmp (const char *, const char *, size_t);
int mutt_strncmp (const char *, const char *, size_t);
int safe_open (const char *, int);
int safe_symlink (const char *, const char *);
int safe_rename (const char *, const char *);
int safe_fclose (FILE **);

size_t mutt_quote_filename (char *, size_t, const char *);
size_t mutt_strlen (const char *);

void *safe_calloc (size_t, size_t);
void *safe_malloc (size_t);
void mutt_nocurses_error (const char *, ...);
void mutt_remove_trailing_ws (char *);
void mutt_sanitize_filename (char *, short);
void mutt_str_replace (char **p, const char *s);
void mutt_str_adjust (char **p);
void mutt_unlink (const char *);
void safe_free (void **);
void safe_realloc (void **, size_t);
// # 832 "mutt.h" 2
// # 1 "globals.h" 1
// # 19 "globals.h"
extern void (*mutt_error) (const char *, ...);
extern void (*mutt_message) (const char *, ...);

extern CONTEXT *Context;

extern char Errorbuf[256];
extern char AttachmentMarker[256];


extern char *MuttDotlock;


extern ADDRESS *From;

extern char *AliasFile;
extern char *AliasFmt;
extern char *AttachSep;
extern char *Attribution;
extern char *AttachFormat;
extern char *Charset;
extern char *ComposeFormat;
extern char *DefaultHook;
extern char *DateFmt;
extern char *DisplayFilter;
extern char *DsnNotify;
extern char *DsnReturn;
extern char *Editor;
extern char *EscChar;
extern char *FolderFormat;
extern char *ForwFmt;
extern char *Fqdn;
extern char *HdrFmt;
extern char *Homedir;
extern char *Hostname;







extern char *Inbox;
extern char *Ispell;
extern char *Locale;
extern char *MailcapPath;
extern char *Maildir;
extern char *MhFlagged;
extern char *MhReplied;
extern char *MhUnseen;
extern char *MsgFmt;
// # 80 "globals.h"
extern char *Muttrc ;
extern char *Outbox;
extern char *Pager;
extern char *PagerFmt;
extern char *PipeSep;







extern char *PostIndentString;
extern char *Postponed;
extern char *Prefix;
extern char *PrintCmd;
extern char *QueryCmd;
extern char *Realname;
extern char *SendCharset;
extern char *Sendmail;
extern char *Shell;
extern char *Signature;
extern char *SimpleSearch;
extern char *Spoolfile;




extern char *StChars;
extern char *Status;
extern char *Tempdir;
extern char *Tochars;
extern char *Username;
extern char *Visual;

extern char *LastFolder;

extern LIST *AutoViewList ;
extern LIST *AlternativeOrderList ;
extern LIST *HeaderOrderList ;
extern LIST *Ignore ;
extern LIST *UnIgnore ;
extern LIST *MailLists ;
extern LIST *SubscribedLists ;
// # 133 "globals.h"
extern unsigned char Options[];






extern unsigned char QuadOptions[];


extern unsigned short Counter ;

extern short ConnectTimeout;
extern short HistSize;
extern short PagerContext;
extern short PagerIndexLines;
extern short ReadInc;
extern short SendmailWait;
extern short SleepTime ;
extern short Timeout;
extern short WrapMargin;
extern short WriteInc;

extern short ScoreThresholdDelete;
extern short ScoreThresholdRead;
extern short ScoreThresholdFlag;






extern volatile sig_atomic_t SigAlrm ;
extern volatile sig_atomic_t SigInt ;
extern volatile sig_atomic_t SigWinch ;

extern int CurrentMenu;

extern ALIAS *Aliases ;
extern LIST *UserHeader ;
// # 186 "globals.h"
extern const char *Weekdays[];
extern const char *Months[];
// # 832 "mutt.h" 2
// # 21 "enter.c" 2
// # 1 "mutt_menu.h" 1
// # 23 "mutt_menu.h"
// # 1 "keymap.h" 1
// # 22 "keymap.h"
// # 1 "mapping.h" 1
// # 22 "mapping.h"
struct mapping_t
{
  char *name;
  int value;
};

char *mutt_getnamebyvalue (int, const struct mapping_t *);
char *mutt_compile_help (char *, size_t, int, struct mapping_t *);

int mutt_getvaluebyname (const char *, const struct mapping_t *);
// # 23 "keymap.h" 2





typedef short keycode_t;

void km_bind (char *, int, int, char *, char *);
void km_bindkey (char *, int, int);
int km_dokey (int);


struct keymap_t
{
  char *macro;
  char *descr;
  struct keymap_t *next;
  short op;
  short eq;
  short len;
  keycode_t *keys;
};

char *km_keyname (int);
int km_expand_key (char *, size_t, struct keymap_t *);
struct keymap_t *km_find_func (int, int);
void km_init (void);
void km_error_key (int);

enum
{
  MENU_ALIAS,
  MENU_ATTACH,
  MENU_COMPOSE,
  MENU_EDITOR,
  MENU_FOLDER,
  MENU_GENERIC,
  MENU_MAIN,
  MENU_PAGER,
  MENU_POST,
  MENU_QUERY,



  MENU_PGP,
// # 76 "keymap.h"
  MENU_MAX
};


extern struct keymap_t *Keymaps[];


extern int LastKey;

extern struct mapping_t Menus[];

struct binding_t
{
  char *name;
  int op;
  char *seq;
};

struct binding_t *km_get_table (int menu);

extern struct binding_t OpGeneric[];
extern struct binding_t OpPost[];
extern struct binding_t OpMain[];
extern struct binding_t OpAttach[];
extern struct binding_t OpPager[];
extern struct binding_t OpCompose[];
extern struct binding_t OpBrowser[];
extern struct binding_t OpEditor[];
extern struct binding_t OpQuery[];
extern struct binding_t OpAlias[];


extern struct binding_t OpPgp[];






// # 1 "keymap_defs.h" 1
// # 189 "keymap_defs.h"
enum {
 OP_NULL,
 OP_ATTACH_VIEW_MAILCAP,
 OP_ATTACH_VIEW_TEXT,
 OP_ATTACH_COLLAPSE,
 OP_BOTTOM_PAGE,
 OP_BOUNCE_MESSAGE,
 OP_BROWSER_NEW_FILE,
 OP_BROWSER_VIEW_FILE,
 OP_BROWSER_TELL,
 OP_BROWSER_SUBSCRIBE,
 OP_BROWSER_UNSUBSCRIBE,
 OP_BROWSER_TOGGLE_LSUB,
 OP_CHANGE_DIRECTORY,
 OP_CHECK_NEW,
 OP_COMPOSE_ATTACH_FILE,
 OP_COMPOSE_ATTACH_MESSAGE,
 OP_COMPOSE_EDIT_BCC,
 OP_COMPOSE_EDIT_CC,
 OP_COMPOSE_EDIT_DESCRIPTION,
 OP_COMPOSE_EDIT_ENCODING,
 OP_COMPOSE_EDIT_FCC,
 OP_COMPOSE_EDIT_FILE,
 OP_COMPOSE_EDIT_FROM,
 OP_COMPOSE_EDIT_HEADERS,
 OP_COMPOSE_EDIT_MESSAGE,
 OP_COMPOSE_EDIT_MIME,
 OP_COMPOSE_EDIT_REPLY_TO,
 OP_COMPOSE_EDIT_SUBJECT,
 OP_COMPOSE_EDIT_TO,
 OP_CREATE_MAILBOX,
 OP_EDIT_TYPE,
 OP_COMPOSE_GET_ATTACHMENT,
 OP_COMPOSE_ISPELL,
 OP_COMPOSE_NEW_MIME,
 OP_COMPOSE_TOGGLE_RECODE,
 OP_COMPOSE_POSTPONE_MESSAGE,
 OP_COMPOSE_RENAME_FILE,
 OP_COMPOSE_SEND_MESSAGE,
 OP_COMPOSE_TOGGLE_DISPOSITION,
 OP_COMPOSE_TOGGLE_UNLINK,
 OP_COMPOSE_UPDATE_ENCODING,
 OP_COMPOSE_WRITE_MESSAGE,
 OP_COPY_MESSAGE,
 OP_CREATE_ALIAS,
 OP_CURRENT_BOTTOM,
 OP_CURRENT_MIDDLE,
 OP_CURRENT_TOP,
 OP_DECODE_COPY,
 OP_DECODE_SAVE,
 OP_DELETE,
 OP_DELETE_MAILBOX,
 OP_DELETE_SUBTHREAD,
 OP_DELETE_THREAD,
 OP_DISPLAY_ADDRESS,
 OP_DISPLAY_HEADERS,
 OP_DISPLAY_MESSAGE,
 OP_EDIT_MESSAGE,
 OP_EDITOR_BACKSPACE,
 OP_EDITOR_BACKWARD_CHAR,
 OP_EDITOR_BACKWARD_WORD,
 OP_EDITOR_BOL,
 OP_EDITOR_BUFFY_CYCLE,
 OP_EDITOR_COMPLETE,
 OP_EDITOR_COMPLETE_QUERY,
 OP_EDITOR_DELETE_CHAR,
 OP_EDITOR_EOL,
 OP_EDITOR_FORWARD_CHAR,
 OP_EDITOR_FORWARD_WORD,
 OP_EDITOR_HISTORY_DOWN,
 OP_EDITOR_HISTORY_UP,
 OP_EDITOR_KILL_EOL,
 OP_EDITOR_KILL_EOW,
 OP_EDITOR_KILL_LINE,
 OP_EDITOR_KILL_WORD,
 OP_EDITOR_QUOTE_CHAR,
 OP_EDITOR_TRANSPOSE_CHARS,
 OP_EDITOR_CAPITALIZE_WORD,
 OP_EDITOR_DOWNCASE_WORD,
 OP_EDITOR_UPCASE_WORD,
 OP_ENTER_COMMAND,
 OP_ENTER_MASK,
 OP_EXIT,
 OP_FILTER,
 OP_FIRST_ENTRY,
 OP_FLAG_MESSAGE,
 OP_FORWARD_MESSAGE,
 OP_GENERIC_SELECT_ENTRY,
 OP_GROUP_REPLY,
 OP_HALF_DOWN,
 OP_HALF_UP,
 OP_HELP,
 OP_JUMP,
 OP_LAST_ENTRY,
 OP_LIST_REPLY,
 OP_MACRO,
 OP_MAIL,
 OP_MAIN_CHANGE_FOLDER,
 OP_MAIN_CHANGE_FOLDER_READONLY,
 OP_MAIN_CLEAR_FLAG,
 OP_MAIN_DELETE_PATTERN,
 OP_MAIN_IMAP_FETCH,
 OP_MAIN_FETCH_MAIL,
 OP_MAIN_FIRST_MESSAGE,
 OP_MAIN_LAST_MESSAGE,
 OP_MAIN_LIMIT,
 OP_MAIN_NEXT_NEW,
 OP_MAIN_NEXT_SUBTHREAD,
 OP_MAIN_NEXT_THREAD,
 OP_MAIN_NEXT_UNDELETED,
 OP_MAIN_NEXT_UNREAD,
 OP_MAIN_PARENT_MESSAGE,
 OP_MAIN_PREV_THREAD,
 OP_MAIN_PREV_SUBTHREAD,
 OP_MAIN_PREV_UNDELETED,
 OP_MAIN_PREV_NEW,
 OP_MAIN_PREV_UNREAD,
 OP_MAIN_READ_THREAD,
 OP_MAIN_READ_SUBTHREAD,
 OP_MAIN_SET_FLAG,
 OP_MAIN_SYNC_FOLDER,
 OP_MAIN_TAG_PATTERN,
 OP_MAIN_UNDELETE_PATTERN,
 OP_MAIN_UNTAG_PATTERN,
 OP_MIDDLE_PAGE,
 OP_NEXT_ENTRY,
 OP_NEXT_LINE,
 OP_NEXT_PAGE,
 OP_PAGER_BOTTOM,
 OP_PAGER_HIDE_QUOTED,
 OP_PAGER_SKIP_QUOTED,
 OP_PAGER_TOP,
 OP_PIPE,
 OP_PREV_ENTRY,
 OP_PREV_LINE,
 OP_PREV_PAGE,
 OP_PRINT,
 OP_QUERY,
 OP_QUERY_APPEND,
 OP_QUIT,
 OP_RECALL_MESSAGE,
 OP_REDRAW,
 OP_REFORMAT_WINCH,
 OP_REPLY,
 OP_RESEND,
 OP_SAVE,
 OP_SEARCH,
 OP_SEARCH_REVERSE,
 OP_SEARCH_NEXT,
 OP_SEARCH_OPPOSITE,
 OP_SEARCH_TOGGLE,
 OP_SHELL_ESCAPE,
 OP_SORT,
 OP_SORT_REVERSE,
 OP_TAG,
 OP_TAG_PREFIX,
 OP_TAG_SUBTHREAD,
 OP_TAG_THREAD,
 OP_TOGGLE_NEW,
 OP_TOGGLE_WRITE,
 OP_TOGGLE_MAILBOXES,
 OP_TOP_PAGE,
 OP_UNDELETE,
 OP_UNDELETE_THREAD,
 OP_UNDELETE_SUBTHREAD,
 OP_VERSION,
 OP_VIEW_ATTACH,
 OP_VIEW_ATTACHMENTS,
 OP_MAIN_SHOW_LIMIT,
 OP_MAIN_COLLAPSE_THREAD,
 OP_MAIN_COLLAPSE_ALL,
 OP_COMPOSE_ATTACH_KEY,
 OP_COMPOSE_PGP_MENU,
 OP_EXTRACT_KEYS,
 OP_FORGET_PASSPHRASE,
 OP_MAIL_KEY,
 OP_VERIFY_KEY,
 OP_VIEW_ID,
 OP_DECRYPT_SAVE,
 OP_DECRYPT_COPY,
 OP_CHECK_TRADITIONAL,
 OP_MAX
};
// # 116 "keymap.h" 2
// # 24 "mutt_menu.h" 2
// # 37 "mutt_menu.h"
typedef struct menu_t
{
  char *title;
  char *help;
  void *data;
  int current;
  int max;
  int redraw;
  int menu;
  int offset;
  int pagelen;
  int tagprefix;






  char **dialog;
  char *prompt;
  char *keys;


  void (*make_entry) (char *, size_t, struct menu_t *, int);


  int (*search) (struct menu_t *, regex_t *re, int n);

  int (*tag) (struct menu_t *, int i, int m);




  int (*color) (int i);


  int top;
  int oldcurrent;
  char *searchBuf;
  int searchDir;
  int tagged;
} MUTTMENU;

void menu_jump (MUTTMENU *);
void menu_redraw_full (MUTTMENU *);
void menu_redraw_index (MUTTMENU *);
void menu_redraw_status (MUTTMENU *);
void menu_redraw_motion (MUTTMENU *);
void menu_redraw_current (MUTTMENU *);
void menu_first_entry (MUTTMENU *);
void menu_last_entry (MUTTMENU *);
void menu_top_page (MUTTMENU *);
void menu_bottom_page (MUTTMENU *);
void menu_middle_page (MUTTMENU *);
void menu_next_page (MUTTMENU *);
void menu_prev_page (MUTTMENU *);
void menu_next_line (MUTTMENU *);
void menu_prev_line (MUTTMENU *);
void menu_half_up (MUTTMENU *);
void menu_half_down (MUTTMENU *);
void menu_current_top (MUTTMENU *);
void menu_current_middle (MUTTMENU *);
void menu_current_bottom (MUTTMENU *);
void menu_check_recenter (MUTTMENU *);
void menu_status_line (char *, size_t, MUTTMENU *, const char *);

MUTTMENU *mutt_new_menu (void);
void mutt_menuDestroy (MUTTMENU **);
int mutt_menuLoop (MUTTMENU *);


void index_make_entry (char *, size_t, struct menu_t *, int);
int index_color (int);
// # 22 "enter.c" 2
// # 1 "mutt_curses.h" 1
// # 40 "mutt_curses.h"
// # 1 "/usr/include/ncursesw/ncurses.h" 1 3 4
// # 66 "/usr/include/ncursesw/ncurses.h" 3 4
// # 1 "/usr/include/ncursesw/ncurses_dll.h" 1 3 4
// # 67 "/usr/include/ncursesw/ncurses.h" 2 3 4
// # 122 "/usr/include/ncursesw/ncurses.h" 3 4
typedef unsigned long chtype;
typedef unsigned long mmask_t;



// # 1 "/usr/include/ncursesw/unctrl.h" 1 3 4
// # 54 "/usr/include/ncursesw/unctrl.h" 3 4
// # 1 "/usr/include/ncursesw/curses.h" 1 3 4
// # 55 "/usr/include/ncursesw/unctrl.h" 2 3 4


 char * unctrl (chtype);
// # 128 "/usr/include/ncursesw/ncurses.h" 2 3 4
// # 151 "/usr/include/ncursesw/ncurses.h" 3 4
typedef unsigned char NCURSES_BOOL;
// # 161 "/usr/include/ncursesw/ncurses.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdbool.h" 1 3 4
// # 162 "/usr/include/ncursesw/ncurses.h" 2 3 4
// # 202 "/usr/include/ncursesw/ncurses.h" 3 4
extern int COLORS;
extern int COLOR_PAIRS;
// # 220 "/usr/include/ncursesw/ncurses.h" 3 4
extern chtype acs_map[];
// # 310 "/usr/include/ncursesw/ncurses.h" 3 4
typedef struct screen SCREEN;
typedef struct _win_st WINDOW;

typedef chtype attr_t;
// # 349 "/usr/include/ncursesw/ncurses.h" 3 4
struct ldat;

struct _win_st
{
 short _cury, _curx;


 short _maxy, _maxx;
 short _begy, _begx;

 short _flags;


 attr_t _attrs;
 chtype _bkgd;


 _Bool _notimeout;
 _Bool _clear;
 _Bool _leaveok;
 _Bool _scroll;
 _Bool _idlok;
 _Bool _idcok;
 _Bool _immed;
 _Bool _sync;
 _Bool _use_keypad;
 int _delay;

 struct ldat *_line;


 short _regtop;
 short _regbottom;


 int _parx;
 int _pary;
 WINDOW *_parent;


 struct pdat
 {
     short _pad_y, _pad_x;
     short _pad_top, _pad_left;
     short _pad_bottom, _pad_right;
 } _pad;

 short _yoffset;







};

extern WINDOW * stdscr;
extern WINDOW * curscr;
extern WINDOW * newscr;

extern int LINES;
extern int COLS;
extern int TABSIZE;




extern int ESCDELAY;

extern char ttytype[];





extern _Bool is_term_resized (int, int);
extern char * keybound (int, int);
extern const char * curses_version (void);
extern int assume_default_colors (int, int);
extern int define_key (const char *, int);
extern int key_defined (const char *);
extern int keyok (int, _Bool);
extern int resize_term (int, int);
extern int resizeterm (int, int);
extern int use_default_colors (void);
extern int use_extended_names (_Bool);
extern int use_legacy_coding (int);
extern int wresize (WINDOW *, int, int);
extern void nofilter(void);
// # 537 "/usr/include/ncursesw/ncurses.h" 3 4
extern int addch (const chtype);
extern int addchnstr (const chtype *, int);
extern int addchstr (const chtype *);
extern int addnstr (const char *, int);
extern int addstr (const char *);
extern int attroff (int);
extern int attron (int);
extern int attrset (int);
extern int attr_get (attr_t *, short *, void *);
extern int attr_off (attr_t, void *);
extern int attr_on (attr_t, void *);
extern int attr_set (attr_t, short, void *);
extern int baudrate (void);
extern int beep (void);
extern int bkgd (chtype);
extern void bkgdset (chtype);
extern int border (chtype,chtype,chtype,chtype,chtype,chtype,chtype,chtype);
extern int box (WINDOW *, chtype, chtype);
extern _Bool can_change_color (void);
extern int cbreak (void);
extern int chgat (int, attr_t, short, const void *);
extern int clear (void);
extern int clearok (WINDOW *,_Bool);
extern int clrtobot (void);
extern int clrtoeol (void);
extern int color_content (short,short*,short*,short*);
extern int color_set (short,void*);
extern int COLOR_PAIR (int);
extern int copywin (const WINDOW*,WINDOW*,int,int,int,int,int,int,int);
extern int curs_set (int);
extern int def_prog_mode (void);
extern int def_shell_mode (void);
extern int delay_output (int);
extern int delch (void);
extern void delscreen (SCREEN *);
extern int delwin (WINDOW *);
extern int deleteln (void);
extern WINDOW * derwin (WINDOW *,int,int,int,int);
extern int doupdate (void);
extern WINDOW * dupwin (WINDOW *);
extern int echo (void);
extern int echochar (const chtype);
extern int erase (void);
extern int endwin (void);
extern char erasechar (void);
extern void filter (void);
extern int flash (void);
extern int flushinp (void);
extern chtype getbkgd (WINDOW *);
extern int getch (void);
extern int getnstr (char *, int);
extern int getstr (char *);
extern WINDOW * getwin (FILE *);
extern int halfdelay (int);
extern _Bool has_colors (void);
extern _Bool has_ic (void);
extern _Bool has_il (void);
extern int hline (chtype, int);
extern void idcok (WINDOW *, _Bool);
extern int idlok (WINDOW *, _Bool);
extern void immedok (WINDOW *, _Bool);
extern chtype inch (void);
extern int inchnstr (chtype *, int);
extern int inchstr (chtype *);
extern WINDOW * initscr (void);
extern int init_color (short,short,short,short);
extern int init_pair (short,short,short);
extern int innstr (char *, int);
extern int insch (chtype);
extern int insdelln (int);
extern int insertln (void);
extern int insnstr (const char *, int);
extern int insstr (const char *);
extern int instr (char *);
extern int intrflush (WINDOW *,_Bool);
extern _Bool isendwin (void);
extern _Bool is_linetouched (WINDOW *,int);
extern _Bool is_wintouched (WINDOW *);
extern char * keyname (int);
extern int keypad (WINDOW *,_Bool);
extern char killchar (void);
extern int leaveok (WINDOW *,_Bool);
extern char * longname (void);
extern int meta (WINDOW *,_Bool);
extern int move (int, int);
extern int mvaddch (int, int, const chtype);
extern int mvaddchnstr (int, int, const chtype *, int);
extern int mvaddchstr (int, int, const chtype *);
extern int mvaddnstr (int, int, const char *, int);
extern int mvaddstr (int, int, const char *);
extern int mvchgat (int, int, int, attr_t, short, const void *);
extern int mvcur (int,int,int,int);
extern int mvdelch (int, int);
extern int mvderwin (WINDOW *, int, int);
extern int mvgetch (int, int);
extern int mvgetnstr (int, int, char *, int);
extern int mvgetstr (int, int, char *);
extern int mvhline (int, int, chtype, int);
extern chtype mvinch (int, int);
extern int mvinchnstr (int, int, chtype *, int);
extern int mvinchstr (int, int, chtype *);
extern int mvinnstr (int, int, char *, int);
extern int mvinsch (int, int, chtype);
extern int mvinsnstr (int, int, const char *, int);
extern int mvinsstr (int, int, const char *);
extern int mvinstr (int, int, char *);
extern int mvprintw (int,int, const char *,...)
  ;
extern int mvscanw (int,int, char *,...)
  ;
extern int mvvline (int, int, chtype, int);
extern int mvwaddch (WINDOW *, int, int, const chtype);
extern int mvwaddchnstr (WINDOW *, int, int, const chtype *, int);
extern int mvwaddchstr (WINDOW *, int, int, const chtype *);
extern int mvwaddnstr (WINDOW *, int, int, const char *, int);
extern int mvwaddstr (WINDOW *, int, int, const char *);
extern int mvwchgat (WINDOW *, int, int, int, attr_t, short, const void *);
extern int mvwdelch (WINDOW *, int, int);
extern int mvwgetch (WINDOW *, int, int);
extern int mvwgetnstr (WINDOW *, int, int, char *, int);
extern int mvwgetstr (WINDOW *, int, int, char *);
extern int mvwhline (WINDOW *, int, int, chtype, int);
extern int mvwin (WINDOW *,int,int);
extern chtype mvwinch (WINDOW *, int, int);
extern int mvwinchnstr (WINDOW *, int, int, chtype *, int);
extern int mvwinchstr (WINDOW *, int, int, chtype *);
extern int mvwinnstr (WINDOW *, int, int, char *, int);
extern int mvwinsch (WINDOW *, int, int, chtype);
extern int mvwinsnstr (WINDOW *, int, int, const char *, int);
extern int mvwinsstr (WINDOW *, int, int, const char *);
extern int mvwinstr (WINDOW *, int, int, char *);
extern int mvwprintw (WINDOW*,int,int, const char *,...)
  ;
extern int mvwscanw (WINDOW *,int,int, char *,...)
  ;
extern int mvwvline (WINDOW *,int, int, chtype, int);
extern int napms (int);
extern WINDOW * newpad (int,int);
extern SCREEN * newterm ( char *,FILE *,FILE *);
extern WINDOW * newwin (int,int,int,int);
extern int nl (void);
extern int nocbreak (void);
extern int nodelay (WINDOW *,_Bool);
extern int noecho (void);
extern int nonl (void);
extern void noqiflush (void);
extern int noraw (void);
extern int notimeout (WINDOW *,_Bool);
extern int overlay (const WINDOW*,WINDOW *);
extern int overwrite (const WINDOW*,WINDOW *);
extern int pair_content (short,short*,short*);
extern int PAIR_NUMBER (int);
extern int pechochar (WINDOW *, const chtype);
extern int pnoutrefresh (WINDOW*,int,int,int,int,int,int);
extern int prefresh (WINDOW *,int,int,int,int,int,int);
extern int printw (const char *,...)
  ;
extern int putp (const char *);
extern int putwin (WINDOW *, FILE *);
extern void qiflush (void);
extern int raw (void);
extern int redrawwin (WINDOW *);
extern int refresh (void);
extern int resetty (void);
extern int reset_prog_mode (void);
extern int reset_shell_mode (void);
extern int ripoffline (int, int (*)(WINDOW *, int));
extern int savetty (void);
extern int scanw ( char *,...)
  ;
extern int scr_dump (const char *);
extern int scr_init (const char *);
extern int scrl (int);
extern int scroll (WINDOW *);
extern int scrollok (WINDOW *,_Bool);
extern int scr_restore (const char *);
extern int scr_set (const char *);
extern int setscrreg (int,int);
extern SCREEN * set_term (SCREEN *);
extern int slk_attroff (const chtype);
extern int slk_attr_off (const attr_t, void *);
extern int slk_attron (const chtype);
extern int slk_attr_on (attr_t,void*);
extern int slk_attrset (const chtype);
extern attr_t slk_attr (void);
extern int slk_attr_set (const attr_t,short,void*);
extern int slk_clear (void);
extern int slk_color (short);
extern int slk_init (int);
extern char * slk_label (int);
extern int slk_noutrefresh (void);
extern int slk_refresh (void);
extern int slk_restore (void);
extern int slk_set (int,const char *,int);
extern int slk_touch (void);
extern int standout (void);
extern int standend (void);
extern int start_color (void);
extern WINDOW * subpad (WINDOW *, int, int, int, int);
extern WINDOW * subwin (WINDOW *,int,int,int,int);
extern int syncok (WINDOW *, _Bool);
extern chtype termattrs (void);
extern char * termname (void);
extern int tigetflag ( char *);
extern int tigetnum ( char *);
extern char * tigetstr ( char *);
extern void timeout (int);
extern int touchline (WINDOW *, int, int);
extern int touchwin (WINDOW *);
extern char * tparm ( char *, ...);
extern int typeahead (int);
extern int ungetch (int);
extern int untouchwin (WINDOW *);
extern void use_env (_Bool);
extern int vidattr (chtype);
extern int vidputs (chtype, int (*)(int));
extern int vline (chtype, int);
extern int vwprintw (WINDOW *, const char *,va_list);
extern int vw_printw (WINDOW *, const char *,va_list);
extern int vwscanw (WINDOW *, char *,va_list);
extern int vw_scanw (WINDOW *, char *,va_list);
extern int waddch (WINDOW *, const chtype);
extern int waddchnstr (WINDOW *,const chtype *,int);
extern int waddchstr (WINDOW *,const chtype *);
extern int waddnstr (WINDOW *,const char *,int);
extern int waddstr (WINDOW *,const char *);
extern int wattron (WINDOW *, int);
extern int wattroff (WINDOW *, int);
extern int wattrset (WINDOW *, int);
extern int wattr_get (WINDOW *, attr_t *, short *, void *);
extern int wattr_on (WINDOW *, attr_t, void *);
extern int wattr_off (WINDOW *, attr_t, void *);
extern int wattr_set (WINDOW *, attr_t, short, void *);
extern int wbkgd (WINDOW *, chtype);
extern void wbkgdset (WINDOW *,chtype);
extern int wborder (WINDOW *,chtype,chtype,chtype,chtype,chtype,chtype,chtype,chtype);
extern int wchgat (WINDOW *, int, attr_t, short, const void *);
extern int wclear (WINDOW *);
extern int wclrtobot (WINDOW *);
extern int wclrtoeol (WINDOW *);
extern int wcolor_set (WINDOW*,short,void*);
extern void wcursyncup (WINDOW *);
extern int wdelch (WINDOW *);
extern int wdeleteln (WINDOW *);
extern int wechochar (WINDOW *, const chtype);
extern int werase (WINDOW *);
extern int wgetch (WINDOW *);
extern int wgetnstr (WINDOW *,char *,int);
extern int wgetstr (WINDOW *, char *);
extern int whline (WINDOW *, chtype, int);
extern chtype winch (WINDOW *);
extern int winchnstr (WINDOW *, chtype *, int);
extern int winchstr (WINDOW *, chtype *);
extern int winnstr (WINDOW *, char *, int);
extern int winsch (WINDOW *, chtype);
extern int winsdelln (WINDOW *,int);
extern int winsertln (WINDOW *);
extern int winsnstr (WINDOW *, const char *,int);
extern int winsstr (WINDOW *, const char *);
extern int winstr (WINDOW *, char *);
extern int wmove (WINDOW *,int,int);
extern int wnoutrefresh (WINDOW *);
extern int wprintw (WINDOW *, const char *,...)
  ;
extern int wredrawln (WINDOW *,int,int);
extern int wrefresh (WINDOW *);
extern int wscanw (WINDOW *, char *,...)
  ;
extern int wscrl (WINDOW *,int);
extern int wsetscrreg (WINDOW *,int,int);
extern int wstandout (WINDOW *);
extern int wstandend (WINDOW *);
extern void wsyncdown (WINDOW *);
extern void wsyncup (WINDOW *);
extern void wtimeout (WINDOW *,int);
extern int wtouchln (WINDOW *,int,int,int);
extern int wvline (WINDOW *,chtype,int);




extern int getcurx (const WINDOW *);
extern int getcury (const WINDOW *);
extern int getbegx (const WINDOW *);
extern int getbegy (const WINDOW *);
extern int getmaxx (const WINDOW *);
extern int getmaxy (const WINDOW *);
extern int getparx (const WINDOW *);
extern int getpary (const WINDOW *);
// # 1532 "/usr/include/ncursesw/ncurses.h" 3 4
typedef struct
{
    short id;
    int x, y, z;
    mmask_t bstate;
}
MEVENT;

extern int getmouse (MEVENT *);
extern int ungetmouse (MEVENT *);
extern mmask_t mousemask (mmask_t, mmask_t *);
extern _Bool wenclose (const WINDOW *, int, int);
extern int mouseinterval (int);
extern _Bool wmouse_trafo (const WINDOW*, int*, int*, _Bool);
extern _Bool mouse_trafo (int*, int*, _Bool);





extern int mcprint (char *, int);
extern int has_key (int);



extern void _tracef (const char *, ...) ;
extern void _tracedump (const char *, WINDOW *);
extern char * _traceattr (attr_t);
extern char * _traceattr2 (int, chtype);
extern char * _nc_tracebits (void);
extern char * _tracechar (int);
extern char * _tracechtype (chtype);
extern char * _tracechtype2 (int, chtype);
// # 1574 "/usr/include/ncursesw/ncurses.h" 3 4
extern char * _tracemouse (const MEVENT *);
extern void trace (const unsigned int);
// # 41 "mutt_curses.h" 2
// # 76 "mutt_curses.h"
void mutt_curs_set (int);
// # 90 "mutt_curses.h"
event_t mutt_getch (void);

void mutt_endwin (const char *);
void mutt_flushinp (void);
void mutt_refresh (void);
void mutt_resize_screen (void);
void mutt_ungetch (int, int);





enum
{
  MT_COLOR_HDEFAULT = 0,
  MT_COLOR_QUOTED,
  MT_COLOR_SIGNATURE,
  MT_COLOR_INDICATOR,
  MT_COLOR_STATUS,
  MT_COLOR_TREE,
  MT_COLOR_NORMAL,
  MT_COLOR_ERROR,
  MT_COLOR_TILDE,
  MT_COLOR_MARKERS,
  MT_COLOR_BODY,
  MT_COLOR_HEADER,
  MT_COLOR_MESSAGE,
  MT_COLOR_ATTACHMENT,
  MT_COLOR_SEARCH,
  MT_COLOR_BOLD,
  MT_COLOR_UNDERLINE,
  MT_COLOR_INDEX,
  MT_COLOR_MAX
};

typedef struct color_line
{
  regex_t rx;
  char *pattern;
  pattern_t *color_pattern;

  short fg;
  short bg;
  int pair;
  struct color_line *next;
} COLOR_LINE;

extern int *ColorQuote;
extern int ColorQuoteUsed;
extern int ColorDefs[];
extern COLOR_LINE *ColorHdrList;
extern COLOR_LINE *ColorBodyList;
extern COLOR_LINE *ColorIndexList;

void ci_init_color (void);
void ci_start_color (void);
// # 23 "enter.c" 2

// # 1 "history.h" 1
// # 22 "history.h"
enum history_class
{
  HC_CMD,
  HC_ALIAS,
  HC_COMMAND,
  HC_FILE,
  HC_PATTERN,
  HC_OTHER,
  HC_LAST
};



typedef enum history_class history_class_t;

void mutt_init_history(void);
void mutt_history_add(history_class_t, const char *);
char *mutt_history_next(history_class_t);
char *mutt_history_prev(history_class_t);
// # 25 "enter.c" 2




enum
{
  M_REDRAW_INIT = 1,
  M_REDRAW_LINE
};

static int my_wcwidth (wchar_t wc)
{
  int n = wcwidth (wc);
  if ((iswprint(wc) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (wc >= 0xa0))) && n > 0)
    return n;
  if (!(wc & ~0x7f))
    return 2;
  if (!(wc & ~0xffff))
    return 6;
  return 10;
}




static int my_wcswidth (const wchar_t *s, size_t n)
{
  int w = 0;
  while (n--)
    w += my_wcwidth (*s++);
  return w;
}

static int my_addwch (wchar_t wc)
{
  int n = wcwidth (wc);
  if ((iswprint(wc) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (wc >= 0xa0))) && n > 0)
    return mutt_addwch (wc);
  if (!(wc & ~0x7f))
    return printw ("^%c", ((int)wc + 0x40) & 0x7f);
  if (!(wc & ~0xffff))
    return printw ("\\u%04x", (int)wc);
  return printw ("\\u%08x", (int)wc);
}

static size_t width_ceiling (const wchar_t *s, size_t n, int w1)
{
  const wchar_t *s0 = s;
  int w = 0;
  for (; n; s++, n--)
    if ((w += my_wcwidth (*s)) > w1)
      break;
  return s - s0;
}

static void my_wcstombs (char *dest, size_t dlen, const wchar_t *src, size_t slen)
{
  mbstate_t st;
  size_t k;


  memset (&st, 0, sizeof (st));
  for (; slen && dlen >= 16; dest += k, dlen -= k, src++, slen--)
    if ((k = wcrtomb (dest, *src, &st)) == (size_t)(-1))
      break;


  if (dlen >= 16) {
    wcrtomb (dest, 0, &st);
    return;
  }


  {
    char buf[3 * 16];
    char *p = buf;

    for (; slen && p - buf < dlen; p += k, src++, slen--)
      if ((k = wcrtomb (p, *src, &st)) == (size_t)(-1))
 break;
    p += wcrtomb (p, 0, &st);


    if (p - buf <= dlen) {
      memcpy (dest, buf, p - buf);
      return;
    }


    memcpy (dest, buf, dlen);
    dest[dlen - 1] = '\0';
  }
}

size_t my_mbstowcs (wchar_t **pwbuf, size_t *pwbuflen, size_t i, char *buf)
{
  wchar_t wc;
  mbstate_t st;
  size_t k;
  wchar_t *wbuf;
  size_t wbuflen;

  wbuf = *pwbuf, wbuflen = *pwbuflen;
  memset (&st, 0, sizeof (st));
  for (; (k = mbrtowc (&wc, buf, 16, &st)) &&
  k != (size_t)(-1) && k != (size_t)(-2); buf += k)
  {
    if (i >= wbuflen)
    {
      wbuflen = i + 20;
      safe_realloc ((void **) &wbuf, wbuflen * sizeof (*wbuf));
    }
    wbuf[i++] = wc;
  }
  *pwbuf = wbuf, *pwbuflen = wbuflen;
  return i;
}





static void replace_part (ENTER_STATE *state, size_t from, char *buf)
{

  size_t savelen = state->lastchar - state->curpos;
  wchar_t *savebuf = safe_malloc (savelen * sizeof (wchar_t));
  memcpy (savebuf, state->wbuf + state->curpos, savelen * sizeof (wchar_t));


  state->curpos = my_mbstowcs (&state->wbuf, &state->wbuflen, from, buf);


  if (state->curpos + savelen > state->wbuflen)
  {
    state->wbuflen = state->curpos + savelen;
    safe_realloc ((void **) &state->wbuf, state->wbuflen * sizeof (wchar_t));
  }


  memcpy (state->wbuf + state->curpos, savebuf, savelen * sizeof (wchar_t));
  state->lastchar = state->curpos + savelen;

  safe_free ((void **) &savebuf);
}
// # 178 "enter.c"
int mutt_enter_string(char *buf, size_t buflen, int y, int x, int flags)
{
  int rv;
  ENTER_STATE *es = safe_calloc (1, sizeof (ENTER_STATE));
  rv = _mutt_enter_string (buf, buflen, y, x, flags, 0, ((void *)0), ((void *)0), es);
  mutt_free_enter_state (&es);
  return rv;
}

int _mutt_enter_string (char *buf, size_t buflen, int y, int x,
   int flags, int multiple, char ***files, int *numfiles,
   ENTER_STATE *state)
{
  int width = COLS - x - 1;
  int redraw;
  int pass = (flags & (1<<4));
  int first = 1;
  int ch, w, r;
  size_t i;
  wchar_t *tempbuf = 0;
  size_t templen = 0;
  history_class_t hclass;
  wchar_t wc;
  mbstate_t mbstate;

  int rv = 0;
  memset (&mbstate, 0, sizeof (mbstate));

  if (state->wbuf)
  {

    redraw = M_REDRAW_LINE;
  }
  else
  {

    state->wbuflen = 0;
    state->lastchar = my_mbstowcs (&state->wbuf, &state->wbuflen, 0, buf);
    redraw = M_REDRAW_INIT;
  }

  if (flags & ((1<<1) | (1<<2)))
    hclass = HC_FILE;
  else if (flags & (1<<3))
    hclass = HC_CMD;
  else if (flags & 1)
    hclass = HC_ALIAS;
  else if (flags & (1<<6))
    hclass = HC_COMMAND;
  else if (flags & (1<<7))
    hclass = HC_PATTERN;
  else
    hclass = HC_OTHER;

  for (;;)
  {
    if (redraw && !pass)
    {
      if (redraw == M_REDRAW_INIT)
      {

 state->curpos = state->lastchar;
 state->begin = width_ceiling (state->wbuf, state->lastchar, my_wcswidth (state->wbuf, state->lastchar) - width + 1);
      }
      if (state->curpos < state->begin ||
   my_wcswidth (state->wbuf + state->begin, state->curpos - state->begin) >= width)
 state->begin = width_ceiling (state->wbuf, state->lastchar, my_wcswidth (state->wbuf, state->curpos) - width / 2);
      wmove(stdscr,y,x);
      w = 0;
      for (i = state->begin; i < state->lastchar; i++)
      {
 w += my_wcwidth (state->wbuf[i]);
 if (w > width)
   break;
 my_addwch (state->wbuf[i]);
      }
      wclrtoeol(stdscr);
      wmove(stdscr,y,x + my_wcswidth (state->wbuf + state->begin, state->curpos - state->begin));
    }
    mutt_refresh ();

    if ((ch = km_dokey (MENU_EDITOR)) == -1)
    {
      rv = -1;
      goto bye;
    }

    if (ch != OP_NULL)
    {
      first = 0;
      if (ch != OP_EDITOR_COMPLETE)
 state->tabs = 0;
      redraw = M_REDRAW_LINE;
      switch (ch)
      {
 case OP_EDITOR_HISTORY_UP:
   state->curpos = state->lastchar;
   replace_part (state, 0, mutt_history_prev (hclass));
   redraw = M_REDRAW_INIT;
   break;

 case OP_EDITOR_HISTORY_DOWN:
   state->curpos = state->lastchar;
   replace_part (state, 0, mutt_history_next (hclass));
   redraw = M_REDRAW_INIT;
   break;

 case OP_EDITOR_BACKSPACE:
   if (state->curpos == 0)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     i = state->curpos;
     while (i && ((iswprint(state->wbuf[i - 1]) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (state->wbuf[i - 1] >= 0xa0))) && !wcwidth (state->wbuf[i - 1])))
       --i;
     if (i)
       --i;
     memmove (state->wbuf + i, state->wbuf + state->curpos, (state->lastchar - state->curpos) * sizeof (wchar_t));
     state->lastchar -= state->curpos - i;
     state->curpos = i;
   }
   break;

 case OP_EDITOR_BOL:
   state->curpos = 0;
   break;

 case OP_EDITOR_EOL:
   redraw= M_REDRAW_INIT;
   break;

 case OP_EDITOR_KILL_LINE:
   state->curpos = state->lastchar = 0;
   break;

 case OP_EDITOR_KILL_EOL:
   state->lastchar = state->curpos;
   break;

 case OP_EDITOR_BACKWARD_CHAR:
   if (state->curpos == 0)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     while (state->curpos && ((iswprint(state->wbuf[state->curpos - 1]) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (state->wbuf[state->curpos - 1] >= 0xa0))) && !wcwidth (state->wbuf[state->curpos - 1])))
       state->curpos--;
     if (state->curpos)
       state->curpos--;
   }
   break;

 case OP_EDITOR_FORWARD_CHAR:
   if (state->curpos == state->lastchar)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     ++state->curpos;
     while (state->curpos < state->lastchar && ((iswprint(state->wbuf[state->curpos]) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (state->wbuf[state->curpos] >= 0xa0))) && !wcwidth (state->wbuf[state->curpos])))
       ++state->curpos;
   }
   break;

 case OP_EDITOR_BACKWARD_WORD:
   if (state->curpos == 0)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     while (state->curpos && iswspace (state->wbuf[state->curpos - 1]))
       --state->curpos;
     while (state->curpos && !iswspace (state->wbuf[state->curpos - 1]))
       --state->curpos;
   }
   break;

 case OP_EDITOR_FORWARD_WORD:
   if (state->curpos == state->lastchar)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     while (state->curpos < state->lastchar && iswspace (state->wbuf[state->curpos]))
       ++state->curpos;
     while (state->curpos < state->lastchar && !iswspace (state->wbuf[state->curpos]))
       ++state->curpos;
   }
   break;

 case OP_EDITOR_CAPITALIZE_WORD:
 case OP_EDITOR_UPCASE_WORD:
 case OP_EDITOR_DOWNCASE_WORD:
   if (state->curpos == state->lastchar)
   {
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
     break;
   }
   while (state->curpos && !iswspace (state->wbuf[state->curpos]))
     --state->curpos;
   while (state->curpos < state->lastchar && iswspace (state->wbuf[state->curpos]))
     ++state->curpos;
   while (state->curpos < state->lastchar && !iswspace (state->wbuf[state->curpos]))
   {
     if (ch == OP_EDITOR_DOWNCASE_WORD)
       state->wbuf[state->curpos] = towlower (state->wbuf[state->curpos]);
     else
     {
       state->wbuf[state->curpos] = towupper (state->wbuf[state->curpos]);
       if (ch == OP_EDITOR_CAPITALIZE_WORD)
  ch = OP_EDITOR_DOWNCASE_WORD;
     }
     state->curpos++;
   }
   break;

 case OP_EDITOR_DELETE_CHAR:
   if (state->curpos == state->lastchar)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
   {
     i = state->curpos;
     while (i < state->lastchar && ((iswprint(state->wbuf[i]) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (state->wbuf[i] >= 0xa0))) && !wcwidth (state->wbuf[i])))
       ++i;
     if (i < state->lastchar)
       ++i;
     while (i < state->lastchar && ((iswprint(state->wbuf[i]) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (state->wbuf[i] >= 0xa0))) && !wcwidth (state->wbuf[i])))
       ++i;
     memmove (state->wbuf + state->curpos, state->wbuf + i, (state->lastchar - i) * sizeof (wchar_t));
     state->lastchar -= i - state->curpos;
   }
   break;

 case OP_EDITOR_KILL_WORD:

   if (state->curpos != 0)
   {
     i = state->curpos;
     while (i && iswspace (state->wbuf[i - 1]))
       --i;
     if (i)
     {
       if (iswalnum (state->wbuf[i - 1]))
       {
  for (--i; i && iswalnum (state->wbuf[i - 1]); i--)
    ;
       }
       else
  --i;
     }
     memmove (state->wbuf + i, state->wbuf + state->curpos,
       (state->lastchar - state->curpos) * sizeof (wchar_t));
     state->lastchar += i - state->curpos;
     state->curpos = i;
   }
   break;

 case OP_EDITOR_KILL_EOW:

   for (i = state->curpos;
        i < state->lastchar && iswspace (state->wbuf[i]); i++)
     ;
   for (; i < state->lastchar && !iswspace (state->wbuf[i]); i++)
     ;
   memmove (state->wbuf + state->curpos, state->wbuf + i,
     (state->lastchar - i) * sizeof (wchar_t));
   state->lastchar += state->curpos - i;
   break;

 case OP_EDITOR_BUFFY_CYCLE:
   if (flags & (1<<2))
   {
     first = 1;
     my_wcstombs (buf, buflen, state->wbuf, state->curpos);
     mutt_buffy (buf, buflen);
     state->curpos = state->lastchar = my_mbstowcs (&state->wbuf, &state->wbuflen, 0, buf);
     break;
   }
   else if (!(flags & (1<<1)))
     goto self_insert;


 case OP_EDITOR_COMPLETE:
   state->tabs++;
   if (flags & (1<<3))
   {
     for (i = state->curpos; i && state->wbuf[i-1] != ' '; i--)
       ;
     my_wcstombs (buf, buflen, state->wbuf + i, state->curpos - i);
     if (tempbuf && templen == state->lastchar - i &&
  !memcmp (tempbuf, state->wbuf + i, (state->lastchar - i) * sizeof (wchar_t)))
     {
       _mutt_select_file(buf,buflen,(flags & (1<<2)) ? (1<<2) : 0,((void *)0),((void *)0));
       Options[OPTNEEDREDRAW/8] |= (1 << (OPTNEEDREDRAW % 8));
       if (*buf)
  replace_part (state, i, buf);
       rv = 1;
       goto bye;
     }
     if (!mutt_complete (buf, buflen))
     {
       templen = state->lastchar - i;
       safe_realloc ((void **) &tempbuf, templen * sizeof (wchar_t));
     }
     else
       do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);

     replace_part (state, i, buf);
   }
   else if (flags & 1)
   {

     for (i = state->curpos; i && state->wbuf[i-1] != ','; i--)
       ;
     for (; i < state->lastchar && state->wbuf[i] == ' '; i++)
       ;
     my_wcstombs (buf, buflen, state->wbuf + i, state->curpos - i);
     r = mutt_alias_complete (buf, buflen);
     replace_part (state, i, buf);
     if (!r)
     {
       rv = 1;
       goto bye;
     }
     break;
   }
   else if (flags & (1<<6))
   {
     my_wcstombs (buf, buflen, state->wbuf, state->curpos);
     i = strlen (buf);
     if (i && buf[i - 1] == '=' &&
  mutt_var_value_complete (buf, buflen, i))
       state->tabs = 0;
     else if (!mutt_command_complete (buf, buflen, i, state->tabs))
       do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
     replace_part (state, 0, buf);
   }
   else if (flags & ((1<<1) | (1<<2)))
   {
     my_wcstombs (buf, buflen, state->wbuf, state->curpos);


     if ((!tempbuf && !state->lastchar) || (tempbuf && templen == state->lastchar &&
  !memcmp (tempbuf, state->wbuf, state->lastchar * sizeof (wchar_t))))
     {
       _mutt_select_file (buf, buflen,
     ((flags & (1<<2)) ? (1<<2) : 0) | (multiple ? (1<<1) : 0),
     files, numfiles);
       Options[OPTNEEDREDRAW/8] |= (1 << (OPTNEEDREDRAW % 8));
       if (*buf)
       {
  mutt_pretty_mailbox (buf);
  if (!pass)
    mutt_history_add (hclass, buf);
  rv = 0;
  goto bye;
       }


       rv = 1;
       goto bye;
     }

     if (!mutt_complete (buf, buflen))
     {
       templen = state->lastchar;
       safe_realloc ((void **) &tempbuf, templen * sizeof (wchar_t));
       memcpy (tempbuf, state->wbuf, templen * sizeof (wchar_t));
     }
     else
       do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
     replace_part (state, 0, buf);
   }
   else
     goto self_insert;
   break;

 case OP_EDITOR_COMPLETE_QUERY:
   if (flags & 1)
   {

     if ((i = state->curpos))
     {
       for (; i && state->wbuf[i - 1] != ','; i--)
  ;
       for (; i < state->curpos && state->wbuf[i] == ' '; i++)
  ;
     }

     my_wcstombs (buf, buflen, state->wbuf + i, state->curpos - i);
     mutt_query_complete (buf, buflen);
     replace_part (state, i, buf);

     rv = 1;
     goto bye;
   }
   else
     goto self_insert;

 case OP_EDITOR_QUOTE_CHAR:
   {
     event_t event;

     event = mutt_getch ();
     if (event.ch != -1)
     {
       LastKey = event.ch;
       goto self_insert;
     }
   }

 case OP_EDITOR_TRANSPOSE_CHARS:
   if (state->lastchar < 2)
     do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
   else
 {
     wchar_t t;

     if (state->curpos == 0)
       state->curpos = 2;
     else if (state->curpos < state->lastchar)
       ++state->curpos;

     t = state->wbuf[state->curpos - 2];
     state->wbuf[state->curpos - 2] = state->wbuf[state->curpos - 1];
     state->wbuf[state->curpos - 1] = t;
   }
   break;

 default:
   do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
      }
    }
    else
    {

self_insert:

      state->tabs = 0;

      ch = LastKey;



      if (ch == 0527)
 ch = '\r';



      if (ch & ~0xff)
 continue;


      {
 char c;
 size_t k;

 c = ch;
 k = mbrtowc (&wc, &c, 1, &mbstate);
 if (k == (size_t)(-2))
   continue;
 else if (k && k != 1)
 {
   memset (&mbstate, 0, sizeof (mbstate));
   continue;
 }
      }

      if (first && (flags & (1<<5)))
      {
 first = 0;
 if ((iswprint(wc) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (wc >= 0xa0))))
   state->curpos = state->lastchar = 0;
      }

      if (wc == '\r' || wc == '\n')
      {

 my_wcstombs (buf, buflen, state->wbuf, state->lastchar);
 if (!pass)
   mutt_history_add (hclass, buf);

 if (multiple)
 {
   char **tfiles;
   *numfiles = 1;
   tfiles = safe_malloc (*numfiles * sizeof (char *));
   mutt_expand_path (buf, buflen);
   tfiles[0] = safe_strdup (buf);
   *files = tfiles;
 }
 rv = 0;
 goto bye;
      }
      else if (wc && (wc < ' ' || (iswprint(wc) || ((Options[OPTLOCALES/8] & (1 << (OPTLOCALES % 8))) ? 0 : (wc >= 0xa0)))))
      {
 if (state->lastchar >= state->wbuflen)
 {
   state->wbuflen = state->lastchar + 20;
   safe_realloc ((void **) &state->wbuf, state->wbuflen * sizeof (wchar_t));
 }
 memmove (state->wbuf + state->curpos + 1, state->wbuf + state->curpos, (state->lastchar - state->curpos) * sizeof (wchar_t));
 state->wbuf[state->curpos++] = wc;
 state->lastchar++;
      }
      else
      {
 mutt_flushinp ();
 do { if ((Options[OPTBEEP/8] & (1 << (OPTBEEP % 8)))) beep(); } while (0);
      }
    }
  }

  bye:

  safe_free ((void **) &tempbuf);
  return rv;
}

void mutt_free_enter_state (ENTER_STATE **esp)
{
  if (!esp) return;

  safe_free ((void **) &(*esp)->wbuf);
  safe_free ((void **) esp);
}
