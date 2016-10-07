// # 1 "HTFile.c"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "HTFile.c"
// # 30 "HTFile.c"
// # 1 "wwwsys.h" 1
// # 132 "wwwsys.h"
// # 1 "../../wwwconf.h" 1
// # 133 "wwwsys.h" 2
// # 782 "wwwsys.h"
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

// # 783 "wwwsys.h" 2




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



// # 788 "wwwsys.h" 2
// # 796 "wwwsys.h"
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

// # 797 "wwwsys.h" 2
// # 805 "wwwsys.h"
// # 1 "/usr/include/fcntl.h" 1 3 4
// # 30 "/usr/include/fcntl.h" 3 4




// # 1 "/usr/include/bits/fcntl.h" 1 3 4
// # 25 "/usr/include/bits/fcntl.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 26 "/usr/include/bits/fcntl.h" 2 3 4
// # 155 "/usr/include/bits/fcntl.h" 3 4
struct flock
  {
    short int l_type;
    short int l_whence;

    __off_t l_start;
    __off_t l_len;




    __pid_t l_pid;
  };
// # 222 "/usr/include/bits/fcntl.h" 3 4

// # 251 "/usr/include/bits/fcntl.h" 3 4

// # 35 "/usr/include/fcntl.h" 2 3 4
// # 76 "/usr/include/fcntl.h" 3 4
extern int fcntl (int __fd, int __cmd, ...);
// # 85 "/usr/include/fcntl.h" 3 4
extern int open (__const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
// # 130 "/usr/include/fcntl.h" 3 4
extern int creat (__const char *__file, __mode_t __mode) __attribute__ ((__nonnull__ (1)));
// # 176 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise (int __fd, __off_t __offset, __off_t __len,
     int __advise) __attribute__ ((__nothrow__));
// # 198 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate (int __fd, __off_t __offset, __off_t __len);
// # 213 "/usr/include/fcntl.h" 3 4

// # 806 "wwwsys.h" 2
// # 822 "wwwsys.h"
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
// # 823 "wwwsys.h" 2





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

// # 829 "wwwsys.h" 2
// # 845 "wwwsys.h"
// # 1 "/usr/include/netinet/in.h" 1 3 4
// # 23 "/usr/include/netinet/in.h" 3 4
// # 1 "/usr/include/stdint.h" 1 3 4
// # 28 "/usr/include/stdint.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 29 "/usr/include/stdint.h" 2 3 4
// # 49 "/usr/include/stdint.h" 3 4
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;
// # 66 "/usr/include/stdint.h" 3 4
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;
// # 91 "/usr/include/stdint.h" 3 4
typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
// # 104 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
// # 123 "/usr/include/stdint.h" 3 4
typedef unsigned long int uintptr_t;
// # 135 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
// # 24 "/usr/include/netinet/in.h" 2 3 4
// # 1 "/usr/include/sys/socket.h" 1 3 4
// # 25 "/usr/include/sys/socket.h" 3 4


// # 1 "/usr/include/sys/uio.h" 1 3 4
// # 26 "/usr/include/sys/uio.h" 3 4



// # 1 "/usr/include/bits/uio.h" 1 3 4
// # 44 "/usr/include/bits/uio.h" 3 4
struct iovec
  {
    void *iov_base;
    size_t iov_len;
  };
// # 30 "/usr/include/sys/uio.h" 2 3 4
// # 40 "/usr/include/sys/uio.h" 3 4
extern ssize_t readv (int __fd, __const struct iovec *__iovec, int __count);
// # 50 "/usr/include/sys/uio.h" 3 4
extern ssize_t writev (int __fd, __const struct iovec *__iovec, int __count);


// # 28 "/usr/include/sys/socket.h" 2 3 4

// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 30 "/usr/include/sys/socket.h" 2 3 4





// # 1 "/usr/include/bits/socket.h" 1 3 4
// # 29 "/usr/include/bits/socket.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 30 "/usr/include/bits/socket.h" 2 3 4

// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 32 "/usr/include/bits/socket.h" 2 3 4
// # 41 "/usr/include/bits/socket.h" 3 4
enum __socket_type
{
  SOCK_STREAM = 1,


  SOCK_DGRAM = 2,


  SOCK_RAW = 3,

  SOCK_RDM = 4,

  SOCK_SEQPACKET = 5,


  SOCK_PACKET = 10



};
// # 142 "/usr/include/bits/socket.h" 3 4
// # 1 "/usr/include/bits/sockaddr.h" 1 3 4
// # 29 "/usr/include/bits/sockaddr.h" 3 4
typedef unsigned short int sa_family_t;
// # 143 "/usr/include/bits/socket.h" 2 3 4


struct sockaddr
  {
    sa_family_t sa_family;
    char sa_data[14];
  };
// # 162 "/usr/include/bits/socket.h" 3 4
struct sockaddr_storage
  {
    sa_family_t ss_family;
    __uint64_t __ss_align;
    char __ss_padding[(128 - (2 * sizeof (__uint64_t)))];
  };



enum
  {
    MSG_OOB = 0x01,

    MSG_PEEK = 0x02,

    MSG_DONTROUTE = 0x04,






    MSG_CTRUNC = 0x08,

    MSG_PROXY = 0x10,

    MSG_TRUNC = 0x20,

    MSG_DONTWAIT = 0x40,

    MSG_EOR = 0x80,

    MSG_WAITALL = 0x100,

    MSG_FIN = 0x200,

    MSG_SYN = 0x400,

    MSG_CONFIRM = 0x800,

    MSG_RST = 0x1000,

    MSG_ERRQUEUE = 0x2000,

    MSG_NOSIGNAL = 0x4000,

    MSG_MORE = 0x8000

  };




struct msghdr
  {
    void *msg_name;
    socklen_t msg_namelen;

    struct iovec *msg_iov;
    size_t msg_iovlen;

    void *msg_control;
    size_t msg_controllen;




    int msg_flags;
  };


struct cmsghdr
  {
    size_t cmsg_len;




    int cmsg_level;
    int cmsg_type;

    __extension__ unsigned char __cmsg_data [];

  };
// # 263 "/usr/include/bits/socket.h" 3 4
extern struct cmsghdr *__cmsg_nxthdr (struct msghdr *__mhdr,
          struct cmsghdr *__cmsg) __attribute__ ((__nothrow__));
// # 290 "/usr/include/bits/socket.h" 3 4
enum
  {
    SCM_RIGHTS = 0x01


    , SCM_CREDENTIALS = 0x02


  };



struct ucred
{
  pid_t pid;
  uid_t uid;
  gid_t gid;
};


// # 1 "/usr/include/asm/socket.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/socket.h" 1 3 4



// # 1 "/usr/include/asm/sockios.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/sockios.h" 1 3 4
// # 6 "/usr/include/asm/sockios.h" 2 3 4
// # 5 "/usr/include/asm-x86_64/socket.h" 2 3 4
// # 6 "/usr/include/asm/socket.h" 2 3 4
// # 311 "/usr/include/bits/socket.h" 2 3 4



struct linger
  {
    int l_onoff;
    int l_linger;
  };
// # 36 "/usr/include/sys/socket.h" 2 3 4




struct osockaddr
  {
    unsigned short int sa_family;
    unsigned char sa_data[14];
  };




enum
{
  SHUT_RD = 0,

  SHUT_WR,

  SHUT_RDWR

};
// # 100 "/usr/include/sys/socket.h" 3 4
extern int socket (int __domain, int __type, int __protocol) __attribute__ ((__nothrow__));





extern int socketpair (int __domain, int __type, int __protocol,
         int __fds[2]) __attribute__ ((__nothrow__));


extern int bind (int __fd, __const struct sockaddr * __addr, socklen_t __len)
     __attribute__ ((__nothrow__));


extern int getsockname (int __fd, struct sockaddr *__restrict __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__));
// # 124 "/usr/include/sys/socket.h" 3 4
extern int connect (int __fd, __const struct sockaddr * __addr, socklen_t __len);



extern int getpeername (int __fd, struct sockaddr *__restrict __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__));






extern ssize_t send (int __fd, __const void *__buf, size_t __n, int __flags);






extern ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);






extern ssize_t sendto (int __fd, __const void *__buf, size_t __n,
         int __flags, __const struct sockaddr * __addr,
         socklen_t __addr_len);
// # 161 "/usr/include/sys/socket.h" 3 4
extern ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n,
    int __flags, struct sockaddr *__restrict __addr,
    socklen_t *__restrict __addr_len);







extern ssize_t sendmsg (int __fd, __const struct msghdr *__message,
   int __flags);






extern ssize_t recvmsg (int __fd, struct msghdr *__message, int __flags);





extern int getsockopt (int __fd, int __level, int __optname,
         void *__restrict __optval,
         socklen_t *__restrict __optlen) __attribute__ ((__nothrow__));




extern int setsockopt (int __fd, int __level, int __optname,
         __const void *__optval, socklen_t __optlen) __attribute__ ((__nothrow__));





extern int listen (int __fd, int __n) __attribute__ ((__nothrow__));
// # 209 "/usr/include/sys/socket.h" 3 4
extern int accept (int __fd, struct sockaddr *__restrict __addr,
     socklen_t *__restrict __addr_len);







extern int shutdown (int __fd, int __how) __attribute__ ((__nothrow__));




extern int sockatmark (int __fd) __attribute__ ((__nothrow__));







extern int isfdtype (int __fd, int __fdtype) __attribute__ ((__nothrow__));
// # 240 "/usr/include/sys/socket.h" 3 4

// # 25 "/usr/include/netinet/in.h" 2 3 4






enum
  {
    IPPROTO_IP = 0,

    IPPROTO_HOPOPTS = 0,

    IPPROTO_ICMP = 1,

    IPPROTO_IGMP = 2,

    IPPROTO_IPIP = 4,

    IPPROTO_TCP = 6,

    IPPROTO_EGP = 8,

    IPPROTO_PUP = 12,

    IPPROTO_UDP = 17,

    IPPROTO_IDP = 22,

    IPPROTO_TP = 29,

    IPPROTO_IPV6 = 41,

    IPPROTO_ROUTING = 43,

    IPPROTO_FRAGMENT = 44,

    IPPROTO_RSVP = 46,

    IPPROTO_GRE = 47,

    IPPROTO_ESP = 50,

    IPPROTO_AH = 51,

    IPPROTO_ICMPV6 = 58,

    IPPROTO_NONE = 59,

    IPPROTO_DSTOPTS = 60,

    IPPROTO_MTP = 92,

    IPPROTO_ENCAP = 98,

    IPPROTO_PIM = 103,

    IPPROTO_COMP = 108,

    IPPROTO_SCTP = 132,

    IPPROTO_RAW = 255,

    IPPROTO_MAX
  };



typedef uint16_t in_port_t;


enum
  {
    IPPORT_ECHO = 7,
    IPPORT_DISCARD = 9,
    IPPORT_SYSTAT = 11,
    IPPORT_DAYTIME = 13,
    IPPORT_NETSTAT = 15,
    IPPORT_FTP = 21,
    IPPORT_TELNET = 23,
    IPPORT_SMTP = 25,
    IPPORT_TIMESERVER = 37,
    IPPORT_NAMESERVER = 42,
    IPPORT_WHOIS = 43,
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,


    IPPORT_EXECSERVER = 512,
    IPPORT_LOGINSERVER = 513,
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,


    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,


    IPPORT_RESERVED = 1024,


    IPPORT_USERRESERVED = 5000
  };



typedef uint32_t in_addr_t;
struct in_addr
  {
    in_addr_t s_addr;
  };
// # 193 "/usr/include/netinet/in.h" 3 4
struct in6_addr
  {
    union
      {
 uint8_t u6_addr8[16];
 uint16_t u6_addr16[8];
 uint32_t u6_addr32[4];
      } in6_u;



  };

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;
// # 219 "/usr/include/netinet/in.h" 3 4
struct sockaddr_in
  {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;


    unsigned char sin_zero[sizeof (struct sockaddr) -
      (sizeof (unsigned short int)) -
      sizeof (in_port_t) -
      sizeof (struct in_addr)];
  };


struct sockaddr_in6
  {
    sa_family_t sin6_family;
    in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t sin6_scope_id;
  };



struct ip_mreq
  {

    struct in_addr imr_multiaddr;


    struct in_addr imr_interface;
  };

struct ip_mreq_source
  {

    struct in_addr imr_multiaddr;


    struct in_addr imr_interface;


    struct in_addr imr_sourceaddr;
  };


struct ipv6_mreq
  {

    struct in6_addr ipv6mr_multiaddr;


    unsigned int ipv6mr_interface;
  };



struct group_req
  {

    uint32_t gr_interface;


    struct sockaddr_storage gr_group;
  };

struct group_source_req
  {

    uint32_t gsr_interface;


    struct sockaddr_storage gsr_group;


    struct sockaddr_storage gsr_source;
  };



struct ip_msfilter
  {

    struct in_addr imsf_multiaddr;


    struct in_addr imsf_interface;


    uint32_t imsf_fmode;


    uint32_t imsf_numsrc;

    struct in_addr imsf_slist[1];
  };





struct group_filter
  {

    uint32_t gf_interface;


    struct sockaddr_storage gf_group;


    uint32_t gf_fmode;


    uint32_t gf_numsrc;

    struct sockaddr_storage gf_slist[1];
};
// # 345 "/usr/include/netinet/in.h" 3 4
// # 1 "/usr/include/bits/in.h" 1 3 4
// # 82 "/usr/include/bits/in.h" 3 4
struct ip_opts
  {
    struct in_addr ip_dst;
    char ip_opts[40];
  };


struct ip_mreqn
  {
    struct in_addr imr_multiaddr;
    struct in_addr imr_address;
    int imr_ifindex;
  };


struct in_pktinfo
  {
    int ipi_ifindex;
    struct in_addr ipi_spec_dst;
    struct in_addr ipi_addr;
  };
// # 346 "/usr/include/netinet/in.h" 2 3 4
// # 354 "/usr/include/netinet/in.h" 3 4
extern uint32_t ntohl (uint32_t __netlong) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));




// # 1 "/usr/include/bits/byteswap.h" 1 3 4
// # 27 "/usr/include/bits/byteswap.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 28 "/usr/include/bits/byteswap.h" 2 3 4
// # 366 "/usr/include/netinet/in.h" 2 3 4
// # 428 "/usr/include/netinet/in.h" 3 4
extern int bindresvport (int __sockfd, struct sockaddr_in *__sock_in) __attribute__ ((__nothrow__));


extern int bindresvport6 (int __sockfd, struct sockaddr_in6 *__sock_in)
     __attribute__ ((__nothrow__));
// # 456 "/usr/include/netinet/in.h" 3 4
struct in6_pktinfo
  {
    struct in6_addr ipi6_addr;
    unsigned int ipi6_ifindex;
  };


struct ip6_mtuinfo
  {
    struct sockaddr_in6 ip6m_addr;
    uint32_t ip6m_mtu;
  };
// # 551 "/usr/include/netinet/in.h" 3 4

// # 846 "wwwsys.h" 2
// # 854 "wwwsys.h"
// # 1 "/usr/include/netinet/tcp.h" 1 3 4
// # 89 "/usr/include/netinet/tcp.h" 3 4
struct tcphdr
  {
    u_int16_t source;
    u_int16_t dest;
    u_int32_t seq;
    u_int32_t ack_seq;

    u_int16_t res1:4;
    u_int16_t doff:4;
    u_int16_t fin:1;
    u_int16_t syn:1;
    u_int16_t rst:1;
    u_int16_t psh:1;
    u_int16_t ack:1;
    u_int16_t urg:1;
    u_int16_t res2:2;
// # 118 "/usr/include/netinet/tcp.h" 3 4
    u_int16_t window;
    u_int16_t check;
    u_int16_t urg_ptr;
};


enum
{
  TCP_ESTABLISHED = 1,
  TCP_SYN_SENT,
  TCP_SYN_RECV,
  TCP_FIN_WAIT1,
  TCP_FIN_WAIT2,
  TCP_TIME_WAIT,
  TCP_CLOSE,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_LISTEN,
  TCP_CLOSING
};
// # 176 "/usr/include/netinet/tcp.h" 3 4
enum tcp_ca_state
{
  TCP_CA_Open = 0,
  TCP_CA_Disorder = 1,
  TCP_CA_CWR = 2,
  TCP_CA_Recovery = 3,
  TCP_CA_Loss = 4
};

struct tcp_info
{
  u_int8_t tcpi_state;
  u_int8_t tcpi_ca_state;
  u_int8_t tcpi_retransmits;
  u_int8_t tcpi_probes;
  u_int8_t tcpi_backoff;
  u_int8_t tcpi_options;
  u_int8_t tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;

  u_int32_t tcpi_rto;
  u_int32_t tcpi_ato;
  u_int32_t tcpi_snd_mss;
  u_int32_t tcpi_rcv_mss;

  u_int32_t tcpi_unacked;
  u_int32_t tcpi_sacked;
  u_int32_t tcpi_lost;
  u_int32_t tcpi_retrans;
  u_int32_t tcpi_fackets;


  u_int32_t tcpi_last_data_sent;
  u_int32_t tcpi_last_ack_sent;
  u_int32_t tcpi_last_data_recv;
  u_int32_t tcpi_last_ack_recv;


  u_int32_t tcpi_pmtu;
  u_int32_t tcpi_rcv_ssthresh;
  u_int32_t tcpi_rtt;
  u_int32_t tcpi_rttvar;
  u_int32_t tcpi_snd_ssthresh;
  u_int32_t tcpi_snd_cwnd;
  u_int32_t tcpi_advmss;
  u_int32_t tcpi_reordering;
};
// # 855 "wwwsys.h" 2
// # 863 "wwwsys.h"
// # 1 "/usr/include/sys/file.h" 1 3 4
// # 28 "/usr/include/sys/file.h" 3 4

// # 51 "/usr/include/sys/file.h" 3 4
extern int flock (int __fd, int __operation) __attribute__ ((__nothrow__));



// # 864 "wwwsys.h" 2
// # 873 "wwwsys.h"
// # 1 "/usr/include/sys/ioctl.h" 1 3 4
// # 24 "/usr/include/sys/ioctl.h" 3 4



// # 1 "/usr/include/bits/ioctls.h" 1 3 4
// # 24 "/usr/include/bits/ioctls.h" 3 4
// # 1 "/usr/include/asm/ioctls.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/ioctls.h" 1 3 4



// # 1 "/usr/include/asm/ioctl.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/ioctl.h" 1 3 4
// # 1 "/usr/include/asm-generic/ioctl.h" 1 3 4
// # 1 "/usr/include/asm-x86_64/ioctl.h" 2 3 4
// # 6 "/usr/include/asm/ioctl.h" 2 3 4
// # 5 "/usr/include/asm-x86_64/ioctls.h" 2 3 4
// # 6 "/usr/include/asm/ioctls.h" 2 3 4
// # 25 "/usr/include/bits/ioctls.h" 2 3 4
// # 28 "/usr/include/sys/ioctl.h" 2 3 4


// # 1 "/usr/include/bits/ioctl-types.h" 1 3 4
// # 28 "/usr/include/bits/ioctl-types.h" 3 4
struct winsize
  {
    unsigned short int ws_row;
    unsigned short int ws_col;
    unsigned short int ws_xpixel;
    unsigned short int ws_ypixel;
  };


struct termio
  {
    unsigned short int c_iflag;
    unsigned short int c_oflag;
    unsigned short int c_cflag;
    unsigned short int c_lflag;
    unsigned char c_line;
    unsigned char c_cc[8];
};
// # 31 "/usr/include/sys/ioctl.h" 2 3 4






// # 1 "/usr/include/sys/ttydefaults.h" 1 3 4
// # 38 "/usr/include/sys/ioctl.h" 2 3 4




extern int ioctl (int __fd, unsigned long int __request, ...) __attribute__ ((__nothrow__));


// # 874 "wwwsys.h" 2




// # 1 "/usr/include/termios.h" 1 3 4
// # 36 "/usr/include/termios.h" 3 4




// # 1 "/usr/include/bits/termios.h" 1 3 4
// # 25 "/usr/include/bits/termios.h" 3 4
typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;


struct termios
  {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_line;
    cc_t c_cc[32];
    speed_t c_ispeed;
    speed_t c_ospeed;


  };
// # 41 "/usr/include/termios.h" 2 3 4
// # 49 "/usr/include/termios.h" 3 4
extern speed_t cfgetospeed (__const struct termios *__termios_p) __attribute__ ((__nothrow__));


extern speed_t cfgetispeed (__const struct termios *__termios_p) __attribute__ ((__nothrow__));


extern int cfsetospeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));


extern int cfsetispeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));



extern int cfsetspeed (struct termios *__termios_p, speed_t __speed) __attribute__ ((__nothrow__));




extern int tcgetattr (int __fd, struct termios *__termios_p) __attribute__ ((__nothrow__));



extern int tcsetattr (int __fd, int __optional_actions,
        __const struct termios *__termios_p) __attribute__ ((__nothrow__));




extern void cfmakeraw (struct termios *__termios_p) __attribute__ ((__nothrow__));



extern int tcsendbreak (int __fd, int __duration) __attribute__ ((__nothrow__));





extern int tcdrain (int __fd);



extern int tcflush (int __fd, int __queue_selector) __attribute__ ((__nothrow__));



extern int tcflow (int __fd, int __action) __attribute__ ((__nothrow__));
// # 105 "/usr/include/termios.h" 3 4
// # 1 "/usr/include/sys/ttydefaults.h" 1 3 4
// # 106 "/usr/include/termios.h" 2 3 4



// # 879 "wwwsys.h" 2




// # 1 "/usr/include/sys/time.h" 1 3 4
// # 27 "/usr/include/sys/time.h" 3 4
// # 1 "/usr/include/time.h" 1 3 4
// # 28 "/usr/include/sys/time.h" 2 3 4

// # 1 "/usr/include/bits/time.h" 1 3 4
// # 30 "/usr/include/sys/time.h" 2 3 4
// # 39 "/usr/include/sys/time.h" 3 4

