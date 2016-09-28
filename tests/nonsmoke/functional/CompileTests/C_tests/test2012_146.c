// # 1 "parse.c"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "parse.c"
// # 19 "parse.c"
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
// # 20 "parse.c" 2

// # 1 "mailbox.h" 1
// # 35 "mailbox.h"
enum
{
  M_NEW_MAIL = 1,
  M_LOCKED,
  M_REOPENED,
  M_FLAGS
};

typedef struct
{
  FILE *fp;
  char *path;
  short magic;
  short write;
  struct {
    unsigned read : 1;
    unsigned flagged : 1;
    unsigned replied : 1;
  } flags;
} MESSAGE;

CONTEXT *mx_open_mailbox (const char *, int, CONTEXT *);

MESSAGE *mx_open_message (CONTEXT *, int);
MESSAGE *mx_open_new_message (CONTEXT *, HEADER *, int);

void mx_fastclose_mailbox (CONTEXT *);

int mx_close_mailbox (CONTEXT *, int *);
int mx_sync_mailbox (CONTEXT *, int *);
int mx_commit_message (MESSAGE *, CONTEXT *);
int mx_close_message (MESSAGE **);
int mx_get_magic (const char *);
int mx_set_magic (const char *);
int mx_check_mailbox (CONTEXT *, int *, int);







int mx_access (const char*, int);
// # 22 "parse.c" 2
// # 1 "mime.h" 1
// # 20 "mime.h"
enum
{
  TYPEOTHER,
  TYPEAUDIO,
  TYPEAPPLICATION,
  TYPEIMAGE,
  TYPEMESSAGE,
  TYPEMODEL,
  TYPEMULTIPART,
  TYPETEXT,
  TYPEVIDEO
};


enum
{
  ENCOTHER,
  ENC7BIT,
  ENC8BIT,
  ENCQUOTEDPRINTABLE,
  ENCBASE64,
  ENCBINARY,
  ENCUUENCODED
};


enum
{
  DISPINLINE,
  DISPATTACH,
  DISPFORMDATA
};




extern int Index_hex[];
extern int Index_64[];
extern char B64Chars[];
// # 69 "mime.h"
extern const char *BodyTypes[];
extern const char *BodyEncodings[];






extern char MimeSpecials[];
// # 23 "parse.c" 2
// # 1 "rfc2047.h" 1
// # 19 "rfc2047.h"
char *mutt_choose_charset (const char *fromcode, const char *charsets,
        char *u, size_t ulen, char **d, size_t *dlen);

void _rfc2047_encode_string (char **, int, int);
void rfc2047_encode_adrlist (ADDRESS *, const char *);



void rfc2047_decode (char **);
void rfc2047_decode_adrlist (ADDRESS *);
// # 24 "parse.c" 2
// # 1 "rfc2231.h" 1
// # 22 "rfc2231.h"
void rfc2231_decode_parameters (PARAMETER **);
int rfc2231_encode_string (char **);
// # 25 "parse.c" 2



// # 1 "pgp.h" 1
// # 22 "pgp.h"
// # 1 "pgplib.h" 1
// # 43 "pgplib.h"
typedef struct pgp_signature
{
  struct pgp_signature *next;
  unsigned char sigtype;
  unsigned long sid1;
  unsigned long sid2;
}
pgp_sig_t;

typedef struct pgp_keyinfo
{
  char *keyid;
  struct pgp_uid *address;
  int flags;
  short keylen;
  time_t gen_time;
  int numalg;
  const char *algorithm;
  struct pgp_keyinfo *parent;
  struct pgp_signature *sigs;
  struct pgp_keyinfo *next;
}
pgp_key_t;

typedef struct pgp_uid
{
  char *addr;
  short trust;
  int flags;
  struct pgp_keyinfo *parent;
  struct pgp_uid *next;
  struct pgp_signature *sigs;
}
pgp_uid_t;

enum pgp_version
{
  PGP_V2,
  PGP_V3,
  PGP_GPG,
  PGP_UNKNOWN
};

enum pgp_ring
{
  PGP_PUBRING,
  PGP_SECRING
};

typedef enum pgp_ring pgp_ring_t;



const char *pgp_pkalgbytype (unsigned char);

pgp_key_t *pgp_remove_key (pgp_key_t **, pgp_key_t *);
pgp_uid_t *pgp_copy_uids (pgp_uid_t *, pgp_key_t *);

short pgp_canencrypt (unsigned char);
short pgp_cansign (unsigned char);
short pgp_get_abilities (unsigned char);

void pgp_free_key (pgp_key_t **kpp);
// # 23 "pgp.h" 2

extern REGEXP PgpGoodSign;

extern char *PgpSignAs;
extern short PgpTimeout;
extern char *PgpEntryFormat;




extern char *PgpClearSignCommand;
extern char *PgpDecodeCommand;
extern char *PgpVerifyCommand;
extern char *PgpDecryptCommand;
extern char *PgpSignCommand;
extern char *PgpEncryptSignCommand;
extern char *PgpEncryptOnlyCommand;
extern char *PgpImportCommand;
extern char *PgpExportCommand;
extern char *PgpVerifyKeyCommand;
extern char *PgpListSecringCommand;
extern char *PgpListPubringCommand;
extern char *PgpGetkeysCommand;



