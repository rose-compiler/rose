// # 1 "mail_run.c"
// # 1 "/home/dquinlan/ROSE/PONTETEC/postfix-2.9.4/src/global//"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "mail_run.c"
// # 59 "mail_run.c"
// # 1 "../../include/sys_defs.h" 1
// # 748 "../../include/sys_defs.h"
// # 1 "/usr/include/sys/types.h" 1 3 4
// # 27 "/usr/include/sys/types.h" 3 4
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
// # 28 "/usr/include/sys/types.h" 2 3 4



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
// # 32 "/usr/include/sys/types.h" 2 3 4



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



// # 749 "../../include/sys_defs.h" 2
// # 796 "../../include/sys_defs.h"
// # 1 "/usr/include/linux/version.h" 1 3 4
// # 797 "../../include/sys_defs.h" 2
// # 1426 "../../include/sys_defs.h"
typedef int WAIT_STATUS_T;
// # 1495 "../../include/sys_defs.h"
extern int closefrom(int);
// # 1632 "../../include/sys_defs.h"
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
// # 1633 "../../include/sys_defs.h" 2
// # 1702 "../../include/sys_defs.h"
extern int REMOVE(const char *);
// # 60 "mail_run.c" 2
// # 1 "/usr/include/sys/wait.h" 1 3 4
// # 29 "/usr/include/sys/wait.h" 3 4


// # 1 "/usr/include/signal.h" 1 3 4
// # 31 "/usr/include/signal.h" 3 4


// # 1 "/usr/include/bits/sigset.h" 1 3 4
// # 103 "/usr/include/bits/sigset.h" 3 4
extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
// # 117 "/usr/include/bits/sigset.h" 3 4
extern __inline int __sigismember (__const __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return (__set->__val[__word] & __mask) ? 1 : 0; }
extern __inline int __sigaddset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] |= __mask), 0); }
extern __inline int __sigdelset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] &= ~__mask), 0); }
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







// # 1 "/usr/include/time.h" 1 3 4
// # 210 "/usr/include/signal.h" 2 3 4


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


// # 33 "/usr/include/sys/wait.h" 2 3 4





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




// # 61 "mail_run.c" 2
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

// # 62 "mail_run.c" 2
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

// # 63 "mail_run.c" 2



// # 1 "../../include/msg.h" 1
// # 17 "../../include/msg.h"
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 43 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
// # 105 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
// # 18 "../../include/msg.h" 2
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

// # 19 "../../include/msg.h" 2




typedef void (*MSG_CLEANUP_FN) (void);

extern int msg_verbose;

extern void __attribute__ ((format (printf, (1), (2)))) msg_info(const char *,...);
extern void __attribute__ ((format (printf, (1), (2)))) msg_warn(const char *,...);
extern void __attribute__ ((format (printf, (1), (2)))) msg_error(const char *,...);
extern void __attribute__((__noreturn__)) __attribute__ ((format (printf, (1), (2)))) msg_fatal(const char *,...);
extern void __attribute__((__noreturn__)) __attribute__ ((format (printf, (2), (3)))) msg_fatal_status(int, const char *,...);
extern void __attribute__((__noreturn__)) __attribute__ ((format (printf, (1), (2)))) msg_panic(const char *,...);

extern void vmsg_info(const char *, va_list);
extern void vmsg_warn(const char *, va_list);
extern void vmsg_error(const char *, va_list);
extern void __attribute__((__noreturn__)) vmsg_fatal(const char *, va_list);
extern void __attribute__((__noreturn__)) vmsg_fatal_status(int, const char *, va_list);
extern void __attribute__((__noreturn__)) vmsg_panic(const char *, va_list);

extern int msg_error_limit(int);
extern void msg_error_clear(void);
extern MSG_CLEANUP_FN msg_cleanup(MSG_CLEANUP_FN);

extern void __attribute__ ((format (printf, (4), (5)))) msg_rate_delay(time_t *, int,
              void (*log_fn) (const char *,...),
                 const char *,...);
// # 67 "mail_run.c" 2
// # 1 "../../include/stringops.h" 1
// # 17 "../../include/stringops.h"
// # 1 "../../include/vstring.h" 1
// # 22 "../../include/vstring.h"
// # 1 "../../include/vbuf.h" 1
// # 32 "../../include/vbuf.h"
typedef struct VBUF VBUF;
typedef int (*VBUF_GET_READY_FN) (VBUF *);
typedef int (*VBUF_PUT_READY_FN) (VBUF *);
typedef int (*VBUF_SPACE_FN) (VBUF *, ssize_t);

struct VBUF {
    int flags;
    unsigned char *data;
    ssize_t len;
    ssize_t cnt;
    unsigned char *ptr;
    VBUF_GET_READY_FN get_ready;
    VBUF_PUT_READY_FN put_ready;
    VBUF_SPACE_FN space;
};
// # 93 "../../include/vbuf.h"
extern int vbuf_get(VBUF *);
extern int vbuf_put(VBUF *, int);
extern int vbuf_unget(VBUF *, int);
extern ssize_t vbuf_read(VBUF *, char *, ssize_t);
extern ssize_t vbuf_write(VBUF *, const char *, ssize_t);
// # 23 "../../include/vstring.h" 2





