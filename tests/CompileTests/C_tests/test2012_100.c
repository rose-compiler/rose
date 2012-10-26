// # 1 "fnmatch.c"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "fnmatch.c"
// # 17 "fnmatch.c"
// # 1 "../config.h" 1
// # 18 "fnmatch.c" 2
// # 29 "fnmatch.c"
// # 1 "./fnmatch.h" 1
// # 70 "./fnmatch.h"
extern int posix_fnmatch (const char *__pattern, const char *__name,
                    int __flags)
     __attribute__ ((__nonnull__ (1, 2)));
// # 30 "fnmatch.c" 2

// # 1 "./alloca.h" 1
// # 32 "fnmatch.c" 2
// # 1 "/usr/include/assert.h" 1 3 4
// # 36 "/usr/include/assert.h" 3 4
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
// # 37 "/usr/include/assert.h" 2 3 4
// # 65 "/usr/include/assert.h" 3 4



extern void __assert_fail (__const char *__assertion, __const char *__file,
      unsigned int __line, __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, __const char *__file,
      unsigned int __line,
      __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



// # 33 "fnmatch.c" 2
// # 1 "./ctype.h" 1
// # 29 "./ctype.h"
       
// # 30 "./ctype.h" 3





// # 1 "/usr/include/ctype.h" 1 3 4
// # 28 "/usr/include/ctype.h" 3 4
// # 1 "/usr/include/bits/types.h" 1 3 4
// # 28 "/usr/include/bits/types.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 29 "/usr/include/bits/types.h" 2 3 4


// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 214 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long unsigned int size_t;
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
// # 29 "/usr/include/ctype.h" 2 3 4


// # 41 "/usr/include/ctype.h" 3 4
// # 1 "/usr/include/endian.h" 1 3 4
// # 37 "/usr/include/endian.h" 3 4
// # 1 "/usr/include/bits/endian.h" 1 3 4
// # 38 "/usr/include/endian.h" 2 3 4
// # 42 "/usr/include/ctype.h" 2 3 4






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








extern int isblank (int) __attribute__ ((__nothrow__));






extern int isctype (int __c, int __mask) __attribute__ ((__nothrow__));






extern int isascii (int __c) __attribute__ ((__nothrow__));



extern int toascii (int __c) __attribute__ ((__nothrow__));



extern int _toupper (int) __attribute__ ((__nothrow__));
extern int _tolower (int) __attribute__ ((__nothrow__));
// # 233 "/usr/include/ctype.h" 3 4
// # 1 "/usr/include/xlocale.h" 1 3 4
// # 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;
// # 234 "/usr/include/ctype.h" 2 3 4
// # 247 "/usr/include/ctype.h" 3 4
extern int isalnum_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isalpha_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int iscntrl_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isdigit_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int islower_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isgraph_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isprint_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int ispunct_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isspace_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isupper_l (int, __locale_t) __attribute__ ((__nothrow__));
extern int isxdigit_l (int, __locale_t) __attribute__ ((__nothrow__));

extern int isblank_l (int, __locale_t) __attribute__ ((__nothrow__));



extern int __tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
extern int tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));


extern int __toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
extern int toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
// # 323 "/usr/include/ctype.h" 3 4

// # 36 "./ctype.h" 2 3
// # 34 "fnmatch.c" 2
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
// # 55 "/usr/include/errno.h" 3 4
extern char *program_invocation_name, *program_invocation_short_name;




// # 69 "/usr/include/errno.h" 3 4
typedef int error_t;
// # 35 "fnmatch.c" 2
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 152 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long int ptrdiff_t;
// # 326 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef int wchar_t;
// # 36 "fnmatch.c" 2
// # 1 "./stdbool.h" 1
// # 37 "fnmatch.c" 2
// # 1 "./stdlib.h" 1
// # 20 "./stdlib.h"
       
// # 21 "./stdlib.h" 3
// # 35 "./stdlib.h" 3
// # 1 "/usr/include/stdlib.h" 1 3 4
// # 33 "/usr/include/stdlib.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "/usr/include/stdlib.h" 2 3 4








// # 1 "/usr/include/bits/waitflags.h" 1 3 4
// # 43 "/usr/include/stdlib.h" 2 3 4
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
// # 44 "/usr/include/stdlib.h" 2 3 4
// # 68 "/usr/include/stdlib.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
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







__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;


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





extern float strtof (__const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern long double strtold (__const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





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

// # 240 "/usr/include/stdlib.h" 3 4
extern long int strtol_l (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base,
     __locale_t __loc) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 4))) ;

extern unsigned long int strtoul_l (__const char *__restrict __nptr,
        char **__restrict __endptr,
        int __base, __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 4))) ;

__extension__
extern long long int strtoll_l (__const char *__restrict __nptr,
    char **__restrict __endptr, int __base,
    __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 4))) ;

__extension__
extern unsigned long long int strtoull_l (__const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 4))) ;

extern double strtod_l (__const char *__restrict __nptr,
   char **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3))) ;

extern float strtof_l (__const char *__restrict __nptr,
         char **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3))) ;

extern long double strtold_l (__const char *__restrict __nptr,
         char **__restrict __endptr,
         __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3))) ;






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




// # 1 "./sys/types.h" 1 3 4
// # 20 "./sys/types.h" 3 4
       
// # 21 "./sys/types.h" 3






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






typedef __ino64_t ino64_t;




typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;






typedef __off64_t off64_t;




typedef __pid_t pid_t;




typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
// # 133 "/usr/include/sys/types.h" 3 4
// # 1 "./time.h" 1 3 4
// # 20 "./time.h" 3 4
       
// # 21 "./time.h" 3
// # 33 "./time.h" 3
// # 1 "/usr/include/time.h" 1 3 4
// # 59 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



// # 75 "/usr/include/time.h" 3 4


typedef __time_t time_t;



// # 93 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
// # 105 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
// # 34 "./time.h" 2 3
// # 134 "/usr/include/sys/types.h" 2 3 4



typedef __useconds_t useconds_t;



typedef __suseconds_t suseconds_t;





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
// # 220 "/usr/include/sys/types.h" 3 4
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





// # 1 "./time.h" 1 3 4
// # 20 "./time.h" 3 4
       
// # 21 "./time.h" 3
// # 33 "./time.h" 3
// # 1 "/usr/include/time.h" 1 3 4
// # 121 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
// # 34 "./time.h" 2 3
// # 45 "/usr/include/sys/select.h" 2 3 4

// # 1 "/usr/include/bits/time.h" 1 3 4
// # 69 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
// # 47 "/usr/include/sys/select.h" 2 3 4
// # 55 "/usr/include/sys/select.h" 3 4
typedef long int __fd_mask;
// # 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {



    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];





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




typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
// # 262 "/usr/include/sys/types.h" 3 4
typedef __blkcnt64_t blkcnt64_t;
typedef __fsblkcnt64_t fsblkcnt64_t;
typedef __fsfilcnt64_t fsfilcnt64_t;





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



// # 28 "./sys/types.h" 2 3
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
// # 617 "/usr/include/stdlib.h" 3 4
extern void *valloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));






extern void _Exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));






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
// # 719 "/usr/include/stdlib.h" 3 4
extern int mkstemp64 (char *__template) __attribute__ ((__nonnull__ (1))) ;
// # 729 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;








extern int system (__const char *__command) ;







extern char *canonicalize_file_name (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
// # 756 "/usr/include/stdlib.h" 3 4
extern char *realpath (__const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__)) ;






typedef int (*__compar_fn_t) (__const void *, __const void *);


typedef __compar_fn_t comparison_fn_t;






extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));



extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;



__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;







extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;




__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
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
// # 909 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *__const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;





extern void setkey (__const char *__key) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







extern int posix_openpt (int __oflag) ;







extern int grantpt (int __fd) __attribute__ ((__nothrow__));



extern int unlockpt (int __fd) __attribute__ ((__nothrow__));