int pgp_check_traditional (FILE *, BODY *, int);
BODY *pgp_decrypt_part (BODY *, STATE *, FILE *, BODY *);
BODY *pgp_make_key_attachment (char *);
const char *pgp_micalg (const char *fname);

char *_pgp_keyid (pgp_key_t *);
char *pgp_keyid (pgp_key_t *);


int mutt_check_pgp (HEADER * h);
int mutt_is_application_pgp (BODY *);
int mutt_is_multipart_encrypted (BODY *);
int mutt_is_multipart_signed (BODY *);
int mutt_parse_pgp_hdr (char *, int);
int pgp_decrypt_mime (FILE *, FILE **, BODY *, BODY **);
int pgp_get_keys (HEADER *, char **);
int pgp_protect (HEADER *, char *);
int pgp_query (BODY *);

int pgp_valid_passphrase (void);


pgp_key_t *pgp_ask_for_key (char *, char *, short, pgp_ring_t);
pgp_key_t *pgp_get_candidates (pgp_ring_t, LIST *);
pgp_key_t *pgp_getkeybyaddr (ADDRESS *, short, pgp_ring_t);
pgp_key_t *pgp_getkeybystr (char *, short, pgp_ring_t);

void mutt_forget_passphrase (void);
void pgp_application_pgp_handler (BODY *, STATE *);
void pgp_encrypted_handler (BODY *, STATE *);
void pgp_extract_keys_from_attachment_list (FILE * fp, int tag, BODY * top);
void pgp_extract_keys_from_messages (HEADER * hdr);
void pgp_signed_handler (BODY *, STATE *);
void pgp_void_passphrase (void);





pid_t pgp_invoke_decode (FILE **pgpin, FILE **pgpout, FILE **pgperr,
    int pgpinfd, int pgpoutfd, int pgperrfd,
    const char *fname, short need_passphrase);
pid_t pgp_invoke_verify (FILE **pgpin, FILE **pgpout, FILE **pgperr,
    int pgpinfd, int pgpoutfd, int pgperrfd,
    const char *fname, const char *sig_fname);
pid_t pgp_invoke_decrypt (FILE **pgpin, FILE **pgpout, FILE **pgperr,
     int pgpinfd, int pgpoutfd, int pgperrfd,
     const char *fname);
pid_t pgp_invoke_sign (FILE **pgpin, FILE **pgpout, FILE **pgperr,
         int pgpinfd, int pgpoutfd, int pgperrfd,
         const char *fname);
pid_t pgp_invoke_encrypt (FILE **pgpin, FILE **pgpout, FILE **pgperr,
     int pgpinfd, int pgpoutfd, int pgperrfd,
     const char *fname, const char *uids, int sign);
pid_t pgp_invoke_export (FILE **pgpin, FILE **pgpout, FILE **pgperr,
    int pgpinfd, int pgpoutfd, int pgperrfd,
    const char *uids);
pid_t pgp_invoke_verify_key (FILE **pgpin, FILE **pgpout, FILE **pgperr,
        int pgpinfd, int pgpoutfd, int pgperrfd,
        const char *uids);
pid_t pgp_invoke_list_keys (FILE **pgpin, FILE **pgpout, FILE **pgperr,
       int pgpinfd, int pgpoutfd, int pgperrfd,
       pgp_ring_t keyring, LIST *hints);
pid_t pgp_invoke_traditional (FILE **pgpin, FILE **pgpout, FILE **pgperr,
     int pgpinfd, int pgpoutfd, int pgperrfd,
     const char *fname, const char *uids, int flags);


void pgp_invoke_import (const char *fname);
void pgp_invoke_getkeys (ADDRESS *);
// # 29 "parse.c" 2





// # 1 "/usr/include/ctype.h" 1 3 4
// # 30 "/usr/include/ctype.h" 3 4

// # 48 "/usr/include/ctype.h" 3 4
enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))
};
// # 81 "/usr/include/ctype.h" 3 4
extern __const unsigned short int **__ctype_b_loc (void)
     __attribute__ ((__const));
extern __const __int32_t **__ctype_tolower_loc (void)
     __attribute__ ((__const));
extern __const __int32_t **__ctype_toupper_loc (void)
     __attribute__ ((__const));
// # 96 "/usr/include/ctype.h" 3 4






extern int isalnum (int) __attribute__ ((__nothrow__));
extern int isalpha (int) __attribute__ ((__nothrow__));
extern int iscntrl (int) __attribute__ ((__nothrow__));
extern int isdigit (int) __attribute__ ((__nothrow__));
extern int islower (int) __attribute__ ((__nothrow__));
extern int isgraph (int) __attribute__ ((__nothrow__));
extern int isprint (int) __attribute__ ((__nothrow__));
extern int ispunct (int) __attribute__ ((__nothrow__));
extern int isspace (int) __attribute__ ((__nothrow__));
extern int isupper (int) __attribute__ ((__nothrow__));
extern int isxdigit (int) __attribute__ ((__nothrow__));



extern int tolower (int __c) __attribute__ ((__nothrow__));


extern int toupper (int __c) __attribute__ ((__nothrow__));


// # 142 "/usr/include/ctype.h" 3 4
extern int isascii (int __c) __attribute__ ((__nothrow__));



extern int toascii (int __c) __attribute__ ((__nothrow__));



extern int _toupper (int) __attribute__ ((__nothrow__));
extern int _tolower (int) __attribute__ ((__nothrow__));
// # 323 "/usr/include/ctype.h" 3 4