typedef struct VSTRING {
    VBUF vbuf;
    ssize_t maxlen;
} VSTRING;

extern VSTRING *vstring_alloc(ssize_t);
extern void vstring_ctl(VSTRING *,...);
extern VSTRING *vstring_truncate(VSTRING *, ssize_t);
extern VSTRING *vstring_free(VSTRING *);
extern VSTRING *vstring_strcpy(VSTRING *, const char *);
extern VSTRING *vstring_strncpy(VSTRING *, const char *, ssize_t);
extern VSTRING *vstring_strcat(VSTRING *, const char *);
extern VSTRING *vstring_strncat(VSTRING *, const char *, ssize_t);
extern VSTRING *vstring_memcpy(VSTRING *, const char *, ssize_t);
extern VSTRING *vstring_memcat(VSTRING *, const char *, ssize_t);
extern char *vstring_memchr(VSTRING *, int);
extern VSTRING *vstring_insert(VSTRING *, ssize_t, const char *, ssize_t);
extern VSTRING *vstring_prepend(VSTRING *, const char *, ssize_t);
extern VSTRING *__attribute__ ((format (printf, (2), (3)))) vstring_sprintf(VSTRING *, const char *,...);
extern VSTRING *__attribute__ ((format (printf, (2), (3)))) vstring_sprintf_append(VSTRING *, const char *,...);
extern VSTRING *__attribute__ ((format (printf, (2), (3)))) vstring_sprintf_prepend(VSTRING *, const char *, ...);
extern char *vstring_export(VSTRING *);
extern VSTRING *vstring_import(char *);
// # 87 "../../include/vstring.h"
extern VSTRING *vstring_vsprintf(VSTRING *, const char *, va_list);
extern VSTRING *vstring_vsprintf_append(VSTRING *, const char *, va_list);
// # 18 "../../include/stringops.h" 2




extern char *printable(char *, int);
extern char *neuter(char *, const char *, int);
extern char *lowercase(char *);
extern char *uppercase(char *);
extern char *skipblanks(const char *);
extern char *trimblanks(char *, int);
extern char *concatenate(const char *,...);
extern char *mystrtok(char **, const char *);
extern char *translit(char *, const char *, const char *);


extern char *postfix_basename(const char *);

extern char *sane_basename(VSTRING *, const char *);
extern char *sane_dirname(VSTRING *, const char *);
extern VSTRING *unescape(VSTRING *, const char *);
extern VSTRING *escape(VSTRING *, const char *, ssize_t);
extern int alldig(const char *);
extern int allprint(const char *);
extern int allspace(const char *);
extern int allascii(const char *);
extern const char *split_nameval(char *, char **, char **);
extern int valid_utf_8(const char *, ssize_t);
// # 68 "mail_run.c" 2
// # 1 "../../include/mymalloc.h" 1
// # 17 "../../include/mymalloc.h"
extern char *mymalloc(ssize_t);
extern char *myrealloc(char *, ssize_t);
extern void myfree(char *);
extern char *mystrdup(const char *);
extern char *mystrndup(const char *, ssize_t);
extern char *mymemdup(const char *, ssize_t);
// # 69 "mail_run.c" 2



// # 1 "mail_params.h" 1
// # 17 "mail_params.h"
typedef int bool;






extern char *var_mail_name;






extern bool var_helpful_warnings;






extern bool var_show_unk_rcpt_table;
// # 50 "mail_params.h"
extern char *var_notify_classes;






extern char *var_empty_addr;







extern char *var_mail_owner;
extern uid_t var_owner_uid;
extern gid_t var_owner_gid;



extern char *var_sgid_group;
extern gid_t var_sgid_gid;



extern char *var_default_privs;
extern uid_t var_default_uid;
extern gid_t var_default_gid;
// # 87 "mail_params.h"
extern char *var_flush_acl;



extern char *var_showq_acl;



extern char *var_submit_acl;







extern char *var_myorigin;







extern char *var_mydest;





extern char *var_myhostname;



extern char *var_mydomain;






extern char *var_local_transport;






extern char *var_bounce_rcpt;



extern char *var_2bounce_rcpt;



extern char *var_delay_rcpt;



extern char *var_error_rcpt;
// # 156 "mail_params.h"
extern char *var_inet_interfaces;



extern char *var_proxy_interfaces;






extern char *var_masq_domains;



extern char *var_masq_exceptions;
// # 182 "mail_params.h"
extern char *var_masq_classes;






extern char *var_relayhost;



extern char *var_snd_relay_maps;



extern char *var_null_relay_maps_key;





extern char *var_fallback_relay;



extern bool var_disable_dns;
// # 216 "mail_params.h"
extern char *var_smtp_host_lookup;
// # 225 "mail_params.h"
extern char *var_smtp_dns_res_opt;