extern char *ptsname (int __fd) __attribute__ ((__nothrow__)) ;






extern int ptsname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));


extern int getpt (void);






extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 977 "/usr/include/stdlib.h" 3 4

// # 36 "./stdlib.h" 2 3





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 42 "./stdlib.h" 2 3
// # 606 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 626 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1032 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1154 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1179 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1242 "./stdlib.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 38 "fnmatch.c" 2
// # 1 "./string.h" 1
// # 22 "./string.h"
       
// # 23 "./string.h" 3




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





extern void *rawmemchr (__const void *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


extern void *memrchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





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

// # 121 "/usr/include/string.h" 3 4
extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));




extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
// # 165 "/usr/include/string.h" 3 4


extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strchrnul (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





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




extern char *strcasestr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memmem (__const void *__haystack, size_t __haystacklen,
       __const void *__needle, size_t __needlelen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 3)));



extern void *__mempcpy (void *__restrict __dest,
   __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern void *mempcpy (void *__restrict __dest,
        __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (__const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

// # 281 "/usr/include/string.h" 3 4
extern char *strerror_r (int __errnum, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));





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




extern int ffsl (long int __l) __attribute__ ((__nothrow__)) __attribute__ ((__const__));

__extension__ extern int ffsll (long long int __ll)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));




extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));





extern int strcasecmp_l (__const char *__s1, __const char *__s2,
    __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern int strncasecmp_l (__const char *__s1, __const char *__s2,
     size_t __n, __locale_t __loc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 4)));





extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));




extern int strverscmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strsignal (int __sig) __attribute__ ((__nothrow__));


extern char *__stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strfry (char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern void *memfrob (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern char *basename (__const char *__filename) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
// # 426 "/usr/include/string.h" 3 4

// # 28 "./string.h" 2 3





// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 34 "./string.h" 2 3
// # 422 "./string.h" 3
extern int _gl_cxxalias_dummy;
// # 432 "./string.h" 3
extern int _gl_cxxalias_dummy;
// # 486 "./string.h" 3
extern int _gl_cxxalias_dummy;


extern int _gl_cxxalias_dummy;
// # 508 "./string.h" 3
extern int _gl_cxxalias_dummy;







extern int _gl_cxxalias_dummy;
// # 563 "./string.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 675 "./string.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 753 "./string.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1044 "./string.h" 3
extern size_t mbslen (const char *string) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1109 "./string.h" 3
extern char * mbsstr (const char *haystack, const char *needle)
     __attribute__ ((__pure__))
     __attribute__ ((__nonnull__ (1, 2)));
// # 1121 "./string.h" 3
extern int mbscasecmp (const char *s1, const char *s2)
     __attribute__ ((__pure__))
     __attribute__ ((__nonnull__ (1, 2)));
// # 1257 "./string.h" 3
extern char * rpl_strerror (int);
extern int _gl_cxxalias_dummy;



extern int _gl_cxxalias_dummy;
// # 39 "fnmatch.c" 2
// # 47 "fnmatch.c"
// # 1 "./wctype.h" 1
// # 32 "./wctype.h"
       
// # 33 "./wctype.h" 3
// # 42 "./wctype.h" 3
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 43 "./wctype.h" 2 3
// # 1 "/usr/include/stdio.h" 1 3 4
// # 30 "/usr/include/stdio.h" 3 4




// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 35 "/usr/include/stdio.h" 2 3 4
// # 44 "/usr/include/stdio.h" 3 4


typedef struct _IO_FILE FILE;





// # 62 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
// # 72 "/usr/include/stdio.h" 3 4
// # 1 "/usr/include/libio.h" 1 3 4
// # 32 "/usr/include/libio.h" 3 4
// # 1 "/usr/include/_G_config.h" 1 3 4
// # 14 "/usr/include/_G_config.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 355 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef unsigned int wint_t;
// # 15 "/usr/include/_G_config.h" 2 3 4
// # 24 "/usr/include/_G_config.h" 3 4
// # 1 "./wchar.h" 1 3 4
// # 30 "./wchar.h" 3 4
       
// # 31 "./wchar.h" 3
// # 47 "./wchar.h" 3
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
// # 48 "./wchar.h" 2 3
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
// # 1 "./wchar.h" 1 3 4
// # 30 "./wchar.h" 3 4
       
// # 31 "./wchar.h" 3
// # 47 "./wchar.h" 3
// # 1 "/usr/include/wchar.h" 1 3 4
// # 48 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 49 "/usr/include/wchar.h" 2 3 4
// # 48 "./wchar.h" 2 3
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




typedef __io_read_fn cookie_read_function_t;
typedef __io_write_fn cookie_write_function_t;
typedef __io_seek_fn cookie_seek_function_t;
typedef __io_close_fn cookie_close_function_t;


typedef struct
{
  __io_read_fn *read;
  __io_write_fn *write;
  __io_seek_fn *seek;
  __io_close_fn *close;
} _IO_cookie_io_functions_t;
typedef _IO_cookie_io_functions_t cookie_io_functions_t;

struct _IO_cookie_file;


extern void _IO_cookie_init (struct _IO_cookie_file *__cfile, int __read_write,
        void *__cookie, _IO_cookie_io_functions_t __fns);







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




typedef __gnuc_va_list va_list;
// # 86 "/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;





typedef _G_fpos64_t fpos64_t;
// # 138 "/usr/include/stdio.h" 3 4
// # 1 "/usr/include/bits/stdio_lim.h" 1 3 4
// # 139 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;









extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));




extern int renameat (int __oldfd, __const char *__old, int __newfd,
       __const char *__new) __attribute__ ((__nothrow__));








extern FILE *tmpfile (void);
// # 181 "/usr/include/stdio.h" 3 4
extern FILE *tmpfile64 (void);



extern char *tmpnam (char *__s) __attribute__ ((__nothrow__));





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__));
// # 203 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__));








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

// # 228 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
// # 238 "/usr/include/stdio.h" 3 4
extern int fcloseall (void);









extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes);




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream);
// # 269 "/usr/include/stdio.h" 3 4


extern FILE *fopen64 (__const char *__restrict __filename,
        __const char *__restrict __modes);
extern FILE *freopen64 (__const char *__restrict __filename,
   __const char *__restrict __modes,
   FILE *__restrict __stream);




extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__));





extern FILE *fopencookie (void *__restrict __magic_cookie,
     __const char *__restrict __modes,
     _IO_cookie_io_functions_t __io_funcs) __attribute__ ((__nothrow__));


extern FILE *fmemopen (void *__s, size_t __len, __const char *__modes) __attribute__ ((__nothrow__));




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__));






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






extern int vasprintf (char **__restrict __ptr, __const char *__restrict __f,
        __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0)));
extern int __asprintf (char **__restrict __ptr,
         __const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3)));
extern int asprintf (char **__restrict __ptr,
       __const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3)));







extern int vdprintf (int __fd, __const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, __const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));








extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));








extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (__const char *__restrict __s,
      __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__scanf__, 2, 0)));









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

// # 550 "/usr/include/stdio.h" 3 4
extern char *fgets_unlocked (char *__restrict __s, int __n,
        FILE *__restrict __stream) ;
// # 566 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;








extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s) ;

// # 627 "/usr/include/stdio.h" 3 4
extern int fputs_unlocked (__const char *__restrict __s,
      FILE *__restrict __stream);
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



extern int fseeko64 (FILE *__stream, __off64_t __off, int __whence);
extern __off64_t ftello64 (FILE *__stream) ;
extern int fgetpos64 (FILE *__restrict __stream, fpos64_t *__restrict __pos);
extern int fsetpos64 (FILE *__stream, __const fpos64_t *__pos);




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


extern int _sys_nerr;
extern __const char *__const _sys_errlist[];
// # 755 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
// # 774 "/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));





extern char *cuserid (char *__s);




struct obstack;


extern int obstack_printf (struct obstack *__restrict __obstack,
      __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3)));