// # 35 "parse.c" 2







static char *read_rfc822_line (FILE *f, char *line, size_t *linelen)
{
  char *buf = line;
  char ch;
  size_t offset = 0;

  while (1)
  {
    if (fgets (buf, *linelen - offset, f) == ((void *)0) ||
 (((*__ctype_b_loc ())[(int) (((unsigned char)*line))] & (unsigned short int) _ISspace) && !offset))
    {
      *line = 0;
      return (line);
    }

    buf += strlen (buf) - 1;
    if (*buf == '\n')
    {

      while (((*__ctype_b_loc ())[(int) (((unsigned char)*buf))] & (unsigned short int) _ISspace))
 *buf-- = 0;



      if ((ch = fgetc (f)) != ' ' && ch != '\t')
      {
 ungetc (ch, f);
 return (line);
      }


      while ((ch = fgetc (f)) == ' ' || ch == '\t')
 ;
      ungetc (ch, f);
      *++buf = ' ';

    }

    buf++;
    offset = buf - line;
    if (*linelen < offset + 256)
    {

      *linelen += 256;
      safe_realloc ((void **) &line, *linelen);
      buf = line + offset;
    }
  }

}

static LIST *mutt_parse_references (char *s, int in_reply_to)
{
  LIST *t, *lst = ((void *)0);
  int m, n = 0;
  char *o = ((void *)0), *new, *at;

  while ((s = strtok (s, " \t;")) != ((void *)0))
  {






    t = ((void *)0);
    new = ((void *)0);

    if (*s == '<')
    {
      n = strlen (s);
      if (s[n-1] != '>')
      {
 o = s;
 s = ((void *)0);
 continue;
      }

      new = safe_strdup (s);
    }
    else if (o)
    {
      m = strlen (s);
      if (s[m - 1] == '>')
      {
 new = safe_malloc (sizeof (char) * (n + m + 1));
 strcpy (new, o);
 strcpy (new + n, s);
      }
    }
    if (new)
    {






      if (!(at = strchr (new, '@')) || strchr (at + 1, '@')
   || (in_reply_to && at - new <= 8))
 safe_free ((void **) &new);
      else
      {
 t = (LIST *) safe_malloc (sizeof (LIST));
 t->data = new;
 t->next = lst;
 lst = t;
      }
    }
    o = ((void *)0);
    s = ((void *)0);
  }

  return (lst);
}

int mutt_check_encoding (const char *c)
{
  if (ascii_strncasecmp ("7bit", c, sizeof ("7bit")-1) == 0)
    return (ENC7BIT);
  else if (ascii_strncasecmp ("8bit", c, sizeof ("8bit")-1) == 0)
    return (ENC8BIT);
  else if (ascii_strncasecmp ("binary", c, sizeof ("binary")-1) == 0)
    return (ENCBINARY);
  else if (ascii_strncasecmp ("quoted-printable", c, sizeof ("quoted-printable")-1) == 0)
    return (ENCQUOTEDPRINTABLE);
  else if (ascii_strncasecmp ("base64", c, sizeof("base64")-1) == 0)
    return (ENCBASE64);
  else if (ascii_strncasecmp ("x-uuencode", c, sizeof("x-uuencode")-1) == 0)
    return (ENCUUENCODED);




  else
    return (ENCOTHER);
}

static PARAMETER *parse_parameters (const char *s)
{
  PARAMETER *head = 0, *cur = 0, *new;
  char buffer[1024];
  const char *p;
  size_t i;

  ;

  while (*s)
  {
    if ((p = strpbrk (s, "=;")) == ((void *)0))
    {
      ;
      goto bail;
    }


    if (*p != ';')
    {
      i = p - s;

      new = safe_calloc (1, sizeof (PARAMETER));

      new->attribute = safe_malloc (i + 1);
      memcpy (new->attribute, s, i);
      new->attribute[i] = 0;


      while (((*__ctype_b_loc ())[(int) (((unsigned char)new->attribute[--i]))] & (unsigned short int) _ISspace))
 new->attribute[i] = 0;

      s = p + 1;
      while (*(s) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(s)))] & (unsigned short int) _ISspace)) s++;;

      if (*s == '"')
      {
 s++;
 for (i=0; *s && *s != '"' && i < sizeof (buffer) - 1; i++, s++)
 {
   if (*s == '\\')
   {

     buffer[i] = s[1];
     if (!*++s)
       break;
   }
   else
     buffer[i] = *s;
 }
 buffer[i] = 0;
 if (*s)
   s++;
      }
      else
      {
 for (i=0; *s && *s != ' ' && *s != ';' && i < sizeof (buffer) - 1; i++, s++)
   buffer[i] = *s;
 buffer[i] = 0;
      }

      new->value = safe_strdup (buffer);

      ;




      if (head)
      {
 cur->next = new;
 cur = cur->next;
      }
      else
 head = cur = new;
    }
    else
    {
      ;
      s = p;
    }


    if (*s != ';' && (s = strchr (s, ';')) == ((void *)0))
 break;

    do
    {
      s++;


      while (*(s) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(s)))] & (unsigned short int) _ISspace)) s++;;
    }
    while (*s == ';');
  }

  bail:

  rfc2231_decode_parameters (&head);
  return (head);
}