extern int var_smtp_mxaddr_limit;





extern int var_smtp_mxsess_limit;
// # 246 "mail_params.h"
extern char *var_queue_dir;
// # 255 "mail_params.h"
extern char *var_daemon_dir;





extern char *var_command_dir;
// # 270 "mail_params.h"
extern char *var_pid_dir;
// # 279 "mail_params.h"
extern char *var_data_dir;




extern time_t var_starttime;
// # 293 "mail_params.h"
extern char *var_config_dir;



extern char *var_config_dirs;
// # 307 "mail_params.h"
extern char *var_db_type;







extern char *var_syslog_facility;
// # 331 "mail_params.h"
extern char *var_always_bcc;
// # 342 "mail_params.h"
extern char *var_rcpt_witheld;







extern bool var_always_add_hdrs;







extern bool var_strict_rfc821_env;







extern bool var_broken_auth_clients;






extern bool var_disable_vrfy_cmd;






extern char *var_virt_alias_maps;



extern char *var_virt_alias_doms;



extern int var_virt_alias_code;



extern char *var_canonical_maps;



extern char *var_send_canon_maps;



extern char *var_rcpt_canon_maps;
// # 412 "mail_params.h"
extern char *var_canon_classes;




extern char *var_send_canon_classes;




extern char *var_rcpt_canon_classes;



extern char *var_send_bcc_maps;



extern char *var_rcpt_bcc_maps;



extern char *var_transport_maps;



extern char *var_def_transport;



extern char *var_snd_def_xport_maps;



extern char *var_null_def_xport_maps_key;






extern bool var_swap_bangpath;



extern bool var_append_at_myorigin;



extern bool var_append_dot_mydomain;



extern bool var_percent_hack;
// # 476 "mail_params.h"
extern char *var_alias_maps;






extern bool var_biff;






extern char *var_allow_commands;




extern int var_command_maxtime;



extern char *var_allow_files;



extern char *var_local_cmd_shell;



extern char *var_alias_db_map;



extern char *var_luser_relay;






extern char *var_mail_spool_dir;



extern char *var_home_mailbox;



extern char *var_mailbox_command;



extern char *var_mailbox_cmd_maps;



extern char *var_mailbox_transport;



extern char *var_mbox_transp_maps;



extern char *var_fallback_transport;



extern char *var_fbck_transp_maps;






extern char *var_forward_path;






extern char *var_exec_directory;





extern char *var_exec_exp_filter;





extern char *var_mailbox_lock;







extern long var_mailbox_limit;






extern char *var_prop_extension;



extern char *var_rcpt_delim;





extern char *var_cmd_exp_filter;





extern char *var_fwd_exp_filter;



extern char *var_deliver_hdr;
// # 617 "mail_params.h"
extern bool var_enable_orcpt;



extern bool var_exp_own_alias;



extern bool var_stat_home_dir;







extern int var_dup_filter_limit;



extern bool var_tls_append_def_CA;



extern char *var_tls_rand_exch_name;







extern char *var_tls_rand_source;



extern int var_tls_rand_bytes;



extern int var_tls_daemon_rand_bytes;



extern int var_tls_reseed_period;



extern int var_tls_prng_upd_period;






extern char *var_relocated_maps;
// # 685 "mail_params.h"
extern int var_min_backoff_time;



extern int var_max_backoff_time;



extern int var_max_queue_time;







extern int var_dsn_queue_time;



extern int var_delay_warn_time;






extern int var_qmgr_active_limit;



extern int var_qmgr_rcpt_limit;



extern int var_qmgr_msg_rcpt_limit;




extern int var_xport_rcpt_limit;




extern int var_stack_rcpt_limit;




extern int var_xport_refill_limit;




extern int var_xport_refill_delay;







extern int var_delivery_slot_cost;




extern int var_delivery_slot_loan;




extern int var_delivery_slot_discount;




extern int var_min_delivery_slots;



extern int var_qmgr_fudge;







extern int var_init_dest_concurrency;




extern int var_dest_con_limit;



extern int var_local_con_lim;







extern int var_dest_rcpt_limit;



extern int var_local_rcpt_lim;






extern int var_transport_retry_time;






extern char *var_defer_xports;







extern int var_qmgr_clog_warn_time;






extern int var_proc_limit;






extern int var_throttle_time;






extern char *var_master_disable;







extern int var_use_limit;







extern int var_idle_limit;







extern int var_event_drain;







extern int var_ipc_idle_limit;
// # 880 "mail_params.h"
extern int var_ipc_ttl_limit;







extern int var_line_limit;






extern char *var_debug_peer_list;



extern int var_debug_peer_level;







extern char *var_hash_queue_names;



extern int var_hash_queue_depth;







extern bool var_long_queue_ids;
// # 929 "mail_params.h"
extern char *var_inet_protocols;
// # 942 "mail_params.h"
extern char *var_bestmx_transp;





extern int var_smtp_cache_conn;





extern int var_smtp_reuse_time;