extern int obstack_vprintf (struct obstack *__restrict __obstack,
       __const char *__restrict __format,
       __gnuc_va_list __args)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0)));







extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
// # 844 "/usr/include/stdio.h" 3 4

// # 44 "./wctype.h" 2 3
// # 1 "./time.h" 1 3
// # 20 "./time.h" 3
       
// # 21 "./time.h" 3
// # 39 "./time.h" 3
// # 1 "/usr/include/time.h" 1 3 4
// # 31 "/usr/include/time.h" 3 4








// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 40 "/usr/include/time.h" 2 3 4



// # 1 "/usr/include/bits/time.h" 1 3 4
// # 44 "/usr/include/time.h" 2 3 4
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





extern char *strptime (__const char *__restrict __s,
         __const char *__restrict __fmt, struct tm *__tp)
     __attribute__ ((__nothrow__));







extern size_t strftime_l (char *__restrict __s, size_t __maxsize,
     __const char *__restrict __format,
     __const struct tm *__restrict __tp,
     __locale_t __loc) __attribute__ ((__nothrow__));

extern char *strptime_l (__const char *__restrict __s,
    __const char *__restrict __fmt, struct tm *__tp,
    __locale_t __loc) __attribute__ ((__nothrow__));






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
// # 389 "/usr/include/time.h" 3 4
extern int getdate_err;
// # 398 "/usr/include/time.h" 3 4
extern struct tm *getdate (__const char *__string);
// # 412 "/usr/include/time.h" 3 4
extern int getdate_r (__const char *__restrict __string,
        struct tm *__restrict __resbufp);



// # 40 "./time.h" 2 3


// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 43 "./time.h" 2 3
// # 407 "./time.h" 3
struct __time_t_must_be_integral {
  unsigned int __floating_time_t_unsupported : (time_t) 1;
};
// # 45 "./wctype.h" 2 3
// # 1 "./wchar.h" 1 3
// # 30 "./wchar.h" 3
       
// # 31 "./wchar.h" 3
// # 79 "./wchar.h" 3
// # 1 "/usr/include/wchar.h" 1 3 4
// # 40 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 41 "/usr/include/wchar.h" 2 3 4







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




extern int wcscasecmp (__const wchar_t *__s1, __const wchar_t *__s2) __attribute__ ((__nothrow__));


extern int wcsncasecmp (__const wchar_t *__s1, __const wchar_t *__s2,
   size_t __n) __attribute__ ((__nothrow__));





extern int wcscasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
    __locale_t __loc) __attribute__ ((__nothrow__));

extern int wcsncasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
     size_t __n, __locale_t __loc) __attribute__ ((__nothrow__));





extern int wcscoll (__const wchar_t *__s1, __const wchar_t *__s2) __attribute__ ((__nothrow__));



extern size_t wcsxfrm (wchar_t *__restrict __s1,
         __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));








extern int wcscoll_l (__const wchar_t *__s1, __const wchar_t *__s2,
        __locale_t __loc) __attribute__ ((__nothrow__));




extern size_t wcsxfrm_l (wchar_t *__s1, __const wchar_t *__s2,
    size_t __n, __locale_t __loc) __attribute__ ((__nothrow__));


extern wchar_t *wcsdup (__const wchar_t *__s) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__));




extern wchar_t *wcschr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcsrchr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));





extern wchar_t *wcschrnul (__const wchar_t *__s, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));





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




extern wchar_t *wcswcs (__const wchar_t *__haystack, __const wchar_t *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));




extern size_t wcsnlen (__const wchar_t *__s, size_t __maxlen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));





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





extern wchar_t *wmempcpy (wchar_t *__restrict __s1,
     __const wchar_t *__restrict __s2, size_t __n)
     __attribute__ ((__nothrow__));






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






extern size_t mbsnrtowcs (wchar_t *__restrict __dst,
     __const char **__restrict __src, size_t __nmc,
     size_t __len, mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));



extern size_t wcsnrtombs (char *__restrict __dst,
     __const wchar_t **__restrict __src,
     size_t __nwc, size_t __len,
     mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));






extern int wcwidth (wchar_t __c) __attribute__ ((__nothrow__));



extern int wcswidth (__const wchar_t *__s, size_t __n) __attribute__ ((__nothrow__));






extern double wcstod (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));



extern float wcstof (__const wchar_t *__restrict __nptr,
       wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));
extern long double wcstold (__const wchar_t *__restrict __nptr,
       wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));





extern long int wcstol (__const wchar_t *__restrict __nptr,
   wchar_t **__restrict __endptr, int __base) __attribute__ ((__nothrow__));



extern unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
      wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));




__extension__
extern long long int wcstoll (__const wchar_t *__restrict __nptr,
         wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));



__extension__
extern unsigned long long int wcstoull (__const wchar_t *__restrict __nptr,
     wchar_t **__restrict __endptr,
     int __base) __attribute__ ((__nothrow__));






__extension__
extern long long int wcstoq (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));



__extension__
extern unsigned long long int wcstouq (__const wchar_t *__restrict __nptr,
           wchar_t **__restrict __endptr,
           int __base) __attribute__ ((__nothrow__));
// # 467 "/usr/include/wchar.h" 3 4
extern long int wcstol_l (__const wchar_t *__restrict __nptr,
     wchar_t **__restrict __endptr, int __base,
     __locale_t __loc) __attribute__ ((__nothrow__));

extern unsigned long int wcstoul_l (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr,
        int __base, __locale_t __loc) __attribute__ ((__nothrow__));

__extension__
extern long long int wcstoll_l (__const wchar_t *__restrict __nptr,
    wchar_t **__restrict __endptr,
    int __base, __locale_t __loc) __attribute__ ((__nothrow__));

__extension__
extern unsigned long long int wcstoull_l (__const wchar_t *__restrict __nptr,
       wchar_t **__restrict __endptr,
       int __base, __locale_t __loc)
     __attribute__ ((__nothrow__));

extern double wcstod_l (__const wchar_t *__restrict __nptr,
   wchar_t **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__));

extern float wcstof_l (__const wchar_t *__restrict __nptr,
         wchar_t **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__));

extern long double wcstold_l (__const wchar_t *__restrict __nptr,
         wchar_t **__restrict __endptr,
         __locale_t __loc) __attribute__ ((__nothrow__));





extern double __wcstod_internal (__const wchar_t *__restrict __nptr,
     wchar_t **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__));
extern float __wcstof_internal (__const wchar_t *__restrict __nptr,
    wchar_t **__restrict __endptr, int __group)
     __attribute__ ((__nothrow__));
extern long double __wcstold_internal (__const wchar_t *__restrict __nptr,
           wchar_t **__restrict __endptr,
           int __group) __attribute__ ((__nothrow__));
// # 593 "/usr/include/wchar.h" 3 4
extern wchar_t *wcpcpy (wchar_t *__dest, __const wchar_t *__src) __attribute__ ((__nothrow__));



extern wchar_t *wcpncpy (wchar_t *__dest, __const wchar_t *__src, size_t __n)
     __attribute__ ((__nothrow__));
// # 607 "/usr/include/wchar.h" 3 4
extern __FILE *open_wmemstream (wchar_t **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__));






extern int fwide (__FILE *__fp, int __mode) __attribute__ ((__nothrow__));






extern int fwprintf (__FILE *__restrict __stream,
       __const wchar_t *__restrict __format, ...)
                                                           ;




extern int wprintf (__const wchar_t *__restrict __format, ...)
                                                           ;

extern int swprintf (wchar_t *__restrict __s, size_t __n,
       __const wchar_t *__restrict __format, ...)
     __attribute__ ((__nothrow__)) ;





extern int vfwprintf (__FILE *__restrict __s,
        __const wchar_t *__restrict __format,
        __gnuc_va_list __arg)
                                                           ;