int mutt_check_mime_type (const char *s)
{
  if (ascii_strcasecmp ("text", s) == 0)
    return TYPETEXT;
  else if (ascii_strcasecmp ("multipart", s) == 0)
    return TYPEMULTIPART;




  else if (ascii_strcasecmp ("application", s) == 0)
    return TYPEAPPLICATION;
  else if (ascii_strcasecmp ("message", s) == 0)
    return TYPEMESSAGE;
  else if (ascii_strcasecmp ("image", s) == 0)
    return TYPEIMAGE;
  else if (ascii_strcasecmp ("audio", s) == 0)
    return TYPEAUDIO;
  else if (ascii_strcasecmp ("video", s) == 0)
    return TYPEVIDEO;
  else if (ascii_strcasecmp ("model", s) == 0)
    return TYPEMODEL;
  else
    return TYPEOTHER;
}

void mutt_parse_content_type (char *s, BODY *ct)
{
  char *pc;
  char *subtype;

  safe_free((void **)&ct->subtype);
  mutt_free_parameter(&ct->parameter);


  if ((pc = strchr(s, ';')) != ((void *)0))
  {
    *pc++ = 0;
    while (*pc && ((*__ctype_b_loc ())[(int) (((unsigned char)*pc))] & (unsigned short int) _ISspace))
      pc++;
    ct->parameter = parse_parameters(pc);




    if ((pc = mutt_get_parameter( "name", ct->parameter)) != 0 && !ct->filename)
      ct->filename = safe_strdup(pc);







  }


  if ((subtype = strchr(s, '/')))
  {
    *subtype++ = '\0';
    for(pc = subtype; *pc && !((*__ctype_b_loc ())[(int) (((unsigned char)*pc))] & (unsigned short int) _ISspace) && *pc != ';'; pc++)
      ;
    *pc = '\0';
    ct->subtype = safe_strdup (subtype);
  }


  ct->type = mutt_check_mime_type (s);






  if (ct->type == TYPEOTHER)
  {
    ct->xtype = safe_strdup (s);
  }

  if (ct->subtype == ((void *)0))
  {



    if (ct->type == TYPETEXT)
      ct->subtype = safe_strdup ("plain");
    else if (ct->type == TYPEAUDIO)
      ct->subtype = safe_strdup ("basic");
    else if (ct->type == TYPEMESSAGE)
      ct->subtype = safe_strdup ("rfc822");
    else if (ct->type == TYPEOTHER)
    {
      char buffer[128];

      ct->type = TYPEAPPLICATION;
      snprintf (buffer, sizeof (buffer), "x-%s", s);
      ct->subtype = safe_strdup (buffer);
    }
    else
      ct->subtype = safe_strdup ("x-unknown");
  }


  if (ct->type == TYPETEXT)
  {
    if (!(pc = mutt_get_parameter ("charset", ct->parameter)))
      mutt_set_parameter ("charset", "us-ascii", &ct->parameter);
  }

}

static void parse_content_disposition (char *s, BODY *ct)
{
  PARAMETER *parms;

  if (!ascii_strncasecmp ("inline", s, 6))
    ct->disposition = DISPINLINE;
  else if (!ascii_strncasecmp ("form-data", s, 9))
    ct->disposition = DISPFORMDATA;
  else
    ct->disposition = DISPATTACH;


  if ((s = strchr (s, ';')) != ((void *)0))
  {
    s++;
    while (*(s) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(s)))] & (unsigned short int) _ISspace)) s++;;
    if ((s = mutt_get_parameter ("filename", (parms = parse_parameters (s)))) != 0)
      mutt_str_replace (&ct->filename, s);
    if ((s = mutt_get_parameter ("name", parms)) != 0)
      ct->form_name = safe_strdup (s);
    mutt_free_parameter (&parms);
  }
}
// # 424 "parse.c"
BODY *mutt_read_mime_header (FILE *fp, int digest)
{
  BODY *p = mutt_new_body();
  char *c;
  char *line = safe_malloc (1024);
  size_t linelen = 1024;

  p->hdr_offset = ftell(fp);

  p->encoding = ENC7BIT;
  p->type = digest ? TYPEMESSAGE : TYPETEXT;
  p->disposition = DISPINLINE;

  while (*(line = read_rfc822_line (fp, line, &linelen)) != 0)
  {

    if ((c = strchr (line, ':')))
    {
      *c = 0;
      c++;
      while (*(c) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(c)))] & (unsigned short int) _ISspace)) c++;;
      if (!*c)
      {
 ;
 continue;
      }
    }
    else
    {
      ;
      break;
    }

    if (!ascii_strncasecmp ("content-", line, 8))
    {
      if (!ascii_strcasecmp ("type", line + 8))
 mutt_parse_content_type (c, p);
      else if (!ascii_strcasecmp ("transfer-encoding", line + 8))
 p->encoding = mutt_check_encoding (c);
      else if (!ascii_strcasecmp ("disposition", line + 8))
 parse_content_disposition (c, p);
      else if (!ascii_strcasecmp ("description", line + 8))
      {
 mutt_str_replace (&p->description, c);
 rfc2047_decode (&p->description);
      }
    }
// # 487 "parse.c"
  }
  p->offset = ftell (fp);
  if (p->type == TYPETEXT && !p->subtype)
    p->subtype = safe_strdup ("plain");
  else if (p->type == TYPEMESSAGE && !p->subtype)
    p->subtype = safe_strdup ("rfc822");

  safe_free((void **)&line);

  return (p);
}