extern char *var_smtp_cache_dest;
// # 970 "mail_params.h"
extern bool var_smtp_cache_demand;



extern int var_smtp_conn_tmout;





extern int var_smtp_helo_tmout;



extern int var_smtp_xfwd_tmout;





extern int var_smtp_starttls_tmout;



extern int var_smtp_mail_tmout;



extern int var_smtp_rcpt_tmout;



extern int var_smtp_data0_tmout;



extern int var_smtp_data1_tmout;



extern int var_smtp_data2_tmout;



extern int var_smtp_rset_tmout;



extern int var_smtp_quit_tmout;





extern int var_smtp_quote_821_env;





extern bool var_smtp_skip_5xx_greeting;



extern bool var_ign_mx_lookup_err;



extern bool var_skip_quit_resp;







extern bool var_smtp_always_ehlo;



extern bool var_smtp_never_ehlo;





extern char *var_smtp_resp_filter;





extern char *var_smtp_bind_addr;





extern char *var_smtp_bind_addr6;





extern char *var_smtp_helo_name;





extern bool var_smtp_rand_addr;





extern int var_smtp_line_limit;





extern int var_smtp_pix_thresh;





extern int var_smtp_pix_delay;
// # 1110 "mail_params.h"
extern char *var_smtp_pix_bug_words;





extern char *var_smtp_pix_bug_maps;





extern bool var_smtp_defer_mxaddr;



extern bool var_smtp_send_xforward;





extern char *var_smtp_generic_maps;
// # 1141 "mail_params.h"
extern char *var_smtpd_banner;



extern int var_smtpd_tmout;



extern int var_smtpd_starttls_tmout;



extern int var_smtpd_rcpt_limit;



extern int var_smtpd_soft_erlim;



extern int var_smtpd_hard_erlim;



extern int var_smtpd_err_sleep;



extern int var_smtpd_junk_cmd_limit;



extern int var_smtpd_rcpt_overlim;



extern int var_smtpd_hist_thrsh;



extern char *var_smtpd_noop_cmds;



extern char *var_smtpd_forbid_cmds;



extern char *var_smtpd_cmd_filter;



extern bool var_smtpd_tls_wrappermode;



extern char *var_smtpd_tls_level;



extern bool var_smtpd_use_tls;



extern bool var_smtpd_enforce_tls;



extern bool var_smtpd_tls_auth_only;



extern bool var_smtpd_tls_ask_ccert;



extern bool var_smtpd_tls_req_ccert;



extern int var_smtpd_tls_ccert_vd;



extern char *var_smtpd_tls_cert_file;



extern char *var_smtpd_tls_key_file;



extern char *var_smtpd_tls_dcert_file;



extern char *var_smtpd_tls_dkey_file;



extern char *var_smtpd_tls_eccert_file;



extern char *var_smtpd_tls_eckey_file;



extern char *var_smtpd_tls_CAfile;



extern char *var_smtpd_tls_CApath;



extern char *var_smtpd_tls_proto;



extern char *var_smtpd_tls_mand_proto;



extern char *var_smtpd_tls_ciph;



extern char *var_smtpd_tls_mand_ciph;



extern char *var_smtpd_tls_excl_ciph;



extern char *var_smtpd_tls_mand_excl;



extern char *var_smtpd_tls_fpt_dgst;



extern char *var_smtpd_tls_dh512_param_file;



extern char *var_smtpd_tls_dh1024_param_file;



extern char *var_smtpd_tls_eecdh;



extern char *var_smtpd_tls_loglevel;



extern bool var_smtpd_tls_received_header;



extern char *var_smtpd_tls_scache_db;



extern int var_smtpd_tls_scache_timeout;



extern bool var_smtpd_tls_set_sessid;



extern bool var_smtpd_delay_open;





extern char *var_smtp_tls_per_site;





extern bool var_smtp_use_tls;





extern bool var_smtp_enforce_tls;





extern bool var_smtp_tls_enforce_peername;





extern char *var_smtp_tls_level;





extern int var_smtp_tls_scert_vd;





extern char *var_smtp_tls_cert_file;





extern char *var_smtp_tls_key_file;





extern char *var_smtp_tls_dcert_file;





extern char *var_smtp_tls_dkey_file;





extern char *var_smtp_tls_eccert_file;





extern char *var_smtp_tls_eckey_file;





extern char *var_smtp_tls_CAfile;





extern char *var_smtp_tls_CApath;





extern char *var_smtp_tls_ciph;





extern char *var_smtp_tls_mand_ciph;





extern char *var_smtp_tls_excl_ciph;





extern char *var_smtp_tls_mand_excl;





extern char *var_smtp_tls_fpt_dgst;





extern char *var_smtp_tls_loglevel;
extern char *var_lmtp_tls_loglevel;





extern bool var_smtp_tls_note_starttls_offer;





extern char *var_smtp_tls_scache_db;
extern char *var_lmtp_tls_scache_db;





extern int var_smtp_tls_scache_timeout;
extern int var_lmtp_tls_scache_timeout;