// # 57 "/usr/include/sys/time.h" 3 4
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

typedef struct timezone *__restrict __timezone_ptr_t;
// # 73 "/usr/include/sys/time.h" 3 4
extern int gettimeofday (struct timeval *__restrict __tv,
    __timezone_ptr_t __tz) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int settimeofday (__const struct timeval *__tv,
    __const struct timezone *__tz)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int adjtime (__const struct timeval *__delta,
      struct timeval *__olddelta) __attribute__ ((__nothrow__));




enum __itimer_which
  {

    ITIMER_REAL = 0,


    ITIMER_VIRTUAL = 1,



    ITIMER_PROF = 2

  };



struct itimerval
  {

    struct timeval it_interval;

    struct timeval it_value;
  };






typedef int __itimer_which_t;




extern int getitimer (__itimer_which_t __which,
        struct itimerval *__value) __attribute__ ((__nothrow__));




extern int setitimer (__itimer_which_t __which,
        __const struct itimerval *__restrict __new,
        struct itimerval *__restrict __old) __attribute__ ((__nothrow__));




extern int utimes (__const char *__file, __const struct timeval __tvp[2])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int lutimes (__const char *__file, __const struct timeval __tvp[2])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int futimes (int __fd, __const struct timeval __tvp[2]) __attribute__ ((__nothrow__));
// # 191 "/usr/include/sys/time.h" 3 4

// # 884 "wwwsys.h" 2
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

// # 885 "wwwsys.h" 2
// # 897 "wwwsys.h"
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

// # 898 "wwwsys.h" 2
// # 906 "wwwsys.h"
// # 1 "/usr/include/syslog.h" 1 3 4
// # 1 "/usr/include/sys/syslog.h" 1 3 4
// # 37 "/usr/include/sys/syslog.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 38 "/usr/include/sys/syslog.h" 2 3 4


// # 1 "/usr/include/bits/syslog-path.h" 1 3 4
// # 41 "/usr/include/sys/syslog.h" 2 3 4
// # 169 "/usr/include/sys/syslog.h" 3 4






extern void closelog (void);





extern void openlog (__const char *__ident, int __option, int __facility);


extern int setlogmask (int __mask) __attribute__ ((__nothrow__));





extern void syslog (int __pri, __const char *__fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
// # 200 "/usr/include/sys/syslog.h" 3 4
extern void vsyslog (int __pri, __const char *__fmt, __gnuc_va_list __ap)
     __attribute__ ((__format__ (__printf__, 2, 0)));
// # 213 "/usr/include/sys/syslog.h" 3 4

// # 1 "/usr/include/syslog.h" 2 3 4
// # 907 "wwwsys.h" 2
// # 943 "wwwsys.h"
// # 1 "/usr/include/sys/ipc.h" 1 3 4
// # 29 "/usr/include/sys/ipc.h" 3 4
// # 1 "/usr/include/bits/ipctypes.h" 1 3 4
// # 33 "/usr/include/bits/ipctypes.h" 3 4
typedef int __ipc_pid_t;
// # 30 "/usr/include/sys/ipc.h" 2 3 4
// # 1 "/usr/include/bits/ipc.h" 1 3 4
// # 43 "/usr/include/bits/ipc.h" 3 4
struct ipc_perm
  {
    __key_t __key;
    __uid_t uid;
    __gid_t gid;
    __uid_t cuid;
    __gid_t cgid;
    unsigned short int mode;
    unsigned short int __pad1;
    unsigned short int __seq;
    unsigned short int __pad2;
    unsigned long int __unused1;
    unsigned long int __unused2;
  };
// # 31 "/usr/include/sys/ipc.h" 2 3 4
// # 52 "/usr/include/sys/ipc.h" 3 4



extern key_t ftok (__const char *__pathname, int __proj_id) __attribute__ ((__nothrow__));


// # 944 "wwwsys.h" 2




// # 1 "/usr/include/errno.h" 1 3 4
// # 32 "/usr/include/errno.h" 3 4




// # 1 "/usr/include/bits/errno.h" 1 3 4
// # 25 "/usr/include/bits/errno.h" 3 4
// # 1 "/usr/include/linux/errno.h" 1 3 4



// # 1 "/usr/include/asm/errno.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/errno.h" 1 3 4



// # 1 "/usr/include/asm-generic/errno.h" 1 3 4



// # 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
// # 5 "/usr/include/asm-generic/errno.h" 2 3 4
// # 5 "/usr/include/asm-x86_64/errno.h" 2 3 4
// # 6 "/usr/include/asm/errno.h" 2 3 4
// # 5 "/usr/include/linux/errno.h" 2 3 4
// # 26 "/usr/include/bits/errno.h" 2 3 4
// # 43 "/usr/include/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
// # 37 "/usr/include/errno.h" 2 3 4
// # 59 "/usr/include/errno.h" 3 4

// # 949 "wwwsys.h" 2
// # 961 "wwwsys.h"
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

// # 962 "wwwsys.h" 2




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




// # 967 "wwwsys.h" 2




// # 1 "/usr/include/arpa/inet.h" 1 3 4
// # 31 "/usr/include/arpa/inet.h" 3 4




extern in_addr_t inet_addr (__const char *__cp) __attribute__ ((__nothrow__));


extern in_addr_t inet_lnaof (struct in_addr __in) __attribute__ ((__nothrow__));



extern struct in_addr inet_makeaddr (in_addr_t __net, in_addr_t __host)
     __attribute__ ((__nothrow__));


extern in_addr_t inet_netof (struct in_addr __in) __attribute__ ((__nothrow__));



extern in_addr_t inet_network (__const char *__cp) __attribute__ ((__nothrow__));



extern char *inet_ntoa (struct in_addr __in) __attribute__ ((__nothrow__));




extern int inet_pton (int __af, __const char *__restrict __cp,
        void *__restrict __buf) __attribute__ ((__nothrow__));




extern __const char *inet_ntop (int __af, __const void *__restrict __cp,
    char *__restrict __buf, socklen_t __len)
     __attribute__ ((__nothrow__));






extern int inet_aton (__const char *__cp, struct in_addr *__inp) __attribute__ ((__nothrow__));



extern char *inet_neta (in_addr_t __net, char *__buf, size_t __len) __attribute__ ((__nothrow__));




extern char *inet_net_ntop (int __af, __const void *__cp, int __bits,
       char *__buf, size_t __len) __attribute__ ((__nothrow__));




extern int inet_net_pton (int __af, __const char *__cp,
     void *__buf, size_t __len) __attribute__ ((__nothrow__));




extern unsigned int inet_nsap_addr (__const char *__cp,
        unsigned char *__buf, int __len) __attribute__ ((__nothrow__));



extern char *inet_nsap_ntoa (int __len, __const unsigned char *__cp,
        char *__buf) __attribute__ ((__nothrow__));



// # 972 "wwwsys.h" 2
// # 980 "wwwsys.h"
// # 1 "/usr/include/netdb.h" 1 3 4
// # 33 "/usr/include/netdb.h" 3 4
// # 1 "/usr/include/rpc/netdb.h" 1 3 4
// # 42 "/usr/include/rpc/netdb.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 43 "/usr/include/rpc/netdb.h" 2 3 4



struct rpcent
{
  char *r_name;
  char **r_aliases;
  int r_number;
};

extern void setrpcent (int __stayopen) __attribute__ ((__nothrow__));
extern void endrpcent (void) __attribute__ ((__nothrow__));
extern struct rpcent *getrpcbyname (__const char *__name) __attribute__ ((__nothrow__));
extern struct rpcent *getrpcbynumber (int __number) __attribute__ ((__nothrow__));
extern struct rpcent *getrpcent (void) __attribute__ ((__nothrow__));


extern int getrpcbyname_r (__const char *__name, struct rpcent *__result_buf,
      char *__buffer, size_t __buflen,
      struct rpcent **__result) __attribute__ ((__nothrow__));

extern int getrpcbynumber_r (int __number, struct rpcent *__result_buf,
        char *__buffer, size_t __buflen,
        struct rpcent **__result) __attribute__ ((__nothrow__));

extern int getrpcent_r (struct rpcent *__result_buf, char *__buffer,
   size_t __buflen, struct rpcent **__result) __attribute__ ((__nothrow__));



// # 34 "/usr/include/netdb.h" 2 3 4
// # 43 "/usr/include/netdb.h" 3 4
// # 1 "/usr/include/bits/netdb.h" 1 3 4
// # 27 "/usr/include/bits/netdb.h" 3 4
struct netent
{
  char *n_name;
  char **n_aliases;
  int n_addrtype;
  uint32_t n_net;
};
// # 44 "/usr/include/netdb.h" 2 3 4
// # 54 "/usr/include/netdb.h" 3 4







extern int *__h_errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
// # 88 "/usr/include/netdb.h" 3 4
extern void herror (__const char *__str) __attribute__ ((__nothrow__));


extern __const char *hstrerror (int __err_num) __attribute__ ((__nothrow__));




struct hostent
{
  char *h_name;
  char **h_aliases;
  int h_addrtype;
  int h_length;
  char **h_addr_list;

};






extern void sethostent (int __stay_open);





extern void endhostent (void);






extern struct hostent *gethostent (void);






extern struct hostent *gethostbyaddr (__const void *__addr, __socklen_t __len,
          int __type);





extern struct hostent *gethostbyname (__const char *__name);
// # 149 "/usr/include/netdb.h" 3 4
extern struct hostent *gethostbyname2 (__const char *__name, int __af);
// # 161 "/usr/include/netdb.h" 3 4
extern int gethostent_r (struct hostent *__restrict __result_buf,
    char *__restrict __buf, size_t __buflen,
    struct hostent **__restrict __result,
    int *__restrict __h_errnop);

extern int gethostbyaddr_r (__const void *__restrict __addr, __socklen_t __len,
       int __type,
       struct hostent *__restrict __result_buf,
       char *__restrict __buf, size_t __buflen,
       struct hostent **__restrict __result,
       int *__restrict __h_errnop);

extern int gethostbyname_r (__const char *__restrict __name,
       struct hostent *__restrict __result_buf,
       char *__restrict __buf, size_t __buflen,
       struct hostent **__restrict __result,
       int *__restrict __h_errnop);

extern int gethostbyname2_r (__const char *__restrict __name, int __af,
        struct hostent *__restrict __result_buf,
        char *__restrict __buf, size_t __buflen,
        struct hostent **__restrict __result,
        int *__restrict __h_errnop);
// # 192 "/usr/include/netdb.h" 3 4
extern void setnetent (int __stay_open);





extern void endnetent (void);






extern struct netent *getnetent (void);






extern struct netent *getnetbyaddr (uint32_t __net, int __type);





extern struct netent *getnetbyname (__const char *__name);
// # 231 "/usr/include/netdb.h" 3 4
extern int getnetent_r (struct netent *__restrict __result_buf,
   char *__restrict __buf, size_t __buflen,
   struct netent **__restrict __result,
   int *__restrict __h_errnop);

extern int getnetbyaddr_r (uint32_t __net, int __type,
      struct netent *__restrict __result_buf,
      char *__restrict __buf, size_t __buflen,
      struct netent **__restrict __result,
      int *__restrict __h_errnop);

extern int getnetbyname_r (__const char *__restrict __name,
      struct netent *__restrict __result_buf,
      char *__restrict __buf, size_t __buflen,
      struct netent **__restrict __result,
      int *__restrict __h_errnop);




struct servent
{
  char *s_name;
  char **s_aliases;
  int s_port;
  char *s_proto;
};






extern void setservent (int __stay_open);





extern void endservent (void);






extern struct servent *getservent (void);






extern struct servent *getservbyname (__const char *__name,
          __const char *__proto);






extern struct servent *getservbyport (int __port, __const char *__proto);
// # 303 "/usr/include/netdb.h" 3 4
extern int getservent_r (struct servent *__restrict __result_buf,
    char *__restrict __buf, size_t __buflen,
    struct servent **__restrict __result);

extern int getservbyname_r (__const char *__restrict __name,
       __const char *__restrict __proto,
       struct servent *__restrict __result_buf,
       char *__restrict __buf, size_t __buflen,
       struct servent **__restrict __result);

extern int getservbyport_r (int __port, __const char *__restrict __proto,
       struct servent *__restrict __result_buf,
       char *__restrict __buf, size_t __buflen,
       struct servent **__restrict __result);




struct protoent
{
  char *p_name;
  char **p_aliases;
  int p_proto;
};






extern void setprotoent (int __stay_open);





extern void endprotoent (void);






extern struct protoent *getprotoent (void);





extern struct protoent *getprotobyname (__const char *__name);





extern struct protoent *getprotobynumber (int __proto);
// # 369 "/usr/include/netdb.h" 3 4
extern int getprotoent_r (struct protoent *__restrict __result_buf,
     char *__restrict __buf, size_t __buflen,
     struct protoent **__restrict __result);

extern int getprotobyname_r (__const char *__restrict __name,
        struct protoent *__restrict __result_buf,
        char *__restrict __buf, size_t __buflen,
        struct protoent **__restrict __result);

extern int getprotobynumber_r (int __proto,
          struct protoent *__restrict __result_buf,
          char *__restrict __buf, size_t __buflen,
          struct protoent **__restrict __result);
// # 390 "/usr/include/netdb.h" 3 4
extern int setnetgrent (__const char *__netgroup);







extern void endnetgrent (void);
// # 407 "/usr/include/netdb.h" 3 4
extern int getnetgrent (char **__restrict __hostp,
   char **__restrict __userp,
   char **__restrict __domainp);
// # 418 "/usr/include/netdb.h" 3 4
extern int innetgr (__const char *__netgroup, __const char *__host,
      __const char *__user, __const char *domain);







extern int getnetgrent_r (char **__restrict __hostp,
     char **__restrict __userp,
     char **__restrict __domainp,
     char *__restrict __buffer, size_t __buflen);
// # 446 "/usr/include/netdb.h" 3 4
extern int rcmd (char **__restrict __ahost, unsigned short int __rport,
   __const char *__restrict __locuser,
   __const char *__restrict __remuser,
   __const char *__restrict __cmd, int *__restrict __fd2p);
// # 458 "/usr/include/netdb.h" 3 4
extern int rcmd_af (char **__restrict __ahost, unsigned short int __rport,
      __const char *__restrict __locuser,
      __const char *__restrict __remuser,
      __const char *__restrict __cmd, int *__restrict __fd2p,
      sa_family_t __af);
// # 474 "/usr/include/netdb.h" 3 4
extern int rexec (char **__restrict __ahost, int __rport,
    __const char *__restrict __name,
    __const char *__restrict __pass,
    __const char *__restrict __cmd, int *__restrict __fd2p);
// # 486 "/usr/include/netdb.h" 3 4
extern int rexec_af (char **__restrict __ahost, int __rport,
       __const char *__restrict __name,
       __const char *__restrict __pass,
       __const char *__restrict __cmd, int *__restrict __fd2p,
       sa_family_t __af);
// # 500 "/usr/include/netdb.h" 3 4
extern int ruserok (__const char *__rhost, int __suser,
      __const char *__remuser, __const char *__locuser);
// # 510 "/usr/include/netdb.h" 3 4
extern int ruserok_af (__const char *__rhost, int __suser,
         __const char *__remuser, __const char *__locuser,
         sa_family_t __af);
// # 522 "/usr/include/netdb.h" 3 4
extern int rresvport (int *__alport);
// # 531 "/usr/include/netdb.h" 3 4
extern int rresvport_af (int *__alport, sa_family_t __af);






struct addrinfo
{
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  socklen_t ai_addrlen;
  struct sockaddr *ai_addr;
  char *ai_canonname;
  struct addrinfo *ai_next;
};
// # 631 "/usr/include/netdb.h" 3 4
extern int getaddrinfo (__const char *__restrict __name,
   __const char *__restrict __service,
   __const struct addrinfo *__restrict __req,
   struct addrinfo **__restrict __pai);


extern void freeaddrinfo (struct addrinfo *__ai) __attribute__ ((__nothrow__));


extern __const char *gai_strerror (int __ecode) __attribute__ ((__nothrow__));





extern int getnameinfo (__const struct sockaddr *__restrict __sa,
   socklen_t __salen, char *__restrict __host,
   socklen_t __hostlen, char *__restrict __serv,
   socklen_t __servlen, unsigned int __flags);
// # 682 "/usr/include/netdb.h" 3 4

// # 981 "wwwsys.h" 2
// # 1029 "wwwsys.h"
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
// # 445 "/usr/include/stdlib.h" 3 4
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

// # 1030 "wwwsys.h" 2




// # 1 "/usr/include/malloc.h" 1 3 4
// # 24 "/usr/include/malloc.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 152 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long int ptrdiff_t;
// # 25 "/usr/include/malloc.h" 2 3 4
// # 46 "/usr/include/malloc.h" 3 4



extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;


extern void *calloc (size_t __nmemb, size_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__)) ;



extern void *realloc (void *__ptr, size_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));


extern void free (void *__ptr) __attribute__ ((__nothrow__));


extern void cfree (void *__ptr) __attribute__ ((__nothrow__));


extern void *memalign (size_t __alignment, size_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__)) ;


extern void *valloc (size_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__)) ;



extern void * pvalloc (size_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__)) ;



extern void *(*__morecore) (ptrdiff_t __size);


extern void *__default_morecore (ptrdiff_t __size) __attribute__ ((__nothrow__))
       __attribute__ ((__malloc__));



struct mallinfo {
  int arena;
  int ordblks;
  int smblks;
  int hblks;
  int hblkhd;
  int usmblks;
  int fsmblks;
  int uordblks;
  int fordblks;
  int keepcost;
};


extern struct mallinfo mallinfo (void) __attribute__ ((__nothrow__));
// # 130 "/usr/include/malloc.h" 3 4
extern int mallopt (int __param, int __val) __attribute__ ((__nothrow__));



extern int malloc_trim (size_t __pad) __attribute__ ((__nothrow__));



extern size_t malloc_usable_size (void *__ptr) __attribute__ ((__nothrow__));


extern void malloc_stats (void) __attribute__ ((__nothrow__));


extern void *malloc_get_state (void) __attribute__ ((__nothrow__));



extern int malloc_set_state (void *__ptr) __attribute__ ((__nothrow__));




extern void (*__malloc_initialize_hook) (void);

extern void (*__free_hook) (void *__ptr, __const void *);

extern void *(*__malloc_hook) (size_t __size, __const void *);

extern void *(*__realloc_hook) (void *__ptr, size_t __size, __const void *);

extern void *(*__memalign_hook) (size_t __alignment, size_t __size, __const void *);


extern void (*__after_morecore_hook) (void);


extern void __malloc_check_init (void) __attribute__ ((__nothrow__));



// # 1035 "wwwsys.h" 2




// # 1 "/usr/include/memory.h" 1 3 4
// # 1040 "wwwsys.h" 2
// # 1054 "wwwsys.h"
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

// # 1055 "wwwsys.h" 2
// # 1070 "wwwsys.h"
// # 1 "/usr/include/sys/resource.h" 1 3 4
// # 25 "/usr/include/sys/resource.h" 3 4
// # 1 "/usr/include/bits/resource.h" 1 3 4
// # 33 "/usr/include/bits/resource.h" 3 4
enum __rlimit_resource
{

  RLIMIT_CPU = 0,



  RLIMIT_FSIZE = 1,



  RLIMIT_DATA = 2,



  RLIMIT_STACK = 3,



  RLIMIT_CORE = 4,






  __RLIMIT_RSS = 5,



  RLIMIT_NOFILE = 7,
  __RLIMIT_OFILE = RLIMIT_NOFILE,




  RLIMIT_AS = 9,



  __RLIMIT_NPROC = 6,



  __RLIMIT_MEMLOCK = 8,



  __RLIMIT_LOCKS = 10,



  __RLIMIT_SIGPENDING = 11,



  __RLIMIT_MSGQUEUE = 12,





  __RLIMIT_NICE = 13,




  __RLIMIT_RTPRIO = 14,


  __RLIMIT_NLIMITS = 15,
  __RLIM_NLIMITS = __RLIMIT_NLIMITS


};
// # 127 "/usr/include/bits/resource.h" 3 4
typedef __rlim_t rlim_t;







struct rlimit
  {

    rlim_t rlim_cur;

    rlim_t rlim_max;
  };
// # 154 "/usr/include/bits/resource.h" 3 4
enum __rusage_who
{

  RUSAGE_SELF = 0,



  RUSAGE_CHILDREN = -1,
// # 171 "/usr/include/bits/resource.h" 3 4
};


// # 1 "/usr/include/bits/time.h" 1 3 4
// # 175 "/usr/include/bits/resource.h" 2 3 4


struct rusage
  {

    struct timeval ru_utime;

    struct timeval ru_stime;

    long int ru_maxrss;


    long int ru_ixrss;

    long int ru_idrss;

    long int ru_isrss;


    long int ru_minflt;

    long int ru_majflt;

    long int ru_nswap;


    long int ru_inblock;

    long int ru_oublock;

    long int ru_msgsnd;

    long int ru_msgrcv;

    long int ru_nsignals;



    long int ru_nvcsw;


    long int ru_nivcsw;
  };







enum __priority_which
{
  PRIO_PROCESS = 0,

  PRIO_PGRP = 1,

  PRIO_USER = 2

};
// # 26 "/usr/include/sys/resource.h" 2 3 4







// # 43 "/usr/include/sys/resource.h" 3 4
typedef int __rlimit_resource_t;
typedef int __rusage_who_t;
typedef int __priority_which_t;





extern int getrlimit (__rlimit_resource_t __resource,
        struct rlimit *__rlimits) __attribute__ ((__nothrow__));
// # 70 "/usr/include/sys/resource.h" 3 4
extern int setrlimit (__rlimit_resource_t __resource,
        __const struct rlimit *__rlimits) __attribute__ ((__nothrow__));
// # 88 "/usr/include/sys/resource.h" 3 4
extern int getrusage (__rusage_who_t __who, struct rusage *__usage) __attribute__ ((__nothrow__));