void mutt_parse_part (FILE *fp, BODY *b)
{
  char *bound = 0;

  switch (b->type)
  {
    case TYPEMULTIPART:





          bound = mutt_get_parameter ("boundary", b->parameter);

      fseek (fp, b->offset, 0);
      b->parts = mutt_parse_multipart (fp, bound,
     b->offset + b->length,
     ascii_strcasecmp ("digest", b->subtype) == 0);
      break;

    case TYPEMESSAGE:
      if (b->subtype)
      {
 fseek (fp, b->offset, 0);
 if (mutt_is_message_type(b->type, b->subtype))
   b->parts = mutt_parse_messageRFC822 (fp, b);
 else if (ascii_strcasecmp (b->subtype, "external-body") == 0)
   b->parts = mutt_read_mime_header (fp, 0);
 else
   return;
      }
      break;

    default:
      return;
  }


  if (!b->parts)
  {
    b->type = TYPETEXT;
    mutt_str_replace (&b->subtype, "plain");
  }
}
// # 555 "parse.c"
BODY *mutt_parse_messageRFC822 (FILE *fp, BODY *parent)
{
  BODY *msg;

  parent->hdr = safe_calloc (1, sizeof (HEADER));
  parent->hdr->offset = ftell (fp);
  parent->hdr->env = mutt_read_rfc822_header (fp, parent->hdr, 0, 0);
  msg = parent->hdr->content;




  msg->length = parent->length - (msg->offset - parent->offset);


  if (msg->length < 0)
    msg->length = 0;

  mutt_parse_part(fp, msg);
  return (msg);
}
// # 590 "parse.c"
BODY *mutt_parse_multipart (FILE *fp, const char *boundary, long end_off, int digest)
{



  int blen, len, crlf = 0;
  char buffer[1024];
  BODY *head = 0, *last = 0, *new = 0;
  int i;
  int final = 0;

  if (!boundary)
  {
    mutt_error (gettext ("multipart message has no boundary parameter!"));
    return (((void *)0));
  }

  blen = mutt_strlen (boundary);
  while (ftell (fp) < end_off && fgets (buffer, 1024, fp) != ((void *)0))
  {
    len = mutt_strlen (buffer);

    crlf = (len > 1 && buffer[len - 2] == '\r') ? 1 : 0;

    if (buffer[0] == '-' && buffer[1] == '-' &&
 mutt_strncmp (buffer + 2, boundary, blen) == 0)
    {
      if (last)
      {
 last->length = ftell (fp) - last->offset - len - 1 - crlf;
 if (last->parts && last->parts->length == 0)
   last->parts->length = ftell (fp) - last->parts->offset - len - 1 - crlf;

 if (last->length < 0)
   last->length = 0;
      }


      for (i = len - 1; ((*__ctype_b_loc ())[(int) (((unsigned char)buffer[i]))] & (unsigned short int) _ISspace) && i >= blen + 2; i--)
        buffer[i] = 0;


      if (mutt_strcmp (buffer + blen + 2, "--") == 0)
      {
 final = 1;
 break;
      }
      else if (buffer[2 + blen] == 0)
      {
 new = mutt_read_mime_header (fp, digest);
// # 655 "parse.c"
 if(new->offset > end_off)
 {
   mutt_free_body(&new);
   break;
 }
 if (head)
 {
   last->next = new;
   last = new;
 }
 else
   last = head = new;
      }
    }
  }


  if (last && last->length == 0 && !final)
    last->length = end_off - last->offset;


  for(last = head; last; last = last->next)
    mutt_parse_part(fp, last);

  return (head);
}

static const char *uncomment_timezone (char *buf, size_t buflen, const char *tz)
{
  char *p;
  size_t len;

  if (*tz != '(')
    return tz;
  tz++;
  while (*(tz) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(tz)))] & (unsigned short int) _ISspace)) tz++;;
  if ((p = strpbrk (tz, " )")) == ((void *)0))
    return tz;
  len = p - tz;
  if (len > buflen - 1)
    len = buflen - 1;
  memcpy (buf, tz, len);
  buf[len] = 0;
  return buf;
}