extern char *var_smtp_tls_policy;





extern char *var_smtp_tls_proto;





extern char *var_smtp_tls_mand_proto;





extern char *var_smtp_tls_vfy_cmatch;
// # 1491 "mail_params.h"
extern char *var_smtp_tls_sec_cmatch;






extern char *var_smtp_tls_fpt_cmatch;





extern bool var_smtp_tls_blk_early_mail_reply;






extern bool var_smtpd_sasl_enable;



extern bool var_smtpd_sasl_auth_hdr;



extern char *var_smtpd_sasl_opts;



extern char *var_smtpd_sasl_path;



extern char *var_cyrus_conf_path;



extern char *var_smtpd_sasl_tls_opts;



extern char *var_smtpd_sasl_realm;



extern char *var_smtpd_sasl_exceptions_networks;







extern char *var_smtpd_sasl_type;



extern char *var_smtpd_snd_auth_maps;
// # 1564 "mail_params.h"
extern bool var_smtp_sasl_enable;



extern char *var_smtp_sasl_passwd;



extern char *var_smtp_sasl_opts;



extern char *var_smtp_sasl_path;





extern char *var_smtp_sasl_mechs;
// # 1592 "mail_params.h"
extern char *var_smtp_sasl_type;





extern char *var_smtp_sasl_tls_opts;





extern char *var_smtp_sasl_tlsv_opts;



extern bool var_smtp_dummy_mail_auth;
// # 1617 "mail_params.h"
extern char *var_lmtpd_banner;



extern int var_lmtpd_tmout;



extern int var_lmtpd_rcpt_limit;



extern int var_lmtpd_soft_erlim;



extern int var_lmtpd_hard_erlim;



extern int var_lmtpd_err_sleep;



extern int var_lmtpd_junk_cmd_limit;






extern bool var_lmtpd_sasl_enable;



extern char *var_lmtpd_sasl_opts;



extern char *var_lmtpd_sasl_realm;






extern bool var_lmtp_sasl_enable;



extern char *var_lmtp_sasl_passwd;



extern char *var_lmtp_sasl_opts;



extern char *var_lmtp_sasl_path;



extern bool var_lmtp_dummy_mail_auth;
// # 1688 "mail_params.h"
extern int var_cyrus_sasl_authzid;
// # 1697 "mail_params.h"
extern bool var_smtp_sasl_auth_soft_bounce;





extern char *var_smtp_sasl_auth_cache_name;





extern int var_smtp_sasl_auth_cache_time;
// # 1719 "mail_params.h"
extern char *var_lmtp_tcp_port;



extern bool var_lmtp_assume_final;



extern bool var_lmtp_cache_conn;



extern bool var_lmtp_skip_quit_resp;



extern int var_lmtp_conn_tmout;



extern int var_lmtp_rset_tmout;



extern int var_lmtp_lhlo_tmout;



extern int var_lmtp_xfwd_tmout;



extern int var_lmtp_mail_tmout;



extern int var_lmtp_rcpt_tmout;



extern int var_lmtp_data0_tmout;



extern int var_lmtp_data1_tmout;



extern int var_lmtp_data2_tmout;



extern int var_lmtp_quit_tmout;



extern bool var_lmtp_send_xforward;







extern int var_hopcount_limit;



extern int var_header_limit;



extern int var_token_limit;



extern int var_virt_recur_limit;



extern int var_virt_expan_limit;






extern long var_message_limit;



extern int var_queue_minfree;



extern char *var_header_checks;



extern char *var_mimehdr_checks;



extern char *var_nesthdr_checks;



extern char *var_body_checks;



extern int var_body_check_len;






extern int var_bounce_limit;







extern char *var_double_bounce_sender;






extern int var_fork_tries;



extern int var_fork_delay;






extern int var_flock_tries;



extern int var_flock_delay;



extern int var_flock_stale;



extern int var_mailtool_compat;







extern int var_daemon_timeout;



extern int var_qmgr_daemon_timeout;







extern int var_ipc_timeout;



extern int var_qmgr_ipc_timeout;






extern int var_trigger_timeout;







extern char *var_mynetworks;



extern char *var_mynetworks_style;







extern char *var_relay_domains;



extern char *var_relay_transport;



extern char *var_relay_rcpt_maps;



extern int var_relay_rcpt_code;



extern char *var_smtpd_relay_ccerts;



extern char *var_client_checks;



extern bool var_helo_required;



extern char *var_helo_checks;



extern char *var_mail_checks;



extern char *var_rcpt_checks;



extern char *var_etrn_checks;



extern char *var_data_checks;



extern char *var_eod_checks;



extern char *var_rest_classes;



extern bool var_allow_untrust_route;
// # 1993 "mail_params.h"
extern int var_reject_code;




extern int var_defer_code;






extern char *var_reject_tmpf_act;






extern int var_plaintext_code;







extern int var_unk_client_code;
// # 2032 "mail_params.h"
extern int var_bad_name_code;





extern int var_unk_name_code;



extern char *var_unk_name_tf_act;