extern int vwprintf (__const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
                                                           ;


extern int vswprintf (wchar_t *__restrict __s, size_t __n,
        __const wchar_t *__restrict __format,
        __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) ;






extern int fwscanf (__FILE *__restrict __stream,
      __const wchar_t *__restrict __format, ...)
                                                          ;




extern int wscanf (__const wchar_t *__restrict __format, ...)
                                                          ;

extern int swscanf (__const wchar_t *__restrict __s,
      __const wchar_t *__restrict __format, ...)
     __attribute__ ((__nothrow__)) ;











extern int vfwscanf (__FILE *__restrict __s,
       __const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
                                                          ;




extern int vwscanf (__const wchar_t *__restrict __format,
      __gnuc_va_list __arg)
                                                          ;

extern int vswscanf (__const wchar_t *__restrict __s,
       __const wchar_t *__restrict __format,
       __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) ;










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

// # 768 "/usr/include/wchar.h" 3 4
extern wint_t getwc_unlocked (__FILE *__stream);
extern wint_t getwchar_unlocked (void);







extern wint_t fgetwc_unlocked (__FILE *__stream);







extern wint_t fputwc_unlocked (wchar_t __wc, __FILE *__stream);
// # 794 "/usr/include/wchar.h" 3 4
extern wint_t putwc_unlocked (wchar_t __wc, __FILE *__stream);
extern wint_t putwchar_unlocked (wchar_t __wc);
// # 804 "/usr/include/wchar.h" 3 4
extern wchar_t *fgetws_unlocked (wchar_t *__restrict __ws, int __n,
     __FILE *__restrict __stream);







extern int fputws_unlocked (__const wchar_t *__restrict __ws,
       __FILE *__restrict __stream);







extern size_t wcsftime (wchar_t *__restrict __s, size_t __maxsize,
   __const wchar_t *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));







extern size_t wcsftime_l (wchar_t *__restrict __s, size_t __maxsize,
     __const wchar_t *__restrict __format,
     __const struct tm *__restrict __tp,
     __locale_t __loc) __attribute__ ((__nothrow__));
// # 858 "/usr/include/wchar.h" 3 4

// # 80 "./wchar.h" 2 3
// # 464 "./wchar.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 483 "./wchar.h" 3
extern int rpl_wctob (wint_t wc) __attribute__ ((__pure__));
extern int _gl_cxxalias_dummy;







extern int _gl_cxxalias_dummy;
// # 515 "./wchar.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 543 "./wchar.h" 3
extern int _gl_cxxalias_dummy;


extern int _gl_cxxalias_dummy;
// # 569 "./wchar.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 600 "./wchar.h" 3
extern int _gl_cxxalias_dummy;



extern int _gl_cxxalias_dummy;
// # 662 "./wchar.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 756 "./wchar.h" 3
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 1085 "./wchar.h" 3
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
// # 46 "./wctype.h" 2 3






// # 1 "/usr/include/wctype.h" 1 3 4
// # 35 "/usr/include/wctype.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 36 "/usr/include/wctype.h" 2 3 4
// # 45 "/usr/include/wctype.h" 3 4

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





extern int iswblank (wint_t __wc) __attribute__ ((__nothrow__));
// # 185 "/usr/include/wctype.h" 3 4
extern wctype_t wctype (__const char *__property) __attribute__ ((__nothrow__));



extern int iswctype (wint_t __wc, wctype_t __desc) __attribute__ ((__nothrow__));










typedef __const __int32_t *wctrans_t;







extern wint_t towlower (wint_t __wc) __attribute__ ((__nothrow__));


extern wint_t towupper (wint_t __wc) __attribute__ ((__nothrow__));



// # 227 "/usr/include/wctype.h" 3 4





extern wctrans_t wctrans (__const char *__property) __attribute__ ((__nothrow__));


extern wint_t towctrans (wint_t __wc, wctrans_t __desc) __attribute__ ((__nothrow__));








extern int iswalnum_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));





extern int iswalpha_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));


extern int iswcntrl_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));



extern int iswdigit_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));



extern int iswgraph_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswlower_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));


extern int iswprint_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswpunct_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswspace_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswupper_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswxdigit_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));




extern int iswblank_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));



extern wctype_t wctype_l (__const char *__property, __locale_t __locale)
     __attribute__ ((__nothrow__));



extern int iswctype_l (wint_t __wc, wctype_t __desc, __locale_t __locale)
     __attribute__ ((__nothrow__));







extern wint_t towlower_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));


extern wint_t towupper_l (wint_t __wc, __locale_t __locale) __attribute__ ((__nothrow__));



extern wctrans_t wctrans_l (__const char *__property, __locale_t __locale)
     __attribute__ ((__nothrow__));


extern wint_t towctrans_l (wint_t __wc, wctrans_t __desc,
      __locale_t __locale) __attribute__ ((__nothrow__));




// # 53 "./wctype.h" 2 3
// # 677 "./wctype.h" 3
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;





extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
// # 739 "./wctype.h" 3
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
// # 753 "./wctype.h" 3
extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;

extern int _gl_cxxalias_dummy;
extern int _gl_cxxalias_dummy;
// # 48 "fnmatch.c" 2
// # 1 "./wchar.h" 1
// # 30 "./wchar.h"
       
// # 31 "./wchar.h" 3
// # 49 "fnmatch.c" 2
// # 136 "fnmatch.c"
static int posixly_correct;
// # 170 "fnmatch.c"
// # 1 "fnmatch_loop.c" 1
// # 19 "fnmatch_loop.c"
static int ext_match (int opt, const char *pattern, const char *string,
                const char *string_end, _Bool no_leading_period, int flags)
     ;
static const char *end_pattern (const char *patternp) ;

static int