static struct tz_t
{
  char tzname[5];
  unsigned char zhours;
  unsigned char zminutes;
  unsigned char zoccident;
}
TimeZones[] =
{
  { "aat", 1, 0, 1 },
  { "adt", 4, 0, 0 },
  { "ast", 3, 0, 0 },

  { "bst", 1, 0, 0 },
  { "cat", 1, 0, 0 },
  { "cdt", 5, 0, 1 },
  { "cest", 2, 0, 0 },
  { "cet", 1, 0, 0 },
  { "cst", 6, 0, 1 },


  { "eat", 3, 0, 0 },
  { "edt", 4, 0, 1 },
  { "eest", 3, 0, 0 },
  { "eet", 2, 0, 0 },
  { "egst", 0, 0, 0 },
  { "egt", 1, 0, 1 },
  { "est", 5, 0, 1 },
  { "gmt", 0, 0, 0 },
  { "gst", 4, 0, 0 },
  { "hkt", 8, 0, 0 },
  { "ict", 7, 0, 0 },
  { "idt", 3, 0, 0 },
  { "ist", 2, 0, 0 },

  { "jst", 9, 0, 0 },
  { "kst", 9, 0, 0 },
  { "mdt", 6, 0, 1 },
  { "met", 1, 0, 0 },
  { "msd", 4, 0, 0 },
  { "msk", 3, 0, 0 },
  { "mst", 7, 0, 1 },
  { "nzdt", 13, 0, 0 },
  { "nzst", 12, 0, 0 },
  { "pdt", 7, 0, 1 },
  { "pst", 8, 0, 1 },
  { "sat", 2, 0, 0 },
  { "smt", 4, 0, 0 },
  { "sst", 11, 0, 1 },

  { "utc", 0, 0, 0 },
  { "wat", 0, 0, 0 },
  { "west", 1, 0, 0 },
  { "wet", 0, 0, 0 },
  { "wgst", 2, 0, 1 },
  { "wgt", 3, 0, 1 },
  { "wst", 8, 0, 0 },
};
// # 767 "parse.c"
time_t mutt_parse_date (const char *s, HEADER *h)
{
  int count = 0;
  char *t;
  int hour, min, sec;
  struct tm tm;
  int i;
  int tz_offset = 0;
  int zhours = 0;
  int zminutes = 0;
  int zoccident = 0;
  const char *ptz;
  char tzstr[128];
  char scratch[128];





  strncpy(scratch,s,sizeof (scratch)), *(scratch+(sizeof (scratch))-1)=0;


  if ((t = strchr (scratch, ',')))
    t++;
  else
    t = scratch;
  while (*(t) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(t)))] & (unsigned short int) _ISspace)) t++;;

  memset (&tm, 0, sizeof (tm));

  while ((t = strtok (t, " \t")) != ((void *)0))
  {
    switch (count)
    {
      case 0:
 if (!((*__ctype_b_loc ())[(int) (((unsigned char) *t))] & (unsigned short int) _ISdigit))
   return (-1);
 tm.tm_mday = atoi (t);
 if (tm.tm_mday > 31)
   return (-1);
 break;

      case 1:
 if ((i = mutt_check_month (t)) < 0)
   return (-1);
 tm.tm_mon = i;
 break;

      case 2:
 tm.tm_year = atoi (t);
        if (tm.tm_year < 50)
   tm.tm_year += 100;
        else if (tm.tm_year >= 1900)
   tm.tm_year -= 1900;
 break;

      case 3:
 if (sscanf (t, "%d:%d:%d", &hour, &min, &sec) == 3)
   ;
 else if (sscanf (t, "%d:%d", &hour, &min) == 2)
   sec = 0;
 else
 {
   ;
   return(-1);
 }
 tm.tm_hour = hour;
 tm.tm_min = min;
 tm.tm_sec = sec;
 break;

      case 4:



 ptz = uncomment_timezone (tzstr, sizeof (tzstr), t);

 if (*ptz == '+' || *ptz == '-')
 {
   if (ptz[1] && ptz[2] && ptz[3] && ptz[4]
       && ((*__ctype_b_loc ())[(int) (((unsigned char) ptz[1]))] & (unsigned short int) _ISdigit) && ((*__ctype_b_loc ())[(int) (((unsigned char) ptz[2]))] & (unsigned short int) _ISdigit)
       && ((*__ctype_b_loc ())[(int) (((unsigned char) ptz[3]))] & (unsigned short int) _ISdigit) && ((*__ctype_b_loc ())[(int) (((unsigned char) ptz[4]))] & (unsigned short int) _ISdigit))
   {
     zhours = (ptz[1] - '0') * 10 + (ptz[2] - '0');
     zminutes = (ptz[3] - '0') * 10 + (ptz[4] - '0');

     if (ptz[0] == '-')
       zoccident = 1;
   }
 }
 else
 {
   struct tz_t *tz;

   tz = bsearch (ptz, TimeZones, sizeof TimeZones/sizeof (struct tz_t),
   sizeof (struct tz_t),
   (int (*)(const void *, const void *)) ascii_strcasecmp

                                      );

   if (tz)
   {
     zhours = tz->zhours;
     zminutes = tz->zminutes;
     zoccident = tz->zoccident;
   }


   if (ascii_strcasecmp (t, "MET") == 0)
   {
     if ((t = strtok (((void *)0), " \t")) != ((void *)0))
     {
       if (!ascii_strcasecmp (t, "DST"))
  zhours++;
     }
   }
 }
 tz_offset = zhours * 3600 + zminutes * 60;
 if (!zoccident)
   tz_offset = -tz_offset;
 break;
    }
    count++;
    t = 0;
  }

  if (count < 4)
  {
    ;
    return (-1);
  }

  if (h)
  {
    h->zhours = zhours;
    h->zminutes = zminutes;
    h->zoccident = zoccident;
  }

  return (mutt_mktime (&tm, 0) + tz_offset);
}


static char *extract_message_id (const char *s)
{
  const char *p;
  char *r;
  size_t l;

  if ((s = strchr (s, '<')) == ((void *)0) || (p = strchr (s, '>')) == ((void *)0))
    return (((void *)0));
  l = (size_t)(p - s) + 1;
  r = safe_malloc (l + 1);
  memcpy (r, s, l);
  r[l] = 0;
  return (r);
}

void mutt_parse_mime_message (CONTEXT *ctx, HEADER *cur)
{
  MESSAGE *msg;

  if (cur->content->type != TYPEMESSAGE && cur->content->type != TYPEMULTIPART)
    return;

  if (cur->content->parts)
    return;

  if ((msg = mx_open_message (ctx, cur->msgno)))
  {
    mutt_parse_part (msg->fp, cur->content);



    cur->pgp = pgp_query (cur->content);



    mx_close_message (&msg);
  }
}