extern int var_non_fqdn_code;
// # 2061 "mail_params.h"
extern int var_unk_addr_code;



extern char *var_unk_addr_tf_act;



extern bool var_smtpd_rej_unl_from;



extern bool var_smtpd_rej_unl_rcpt;




extern int var_unv_rcpt_rcode;




extern int var_unv_from_rcode;



extern int var_unv_rcpt_dcode;



extern int var_unv_from_dcode;



extern char *var_unv_rcpt_tf_act;



extern char *var_unv_from_tf_act;



extern char *var_unv_rcpt_why;



extern char *var_unv_from_why;




extern int var_mul_rcpt_code;
// # 2121 "mail_params.h"
extern int var_relay_code;





extern char *var_perm_mx_networks;



extern int var_map_reject_code;



extern int var_map_defer_code;
// # 2171 "mail_params.h"
extern char *var_rbl_reply_maps;



extern char *var_def_rbl_reply;




extern int var_maps_rbl_code;



extern char *var_maps_rbl_domains;



extern int var_smtpd_delay_reject;





extern char *var_smtpd_null_key;





extern char *var_smtpd_exp_filter;



extern bool var_smtpd_peername_lookup;






extern char *var_local_rcpt_maps;



extern int var_local_rcpt_code;
// # 2240 "mail_params.h"
extern char *var_proxy_read_maps;






extern char *var_proxy_write_maps;



extern char *var_proxy_read_acl;



extern char *var_proxy_write_acl;





extern char *var_procname;


extern int var_pid;
// # 2273 "mail_params.h"
extern bool var_dont_remove;






extern bool var_soft_bounce;






extern bool var_ownreq_special;






extern bool var_allow_min_user;

extern void mail_params_init(void);






extern char *var_filter_xport;



extern char *var_def_filter_nexthop;






extern char *var_fflush_domains;



extern int var_fflush_purge;



extern int var_fflush_refresh;







extern char *var_import_environ;



extern char *var_export_environ;






extern char *var_virt_transport;



extern char *var_virt_mailbox_maps;



extern char *var_virt_mailbox_doms;



extern int var_virt_mailbox_code;



extern char *var_virt_uid_maps;



extern char *var_virt_gid_maps;



extern int var_virt_minimum_uid;



extern char *var_virt_mailbox_base;



extern long var_virt_mailbox_limit;



extern char *var_virt_mailbox_lock;
// # 2389 "mail_params.h"
extern char *var_syslog_name;






extern char *var_qmqpd_clients;



extern int var_qmqpd_timeout;



extern int var_qmqpd_err_sleep;







extern char *var_verp_delims;



extern char *var_verp_filter;



extern bool var_verp_bounce_off;



extern char *var_verp_clients;






extern char *var_xclient_hosts;






extern char *var_xforward_hosts;
// # 2455 "mail_params.h"
extern int var_in_flow_delay;
// # 2468 "mail_params.h"
extern char *var_par_dom_match;
// # 2477 "mail_params.h"
extern int var_fault_inj_code;
// # 2526 "mail_params.h"
extern bool var_resolve_dequoted;



extern bool var_resolve_nulldom;



extern bool var_resolve_num_dom;
// # 2545 "mail_params.h"
extern char *var_bounce_service;



extern char *var_cleanup_service;



extern char *var_defer_service;



extern char *var_pickup_service;



extern char *var_queue_service;





extern char *var_rewrite_service;



extern char *var_showq_service;



extern char *var_error_service;



extern char *var_flush_service;






extern char *var_scache_service;



extern int var_scache_proto_tmout;



extern int var_scache_ttl_lim;



extern int var_scache_stat_time;






extern char *var_verify_service;



extern char *var_verify_map;



extern int var_verify_pos_exp;



extern int var_verify_pos_try;



extern int var_verify_neg_exp;



extern int var_verify_neg_try;



extern bool var_verify_neg_cache;



extern int var_verify_scan_cache;



extern char *var_verify_sender;



extern int var_verify_sender_ttl;



extern int var_verify_poll_count;



extern int var_verify_poll_delay;



extern char *var_vrfy_local_xport;



extern char *var_vrfy_virt_xport;



extern char *var_vrfy_relay_xport;



extern char *var_vrfy_def_xport;



extern char *var_snd_def_xport_maps;



extern char *var_vrfy_relayhost;



extern char *var_vrfy_relay_maps;



extern char *var_vrfy_xport_maps;






extern char *var_trace_service;






extern char *var_proxymap_service;



extern char *var_proxywrite_service;






extern char *var_mbx_defer_errs;



extern char *var_mdr_defer_errs;






extern int var_db_create_buf;



extern int var_db_read_buf;






extern int var_qattr_count_limit;






extern int var_mime_maxdepth;



extern int var_mime_bound_len;



extern bool var_disable_mime_input;



extern bool var_disable_mime_oconv;



extern bool var_strict_8bitmime;



extern bool var_strict_7bit_hdrs;



extern bool var_strict_8bit_body;