extern int getpriority (__priority_which_t __which, id_t __who) __attribute__ ((__nothrow__));



extern int setpriority (__priority_which_t __which, id_t __who, int __prio)
     __attribute__ ((__nothrow__));


// # 1071 "wwwsys.h" 2
// # 1079 "wwwsys.h"
// # 1 "/usr/include/dirent.h" 1 3 4
// # 28 "/usr/include/dirent.h" 3 4

// # 62 "/usr/include/dirent.h" 3 4
// # 1 "/usr/include/bits/dirent.h" 1 3 4
// # 23 "/usr/include/bits/dirent.h" 3 4
struct dirent
  {

    __ino_t d_ino;
    __off_t d_off;




    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[256];
  };
// # 63 "/usr/include/dirent.h" 2 3 4
// # 98 "/usr/include/dirent.h" 3 4
enum
  {
    DT_UNKNOWN = 0,

    DT_FIFO = 1,

    DT_CHR = 2,

    DT_DIR = 4,

    DT_BLK = 6,

    DT_REG = 8,

    DT_LNK = 10,

    DT_SOCK = 12,

    DT_WHT = 14

  };
// # 128 "/usr/include/dirent.h" 3 4
typedef struct __dirstream DIR;






extern DIR *opendir (__const char *__name) __attribute__ ((__nonnull__ (1)));
// # 150 "/usr/include/dirent.h" 3 4
extern int closedir (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
// # 163 "/usr/include/dirent.h" 3 4
extern struct dirent *readdir (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
// # 184 "/usr/include/dirent.h" 3 4
extern int readdir_r (DIR *__restrict __dirp,
        struct dirent *__restrict __entry,
        struct dirent **__restrict __result)
     __attribute__ ((__nonnull__ (1, 2, 3)));
// # 209 "/usr/include/dirent.h" 3 4
extern void rewinddir (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern void seekdir (DIR *__dirp, long int __pos) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int telldir (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int dirfd (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 243 "/usr/include/dirent.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 244 "/usr/include/dirent.h" 2 3 4






extern int scandir (__const char *__restrict __dir,
      struct dirent ***__restrict __namelist,
      int (*__selector) (__const struct dirent *),
      int (*__cmp) (__const void *, __const void *))
     __attribute__ ((__nonnull__ (1, 2)));
// # 280 "/usr/include/dirent.h" 3 4
extern int alphasort (__const void *__e1, __const void *__e2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
// # 324 "/usr/include/dirent.h" 3 4
extern __ssize_t getdirentries (int __fd, char *__restrict __buf,
    size_t __nbytes,
    __off_t *__restrict __basep)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));
// # 349 "/usr/include/dirent.h" 3 4

// # 1080 "wwwsys.h" 2
// # 1109 "wwwsys.h"
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 105 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
// # 1110 "wwwsys.h" 2
// # 1118 "wwwsys.h"
// # 1 "/usr/include/sys/wait.h" 1 3 4
// # 29 "/usr/include/sys/wait.h" 3 4


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




// # 32 "/usr/include/sys/wait.h" 2 3 4






// # 1 "/usr/include/bits/waitflags.h" 1 3 4
// # 39 "/usr/include/sys/wait.h" 2 3 4
// # 63 "/usr/include/sys/wait.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
// # 80 "/usr/include/sys/wait.h" 3 4
// # 1 "/usr/include/bits/waitstatus.h" 1 3 4
// # 67 "/usr/include/bits/waitstatus.h" 3 4
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
// # 81 "/usr/include/sys/wait.h" 2 3 4
// # 102 "/usr/include/sys/wait.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
// # 116 "/usr/include/sys/wait.h" 3 4
extern __pid_t wait (__WAIT_STATUS __stat_loc);
// # 139 "/usr/include/sys/wait.h" 3 4
extern __pid_t waitpid (__pid_t __pid, int *__stat_loc, int __options);



// # 1 "/usr/include/bits/siginfo.h" 1 3 4
// # 25 "/usr/include/bits/siginfo.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 26 "/usr/include/bits/siginfo.h" 2 3 4
// # 144 "/usr/include/sys/wait.h" 2 3 4
// # 155 "/usr/include/sys/wait.h" 3 4
extern int waitid (idtype_t __idtype, __id_t __id, siginfo_t *__infop,
     int __options);





struct rusage;






extern __pid_t wait3 (__WAIT_STATUS __stat_loc, int __options,
        struct rusage * __usage) __attribute__ ((__nothrow__));





struct rusage;


extern __pid_t wait4 (__pid_t __pid, __WAIT_STATUS __stat_loc, int __options,
        struct rusage *__usage) __attribute__ ((__nothrow__));




// # 1119 "wwwsys.h" 2
// # 1283 "wwwsys.h"
typedef struct sockaddr_in SockA;
// # 1319 "wwwsys.h"
extern int (*__errno_location ());
// # 1570 "wwwsys.h"
typedef unsigned long ms_t;
// # 31 "HTFile.c" 2
// # 1 "WWWUtil.h" 1
// # 62 "WWWUtil.h"
// # 1 "HTUtils.h" 1
// # 43 "HTUtils.h"
typedef int HTPrintCallback(const char * fmt, va_list pArgs);
extern void HTPrint_setCallback(HTPrintCallback * pCall);
extern HTPrintCallback * HTPrint_getCallback(void);

extern int HTPrint(const char * fmt, ...);
// # 96 "HTUtils.h"
extern unsigned int WWW_TraceFlag;
// # 116 "HTUtils.h"
typedef enum _HTTraceFlags {
    SHOW_UTIL_TRACE = 0x1,
    SHOW_APP_TRACE = 0x2,
    SHOW_CACHE_TRACE = 0x4,
    SHOW_SGML_TRACE = 0x8,
    SHOW_BIND_TRACE = 0x10,
    SHOW_THREAD_TRACE = 0x20,
    SHOW_STREAM_TRACE = 0x40,
    SHOW_PROTOCOL_TRACE = 0x80,
    SHOW_MEM_TRACE = 0x100,
    SHOW_URI_TRACE = 0x200,
    SHOW_AUTH_TRACE = 0x400,
    SHOW_ANCHOR_TRACE = 0x800,
    SHOW_PICS_TRACE = 0x1000,
    SHOW_CORE_TRACE = 0x2000,
    SHOW_MUX_TRACE = 0x4000,
    SHOW_SQL_TRACE = 0x8000,
    SHOW_XML_TRACE = 0x10000,
    SHOW_ALL_TRACE = (int) 0xFFFFFFFF
} HTTraceFlags;
// # 176 "HTUtils.h"
typedef int HTTraceCallback(const char * fmt, va_list pArgs);
extern void HTTrace_setCallback(HTTraceCallback * pCall);
extern HTTraceCallback * HTTrace_getCallback(void);
// # 195 "HTUtils.h"
extern int HTTrace(const char * fmt, ...);
// # 210 "HTUtils.h"
typedef int HTTraceDataCallback(char * data, size_t len, char * fmt, va_list pArgs);
extern void HTTraceData_setCallback(HTTraceDataCallback * pCall);
extern HTTraceDataCallback * HTTraceData_getCallback(void);
// # 221 "HTUtils.h"
extern int HTTraceData(char * data, size_t len, char * fmt, ...);
// # 235 "HTUtils.h"
extern void HTDebugBreak(char * file, unsigned long line, const char * fmt, ...);
// # 405 "HTUtils.h"
// # 1 "HTMemory.h" 1
// # 48 "HTMemory.h"
// # 1 "HTUtils.h" 1
// # 49 "HTMemory.h" 2
// # 60 "HTMemory.h"
extern void* HTMemory_malloc(size_t size);
extern void* HTMemory_calloc(size_t count, size_t size);
extern void* HTMemory_realloc(void * ptr, size_t size);
extern void HTMemory_free(void* ptr);
// # 104 "HTMemory.h"
typedef void HTMemoryCallback(size_t size);

extern char HTMemoryCall_add (HTMemoryCallback * cbf);
// # 117 "HTMemory.h"
extern char HTMemoryCall_delete (HTMemoryCallback * cbf);
extern char HTMemoryCall_deleteAll (void);
// # 137 "HTMemory.h"
typedef void HTMemory_exitCallback(char *name, char *file, unsigned long line);

extern void HTMemory_setExit(HTMemory_exitCallback * pExit);
extern HTMemory_exitCallback * HTMemory_exit(void);
// # 156 "HTMemory.h"
extern void HTMemory_outofmem(char * name, char * file, unsigned long line);
// # 406 "HTUtils.h" 2
// # 63 "WWWUtil.h" 2
// # 75 "WWWUtil.h"
// # 1 "HTArray.h" 1
// # 43 "HTArray.h"
typedef struct {
    int size;
    int growby;
    int allocated;
    void ** data;
} HTArray;
// # 60 "HTArray.h"
extern HTArray * HTArray_new (int grow);
// # 70 "HTArray.h"
extern char HTArray_delete (HTArray * array);
// # 80 "HTArray.h"
extern char HTArray_clear (HTArray * array);
// # 90 "HTArray.h"
extern char HTArray_addObject (HTArray * array, void * object);
// # 116 "HTArray.h"
typedef int HTComparer (const void * a, const void * b);







extern char HTArray_sort (HTArray * array, HTComparer * comp);
// # 76 "WWWUtil.h" 2
// # 87 "WWWUtil.h"
// # 1 "HTAssoc.h" 1
// # 31 "HTAssoc.h"
// # 1 "HTList.h" 1
// # 33 "HTList.h"
typedef struct _HTList HTList;

struct _HTList {
  void * object;
  HTList * next;
};
// # 48 "HTList.h"
extern HTList * HTList_new (void);
extern char HTList_delete (HTList *me);
// # 59 "HTList.h"
extern char HTList_addObject (HTList *me, void *newObject);







extern char HTList_appendObject (HTList * me, void * newObject);
// # 77 "HTList.h"
extern HTList * HTList_addList (HTList * me, void * newObject);
extern HTList * HTList_appendList (HTList * me, void * newObject);
// # 92 "HTList.h"
extern char HTList_removeObject (HTList * me, void * oldObject);
extern char HTList_quickRemoveElement (HTList * me, HTList * last);
extern char HTList_removeObjectAll (HTList * me, void * oldObject);

extern void * HTList_removeLastObject (HTList * me);
extern void * HTList_removeFirstObject (HTList * me);
// # 108 "HTList.h"
extern int HTList_count (HTList *me);
// # 120 "HTList.h"
extern int HTList_indexOf (HTList * me, void * object);
extern int HTList_indexOfElement (HTList * me, HTList * element);
extern void * HTList_objectAt (HTList * me, int position);
extern HTList * HTList_elementOf (HTList * me, void * object, HTList ** pLast);
// # 143 "HTList.h"
extern void * HTList_firstObject (HTList * me);
// # 170 "HTList.h"
extern char HTList_insertionSort(HTList * list, HTComparer * comp);
// # 32 "HTAssoc.h" 2

typedef HTList HTAssocList;

typedef struct {
    char * name;
    char * value;
} HTAssoc;
// # 48 "HTAssoc.h"
extern HTAssocList * HTAssocList_new (void);
extern char HTAssocList_delete (HTAssocList * alist);
// # 63 "HTAssoc.h"
extern char HTAssocList_addObject (
 HTAssocList * alist,
 const char * name,
 const char * value);

extern char HTAssocList_replaceObject (
 HTAssocList * list,
 const char * name,
 const char * value);
// # 82 "HTAssoc.h"
extern char HTAssocList_removeObject (
 HTAssocList * list,
 const char * name);
// # 103 "HTAssoc.h"
extern char * HTAssocList_findObject (
 HTAssocList * alist,
 const char * name);
// # 115 "HTAssoc.h"
extern char * HTAssocList_findObjectExact (
 HTAssocList * alist,
 const char * name);
// # 127 "HTAssoc.h"
extern char * HTAssocList_findObjectCaseSensitive (
 HTAssocList * list,
 const char * name);
// # 139 "HTAssoc.h"
extern char * HTAssocList_findObjectCaseSensitiveExact (
 HTAssocList * list,
 const char * name);
// # 88 "WWWUtil.h" 2
// # 99 "WWWUtil.h"
// # 1 "HTAtom.h" 1
// # 39 "HTAtom.h"
typedef struct _HTAtom HTAtom;
struct _HTAtom {
 HTAtom * next;
 char * name;
};
// # 56 "HTAtom.h"
extern HTAtom * HTAtom_for (const char * string);
extern HTAtom * HTAtom_caseFor (const char * string);
// # 80 "HTAtom.h"
extern HTList * HTAtom_templateMatches (const char * templ);
// # 91 "HTAtom.h"
extern void HTAtom_deleteAll (void);
// # 100 "WWWUtil.h" 2
// # 111 "WWWUtil.h"
// # 1 "HTChunk.h" 1
// # 50 "HTChunk.h"
typedef struct _HTChunk HTChunk;

extern HTChunk * HTChunk_new (int growby);
// # 62 "HTChunk.h"
extern void HTChunk_delete (HTChunk * ch);
// # 77 "HTChunk.h"
extern void HTChunk_clear (HTChunk * ch);
// # 91 "HTChunk.h"
extern void HTChunk_ensure (HTChunk * ch, int extra_size);
// # 101 "HTChunk.h"
extern void HTChunk_putc (HTChunk * ch, char c);
// # 112 "HTChunk.h"
extern void HTChunk_puts (HTChunk * ch, const char *str);
// # 122 "HTChunk.h"
extern void HTChunk_putb (HTChunk * ch, const char *block, int len);
// # 134 "HTChunk.h"
extern char * HTChunk_data (HTChunk * ch);
// # 144 "HTChunk.h"
extern int HTChunk_size (HTChunk * ch);
// # 160 "HTChunk.h"
extern char HTChunk_truncate (HTChunk * ch, int size);
extern char HTChunk_setSize (HTChunk * ch, int size);
// # 174 "HTChunk.h"
extern void HTChunk_terminate (HTChunk * ch);
// # 187 "HTChunk.h"
extern HTChunk * HTChunk_fromCString (char * str, int grow);
extern char * HTChunk_toCString (HTChunk * ch);
// # 205 "HTChunk.h"
extern HTChunk * HTChunk_fromBuffer (char * buf, int buflen, int size, int grow);
// # 112 "WWWUtil.h" 2
// # 146 "WWWUtil.h"
// # 1 "HTString.h" 1
// # 47 "HTString.h"
extern char * HTSACopy (char **dest, const char *src);
extern char * HTSACat (char **dest, const char *src);
// # 58 "HTString.h"
extern char * StrAllocMCopy (char ** dest, ...);
extern char * StrAllocMCat (char ** dest, ...);
// # 72 "HTString.h"
extern int strcasecomp (const char *a, const char *b);
extern int strncasecomp (const char *a, const char *b, int n);
// # 83 "HTString.h"
extern int tailcomp(const char * s1, const char * s2);
extern int tailcasecomp(const char * s1, const char * s2);
// # 110 "HTString.h"
extern char * HTStrMatch (const char * tmpl, const char * name);
extern char * HTStrCaseMatch (const char * tmpl, const char * name);
// # 121 "HTString.h"
extern char * HTStrCaseStr (char * s1, char * s2);
// # 132 "HTString.h"
extern char * HTStrip (char * s);
// # 147 "WWWUtil.h" 2
// # 162 "WWWUtil.h"
// # 1 "HTUU.h" 1
// # 38 "HTUU.h"
extern int HTUU_encode (unsigned char * bufin, unsigned int nbytes,
   char * bufcoded);

extern int HTUU_decode (char * bufcoded, unsigned char * bufplain,
   int outbufsize);
// # 163 "WWWUtil.h" 2
// # 32 "HTFile.c" 2
// # 1 "WWWCore.h" 1
// # 60 "WWWCore.h"
// # 1 "HTLib.h" 1
// # 29 "HTLib.h"
// # 1 "HTUser.h" 1
// # 36 "HTUser.h"
typedef struct _HTUserProfile HTUserProfile;
// # 51 "HTUser.h"
extern HTUserProfile * HTUserProfile_new (const char * name, void * context);
// # 61 "HTUser.h"
extern char HTUserProfile_localize (HTUserProfile * up);






extern char HTUserProfile_delete (HTUserProfile * up);
// # 83 "HTUser.h"
extern char * HTUserProfile_fqdn (HTUserProfile * up);
extern char HTUserProfile_setFqdn (HTUserProfile * up, const char * fqdn);
// # 96 "HTUser.h"
extern char * HTUserProfile_email (HTUserProfile * up);
extern char HTUserProfile_setEmail (HTUserProfile * up, const char * email);
// # 107 "HTUser.h"
extern char * HTUserProfile_news (HTUserProfile * host);
extern char HTUserProfile_setNews (HTUserProfile * host, const char * news);
// # 123 "HTUser.h"
extern char * HTUserProfile_tmp (HTUserProfile * host);
extern char HTUserProfile_setTmp (HTUserProfile * host, const char * tmp);
// # 137 "HTUser.h"
extern time_t HTUserProfile_timezone (HTUserProfile * up);
extern char HTUserProfile_setTimezone (HTUserProfile * up, time_t timezone);
// # 150 "HTUser.h"
extern void * HTUserProfile_context (HTUserProfile * up);
extern char HTUserProfile_setContext (HTUserProfile * up, void * context);
// # 30 "HTLib.h" 2
// # 40 "HTLib.h"
extern char HTLibInit (const char * AppName, const char * AppVersion);
extern char HTLibTerminate (void);
// # 51 "HTLib.h"
extern const char * HTLib_name (void);
extern const char * HTLib_version (void);
// # 61 "HTLib.h"
extern char HTLib_isInitialized (void);
// # 82 "HTLib.h"
extern const char * HTLib_appName (void);
extern char HTLib_setAppName (const char * name);
// # 95 "HTLib.h"
extern const char * HTLib_appVersion (void);
extern char HTLib_setAppVersion (const char * version);
// # 108 "HTLib.h"
extern char HTLib_secure (void);
extern void HTLib_setSecure (char mode);
// # 122 "HTLib.h"
extern HTUserProfile * HTLib_userProfile (void);
extern char HTLib_setUserProfile (HTUserProfile * up);
// # 61 "WWWCore.h" 2
// # 73 "WWWCore.h"
// # 1 "HTReq.h" 1
// # 66 "HTReq.h"
typedef long HTRequestID;
typedef struct _HTRequest HTRequest;

// # 1 "HTEvent.h" 1
// # 56 "HTEvent.h"
typedef enum _HTPriority {
    HT_PRIORITY_INV = -1,
    HT_PRIORITY_OFF = 0,
    HT_PRIORITY_MIN = 1,
    HT_PRIORITY_MAX = 20
} HTPriority;


typedef enum {
// # 75 "HTEvent.h"
    HTEvent_READ = (0x001 | 0 << 0x10),
    HTEvent_ACCEPT = (0x002 | 0 << 0x10),
    HTEvent_CLOSE = (0x004 | 0 << 0x10),
    HTEvent_WRITE = (0x008 | 1 << 0x10),
    HTEvent_CONNECT = (0x010 | 1 << 0x10),
    HTEvent_OOB = (0x020 | 2 << 0x10),
    HTEvent_TYPES = 3,






    HTEvent_TIMEOUT = (0x040 | 3 << 0x10),
    HTEvent_BEGIN = (0x000 | 3 << 0x10),
    HTEvent_END = (0x080 | 3 << 0x10),
    HTEvent_FLUSH = (0x100 | 3 << 0x10),
    HTEvent_RESET = (0x200 | 3 << 0x10),
    HTEvent_ALL = 0xFFFF
} HTEventType;
// # 112 "HTEvent.h"
extern char * HTEvent_type2str(HTEventType type);
// # 124 "HTEvent.h"
typedef int HTEventCallback (int, void *, HTEventType);
typedef struct _HTEvent HTEvent;


// # 1 "HTReq.h" 1
// # 129 "HTEvent.h" 2
// # 148 "HTEvent.h"
extern int HTEvent_register (int, HTEventType, HTEvent *);
extern int HTEvent_unregister (int, HTEventType);
// # 173 "HTEvent.h"
typedef int HTEvent_registerCallback(int, HTEventType, HTEvent *);
typedef int HTEvent_unregisterCallback(int, HTEventType);

extern void HTEvent_setRegisterCallback(HTEvent_registerCallback *);
extern void HTEvent_setUnregisterCallback(HTEvent_unregisterCallback *);
// # 188 "HTEvent.h"
extern char HTEvent_isCallbacksRegistered(void);







extern HTEvent * HTEvent_new (HTEventCallback * cbf, void * context,
         HTPriority pritority, int timeoutInMillis);
extern char HTEvent_delete (HTEvent * event);
// # 208 "HTEvent.h"
extern char HTEvent_setParam(HTEvent * event, void * param);
extern char HTEvent_setPriority(HTEvent * event, HTPriority priority);
extern char HTEvent_setTimeout(HTEvent * event, int timeoutInMillis);
extern char HTEvent_setCallback(HTEvent * event, HTEventCallback * cbf);
// # 221 "HTEvent.h"
struct _HTEvent {
    HTPriority priority;
    int millis;



    HTEventCallback * cbf;
    void * param;
    HTRequest * request;
};
// # 70 "HTReq.h" 2


// # 1 "HTFormat.h" 1
// # 74 "HTFormat.h"
// # 1 "HTStream.h" 1
// # 70 "HTStream.h"
typedef struct _HTStream HTStream;

typedef struct _HTStreamClass {

    char * name;






    int (*flush) (HTStream * me);
// # 92 "HTStream.h"
    int (*_free) (HTStream * me);
// # 102 "HTStream.h"
    int (*abort) (HTStream * me, HTList * errorlist);







    int (*put_character)(HTStream * me, char ch);

    int (*put_string) (HTStream * me, const char * str);

    int (*put_block) (HTStream * me, const char * str, int len);
// # 124 "HTStream.h"
} HTStreamClass;
// # 141 "HTStream.h"
extern HTStream * HTBlackHole (void);
// # 153 "HTStream.h"
extern HTStream * HTErrorStream (void);
// # 75 "HTFormat.h" 2


// # 1 "HTAnchor.h" 1
// # 62 "HTAnchor.h"
typedef HTAtom * HTFormat;
typedef HTAtom * HTLevel;
typedef HTAtom * HTEncoding;
typedef HTAtom * HTCharset;
typedef HTAtom * HTLanguage;

typedef struct _HTAnchor HTAnchor;
typedef struct _HTParentAnchor HTParentAnchor;
typedef struct _HTChildAnchor HTChildAnchor;

// # 1 "HTLink.h" 1
// # 42 "HTLink.h"
typedef struct _HTLink HTLink;


// # 1 "HTMethod.h" 1
// # 40 "HTMethod.h"
typedef enum {
    METHOD_INVALID = 0x0,
    METHOD_GET = 0x1,
    METHOD_HEAD = 0x2,
    METHOD_POST = 0x4,
    METHOD_PUT = 0x8,
    METHOD_PATCH = 0x10,
    METHOD_DELETE = 0x20,
    METHOD_TRACE = 0x40,
    METHOD_OPTIONS = 0x80,
    METHOD_LINK = 0x100,
// # 73 "HTMethod.h"
    METHOD_UNLINK = 0x200
} HTMethod;
// # 84 "HTMethod.h"
extern HTMethod HTMethod_enum (const char * name);
// # 94 "HTMethod.h"
extern const char * HTMethod_name (HTMethod method);
// # 132 "HTMethod.h"
extern char HTMethod_hasEntity(HTMethod me);
// # 148 "HTMethod.h"
extern char HTMethod_setExtensionMethod (HTMethod method, const char * name, char hasEntity);
extern char HTMethod_deleteExtensionMethod (HTMethod method);
// # 46 "HTLink.h" 2
// # 1 "HTAnchor.h" 1
// # 47 "HTLink.h" 2
// # 55 "HTLink.h"
typedef HTAtom * HTLinkType;

typedef enum _HTLinkResult {
    HT_LINK_INVALID = -1,
    HT_LINK_NONE = 0,
    HT_LINK_ERROR,
    HT_LINK_OK
} HTLinkResult;

struct _HTLink {
    HTAnchor * dest;
    HTLinkType type;
    HTMethod method;
    HTLinkResult result;
};

HTLink * HTLink_new (void);






char HTLink_delete (HTLink * link);






extern char HTLink_removeAll (HTAnchor * me);
// # 108 "HTLink.h"
extern char HTLink_add (
        HTAnchor * source,
        HTAnchor * destination,
        HTLinkType type,
        HTMethod method);






extern char HTLink_remove (
        HTAnchor * source,
        HTAnchor * destination);







extern HTLink * HTLink_find (HTAnchor * source, HTAnchor * destination);






extern HTLink * HTLink_findType (HTAnchor * me, HTLinkType type);
// # 148 "HTLink.h"
extern char HTLink_setDestination (HTLink * link, HTAnchor * dest);
extern HTAnchor * HTLink_destination (HTLink * link);
// # 159 "HTLink.h"
extern char HTLink_setType (HTLink * link, HTLinkType type);
extern HTLinkType HTLink_type (HTLink * link);
// # 169 "HTLink.h"
extern char HTLink_setMethod (HTLink * link, HTMethod method);
extern HTMethod HTLink_method (HTLink * link);
// # 181 "HTLink.h"
extern char HTLink_setResult (HTLink * link, HTLinkResult result);
extern HTLinkResult HTLink_result (HTLink * link);
// # 73 "HTAnchor.h" 2

// # 1 "HTResponse.h" 1
// # 45 "HTResponse.h"
typedef struct _HTResponse HTResponse;




// # 1 "HTFormat.h" 1
// # 51 "HTResponse.h" 2
// # 67 "HTResponse.h"
extern HTResponse * HTResponse_new (void);
// # 77 "HTResponse.h"
extern char HTResponse_delete (HTResponse * response);
// # 92 "HTResponse.h"
extern HTAnchor * HTResponse_redirection (HTResponse * response);
extern char HTResponse_setRedirection (HTResponse * response, HTAnchor * anchor);
// # 109 "HTResponse.h"
extern time_t HTResponse_retryTime (HTResponse * response);
extern char HTResponse_setRetryTime (HTResponse * response, time_t retry);
// # 125 "HTResponse.h"
extern char HTResponse_addChallenge (HTResponse * response, char * token, char * value);

extern char HTResponse_deleteChallengeAll (HTResponse * response);

extern HTAssocList * HTResponse_challenge (HTResponse * response);







extern char HTResponse_setRealm (HTResponse * response, char * realm);
extern const char * HTResponse_realm (HTResponse * response);







extern char HTResponse_setScheme (HTResponse * response, char * scheme);
extern const char * HTResponse_scheme (HTResponse * response);
// # 159 "HTResponse.h"
extern char HTResponse_addConnection (HTResponse * response,
                                            char * token, char * value);
extern char HTResponse_deleteConnectionAll (HTResponse * response);
extern HTAssocList * HTResponse_connection (HTResponse * response);
// # 182 "HTResponse.h"
extern char HTResponse_addProtocol (HTResponse * response,
                                          char * token, char * value);
extern char HTResponse_deleteProtocolAll (HTResponse * response);
extern HTAssocList * HTResponse_protocol (HTResponse * response);
// # 196 "HTResponse.h"
extern char HTResponse_addProtocolInfo (HTResponse * response,
                                              char * token, char * value);
extern char HTResponse_deleteProtocolInfoAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolInfo (HTResponse * response);
// # 210 "HTResponse.h"
extern char HTResponse_addProtocolRequest (HTResponse * response,
                                                 char * token, char * value);
extern char HTResponse_deleteProtocolRequestAll (HTResponse * response);
extern HTAssocList * HTResponse_protocolRequest (HTResponse * response);
// # 225 "HTResponse.h"
extern char HTResponse_addCacheControl (HTResponse * response,
                                              char * token, char *value);
extern char HTResponse_deleteCacheControlAll (HTResponse * response);
extern HTAssocList * HTResponse_cacheControl (HTResponse * response);
// # 239 "HTResponse.h"
typedef enum _HTCachable {
    HT_NO_CACHE = 0,
    HT_CACHE_ALL = 1,
    HT_CACHE_ETAG = 2,
    HT_CACHE_NOT_MODIFIED = 3
} HTCachable;

extern HTCachable HTResponse_isCachable (HTResponse * me);
extern char HTResponse_setCachable (HTResponse * me, HTCachable mode);
// # 256 "HTResponse.h"
extern char * HTResponse_etag (HTResponse * me);
// # 270 "HTResponse.h"
extern char HTResponse_isCached (HTResponse * me, char mode);
// # 280 "HTResponse.h"
extern time_t HTResponse_maxAge (HTResponse * response);
extern char HTResponse_mustRevalidate (HTResponse * response);
extern char * HTResponse_noCache (HTResponse * response);
// # 299 "HTResponse.h"
extern char HTResponse_addRange (HTResponse * response,
                                       char * unit, char * range);
extern char HTResponse_deleteRangeAll (HTResponse * response);
extern HTAssocList * HTResponse_range (HTResponse * response);
// # 313 "HTResponse.h"
extern long int HTResponse_length (HTResponse * response);
extern void HTResponse_setLength (HTResponse * response, long int length);
extern void HTResponse_addLength (HTResponse * response, long int deltalength);
// # 326 "HTResponse.h"
extern HTFormat HTResponse_format (HTResponse * response);
extern void HTResponse_setFormat (HTResponse * response, HTFormat format);
// # 337 "HTResponse.h"
extern HTAssocList * HTResponse_formatParam (HTResponse * response);
extern char HTResponse_addFormatParam (HTResponse * response,
     const char * name, const char * value);







extern HTCharset HTResponse_charset (HTResponse * response);
extern char HTResponse_setCharset (HTResponse * response, HTCharset charset);







extern HTLevel HTResponse_level (HTResponse * response);
extern char HTResponse_setLevel (HTResponse * response, HTLevel level);
// # 367 "HTResponse.h"
extern HTList * HTResponse_encoding (HTResponse * response);
extern char HTResponse_addEncoding (HTResponse * response, HTEncoding enc);
// # 378 "HTResponse.h"
extern HTList * HTResponse_transfer (HTResponse * response);
extern char HTResponse_addTransfer (HTResponse * response, HTEncoding te);
// # 389 "HTResponse.h"
extern HTEncoding HTResponse_contentTransferEncoding (HTResponse * response);
extern char HTResponse_setContentTransferEncoding (HTResponse * response,
                                                   HTEncoding cte);
// # 401 "HTResponse.h"
extern char HTResponse_addVariant (HTResponse * me, char * token, char * value);
extern char HTResponse_deleteVariantAll (HTResponse * me);
extern HTAssocList * HTResponse_variant (HTResponse * me);
// # 413 "HTResponse.h"
extern char HTResponse_addTrailer (HTResponse * me, char * token, char * value);
extern char HTResponse_deleteTrailerAll (HTResponse * me);
extern HTAssocList * HTResponse_trailer (HTResponse * me);
// # 428 "HTResponse.h"
extern char HTResponse_addHeader (HTResponse * response,
                                        char * token, char * value);
extern char HTResponse_deleteHeaderAll (HTResponse * response);
extern HTAssocList * HTResponse_header (HTResponse * response);

extern HTAssocList * HTResponse_handOverHeader (HTResponse * me);
// # 444 "HTResponse.h"
extern char * HTResponse_reason (HTResponse * me);
extern char HTResponse_setReason (HTResponse * me, char * reason);
// # 75 "HTAnchor.h" 2
// # 120 "HTAnchor.h"
extern HTAnchor * HTAnchor_findAddress (const char * address);
// # 133 "HTAnchor.h"
extern HTChildAnchor * HTAnchor_findChild (HTParentAnchor *parent,
       const char * tag);
// # 148 "HTAnchor.h"
extern HTChildAnchor * HTAnchor_findChildAndLink (
  HTParentAnchor * parent,
  const char * tag,
  const char * href,
  HTLinkType ltype);
// # 164 "HTAnchor.h"
extern char HTAnchor_delete (HTParentAnchor *me);
// # 179 "HTAnchor.h"
extern char HTAnchor_clearAll (HTList * documents);
// # 196 "HTAnchor.h"
extern char HTAnchor_deleteAll (HTList * objects);
// # 214 "HTAnchor.h"
extern HTArray * HTAnchor_getArray (int growby);
// # 233 "HTAnchor.h"
extern char HTAnchor_setMainLink (HTAnchor * anchor, HTLink * link);
extern HTLink * HTAnchor_mainLink (HTAnchor * anchor);

extern HTAnchor * HTAnchor_followMainLink (HTAnchor * anchor);







extern char HTAnchor_setSubLinks (HTAnchor * anchor, HTList * list);
extern HTList * HTAnchor_subLinks (HTAnchor * anchor);
// # 259 "HTAnchor.h"
extern HTLink * HTAnchor_findLinkType (HTAnchor * me, HTLinkType type);
// # 275 "HTAnchor.h"
extern HTParentAnchor * HTAnchor_parent (HTAnchor *me);
// # 284 "HTAnchor.h"
extern char HTAnchor_hasChildren (HTParentAnchor *me);







extern char HTAnchor_isChild (HTAnchor * me);
// # 307 "HTAnchor.h"
extern char * HTAnchor_view (HTAnchor * me);
// # 326 "HTAnchor.h"
extern char * HTAnchor_address (HTAnchor * me);
// # 344 "HTAnchor.h"
extern char * HTAnchor_expandedAddress (HTAnchor * me);
// # 353 "HTAnchor.h"
extern char * HTAnchor_physical (HTParentAnchor * me);
extern void HTAnchor_setPhysical (HTParentAnchor * me, char * protocol);
extern void HTAnchor_clearPhysical (HTParentAnchor * me);
// # 369 "HTAnchor.h"
extern void HTAnchor_setDocument (HTParentAnchor *me, void * doc);
extern void * HTAnchor_document (HTParentAnchor *me);
// # 384 "HTAnchor.h"
extern void HTAnchor_clearHeader (HTParentAnchor *me);
// # 397 "HTAnchor.h"
extern char HTAnchor_update (HTParentAnchor * me, HTResponse * response);






extern void HTAnchor_clearIndex (HTParentAnchor * me);
extern void HTAnchor_setIndex (HTParentAnchor * me);
extern char HTAnchor_isIndex (HTParentAnchor * me);
// # 418 "HTAnchor.h"
extern const char * HTAnchor_title (HTParentAnchor *me);
extern void HTAnchor_setTitle (HTParentAnchor *me,
      const char * title);
extern void HTAnchor_appendTitle (HTParentAnchor *me,
      const char * title);
// # 431 "HTAnchor.h"
extern HTAssocList * HTAnchor_meta (HTParentAnchor * me);
extern char HTAnchor_addMeta (HTParentAnchor * me,
         const char * name, const char * value);
// # 447 "HTAnchor.h"
extern char * HTAnchor_robots (HTParentAnchor * me);
// # 461 "HTAnchor.h"
extern char * HTAnchor_base (HTParentAnchor * me);
extern char HTAnchor_setBase (HTParentAnchor * me, char * base);
// # 475 "HTAnchor.h"
extern char * HTAnchor_location (HTParentAnchor * me);
extern char HTAnchor_setLocation (HTParentAnchor * me, char * location);







extern HTFormat HTAnchor_format (HTParentAnchor *me);
extern void HTAnchor_setFormat (HTParentAnchor *me,
      HTFormat form);
// # 499 "HTAnchor.h"
extern HTAssocList * HTAnchor_formatParam (HTParentAnchor * me);

extern char HTAnchor_addFormatParam (HTParentAnchor * me,
     const char * name, const char * value);







extern HTCharset HTAnchor_charset (HTParentAnchor *me);
extern char HTAnchor_setCharset (HTParentAnchor *me,
      HTCharset charset);







extern HTLevel HTAnchor_level (HTParentAnchor * me);
extern char HTAnchor_setLevel (HTParentAnchor * me,
      HTLevel level);







extern HTList * HTAnchor_language (HTParentAnchor * me);
extern char HTAnchor_addLanguage (HTParentAnchor *me, HTLanguage lang);
extern char HTAnchor_deleteLanguageAll (HTParentAnchor * me);







extern HTList * HTAnchor_encoding (HTParentAnchor * me);
extern char HTAnchor_addEncoding (HTParentAnchor * me, HTEncoding enc);
extern char HTAnchor_deleteEncoding (HTParentAnchor * me, HTEncoding enc);
extern char HTAnchor_deleteEncodingAll (HTParentAnchor * me);
// # 553 "HTAnchor.h"
extern HTEncoding HTAnchor_contentTransferEncoding (HTParentAnchor *me);
extern void HTAnchor_setContentTransferEncoding (HTParentAnchor *me,
                       HTEncoding cte);







extern long int HTAnchor_length (HTParentAnchor * me);
extern void HTAnchor_setLength (HTParentAnchor * me, long int length);
extern void HTAnchor_addLength (HTParentAnchor * me, long int deltalength);







extern char * HTAnchor_md5 (HTParentAnchor * me);
extern char HTAnchor_setMd5 (HTParentAnchor * me, const char * hash);







extern HTMethod HTAnchor_allow (HTParentAnchor * me);
extern void HTAnchor_setAllow (HTParentAnchor * me, HTMethod methodset);
extern void HTAnchor_appendAllow (HTParentAnchor * me, HTMethod methodset);







extern char * HTAnchor_version (HTParentAnchor * me);
extern void HTAnchor_setVersion (HTParentAnchor * me, const char * version);
// # 603 "HTAnchor.h"
extern time_t HTAnchor_date (HTParentAnchor * me);
extern void HTAnchor_setDate (HTParentAnchor * me, const time_t date);
// # 614 "HTAnchor.h"
extern time_t HTAnchor_lastModified (HTParentAnchor * me);
extern void HTAnchor_setLastModified (HTParentAnchor * me, const time_t lm);
// # 627 "HTAnchor.h"
extern char * HTAnchor_etag (HTParentAnchor * me);
extern void HTAnchor_setEtag (HTParentAnchor * me, const char * etag);
extern char HTAnchor_isEtagWeak (HTParentAnchor * me);
// # 642 "HTAnchor.h"
extern time_t HTAnchor_age (HTParentAnchor * me);
extern void HTAnchor_setAge (HTParentAnchor * me, const time_t age);







extern time_t HTAnchor_expires (HTParentAnchor * me);
extern void HTAnchor_setExpires (HTParentAnchor * me, const time_t exp);







extern char * HTAnchor_derived (HTParentAnchor *me);
extern void HTAnchor_setDerived (HTParentAnchor *me, const char *derived_from);
// # 672 "HTAnchor.h"
extern char HTAnchor_headerParsed (HTParentAnchor *me);
extern void HTAnchor_setHeaderParsed (HTParentAnchor *me);
// # 684 "HTAnchor.h"
extern char HTAnchor_setHeader (HTParentAnchor * me, HTAssocList * list);
extern HTAssocList * HTAnchor_header (HTParentAnchor * me);
// # 78 "HTFormat.h" 2
// # 267 "HTFormat.h"
typedef HTStream * HTConverter (HTRequest * request,
     void * param,
     HTFormat input_format,
     HTFormat output_format,
     HTStream * output_stream);

extern void HTConversion_add (HTList * conversions,
    const char * rep_in,
    const char * rep_out,
    HTConverter * converter,
    double quality,
    double secs,
    double secs_per_byte);

extern void HTConversion_deleteAll (HTList * list);
// # 301 "HTFormat.h"
extern void HTPresentation_setConverter (HTConverter * pconv);
extern HTConverter * HTPresentation_converter (void);






extern void HTPresentation_add (HTList * conversions,
    const char * representation,
    const char * command,
    const char * test_command,
    double quality,
    double secs,
    double secs_per_byte);

extern void HTPresentation_deleteAll (HTList * list);
// # 328 "HTFormat.h"
extern HTConverter HTThroughLine;
extern HTConverter HTBlackHoleConverter;
extern HTConverter HTSaveConverter;
// # 369 "HTFormat.h"
typedef HTStream * HTCoder (HTRequest * request,
     void * param,
     HTEncoding coding,
     HTStream * target);
// # 387 "HTFormat.h"
extern HTCoder HTIdentityCoding;
// # 398 "HTFormat.h"
typedef struct _HTCoding HTCoding;
// # 436 "HTFormat.h"
extern char HTCoding_add (HTList * list,
    const char * encoding,
    HTCoder * encoder,
    HTCoder * decoder,
    double quality);

extern void HTCoding_deleteAll (HTList * list);

extern const char * HTCoding_name (HTCoding * me);

extern double HTCoding_quality (HTCoding * me);
// # 461 "HTFormat.h"
extern void HTCharset_add (HTList * list, const char * charset, double quality);



typedef struct _HTAcceptNode {
    HTAtom * atom;
    double quality;
} HTAcceptNode;




extern void HTCharset_deleteAll (HTList * list);
// # 489 "HTFormat.h"
extern void HTLanguage_add (HTList * list, const char * lang, double quality);
extern void HTLanguage_deleteAll (HTList * list);
// # 510 "HTFormat.h"
extern void HTFormat_setConversion (HTList * list);
extern HTList * HTFormat_conversion (void);

extern void HTFormat_addConversion (const char * input_format,
        const char * output_format,
        HTConverter * converter,
        double quality,
        double secs,
        double secs_per_byte);






extern void HTFormat_setContentCoding (HTList * list);
extern HTList * HTFormat_contentCoding (void);

extern char HTFormat_addCoding ( char * encoding,
     HTCoder * encoder,
     HTCoder * decoder,
     double quality);
// # 549 "HTFormat.h"
extern void HTFormat_setTransferCoding (HTList * list);
extern HTList * HTFormat_transferCoding (void);

extern char HTFormat_addTransferCoding ( char * encoding,
      HTCoder * encoder,
      HTCoder * decoder,
      double quality);
// # 574 "HTFormat.h"
extern void HTFormat_setLanguage (HTList * list);
extern HTList * HTFormat_language (void);






extern void HTFormat_setCharset (HTList * list);
extern HTList * HTFormat_charset (void);
// # 592 "HTFormat.h"
extern void HTFormat_deleteAll (void);
// # 608 "HTFormat.h"
extern HTStream * HTStreamStack (HTFormat rep_in,
     HTFormat rep_out,
     HTStream * output_stream,
     HTRequest * request,
     char guess);
// # 622 "HTFormat.h"
extern double HTStackValue (HTList * conversions,
     HTFormat format_in,
     HTFormat format_out,
     double initial_value,
     long int length);
// # 645 "HTFormat.h"
extern HTStream * HTContentCodingStack (HTEncoding coding,
     HTStream * target,
     HTRequest * request,
     void * param,
     char encoding);







extern HTStream * HTContentEncodingStack (HTList * encodings,
       HTStream * target,
       HTRequest * request,
       void * param);
// # 670 "HTFormat.h"
extern HTStream * HTContentDecodingStack (HTList * encodings,
       HTStream * target,
       HTRequest * request,
       void * param);
// # 692 "HTFormat.h"
extern HTStream * HTTransferCodingStack (HTEncoding coding,
      HTStream * target,
      HTRequest * request,
      void * param,
      char encoding);







extern HTStream * HTTransferEncodingStack (HTList * encodings,
        HTStream * target,
        HTRequest * request,
        void * param);
// # 717 "HTFormat.h"
extern HTStream * HTTransferDecodingStack (HTList * encodings,
       HTStream * target,
       HTRequest * request,
       void * param);
// # 732 "HTFormat.h"
extern HTStream * HTContentTransferCodingStack (HTEncoding encoding,
             HTStream * target,
             HTRequest * request,
             void * param,
             char encode);
// # 745 "HTFormat.h"
typedef struct _HTPresentation {
    HTFormat rep;
    HTFormat rep_out;
    HTConverter *converter;
    char * command;
    char * test_command;
    double quality;
    double secs;
    double secs_per_byte;
} HTPresentation;
// # 73 "HTReq.h" 2

// # 1 "HTError.h" 1
// # 36 "HTError.h"
typedef struct _HTError HTError;

typedef enum _HTSeverity {
    ERR_UNKNOWN = 0x0,
    ERR_FATAL = 0x1,
    ERR_NON_FATAL = 0x2,
    ERR_WARN = 0x4,
    ERR_INFO = 0x8
} HTSeverity;
// # 61 "HTError.h"
typedef struct _HTErrorMessage {
    int code;
    char * msg;
    char * url;
} HTErrorMessage;
// # 82 "HTError.h"
typedef enum _HTErrorElement {
 HTERR_CONTINUE = 0,
 HTERR_SWITCHING,
 HTERR_OK,
 HTERR_CREATED,
 HTERR_ACCEPTED,
 HTERR_NON_AUTHORITATIVE,
 HTERR_NO_CONTENT,
 HTERR_RESET,
 HTERR_PARTIAL,
 HTERR_PARTIAL_OK,
 HTERR_MULTIPLE,
 HTERR_MOVED,
 HTERR_FOUND,
 HTERR_METHOD,
 HTERR_NOT_MODIFIED,
 HTERR_USE_PROXY,
 HTERR_PROXY_REDIRECT,
 HTERR_TEMP_REDIRECT,
 HTERR_BAD_REQUEST,
 HTERR_UNAUTHORIZED,
 HTERR_PAYMENT_REQUIRED,
 HTERR_FORBIDDEN,
 HTERR_NOT_FOUND,
 HTERR_NOT_ALLOWED,
 HTERR_NONE_ACCEPTABLE,
 HTERR_PROXY_UNAUTHORIZED,
 HTERR_TIMEOUT,
 HTERR_CONFLICT,
 HTERR_GONE,
 HTERR_LENGTH_REQUIRED,
 HTERR_PRECON_FAILED,
 HTERR_TOO_BIG,
 HTERR_URI_TOO_BIG,
 HTERR_UNSUPPORTED,
 HTERR_BAD_RANGE,
 HTERR_EXPECTATION_FAILED,
 HTERR_REAUTH,
 HTERR_PROXY_REAUTH,
 HTERR_INTERNAL,
 HTERR_NOT_IMPLEMENTED,
 HTERR_BAD_GATE,
 HTERR_DOWN,
 HTERR_GATE_TIMEOUT,
 HTERR_BAD_VERSION,
 HTERR_NO_PARTIAL_UPDATE,
// # 138 "HTError.h"
 HTERR_STALE,
 HTERR_REVALIDATION_FAILED,
 HTERR_DISCONNECTED_CACHE,
 HTERR_HEURISTIC_EXPIRATION,
 HTERR_TRANSFORMED,
 HTERR_CACHE,


 HTERR_NO_REMOTE_HOST,
 HTERR_NO_HOST,
 HTERR_NO_FILE,
 HTERR_FTP_SERVER,
 HTERR_FTP_NO_RESPONSE,
        HTERR_FTP_LOGIN_FAILURE,
 HTERR_TIME_OUT,
 HTERR_GOPHER_SERVER,
 HTERR_INTERRUPTED,
 HTERR_CON_INTR,
 HTERR_CSO_SERVER,
 HTERR_HTTP09,
 HTERR_BAD_REPLY,
 HTERR_UNKNOWN_AA,
 HTERR_NEWS_SERVER,
 HTERR_FILE_TO_FTP,
 HTERR_AUTO_REDIRECT,
 HTERR_MAX_REDIRECT,
 HTERR_EOF,
 HTERR_WAIS_OVERFLOW,
 HTERR_WAIS_MODULE,
 HTERR_WAIS_NO_CONNECT,
 HTERR_SYSTEM,
 HTERR_CLASS,
 HTERR_ACCESS,
 HTERR_LOGIN,
        HTERR_CACHE_EXPIRED,
        HTERR_NO_AUTO_RULES,
        HTERR_NO_AUTO_PROXY,
 HTERR_ELEMENTS
} HTErrorElement;
// # 203 "HTError.h"
typedef enum _HTErrorShow {
    HT_ERR_SHOW_FATAL = 0x1,
    HT_ERR_SHOW_NON_FATAL = 0x3,
    HT_ERR_SHOW_WARNING = 0x7,
    HT_ERR_SHOW_INFO = 0xF,
    HT_ERR_SHOW_PARS = 0x10,
    HT_ERR_SHOW_LOCATION = 0x20,
    HT_ERR_SHOW_IGNORE = 0x40,
    HT_ERR_SHOW_FIRST = 0x80,
    HT_ERR_SHOW_LINKS = 0x100,
    HT_ERR_SHOW_DEFAULT = 0x13,
    HT_ERR_SHOW_DETAILED = 0x1F,
    HT_ERR_SHOW_DEBUG = 0x7F
} HTErrorShow;

extern HTErrorShow HTError_show (void);
extern char HTError_setShow (HTErrorShow mask);
// # 236 "HTError.h"
extern char HTError_add (HTList * list,
    HTSeverity severity,
    char ignore,
    int element,
    void * par,
    unsigned int length,
    char * where);
// # 256 "HTError.h"
extern char HTError_addSystem (HTList * list,
          HTSeverity severity,
          int errornumber,
          char ignore,
          char * syscall);
// # 270 "HTError.h"
extern char HTError_deleteAll (HTList * list);
// # 280 "HTError.h"
extern char HTError_deleteLast (HTList * list);
// # 293 "HTError.h"
extern char HTError_doShow (HTError * info);
// # 304 "HTError.h"
extern char HTError_ignoreLast (HTList * list);
extern char HTError_setIgnore (HTError * info);
// # 319 "HTError.h"
extern int HTError_index (HTError * info);
// # 328 "HTError.h"
extern HTSeverity HTError_severity (HTError * info);






extern char HTError_hasSeverity (HTList * list, HTSeverity severity);







extern void * HTError_parameter (HTError * info, int * length);







extern const char * HTError_location (HTError * info);
// # 75 "HTReq.h" 2
// # 1 "HTNet.h" 1
// # 52 "HTNet.h"
typedef struct _HTNet HTNet;




// # 1 "HTTrans.h" 1
// # 52 "HTTrans.h"
typedef struct _HTTransport HTTransport;

typedef enum _HTTransportMode {
    HT_TP_SINGLE = 0,
    HT_TP_PIPELINE = 1,
    HT_TP_INTERLEAVE = 2
} HTTransportMode;

// # 1 "HTIOStream.h" 1
// # 33 "HTIOStream.h"
typedef struct _HTInputStream HTInputStream;
typedef struct _HTOutputStream HTOutputStream;



// # 1 "HTChannl.h" 1
// # 41 "HTChannl.h"
typedef struct _HTChannel HTChannel;

// # 1 "HTHost.h" 1
// # 32 "HTHost.h"
typedef struct _HTHost HTHost;


// # 1 "HTChannl.h" 1
// # 36 "HTHost.h" 2


// # 1 "HTProt.h" 1
// # 57 "HTProt.h"
typedef struct _HTProtocol HTProtocol;
typedef u_short HTProtocolId;




// # 1 "HTTrans.h" 1
// # 64 "HTProt.h" 2
// # 81 "HTProt.h"
typedef int HTProtCallback (int, HTRequest *);

extern char HTProtocol_add (const char * name,
                            const char * transport,
                            HTProtocolId port,
                            char preemptive,
                            HTProtCallback * client,
                            HTProtCallback * server);







extern char HTProtocol_delete (const char * name);






extern char HTProtocol_deleteAll (void);
// # 114 "HTProt.h"
extern HTProtocol * HTProtocol_find (HTRequest * request, const char * access);







extern HTProtCallback * HTProtocol_client (HTProtocol * protocol);
extern HTProtCallback * HTProtocol_server (HTProtocol * protocol);
// # 132 "HTProt.h"
extern HTProtocolId HTProtocol_id (HTProtocol * protocol);







extern const char * HTProtocol_name (HTProtocol * protocol);







extern char HTProtocol_preemptive (HTProtocol * protocol);







extern char HTProtocol_setTransport (HTProtocol * protoccol,
                                     const char * transport);
extern const char * HTProtocol_transport (HTProtocol * protocol);
// # 39 "HTHost.h" 2
// # 1 "HTTimer.h" 1
// # 52 "HTTimer.h"
typedef struct _HTTimer HTTimer;

typedef int HTTimerCallback (HTTimer *, void *, HTEventType type);
// # 64 "HTTimer.h"
extern HTTimer * HTTimer_new (HTTimer *, HTTimerCallback *,
         void *, ms_t millis,
                              char relative, char repetitive);
extern char HTTimer_delete (HTTimer * timer);
extern char HTTimer_deleteAll (void);
extern char HTTimer_expireAll (void);
// # 79 "HTTimer.h"
extern int HTTimer_dispatch (HTTimer * timer);
// # 90 "HTTimer.h"
extern int HTTimer_next (ms_t * pSoonest);







extern char HTTimer_refresh(HTTimer * timer, ms_t now);
// # 111 "HTTimer.h"
extern ms_t HTTimer_expiresAbsolute (HTTimer * timer);
// # 123 "HTTimer.h"
extern ms_t HTTimer_expiresRelative (HTTimer * timer);
// # 133 "HTTimer.h"
extern char HTTimer_hasTimerExpired (HTTimer * timer);







extern HTTimerCallback * HTTimer_callback (HTTimer * timer);







extern char HTTimer_isRelative (HTTimer * timer);
// # 161 "HTTimer.h"
typedef char HTTimerSetCallback (HTTimer * timer);

extern char HTTimer_registerSetTimerCallback (HTTimerSetCallback * cbf);
extern char HTTimer_registerDeleteTimerCallback (HTTimerSetCallback * cbf);
// # 40 "HTHost.h" 2
// # 65 "HTHost.h"
extern HTHost * HTHost_new (char * host, u_short u_port);
extern HTHost * HTHost_newWParse(HTRequest * request, char * url, u_short u_port);
extern int HTHost_hash (HTHost * host);
// # 84 "HTHost.h"
extern HTHost * HTHost_find (char * host);
// # 94 "HTHost.h"
extern void HTHost_deleteAll (void);
// # 106 "HTHost.h"
extern char HTHost_isIdle (HTHost * host);
// # 124 "HTHost.h"
extern char * HTHost_name (HTHost * host);
// # 141 "HTHost.h"
extern char * HTHost_class (HTHost * host);
extern void HTHost_setClass (HTHost * host, char * s_class);

extern int HTHost_version (HTHost * host);
extern void HTHost_setVersion (HTHost * host, int version);
// # 155 "HTHost.h"
extern HTMethod HTHost_publicMethods (HTHost * me);
extern void HTHost_setPublicMethods (HTHost * me, HTMethod methodset);
extern void HTHost_appendPublicMethods (HTHost * me, HTMethod methodset);
// # 167 "HTHost.h"
extern char * HTHost_server (HTHost * host);
extern char HTHost_setServer (HTHost * host, const char * server);
// # 178 "HTHost.h"
extern char * HTHost_userAgent (HTHost * host);
extern char HTHost_setUserAgent (HTHost * host, const char * userAgent);
// # 199 "HTHost.h"
extern char * HTHost_rangeUnits (HTHost * host);
extern char HTHost_setRangeUnits (HTHost * host, const char * units);
extern char HTHost_isRangeUnitAcceptable (HTHost * host, const char * unit);
// # 212 "HTHost.h"
extern void HTHost_setContext (HTHost * me, void * context);
extern void * HTHost_context (HTHost * me);
// # 227 "HTHost.h"
extern int HTHost_addNet (HTHost * host, HTNet * net);
extern char HTHost_deleteNet (HTHost * host, HTNet * net, int status);

extern HTList * HTHost_net (HTHost * host);
// # 249 "HTHost.h"
extern int HTHost_connect (HTHost * host, HTNet * net, char * url);

extern int HTHost_accept (HTHost * host, HTNet * net, char * url);

extern int HTHost_listen (HTHost * host, HTNet * net, char * url);
// # 265 "HTHost.h"
extern char HTHost_setCloseNotification (HTHost * host, char mode);
extern char HTHost_closeNotification (HTHost * host);
// # 277 "HTHost.h"
extern char HTHost_setChannel (HTHost * host, HTChannel * channel);
extern HTChannel * HTHost_channel (HTHost * host);
// # 289 "HTHost.h"
extern char HTHost_clearChannel (HTHost * host, int status);
// # 305 "HTHost.h"
extern HTTransportMode HTHost_mode (HTHost * host, char * active);
extern char HTHost_setMode (HTHost * host, HTTransportMode mode);
// # 327 "HTHost.h"
extern HTNet * HTHost_nextPendingNet (HTHost * host);







extern HTHost * HTHost_nextPendingHost (void);
// # 348 "HTHost.h"
extern char HTHost_launchPending (HTHost * host);
// # 359 "HTHost.h"
extern void HTHost_enable_PendingReqLaunch (void);
extern void HTHost_disable_PendingReqLaunch (void);
// # 375 "HTHost.h"
extern char HTHost_setPersistent (HTHost * host, char persistent,
                                  HTTransportMode mode);
extern char HTHost_isPersistent (HTHost * host);
// # 398 "HTHost.h"
extern char HTHost_setActiveTimeout (ms_t timeout);
extern ms_t HTHost_activeTimeout (void);






extern time_t HTHost_persistTimeout (void);
extern char HTHost_setPersistTimeout (time_t timeout);






extern void HTHost_setPersistExpires (HTHost * host, time_t expires);
extern time_t HTHost_persistExpires (HTHost * host);
// # 429 "HTHost.h"
extern void HTHost_setReqsPerConnection (HTHost * host, int reqs);
extern int HTHost_reqsPerConnection (HTHost * host);
extern void HTHost_setReqsMade (HTHost * host, int reqs);
extern int HTHost_reqsMade (HTHost * host);
// # 447 "HTHost.h"
extern HTNet * HTHost_firstNet (HTHost * host);
extern HTNet * HTHost_getReadNet (HTHost * host);
extern HTNet * HTHost_getWriteNet (HTHost * host);







extern HTInputStream * HTHost_getInput (HTHost * host, HTTransport * transport,
            void * param, int mode);

extern HTOutputStream * HTHost_getOutput (HTHost * host, HTTransport * tp,
       void * param, int mode);
// # 472 "HTHost.h"
extern int HTHost_read(HTHost * host, HTNet * net);

extern char HTHost_setConsumed(HTHost * host, size_t bytes);
extern char HTHost_setRemainingRead(HTHost * host, size_t remainaing);
extern size_t HTHost_remainingRead (HTHost * host);
// # 495 "HTHost.h"
extern char HTHost_setMaxPipelinedRequests (int max);
extern int HTHost_maxPipelinedRequests (void);
// # 507 "HTHost.h"
extern int HTHost_numberOfOutstandingNetObjects (HTHost * host);
extern int HTHost_numberOfPendingNetObjects (HTHost * host);
// # 523 "HTHost.h"
extern char HTHost_recoverPipe (HTHost * host);
extern char HTHost_doRecover (HTHost * host);
// # 537 "HTHost.h"
extern char HTHost_killPipe (HTHost * host);
// # 548 "HTHost.h"
extern int HTHost_register(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_unregister(HTHost * host, HTNet * net, HTEventType type);
extern int HTHost_tickleFirstNet(HTHost * host, HTEventType type);

extern SockA * HTHost_getSockAddr(HTHost * host);
// # 565 "HTHost.h"
extern int HTHost_eventTimeout (void);
extern void HTHost_setEventTimeout (int millis);
// # 577 "HTHost.h"
extern char HTHost_setWriteDelay (HTHost * host, ms_t delay);
extern ms_t HTHost_writeDelay (HTHost * host);
extern int HTHost_findWriteDelay(HTHost * host, ms_t lastFlushTime, int buffSize);
// # 588 "HTHost.h"
extern int HTHost_forceFlush(HTHost * host);







extern char HTHost_setDefaultWriteDelay (ms_t delay);
extern ms_t HTHost_defaultWriteDelay (void);
// # 610 "HTHost.h"
extern char HTHost_setHome (HTHost * host, int home);
extern int HTHost_home (HTHost * host);

extern char HTHost_setRetry (HTHost * host, int retry);
extern int HTHost_retry (HTHost * host);
extern char HTHost_decreaseRetry (HTHost * host);
// # 626 "HTHost.h"
typedef int HTHost_ActivateRequestCallback (HTRequest * request);
extern void HTHost_setActivateRequestCallback
  (HTHost_ActivateRequestCallback * cbf);
// # 44 "HTChannl.h" 2
// # 1 "HTIOStream.h" 1
// # 45 "HTChannl.h" 2
// # 62 "HTChannl.h"
extern HTChannel * HTChannel_new (int sockfd, FILE * fp, char active);







extern char HTChannel_delete (HTChannel * channel, int status);
extern char HTChannel_deleteAll (void);
extern char HTChannel_safeDeleteAll (void);
// # 82 "HTChannl.h"
extern HTChannel * HTChannel_find (int sockfd);
// # 95 "HTChannl.h"
extern int HTChannel_socket (HTChannel * channel);
extern char HTChannel_setSocket (HTChannel * channel, int socket);

extern FILE * HTChannel_file (HTChannel * channel);
extern char HTChannel_setFile (HTChannel * channel, FILE * fp);
// # 109 "HTChannl.h"
extern char HTChannel_setHost (HTChannel * ch, HTHost * host);
extern HTHost * HTChannel_host (HTChannel * ch);
// # 123 "HTChannl.h"
extern void HTChannel_upSemaphore (HTChannel * channel);
extern void HTChannel_downSemaphore (HTChannel * channel);
extern void HTChannel_setSemaphore (HTChannel * channel, int semaphore);
// # 141 "HTChannl.h"
extern char HTChannel_setInput (HTChannel * ch, HTInputStream * input);
extern HTInputStream * HTChannel_input (HTChannel * ch);
extern char HTChannel_deleteInput (HTChannel * channel, int status);

extern char HTChannel_setOutput (HTChannel * ch, HTOutputStream * output);
extern HTOutputStream * HTChannel_output (HTChannel * ch);
extern char HTChannel_deleteOutput (HTChannel * channel, int status);

extern HTInputStream * HTChannel_getChannelIStream (HTChannel * ch);
extern HTOutputStream * HTChannel_getChannelOStream (HTChannel * ch);
// # 39 "HTIOStream.h" 2
// # 49 "HTIOStream.h"
typedef struct _HTInputStreamClass {

    char * name;






    int (*flush) (HTInputStream * me);
// # 69 "HTIOStream.h"
    int (*_free) (HTInputStream * me);
// # 80 "HTIOStream.h"
    int (*abort) (HTInputStream * me, HTList * errorlist);







    int (*read) (HTInputStream * me);







    int (*close) (HTInputStream * me);







    int (*consumed) (HTInputStream * me, size_t bytes);
// # 113 "HTIOStream.h"
} HTInputStreamClass;
// # 126 "HTIOStream.h"
typedef struct _HTOutputStreamClass {

    char * name;

    int (*flush) (HTOutputStream * me);

    int (*_free) (HTOutputStream * me);

    int (*abort) (HTOutputStream * me, HTList * errorlist);

    int (*put_character)(HTOutputStream * me, char ch);

    int (*put_string) (HTOutputStream * me, const char * str);

    int (*put_block) (HTOutputStream * me, const char * str, int len);
// # 150 "HTIOStream.h"
    int (*close) (HTOutputStream * me);
// # 159 "HTIOStream.h"
} HTOutputStreamClass;
// # 182 "HTIOStream.h"
typedef HTInputStream * HTInput_new (HTHost * host,
      HTChannel * ch,
      void * param,
      int mode);







typedef HTOutputStream * HTOutput_new (HTHost * host,
      HTChannel * ch,
      void * param,
      int mode);







typedef HTOutputStream * HTOutputConverter_new(
 HTHost * host,
 HTChannel * ch,
 void * param,
 int mode,
 HTOutputStream * target);
// # 61 "HTTrans.h" 2
// # 75 "HTTrans.h"
extern char HTTransport_add (const char * name,
        HTTransportMode mode,
        HTInput_new * get_input,
        HTOutput_new * get_output);
// # 89 "HTTrans.h"
extern char HTTransport_delete (const char * name);
// # 100 "HTTrans.h"
extern char HTTransport_deleteAll (void);
// # 114 "HTTrans.h"
extern HTTransport * HTTransport_find (HTRequest * request, const char * name);
// # 126 "HTTrans.h"
extern HTTransportMode HTTransport_mode (HTTransport * tp);
extern char HTTransport_setMode (HTTransport * tp, HTTransportMode mode);







struct _HTTransport {
    char * name;
    HTTransportMode mode;
    HTInput_new * input_new;
    HTOutput_new * output_new;
};
// # 58 "HTNet.h" 2



// # 1 "HTDNS.h" 1
// # 43 "HTDNS.h"
typedef struct _HTdns HTdns;
// # 58 "HTDNS.h"
extern void HTDNS_setTimeout (time_t timeout);
extern time_t HTDNS_timeout (time_t timeout);
// # 74 "HTDNS.h"
extern HTdns * HTDNS_add (HTList * list, struct hostent * element,
     char * host, int * homes);
// # 85 "HTDNS.h"
extern char HTDNS_delete (const char * host);
// # 96 "HTDNS.h"
extern char HTDNS_deleteAll (void);
// # 110 "HTDNS.h"
extern char HTDNS_updateWeigths (HTdns *dns, int cur, ms_t deltatime);
// # 126 "HTDNS.h"
extern char * HTGetHostBySock (int soc);
// # 139 "HTDNS.h"
extern int HTGetHostByName (HTHost * host, char *hostname, HTRequest * request);
// # 62 "HTNet.h" 2
// # 91 "HTNet.h"
typedef enum _HTFilterOrder {
    HT_FILTER_FIRST = 0x0,
    HT_FILTER_EARLY = 0x3FFF,
    HT_FILTER_MIDDLE = 0x7FFF,
    HT_FILTER_LATE = 0xBFFE,
    HT_FILTER_LAST = 0xFFFF
} HTFilterOrder;
// # 127 "HTNet.h"
typedef int HTNetBefore (HTRequest * request, void * param, int mode);
// # 136 "HTNet.h"
extern char HTNetCall_addBefore (HTList * list, HTNetBefore * before,
     const char * tmplate, void * param,
                                 HTFilterOrder order);







extern char HTNetCall_deleteBefore (HTList * list, HTNetBefore * before);






extern char HTNetCall_deleteBeforeAll (HTList * list);
// # 163 "HTNet.h"
extern int HTNetCall_executeBefore (HTList * list, HTRequest * request);
// # 222 "HTNet.h"
typedef int HTNetAfter (HTRequest * request, HTResponse * response,
                        void * param, int status);
// # 232 "HTNet.h"
extern char HTNetCall_addAfter (HTList * list, HTNetAfter * after,
    const char * tmplate, void * param,
    int status, HTFilterOrder order);







extern char HTNetCall_deleteAfter (HTList * list, HTNetAfter * after);
extern char HTNetCall_deleteAfterStatus (HTList * list, int status);






extern char HTNetCall_deleteAfterAll (HTList * list);
// # 261 "HTNet.h"
extern int HTNetCall_executeAfter (HTList * list, HTRequest * request,
       int status);
// # 279 "HTNet.h"
extern char HTNet_setBefore (HTList * list);

extern HTList * HTNet_before (void);

extern char HTNet_addBefore (HTNetBefore * before, const char * tmplate,
        void * param, HTFilterOrder order);

extern char HTNet_deleteBefore (HTNetBefore * before);






extern int HTNet_executeBeforeAll (HTRequest * request);
// # 303 "HTNet.h"
extern char HTNet_setAfter (HTList * list);

extern HTList * HTNet_after (void);

extern char HTNet_addAfter (HTNetAfter * after, const char * tmplate,
       void * param, int status,
                            HTFilterOrder order);

extern char HTNet_deleteAfter (HTNetAfter * after);

extern char HTNet_deleteAfterStatus (int status);






extern int HTNet_executeAfterAll (HTRequest * request, int status);
// # 343 "HTNet.h"
extern char HTNet_setMaxSocket (int newmax);
extern int HTNet_maxSocket (void);







extern void HTNet_increaseSocket (void);
extern void HTNet_decreaseSocket (void);

extern int HTNet_availableSockets (void);







extern void HTNet_increasePersistentSocket (void);
extern void HTNet_decreasePersistentSocket (void);

extern int HTNet_availablePersistentSockets (void);
// # 377 "HTNet.h"
extern char HTNet_isIdle (void);
// # 388 "HTNet.h"
extern HTList *HTNet_activeQueue (void);
extern char HTNet_idle (void);
// # 411 "HTNet.h"
extern char HTNet_idle (void);
// # 421 "HTNet.h"
extern char HTNet_isEmpty (void);
extern int HTNet_count (void);
// # 433 "HTNet.h"
extern HTList *HTNet_pendingQueue (void);
// # 455 "HTNet.h"
extern char HTNet_newClient (HTRequest * request);
// # 464 "HTNet.h"
extern char HTNet_newServer (HTRequest * request);






extern HTNet * HTNet_new (HTHost * host);
// # 482 "HTNet.h"
extern HTNet * HTNet_dup (HTNet * src);
extern char HTNet_deleteDup (HTNet * dup);
// # 492 "HTNet.h"
extern char HTNet_start (HTNet * net);
// # 503 "HTNet.h"
extern char HTNet_execute (HTNet * net, HTEventType type);

extern HTEvent * HTNet_event (HTNet * net);
extern char HTNet_setEventParam (HTNet * net, void * eventParam);
extern void * HTNet_eventParam (HTNet * net);
extern char HTNet_setEventCallback(HTNet * net, HTEventCallback * cbf);
extern HTEventCallback * HTNet_eventCallback(HTNet * net);
// # 524 "HTNet.h"
extern char HTNet_delete (HTNet * me, int status);
// # 535 "HTNet.h"
extern char HTNet_deleteAll (void);
// # 549 "HTNet.h"
extern char HTNet_wait (HTNet *net);
// # 565 "HTNet.h"
extern HTPriority HTNet_priority (HTNet * net);
extern char HTNet_setPriority (HTNet * net, HTPriority priority);
// # 578 "HTNet.h"
extern char HTNet_persistent (HTNet * net);







extern char HTNet_setPersistent (HTNet * net,
                                 char persistent,
                                 HTTransportMode mode);
// # 603 "HTNet.h"
extern char HTNet_killPipe (HTNet * net);
// # 617 "HTNet.h"
extern char HTNet_kill (HTNet * me);
// # 630 "HTNet.h"
extern char HTNet_killAll (void);
// # 648 "HTNet.h"
extern HTOutputStream * HTNet_getOutput (HTNet * me, void * param, int mode);
// # 661 "HTNet.h"
extern char HTNet_setContext (HTNet * net, void * context);
extern void * HTNet_context (HTNet * net);







extern char HTNet_setSocket (HTNet * net, int sockfd);
extern int HTNet_socket (HTNet * net);
// # 686 "HTNet.h"
extern char HTNet_preemptive (HTNet * net);
// # 697 "HTNet.h"
extern char HTNet_setRequest (HTNet * net, HTRequest * request);
extern HTRequest * HTNet_request (HTNet * net);







extern char HTNet_setProtocol (HTNet * net, HTProtocol * protocol);
extern HTProtocol * HTNet_protocol (HTNet * net);
// # 718 "HTNet.h"
extern char HTNet_setTransport (HTNet * net, HTTransport * tp);
extern HTTransport * HTNet_transport (HTNet * net);







extern char HTNet_setChannel (HTNet * net, HTChannel * channel);
extern HTChannel * HTNet_channel (HTNet * net);







extern char HTNet_setHost (HTNet * net, HTHost * host);
extern HTHost * HTNet_host (HTNet * net);
// # 748 "HTNet.h"
extern char HTNet_setDns (HTNet * net, HTdns * dns);
extern HTdns * HTNet_dns (HTNet * net);







extern HTStream * HTNet_readStream(HTNet * net);
extern char HTNet_setReadStream (HTNet * net, HTStream * stream);
// # 770 "HTNet.h"
extern char HTNet_setRawBytesCount (HTNet * net, char mode);
extern char HTNet_rawBytesCount (HTNet * net);
// # 76 "HTReq.h" 2
// # 98 "HTReq.h"
extern HTRequest * HTRequest_new (void);
// # 110 "HTReq.h"
extern char HTRequest_clear (HTRequest * me);
// # 121 "HTReq.h"
extern HTRequest * HTRequest_dup (HTRequest * src);
// # 134 "HTReq.h"
extern HTRequest * HTRequest_dupInternal (HTRequest * src);

extern char HTRequest_setInternal (HTRequest * request, char mode);
extern char HTRequest_internal (HTRequest * request);
// # 147 "HTReq.h"
extern void HTRequest_delete (HTRequest * request);
// # 164 "HTReq.h"
extern char HTLoad (HTRequest * request, char recursive);
extern char HTServe(HTRequest * request, char recursive);
// # 180 "HTReq.h"
extern char HTRequest_kill(HTRequest * request);
// # 206 "HTReq.h"
extern void HTRequest_setAnchor (HTRequest *request, HTAnchor *anchor);
extern HTParentAnchor * HTRequest_anchor (HTRequest *request);

extern HTChildAnchor * HTRequest_childAnchor (HTRequest * request);
// # 222 "HTReq.h"
extern char HTRequest_setUserProfile (HTRequest * request, HTUserProfile * up);
extern HTUserProfile * HTRequest_userProfile (HTRequest * request);
// # 236 "HTReq.h"
extern HTNet * HTRequest_net (HTRequest * request);
extern char HTRequest_setNet (HTRequest * request, HTNet * net);
// # 253 "HTReq.h"
extern HTResponse * HTRequest_response (HTRequest * request);
extern char HTRequest_setResponse (HTRequest * request, HTResponse * response);
// # 274 "HTReq.h"
extern void HTRequest_setMethod (HTRequest *request, HTMethod method);
extern HTMethod HTRequest_method (HTRequest *request);
// # 289 "HTReq.h"
extern HTPriority HTRequest_priority (HTRequest * request);
extern char HTRequest_setPriority (HTRequest * request, HTPriority priority);
// # 305 "HTReq.h"
extern char HTRequest_setFlush (HTRequest * me, char mode);
extern char HTRequest_flush (HTRequest * me);
// # 319 "HTReq.h"
extern int HTRequest_forceFlush (HTRequest * request);
// # 333 "HTReq.h"
extern HTList * HTRequest_error (HTRequest * request);
extern void HTRequest_setError (HTRequest * request, HTList * list);
extern void HTRequest_deleteAllErrors (HTRequest * request);







extern char HTRequest_addError (HTRequest * request,
    HTSeverity severity,
    char ignore,
    int element,
    void * par,
    unsigned int length,
    char * where);

extern char HTRequest_addSystemError (HTRequest * request,
          HTSeverity severity,
          int errornumber,
          char ignore,
          char * syscall);
// # 377 "HTReq.h"
extern char HTRequest_setMaxRetry (int newmax);
extern int HTRequest_maxRetry (void);

extern int HTRequest_retrys (HTRequest * request);
extern char HTRequest_doRetry (HTRequest *request);
extern char HTRequest_addRetry (HTRequest * request);

extern int HTRequest_AAretrys (HTRequest * request);
extern char HTRequest_addAARetry (HTRequest * request);
// # 399 "HTReq.h"
extern char HTRequest_setMaxForwards (HTRequest * request, int maxforwards);
extern int HTRequest_maxForwards (HTRequest * request);
// # 415 "HTReq.h"
extern void HTRequest_setPreemptive (HTRequest *request, char mode);
extern char HTRequest_preemptive (HTRequest *request);
// # 430 "HTReq.h"
extern void HTRequest_setNegotiation (HTRequest *request, char mode);
extern char HTRequest_negotiation (HTRequest *request);
// # 445 "HTReq.h"
typedef enum _HTPreconditions {
    HT_NO_MATCH = 0,
    HT_MATCH_THIS,
    HT_MATCH_ANY,
    HT_DONT_MATCH_THIS,
    HT_DONT_MATCH_ANY
} HTPreconditions;

extern void HTRequest_setPreconditions (HTRequest * me, HTPreconditions mode);
extern HTPreconditions HTRequest_preconditions (HTRequest * me);
// # 465 "HTReq.h"
typedef struct _HTMIMEParseSet HTMIMEParseSet;
extern void HTRequest_setMIMEParseSet (HTRequest *request,
           HTMIMEParseSet * parseSet, char local);
extern HTMIMEParseSet * HTRequest_MIMEParseSet (HTRequest *request,
           char * pLocal);
// # 491 "HTReq.h"
typedef enum _HTGnHd {
    HT_G_CC = 0x1,
    HT_G_CONNECTION = 0x2,
    HT_G_DATE = 0x4,
    HT_G_PRAGMA_NO_CACHE= 0x8,
    HT_G_FORWARDED = 0x10,
    HT_G_MESSAGE_ID = 0x20,
    HT_G_MIME = 0x40,
    HT_G_TRAILER = 0x80,
    HT_G_TRANSFER = 0x100,
    HT_G_EXTRA_HEADERS = 0x200
} HTGnHd;





extern void HTRequest_setGnHd (HTRequest *request, HTGnHd gnhd);
extern void HTRequest_addGnHd (HTRequest *request, HTGnHd gnhd);
extern HTGnHd HTRequest_gnHd (HTRequest *request);
// # 523 "HTReq.h"
typedef enum _HTRqHd {
    HT_C_ACCEPT_TYPE = 0x1,
    HT_C_ACCEPT_CHAR = 0x2,
    HT_C_ACCEPT_ENC = 0x4,
    HT_C_ACCEPT_TE = 0x8,
    HT_C_ACCEPT_LAN = 0x10,
    HT_C_AUTH = 0x20,
    HT_C_EXPECT = 0x40,
    HT_C_FROM = 0x80,
    HT_C_HOST = 0x100,
    HT_C_IMS = 0x200,
    HT_C_IF_MATCH = 0x400,
    HT_C_IF_MATCH_ANY = 0x800,
    HT_C_IF_NONE_MATCH = 0x1000,
    HT_C_IF_NONE_MATCH_ANY=0x2000,
    HT_C_IF_RANGE = 0x4000,
    HT_C_IF_UNMOD_SINCE = 0x8000,
    HT_C_MAX_FORWARDS = 0x10000,
    HT_C_RANGE = 0x20000,
    HT_C_REFERER = 0x40000,
    HT_C_USER_AGENT = 0x80000
} HTRqHd;






extern void HTRequest_setRqHd (HTRequest *request, HTRqHd rqhd);
extern void HTRequest_addRqHd (HTRequest *request, HTRqHd rqhd);
extern HTRqHd HTRequest_rqHd (HTRequest *request);
// # 565 "HTReq.h"
typedef enum _HTRsHd {
    HT_S_AGE = 0x1,
    HT_S_LOCATION = 0x2,
    HT_S_PROXY_AUTH = 0x4,
    HT_S_PUBLIC = 0x8,
    HT_S_RETRY_AFTER = 0x10,
    HT_S_SERVER = 0x20,
    HT_S_VARY = 0x40,
    HT_S_WARNING = 0x80,
    HT_S_WWW_AUTH = 0x100,
    HT_S_TRAILER = 0x200
} HTRsHd;



extern void HTRequest_setRsHd (HTRequest * request, HTRsHd rshd);
extern void HTRequest_addRsHd (HTRequest * request, HTRsHd rshd);
extern HTRsHd HTRequest_rsHd (HTRequest * request);
// # 596 "HTReq.h"
typedef enum _HTEnHd {
    HT_E_ALLOW = 0x1,
    HT_E_CONTENT_BASE = 0x2,
    HT_E_CONTENT_ENCODING = 0x4,
    HT_E_CONTENT_LANGUAGE = 0x8,
    HT_E_CONTENT_LENGTH = 0x10,
    HT_E_CONTENT_LOCATION = 0x20,
    HT_E_CONTENT_MD5 = 0x40,
    HT_E_CONTENT_RANGE = 0x80,
    HT_E_CTE = 0x100,
    HT_E_CONTENT_TYPE = 0x200,
    HT_E_DERIVED_FROM = 0x400,
    HT_E_ETAG = 0x800,
    HT_E_EXPIRES = 0x1000,
    HT_E_LAST_MODIFIED = 0x2000,
    HT_E_LINK = 0x4000,
    HT_E_TITLE = 0x8000,
    HT_E_URI = 0x10000,
    HT_E_VERSION = 0x20000
} HTEnHd;



extern void HTRequest_setEnHd (HTRequest *request, HTEnHd enhd);
extern void HTRequest_addEnHd (HTRequest *request, HTEnHd enhd);
extern HTEnHd HTRequest_enHd (HTRequest *request);
// # 650 "HTReq.h"
extern char HTRequest_addExtraHeader (HTRequest * request,
                                            char * token, char * value);
extern HTAssocList * HTRequest_extraHeader (HTRequest * request);
extern char HTRequest_deleteExtraHeaderAll (HTRequest * request);
// # 667 "HTReq.h"
extern void HTRequest_setGenerator (HTRequest *request, HTList *gens,
                                    char override);
extern HTList * HTRequest_generator (HTRequest *request, char *override);
// # 711 "HTReq.h"
extern void HTRequest_setConversion (HTRequest *request, HTList *type, char override);
extern HTList * HTRequest_conversion (HTRequest *request);
// # 722 "HTReq.h"
extern void HTRequest_setEncoding (HTRequest *request, HTList *enc, char override);
extern HTList * HTRequest_encoding (HTRequest *request);
// # 733 "HTReq.h"
extern void HTRequest_setTransfer (HTRequest *request, HTList *te, char override);
extern HTList * HTRequest_transfer (HTRequest *request);
// # 745 "HTReq.h"
extern void HTRequest_setLanguage (HTRequest *request, HTList *lang, char override);
extern HTList * HTRequest_language (HTRequest *request);
// # 756 "HTReq.h"
extern void HTRequest_setCharset (HTRequest *request, HTList *charset, char override);
extern HTList * HTRequest_charset (HTRequest *request);
// # 774 "HTReq.h"
typedef enum _HTReload {
    HT_CACHE_OK = 0x0,
    HT_CACHE_FLUSH_MEM = 0x1,
    HT_CACHE_VALIDATE = 0x2,
    HT_CACHE_END_VALIDATE = 0x4,
    HT_CACHE_RANGE_VALIDATE = 0x8,
    HT_CACHE_FLUSH = 0x10,
    HT_CACHE_ERROR = 0x20
} HTReload;

extern void HTRequest_setReloadMode (HTRequest *request, HTReload mode);
extern HTReload HTRequest_reloadMode (HTRequest *request);
// # 800 "HTReq.h"
extern char * HTRequest_defaultPutName (HTRequest * me);
extern char HTRequest_setDefaultPutName (HTRequest * me, char * name);
extern char HTRequest_deleteDefaultPutName (HTRequest * me);
// # 816 "HTReq.h"
extern char HTRequest_addCacheControl (HTRequest * request,
                                              char * token, char *value);
extern char HTRequest_deleteCacheControlAll (HTRequest * request);
extern HTAssocList * HTRequest_cacheControl (HTRequest * request);
// # 832 "HTReq.h"
extern time_t HTRequest_date (HTRequest * request);
extern char HTRequest_setDate (HTRequest * request, time_t date);
// # 846 "HTReq.h"
extern char HTRequest_addExpect (HTRequest * me,
     char * token, char * value);
extern char HTRequest_deleteExpect (HTRequest * me);
extern HTAssocList * HTRequest_expect (HTRequest * me);
// # 866 "HTReq.h"
extern char HTRequest_addRange (HTRequest * request,
                                      char * unit, char * range);
extern char HTRequest_deleteRangeAll (HTRequest * request);
extern HTAssocList * HTRequest_range (HTRequest * request);
// # 882 "HTReq.h"
extern char HTRequest_addConnection (HTRequest * request,
                                            char * token, char * value);
extern char HTRequest_deleteConnection (HTRequest * request);
extern HTAssocList * HTRequest_connection (HTRequest * request);
// # 900 "HTReq.h"
extern char HTRequest_addCredentials (HTRequest * request,
                                            char * token, char * value);
extern char HTRequest_deleteCredentialsAll (HTRequest * request);
extern HTAssocList * HTRequest_credentials (HTRequest * request);
// # 913 "HTReq.h"
extern char HTRequest_setRealm (HTRequest * request, char * realm);
extern const char * HTRequest_realm (HTRequest * request);
extern char HTRequest_deleteRealm (HTRequest * me);
// # 927 "HTReq.h"
extern void HTRequest_setParent (HTRequest *request, HTParentAnchor *parent);
extern HTParentAnchor * HTRequest_parent (HTRequest *request);
// # 957 "HTReq.h"
extern char HTRequest_addBefore (HTRequest * request, HTNetBefore * filter,
     const char * tmplate, void * param,
                                 HTFilterOrder order, char override);
extern HTList * HTRequest_before (HTRequest * request, char * override);
// # 970 "HTReq.h"
extern char HTRequest_deleteBefore (HTRequest * request, HTNetBefore * filter);
extern char HTRequest_deleteBeforeAll (HTRequest * request);
// # 992 "HTReq.h"
extern char HTRequest_addAfter (HTRequest * request, HTNetAfter * filter,
    const char * tmplate, void * param,
                                int status, HTFilterOrder order,
                                char override);
extern HTList * HTRequest_after (HTRequest * request, char * override);
// # 1007 "HTReq.h"
extern char HTRequest_deleteAfter (HTRequest * request, HTNetAfter * filter);
extern char HTRequest_deleteAfterStatus (HTRequest * request, int status);
extern char HTRequest_deleteAfterAll (HTRequest * request);
// # 1041 "HTReq.h"
typedef int HTPostCallback (HTRequest * request, HTStream * target);

extern void HTRequest_setPostCallback (HTRequest * request, HTPostCallback * cbf);
extern HTPostCallback * HTRequest_postCallback (HTRequest * request);
// # 1054 "HTReq.h"
extern char HTRequest_setEntityAnchor (HTRequest * request, HTParentAnchor * anchor);
extern HTParentAnchor * HTRequest_entityAnchor (HTRequest * request);
// # 1067 "HTReq.h"
extern void HTRequest_setInputStream (HTRequest * request, HTStream * input);
extern HTStream *HTRequest_inputStream (HTRequest * request);
// # 1078 "HTReq.h"
extern char HTRequest_isPostWeb (HTRequest * request);
// # 1089 "HTReq.h"
extern char HTRequest_setSource (HTRequest * request, HTRequest * source);
extern HTRequest * HTRequest_source (HTRequest * request);
// # 1105 "HTReq.h"
extern void HTRequest_setOutputStream (HTRequest *request, HTStream *output);
extern HTStream *HTRequest_outputStream (HTRequest *request);
// # 1119 "HTReq.h"
extern void HTRequest_setOutputFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_outputFormat (HTRequest *request);
// # 1130 "HTReq.h"
extern void HTRequest_setOutputConnected (HTRequest * request, char mode);
extern char HTRequest_outputConnected (HTRequest * request);
// # 1145 "HTReq.h"
extern void HTRequest_setDebugStream (HTRequest *request, HTStream *debug);
extern HTStream *HTRequest_debugStream (HTRequest *request);
// # 1158 "HTReq.h"
extern void HTRequest_setDebugFormat (HTRequest *request, HTFormat format);
extern HTFormat HTRequest_debugFormat (HTRequest *request);
// # 1172 "HTReq.h"
typedef int HTRequestCallback (HTRequest * request, void *param);

extern void HTRequest_setCallback (HTRequest *request, HTRequestCallback *cb);
extern HTRequestCallback *HTRequest_callback (HTRequest *request);
// # 1184 "HTReq.h"
extern void HTRequest_setContext (HTRequest *request, void *context);
extern void *HTRequest_context (HTRequest *request);
// # 1197 "HTReq.h"
extern void HTRequest_setFullURI (HTRequest *request, char mode);
extern char HTRequest_fullURI (HTRequest *request);
// # 1213 "HTReq.h"
extern char HTRequest_setProxy (HTRequest * request, const char * proxy);
extern char * HTRequest_proxy (HTRequest * request);
extern char HTRequest_deleteProxy (HTRequest * request);
// # 1248 "HTReq.h"
extern char HTRequest_setMessageBody (HTRequest * request, const char * body);
extern char HTRequest_deleteMessageBody (HTRequest * request);
extern char * HTRequest_messageBody (HTRequest * request);

extern char HTRequest_setMessageBodyLength (HTRequest * request, long int length);
extern long int HTRequest_messageBodyLength (HTRequest * request);

extern char HTRequest_setMessageBodyFormat (HTRequest * request, HTFormat format);
extern HTFormat HTRequest_messageBodyFormat (HTRequest * request);
// # 1270 "HTReq.h"
extern long HTRequest_bodyRead (HTRequest * request);
extern long HTRequest_bodyWritten (HTRequest * request);







extern long HTRequest_bytesRead (HTRequest * request);
extern long HTRequest_bytesWritten (HTRequest * request);
// # 74 "WWWCore.h" 2
// # 120 "WWWCore.h"
// # 1 "HTParse.h" 1
// # 38 "HTParse.h"
// # 1 "HTEscape.h" 1
// # 46 "HTEscape.h"
typedef enum _HTURIEncoding {
    URL_XALPHAS = 0x1,
    URL_XPALPHAS = 0x2,
    URL_PATH = 0x4,
    URL_DOSFILE = 0x8
} HTURIEncoding;

extern char * HTEscape (const char * str, HTURIEncoding mask);
// # 63 "HTEscape.h"
extern char HTAsciiHexToChar (char c);
// # 77 "HTEscape.h"
extern char * HTUnEscape (char * str);
// # 39 "HTParse.h" 2
// # 80 "HTParse.h"
extern char * HTParse (const char * aName, const char * relatedName,
   int wanted);
// # 105 "HTParse.h"
extern char * HTRelative (const char * aName, const char *relatedName);
// # 117 "HTParse.h"
extern char HTURL_isAbsolute (const char * url);
// # 166 "HTParse.h"
extern char *HTSimplify (char **filename);
// # 192 "HTParse.h"
extern char HTCleanTelnetString (char * str);
// # 121 "WWWCore.h" 2
// # 148 "WWWCore.h"
// # 1 "HTUTree.h" 1
// # 73 "HTUTree.h"
typedef struct _HTUTree HTUTree;
typedef struct _HTURealm HTURealm;
typedef struct _HTUTemplate HTUTemplate;
// # 86 "HTUTree.h"
typedef int HTUTree_gc (void * context);

extern HTUTree * HTUTree_new (const char * root,
         const char * host,
         int port,
         HTUTree_gc * gc);
// # 101 "HTUTree.h"
extern char HTUTree_delete (const char * root,
       const char * host,
       int port);
// # 113 "HTUTree.h"
extern char HTUTree_deleteAll (void);
// # 123 "HTUTree.h"
extern HTUTree * HTUTree_find (const char * root,
          const char * host,
          int port);
// # 135 "HTUTree.h"
extern void * HTUTree_findNode (HTUTree * tree,
                                const char * realm, const char * path);





extern char HTUTree_addNode (HTUTree * tree,
                             const char * realm, const char * path,
                             void * context);






extern char HTUTree_replaceNode (HTUTree * tree,
                                 const char * realm, const char * path,
                                 void * context);





extern char HTUTree_deleteNode (HTUTree * tree,
                                const char * realm, const char * path);
// # 149 "WWWCore.h" 2
// # 161 "WWWCore.h"
// # 1 "HTWWWStr.h" 1
// # 55 "HTWWWStr.h"
extern char * HTNextField (char** pstr);
// # 67 "HTWWWStr.h"
extern char * HTNextPair (char ** pstr);
// # 79 "HTWWWStr.h"
extern char * HTNextParam (char ** pstr);
// # 90 "HTWWWStr.h"
extern char * HTNextLWSToken (char ** pstr);
// # 101 "HTWWWStr.h"
extern char * HTNextSegment (char ** pstr);
// # 112 "HTWWWStr.h"
extern char * HTNextElement (char ** pstr);
// # 124 "HTWWWStr.h"
extern char * HTNextSExp (char ** exp, char ** param);
// # 137 "HTWWWStr.h"
typedef enum _HTEOLState {
    EOL_ERR = -1,
    EOL_BEGIN = 0,
    EOL_FCR,
    EOL_FLF,
    EOL_DOT,
    EOL_SCR,
    EOL_SLF,

    EOL_END,
    EOL_FOLD,
    EOL_LINE
} HTEOLState;
// # 160 "HTWWWStr.h"
extern const char * HTDateTimeStr (time_t *calendar, char local);
// # 171 "HTWWWStr.h"
extern char HTDateDirStr (time_t * time, char * str, int len);
// # 187 "HTWWWStr.h"
extern time_t HTParseTime (const char * str, HTUserProfile * up, char expand);
// # 198 "HTWWWStr.h"
extern const char * HTMessageIdStr (HTUserProfile * up);
// # 211 "HTWWWStr.h"
extern char HTMIMEMatch (HTAtom * tmplate, HTAtom * actual);
// # 224 "HTWWWStr.h"
extern void HTNumToStr (unsigned long n, char *str, int len);
// # 245 "HTWWWStr.h"
extern char * HTWWWToLocal (const char * url, const char * base,
       HTUserProfile * up);
// # 259 "HTWWWStr.h"
extern char * HTLocalToWWW (const char * local, const char * access);
// # 162 "WWWCore.h" 2
// # 196 "WWWCore.h"
// # 1 "HTMemLog.h" 1
// # 39 "HTMemLog.h"
extern int HTMemLog_open (char * logName, size_t size, char keepOpen);
extern int HTMemLog_add (char * buf, size_t len);
extern int HTMemLog_flush (void);
extern void HTMemLog_close (void);
extern HTTraceDataCallback HTMemLog_callback;
// # 197 "WWWCore.h" 2
// # 220 "WWWCore.h"
// # 1 "HTAlert.h" 1
// # 67 "HTAlert.h"
typedef enum _HTAlertOpcode {
    HT_PROG_DNS = 0x1,
    HT_PROG_CONNECT = 0x2,
    HT_PROG_ACCEPT = 0x4,
    HT_PROG_READ = 0x8,
    HT_PROG_WRITE = 0x10,
    HT_PROG_DONE = 0x20,
    HT_PROG_INTERRUPT = 0x40,
    HT_PROG_OTHER = 0x80,
    HT_PROG_TIMEOUT = 0x100,
    HT_PROG_LOGIN = 0x200,
    HT_A_PROGRESS = 0xFFFF,



    HT_A_MESSAGE = 0x1<<16,
    HT_A_CONFIRM = 0x2<<16,
    HT_A_PROMPT = 0x4<<16,
    HT_A_SECRET = 0x8<<16,
    HT_A_USER_PW = 0x10<<16
} HTAlertOpcode;

typedef struct _HTAlertPar HTAlertPar;

typedef char HTAlertCallback (HTRequest * request, HTAlertOpcode op,
    int msgnum, const char * dfault, void * input,
    HTAlertPar * reply);
// # 109 "HTAlert.h"
typedef enum _HTAlertMsg {
    HT_MSG_NULL = -1,
    HT_MSG_UID = 0,
    HT_MSG_PROXY_UID,
    HT_MSG_FTP_UID,
    HT_MSG_PW,
    HT_MSG_FILENAME,
    HT_MSG_ACCOUNT,
    HT_MSG_METHOD,
    HT_MSG_MOVED,
    HT_MSG_RULES,
    HT_MSG_FILE_REPLACE,
    HT_MSG_RETRY_AUTHENTICATION,
    HT_MSG_RETRY_PROXY_AUTH,
    HT_MSG_REDO,
    HT_MSG_BIG_PUT,
    HT_MSG_SOURCE_MOVED,
    HT_MSG_DESTINATION_MOVED,
    HT_MSG_REDIRECTION,
    HT_MSG_PROXY,
    HT_MSG_CACHE_LOCK,
    HT_MSG_ACCEPT_COOKIE,
    HT_MSG_ELEMENTS
} HTAlertMsg;
// # 143 "HTAlert.h"
extern void HTAlert_setInteractive (char interative);
extern char HTAlert_interactive (void);
// # 167 "HTAlert.h"
extern char HTAlertCall_add (HTList * list, HTAlertCallback * cbf,
        HTAlertOpcode opcode);
// # 178 "HTAlert.h"
extern char HTAlertCall_delete (HTList * list, HTAlertCallback * cbf);
// # 188 "HTAlert.h"
extern char HTAlertCall_deleteOpcode (HTList * list, HTAlertOpcode opcode);
// # 198 "HTAlert.h"
extern char HTAlertCall_deleteAll (HTList * list);
// # 209 "HTAlert.h"
extern HTAlertCallback * HTAlertCall_find(HTList * list, HTAlertOpcode opcode);
// # 222 "HTAlert.h"
extern HTAlertPar * HTAlert_newReply (void);
extern void HTAlert_deleteReply (HTAlertPar * old);
// # 236 "HTAlert.h"
extern char HTAlert_setReplyMessage (HTAlertPar * me, const char *message);
extern char HTAlert_assignReplyMessage (HTAlertPar * me, char * message);






extern char * HTAlert_replyMessage (HTAlertPar * me);




extern char * HTAlert_replySecret (HTAlertPar * me);
extern char HTAlert_setReplySecret (HTAlertPar * me, const char * secret);

extern void * HTAlert_replyOutput (HTAlertPar * me);
extern char HTAlert_setReplyOutput (HTAlertPar * me, void * output);
// # 265 "HTAlert.h"
extern void HTAlert_setGlobal (HTList * list);
extern HTList * HTAlert_global (void);
// # 281 "HTAlert.h"
extern char HTAlert_add (HTAlertCallback * cbf, HTAlertOpcode opcode);
// # 292 "HTAlert.h"
extern char HTAlert_delete (HTAlertCallback * cbf);
extern char HTAlert_deleteOpcode (HTAlertOpcode opcode);







extern char HTAlert_deleteAll (void);







extern HTAlertCallback * HTAlert_find (HTAlertOpcode opcode);
// # 221 "WWWCore.h" 2
// # 264 "WWWCore.h"
// # 1 "HTStruct.h" 1
// # 44 "HTStruct.h"
typedef struct _HTStructured HTStructured;

typedef struct _HTStructuredClass {

    char * name;

    int (*flush) (HTStructured * me);

    int (*_free) (HTStructured * me);

    int (*abort) (HTStructured * me, HTList * errorlist);

    int (*put_character)(HTStructured * me, char ch);

    int (*put_string) (HTStructured * me, const char * str);

    int (*put_block) (HTStructured * me, const char * str, int len);
// # 70 "HTStruct.h"
    void (*start_element)(HTStructured *me,
     int element_number,
     const char * attribute_present,
     const char ** attribute_value);

    void (*end_element) (HTStructured * me, int element_number);

    void (*put_entity) (HTStructured * me, int entity_number);

    int (*unparsed_begin_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_end_element)(HTStructured * me, const char * str, int len);

    int (*unparsed_entity)(HTStructured * me, const char * str, int len);

} HTStructuredClass;
// # 265 "WWWCore.h" 2
// # 277 "WWWCore.h"
// # 1 "HTNoFree.h" 1
// # 35 "HTNoFree.h"
extern HTStream * HTNoFreeStream_new (HTStream * target);

extern int HTNoFreeStream_delete (HTStream * me);
// # 278 "WWWCore.h" 2
// # 339 "WWWCore.h"
// # 1 "HTInet.h" 1
// # 30 "HTInet.h"
// # 1 "HTHstMan.h" 1
// # 41 "HTHstMan.h"
typedef enum _TCPState {
    TCP_DNS_ERROR = -3,
    TCP_ERROR = -2,
    TCP_CONNECTED = -1,
    TCP_BEGIN = 0,
    TCP_CHANNEL,
    TCP_DNS,
    TCP_NEED_SOCKET,
    TCP_NEED_BIND,
    TCP_NEED_LISTEN,
    TCP_NEED_CONNECT,
    TCP_IN_USE
} TCPState;

struct _HTHost {
    int hash;


    char * hostname;
    u_short u_port;
    time_t ntime;
    char * type;
    int version;
    HTMethod methods;
    char * server;
    char * user_agent;
    char * range_units;


    time_t expires;
    int reqsPerConnection;
    int reqsMade;


    HTList * pipeline;
    HTList * pending;
    HTNet * doit;
    HTNet * lock;
    HTNet * listening;
    char persistent;
    HTTransportMode mode;
    HTTimer * timer;
    char do_recover;
    int recovered;
    char close_notification;
    char broken_pipe;


    HTChannel * channel;


    HTdns * dns;
    TCPState tcpstate;
    SockA sock_addr;
    int retry;
    int home;
    ms_t connecttime;


    HTEvent * events[HTEvent_TYPES];
    HTEventType registeredFor;
    size_t remainingRead;


    ms_t delay;
    void * context;
    int forceWriteFlush;
    int inFlush;

};
// # 31 "HTInet.h" 2
// # 42 "HTInet.h"
extern char * HTErrnoString (int errnum);
extern int HTInetStatus (int errnum, char * where);
// # 56 "HTInet.h"
extern unsigned int HTCardinal (int * pstatus,
    char ** pp,
    unsigned int max_value);
// # 71 "HTInet.h"
extern const char * HTInetString (struct sockaddr_in * sin);
// # 83 "HTInet.h"
extern int HTParseInet (HTHost * host, char * hostname, HTRequest * request);
// # 93 "HTInet.h"
extern time_t HTGetTimeZoneOffset (void);
// # 102 "HTInet.h"
extern ms_t HTGetTimeInMillis (void);
// # 113 "HTInet.h"
extern char * HTGetHostName (void);
// # 131 "HTInet.h"
extern char * HTGetMailAddress (void);
// # 140 "HTInet.h"
extern char * HTGetNewsServer (void);
// # 152 "HTInet.h"
extern char * HTGetTmpFileName (const char * dir);
// # 340 "WWWCore.h" 2
// # 33 "HTFile.c" 2
// # 1 "WWWDir.h" 1
// # 55 "WWWDir.h"
// # 1 "HTDir.h" 1
// # 33 "HTDir.h"
// # 1 "HTIcons.h" 1
// # 34 "HTIcons.h"
// # 1 "WWWLib.h" 1
// # 35 "HTIcons.h" 2




typedef struct _HTIconNode HTIconNode;
// # 52 "HTIcons.h"
extern char HTIcon_add (const char * url, const char * prefix,
    char * alt, char * type_templ);
// # 69 "HTIcons.h"
extern char HTIcon_addUnknown (const char * url, const char * prefix,
    char * alt);
// # 80 "HTIcons.h"
extern char HTIcon_addBlank (const char * url, const char * prefix,
    char * alt);
// # 91 "HTIcons.h"
extern char HTIcon_addParent (const char * url, const char * prefix,
    char * alt);
// # 101 "HTIcons.h"
extern char HTIcon_addDir (const char * url, const char * prefix,
    char * alt);
// # 113 "HTIcons.h"
typedef enum _HTFileMode {
    HT_IS_FILE,
    HT_IS_DIR,
    HT_IS_BLANK,
    HT_IS_PARENT
} HTFileMode;


extern HTIconNode * HTIcon_find (HTFileMode mode,
     HTFormat content_type,
     HTEncoding content_encoding);
// # 134 "HTIcons.h"
extern char * HTIcon_url (HTIconNode * node);
// # 145 "HTIcons.h"
extern char * HTIcon_alternative (HTIconNode * node, char brackets);
// # 155 "HTIcons.h"
extern void HTIcon_deleteAll (void);
// # 34 "HTDir.h" 2
// # 46 "HTDir.h"
typedef enum _HTDirShow {
    HT_DS_SIZE = 0x1,
    HT_DS_DATE = 0x2,
    HT_DS_HID = 0x4,
    HT_DS_DES = 0x8,
    HT_DS_ICON = 0x10,
    HT_DS_HOTI = 0x20
} HTDirShow;

typedef enum _HTDirKey {
    HT_DK_NONE = 0,
    HT_DK_CSEN = 1,
    HT_DK_CINS = 2
} HTDirKey;
// # 72 "HTDir.h"
extern char HTDir_setWidth (int minfile, int maxfile);
// # 91 "HTDir.h"
extern char HTDir_setWidth (int minfile, int maxfile);
// # 102 "HTDir.h"
typedef struct _HTDir HTDir;

extern HTDir * HTDir_new (HTRequest * request, HTDirShow show, HTDirKey key);
// # 115 "HTDir.h"
extern char HTDir_addElement (HTDir *dir, char *name, char *date,
     char *size, HTFileMode mode);
// # 127 "HTDir.h"
extern char HTDir_free (HTDir * dir);
// # 56 "WWWDir.h" 2
// # 90 "WWWDir.h"
// # 1 "HTDescpt.h" 1
// # 45 "HTDescpt.h"
extern char * HTDescriptionFile;
// # 73 "HTDescpt.h"
extern char HTPeekTitles;
// # 85 "HTDescpt.h"
extern HTList * HTReadDescriptions (char * dirname);
// # 98 "HTDescpt.h"
extern char * HTGetDescription (HTList * descriptions,
           char * dirname,
           char * filename,
           HTFormat format);
// # 121 "HTDescpt.h"
extern void HTFreeDescriptions (HTList * descriptions);
// # 91 "WWWDir.h" 2
// # 34 "HTFile.c" 2
// # 1 "WWWTrans.h" 1
// # 52 "WWWTrans.h"
// # 1 "HTANSI.h" 1
// # 56 "HTANSI.h"
extern HTInput_new HTANSIReader_new;







extern HTOutput_new HTANSIWriter_new;
// # 53 "WWWTrans.h" 2
// # 1 "HTLocal.h" 1
// # 42 "HTLocal.h"
typedef int HTLocalMode;
// # 84 "HTLocal.h"
extern int HTFileOpen (HTNet * net, char * local, HTLocalMode mode);
// # 96 "HTLocal.h"
extern int HTFileClose (HTNet * net);
// # 54 "WWWTrans.h" 2







// # 1 "HTTCP.h" 1
// # 47 "HTTCP.h"
extern int HTDoConnect (HTNet * net);
// # 67 "HTTCP.h"
extern int HTDoAccept (HTNet * listen, HTNet * accept);
// # 81 "HTTCP.h"
extern int HTDoListen (HTNet * net, HTNet * accept, int backlog);
// # 91 "HTTCP.h"
extern int HTDoClose (HTNet * net);
// # 62 "WWWTrans.h" 2
// # 1 "HTSocket.h" 1
// # 37 "HTSocket.h"
extern HTProtCallback HTLoadSocket;
// # 63 "WWWTrans.h" 2
// # 1 "HTReader.h" 1
// # 60 "HTReader.h"
extern HTInput_new HTReader_new;
// # 64 "WWWTrans.h" 2
// # 1 "HTWriter.h" 1
// # 43 "HTWriter.h"
extern HTOutput_new HTWriter_new;

extern char HTWriter_set (HTOutputStream * me,
     HTNet * net,
     HTChannel * ch,
     void * param,
     int mode);
// # 65 "WWWTrans.h" 2
// # 1 "HTBufWrt.h" 1
// # 66 "HTBufWrt.h"
extern HTOutput_new HTBufferWriter_new;







extern HTOutputConverter_new HTBufferConverter_new;
// # 66 "WWWTrans.h" 2
// # 35 "HTFile.c" 2
// # 1 "HTReqMan.h" 1
// # 38 "HTReqMan.h"
// # 1 "HTAABrow.h" 1
// # 44 "HTAABrow.h"
extern HTNetBefore HTBasic_generate;
extern HTNetAfter HTBasic_parse;
extern HTUTree_gc HTBasic_delete;
// # 56 "HTAABrow.h"
extern HTNetBefore HTDigest_generate;
extern HTNetAfter HTDigest_parse;
extern HTNetAfter HTDigest_updateInfo;
extern HTUTree_gc HTDigest_delete;
// # 39 "HTReqMan.h" 2


// # 1 "HTMIMPrs.h" 1
// # 32 "HTMIMPrs.h"
// # 1 "HTHeader.h" 1
// # 44 "HTHeader.h"
typedef int HTParserCallback (HTRequest * request, HTResponse * response,
                              char * token, char * value);
// # 68 "HTHeader.h"
extern char HTGenerator_add (HTList * gens, HTPostCallback * callback);
extern char HTGenerator_delete (HTList * gens, HTPostCallback * callback);
extern char HTGenerator_deleteAll (HTList * gens);
// # 87 "HTHeader.h"
extern void HTHeader_setMIMEParseSet (HTMIMEParseSet * list);
extern HTMIMEParseSet * HTHeader_MIMEParseSet (void);
extern char HTHeader_addParser (const char * token, char case_sensitive,
    HTParserCallback * callback);
extern char HTHeader_addRegexParser (const char * token, char case_sensitive,
    HTParserCallback * callback);
extern char HTHeader_deleteParser (const char * token);







extern void HTHeader_setGenerator (HTList * list);
extern char HTHeader_addGenerator (HTPostCallback * callback);
extern char HTHeader_deleteGenerator (HTPostCallback * callback);
extern HTList * HTHeader_generator (void);







extern void HTHeader_deleteAll (void);
// # 33 "HTMIMPrs.h" 2
// # 45 "HTMIMPrs.h"
typedef struct _HTMIMEParseEl HTMIMEParseEl;

struct _HTMIMEParseSet {
    int size;
    HTMIMEParseEl ** parsers;
    HTMIMEParseEl * regexParsers;
};
// # 66 "HTMIMPrs.h"
extern HTMIMEParseSet * HTMIMEParseSet_new(int hashSize);
extern int HTMIMEParseSet_deleteAll (HTMIMEParseSet * me);
// # 80 "HTMIMPrs.h"
extern HTMIMEParseEl * HTMIMEParseSet_add (HTMIMEParseSet * me,
        const char * token,
        char caseSensitive,
        HTParserCallback * callback);
extern HTMIMEParseEl * HTMIMEParseSet_addRegex (HTMIMEParseSet * me,
      const char * token,
      char caseSensitive,
      HTParserCallback * callback);
extern int HTMIMEParseSet_delete (HTMIMEParseSet * me, const char * token);
// # 98 "HTMIMPrs.h"
extern int HTMIMEParseSet_dispatch (HTMIMEParseSet * me, HTRequest * request,
        char * token, char * value, char * pFound);
// # 42 "HTReqMan.h" 2
// # 51 "HTReqMan.h"
struct _HTRequest {

    char internal;

    time_t date;

    HTMethod method;

    char flush;

    HTPriority priority;
// # 87 "HTReqMan.h"
    HTUserProfile * userprofile;
// # 98 "HTReqMan.h"
    HTNet * net;
// # 109 "HTReqMan.h"
    HTResponse * response;







    HTList * error_stack;







    int retrys;
    int max_forwards;
    int AAretrys;
// # 140 "HTReqMan.h"
    char preemptive;
// # 152 "HTReqMan.h"
    char ContentNegotiation;







    HTPreconditions preconditions;
// # 170 "HTReqMan.h"
    HTGnHd GenMask;
    HTRsHd ResponseMask;
    HTRqHd RequestMask;
    HTEnHd EntityMask;
// # 183 "HTReqMan.h"
    HTMIMEParseSet * parseSet;
    char pars_local;
// # 194 "HTReqMan.h"
    HTList * conversions;
    char conv_local;

    HTList * encodings;
    char enc_local;

    HTList * tes;
    char te_local;

    HTList * languages;
    char lang_local;

    HTList * charsets;
    char char_local;

    HTList * befores;
    char befores_local;

    HTList * afters;
    char afters_local;
// # 223 "HTReqMan.h"
    char * proxy;
    char full_uri;
// # 235 "HTReqMan.h"
    HTReload reload;
    HTAssocList * cache_control;
// # 246 "HTReqMan.h"
   char * default_put_name;
// # 257 "HTReqMan.h"
    HTAssocList * byte_ranges;
// # 268 "HTReqMan.h"
    HTAssocList * connection;
// # 281 "HTReqMan.h"
    HTAssocList * expect;
// # 293 "HTReqMan.h"
    char * realm;
    HTAssocList * credentials;
// # 311 "HTReqMan.h"
    HTAssocList * extra_headers;
// # 321 "HTReqMan.h"
    HTList * generators;
    char gens_local;
// # 333 "HTReqMan.h"
    HTAssocList * mandatory;
    HTAssocList * optional;







    HTParentAnchor * anchor;

    HTChildAnchor * childAnchor;
    HTParentAnchor * parentAnchor;







    HTStream * output_stream;
    HTStream * orig_output_stream;
    HTFormat output_format;
    char connected;

    HTStream * debug_stream;
    HTStream * orig_debug_stream;
    HTFormat debug_format;







    HTStream * input_stream;
    HTFormat input_format;







    HTPostCallback * PostCallback;







    HTRequestCallback * callback;
    void * context;







    HTRequest * source;
    HTParentAnchor * source_anchor;

    HTRequest * mainDestination;
    HTList * destinations;
    int destRequests;
    int destStreams;




};
// # 416 "HTReqMan.h"
extern char HTRequest_addDestination (HTRequest * src, HTRequest * dest);
extern char HTRequest_removeDestination (HTRequest * dest);
extern char HTRequest_destinationsReady (HTRequest * me);

extern char HTRequest_linkDestination (HTRequest * dest);
extern char HTRequest_unlinkDestination (HTRequest * dest);

extern char HTRequest_removePostWeb (HTRequest * me);
extern char HTRequest_killPostWeb (HTRequest * me);
// # 36 "HTFile.c" 2
// # 1 "HTBind.h" 1
// # 69 "HTBind.h"
extern char HTBind_init (void);
extern char HTBind_deleteAll (void);
// # 81 "HTBind.h"
extern void HTBind_caseSensitive (char sensitive);
// # 92 "HTBind.h"
extern const char *HTBind_delimiters (void);
extern void HTBind_setDelimiters (const char * new_suffixes);
// # 115 "HTBind.h"
extern char HTBind_add (const char * suffix,
     const char * representation,
     const char * encoding,
     const char * transfer,
     const char * language,
     double value);

extern char HTBind_addType (const char * suffix,
     const char * format,
     double value);

extern char HTBind_addEncoding (const char * suffix,
     const char * encoding,
     double value);

extern char HTBind_addTransfer (const char * suffix,
     const char * transfer,
     double value);

extern char HTBind_addLanguage (const char * suffix,
     const char * language,
     double value);
// # 164 "HTBind.h"
extern char * HTBind_getSuffix (HTParentAnchor * anchor);
// # 178 "HTBind.h"
extern char HTBind_getAnchorBindings (HTParentAnchor * anchor);
// # 192 "HTBind.h"
extern char HTBind_getResponseBindings (HTResponse * response,
                                         const char * url);
// # 207 "HTBind.h"
extern char HTBind_getFormat (const char * filename,
         HTFormat * format,
         HTEncoding * enc,
         HTEncoding * cte,
         HTLanguage * lang,
         double * quality);
// # 37 "HTFile.c" 2
// # 1 "HTMulti.h" 1
// # 49 "HTMulti.h"
extern void HTAddWelcome (char * welcome_name);
// # 60 "HTMulti.h"
extern char * HTMulti (HTRequest * req,
        char * path,
        struct stat * stat_info);
// # 38 "HTFile.c" 2
// # 1 "HTFile.h" 1
// # 32 "HTFile.h"
extern HTProtCallback HTLoadFile;
// # 43 "HTFile.h"
typedef enum _HTDirAccess {
    HT_DIR_FORBID,
    HT_DIR_SELECTIVE,
    HT_DIR_OK
} HTDirAccess;



extern HTDirAccess HTFile_dirAccess (void);
extern char HTFile_setDirAccess (HTDirAccess mode);
// # 63 "HTFile.h"
typedef enum _HTDirReadme {
    HT_DIR_README_NONE,
    HT_DIR_README_TOP,
    HT_DIR_README_BOTTOM
} HTDirReadme;



extern HTDirReadme HTFile_dirReadme (void);
extern char HTFile_setDirReadme (HTDirReadme mode);
// # 88 "HTFile.h"
extern char HTFile_doFileSuffixBinding (char mode);
extern char HTFile_fileSuffixBinding (void);
// # 39 "HTFile.c" 2


typedef enum _FileState {
    FS_RETRY = -4,
    FS_ERROR = -3,
    FS_NO_DATA = -2,
    FS_GOT_DATA = -1,
    FS_BEGIN = 0,
    FS_PENDING,
    FS_DO_CN,
    FS_NEED_OPEN_FILE,
    FS_NEED_BODY,
    FS_PARSE_DIR,
    FS_TRY_FTP
} FileState;


typedef struct _file_info {
    FileState state;
    char * local;
    struct stat stat_info;
    HTNet * net;
    HTTimer * timer;
} file_info;

struct _HTStream {
    const HTStreamClass * isa;
};

struct _HTInputStream {
    const HTInputStreamClass * isa;
};

static HTDirReadme dir_readme = HT_DIR_README_TOP;
static HTDirAccess dir_access = HT_DIR_OK;
static HTDirShow dir_show = HT_DS_SIZE+HT_DS_DATE+HT_DS_DES+HT_DS_ICON;
static HTDirKey dir_key = HT_DK_CINS;
static char file_suffix_binding = (char)1;






 char HTFile_setDirAccess (HTDirAccess mode)
{
    dir_access = mode;
    return (char)1;
}

 HTDirAccess HTFile_dirAccess (void)
{
    return dir_access;
}




 char HTFile_setDirReadme (HTDirReadme mode)
{
    dir_readme = mode;
    return (char)1;
}

 HTDirReadme HTFile_dirReadme (void)
{
    return dir_readme;
}





 char HTFile_doFileSuffixBinding (char mode)
{
    file_suffix_binding = mode;
    return (char)1;
}

 char HTFile_fileSuffixBinding (void)
{
    return file_suffix_binding;
}
// # 131 "HTFile.c"
static int HTFile_readDir (HTRequest * request, file_info *file)
{

    DIR * dp;
    struct stat file_info;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char *url = HTAnchor_physical(anchor);
    char fullname[4096 +1];
    char *name;
    do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Reading..... directory\n"); } while (0);;
    if (dir_access == HT_DIR_FORBID) {
 HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_FORBIDDEN,
     ((void *)0), 0, "HTFile_readDir");
 return -403;
    }


    if (*(name = (url+strlen(url)-1)) != '/') {
 char *newurl = ((void *)0);
 HTSACopy (&(newurl), url);
 HTSACat (&(newurl), "/");
 {HTMemory_free((file->local));((file->local))=((void *)0);};
 file->local = HTWWWToLocal(newurl, "", HTRequest_userProfile(request));
 {HTMemory_free((newurl));((newurl))=((void *)0);};
    }
    strcpy(fullname, file->local);
    name = fullname+strlen(fullname);


    if (dir_access == HT_DIR_SELECTIVE) {
 strcpy(name, ".www_browsable");
 if (stat(fullname, &file_info)) {
     do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Read dir.... `%s\' not found\n" , ".www_browsable"); } while (0);;
     HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_FORBIDDEN,
         ((void *)0), 0, "HTFile_readDir");
     return -403;
 }
    }

    if ((dp = opendir(file->local))) {
 struct dirent * dirbuf;
 HTDir *dir = HTDir_new(request, dir_show, dir_key);
 char datestr[20];
 char sizestr[10];
 HTFileMode mode;
// # 185 "HTFile.c"
 while ((dirbuf = readdir(dp)))

 {





     if (!strcmp(dirbuf->d_name, ".") || !strcmp(dirbuf->d_name, ".."))

  continue;


     strcpy(name, dirbuf->d_name);
     if (lstat(fullname, &file_info)) {
  do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Read dir.... lstat failed: %s\n" , fullname); } while (0);;
  continue;
     }


     if (((mode_t) file_info.st_mode & 0170000) == 0040000) {




  mode = HT_IS_DIR;
  if (dir_show & HT_DS_SIZE) strcpy(sizestr, "-");
     } else {
  mode = HT_IS_FILE;
  if (dir_show & HT_DS_SIZE)
      HTNumToStr(file_info.st_size, sizestr, 10);
     }
     if (dir_show & HT_DS_DATE)
  HTDateDirStr(&file_info.st_mtim.tv_sec, datestr, 20);


     if (HTDir_addElement(dir, name, datestr, sizestr, mode) != (char)1)
  break;
 }
 closedir(dp);
 HTDir_free(dir);
 return 200;
    } else {
 HTRequest_addSystemError(request, ERR_FATAL, (*__errno_location ()), (char)0, "opendir");
 return -1;
    }



}
// # 247 "HTFile.c"
static char HTEditable (const char * filename, struct stat * stat_info)
{

    int i;
    uid_t myUid;
    int ngroups;
    struct stat fileStatus;
    struct stat *fileptr = stat_info ? stat_info : &fileStatus;
    gid_t groups[65536];
    if (!stat_info) {
 if (stat(filename, &fileStatus))
     return (char)0;
    }
    ngroups = getgroups(65536, groups);
    myUid = geteuid();


    if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) {
        int i;
 do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (" , (unsigned int) fileptr->st_mode , (int) fileptr->st_uid , (int) fileptr->st_gid , (int) myUid , ngroups); } while (0);;




 for (i=0; i<ngroups; i++) do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace(" %d" , (int) groups[i]); } while (0);;
 do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace(")\n"); } while (0);;
    }


    if (fileptr->st_mode & 0002)
     return (char)1;

    if ((fileptr->st_mode & 0200)
     && (fileptr->st_uid == myUid))
     return (char)1;

    if (fileptr->st_mode & 0020)
    {
    for (i=0; i<ngroups; i++) {
            if (groups[i] == fileptr->st_gid)
         return (char)1;
 }
    }
    do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("\tFile is not editable.\n"); } while (0);;
    return (char)0;







}






static int FileCleanup (HTRequest *req, int status)
{
    HTNet * net = HTRequest_net(req);
    file_info * file = (file_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(req);


    if (input) {
        if (status == -902)
            (*input->isa->abort)(input, ((void *)0));
        else
            (*input->isa->_free)(input);
        HTRequest_setInputStream(req, ((void *)0));
    }




    if (file->timer) {
 HTTimer_delete(file->timer);
 file->timer = ((void *)0);
    }

    if (file) {
 {HTMemory_free((file->local));((file->local))=((void *)0);};
 {HTMemory_free((file));((file))=((void *)0);};
    }
    HTNet_delete(net, status);
    return (char)1;
}
// # 347 "HTFile.c"
static int FileEvent (int soc, void * pVoid, HTEventType type);

 int HTLoadFile (int soc, HTRequest * request)
{
    file_info *file;
    HTNet * net = HTRequest_net(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);

    do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Looking for `%s\'\n" , HTAnchor_physical(anchor)); } while (0);;

    if ((file = (file_info *) HTMemory_calloc((1), (sizeof(file_info)))) == ((void *)0))
 HTMemory_outofmem(("HTLoadFILE"), "HTFile.c", 358);
    file->state = FS_BEGIN;
    file->net = net;
    HTNet_setContext(net, file);
    HTNet_setEventCallback(net, FileEvent);
    HTNet_setEventParam(net, file);

    return FileEvent(soc, file, HTEvent_BEGIN);
}

static int ReturnEvent (HTTimer * timer, void * param, HTEventType type)
{
    file_info * file = (file_info *) param;
    if (timer != file->timer)
 HTDebugBreak("HTFile.c", 372, "File timer %p not in sync\n" , timer);
    do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Continuing %p with timer %p\n" , file , timer); } while (0);;




    HTTimer_delete(file->timer);
    file->timer = ((void *)0);




    return FileEvent((-1), file, HTEvent_READ);
}


static int FileEvent (int soc, void * pVoid, HTEventType type)
{
    file_info *file = pVoid;
    int status = -1;
    HTNet * net = file->net;
    HTRequest * request = HTNet_request(net);
    HTParentAnchor * anchor = HTRequest_anchor(request);






    if (type == HTEvent_CLOSE) {
 HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_INTERRUPTED,
      ((void *)0), 0, "HTLoadFile");
 FileCleanup(request, -902);
 return 0;
    }



    while (1) {
 switch (file->state) {
 case FS_BEGIN:


     if (!((HTRequest_method(request)) & (METHOD_GET|METHOD_HEAD))) {
  HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_NOT_ALLOWED,
       ((void *)0), 0, "HTLoadFile");
  file->state = FS_ERROR;
  break;
     }


     if (HTLib_secure()) {
  do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("LoadFile.... No access to local file system\n"); } while (0);;
  file->state = FS_TRY_FTP;
  break;
     }
     file->local = HTWWWToLocal(HTAnchor_physical(anchor), "",
           HTRequest_userProfile(request));
     if (!file->local) {
  file->state = FS_TRY_FTP;
  break;
     }


     {
  HTHost * host = ((void *)0);
  if ((host = HTHost_new("localhost", 0)) == ((void *)0)) return -1;
  HTNet_setHost(net, host);
  if (HTHost_addNet(host, net) == 902) {
      do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Pending...\n"); } while (0);;

      file->state = FS_PENDING;
      return 0;
  }
     }
     file->state = FS_DO_CN;
     break;

 case FS_PENDING:
// # 462 "HTFile.c"
     {
  HTHost * host = ((void *)0);
  if ((host = HTHost_new("localhost", 0)) == ((void *)0)) return -1;
  HTNet_setHost(net, host);
  if (HTHost_addNet(host, net) == 902) {
      do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Pending...\n"); } while (0);;
      file->state = FS_PENDING;
      return 0;
  }
     }
     file->state = FS_DO_CN;
     break;

 case FS_DO_CN:
// # 485 "HTFile.c"
     if (HTRequest_negotiation(request) &&
  ((HTRequest_method(request)) & (METHOD_GET|METHOD_HEAD))) {
   char * conneg = HTMulti(request, file->local,&file->stat_info);
  if (conneg) {
      {HTMemory_free((file->local));((file->local))=((void *)0);};
      file->local = conneg;
      HTAnchor_setPhysical(anchor, conneg);
      do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Load File... Found `%s\'\n" , conneg); } while (0);;
  } else {
      do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Load File... Not found - even tried content negotiation\n"); } while (0);;
      HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_NOT_FOUND,
           ((void *)0), 0, "HTLoadFile");
      file->state = FS_ERROR;
      break;
  }
     } else {
  if (stat(file->local, &file->stat_info) == -1) {
      do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("Load File... Not found `%s\'\n" , file->local); } while (0);;
      HTRequest_addError(request, ERR_FATAL, (char)0, HTERR_NOT_FOUND,
           ((void *)0), 0, "HTLoadFile");
      file->state = FS_ERROR;
      break;
  }
     }





     if (((file->stat_info.st_mode) & 0170000) == 0040000) {
  if (HTRequest_method(request) == METHOD_GET)
      file->state = FS_PARSE_DIR;
  else {
      HTRequest_addError(request, ERR_INFO, (char)0, HTERR_NO_CONTENT,
           ((void *)0), 0, "HTLoadFile");
      file->state = FS_NO_DATA;
  }
  break;
     }





     {
  char editable = HTEditable(file->local, &file->stat_info);
  if (file_suffix_binding) HTBind_getAnchorBindings(anchor);
  if (editable) HTAnchor_appendAllow(anchor, METHOD_PUT);


  if (file->stat_info.st_size)
      HTAnchor_setLength(anchor, file->stat_info.st_size);


  if (file->stat_info.st_mtim.tv_sec > 0)
      HTAnchor_setLastModified(anchor, file->stat_info.st_mtim.tv_sec);


  if (!editable && !file->stat_info.st_size) {
      HTRequest_addError(request, ERR_INFO, (char)0, HTERR_NO_CONTENT,
           ((void *)0), 0, "HTLoadFile");
      file->state = FS_NO_DATA;
  } else {
      file->state = (HTRequest_method(request)==METHOD_GET) ?
   FS_NEED_OPEN_FILE : FS_GOT_DATA;
  }
     }
     break;

   case FS_NEED_OPEN_FILE:
     status = HTFileOpen(net, file->local, 00);
     if (status == 0) {





  {
      HTStream * rstream = HTStreamStack(HTAnchor_format(anchor),
             HTRequest_outputFormat(request),
             HTRequest_outputStream(request),
             request, (char)1);
      HTNet_setReadStream(net, rstream);
      HTRequest_setOutputConnected(request, (char)1);
  }





  {
      HTOutputStream * output = HTNet_getOutput(net, ((void *)0), 0);
      HTRequest_setInputStream(request, (HTStream *) output);
  }







  if (((request) && (request)->source && (request) == (request)->source) && !HTRequest_destinationsReady(request))
      return 0;
  HTRequest_addError(request, ERR_INFO, (char)0, HTERR_OK, ((void *)0), 0,
       "HTLoadFile");
  file->state = FS_NEED_BODY;







  if (HTEvent_isCallbacksRegistered()) {
      if (!HTRequest_preemptive(request)) {
   if (!HTNet_preemptive(net)) {
       do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Returning\n"); } while (0);;
       HTHost_register(HTNet_host(net), net, HTEvent_READ);
   } else if (!file->timer) {
       do { if (((WWW_TraceFlag) & SHOW_PROTOCOL_TRACE)) HTTrace("HTLoadFile.. Returning\n"); } while (0);;
       file->timer =
    HTTimer_new(((void *)0), ReturnEvent, file, 1, (char)1, (char)0);
   }
   return 0;
      }
  }
     } else if (status == -901 || status == 902)
  return 0;
     else {
  HTRequest_addError(request, ERR_INFO, (char)0, HTERR_INTERNAL,
       ((void *)0), 0, "HTLoadFile");
  file->state = FS_ERROR;
     }
     break;

   case FS_NEED_BODY:
     status = HTHost_read(HTNet_host(net), net);
     if (status == -901)
  return 0;
     else if (status == 200 || status == 901) {
  file->state = FS_GOT_DATA;
     } else {
  HTRequest_addError(request, ERR_INFO, (char)0, HTERR_FORBIDDEN,
       ((void *)0), 0, "HTLoadFile");
  file->state = FS_ERROR;
     }
     break;

   case FS_PARSE_DIR:
     status = HTFile_readDir(request, file);
     if (status == 200)
  file->state = FS_GOT_DATA;
     else
  file->state = FS_ERROR;
     break;

   case FS_TRY_FTP:
     {
  char *url = HTAnchor_physical(anchor);
  HTAnchor *anchor;
  char *newname = ((void *)0);
  HTSACopy (&(newname), "ftp:");
  if (!strncmp(url, "file:", 5))
      HTSACat (&(newname), url+5);
  else
      HTSACat (&(newname), url);
  anchor = HTAnchor_findAddress(newname);
  HTRequest_setAnchor(request, anchor);
  {HTMemory_free((newname));((newname))=((void *)0);};
  FileCleanup(request, 900);
  return HTLoad(request, (char)1);
     }
     break;

   case FS_GOT_DATA:
     FileCleanup(request, 200);
     return 0;
     break;

   case FS_NO_DATA:
     FileCleanup(request, 204);
     return 0;
     break;

   case FS_RETRY:
     FileCleanup(request, -503);
     return 0;
     break;

   case FS_ERROR:
     FileCleanup(request, -1);
     return 0;
     break;
 }
    }
}