internal_fnmatch (const char *pattern, const char *string, const char *string_end,
     _Bool no_leading_period, int flags)
{
  register const char *p = pattern, *n = string;
  register unsigned char c;
// # 41 "fnmatch_loop.c"
  while ((c = *p++) != '\0')
    {
      _Bool new_no_leading_period = 0;
      c = ((flags & (1 << 4)) ? tolower (c) : (c));

      switch (c)
        {
        case '?':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_match (c, p, n, string_end, no_leading_period,
                         flags);
              if (res != -1)
                return res;
            }

          if (n == string_end)
            return 1;
          else if (*n == '/' && (flags & (1 << 0)))
            return 1;
          else if (*n == '.' && no_leading_period)
            return 1;
          break;

        case '\\':
          if (!(flags & (1 << 1)))
            {
              c = *p++;
              if (c == '\0')

                return 1;
              c = ((flags & (1 << 4)) ? tolower (c) : (c));
            }
          if (n == string_end || ((flags & (1 << 4)) ? tolower ((unsigned char) *n) : ((unsigned char) *n)) != c)
            return 1;
          break;

        case '*':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_match (c, p, n, string_end, no_leading_period,
                         flags);
              if (res != -1)
                return res;
            }

          if (n != string_end && *n == '.' && no_leading_period)
            return 1;

          for (c = *p++; c == '?' || c == '*'; c = *p++)
            {
              if (*p == '(' && (flags & (1 << 5)) != 0)
                {
                  const char *endp = end_pattern (p);
                  if (endp != p)
                    {

                      p = endp;
                      continue;
                    }
                }

              if (c == '?')
                {

                  if (n == string_end)

                    return 1;
                  else if (*n == '/'
                           && __builtin_expect (flags & (1 << 0), 0))


                    return 1;
                  else



                    ++n;
                }
            }

          if (c == '\0')




            {
              int result = (flags & (1 << 0)) == 0 ? 0 : 1;

              if (flags & (1 << 0))
                {
                  if (flags & (1 << 3))
                    result = 0;
                  else
                    {
                      if (memchr (n, '/', string_end - n) == ((void *)0))
                        result = 0;
                    }
                }

              return result;
            }
          else
            {
              const char *endp;

              endp = memchr (n, (flags & (1 << 0)) ? '/' : '\0', string_end - n);

              if (endp == ((void *)0))
                endp = string_end;

              if (c == '['
                  || (__builtin_expect (flags & (1 << 5), 0) != 0
                      && (c == '@' || c == '+' || c == '!')
                      && *p == '('))
                {
                  int flags2 = ((flags & (1 << 0))
                                ? flags : (flags & ~(1 << 2)));
                  _Bool no_leading_period2 = no_leading_period;

                  for (--p; n < endp; ++n, no_leading_period2 = 0)
                    if (internal_fnmatch (p, n, string_end, no_leading_period2, flags2)
                        == 0)
                      return 0;
                }
              else if (c == '/' && (flags & (1 << 0)))
                {
                  while (n < string_end && *n != '/')
                    ++n;
                  if (n < string_end && *n == '/'
                      && (internal_fnmatch (p, n + 1, string_end, flags & (1 << 2), flags)
                          == 0))
                    return 0;
                }
              else
                {
                  int flags2 = ((flags & (1 << 0))
                                ? flags : (flags & ~(1 << 2)));
                  int no_leading_period2 = no_leading_period;

                  if (c == '\\' && !(flags & (1 << 1)))
                    c = *p;
                  c = ((flags & (1 << 4)) ? tolower (c) : (c));
                  for (--p; n < endp; ++n, no_leading_period2 = 0)
                    if (((flags & (1 << 4)) ? tolower ((unsigned char) *n) : ((unsigned char) *n)) == c
                        && (internal_fnmatch (p, n, string_end, no_leading_period2, flags2)
                            == 0))
                      return 0;
                }
            }


          return 1;

        case '[':
          {

            const char *p_init = p;
            const char *n_init = n;
            register _Bool not;
            char cold;
            unsigned char fn;

            if (posixly_correct == 0)
              posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;

            if (n == string_end)
              return 1;

            if (*n == '.' && no_leading_period)
              return 1;

            if (*n == '/' && (flags & (1 << 0)))

              return 1;

            not = (*p == '!' || (posixly_correct < 0 && *p == '^'));
            if (not)
              ++p;

            fn = ((flags & (1 << 4)) ? tolower ((unsigned char) *n) : ((unsigned char) *n));

            c = *p++;
            for (;;)
              {
                if (!(flags & (1 << 1)) && c == '\\')
                  {
                    if (*p == '\0')
                      return 1;
                    c = ((flags & (1 << 4)) ? tolower ((unsigned char) *p) : ((unsigned char) *p));
                    ++p;

                    goto normal_bracket;
                  }
                else if (c == '[' && *p == ':')
                  {

                    char str[256 + 1];
                    size_t c1 = 0;

                    wctype_t wt;

                    const char *startp = p;

                    for (;;)
                      {
                        if (c1 == 256)


                          return 1;

                        c = *++p;
                        if (c == ':' && p[1] == ']')
                          {
                            p += 2;
                            break;
                          }
                        if (c < 'a' || c >= 'z')
                          {


                            p = startp;
                            c = '[';
                            goto normal_bracket;
                          }
                        str[c1++] = c;
                      }
                    str[c1] = '\0';


                    wt = wctype (str);
                    if (wt == 0)

                      return 1;
// # 287 "fnmatch_loop.c"
                    if (iswctype (btowc ((unsigned char) *n), wt))
                      goto matched;
// # 305 "fnmatch_loop.c"
                    c = *p++;
                  }
// # 413 "fnmatch_loop.c"
                else if (c == '\0')
                  {

                    p = p_init;
                    n = n_init;
                    c = '[';
                    goto normal_match;
                  }
                else
                  {
                    _Bool is_range = 0;
// # 599 "fnmatch_loop.c"
                      {
                        c = ((flags & (1 << 4)) ? tolower (c) : (c));
                      normal_bracket:




                        is_range = (*p == '-' && p[1] != '\0'
                                    && p[1] != ']');

                        if (!is_range && c == fn)
                          goto matched;







                        cold = c;
                        c = *p++;
                      }

                    if (c == '-' && *p != ']')
                      {
// # 843 "fnmatch_loop.c"
                        unsigned char cend = *p++;

                        if (!(flags & (1 << 1)) && cend == '\\')
                          cend = *p++;
                        if (cend == '\0')
                          return 1;


                        if (cold <= fn && fn <= cend)
                          goto matched;


                        c = *p++;
                      }
                  }

                if (c == ']')
                  break;
              }

            if (!not)
              return 1;
            break;

          matched:

            do
              {
              ignore_next:
                c = *p++;

                if (c == '\0')

                  return 1;

                if (!(flags & (1 << 1)) && c == '\\')
                  {
                    if (*p == '\0')
                      return 1;

                    ++p;
                  }
                else if (c == '[' && *p == ':')
                  {
                    int c1 = 0;
                    const char *startp = p;

                    while (1)
                      {
                        c = *++p;
                        if (++c1 == 256)
                          return 1;

                        if (*p == ':' && p[1] == ']')
                          break;

                        if (c < 'a' || c >= 'z')
                          {
                            p = startp;
                            goto ignore_next;
                          }
                      }
                    p += 2;
                    c = *p++;
                  }
                else if (c == '[' && *p == '=')
                  {
                    c = *++p;
                    if (c == '\0')
                      return 1;
                    c = *++p;
                    if (c != '=' || p[1] != ']')
                      return 1;
                    p += 2;
                    c = *p++;
                  }
                else if (c == '[' && *p == '.')
                  {
                    ++p;
                    while (1)
                      {
                        c = *++p;
                        if (c == '\0')
                          return 1;

                        if (*p == '.' && p[1] == ']')
                          break;
                      }
                    p += 2;
                    c = *p++;
                  }
              }
            while (c != ']');
            if (not)
              return 1;
          }
          break;

        case '+':
        case '@':
        case '!':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_match (c, p, n, string_end, no_leading_period, flags);
              if (res != -1)
                return res;
            }
          goto normal_match;

        case '/':
          if (((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))))
            {
              if (n == string_end || c != (unsigned char) *n)
                return 1;

              new_no_leading_period = 1;
              break;
            }

        default:
        normal_match:
          if (n == string_end || c != ((flags & (1 << 4)) ? tolower ((unsigned char) *n) : ((unsigned char) *n)))
            return 1;
        }

      no_leading_period = new_no_leading_period;
      ++n;
    }

  if (n == string_end)
    return 0;

  if ((flags & (1 << 3)) && n != string_end && *n == '/')

    return 0;

  return 1;
}


static const char *

end_pattern (const char *pattern)
{
  const char *p = pattern;

  while (1)
    if (*++p == '\0')

      return pattern;
    else if (*p == '[')
      {

        if (posixly_correct == 0)
          posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;



        if (*++p == '!' || (posixly_correct < 0 && *p == '^'))
          ++p;

        if (*p == ']')
          ++p;

        while (*p != ']')
          if (*p++ == '\0')

            return pattern;
      }
    else if ((*p == '?' || *p == '*' || *p == '+' || *p == '@'
              || *p == '!') && p[1] == '(')
      p = end_pattern (p + 1);
    else if (*p == ')')
      break;

  return p + 1;
}


static int