extern bool var_strict_encoding;



extern int var_auto_8bit_enc_hdr;






extern bool var_sender_routing;



extern char *var_xport_null_key;






extern bool var_oldlog_compat;






extern char *var_smtpd_proxy_filt;



extern char *var_smtpd_proxy_ehlo;



extern int var_smtpd_proxy_tmout;



extern char *var_smtpd_proxy_opts;
// # 2813 "mail_params.h"
extern char *var_smtpd_input_transp;






extern int var_smtpd_policy_tmout;



extern int var_smtpd_policy_idle;



extern int var_smtpd_policy_ttl;
// # 2837 "mail_params.h"
extern int var_smtpd_crate_limit;



extern int var_smtpd_cconn_limit;



extern int var_smtpd_cmail_limit;



extern int var_smtpd_crcpt_limit;



extern int var_smtpd_cntls_limit;



extern char *var_smtpd_hoggers;



extern int var_anvil_time_unit;



extern int var_anvil_stat_time;
// # 2884 "mail_params.h"
extern char *var_remote_rwr_domain;





extern char *var_local_rwr_clients;






extern char *var_smtpd_ehlo_dis_words;



extern char *var_smtpd_ehlo_dis_maps;





extern char *var_smtp_ehlo_dis_words;





extern char *var_smtp_ehlo_dis_maps;




extern const char null_format_string[1];






extern char *var_msg_reject_chars;



extern char *var_msg_strip_chars;






extern bool var_frozen_delivered;



extern bool var_reset_owner_attr;
// # 2949 "mail_params.h"
extern int var_delay_max_res;






extern char *var_bounce_tmpl;
// # 2965 "mail_params.h"
extern bool var_smtp_sender_auth;
// # 2974 "mail_params.h"
extern bool var_smtp_cname_overr;
// # 2992 "mail_params.h"
extern char *var_tls_high_clist;



extern char *var_tls_medium_clist;



extern char *var_tls_low_clist;



extern char *var_tls_export_clist;



extern char *var_tls_null_clist;



extern char *var_tls_eecdh_strong;



extern char *var_tls_eecdh_ultra;



extern bool var_tls_preempt_clist;
// # 3036 "mail_params.h"
extern char *var_tls_bug_tweaks;






extern char *var_smtpd_milters;



extern char *var_cleanup_milters;



extern char *var_milt_def_action;



extern char *var_milt_conn_macros;




extern char *var_milt_helo_macros;





extern char *var_milt_mail_macros;




extern char *var_milt_rcpt_macros;



extern char *var_milt_data_macros;



extern char *var_milt_unk_macros;



extern char *var_milt_eoh_macros;



extern char *var_milt_eod_macros;



extern int var_milt_conn_time;



extern int var_milt_cmd_time;



extern int var_milt_msg_time;



extern char *var_milt_protocol;



extern char *var_milt_def_action;



extern char *var_milt_daemon_name;



extern char *var_milt_v;



extern char *var_milt_head_checks;
// # 3131 "mail_params.h"
extern char *var_int_filt_classes;






extern bool var_smtpd_client_port_log;



extern bool var_qmqpd_client_port_log;






extern char *var_smtp_head_chks;



extern char *var_smtp_mime_chks;



extern char *var_smtp_nest_chks;



extern char *var_smtp_body_chks;
// # 3178 "mail_params.h"
extern char *var_smtp_addr_pref;
// # 3189 "mail_params.h"
extern char *var_conc_pos_feedback;




extern char *var_conc_neg_feedback;







extern int var_conc_cohort_limit;



extern bool var_conc_feedback_debug;




extern int var_dest_rate_delay;






extern char *var_stress;






extern bool var_strict_mbox_owner;






extern int var_inet_windowsize;







extern char *var_multi_conf_dirs;



extern char *var_multi_wrapper;



extern char *var_multi_name;



extern char *var_multi_group;



extern bool var_multi_enable;






extern char *var_multi_start_cmds;



extern char *var_multi_stop_cmds;



extern char *var_multi_cntrl_cmds;






extern char *var_psc_cache_map;



extern char *var_smtpd_service;



extern int var_psc_post_queue_limit;



extern int var_psc_pre_queue_limit;



extern int var_psc_cache_ret;



extern int var_psc_cache_scan;



extern int var_psc_greet_wait;



extern char *var_psc_pregr_banner;



extern char *var_psc_pregr_enable;



extern char *var_psc_pregr_action;



extern int var_psc_pregr_ttl;



extern char *var_psc_dnsbl_sites;



extern int var_psc_dnsbl_thresh;



extern char *var_psc_dnsbl_enable;



extern char *var_psc_dnsbl_action;



extern int var_psc_dnsbl_ttl;



extern char *var_psc_dnsbl_reply;



extern bool var_psc_pipel_enable;



extern char *var_psc_pipel_action;



extern int var_psc_pipel_ttl;



extern bool var_psc_nsmtp_enable;



extern char *var_psc_nsmtp_action;



extern int var_psc_nsmtp_ttl;