int mutt_parse_rfc822_line (ENVELOPE *e, HEADER *hdr, char *line, char *p, short user_hdrs, short weed,
       short do_2047, LIST **lastp)
{
  int matched = 0;
  LIST *last = ((void *)0);

  if (lastp)
    last = *lastp;

  switch (ascii_tolower (line[0]))
  {
    case 'a':
    if (ascii_strcasecmp (line+1, "pparently-to") == 0)
    {
      e->to = rfc822_parse_adrlist (e->to, p);
      matched = 1;
    }
    else if (ascii_strcasecmp (line+1, "pparently-from") == 0)
    {
      e->from = rfc822_parse_adrlist (e->from, p);
      matched = 1;
    }
    break;

    case 'b':
    if (ascii_strcasecmp (line+1, "cc") == 0)
    {
      e->bcc = rfc822_parse_adrlist (e->bcc, p);
      matched = 1;
    }
    break;

    case 'c':
    if (ascii_strcasecmp (line+1, "c") == 0)
    {
      e->cc = rfc822_parse_adrlist (e->cc, p);
      matched = 1;
    }
    else if (ascii_strncasecmp (line + 1, "ontent-", 7) == 0)
    {
      if (ascii_strcasecmp (line+8, "type") == 0)
      {
 if (hdr)
   mutt_parse_content_type (p, hdr->content);
 matched = 1;
      }
      else if (ascii_strcasecmp (line+8, "transfer-encoding") == 0)
      {
 if (hdr)
   hdr->content->encoding = mutt_check_encoding (p);
 matched = 1;
      }
      else if (ascii_strcasecmp (line+8, "length") == 0)
      {
 if (hdr)
 {
   if ((hdr->content->length = atoi (p)) < 0)
     hdr->content->length = -1;
 }
 matched = 1;
      }
      else if (ascii_strcasecmp (line+8, "description") == 0)
      {
 if (hdr)
 {
   mutt_str_replace (&hdr->content->description, p);
   rfc2047_decode (&hdr->content->description);
 }
 matched = 1;
      }
      else if (ascii_strcasecmp (line+8, "disposition") == 0)
      {
 if (hdr)
   parse_content_disposition (p, hdr->content);
 matched = 1;
      }
    }
    break;

    case 'd':
    if (!ascii_strcasecmp ("ate", line + 1))
    {
      mutt_str_replace (&e->date, p);
      if (hdr)
 hdr->date_sent = mutt_parse_date (p, hdr);
      matched = 1;
    }
    break;

    case 'e':
    if (!ascii_strcasecmp ("xpires", line + 1) &&
 hdr && mutt_parse_date (p, ((void *)0)) < time (((void *)0)))
      hdr->expired = 1;
    break;

    case 'f':
    if (!ascii_strcasecmp ("rom", line + 1))
    {
      e->from = rfc822_parse_adrlist (e->from, p);
      matched = 1;
    }
    break;

    case 'i':
    if (!ascii_strcasecmp (line+1, "n-reply-to"))
    {
      mutt_free_list (&e->in_reply_to);
      e->in_reply_to = mutt_parse_references (p, 1);
      matched = 1;
    }
    break;

    case 'l':
    if (!ascii_strcasecmp (line + 1, "ines"))
    {
      if (hdr)
 hdr->lines = atoi (p);
      matched = 1;
    }
    break;

    case 'm':
    if (!ascii_strcasecmp (line + 1, "ime-version"))
    {
      if (hdr)
 hdr->mime = 1;
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "essage-id"))
    {

      safe_free ((void **) &e->message_id);
      e->message_id = extract_message_id (p);
      matched = 1;
    }
    else if (!ascii_strncasecmp (line + 1, "ail-", 4))
    {
      if (!ascii_strcasecmp (line + 5, "reply-to"))
      {

 rfc822_free_address (&e->reply_to);
 e->reply_to = rfc822_parse_adrlist (e->reply_to, p);
 matched = 1;
      }
      else if (!ascii_strcasecmp (line + 5, "followup-to"))
      {
 e->mail_followup_to = rfc822_parse_adrlist (e->mail_followup_to, p);
 matched = 1;
      }
    }
    break;

    case 'r':
    if (!ascii_strcasecmp (line + 1, "eferences"))
    {
      mutt_free_list (&e->references);
      e->references = mutt_parse_references (p, 0);
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "eply-to"))
    {
      e->reply_to = rfc822_parse_adrlist (e->reply_to, p);
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "eturn-path"))
    {
      e->return_path = rfc822_parse_adrlist (e->return_path, p);
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "eceived"))
    {
      if (hdr && !hdr->received)
      {
 char *d = strchr (p, ';');

 if (d)
   hdr->received = mutt_parse_date (d + 1, ((void *)0));
      }
    }
    break;

    case 's':
    if (!ascii_strcasecmp (line + 1, "ubject"))
    {
      if (!e->subject)
 e->subject = safe_strdup (p);
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "ender"))
    {
      e->sender = rfc822_parse_adrlist (e->sender, p);
      matched = 1;
    }
    else if (!ascii_strcasecmp (line + 1, "tatus"))
    {
      if (hdr)
      {
 while (*p)
 {
   switch(*p)
   {
     case 'r':
     hdr->replied = 1;
     break;
     case 'O':
     if ((Options[OPTMARKOLD/8] & (1 << (OPTMARKOLD % 8))))
       hdr->old = 1;
     break;
     case 'R':
     hdr->read = 1;
     break;
   }
   p++;
 }
      }
      matched = 1;
    }
    else if ((!ascii_strcasecmp ("upersedes", line + 1) ||
       !ascii_strcasecmp ("upercedes", line + 1)) && hdr)
      e->supersedes = safe_strdup (p);
    break;

    case 't':
    if (ascii_strcasecmp (line+1, "o") == 0)
    {
      e->to = rfc822_parse_adrlist (e->to, p);
      matched = 1;
    }
    break;

    case 'x':
    if (ascii_strcasecmp (line+1, "-status") == 0)
    {
      if (hdr)
      {
 while (*p)
 {
   switch (*p)
   {
     case 'A':
     hdr->replied = 1;
     break;
     case 'D':
     hdr->deleted = 1;
     break;
     case 'F':
     hdr->flagged = 1;
     break;
     default:
     break;
   }
   p++;
 }
      }
      matched = 1;
    }
    else if (ascii_strcasecmp (line+1, "-label") == 0)
    {
      e->x_label = safe_strdup(p);
      matched = 1;
    }

    default:
    break;
  }


  if (!matched && user_hdrs)
  {

    line[strlen (line)] = ':';

    if (weed && (Options[OPTWEED/8] & (1 << (OPTWEED % 8))) && mutt_matches_ignore (line, Ignore)
 && !mutt_matches_ignore (line, UnIgnore))
      goto done;

    if (last)
    {
      last->next = safe_calloc (1, sizeof (LIST));
      last = last->next;
    }
    else
      last = e->userhdrs = safe_calloc (1, sizeof (LIST));
    last->data = safe_strdup (line);
    if (do_2047)
      rfc2047_decode (&last->data);
  }

  done:

  *lastp = last;
  return matched;
}
// # 1260 "parse.c"
ENVELOPE *mutt_read_rfc822_header (FILE *f, HEADER *hdr, short user_hdrs,
       short weed)
{
  ENVELOPE *e = safe_calloc (1, sizeof (ENVELOPE));
  LIST *last = ((void *)0);
  char *line = safe_malloc (1024);
  char *p;
  long loc;
  int matched;
  size_t linelen = 1024;

  if (hdr)
  {
    if (hdr->content == ((void *)0))
    {
      hdr->content = mutt_new_body ();


      hdr->content->type = TYPETEXT;
      hdr->content->subtype = safe_strdup ("plain");
      hdr->content->encoding = ENC7BIT;
      hdr->content->length = -1;


      hdr->content->disposition = DISPINLINE;
    }
  }

  while ((loc = ftell (f)),
   *(line = read_rfc822_line (f, line, &linelen)) != 0)
  {
    matched = 0;

    if ((p = strpbrk (line, ": \t")) == ((void *)0) || *p != ':')
    {
      char return_path[1024];
      time_t t;


      if (mutt_strncmp (">From ", line, 6) == 0)
 continue;
      else if (is_from (line, return_path, sizeof (return_path), &t))
      {

 if (hdr && !hdr->received)
   hdr->received = t - mutt_local_tz (t);
 continue;
      }

      fseek (f, loc, 0);
      break;
    }

    *p = 0;
    p++;
    while (*(p) && ((*__ctype_b_loc ())[(int) (((unsigned char) *(p)))] & (unsigned short int) _ISspace)) p++;;
    if (!*p)
      continue;

    matched = mutt_parse_rfc822_line (e, hdr, line, p, user_hdrs, weed, 1, &last);

  }

  safe_free((void **)&line);

  if (hdr)
  {
    hdr->content->hdr_offset = hdr->offset;
    hdr->content->offset = ftell (f);


    rfc2047_decode_adrlist (e->from);
    rfc2047_decode_adrlist (e->to);
    rfc2047_decode_adrlist (e->cc);
    rfc2047_decode_adrlist (e->reply_to);
    rfc2047_decode_adrlist (e->mail_followup_to);
    rfc2047_decode_adrlist (e->return_path);
    rfc2047_decode_adrlist (e->sender);

    if (e->subject)
    {
      regmatch_t pmatch[1];

      rfc2047_decode (&e->subject);

      if (regexec (ReplyRegexp.rx, e->subject, 1, pmatch, 0) == 0)
 e->real_subj = e->subject + pmatch[0].rm_eo;
      else
 e->real_subj = e->subject;
    }


    if (hdr->date_sent <= 0)
    {
      ;
      hdr->date_sent = hdr->received;
    }
  }

  return (e);
}

ADDRESS *mutt_parse_adrlist (ADDRESS *p, const char *s)
{
  const char *q;


  if ((q = strpbrk (s, "\"<>():;,\\")) == ((void *)0))
  {
    char tmp[5120];
    char *r;

    strncpy(tmp,s,sizeof (tmp)), *(tmp+(sizeof (tmp))-1)=0;
    r = tmp;
    while ((r = strtok (r, " \t")) != ((void *)0))
    {
      p = rfc822_parse_adrlist (p, r);
      r = ((void *)0);
    }
  }
  else
    p = rfc822_parse_adrlist (p, s);

  return p;
}