ext_match (int opt, const char *pattern, const char *string, const char *string_end,
     _Bool no_leading_period, int flags)
{
  const char *startp;
  size_t level;
  struct patternlist
  {
    struct patternlist *next;
    char str[1];
  } *list = ((void *)0);
  struct patternlist **lastp = &list;
  size_t pattern_len = strlen (pattern);
  const char *p;
  const char *rs;
  enum { ALLOCA_LIMIT = 8000 };


  level = 0;
  for (startp = p = pattern + 1; ; ++p)
    if (*p == '\0')

      return -1;
    else if (*p == '[')
      {

        if (posixly_correct == 0)
          posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;



        if (*++p == '!' || (posixly_correct < 0 && *p == '^'))
          ++p;

        if (*p == ']')
          ++p;

        while (*p != ']')
          if (*p++ == '\0')

            return -1;
      }
    else if ((*p == '?' || *p == '*' || *p == '+' || *p == '@'
              || *p == '!') && p[1] == '(')

      ++level;
    else if (*p == ')')
      {
        if (level-- == 0)
          {
// # 1096 "fnmatch_loop.c"
            struct patternlist *newp; size_t plen; size_t plensize; size_t newpsize; plen = (opt == '?' || opt == '@' ? pattern_len : p - startp + 1UL); plensize = plen * sizeof (char); newpsize = __builtin_offsetof (struct patternlist, str) + plensize; if ((size_t) -1 / sizeof (char) < plen || newpsize < __builtin_offsetof (struct patternlist, str) || ALLOCA_LIMIT <= newpsize) return -1; newp = (struct patternlist *) __builtin_alloca (newpsize); *((char *) mempcpy (newp->str, startp, p - startp)) = '\0'; newp->next = ((void *)0); *lastp = newp; lastp = &newp->next;
            break;
          }
      }
    else if (*p == '|')
      {
        if (level == 0)
          {
            struct patternlist *newp; size_t plen; size_t plensize; size_t newpsize; plen = (opt == '?' || opt == '@' ? pattern_len : p - startp + 1UL); plensize = plen * sizeof (char); newpsize = __builtin_offsetof (struct patternlist, str) + plensize; if ((size_t) -1 / sizeof (char) < plen || newpsize < __builtin_offsetof (struct patternlist, str) || ALLOCA_LIMIT <= newpsize) return -1; newp = (struct patternlist *) __builtin_alloca (newpsize); *((char *) mempcpy (newp->str, startp, p - startp)) = '\0'; newp->next = ((void *)0); *lastp = newp; lastp = &newp->next;
            startp = p + 1;
          }
      }
  ((list != ((void *)0)) ? (void) (0) : (__assert_fail ("list != ((void *)0)", "fnmatch_loop.c", 1108, __PRETTY_FUNCTION__), (void) (0)));
  ((p[-1] == ')') ? (void) (0) : (__assert_fail ("p[-1] == ')'", "fnmatch_loop.c", 1109, __PRETTY_FUNCTION__), (void) (0)));


  switch (opt)
    {
    case '*':
      if (internal_fnmatch (p, string, string_end, no_leading_period, flags) == 0)
        return 0;


    case '+':
      do
        {
          for (rs = string; rs <= string_end; ++rs)


            if (internal_fnmatch (list->str, string, rs, no_leading_period,
                     flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0


                && (internal_fnmatch (p, rs, string_end,
                         rs == string
                         ? no_leading_period
                         : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                         flags & (1 << 0)
                         ? flags : flags & ~(1 << 2)) == 0

                    || (rs != string
                        && internal_fnmatch (pattern - 1, rs, string_end,
                                rs == string
                                ? no_leading_period
                                : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                                flags & (1 << 0)
                                ? flags : flags & ~(1 << 2)) == 0)))

              return 0;
        }
      while ((list = list->next) != ((void *)0));


      return 1;

    case '?':
      if (internal_fnmatch (p, string, string_end, no_leading_period, flags) == 0)
        return 0;


    case '@':
      do




        if (internal_fnmatch (strcat (list->str, p), string, string_end,
                 no_leading_period,
                 flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0)

          return 0;
      while ((list = list->next) != ((void *)0));


      return 1;

    case '!':
      for (rs = string; rs <= string_end; ++rs)
        {
          struct patternlist *runp;

          for (runp = list; runp != ((void *)0); runp = runp->next)
            if (internal_fnmatch (runp->str, string, rs, no_leading_period,
                     flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0)
              break;


          if (runp == ((void *)0)
              && (internal_fnmatch (p, rs, string_end,
                       rs == string
                       ? no_leading_period
                       : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                       flags & (1 << 0) ? flags : flags & ~(1 << 2))
                  == 0))

            return 0;
        }



      return 1;

    default:
      ((! "Invalid extended matching operator") ? (void) (0) : (__assert_fail ("! \"Invalid extended matching operator\"", "fnmatch_loop.c", 1199, __PRETTY_FUNCTION__), (void) (0)));
      break;
    }

  return -1;
}
// # 171 "fnmatch.c" 2
// # 206 "fnmatch.c"
static wctype_t
is_char_class (const wchar_t *wcs)
{
  char s[256 + 1];
  char *cp = s;

  do
    {






      switch (*wcs)
        {
        case L' ': case L'!': case L'"': case L'#': case L'%':
        case L'&': case L'\'': case L'(': case L')': case L'*':
        case L'+': case L',': case L'-': case L'.': case L'/':
        case L'0': case L'1': case L'2': case L'3': case L'4':
        case L'5': case L'6': case L'7': case L'8': case L'9':
        case L':': case L';': case L'<': case L'=': case L'>':
        case L'?':
        case L'A': case L'B': case L'C': case L'D': case L'E':
        case L'F': case L'G': case L'H': case L'I': case L'J':
        case L'K': case L'L': case L'M': case L'N': case L'O':
        case L'P': case L'Q': case L'R': case L'S': case L'T':
        case L'U': case L'V': case L'W': case L'X': case L'Y':
        case L'Z':
        case L'[': case L'\\': case L']': case L'^': case L'_':
        case L'a': case L'b': case L'c': case L'd': case L'e':
        case L'f': case L'g': case L'h': case L'i': case L'j':
        case L'k': case L'l': case L'm': case L'n': case L'o':
        case L'p': case L'q': case L'r': case L's': case L't':
        case L'u': case L'v': case L'w': case L'x': case L'y':
        case L'z': case L'{': case L'|': case L'}': case L'~':
          break;
        default:
          return (wctype_t) 0;
        }



      if (cp == s + 256)
        return (wctype_t) 0;

      *cp++ = (char) *wcs++;
    }
  while (*wcs != L'\0');

  *cp = '\0';




  return wctype (s);

}


// # 1 "fnmatch_loop.c" 1
// # 19 "fnmatch_loop.c"
static int ext_wmatch (wint_t opt, const wchar_t *pattern, const wchar_t *string,
                const wchar_t *string_end, _Bool no_leading_period, int flags)
     ;
static const wchar_t *end_wpattern (const wchar_t *patternp) ;

static int

internal_fnwmatch (const wchar_t *pattern, const wchar_t *string, const wchar_t *string_end,
     _Bool no_leading_period, int flags)
{
  register const wchar_t *p = pattern, *n = string;
  register wint_t c;
// # 41 "fnmatch_loop.c"
  while ((c = *p++) != L'\0')
    {
      _Bool new_no_leading_period = 0;
      c = ((flags & (1 << 4)) ? towlower (c) : (c));

      switch (c)
        {
        case L'?':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_wmatch (c, p, n, string_end, no_leading_period,
                         flags);
              if (res != -1)
                return res;
            }

          if (n == string_end)
            return 1;
          else if (*n == L'/' && (flags & (1 << 0)))
            return 1;
          else if (*n == L'.' && no_leading_period)
            return 1;
          break;

        case L'\\':
          if (!(flags & (1 << 1)))
            {
              c = *p++;
              if (c == L'\0')

                return 1;
              c = ((flags & (1 << 4)) ? towlower (c) : (c));
            }
          if (n == string_end || ((flags & (1 << 4)) ? towlower ((wint_t) *n) : ((wint_t) *n)) != c)
            return 1;
          break;

        case L'*':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_wmatch (c, p, n, string_end, no_leading_period,
                         flags);
              if (res != -1)
                return res;
            }

          if (n != string_end && *n == L'.' && no_leading_period)
            return 1;

          for (c = *p++; c == L'?' || c == L'*'; c = *p++)
            {
              if (*p == L'(' && (flags & (1 << 5)) != 0)
                {
                  const wchar_t *endp = end_wpattern (p);
                  if (endp != p)
                    {

                      p = endp;
                      continue;
                    }
                }

              if (c == L'?')
                {

                  if (n == string_end)

                    return 1;
                  else if (*n == L'/'
                           && __builtin_expect (flags & (1 << 0), 0))


                    return 1;
                  else



                    ++n;
                }
            }

          if (c == L'\0')




            {
              int result = (flags & (1 << 0)) == 0 ? 0 : 1;

              if (flags & (1 << 0))
                {
                  if (flags & (1 << 3))
                    result = 0;
                  else
                    {
                      if (wmemchr (n, L'/', string_end - n) == ((void *)0))
                        result = 0;
                    }
                }

              return result;
            }
          else
            {
              const wchar_t *endp;

              endp = wmemchr (n, (flags & (1 << 0)) ? L'/' : L'\0', string_end - n);

              if (endp == ((void *)0))
                endp = string_end;

              if (c == L'['
                  || (__builtin_expect (flags & (1 << 5), 0) != 0
                      && (c == L'@' || c == L'+' || c == L'!')
                      && *p == L'('))
                {
                  int flags2 = ((flags & (1 << 0))
                                ? flags : (flags & ~(1 << 2)));
                  _Bool no_leading_period2 = no_leading_period;

                  for (--p; n < endp; ++n, no_leading_period2 = 0)
                    if (internal_fnwmatch (p, n, string_end, no_leading_period2, flags2)
                        == 0)
                      return 0;
                }
              else if (c == L'/' && (flags & (1 << 0)))
                {
                  while (n < string_end && *n != L'/')
                    ++n;
                  if (n < string_end && *n == L'/'
                      && (internal_fnwmatch (p, n + 1, string_end, flags & (1 << 2), flags)
                          == 0))
                    return 0;
                }
              else
                {
                  int flags2 = ((flags & (1 << 0))
                                ? flags : (flags & ~(1 << 2)));
                  int no_leading_period2 = no_leading_period;

                  if (c == L'\\' && !(flags & (1 << 1)))
                    c = *p;
                  c = ((flags & (1 << 4)) ? towlower (c) : (c));
                  for (--p; n < endp; ++n, no_leading_period2 = 0)
                    if (((flags & (1 << 4)) ? towlower ((wint_t) *n) : ((wint_t) *n)) == c
                        && (internal_fnwmatch (p, n, string_end, no_leading_period2, flags2)
                            == 0))
                      return 0;
                }
            }


          return 1;

        case L'[':
          {

            const wchar_t *p_init = p;
            const wchar_t *n_init = n;
            register _Bool not;
            wchar_t cold;
            wint_t fn;

            if (posixly_correct == 0)
              posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;

            if (n == string_end)
              return 1;

            if (*n == L'.' && no_leading_period)
              return 1;

            if (*n == L'/' && (flags & (1 << 0)))

              return 1;

            not = (*p == L'!' || (posixly_correct < 0 && *p == L'^'));
            if (not)
              ++p;

            fn = ((flags & (1 << 4)) ? towlower ((wint_t) *n) : ((wint_t) *n));

            c = *p++;
            for (;;)
              {
                if (!(flags & (1 << 1)) && c == L'\\')
                  {
                    if (*p == L'\0')
                      return 1;
                    c = ((flags & (1 << 4)) ? towlower ((wint_t) *p) : ((wint_t) *p));
                    ++p;

                    goto normal_bracket;
                  }
                else if (c == L'[' && *p == L':')
                  {

                    wchar_t str[256 + 1];
                    size_t c1 = 0;

                    wctype_t wt;

                    const wchar_t *startp = p;

                    for (;;)
                      {
                        if (c1 == 256)


                          return 1;

                        c = *++p;
                        if (c == L':' && p[1] == L']')
                          {
                            p += 2;
                            break;
                          }
                        if (c < L'a' || c >= L'z')
                          {


                            p = startp;
                            c = L'[';
                            goto normal_bracket;
                          }
                        str[c1++] = c;
                      }
                    str[c1] = L'\0';


                    wt = is_char_class (str);
                    if (wt == 0)

                      return 1;
// # 287 "fnmatch_loop.c"
                    if (iswctype (((wint_t) *n), wt))
                      goto matched;
// # 305 "fnmatch_loop.c"
                    c = *p++;
                  }
// # 413 "fnmatch_loop.c"
                else if (c == L'\0')
                  {

                    p = p_init;
                    n = n_init;
                    c = L'[';
                    goto normal_match;
                  }
                else
                  {
                    _Bool is_range = 0;
// # 599 "fnmatch_loop.c"
                      {
                        c = ((flags & (1 << 4)) ? towlower (c) : (c));
                      normal_bracket:




                        is_range = (*p == L'-' && p[1] != L'\0'
                                    && p[1] != L']');

                        if (!is_range && c == fn)
                          goto matched;







                        cold = c;
                        c = *p++;
                      }

                    if (c == L'-' && *p != L']')
                      {
// # 843 "fnmatch_loop.c"
                        wint_t cend = *p++;

                        if (!(flags & (1 << 1)) && cend == L'\\')
                          cend = *p++;
                        if (cend == L'\0')
                          return 1;


                        if (cold <= fn && fn <= cend)
                          goto matched;


                        c = *p++;
                      }
                  }

                if (c == L']')
                  break;
              }

            if (!not)
              return 1;
            break;

          matched:

            do
              {
              ignore_next:
                c = *p++;

                if (c == L'\0')

                  return 1;

                if (!(flags & (1 << 1)) && c == L'\\')
                  {
                    if (*p == L'\0')
                      return 1;

                    ++p;
                  }
                else if (c == L'[' && *p == L':')
                  {
                    int c1 = 0;
                    const wchar_t *startp = p;

                    while (1)
                      {
                        c = *++p;
                        if (++c1 == 256)
                          return 1;

                        if (*p == L':' && p[1] == L']')
                          break;

                        if (c < L'a' || c >= L'z')
                          {
                            p = startp;
                            goto ignore_next;
                          }
                      }
                    p += 2;
                    c = *p++;
                  }
                else if (c == L'[' && *p == L'=')
                  {
                    c = *++p;
                    if (c == L'\0')
                      return 1;
                    c = *++p;
                    if (c != L'=' || p[1] != L']')
                      return 1;
                    p += 2;
                    c = *p++;
                  }
                else if (c == L'[' && *p == L'.')
                  {
                    ++p;
                    while (1)
                      {
                        c = *++p;
                        if (c == '\0')
                          return 1;

                        if (*p == L'.' && p[1] == L']')
                          break;
                      }
                    p += 2;
                    c = *p++;
                  }
              }
            while (c != L']');
            if (not)
              return 1;
          }
          break;

        case L'+':
        case L'@':
        case L'!':
          if (__builtin_expect (flags & (1 << 5), 0) && *p == '(')
            {
              int res;

              res = ext_wmatch (c, p, n, string_end, no_leading_period, flags);
              if (res != -1)
                return res;
            }
          goto normal_match;

        case L'/':
          if (((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))))
            {
              if (n == string_end || c != (wint_t) *n)
                return 1;

              new_no_leading_period = 1;
              break;
            }

        default:
        normal_match:
          if (n == string_end || c != ((flags & (1 << 4)) ? towlower ((wint_t) *n) : ((wint_t) *n)))
            return 1;
        }

      no_leading_period = new_no_leading_period;
      ++n;
    }

  if (n == string_end)
    return 0;

  if ((flags & (1 << 3)) && n != string_end && *n == L'/')

    return 0;

  return 1;
}