extern bool var_psc_barlf_enable;



extern char *var_psc_barlf_action;



extern int var_psc_barlf_ttl;



extern char *var_psc_wlist_nets;



extern char *var_psc_blist_nets;



extern char *var_psc_blist_nets;



extern int var_psc_cmd_count;



extern char *var_psc_cmd_time;



extern int var_psc_watchdog;



extern char *var_psc_ehlo_dis_words;



extern char *var_psc_ehlo_dis_maps;



extern char *var_psc_tls_level;



extern bool var_psc_use_tls;



extern bool var_psc_enforce_tls;



extern char *var_psc_forbid_cmds;



extern bool var_psc_helo_required;



extern bool var_psc_disable_vrfy;



extern int var_psc_cconn_limit;



extern char *var_psc_rej_footer;



extern char *var_psc_exp_filter;



extern char *var_psc_cmd_filter;



extern char *var_psc_acl;



extern char *var_psc_wlist_if;



extern char *var_dnsblog_service;



extern int var_dnsblog_delay;



extern char *var_tlsproxy_service;



extern int var_tlsp_watchdog;



extern char *var_tlsp_tls_level;



extern bool var_tlsp_use_tls;



extern bool var_tlsp_enforce_tls;



extern bool var_tlsp_tls_ask_ccert;



extern bool var_tlsp_tls_req_ccert;



extern int var_tlsp_tls_ccert_vd;



extern char *var_tlsp_tls_cert_file;



extern char *var_tlsp_tls_key_file;



extern char *var_tlsp_tls_dcert_file;



extern char *var_tlsp_tls_dkey_file;



extern char *var_tlsp_tls_eccert_file;



extern char *var_tlsp_tls_eckey_file;


extern char *var_tlsp_tls_eckey_file;



extern char *var_tlsp_tls_CAfile;



extern char *var_tlsp_tls_CApath;



extern char *var_tlsp_tls_proto;



extern char *var_tlsp_tls_mand_proto;



extern char *var_tlsp_tls_ciph;



extern char *var_tlsp_tls_mand_ciph;



extern char *var_tlsp_tls_excl_ciph;



extern char *var_tlsp_tls_mand_excl;



extern char *var_tlsp_tls_fpt_dgst;



extern char *var_tlsp_tls_dh512_param_file;



extern char *var_tlsp_tls_dh1024_param_file;



extern char *var_tlsp_tls_eecdh;



extern char *var_tlsp_tls_loglevel;



extern bool var_tlsp_tls_received_header;



extern char *var_tlsp_tls_scache_db;



extern int var_tlsp_tls_scache_timeout;



extern bool var_tlsp_tls_set_sessid;






extern char *var_smtpd_rej_footer;






extern bool var_smtpd_rec_deadline;





extern bool var_smtp_rec_deadline;
// # 3624 "mail_params.h"
extern char *var_sm_fix_eol;






extern bool var_daemon_open_fatal;
// # 73 "mail_run.c" 2
// # 1 "mail_run.h" 1
// # 16 "mail_run.h"
extern int mail_run_foreground(const char *, char **);
extern int mail_run_background(const char *, char **);
extern void __attribute__((__noreturn__)) mail_run_replace(const char *, char **);
// # 74 "mail_run.c" 2



int mail_run_foreground(const char *dir, char **argv)
{
    int count;
    char *path;
    WAIT_STATUS_T status;
    int pid;
    int wpid;



    path = concatenate(dir, "/", argv[0], (char *) 0);

    for (count = 0; count < var_fork_tries; count++) {
 switch (pid = fork()) {
 case -1:
     msg_warn("fork %s: %m", path);
     break;
 case 0:

     (void) msg_cleanup((MSG_CLEANUP_FN) 0);
     execv(path, argv);
     msg_fatal("execv %s: %m", path);
 default:
     do {
  wpid = waitpid(pid, &status, 0);
     } while (wpid == -1 && (*__errno_location ()) == 4);
     { myfree(path); return(wpid == -1 ? -1 : ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) ? ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) >> 8) : 1); }

 }
 sleep(var_fork_delay);
    }
    { myfree(path); return(-1); };
}



int mail_run_background(const char *dir, char **argv)
{
    int count;
    char *path;
    int pid;



    path = concatenate(dir, "/", argv[0], (char *) 0);

    for (count = 0; count < var_fork_tries; count++) {
 switch (pid = fork()) {
 case -1:
     msg_warn("fork %s: %m", path);
     break;
 case 0:

     (void) msg_cleanup((MSG_CLEANUP_FN) 0);
     execv(path, argv);
     msg_fatal("execv %s: %m", path);
 default:
     { myfree(path); return(pid); };
 }
 sleep(var_fork_delay);
    }
    { myfree(path); return(-1); };
}



void __attribute__((__noreturn__)) mail_run_replace(const char *dir, char **argv)
{
    char *path;

    path = concatenate(dir, "/", argv[0], (char *) 0);
    execv(path, argv);
    msg_fatal("execv %s: %m", path);
}