static const wchar_t *

end_wpattern (const wchar_t *pattern)
{
  const wchar_t *p = pattern;

  while (1)
    if (*++p == L'\0')

      return pattern;
    else if (*p == L'[')
      {

        if (posixly_correct == 0)
          posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;



        if (*++p == L'!' || (posixly_correct < 0 && *p == L'^'))
          ++p;

        if (*p == L']')
          ++p;

        while (*p != L']')
          if (*p++ == L'\0')

            return pattern;
      }
    else if ((*p == L'?' || *p == L'*' || *p == L'+' || *p == L'@'
              || *p == L'!') && p[1] == L'(')
      p = end_wpattern (p + 1);
    else if (*p == L')')
      break;

  return p + 1;
}


static int

ext_wmatch (wint_t opt, const wchar_t *pattern, const wchar_t *string, const wchar_t *string_end,
     _Bool no_leading_period, int flags)
{
  const wchar_t *startp;
  size_t level;
  struct patternlist
  {
    struct patternlist *next;
    wchar_t str[1];
  } *list = ((void *)0);
  struct patternlist **lastp = &list;
  size_t pattern_len = wcslen (pattern);
  const wchar_t *p;
  const wchar_t *rs;
  enum { ALLOCA_LIMIT = 8000 };


  level = 0;
  for (startp = p = pattern + 1; ; ++p)
    if (*p == L'\0')

      return -1;
    else if (*p == L'[')
      {

        if (posixly_correct == 0)
          posixly_correct = getenv ("POSIXLY_CORRECT") != ((void *)0) ? 1 : -1;



        if (*++p == L'!' || (posixly_correct < 0 && *p == L'^'))
          ++p;

        if (*p == L']')
          ++p;

        while (*p != L']')
          if (*p++ == L'\0')

            return -1;
      }
    else if ((*p == L'?' || *p == L'*' || *p == L'+' || *p == L'@'
              || *p == L'!') && p[1] == L'(')

      ++level;
    else if (*p == L')')
      {
        if (level-- == 0)
          {
// # 1096 "fnmatch_loop.c"
            struct patternlist *newp; size_t plen; size_t plensize; size_t newpsize; plen = (opt == L'?' || opt == L'@' ? pattern_len : p - startp + 1UL); plensize = plen * sizeof (wchar_t); newpsize = __builtin_offsetof (struct patternlist, str) + plensize; if ((size_t) -1 / sizeof (wchar_t) < plen || newpsize < __builtin_offsetof (struct patternlist, str) || ALLOCA_LIMIT <= newpsize) return -1; newp = (struct patternlist *) __builtin_alloca (newpsize); *((wchar_t *) wmempcpy (newp->str, startp, p - startp)) = L'\0'; newp->next = ((void *)0); *lastp = newp; lastp = &newp->next;
            break;
          }
      }
    else if (*p == L'|')
      {
        if (level == 0)
          {
            struct patternlist *newp; size_t plen; size_t plensize; size_t newpsize; plen = (opt == L'?' || opt == L'@' ? pattern_len : p - startp + 1UL); plensize = plen * sizeof (wchar_t); newpsize = __builtin_offsetof (struct patternlist, str) + plensize; if ((size_t) -1 / sizeof (wchar_t) < plen || newpsize < __builtin_offsetof (struct patternlist, str) || ALLOCA_LIMIT <= newpsize) return -1; newp = (struct patternlist *) __builtin_alloca (newpsize); *((wchar_t *) wmempcpy (newp->str, startp, p - startp)) = L'\0'; newp->next = ((void *)0); *lastp = newp; lastp = &newp->next;
            startp = p + 1;
          }
      }
  ((list != ((void *)0)) ? (void) (0) : (__assert_fail ("list != ((void *)0)", "fnmatch_loop.c", 1108, __PRETTY_FUNCTION__), (void) (0)));
  ((p[-1] == L')') ? (void) (0) : (__assert_fail ("p[-1] == L')'", "fnmatch_loop.c", 1109, __PRETTY_FUNCTION__), (void) (0)));


  switch (opt)
    {
    case L'*':
      if (internal_fnwmatch (p, string, string_end, no_leading_period, flags) == 0)
        return 0;


    case L'+':
      do
        {
          for (rs = string; rs <= string_end; ++rs)


            if (internal_fnwmatch (list->str, string, rs, no_leading_period,
                     flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0


                && (internal_fnwmatch (p, rs, string_end,
                         rs == string
                         ? no_leading_period
                         : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                         flags & (1 << 0)
                         ? flags : flags & ~(1 << 2)) == 0

                    || (rs != string
                        && internal_fnwmatch (pattern - 1, rs, string_end,
                                rs == string
                                ? no_leading_period
                                : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                                flags & (1 << 0)
                                ? flags : flags & ~(1 << 2)) == 0)))

              return 0;
        }
      while ((list = list->next) != ((void *)0));


      return 1;

    case L'?':
      if (internal_fnwmatch (p, string, string_end, no_leading_period, flags) == 0)
        return 0;


    case L'@':
      do




        if (internal_fnwmatch (wcscat (list->str, p), string, string_end,
                 no_leading_period,
                 flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0)

          return 0;
      while ((list = list->next) != ((void *)0));


      return 1;

    case L'!':
      for (rs = string; rs <= string_end; ++rs)
        {
          struct patternlist *runp;

          for (runp = list; runp != ((void *)0); runp = runp->next)
            if (internal_fnwmatch (runp->str, string, rs, no_leading_period,
                     flags & (1 << 0) ? flags : flags & ~(1 << 2)) == 0)
              break;


          if (runp == ((void *)0)
              && (internal_fnwmatch (p, rs, string_end,
                       rs == string
                       ? no_leading_period
                       : rs[-1] == '/' && ((flags & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2))),
                       flags & (1 << 0) ? flags : flags & ~(1 << 2))
                  == 0))

            return 0;
        }



      return 1;

    default:
      ((! "Invalid extended matching operator") ? (void) (0) : (__assert_fail ("! \"Invalid extended matching operator\"", "fnmatch_loop.c", 1199, __PRETTY_FUNCTION__), (void) (0)));
      break;
    }

  return -1;
}
// # 267 "fnmatch.c" 2



int
posix_fnmatch (const char *pattern, const char *string, int flags)
{


  if (__builtin_expect ((__ctype_get_mb_cur_max ()), 1) != 1)
    {
      mbstate_t ps;
      size_t patsize;
      size_t strsize;
      size_t totsize;
      wchar_t *wpattern;
      wchar_t *wstring;
      int res;



      memset (&ps, '\0', sizeof (ps));
      patsize = mbsrtowcs (((void *)0), &pattern, 0, &ps) + 1;
      if (__builtin_expect (patsize != 0, 1))
        {
          ((mbsinit (&ps)) ? (void) (0) : (__assert_fail ("mbsinit (&ps)", "fnmatch.c", 291, __PRETTY_FUNCTION__), (void) (0)));
          strsize = mbsrtowcs (((void *)0), &string, 0, &ps) + 1;
          if (__builtin_expect (strsize != 0, 1))
            {
              ((mbsinit (&ps)) ? (void) (0) : (__assert_fail ("mbsinit (&ps)", "fnmatch.c", 295, __PRETTY_FUNCTION__), (void) (0)));
              totsize = patsize + strsize;
              if (__builtin_expect (! (patsize <= totsize
                                       && totsize <= ((size_t) -1) / sizeof (wchar_t)),
                                    0))
                {
                  (*__errno_location ()) = 12;
                  return -1;
                }


              if (__builtin_expect (totsize < 2000, 1))
                wpattern = (wchar_t *) __builtin_alloca (totsize * sizeof (wchar_t));
              else
                {
                  wpattern = malloc (totsize * sizeof (wchar_t));
                  if (__builtin_expect (! wpattern, 0))
                    {
                      (*__errno_location ()) = 12;
                      return -1;
                    }
                }
              wstring = wpattern + patsize;


              mbsrtowcs (wpattern, &pattern, patsize, &ps);
              ((mbsinit (&ps)) ? (void) (0) : (__assert_fail ("mbsinit (&ps)", "fnmatch.c", 321, __PRETTY_FUNCTION__), (void) (0)));
              mbsrtowcs (wstring, &string, strsize, &ps);

              res = internal_fnwmatch (wpattern, wstring, wstring + strsize - 1,
                                       flags & (1 << 2), flags);

              if (__builtin_expect (! (totsize < 2000), 0))
                free (wpattern);
              return res;
            }
        }
    }



  return internal_fnmatch (pattern, string, string + strlen (string),
                           flags & (1 << 2), flags);
}
