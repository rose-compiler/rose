// ngx_http_upstream_combined.c

# 1 "src/http/ngx_http_upstream.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/http/ngx_http_upstream.c"







# 1 "src/core/ngx_config.h" 1
# 12 "src/core/ngx_config.h"
# 1 "objs/ngx_auto_headers.h" 1
# 13 "src/core/ngx_config.h" 2
# 26 "src/core/ngx_config.h"
# 1 "src/os/unix/ngx_linux_config.h" 1
# 18 "src/os/unix/ngx_linux_config.h"
# 1 "/usr/include/sys/types.h" 1 3 4
# 27 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 329 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 313 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 314 "/usr/include/sys/cdefs.h" 2 3 4
# 330 "/usr/include/features.h" 2 3 4
# 352 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4




# 1 "/usr/include/gnu/stubs-64.h" 1 3 4
# 10 "/usr/include/gnu/stubs.h" 2 3 4
# 353 "/usr/include/features.h" 2 3 4
# 28 "/usr/include/sys/types.h" 2 3 4



# 1 "/usr/include/bits/types.h" 1 3 4
# 28 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 214 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 32 "/usr/include/bits/types.h" 2 3 4


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
# 134 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 135 "/usr/include/bits/types.h" 2 3 4


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
# 32 "/usr/include/sys/types.h" 2 3 4



typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;





typedef __ino64_t ino_t;




typedef __ino64_t ino64_t;




typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;







typedef __off64_t off_t;




typedef __off64_t off64_t;




typedef __pid_t pid_t;




typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 133 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 59 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 75 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 93 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 105 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/usr/include/sys/types.h" 2 3 4



typedef __useconds_t useconds_t;



typedef __suseconds_t suseconds_t;





# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 148 "/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 195 "/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 217 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/endian.h" 1 3 4
# 38 "/usr/include/endian.h" 2 3 4
# 218 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/select.h" 1 3 4
# 31 "/usr/include/sys/select.h" 3 4
# 1 "/usr/include/bits/select.h" 1 3 4
# 32 "/usr/include/sys/select.h" 2 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 23 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 35 "/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 121 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
# 45 "/usr/include/sys/select.h" 2 3 4

# 1 "/usr/include/bits/time.h" 1 3 4
# 69 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 47 "/usr/include/sys/select.h" 2 3 4
# 55 "/usr/include/sys/select.h" 3 4
typedef long int __fd_mask;
# 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {



    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];





  } fd_set;






typedef __fd_mask fd_mask;
# 99 "/usr/include/sys/select.h" 3 4

# 109 "/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 121 "/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



# 221 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/sysmacros.h" 1 3 4
# 29 "/usr/include/sys/sysmacros.h" 3 4
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
# 224 "/usr/include/sys/types.h" 2 3 4




typedef __blksize_t blksize_t;
# 248 "/usr/include/sys/types.h" 3 4
typedef __blkcnt64_t blkcnt_t;



typedef __fsblkcnt64_t fsblkcnt_t;



typedef __fsfilcnt64_t fsfilcnt_t;





typedef __blkcnt64_t blkcnt64_t;
typedef __fsblkcnt64_t fsblkcnt64_t;
typedef __fsfilcnt64_t fsfilcnt64_t;





# 1 "/usr/include/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/pthreadtypes.h" 2 3 4
# 50 "/usr/include/bits/pthreadtypes.h" 3 4
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
# 76 "/usr/include/bits/pthreadtypes.h" 3 4
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
# 101 "/usr/include/bits/pthreadtypes.h" 3 4
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
# 187 "/usr/include/bits/pthreadtypes.h" 3 4
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
# 271 "/usr/include/sys/types.h" 2 3 4



# 19 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/time.h" 1 3 4
# 27 "/usr/include/sys/time.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 28 "/usr/include/sys/time.h" 2 3 4

# 1 "/usr/include/bits/time.h" 1 3 4
# 30 "/usr/include/sys/time.h" 2 3 4
# 39 "/usr/include/sys/time.h" 3 4

# 57 "/usr/include/sys/time.h" 3 4
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

typedef struct timezone *__restrict __timezone_ptr_t;
# 73 "/usr/include/sys/time.h" 3 4
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




typedef enum __itimer_which __itimer_which_t;






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






extern int futimesat (int __fd, __const char *__file,
        __const struct timeval __tvp[2]) __attribute__ ((__nothrow__));
# 191 "/usr/include/sys/time.h" 3 4

# 20 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/unistd.h" 1 3 4
# 28 "/usr/include/unistd.h" 3 4

# 173 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/bits/posix_opt.h" 1 3 4
# 174 "/usr/include/unistd.h" 2 3 4



# 1 "/usr/include/bits/environments.h" 1 3 4
# 23 "/usr/include/bits/environments.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/environments.h" 2 3 4
# 178 "/usr/include/unistd.h" 2 3 4
# 197 "/usr/include/unistd.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 198 "/usr/include/unistd.h" 2 3 4
# 238 "/usr/include/unistd.h" 3 4
typedef __intptr_t intptr_t;






typedef __socklen_t socklen_t;
# 258 "/usr/include/unistd.h" 3 4
extern int access (__const char *__name, int __type) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int euidaccess (__const char *__name, int __type)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int eaccess (__const char *__name, int __type)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int faccessat (int __fd, __const char *__file, int __type, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;
# 304 "/usr/include/unistd.h" 3 4
extern __off64_t lseek (int __fd, __off64_t __offset, int __whence) __asm__ ("" "lseek64") __attribute__ ((__nothrow__));







extern __off64_t lseek64 (int __fd, __off64_t __offset, int __whence)
     __attribute__ ((__nothrow__));






extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, __const void *__buf, size_t __n) ;
# 355 "/usr/include/unistd.h" 3 4
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes, __off64_t __offset) __asm__ ("" "pread64") ;


extern ssize_t pwrite (int __fd, __const void *__buf, size_t __nbytes, __off64_t __offset) __asm__ ("" "pwrite64") ;
# 371 "/usr/include/unistd.h" 3 4
extern ssize_t pread64 (int __fd, void *__buf, size_t __nbytes,
   __off64_t __offset) ;


extern ssize_t pwrite64 (int __fd, __const void *__buf, size_t __n,
    __off64_t __offset) ;







extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__)) ;
# 393 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__));
# 405 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);






extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__));






extern int usleep (__useconds_t __useconds);
# 429 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__)) ;




extern int lchown (__const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;






extern int fchownat (int __fd, __const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;



extern int chdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__)) ;
# 471 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__)) ;





extern char *get_current_dir_name (void) __attribute__ ((__nothrow__));






extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__));


extern char **__environ;

extern char **environ;





extern int execve (__const char *__path, char *__const __argv[],
     char *__const __envp[]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int fexecve (int __fd, char *__const __argv[], char *__const __envp[])
     __attribute__ ((__nothrow__));




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





# 1 "/usr/include/bits/confname.h" 1 3 4
# 26 "/usr/include/bits/confname.h" 3 4
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
# 555 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (__const char *__path, int __name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__));




extern __pid_t getpid (void) __attribute__ ((__nothrow__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__));




extern __pid_t getpgrp (void) __attribute__ ((__nothrow__));
# 591 "/usr/include/unistd.h" 3 4
extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__));

extern __pid_t getpgid (__pid_t __pid) __attribute__ ((__nothrow__));






extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__));
# 617 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__));
# 634 "/usr/include/unistd.h" 3 4
extern __pid_t setsid (void) __attribute__ ((__nothrow__));



extern __pid_t getsid (__pid_t __pid) __attribute__ ((__nothrow__));



extern __uid_t getuid (void) __attribute__ ((__nothrow__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__)) ;



extern int group_member (__gid_t __gid) __attribute__ ((__nothrow__));






extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__));




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__));




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__));






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__));




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__));




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__));





extern int getresuid (__uid_t *__ruid, __uid_t *__euid, __uid_t *__suid)
     __attribute__ ((__nothrow__));



extern int getresgid (__gid_t *__rgid, __gid_t *__egid, __gid_t *__sgid)
     __attribute__ ((__nothrow__));



extern int setresuid (__uid_t __ruid, __uid_t __euid, __uid_t __suid)
     __attribute__ ((__nothrow__));



extern int setresgid (__gid_t __rgid, __gid_t __egid, __gid_t __sgid)
     __attribute__ ((__nothrow__));






extern __pid_t fork (void) __attribute__ ((__nothrow__));






extern __pid_t vfork (void) __attribute__ ((__nothrow__));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__));





extern int ttyslot (void) __attribute__ ((__nothrow__));




extern int link (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int linkat (int __fromfd, __const char *__from, int __tofd,
     __const char *__to, int __flags)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4))) ;




extern int symlink (__const char *__from, __const char *__to)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (__const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int symlinkat (__const char *__from, int __tofd,
        __const char *__to) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3))) ;


extern ssize_t readlinkat (int __fd, __const char *__restrict __path,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3))) ;



extern int unlink (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int unlinkat (int __fd, __const char *__name, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



extern int rmdir (__const char *__path) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 837 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/getopt.h" 1 3 4
# 59 "/usr/include/getopt.h" 3 4
extern char *optarg;
# 73 "/usr/include/getopt.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 152 "/usr/include/getopt.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__));
# 838 "/usr/include/unistd.h" 2 3 4







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
# 923 "/usr/include/unistd.h" 3 4
extern int fsync (int __fd);






extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__));




extern int getpagesize (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__));
# 952 "/usr/include/unistd.h" 3 4
extern int truncate (__const char *__file, __off64_t __length) __asm__ ("" "truncate64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;







extern int truncate64 (__const char *__file, __off64_t __length)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 973 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off64_t __length) __asm__ ("" "ftruncate64") __attribute__ ((__nothrow__)) ;






extern int ftruncate64 (int __fd, __off64_t __length) __attribute__ ((__nothrow__)) ;
# 990 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__));
# 1011 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__));
# 1037 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off64_t __len) __asm__ ("" "lockf64") ;






extern int lockf64 (int __fd, int __cmd, __off64_t __len) ;
# 1065 "/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes);







extern char *crypt (__const char *__key, __const char *__salt)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern void encrypt (char *__block, int __edflag) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void swab (__const void *__restrict __from, void *__restrict __to,
    ssize_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));







extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
# 1103 "/usr/include/unistd.h" 3 4

# 21 "src/os/unix/ngx_linux_config.h" 2
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
# 43 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 105 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 22 "src/os/unix/ngx_linux_config.h" 2
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 152 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long int ptrdiff_t;
# 326 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef int wchar_t;
# 23 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/stdio.h" 1 3 4
# 30 "/usr/include/stdio.h" 3 4




# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 35 "/usr/include/stdio.h" 2 3 4
# 44 "/usr/include/stdio.h" 3 4


typedef struct _IO_FILE FILE;





# 62 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 72 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 32 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 14 "/usr/include/_G_config.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 355 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef unsigned int wint_t;
# 15 "/usr/include/_G_config.h" 2 3 4
# 24 "/usr/include/_G_config.h" 3 4
# 1 "/usr/include/wchar.h" 1 3 4
# 48 "/usr/include/wchar.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 49 "/usr/include/wchar.h" 2 3 4

# 1 "/usr/include/bits/wchar.h" 1 3 4
# 51 "/usr/include/wchar.h" 2 3 4
# 76 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
# 25 "/usr/include/_G_config.h" 2 3 4

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
# 44 "/usr/include/_G_config.h" 3 4
# 1 "/usr/include/gconv.h" 1 3 4
# 28 "/usr/include/gconv.h" 3 4
# 1 "/usr/include/wchar.h" 1 3 4
# 48 "/usr/include/wchar.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 49 "/usr/include/wchar.h" 2 3 4
# 29 "/usr/include/gconv.h" 2 3 4


# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 32 "/usr/include/gconv.h" 2 3 4





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
# 45 "/usr/include/_G_config.h" 2 3 4
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
# 33 "/usr/include/libio.h" 2 3 4
# 167 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 177 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 200 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 268 "/usr/include/libio.h" 3 4
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
# 316 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 325 "/usr/include/libio.h" 3 4
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
# 361 "/usr/include/libio.h" 3 4
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
# 451 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 481 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 73 "/usr/include/stdio.h" 2 3 4
# 86 "/usr/include/stdio.h" 3 4




typedef _G_fpos64_t fpos_t;



typedef _G_fpos64_t fpos64_t;
# 138 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 139 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;









extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));




extern int renameat (int __oldfd, __const char *__old, int __newfd,
       __const char *__new) __attribute__ ((__nothrow__));



# 174 "/usr/include/stdio.h" 3 4
extern FILE *tmpfile (void) __asm__ ("" "tmpfile64");






extern FILE *tmpfile64 (void);



extern char *tmpnam (char *__s) __attribute__ ((__nothrow__));





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__));
# 203 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__));








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 228 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 238 "/usr/include/stdio.h" 3 4
extern int fcloseall (void);




# 259 "/usr/include/stdio.h" 3 4
extern FILE *fopen (__const char *__restrict __filename, __const char *__restrict __modes) __asm__ ("" "fopen64");

extern FILE *freopen (__const char *__restrict __filename, __const char *__restrict __modes, FILE *__restrict __stream) __asm__ ("" "freopen64");









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

# 460 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 471 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 504 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 550 "/usr/include/stdio.h" 3 4
extern char *fgets_unlocked (char *__restrict __s, int __n,
        FILE *__restrict __stream) ;
# 566 "/usr/include/stdio.h" 3 4
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

# 627 "/usr/include/stdio.h" 3 4
extern int fputs_unlocked (__const char *__restrict __s,
      FILE *__restrict __stream);
# 638 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream) ;








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 682 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off64_t __off, int __whence) __asm__ ("" "fseeko64");


extern __off64_t ftello (FILE *__stream) __asm__ ("" "ftello64");








# 707 "/usr/include/stdio.h" 3 4
extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos) __asm__ ("" "fgetpos64");

extern int fsetpos (FILE *__stream, __const fpos_t *__pos) __asm__ ("" "fsetpos64");









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






# 1 "/usr/include/bits/sys_errlist.h" 1 3 4
# 27 "/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];


extern int _sys_nerr;
extern __const char *__const _sys_errlist[];
# 755 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
# 774 "/usr/include/stdio.h" 3 4
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
# 844 "/usr/include/stdio.h" 3 4

# 24 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/stdlib.h" 1 3 4
# 33 "/usr/include/stdlib.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 34 "/usr/include/stdlib.h" 2 3 4








# 1 "/usr/include/bits/waitflags.h" 1 3 4
# 43 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/bits/waitstatus.h" 1 3 4
# 67 "/usr/include/bits/waitstatus.h" 3 4
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
# 44 "/usr/include/stdlib.h" 2 3 4
# 68 "/usr/include/stdlib.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
# 96 "/usr/include/stdlib.h" 3 4


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


# 140 "/usr/include/stdlib.h" 3 4
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

# 236 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/xlocale.h" 1 3 4
# 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;
# 237 "/usr/include/stdlib.h" 2 3 4



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
# 429 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__)) ;


extern long int a64l (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
# 445 "/usr/include/stdlib.h" 3 4
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



# 1 "/usr/include/alloca.h" 1 3 4
# 25 "/usr/include/alloca.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 26 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__));






# 613 "/usr/include/stdlib.h" 2 3 4




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
# 698 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 712 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __asm__ ("" "mkstemp64")
     __attribute__ ((__nonnull__ (1))) ;





extern int mkstemp64 (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 729 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;








extern int system (__const char *__command) ;







extern char *canonicalize_file_name (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 756 "/usr/include/stdlib.h" 3 4
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

# 821 "/usr/include/stdlib.h" 3 4
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
# 909 "/usr/include/stdlib.h" 3 4
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
# 977 "/usr/include/stdlib.h" 3 4

# 25 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/errno.h" 1 3 4
# 32 "/usr/include/errno.h" 3 4




# 1 "/usr/include/bits/errno.h" 1 3 4
# 25 "/usr/include/bits/errno.h" 3 4
# 1 "/usr/include/linux/errno.h" 1 3 4



# 1 "/usr/include/asm/errno.h" 1 3 4




# 1 "/usr/include/asm-x86_64/errno.h" 1 3 4



# 1 "/usr/include/asm-generic/errno.h" 1 3 4



# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 5 "/usr/include/asm-generic/errno.h" 2 3 4
# 5 "/usr/include/asm-x86_64/errno.h" 2 3 4
# 6 "/usr/include/asm/errno.h" 2 3 4
# 5 "/usr/include/linux/errno.h" 2 3 4
# 26 "/usr/include/bits/errno.h" 2 3 4
# 43 "/usr/include/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 37 "/usr/include/errno.h" 2 3 4
# 55 "/usr/include/errno.h" 3 4
extern char *program_invocation_name, *program_invocation_short_name;




# 69 "/usr/include/errno.h" 3 4
typedef int error_t;
# 26 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/string.h" 1 3 4
# 28 "/usr/include/string.h" 3 4





# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 34 "/usr/include/string.h" 2 3 4




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

# 121 "/usr/include/string.h" 3 4
extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));




extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 165 "/usr/include/string.h" 3 4


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

# 281 "/usr/include/string.h" 3 4
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
# 426 "/usr/include/string.h" 3 4

# 27 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/signal.h" 1 3 4
# 31 "/usr/include/signal.h" 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 103 "/usr/include/bits/sigset.h" 3 4
extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
# 34 "/usr/include/signal.h" 2 3 4







typedef __sig_atomic_t sig_atomic_t;

# 58 "/usr/include/signal.h" 3 4
# 1 "/usr/include/bits/signum.h" 1 3 4
# 59 "/usr/include/signal.h" 2 3 4
# 75 "/usr/include/signal.h" 3 4
typedef void (*__sighandler_t) (int);




extern __sighandler_t __sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));

extern __sighandler_t sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));







extern __sighandler_t signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
# 104 "/usr/include/signal.h" 3 4





extern __sighandler_t bsd_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));






extern int kill (__pid_t __pid, int __sig) __attribute__ ((__nothrow__));






extern int killpg (__pid_t __pgrp, int __sig) __attribute__ ((__nothrow__));




extern int raise (int __sig) __attribute__ ((__nothrow__));




extern __sighandler_t ssignal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
extern int gsignal (int __sig) __attribute__ ((__nothrow__));




extern void psignal (int __sig, __const char *__s);
# 153 "/usr/include/signal.h" 3 4
extern int __sigpause (int __sig_or_mask, int __is_sig);
# 162 "/usr/include/signal.h" 3 4
extern int sigpause (int __sig) __asm__ ("__xpg_sigpause");
# 181 "/usr/include/signal.h" 3 4
extern int sigblock (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int sigsetmask (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int siggetmask (void) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
# 196 "/usr/include/signal.h" 3 4
typedef __sighandler_t sighandler_t;




typedef __sighandler_t sig_t;







# 1 "/usr/include/time.h" 1 3 4
# 210 "/usr/include/signal.h" 2 3 4


# 1 "/usr/include/bits/siginfo.h" 1 3 4
# 25 "/usr/include/bits/siginfo.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 26 "/usr/include/bits/siginfo.h" 2 3 4







typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
# 51 "/usr/include/bits/siginfo.h" 3 4
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
# 129 "/usr/include/bits/siginfo.h" 3 4
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
# 273 "/usr/include/bits/siginfo.h" 3 4
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
# 213 "/usr/include/signal.h" 2 3 4



extern int sigemptyset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigfillset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigaddset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigdelset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigismember (__const sigset_t *__set, int __signo)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern int sigisemptyset (__const sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigandset (sigset_t *__set, __const sigset_t *__left,
        __const sigset_t *__right) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3)));


extern int sigorset (sigset_t *__set, __const sigset_t *__left,
       __const sigset_t *__right) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3)));




# 1 "/usr/include/bits/sigaction.h" 1 3 4
# 25 "/usr/include/bits/sigaction.h" 3 4
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
# 247 "/usr/include/signal.h" 2 3 4


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
# 304 "/usr/include/signal.h" 3 4
extern __const char *__const _sys_siglist[65];
extern __const char *__const sys_siglist[65];


struct sigvec
  {
    __sighandler_t sv_handler;
    int sv_mask;

    int sv_flags;

  };
# 328 "/usr/include/signal.h" 3 4
extern int sigvec (int __sig, __const struct sigvec *__vec,
     struct sigvec *__ovec) __attribute__ ((__nothrow__));



# 1 "/usr/include/bits/sigcontext.h" 1 3 4
# 26 "/usr/include/bits/sigcontext.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 27 "/usr/include/bits/sigcontext.h" 2 3 4

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
# 109 "/usr/include/bits/sigcontext.h" 3 4
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
# 334 "/usr/include/signal.h" 2 3 4


extern int sigreturn (struct sigcontext *__scp) __attribute__ ((__nothrow__));
# 346 "/usr/include/signal.h" 3 4
extern int siginterrupt (int __sig, int __interrupt) __attribute__ ((__nothrow__));

# 1 "/usr/include/bits/sigstack.h" 1 3 4
# 26 "/usr/include/bits/sigstack.h" 3 4
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
# 50 "/usr/include/bits/sigstack.h" 3 4
typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
# 349 "/usr/include/signal.h" 2 3 4


# 1 "/usr/include/sys/ucontext.h" 1 3 4
# 23 "/usr/include/sys/ucontext.h" 3 4
# 1 "/usr/include/signal.h" 1 3 4
# 24 "/usr/include/sys/ucontext.h" 2 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 25 "/usr/include/sys/ucontext.h" 2 3 4
# 33 "/usr/include/sys/ucontext.h" 3 4
typedef long int greg_t;





typedef greg_t gregset_t[23];



enum
{
  REG_R8 = 0,

  REG_R9,

  REG_R10,

  REG_R11,

  REG_R12,

  REG_R13,

  REG_R14,

  REG_R15,

  REG_RDI,

  REG_RSI,

  REG_RBP,

  REG_RBX,

  REG_RDX,

  REG_RAX,

  REG_RCX,

  REG_RSP,

  REG_RIP,

  REG_EFL,

  REG_CSGSFS,

  REG_ERR,

  REG_TRAPNO,

  REG_OLDMASK,

  REG_CR2

};


struct _libc_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int padding[3];
};

struct _libc_xmmreg
{
  __uint32_t element[4];
};

struct _libc_fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _libc_fpxreg _st[8];
  struct _libc_xmmreg _xmm[16];
  __uint32_t padding[24];
};


typedef struct _libc_fpstate *fpregset_t;


typedef struct
  {
    gregset_t gregs;

    fpregset_t fpregs;
    unsigned long __reserved1 [8];
} mcontext_t;


typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
  } ucontext_t;
# 352 "/usr/include/signal.h" 2 3 4





extern int sigstack (struct sigstack *__ss, struct sigstack *__oss)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));



extern int sigaltstack (__const struct sigaltstack *__restrict __ss,
   struct sigaltstack *__restrict __oss) __attribute__ ((__nothrow__));







extern int sighold (int __sig) __attribute__ ((__nothrow__));


extern int sigrelse (int __sig) __attribute__ ((__nothrow__));


extern int sigignore (int __sig) __attribute__ ((__nothrow__));


extern __sighandler_t sigset (int __sig, __sighandler_t __disp) __attribute__ ((__nothrow__));






# 1 "/usr/include/bits/sigthread.h" 1 3 4
# 31 "/usr/include/bits/sigthread.h" 3 4
extern int pthread_sigmask (int __how,
       __const __sigset_t *__restrict __newmask,
       __sigset_t *__restrict __oldmask)__attribute__ ((__nothrow__));


extern int pthread_kill (pthread_t __threadid, int __signo) __attribute__ ((__nothrow__));
# 388 "/usr/include/signal.h" 2 3 4






extern int __libc_current_sigrtmin (void) __attribute__ ((__nothrow__));

extern int __libc_current_sigrtmax (void) __attribute__ ((__nothrow__));




# 28 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/pwd.h" 1 3 4
# 28 "/usr/include/pwd.h" 3 4





# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 34 "/usr/include/pwd.h" 2 3 4
# 50 "/usr/include/pwd.h" 3 4
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
# 73 "/usr/include/pwd.h" 3 4
extern void setpwent (void);





extern void endpwent (void);





extern struct passwd *getpwent (void);
# 95 "/usr/include/pwd.h" 3 4
extern struct passwd *fgetpwent (FILE *__stream);







extern int putpwent (__const struct passwd *__restrict __p,
       FILE *__restrict __f);






extern struct passwd *getpwuid (__uid_t __uid);





extern struct passwd *getpwnam (__const char *__name);
# 140 "/usr/include/pwd.h" 3 4
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
# 164 "/usr/include/pwd.h" 3 4
extern int fgetpwent_r (FILE *__restrict __stream,
   struct passwd *__restrict __resultbuf,
   char *__restrict __buffer, size_t __buflen,
   struct passwd **__restrict __result);
# 181 "/usr/include/pwd.h" 3 4
extern int getpw (__uid_t __uid, char *__buffer);



# 29 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/grp.h" 1 3 4
# 28 "/usr/include/grp.h" 3 4





# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 34 "/usr/include/grp.h" 2 3 4
# 43 "/usr/include/grp.h" 3 4
struct group
  {
    char *gr_name;
    char *gr_passwd;
    __gid_t gr_gid;
    char **gr_mem;
  };
# 63 "/usr/include/grp.h" 3 4
extern void setgrent (void);





extern void endgrent (void);





extern struct group *getgrent (void);
# 85 "/usr/include/grp.h" 3 4
extern struct group *fgetgrent (FILE *__stream);
# 95 "/usr/include/grp.h" 3 4
extern int putgrent (__const struct group *__restrict __p,
       FILE *__restrict __f);






extern struct group *getgrgid (__gid_t __gid);





extern struct group *getgrnam (__const char *__name);
# 133 "/usr/include/grp.h" 3 4
extern int getgrent_r (struct group *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct group **__restrict __result);






extern int getgrgid_r (__gid_t __gid, struct group *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct group **__restrict __result);





extern int getgrnam_r (__const char *__restrict __name,
         struct group *__restrict __resultbuf,
         char *__restrict __buffer, size_t __buflen,
         struct group **__restrict __result);
# 163 "/usr/include/grp.h" 3 4
extern int fgetgrent_r (FILE *__restrict __stream,
   struct group *__restrict __resultbuf,
   char *__restrict __buffer, size_t __buflen,
   struct group **__restrict __result);
# 175 "/usr/include/grp.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 176 "/usr/include/grp.h" 2 3 4


extern int setgroups (size_t __n, __const __gid_t *__groups) __attribute__ ((__nothrow__));
# 188 "/usr/include/grp.h" 3 4
extern int getgrouplist (__const char *__user, __gid_t __group,
    __gid_t *__groups, int *__ngroups);
# 199 "/usr/include/grp.h" 3 4
extern int initgroups (__const char *__user, __gid_t __group);




# 30 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/dirent.h" 1 3 4
# 28 "/usr/include/dirent.h" 3 4

# 62 "/usr/include/dirent.h" 3 4
# 1 "/usr/include/bits/dirent.h" 1 3 4
# 23 "/usr/include/bits/dirent.h" 3 4
struct dirent
  {




    __ino64_t d_ino;
    __off64_t d_off;

    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[256];
  };


struct dirent64
  {
    __ino64_t d_ino;
    __off64_t d_off;
    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[256];
  };
# 63 "/usr/include/dirent.h" 2 3 4
# 98 "/usr/include/dirent.h" 3 4
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
# 128 "/usr/include/dirent.h" 3 4
typedef struct __dirstream DIR;






extern DIR *opendir (__const char *__name) __attribute__ ((__nonnull__ (1)));






extern DIR *fdopendir (int __fd);







extern int closedir (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
# 166 "/usr/include/dirent.h" 3 4
extern struct dirent *readdir (DIR *__dirp) __asm__ ("" "readdir64")
     __attribute__ ((__nonnull__ (1)));






extern struct dirent64 *readdir64 (DIR *__dirp) __attribute__ ((__nonnull__ (1)));
# 190 "/usr/include/dirent.h" 3 4
extern int readdir_r (DIR *__restrict __dirp, struct dirent *__restrict __entry, struct dirent **__restrict __result) __asm__ ("" "readdir64_r") __attribute__ ((__nonnull__ (1, 2, 3)));
# 201 "/usr/include/dirent.h" 3 4
extern int readdir64_r (DIR *__restrict __dirp,
   struct dirent64 *__restrict __entry,
   struct dirent64 **__restrict __result)
     __attribute__ ((__nonnull__ (1, 2, 3)));




extern void rewinddir (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern void seekdir (DIR *__dirp, long int __pos) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int telldir (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int dirfd (DIR *__dirp) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







# 1 "/usr/include/bits/posix1_lim.h" 1 3 4
# 153 "/usr/include/bits/posix1_lim.h" 3 4
# 1 "/usr/include/bits/local_lim.h" 1 3 4
# 36 "/usr/include/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 37 "/usr/include/bits/local_lim.h" 2 3 4
# 154 "/usr/include/bits/posix1_lim.h" 2 3 4
# 233 "/usr/include/dirent.h" 2 3 4
# 243 "/usr/include/dirent.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 244 "/usr/include/dirent.h" 2 3 4
# 257 "/usr/include/dirent.h" 3 4
extern int scandir (__const char *__restrict __dir, struct dirent ***__restrict __namelist, int (*__selector) (__const struct dirent *), int (*__cmp) (__const void *, __const void *)) __asm__ ("" "scandir64") __attribute__ ((__nonnull__ (1, 2)));
# 271 "/usr/include/dirent.h" 3 4
extern int scandir64 (__const char *__restrict __dir,
        struct dirent64 ***__restrict __namelist,
        int (*__selector) (__const struct dirent64 *),
        int (*__cmp) (__const void *, __const void *))
     __attribute__ ((__nonnull__ (1, 2)));
# 284 "/usr/include/dirent.h" 3 4
extern int alphasort (__const void *__e1, __const void *__e2) __asm__ ("" "alphasort64") __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 293 "/usr/include/dirent.h" 3 4
extern int alphasort64 (__const void *__e1, __const void *__e2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 304 "/usr/include/dirent.h" 3 4
extern int versionsort (__const void *__e1, __const void *__e2) __asm__ ("" "versionsort64") __attribute__ ((__nothrow__))


     __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));






extern int versionsort64 (__const void *__e1, __const void *__e2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 330 "/usr/include/dirent.h" 3 4
extern __ssize_t getdirentries (int __fd, char *__restrict __buf, size_t __nbytes, __off64_t *__restrict __basep) __asm__ ("" "getdirentries64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));
# 341 "/usr/include/dirent.h" 3 4
extern __ssize_t getdirentries64 (int __fd, char *__restrict __buf,
      size_t __nbytes,
      __off64_t *__restrict __basep)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));





# 31 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/glob.h" 1 3 4
# 24 "/usr/include/glob.h" 3 4

# 88 "/usr/include/glob.h" 3 4
struct stat;

typedef struct
  {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    int gl_flags;



    void (*gl_closedir) (void *);

    struct dirent *(*gl_readdir) (void *);



    void *(*gl_opendir) (__const char *);

    int (*gl_lstat) (__const char *__restrict, struct stat *__restrict);
    int (*gl_stat) (__const char *__restrict, struct stat *__restrict);




  } glob_t;



struct stat64;

typedef struct
  {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    int gl_flags;



    void (*gl_closedir) (void *);

    struct dirent64 *(*gl_readdir) (void *);



    void *(*gl_opendir) (__const char *);

    int (*gl_lstat) (__const char *__restrict, struct stat64 *__restrict);
    int (*gl_stat) (__const char *__restrict, struct stat64 *__restrict);




  } glob64_t;
# 166 "/usr/include/glob.h" 3 4
extern int glob (__const char *__restrict __pattern, int __flags, int (*__errfunc) (__const char *, int), glob_t *__restrict __pglob) __asm__ ("" "glob64") __attribute__ ((__nothrow__));




extern void globfree (glob_t *__pglob) __asm__ ("" "globfree64") __attribute__ ((__nothrow__));



extern int glob64 (__const char *__restrict __pattern, int __flags,
     int (*__errfunc) (__const char *, int),
     glob64_t *__restrict __pglob) __attribute__ ((__nothrow__));

extern void globfree64 (glob64_t *__pglob) __attribute__ ((__nothrow__));
# 189 "/usr/include/glob.h" 3 4
extern int glob_pattern_p (__const char *__pattern, int __quote) __attribute__ ((__nothrow__));



# 32 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/vfs.h" 1 3 4



# 1 "/usr/include/sys/statfs.h" 1 3 4
# 26 "/usr/include/sys/statfs.h" 3 4
# 1 "/usr/include/bits/statfs.h" 1 3 4
# 25 "/usr/include/bits/statfs.h" 3 4
struct statfs
  {
    long int f_type;
    long int f_bsize;







    __fsblkcnt64_t f_blocks;
    __fsblkcnt64_t f_bfree;
    __fsblkcnt64_t f_bavail;
    __fsfilcnt64_t f_files;
    __fsfilcnt64_t f_ffree;

    __fsid_t f_fsid;
    long int f_namelen;
    long int f_frsize;
    long int f_spare[5];
  };


struct statfs64
  {
    long int f_type;
    long int f_bsize;
    __fsblkcnt64_t f_blocks;
    __fsblkcnt64_t f_bfree;
    __fsblkcnt64_t f_bavail;
    __fsfilcnt64_t f_files;
    __fsfilcnt64_t f_ffree;
    __fsid_t f_fsid;
    long int f_namelen;
    long int f_frsize;
    long int f_spare[5];
  };
# 27 "/usr/include/sys/statfs.h" 2 3 4









extern int statfs (__const char *__file, struct statfs *__buf) __asm__ ("" "statfs64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));







extern int statfs64 (__const char *__file, struct statfs64 *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 55 "/usr/include/sys/statfs.h" 3 4
extern int fstatfs (int __fildes, struct statfs *__buf) __asm__ ("" "fstatfs64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));






extern int fstatfs64 (int __fildes, struct statfs64 *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



# 4 "/usr/include/sys/vfs.h" 2 3 4
# 33 "src/os/unix/ngx_linux_config.h" 2

# 1 "/usr/include/sys/uio.h" 1 3 4
# 26 "/usr/include/sys/uio.h" 3 4



# 1 "/usr/include/bits/uio.h" 1 3 4
# 44 "/usr/include/bits/uio.h" 3 4
struct iovec
  {
    void *iov_base;
    size_t iov_len;
  };
# 30 "/usr/include/sys/uio.h" 2 3 4
# 40 "/usr/include/sys/uio.h" 3 4
extern ssize_t readv (int __fd, __const struct iovec *__iovec, int __count);
# 50 "/usr/include/sys/uio.h" 3 4
extern ssize_t writev (int __fd, __const struct iovec *__iovec, int __count);


# 35 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/stat.h" 1 3 4
# 37 "/usr/include/sys/stat.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 38 "/usr/include/sys/stat.h" 2 3 4
# 103 "/usr/include/sys/stat.h" 3 4


# 1 "/usr/include/bits/stat.h" 1 3 4
# 43 "/usr/include/bits/stat.h" 3 4
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
# 88 "/usr/include/bits/stat.h" 3 4
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 103 "/usr/include/bits/stat.h" 3 4
    long int __unused[3];
# 112 "/usr/include/bits/stat.h" 3 4
  };



struct stat64
  {
    __dev_t st_dev;

    __ino64_t st_ino;
    __nlink_t st_nlink;
    __mode_t st_mode;






    __uid_t st_uid;
    __gid_t st_gid;

    int pad0;
    __dev_t st_rdev;
    __off_t st_size;





    __blksize_t st_blksize;
    __blkcnt64_t st_blocks;







    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 164 "/usr/include/bits/stat.h" 3 4
    long int __unused[3];



  };
# 106 "/usr/include/sys/stat.h" 2 3 4
# 215 "/usr/include/sys/stat.h" 3 4
extern int stat (__const char *__restrict __file, struct stat *__restrict __buf) __asm__ ("" "stat64") __attribute__ ((__nothrow__))

     __attribute__ ((__nonnull__ (1, 2)));
extern int fstat (int __fd, struct stat *__buf) __asm__ ("" "fstat64") __attribute__ ((__nothrow__))
     __attribute__ ((__nonnull__ (2)));






extern int stat64 (__const char *__restrict __file,
     struct stat64 *__restrict __buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int fstat64 (int __fd, struct stat64 *__buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 241 "/usr/include/sys/stat.h" 3 4
extern int fstatat (int __fd, __const char *__restrict __file, struct stat *__restrict __buf, int __flag) __asm__ ("" "fstatat64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 250 "/usr/include/sys/stat.h" 3 4
extern int fstatat64 (int __fd, __const char *__restrict __file,
        struct stat64 *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 263 "/usr/include/sys/stat.h" 3 4
extern int lstat (__const char *__restrict __file, struct stat *__restrict __buf) __asm__ ("" "lstat64") __attribute__ ((__nothrow__))


     __attribute__ ((__nonnull__ (1, 2)));





extern int lstat64 (__const char *__restrict __file,
      struct stat64 *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern int chmod (__const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int lchmod (__const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




extern int fchmod (int __fd, __mode_t __mode) __attribute__ ((__nothrow__));





extern int fchmodat (int __fd, __const char *__file, __mode_t mode, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2))) ;






extern __mode_t umask (__mode_t __mask) __attribute__ ((__nothrow__));




extern __mode_t getumask (void) __attribute__ ((__nothrow__));



extern int mkdir (__const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int mkdirat (int __fd, __const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));






extern int mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int mknodat (int __fd, __const char *__path, __mode_t __mode,
      __dev_t __dev) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern int mkfifo (__const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int mkfifoat (int __fd, __const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 391 "/usr/include/sys/stat.h" 3 4
extern int __fxstat (int __ver, int __fildes, struct stat *__stat_buf) __asm__ ("" "__fxstat64") __attribute__ ((__nothrow__))

     __attribute__ ((__nonnull__ (3)));
extern int __xstat (int __ver, __const char *__filename, struct stat *__stat_buf) __asm__ ("" "__xstat64") __attribute__ ((__nothrow__))

     __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat (int __ver, __const char *__filename, struct stat *__stat_buf) __asm__ ("" "__lxstat64") __attribute__ ((__nothrow__))

     __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat (int __ver, int __fildes, __const char *__filename, struct stat *__stat_buf, int __flag) __asm__ ("" "__fxstatat64") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4)));
# 413 "/usr/include/sys/stat.h" 3 4
extern int __fxstat64 (int __ver, int __fildes, struct stat64 *__stat_buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3)));
extern int __xstat64 (int __ver, __const char *__filename,
        struct stat64 *__stat_buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat64 (int __ver, __const char *__filename,
         struct stat64 *__stat_buf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat64 (int __ver, int __fildes, __const char *__filename,
    struct stat64 *__stat_buf, int __flag)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4)));

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


extern __inline__ int
__attribute__ ((__nothrow__)) fstatat (int __fd, __const char *__filename, struct stat *__statbuf, int __flag)

{
  return __fxstatat (1, __fd, __filename, __statbuf, __flag);
}



extern __inline__ int
__attribute__ ((__nothrow__)) mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
{
  return __xmknod (0, __path, __mode, &__dev);
}



extern __inline__ int
__attribute__ ((__nothrow__)) mknodat (int __fd, __const char *__path, __mode_t __mode, __dev_t __dev)

{
  return __xmknodat (0, __fd, __path, __mode, &__dev);
}
# 515 "/usr/include/sys/stat.h" 3 4

# 36 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/fcntl.h" 1 3 4
# 30 "/usr/include/fcntl.h" 3 4




# 1 "/usr/include/bits/fcntl.h" 1 3 4
# 25 "/usr/include/bits/fcntl.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 26 "/usr/include/bits/fcntl.h" 2 3 4

# 1 "/usr/include/bits/uio.h" 1 3 4
# 28 "/usr/include/bits/fcntl.h" 2 3 4
# 155 "/usr/include/bits/fcntl.h" 3 4
struct flock
  {
    short int l_type;
    short int l_whence;




    __off64_t l_start;
    __off64_t l_len;

    __pid_t l_pid;
  };


struct flock64
  {
    short int l_type;
    short int l_whence;
    __off64_t l_start;
    __off64_t l_len;
    __pid_t l_pid;
  };
# 222 "/usr/include/bits/fcntl.h" 3 4





extern ssize_t readahead (int __fd, __off64_t __offset, size_t __count)
    __attribute__ ((__nothrow__));



extern int sync_file_range (int __fd, __off64_t __from, __off64_t __to,
       unsigned int __flags);



extern int vmsplice (int __fdout, const struct iovec *__iov, size_t __count,
       unsigned int __flags);


extern int splice (int __fdin, __off64_t *__offin, int __fdout,
     __off64_t *__offout, size_t __len, unsigned int __flags)
    __attribute__ ((__nothrow__));


extern int tee (int __fdin, int __fdout, size_t __len, unsigned int __flags)
    __attribute__ ((__nothrow__));




# 35 "/usr/include/fcntl.h" 2 3 4
# 76 "/usr/include/fcntl.h" 3 4
extern int fcntl (int __fd, int __cmd, ...);
# 88 "/usr/include/fcntl.h" 3 4
extern int open (__const char *__file, int __oflag, ...) __asm__ ("" "open64")
     __attribute__ ((__nonnull__ (1)));





extern int open64 (__const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
# 113 "/usr/include/fcntl.h" 3 4
extern int openat (int __fd, __const char *__file, int __oflag, ...) __asm__ ("" "openat64") __attribute__ ((__nonnull__ (2)));






extern int openat64 (int __fd, __const char *__file, int __oflag, ...)
     __attribute__ ((__nonnull__ (2)));
# 133 "/usr/include/fcntl.h" 3 4
extern int creat (__const char *__file, __mode_t __mode) __asm__ ("" "creat64") __attribute__ ((__nonnull__ (1)));






extern int creat64 (__const char *__file, __mode_t __mode) __attribute__ ((__nonnull__ (1)));
# 180 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise (int __fd, __off64_t __offset, __off64_t __len, int __advise) __asm__ ("" "posix_fadvise64") __attribute__ ((__nothrow__));







extern int posix_fadvise64 (int __fd, __off64_t __offset, __off64_t __len,
       int __advise) __attribute__ ((__nothrow__));
# 201 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate (int __fd, __off64_t __offset, __off64_t __len) __asm__ ("" "posix_fallocate64");







extern int posix_fallocate64 (int __fd, __off64_t __offset, __off64_t __len);




# 37 "src/os/unix/ngx_linux_config.h" 2

# 1 "/usr/include/sys/wait.h" 1 3 4
# 29 "/usr/include/sys/wait.h" 3 4



# 1 "/usr/include/sys/resource.h" 1 3 4
# 25 "/usr/include/sys/resource.h" 3 4
# 1 "/usr/include/bits/resource.h" 1 3 4
# 33 "/usr/include/bits/resource.h" 3 4
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
# 129 "/usr/include/bits/resource.h" 3 4
typedef __rlim64_t rlim_t;


typedef __rlim64_t rlim64_t;


struct rlimit
  {

    rlim_t rlim_cur;

    rlim_t rlim_max;
  };


struct rlimit64
  {

    rlim64_t rlim_cur;

    rlim64_t rlim_max;
 };



enum __rusage_who
{

  RUSAGE_SELF = 0,



  RUSAGE_CHILDREN = -1,




  RUSAGE_THREAD = 1




};


# 1 "/usr/include/bits/time.h" 1 3 4
# 175 "/usr/include/bits/resource.h" 2 3 4


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
# 26 "/usr/include/sys/resource.h" 2 3 4













typedef enum __rlimit_resource __rlimit_resource_t;
typedef enum __rusage_who __rusage_who_t;
typedef enum __priority_which __priority_which_t;
# 55 "/usr/include/sys/resource.h" 3 4
extern int getrlimit (__rlimit_resource_t __resource, struct rlimit *__rlimits) __asm__ ("" "getrlimit64") __attribute__ ((__nothrow__));






extern int getrlimit64 (__rlimit_resource_t __resource,
   struct rlimit64 *__rlimits) __attribute__ ((__nothrow__));
# 74 "/usr/include/sys/resource.h" 3 4
extern int setrlimit (__rlimit_resource_t __resource, __const struct rlimit *__rlimits) __asm__ ("" "setrlimit64") __attribute__ ((__nothrow__));







extern int setrlimit64 (__rlimit_resource_t __resource,
   __const struct rlimit64 *__rlimits) __attribute__ ((__nothrow__));




extern int getrusage (__rusage_who_t __who, struct rusage *__usage) __attribute__ ((__nothrow__));





extern int getpriority (__priority_which_t __which, id_t __who) __attribute__ ((__nothrow__));



extern int setpriority (__priority_which_t __which, id_t __who, int __prio)
     __attribute__ ((__nothrow__));


# 33 "/usr/include/sys/wait.h" 2 3 4
# 102 "/usr/include/sys/wait.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 116 "/usr/include/sys/wait.h" 3 4
extern __pid_t wait (__WAIT_STATUS __stat_loc);
# 139 "/usr/include/sys/wait.h" 3 4
extern __pid_t waitpid (__pid_t __pid, int *__stat_loc, int __options);



# 1 "/usr/include/bits/siginfo.h" 1 3 4
# 25 "/usr/include/bits/siginfo.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 26 "/usr/include/bits/siginfo.h" 2 3 4
# 144 "/usr/include/sys/wait.h" 2 3 4
# 155 "/usr/include/sys/wait.h" 3 4
extern int waitid (idtype_t __idtype, __id_t __id, siginfo_t *__infop,
     int __options);





struct rusage;






extern __pid_t wait3 (__WAIT_STATUS __stat_loc, int __options,
        struct rusage * __usage) __attribute__ ((__nothrow__));





struct rusage;


extern __pid_t wait4 (__pid_t __pid, __WAIT_STATUS __stat_loc, int __options,
        struct rusage *__usage) __attribute__ ((__nothrow__));




# 39 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/mman.h" 1 3 4
# 26 "/usr/include/sys/mman.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 27 "/usr/include/sys/mman.h" 2 3 4
# 42 "/usr/include/sys/mman.h" 3 4
# 1 "/usr/include/bits/mman.h" 1 3 4
# 43 "/usr/include/sys/mman.h" 2 3 4





# 62 "/usr/include/sys/mman.h" 3 4
extern void * mmap (void *__addr, size_t __len, int __prot, int __flags, int __fd, __off64_t __offset) __asm__ ("" "mmap64") __attribute__ ((__nothrow__));
# 71 "/usr/include/sys/mman.h" 3 4
extern void *mmap64 (void *__addr, size_t __len, int __prot,
       int __flags, int __fd, __off64_t __offset) __attribute__ ((__nothrow__));




extern int munmap (void *__addr, size_t __len) __attribute__ ((__nothrow__));




extern int mprotect (void *__addr, size_t __len, int __prot) __attribute__ ((__nothrow__));







extern int msync (void *__addr, size_t __len, int __flags);




extern int madvise (void *__addr, size_t __len, int __advice) __attribute__ ((__nothrow__));



extern int posix_madvise (void *__addr, size_t __len, int __advice) __attribute__ ((__nothrow__));




extern int mlock (__const void *__addr, size_t __len) __attribute__ ((__nothrow__));


extern int munlock (__const void *__addr, size_t __len) __attribute__ ((__nothrow__));




extern int mlockall (int __flags) __attribute__ ((__nothrow__));



extern int munlockall (void) __attribute__ ((__nothrow__));







extern int mincore (void *__start, size_t __len, unsigned char *__vec)
     __attribute__ ((__nothrow__));
# 134 "/usr/include/sys/mman.h" 3 4
extern void *mremap (void *__addr, size_t __old_len, size_t __new_len,
       int __flags, ...) __attribute__ ((__nothrow__));



extern int remap_file_pages (void *__start, size_t __size, int __prot,
        size_t __pgoff, int __flags) __attribute__ ((__nothrow__));




extern int shm_open (__const char *__name, int __oflag, mode_t __mode);


extern int shm_unlink (__const char *__name);


# 40 "src/os/unix/ngx_linux_config.h" 2

# 1 "/usr/include/sched.h" 1 3 4
# 29 "/usr/include/sched.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 30 "/usr/include/sched.h" 2 3 4


# 1 "/usr/include/bits/sched.h" 1 3 4
# 65 "/usr/include/bits/sched.h" 3 4
struct sched_param
  {
    int __sched_priority;
  };





extern int clone (int (*__fn) (void *__arg), void *__child_stack,
    int __flags, void *__arg, ...) __attribute__ ((__nothrow__));


extern int unshare (int __flags) __attribute__ ((__nothrow__));










struct __sched_param
  {
    int __sched_priority;
  };
# 104 "/usr/include/bits/sched.h" 3 4
typedef unsigned long int __cpu_mask;






typedef struct
{
  __cpu_mask __bits[1024 / (8 * sizeof (__cpu_mask))];
} cpu_set_t;
# 33 "/usr/include/sched.h" 2 3 4







extern int sched_setparam (__pid_t __pid, __const struct sched_param *__param)
     __attribute__ ((__nothrow__));


extern int sched_getparam (__pid_t __pid, struct sched_param *__param) __attribute__ ((__nothrow__));


extern int sched_setscheduler (__pid_t __pid, int __policy,
          __const struct sched_param *__param) __attribute__ ((__nothrow__));


extern int sched_getscheduler (__pid_t __pid) __attribute__ ((__nothrow__));


extern int sched_yield (void) __attribute__ ((__nothrow__));


extern int sched_get_priority_max (int __algorithm) __attribute__ ((__nothrow__));


extern int sched_get_priority_min (int __algorithm) __attribute__ ((__nothrow__));


extern int sched_rr_get_interval (__pid_t __pid, struct timespec *__t) __attribute__ ((__nothrow__));
# 76 "/usr/include/sched.h" 3 4
extern int sched_setaffinity (__pid_t __pid, size_t __cpusetsize,
         __const cpu_set_t *__cpuset) __attribute__ ((__nothrow__));


extern int sched_getaffinity (__pid_t __pid, size_t __cpusetsize,
         cpu_set_t *__cpuset) __attribute__ ((__nothrow__));



# 42 "src/os/unix/ngx_linux_config.h" 2

# 1 "/usr/include/sys/socket.h" 1 3 4
# 25 "/usr/include/sys/socket.h" 3 4




# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 30 "/usr/include/sys/socket.h" 2 3 4





# 1 "/usr/include/bits/socket.h" 1 3 4
# 29 "/usr/include/bits/socket.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 30 "/usr/include/bits/socket.h" 2 3 4

# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
# 11 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/syslimits.h" 1 3 4






# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
# 122 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 149 "/usr/include/limits.h" 3 4
# 1 "/usr/include/bits/posix2_lim.h" 1 3 4
# 150 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/bits/xopen_lim.h" 1 3 4
# 34 "/usr/include/bits/xopen_lim.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 35 "/usr/include/bits/xopen_lim.h" 2 3 4
# 154 "/usr/include/limits.h" 2 3 4
# 123 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 2 3 4
# 8 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/syslimits.h" 2 3 4
# 12 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 2 3 4
# 32 "/usr/include/bits/socket.h" 2 3 4
# 41 "/usr/include/bits/socket.h" 3 4
enum __socket_type
{
  SOCK_STREAM = 1,


  SOCK_DGRAM = 2,


  SOCK_RAW = 3,

  SOCK_RDM = 4,

  SOCK_SEQPACKET = 5,


  SOCK_PACKET = 10



};
# 142 "/usr/include/bits/socket.h" 3 4
# 1 "/usr/include/bits/sockaddr.h" 1 3 4
# 29 "/usr/include/bits/sockaddr.h" 3 4
typedef unsigned short int sa_family_t;
# 143 "/usr/include/bits/socket.h" 2 3 4


struct sockaddr
  {
    sa_family_t sa_family;
    char sa_data[14];
  };
# 162 "/usr/include/bits/socket.h" 3 4
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



    MSG_TRYHARD = MSG_DONTROUTE,


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
# 263 "/usr/include/bits/socket.h" 3 4
extern struct cmsghdr *__cmsg_nxthdr (struct msghdr *__mhdr,
          struct cmsghdr *__cmsg) __attribute__ ((__nothrow__));
# 290 "/usr/include/bits/socket.h" 3 4
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


# 1 "/usr/include/asm/socket.h" 1 3 4




# 1 "/usr/include/asm-x86_64/socket.h" 1 3 4



# 1 "/usr/include/asm/sockios.h" 1 3 4




# 1 "/usr/include/asm-x86_64/sockios.h" 1 3 4
# 6 "/usr/include/asm/sockios.h" 2 3 4
# 5 "/usr/include/asm-x86_64/socket.h" 2 3 4
# 6 "/usr/include/asm/socket.h" 2 3 4
# 311 "/usr/include/bits/socket.h" 2 3 4



struct linger
  {
    int l_onoff;
    int l_linger;
  };
# 36 "/usr/include/sys/socket.h" 2 3 4




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
# 87 "/usr/include/sys/socket.h" 3 4
typedef union { struct sockaddr *__restrict __sockaddr__; struct sockaddr_at *__restrict __sockaddr_at__; struct sockaddr_ax25 *__restrict __sockaddr_ax25__; struct sockaddr_dl *__restrict __sockaddr_dl__; struct sockaddr_eon *__restrict __sockaddr_eon__; struct sockaddr_in *__restrict __sockaddr_in__; struct sockaddr_in6 *__restrict __sockaddr_in6__; struct sockaddr_inarp *__restrict __sockaddr_inarp__; struct sockaddr_ipx *__restrict __sockaddr_ipx__; struct sockaddr_iso *__restrict __sockaddr_iso__; struct sockaddr_ns *__restrict __sockaddr_ns__; struct sockaddr_un *__restrict __sockaddr_un__; struct sockaddr_x25 *__restrict __sockaddr_x25__;
       } __SOCKADDR_ARG __attribute__ ((__transparent_union__));


typedef union { __const struct sockaddr *__restrict __sockaddr__; __const struct sockaddr_at *__restrict __sockaddr_at__; __const struct sockaddr_ax25 *__restrict __sockaddr_ax25__; __const struct sockaddr_dl *__restrict __sockaddr_dl__; __const struct sockaddr_eon *__restrict __sockaddr_eon__; __const struct sockaddr_in *__restrict __sockaddr_in__; __const struct sockaddr_in6 *__restrict __sockaddr_in6__; __const struct sockaddr_inarp *__restrict __sockaddr_inarp__; __const struct sockaddr_ipx *__restrict __sockaddr_ipx__; __const struct sockaddr_iso *__restrict __sockaddr_iso__; __const struct sockaddr_ns *__restrict __sockaddr_ns__; __const struct sockaddr_un *__restrict __sockaddr_un__; __const struct sockaddr_x25 *__restrict __sockaddr_x25__;
       } __CONST_SOCKADDR_ARG __attribute__ ((__transparent_union__));







extern int socket (int __domain, int __type, int __protocol) __attribute__ ((__nothrow__));





extern int socketpair (int __domain, int __type, int __protocol,
         int __fds[2]) __attribute__ ((__nothrow__));


extern int bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
     __attribute__ ((__nothrow__));


extern int getsockname (int __fd, __SOCKADDR_ARG __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__));
# 124 "/usr/include/sys/socket.h" 3 4
extern int connect (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);



extern int getpeername (int __fd, __SOCKADDR_ARG __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__));






extern ssize_t send (int __fd, __const void *__buf, size_t __n, int __flags);






extern ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);






extern ssize_t sendto (int __fd, __const void *__buf, size_t __n,
         int __flags, __CONST_SOCKADDR_ARG __addr,
         socklen_t __addr_len);
# 161 "/usr/include/sys/socket.h" 3 4
extern ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n,
    int __flags, __SOCKADDR_ARG __addr,
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
# 209 "/usr/include/sys/socket.h" 3 4
extern int accept (int __fd, __SOCKADDR_ARG __addr,
     socklen_t *__restrict __addr_len);







extern int shutdown (int __fd, int __how) __attribute__ ((__nothrow__));




extern int sockatmark (int __fd) __attribute__ ((__nothrow__));







extern int isfdtype (int __fd, int __fdtype) __attribute__ ((__nothrow__));
# 240 "/usr/include/sys/socket.h" 3 4

# 44 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/netinet/in.h" 1 3 4
# 23 "/usr/include/netinet/in.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 28 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/stdint.h" 2 3 4
# 49 "/usr/include/stdint.h" 3 4
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;
# 66 "/usr/include/stdint.h" 3 4
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;
# 91 "/usr/include/stdint.h" 3 4
typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 104 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 123 "/usr/include/stdint.h" 3 4
typedef unsigned long int uintptr_t;
# 135 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
# 24 "/usr/include/netinet/in.h" 2 3 4







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
# 193 "/usr/include/netinet/in.h" 3 4
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
# 219 "/usr/include/netinet/in.h" 3 4
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
# 345 "/usr/include/netinet/in.h" 3 4
# 1 "/usr/include/bits/in.h" 1 3 4
# 82 "/usr/include/bits/in.h" 3 4
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
# 346 "/usr/include/netinet/in.h" 2 3 4
# 354 "/usr/include/netinet/in.h" 3 4
extern uint32_t ntohl (uint32_t __netlong) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));




# 1 "/usr/include/bits/byteswap.h" 1 3 4
# 27 "/usr/include/bits/byteswap.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 28 "/usr/include/bits/byteswap.h" 2 3 4
# 366 "/usr/include/netinet/in.h" 2 3 4
# 428 "/usr/include/netinet/in.h" 3 4
extern int bindresvport (int __sockfd, struct sockaddr_in *__sock_in) __attribute__ ((__nothrow__));


extern int bindresvport6 (int __sockfd, struct sockaddr_in6 *__sock_in)
     __attribute__ ((__nothrow__));
# 456 "/usr/include/netinet/in.h" 3 4
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




extern int inet6_option_space (int __nbytes)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
extern int inet6_option_init (void *__bp, struct cmsghdr **__cmsgp,
         int __type) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
extern int inet6_option_append (struct cmsghdr *__cmsg,
    __const uint8_t *__typep, int __multx,
    int __plusy) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
extern uint8_t *inet6_option_alloc (struct cmsghdr *__cmsg, int __datalen,
        int __multx, int __plusy)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
extern int inet6_option_next (__const struct cmsghdr *__cmsg,
         uint8_t **__tptrp)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
extern int inet6_option_find (__const struct cmsghdr *__cmsg,
         uint8_t **__tptrp, int __type)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));



extern int inet6_opt_init (void *__extbuf, socklen_t __extlen) __attribute__ ((__nothrow__));
extern int inet6_opt_append (void *__extbuf, socklen_t __extlen, int __offset,
        uint8_t __type, socklen_t __len, uint8_t __align,
        void **__databufp) __attribute__ ((__nothrow__));
extern int inet6_opt_finish (void *__extbuf, socklen_t __extlen, int __offset)
     __attribute__ ((__nothrow__));
extern int inet6_opt_set_val (void *__databuf, int __offset, void *__val,
         socklen_t __vallen) __attribute__ ((__nothrow__));
extern int inet6_opt_next (void *__extbuf, socklen_t __extlen, int __offset,
      uint8_t *__typep, socklen_t *__lenp,
      void **__databufp) __attribute__ ((__nothrow__));
extern int inet6_opt_find (void *__extbuf, socklen_t __extlen, int __offset,
      uint8_t __type, socklen_t *__lenp,
      void **__databufp) __attribute__ ((__nothrow__));
extern int inet6_opt_get_val (void *__databuf, int __offset, void *__val,
         socklen_t __vallen) __attribute__ ((__nothrow__));



extern socklen_t inet6_rth_space (int __type, int __segments) __attribute__ ((__nothrow__));
extern void *inet6_rth_init (void *__bp, socklen_t __bp_len, int __type,
        int __segments) __attribute__ ((__nothrow__));
extern int inet6_rth_add (void *__bp, __const struct in6_addr *__addr) __attribute__ ((__nothrow__));
extern int inet6_rth_reverse (__const void *__in, void *__out) __attribute__ ((__nothrow__));
extern int inet6_rth_segments (__const void *__bp) __attribute__ ((__nothrow__));
extern struct in6_addr *inet6_rth_getaddr (__const void *__bp, int __index)
     __attribute__ ((__nothrow__));





extern int getipv4sourcefilter (int __s, struct in_addr __interface_addr,
    struct in_addr __group, uint32_t *__fmode,
    uint32_t *__numsrc, struct in_addr *__slist)
     __attribute__ ((__nothrow__));


extern int setipv4sourcefilter (int __s, struct in_addr __interface_addr,
    struct in_addr __group, uint32_t __fmode,
    uint32_t __numsrc,
    __const struct in_addr *__slist)
     __attribute__ ((__nothrow__));



extern int getsourcefilter (int __s, uint32_t __interface_addr,
       __const struct sockaddr *__group,
       socklen_t __grouplen, uint32_t *__fmode,
       uint32_t *__numsrc,
       struct sockaddr_storage *__slist) __attribute__ ((__nothrow__));


extern int setsourcefilter (int __s, uint32_t __interface_addr,
       __const struct sockaddr *__group,
       socklen_t __grouplen, uint32_t __fmode,
       uint32_t __numsrc,
       __const struct sockaddr_storage *__slist) __attribute__ ((__nothrow__));



# 45 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/netinet/tcp.h" 1 3 4
# 89 "/usr/include/netinet/tcp.h" 3 4
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
# 118 "/usr/include/netinet/tcp.h" 3 4
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
# 176 "/usr/include/netinet/tcp.h" 3 4
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
# 46 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/arpa/inet.h" 1 3 4
# 31 "/usr/include/arpa/inet.h" 3 4




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



# 47 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/netdb.h" 1 3 4
# 33 "/usr/include/netdb.h" 3 4
# 1 "/usr/include/rpc/netdb.h" 1 3 4
# 42 "/usr/include/rpc/netdb.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 43 "/usr/include/rpc/netdb.h" 2 3 4



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



# 34 "/usr/include/netdb.h" 2 3 4




# 1 "/usr/include/bits/siginfo.h" 1 3 4
# 25 "/usr/include/bits/siginfo.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 26 "/usr/include/bits/siginfo.h" 2 3 4
# 39 "/usr/include/netdb.h" 2 3 4

# 1 "/usr/include/time.h" 1 3 4
# 41 "/usr/include/netdb.h" 2 3 4


# 1 "/usr/include/bits/netdb.h" 1 3 4
# 27 "/usr/include/bits/netdb.h" 3 4
struct netent
{
  char *n_name;
  char **n_aliases;
  int n_addrtype;
  uint32_t n_net;
};
# 44 "/usr/include/netdb.h" 2 3 4
# 54 "/usr/include/netdb.h" 3 4







extern int *__h_errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 88 "/usr/include/netdb.h" 3 4
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
# 149 "/usr/include/netdb.h" 3 4
extern struct hostent *gethostbyname2 (__const char *__name, int __af);
# 161 "/usr/include/netdb.h" 3 4
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
# 192 "/usr/include/netdb.h" 3 4
extern void setnetent (int __stay_open);





extern void endnetent (void);






extern struct netent *getnetent (void);






extern struct netent *getnetbyaddr (uint32_t __net, int __type);





extern struct netent *getnetbyname (__const char *__name);
# 231 "/usr/include/netdb.h" 3 4
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
# 303 "/usr/include/netdb.h" 3 4
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
# 369 "/usr/include/netdb.h" 3 4
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
# 390 "/usr/include/netdb.h" 3 4
extern int setnetgrent (__const char *__netgroup);







extern void endnetgrent (void);
# 407 "/usr/include/netdb.h" 3 4
extern int getnetgrent (char **__restrict __hostp,
   char **__restrict __userp,
   char **__restrict __domainp);
# 418 "/usr/include/netdb.h" 3 4
extern int innetgr (__const char *__netgroup, __const char *__host,
      __const char *__user, __const char *domain);







extern int getnetgrent_r (char **__restrict __hostp,
     char **__restrict __userp,
     char **__restrict __domainp,
     char *__restrict __buffer, size_t __buflen);
# 446 "/usr/include/netdb.h" 3 4
extern int rcmd (char **__restrict __ahost, unsigned short int __rport,
   __const char *__restrict __locuser,
   __const char *__restrict __remuser,
   __const char *__restrict __cmd, int *__restrict __fd2p);
# 458 "/usr/include/netdb.h" 3 4
extern int rcmd_af (char **__restrict __ahost, unsigned short int __rport,
      __const char *__restrict __locuser,
      __const char *__restrict __remuser,
      __const char *__restrict __cmd, int *__restrict __fd2p,
      sa_family_t __af);
# 474 "/usr/include/netdb.h" 3 4
extern int rexec (char **__restrict __ahost, int __rport,
    __const char *__restrict __name,
    __const char *__restrict __pass,
    __const char *__restrict __cmd, int *__restrict __fd2p);
# 486 "/usr/include/netdb.h" 3 4
extern int rexec_af (char **__restrict __ahost, int __rport,
       __const char *__restrict __name,
       __const char *__restrict __pass,
       __const char *__restrict __cmd, int *__restrict __fd2p,
       sa_family_t __af);
# 500 "/usr/include/netdb.h" 3 4
extern int ruserok (__const char *__rhost, int __suser,
      __const char *__remuser, __const char *__locuser);
# 510 "/usr/include/netdb.h" 3 4
extern int ruserok_af (__const char *__rhost, int __suser,
         __const char *__remuser, __const char *__locuser,
         sa_family_t __af);
# 522 "/usr/include/netdb.h" 3 4
extern int rresvport (int *__alport);
# 531 "/usr/include/netdb.h" 3 4
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



struct gaicb
{
  const char *ar_name;
  const char *ar_service;
  const struct addrinfo *ar_request;
  struct addrinfo *ar_result;

  int __return;
  int __unused[5];
};
# 631 "/usr/include/netdb.h" 3 4
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
# 661 "/usr/include/netdb.h" 3 4
extern int getaddrinfo_a (int __mode, struct gaicb *__list[__restrict],
     int __ent, struct sigevent *__restrict __sig);
# 672 "/usr/include/netdb.h" 3 4
extern int gai_suspend (__const struct gaicb *__const __list[], int __ent,
   __const struct timespec *__timeout);


extern int gai_error (struct gaicb *__req) __attribute__ ((__nothrow__));


extern int gai_cancel (struct gaicb *__gaicbp) __attribute__ ((__nothrow__));



# 48 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/un.h" 1 3 4
# 27 "/usr/include/sys/un.h" 3 4



struct sockaddr_un
  {
    sa_family_t sun_family;
    char sun_path[108];
  };
# 45 "/usr/include/sys/un.h" 3 4

# 49 "src/os/unix/ngx_linux_config.h" 2

# 1 "/usr/include/time.h" 1 3 4
# 31 "/usr/include/time.h" 3 4








# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 40 "/usr/include/time.h" 2 3 4



# 1 "/usr/include/bits/time.h" 1 3 4
# 44 "/usr/include/time.h" 2 3 4
# 132 "/usr/include/time.h" 3 4


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
# 181 "/usr/include/time.h" 3 4



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
# 312 "/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__));


extern int dysize (int __year) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 327 "/usr/include/time.h" 3 4
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
# 389 "/usr/include/time.h" 3 4
extern int getdate_err;
# 398 "/usr/include/time.h" 3 4
extern struct tm *getdate (__const char *__string);
# 412 "/usr/include/time.h" 3 4
extern int getdate_r (__const char *__restrict __string,
        struct tm *__restrict __resbufp);



# 51 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/malloc.h" 1 3 4
# 24 "/usr/include/malloc.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 25 "/usr/include/malloc.h" 2 3 4
# 46 "/usr/include/malloc.h" 3 4



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
# 130 "/usr/include/malloc.h" 3 4
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



# 52 "src/os/unix/ngx_linux_config.h" 2
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
# 53 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/ioctl.h" 1 3 4
# 24 "/usr/include/sys/ioctl.h" 3 4



# 1 "/usr/include/bits/ioctls.h" 1 3 4
# 24 "/usr/include/bits/ioctls.h" 3 4
# 1 "/usr/include/asm/ioctls.h" 1 3 4




# 1 "/usr/include/asm-x86_64/ioctls.h" 1 3 4



# 1 "/usr/include/asm/ioctl.h" 1 3 4




# 1 "/usr/include/asm-x86_64/ioctl.h" 1 3 4
# 1 "/usr/include/asm-generic/ioctl.h" 1 3 4
# 1 "/usr/include/asm-x86_64/ioctl.h" 2 3 4
# 6 "/usr/include/asm/ioctl.h" 2 3 4
# 5 "/usr/include/asm-x86_64/ioctls.h" 2 3 4
# 6 "/usr/include/asm/ioctls.h" 2 3 4
# 25 "/usr/include/bits/ioctls.h" 2 3 4
# 28 "/usr/include/sys/ioctl.h" 2 3 4


# 1 "/usr/include/bits/ioctl-types.h" 1 3 4
# 28 "/usr/include/bits/ioctl-types.h" 3 4
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
# 31 "/usr/include/sys/ioctl.h" 2 3 4






# 1 "/usr/include/sys/ttydefaults.h" 1 3 4
# 38 "/usr/include/sys/ioctl.h" 2 3 4




extern int ioctl (int __fd, unsigned long int __request, ...) __attribute__ ((__nothrow__));


# 54 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/sysctl.h" 1 3 4
# 24 "/usr/include/sys/sysctl.h" 3 4
# 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
# 25 "/usr/include/sys/sysctl.h" 2 3 4
# 44 "/usr/include/sys/sysctl.h" 3 4
# 1 "/usr/include/linux/sysctl.h" 1 3 4
# 21 "/usr/include/linux/sysctl.h" 3 4
# 1 "/usr/include/linux/kernel.h" 1 3 4
# 22 "/usr/include/linux/sysctl.h" 2 3 4
# 1 "/usr/include/linux/types.h" 1 3 4
# 23 "/usr/include/linux/sysctl.h" 2 3 4

struct file;
struct completion;






struct __sysctl_args {
 int *name;
 int nlen;
 void *oldval;
 size_t *oldlenp;
 void *newval;
 size_t newlen;
 unsigned long __unused[4];
};







enum
{
 CTL_KERN=1,
 CTL_VM=2,
 CTL_NET=3,

 CTL_FS=5,
 CTL_DEBUG=6,
 CTL_DEV=7,
 CTL_BUS=8,
 CTL_ABI=9,
 CTL_CPU=10,
 CTL_CRYPTO=11,
};


enum
{
 CTL_BUS_ISA=1
};


enum
{
 INOTIFY_MAX_USER_INSTANCES=1,
 INOTIFY_MAX_USER_WATCHES=2,
 INOTIFY_MAX_QUEUED_EVENTS=3
};


enum
{
 KERN_OSTYPE=1,
 KERN_OSRELEASE=2,
 KERN_OSREV=3,
 KERN_VERSION=4,
 KERN_SECUREMASK=5,
 KERN_PROF=6,
 KERN_NODENAME=7,
 KERN_DOMAINNAME=8,

 KERN_CAP_BSET=14,
 KERN_PANIC=15,
 KERN_EXEC_SHIELD=1000,
 KERN_PRINT_FATAL=1001,
 KERN_VDSO=1002,
 KERN_REALROOTDEV=16,

 KERN_SPARC_REBOOT=21,
 KERN_CTLALTDEL=22,
 KERN_PRINTK=23,
 KERN_NAMETRANS=24,
 KERN_PPC_HTABRECLAIM=25,
 KERN_PPC_ZEROPAGED=26,
 KERN_PPC_POWERSAVE_NAP=27,
 KERN_MODPROBE=28,
 KERN_SG_BIG_BUFF=29,
 KERN_ACCT=30,
 KERN_PPC_L2CR=31,

 KERN_RTSIGNR=32,
 KERN_RTSIGMAX=33,

 KERN_SHMMAX=34,
 KERN_MSGMAX=35,
 KERN_MSGMNB=36,
 KERN_MSGPOOL=37,
 KERN_SYSRQ=38,
 KERN_MAX_THREADS=39,
  KERN_RANDOM=40,
  KERN_SHMALL=41,
  KERN_MSGMNI=42,
  KERN_SEM=43,
  KERN_SPARC_STOP_A=44,
  KERN_SHMMNI=45,
 KERN_OVERFLOWUID=46,
 KERN_OVERFLOWGID=47,
 KERN_SHMPATH=48,
 KERN_HOTPLUG=49,
 KERN_IEEE_EMULATION_WARNINGS=50,
 KERN_S390_USER_DEBUG_LOGGING=51,
 KERN_CORE_USES_PID=52,
 KERN_TAINTED=53,
 KERN_CADPID=54,
 KERN_PIDMAX=55,
   KERN_CORE_PATTERN=56,
 KERN_PANIC_ON_OOPS=57,
 KERN_HPPA_PWRSW=58,
 KERN_HPPA_UNALIGNED=59,
 KERN_PRINTK_RATELIMIT=60,
 KERN_PRINTK_RATELIMIT_BURST=61,
 KERN_PTY=62,
 KERN_NGROUPS_MAX=63,
 KERN_SPARC_SCONS_PWROFF=64,
 KERN_HZ_TIMER=65,
 KERN_UNKNOWN_NMI_PANIC=66,
 KERN_BOOTLOADER_TYPE=67,
 KERN_RANDOMIZE=68,
 KERN_SETUID_DUMPABLE=69,
 KERN_SPIN_RETRY=70,
 KERN_ACPI_VIDEO_FLAGS=71,
 KERN_IA64_UNALIGNED=72,
 KERN_COMPAT_LOG=73,
 KERN_MAX_LOCK_DEPTH=74,
 KERN_KDUMP_ON_INIT=75,
  KERN_PANIC_ON_NMI=76,
 KERN_SCHED_INTERACTIVE=77,
 KERN_LOCK_STAT=78,
 KERN_PROVE_LOCKING=79,
 KERN_SOFTLOCKUP_THRESH=80,
 KERN_SOFTLOCKUP_PANIC=81,
 KERN_HUNG_TASK_PANIC=82,
 KERN_HUNG_TASK_CHECK_COUNT=83,
 KERN_HUNG_TASK_TIMEOUT_SECS=84,
 KERN_HUNG_TASK_WARNINGS=85,
 KERN_NMI_WATCHDOG=86,
};




enum
{
 VM_UNUSED1=1,
 VM_UNUSED2=2,
 VM_UNUSED3=3,
 VM_UNUSED4=4,
 VM_OVERCOMMIT_MEMORY=5,
 VM_UNUSED5=6,
 VM_UNUSED7=7,
 VM_UNUSED8=8,
 VM_UNUSED9=9,
 VM_PAGE_CLUSTER=10,
 VM_DIRTY_BACKGROUND=11,
 VM_DIRTY_RATIO=12,
 VM_DIRTY_WB_CS=13,
 VM_DIRTY_EXPIRE_CS=14,
 VM_NR_PDFLUSH_THREADS=15,
 VM_OVERCOMMIT_RATIO=16,
 VM_PAGEBUF=17,
 VM_HUGETLB_PAGES=18,
 VM_SWAPPINESS=19,
 VM_LOWMEM_RESERVE_RATIO=20,
 VM_MIN_FREE_KBYTES=21,
 VM_MAX_MAP_COUNT=22,
 VM_LAPTOP_MODE=23,
 VM_BLOCK_DUMP=24,
 VM_HUGETLB_GROUP=25,
 VM_VFS_CACHE_PRESSURE=26,
 VM_LEGACY_VA_LAYOUT=27,
 VM_SWAP_TOKEN_TIMEOUT=28,
 VM_DROP_PAGECACHE=29,
 VM_PERCPU_PAGELIST_FRACTION=30,
 VM_ZONE_RECLAIM_MODE=31,
 VM_MIN_UNMAPPED=32,
 VM_PANIC_ON_OOM=33,
 VM_VDSO_ENABLED=34,
 VM_MIN_SLAB=35,
 VM_PAGECACHE=37,
 VM_MMAP_MIN_ADDR=38,
 VM_FLUSH_MMAP=39,
 VM_MAX_WRITEBACK_PAGES=40,
 VM_ZONE_RECLAIM_INTERVAL=41,
 VM_TOPDOWN_ALLOCATE_FAST=42,
 VM_MAX_RECLAIMS=43,
 VM_DEVZERO_OPTIMIZED=44,
 VM_DIRTY_BYTES=45,
 VM_DIRTY_BACKGND_BYTES=46,
};



enum
{
 NET_CORE=1,
 NET_ETHER=2,
 NET_802=3,
 NET_UNIX=4,
 NET_IPV4=5,
 NET_IPX=6,
 NET_ATALK=7,
 NET_NETROM=8,
 NET_AX25=9,
 NET_BRIDGE=10,
 NET_ROSE=11,
 NET_IPV6=12,
 NET_X25=13,
 NET_TR=14,
 NET_DECNET=15,
 NET_ECONET=16,
 NET_SCTP=17,
 NET_LLC=18,
 NET_NETFILTER=19,
 NET_DCCP=20,
 NET_TUX=21,
};


enum
{
 RANDOM_POOLSIZE=1,
 RANDOM_ENTROPY_COUNT=2,
 RANDOM_READ_THRESH=3,
 RANDOM_WRITE_THRESH=4,
 RANDOM_BOOT_ID=5,
 RANDOM_UUID=6
};


enum
{
 PTY_MAX=1,
 PTY_NR=2
};


enum
{
 BUS_ISA_MEM_BASE=1,
 BUS_ISA_PORT_BASE=2,
 BUS_ISA_PORT_SHIFT=3
};



enum
{
 CRYPTO_FIPS=1,
};


enum
{
 NET_CORE_WMEM_MAX=1,
 NET_CORE_RMEM_MAX=2,
 NET_CORE_WMEM_DEFAULT=3,
 NET_CORE_RMEM_DEFAULT=4,

 NET_CORE_MAX_BACKLOG=6,
 NET_CORE_FASTROUTE=7,
 NET_CORE_MSG_COST=8,
 NET_CORE_MSG_BURST=9,
 NET_CORE_OPTMEM_MAX=10,
 NET_CORE_HOT_LIST_LENGTH=11,
 NET_CORE_DIVERT_VERSION=12,
 NET_CORE_NO_CONG_THRESH=13,
 NET_CORE_NO_CONG=14,
 NET_CORE_LO_CONG=15,
 NET_CORE_MOD_CONG=16,
 NET_CORE_DEV_WEIGHT=17,
 NET_CORE_SOMAXCONN=18,
 NET_CORE_BUDGET=19,
 NET_CORE_AEVENT_ETIME=20,
 NET_CORE_AEVENT_RSEQTH=21,
 NET_CORE_XFRM_LARVAL_DROP=22,
 NET_CORE_XFRM_ACQ_EXPIRES=23,
};







enum
{
 NET_UNIX_DESTROY_DELAY=1,
 NET_UNIX_DELETE_DELAY=2,
 NET_UNIX_MAX_DGRAM_QLEN=3,
};


enum
{
 NET_NF_CONNTRACK_MAX=1,
 NET_NF_CONNTRACK_TCP_TIMEOUT_SYN_SENT=2,
 NET_NF_CONNTRACK_TCP_TIMEOUT_SYN_RECV=3,
 NET_NF_CONNTRACK_TCP_TIMEOUT_ESTABLISHED=4,
 NET_NF_CONNTRACK_TCP_TIMEOUT_FIN_WAIT=5,
 NET_NF_CONNTRACK_TCP_TIMEOUT_CLOSE_WAIT=6,
 NET_NF_CONNTRACK_TCP_TIMEOUT_LAST_ACK=7,
 NET_NF_CONNTRACK_TCP_TIMEOUT_TIME_WAIT=8,
 NET_NF_CONNTRACK_TCP_TIMEOUT_CLOSE=9,
 NET_NF_CONNTRACK_UDP_TIMEOUT=10,
 NET_NF_CONNTRACK_UDP_TIMEOUT_STREAM=11,
 NET_NF_CONNTRACK_ICMP_TIMEOUT=12,
 NET_NF_CONNTRACK_GENERIC_TIMEOUT=13,
 NET_NF_CONNTRACK_BUCKETS=14,
 NET_NF_CONNTRACK_LOG_INVALID=15,
 NET_NF_CONNTRACK_TCP_TIMEOUT_MAX_RETRANS=16,
 NET_NF_CONNTRACK_TCP_LOOSE=17,
 NET_NF_CONNTRACK_TCP_BE_LIBERAL=18,
 NET_NF_CONNTRACK_TCP_MAX_RETRANS=19,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_CLOSED=20,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_WAIT=21,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_ECHOED=22,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_ESTABLISHED=23,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_SENT=24,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_RECD=25,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_ACK_SENT=26,
 NET_NF_CONNTRACK_COUNT=27,
 NET_NF_CONNTRACK_ICMPV6_TIMEOUT=28,
 NET_NF_CONNTRACK_FRAG6_TIMEOUT=29,
 NET_NF_CONNTRACK_FRAG6_LOW_THRESH=30,
 NET_NF_CONNTRACK_FRAG6_HIGH_THRESH=31,
 NET_NF_CONNTRACK_CHECKSUM=32,
};


enum
{

 NET_IPV4_FORWARD=8,
 NET_IPV4_DYNADDR=9,

 NET_IPV4_CONF=16,
 NET_IPV4_NEIGH=17,
 NET_IPV4_ROUTE=18,
 NET_IPV4_FIB_HASH=19,
 NET_IPV4_NETFILTER=20,

 NET_IPV4_TCP_TIMESTAMPS=33,
 NET_IPV4_TCP_WINDOW_SCALING=34,
 NET_IPV4_TCP_SACK=35,
 NET_IPV4_TCP_RETRANS_COLLAPSE=36,
 NET_IPV4_DEFAULT_TTL=37,
 NET_IPV4_AUTOCONFIG=38,
 NET_IPV4_NO_PMTU_DISC=39,
 NET_IPV4_TCP_SYN_RETRIES=40,
 NET_IPV4_IPFRAG_HIGH_THRESH=41,
 NET_IPV4_IPFRAG_LOW_THRESH=42,
 NET_IPV4_IPFRAG_TIME=43,
 NET_IPV4_TCP_MAX_KA_PROBES=44,
 NET_IPV4_TCP_KEEPALIVE_TIME=45,
 NET_IPV4_TCP_KEEPALIVE_PROBES=46,
 NET_IPV4_TCP_RETRIES1=47,
 NET_IPV4_TCP_RETRIES2=48,
 NET_IPV4_TCP_FIN_TIMEOUT=49,
 NET_IPV4_IP_MASQ_DEBUG=50,
 NET_TCP_SYNCOOKIES=51,
 NET_TCP_STDURG=52,
 NET_TCP_RFC1337=53,
 NET_TCP_SYN_TAILDROP=54,
 NET_TCP_MAX_SYN_BACKLOG=55,
 NET_IPV4_LOCAL_PORT_RANGE=56,
 NET_IPV4_ICMP_ECHO_IGNORE_ALL=57,
 NET_IPV4_ICMP_ECHO_IGNORE_BROADCASTS=58,
 NET_IPV4_ICMP_SOURCEQUENCH_RATE=59,
 NET_IPV4_ICMP_DESTUNREACH_RATE=60,
 NET_IPV4_ICMP_TIMEEXCEED_RATE=61,
 NET_IPV4_ICMP_PARAMPROB_RATE=62,
 NET_IPV4_ICMP_ECHOREPLY_RATE=63,
 NET_IPV4_ICMP_IGNORE_BOGUS_ERROR_RESPONSES=64,
 NET_IPV4_IGMP_MAX_MEMBERSHIPS=65,
 NET_TCP_TW_RECYCLE=66,
 NET_IPV4_ALWAYS_DEFRAG=67,
 NET_IPV4_TCP_KEEPALIVE_INTVL=68,
 NET_IPV4_INET_PEER_THRESHOLD=69,
 NET_IPV4_INET_PEER_MINTTL=70,
 NET_IPV4_INET_PEER_MAXTTL=71,
 NET_IPV4_INET_PEER_GC_MINTIME=72,
 NET_IPV4_INET_PEER_GC_MAXTIME=73,
 NET_TCP_ORPHAN_RETRIES=74,
 NET_TCP_ABORT_ON_OVERFLOW=75,
 NET_TCP_SYNACK_RETRIES=76,
 NET_TCP_MAX_ORPHANS=77,
 NET_TCP_MAX_TW_BUCKETS=78,
 NET_TCP_FACK=79,
 NET_TCP_REORDERING=80,
 NET_TCP_ECN=81,
 NET_TCP_DSACK=82,
 NET_TCP_MEM=83,
 NET_TCP_WMEM=84,
 NET_TCP_RMEM=85,
 NET_TCP_APP_WIN=86,
 NET_TCP_ADV_WIN_SCALE=87,
 NET_IPV4_NONLOCAL_BIND=88,
 NET_IPV4_ICMP_RATELIMIT=89,
 NET_IPV4_ICMP_RATEMASK=90,
 NET_TCP_TW_REUSE=91,
 NET_TCP_FRTO=92,
 NET_TCP_LOW_LATENCY=93,
 NET_IPV4_IPFRAG_SECRET_INTERVAL=94,
 NET_IPV4_IGMP_MAX_MSF=96,
 NET_TCP_NO_METRICS_SAVE=97,
 NET_TCP_DEFAULT_WIN_SCALE=105,
 NET_TCP_MODERATE_RCVBUF=106,
 NET_TCP_TSO_WIN_DIVISOR=107,
 NET_TCP_BIC_BETA=108,
 NET_IPV4_ICMP_ERRORS_USE_INBOUND_IFADDR=109,
 NET_TCP_CONG_CONTROL=110,
 NET_TCP_ABC=111,
 NET_IPV4_IPFRAG_MAX_DIST=112,
  NET_TCP_MTU_PROBING=113,
 NET_TCP_BASE_MSS=114,
 NET_IPV4_TCP_WORKAROUND_SIGNED_WINDOWS=115,
 NET_TCP_DMA_COPYBREAK=116,
 NET_TCP_SLOW_START_AFTER_IDLE=117,
 NET_CIPSOV4_CACHE_ENABLE=118,
 NET_CIPSOV4_CACHE_BUCKET_SIZE=119,
 NET_CIPSOV4_RBM_OPTFMT=120,
 NET_CIPSOV4_RBM_STRICTVALID=121,
 NET_UDP_MEM=122,
 NET_UDP_RMEM_MIN=123,
 NET_UDP_WMEM_MIN=124,
 NET_IPV4_LOCAL_RESERVED_PORTS=125,
};

enum {
 NET_IPV4_ROUTE_FLUSH=1,
 NET_IPV4_ROUTE_MIN_DELAY=2,
 NET_IPV4_ROUTE_MAX_DELAY=3,
 NET_IPV4_ROUTE_GC_THRESH=4,
 NET_IPV4_ROUTE_MAX_SIZE=5,
 NET_IPV4_ROUTE_GC_MIN_INTERVAL=6,
 NET_IPV4_ROUTE_GC_TIMEOUT=7,
 NET_IPV4_ROUTE_GC_INTERVAL=8,
 NET_IPV4_ROUTE_REDIRECT_LOAD=9,
 NET_IPV4_ROUTE_REDIRECT_NUMBER=10,
 NET_IPV4_ROUTE_REDIRECT_SILENCE=11,
 NET_IPV4_ROUTE_ERROR_COST=12,
 NET_IPV4_ROUTE_ERROR_BURST=13,
 NET_IPV4_ROUTE_GC_ELASTICITY=14,
 NET_IPV4_ROUTE_MTU_EXPIRES=15,
 NET_IPV4_ROUTE_MIN_PMTU=16,
 NET_IPV4_ROUTE_MIN_ADVMSS=17,
 NET_IPV4_ROUTE_SECRET_INTERVAL=18,
 NET_IPV4_ROUTE_GC_MIN_INTERVAL_MS=19,
};

enum
{
 NET_PROTO_CONF_ALL=-2,
 NET_PROTO_CONF_DEFAULT=-3


};

enum
{
 NET_IPV4_CONF_FORWARDING=1,
 NET_IPV4_CONF_MC_FORWARDING=2,
 NET_IPV4_CONF_PROXY_ARP=3,
 NET_IPV4_CONF_ACCEPT_REDIRECTS=4,
 NET_IPV4_CONF_SECURE_REDIRECTS=5,
 NET_IPV4_CONF_SEND_REDIRECTS=6,
 NET_IPV4_CONF_SHARED_MEDIA=7,
 NET_IPV4_CONF_RP_FILTER=8,
 NET_IPV4_CONF_ACCEPT_SOURCE_ROUTE=9,
 NET_IPV4_CONF_BOOTP_RELAY=10,
 NET_IPV4_CONF_LOG_MARTIANS=11,
 NET_IPV4_CONF_TAG=12,
 NET_IPV4_CONF_ARPFILTER=13,
 NET_IPV4_CONF_MEDIUM_ID=14,
 NET_IPV4_CONF_NOXFRM=15,
 NET_IPV4_CONF_NOPOLICY=16,
 NET_IPV4_CONF_FORCE_IGMP_VERSION=17,
 NET_IPV4_CONF_ARP_ANNOUNCE=18,
 NET_IPV4_CONF_ARP_IGNORE=19,
 NET_IPV4_CONF_PROMOTE_SECONDARIES=20,
 NET_IPV4_CONF_ARP_ACCEPT=21,
 NET_IPV4_CONF_ACCEPT_LOCAL=22,
 __NET_IPV4_CONF_MAX
};


enum
{
 NET_IPV4_NF_CONNTRACK_MAX=1,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_SYN_SENT=2,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_SYN_RECV=3,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_ESTABLISHED=4,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_FIN_WAIT=5,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_CLOSE_WAIT=6,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_LAST_ACK=7,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_TIME_WAIT=8,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_CLOSE=9,
 NET_IPV4_NF_CONNTRACK_UDP_TIMEOUT=10,
 NET_IPV4_NF_CONNTRACK_UDP_TIMEOUT_STREAM=11,
 NET_IPV4_NF_CONNTRACK_ICMP_TIMEOUT=12,
 NET_IPV4_NF_CONNTRACK_GENERIC_TIMEOUT=13,
 NET_IPV4_NF_CONNTRACK_BUCKETS=14,
 NET_IPV4_NF_CONNTRACK_LOG_INVALID=15,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_MAX_RETRANS=16,
 NET_IPV4_NF_CONNTRACK_TCP_LOOSE=17,
 NET_IPV4_NF_CONNTRACK_TCP_BE_LIBERAL=18,
 NET_IPV4_NF_CONNTRACK_TCP_MAX_RETRANS=19,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_CLOSED=20,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_WAIT=21,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_ECHOED=22,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_ESTABLISHED=23,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_SENT=24,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_RECD=25,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_ACK_SENT=26,
 NET_IPV4_NF_CONNTRACK_COUNT=27,
 NET_IPV4_NF_CONNTRACK_CHECKSUM=28,
};


enum {
 NET_IPV6_CONF=16,
 NET_IPV6_NEIGH=17,
 NET_IPV6_ROUTE=18,
 NET_IPV6_ICMP=19,
 NET_IPV6_BINDV6ONLY=20,
 NET_IPV6_IP6FRAG_HIGH_THRESH=21,
 NET_IPV6_IP6FRAG_LOW_THRESH=22,
 NET_IPV6_IP6FRAG_TIME=23,
 NET_IPV6_IP6FRAG_SECRET_INTERVAL=24,
 NET_IPV6_MLD_MAX_MSF=25,
 NET_IPV6_OPTIMISTIC_DAD=26,
};

enum {
 NET_IPV6_ROUTE_FLUSH=1,
 NET_IPV6_ROUTE_GC_THRESH=2,
 NET_IPV6_ROUTE_MAX_SIZE=3,
 NET_IPV6_ROUTE_GC_MIN_INTERVAL=4,
 NET_IPV6_ROUTE_GC_TIMEOUT=5,
 NET_IPV6_ROUTE_GC_INTERVAL=6,
 NET_IPV6_ROUTE_GC_ELASTICITY=7,
 NET_IPV6_ROUTE_MTU_EXPIRES=8,
 NET_IPV6_ROUTE_MIN_ADVMSS=9,
 NET_IPV6_ROUTE_GC_MIN_INTERVAL_MS=10
};

enum {
 NET_IPV6_FORWARDING=1,
 NET_IPV6_HOP_LIMIT=2,
 NET_IPV6_MTU=3,
 NET_IPV6_ACCEPT_RA=4,
 NET_IPV6_ACCEPT_REDIRECTS=5,
 NET_IPV6_AUTOCONF=6,
 NET_IPV6_DAD_TRANSMITS=7,
 NET_IPV6_RTR_SOLICITS=8,
 NET_IPV6_RTR_SOLICIT_INTERVAL=9,
 NET_IPV6_RTR_SOLICIT_DELAY=10,
 NET_IPV6_USE_TEMPADDR=11,
 NET_IPV6_TEMP_VALID_LFT=12,
 NET_IPV6_TEMP_PREFERED_LFT=13,
 NET_IPV6_REGEN_MAX_RETRY=14,
 NET_IPV6_MAX_DESYNC_FACTOR=15,
 NET_IPV6_MAX_ADDRESSES=16,
 NET_IPV6_FORCE_MLD_VERSION=17,
 NET_IPV6_ACCEPT_RA_DEFRTR=18,
 NET_IPV6_ACCEPT_RA_PINFO=19,
 NET_IPV6_ACCEPT_RA_RTR_PREF=20,
 NET_IPV6_RTR_PROBE_INTERVAL=21,
 NET_IPV6_ACCEPT_RA_RT_INFO_MAX_PLEN=22,

 NET_IPV6_DISABLE=23,
 NET_IPV6_ACCEPT_DAD=24,
 __NET_IPV6_MAX
};


enum {
 NET_IPV6_ICMP_RATELIMIT=1
};


enum {
 NET_NEIGH_MCAST_SOLICIT=1,
 NET_NEIGH_UCAST_SOLICIT=2,
 NET_NEIGH_APP_SOLICIT=3,
 NET_NEIGH_RETRANS_TIME=4,
 NET_NEIGH_REACHABLE_TIME=5,
 NET_NEIGH_DELAY_PROBE_TIME=6,
 NET_NEIGH_GC_STALE_TIME=7,
 NET_NEIGH_UNRES_QLEN=8,
 NET_NEIGH_PROXY_QLEN=9,
 NET_NEIGH_ANYCAST_DELAY=10,
 NET_NEIGH_PROXY_DELAY=11,
 NET_NEIGH_LOCKTIME=12,
 NET_NEIGH_GC_INTERVAL=13,
 NET_NEIGH_GC_THRESH1=14,
 NET_NEIGH_GC_THRESH2=15,
 NET_NEIGH_GC_THRESH3=16,
 NET_NEIGH_RETRANS_TIME_MS=17,
 NET_NEIGH_REACHABLE_TIME_MS=18,
 __NET_NEIGH_MAX
};


enum {
 NET_DCCP_DEFAULT=1,
};


enum {
 NET_DCCP_DEFAULT_SEQ_WINDOW = 1,
 NET_DCCP_DEFAULT_RX_CCID = 2,
 NET_DCCP_DEFAULT_TX_CCID = 3,
 NET_DCCP_DEFAULT_ACK_RATIO = 4,
 NET_DCCP_DEFAULT_SEND_ACKVEC = 5,
 NET_DCCP_DEFAULT_SEND_NDP = 6,
};


enum {
 NET_IPX_PPROP_BROADCASTING=1,
 NET_IPX_FORWARDING=2
};


enum {
 NET_LLC2=1,
 NET_LLC_STATION=2,
};


enum {
 NET_LLC2_TIMEOUT=1,
};


enum {
 NET_LLC_STATION_ACK_TIMEOUT=1,
};


enum {
 NET_LLC2_ACK_TIMEOUT=1,
 NET_LLC2_P_TIMEOUT=2,
 NET_LLC2_REJ_TIMEOUT=3,
 NET_LLC2_BUSY_TIMEOUT=4,
};


enum {
 NET_ATALK_AARP_EXPIRY_TIME=1,
 NET_ATALK_AARP_TICK_TIME=2,
 NET_ATALK_AARP_RETRANSMIT_LIMIT=3,
 NET_ATALK_AARP_RESOLVE_TIME=4
};



enum {
 NET_NETROM_DEFAULT_PATH_QUALITY=1,
 NET_NETROM_OBSOLESCENCE_COUNT_INITIALISER=2,
 NET_NETROM_NETWORK_TTL_INITIALISER=3,
 NET_NETROM_TRANSPORT_TIMEOUT=4,
 NET_NETROM_TRANSPORT_MAXIMUM_TRIES=5,
 NET_NETROM_TRANSPORT_ACKNOWLEDGE_DELAY=6,
 NET_NETROM_TRANSPORT_BUSY_DELAY=7,
 NET_NETROM_TRANSPORT_REQUESTED_WINDOW_SIZE=8,
 NET_NETROM_TRANSPORT_NO_ACTIVITY_TIMEOUT=9,
 NET_NETROM_ROUTING_CONTROL=10,
 NET_NETROM_LINK_FAILS_COUNT=11,
 NET_NETROM_RESET=12
};


enum {
 NET_AX25_IP_DEFAULT_MODE=1,
 NET_AX25_DEFAULT_MODE=2,
 NET_AX25_BACKOFF_TYPE=3,
 NET_AX25_CONNECT_MODE=4,
 NET_AX25_STANDARD_WINDOW=5,
 NET_AX25_EXTENDED_WINDOW=6,
 NET_AX25_T1_TIMEOUT=7,
 NET_AX25_T2_TIMEOUT=8,
 NET_AX25_T3_TIMEOUT=9,
 NET_AX25_IDLE_TIMEOUT=10,
 NET_AX25_N2=11,
 NET_AX25_PACLEN=12,
 NET_AX25_PROTOCOL=13,
 NET_AX25_DAMA_SLAVE_TIMEOUT=14
};


enum {
 NET_ROSE_RESTART_REQUEST_TIMEOUT=1,
 NET_ROSE_CALL_REQUEST_TIMEOUT=2,
 NET_ROSE_RESET_REQUEST_TIMEOUT=3,
 NET_ROSE_CLEAR_REQUEST_TIMEOUT=4,
 NET_ROSE_ACK_HOLD_BACK_TIMEOUT=5,
 NET_ROSE_ROUTING_CONTROL=6,
 NET_ROSE_LINK_FAIL_TIMEOUT=7,
 NET_ROSE_MAX_VCS=8,
 NET_ROSE_WINDOW_SIZE=9,
 NET_ROSE_NO_ACTIVITY_TIMEOUT=10
};


enum {
 NET_X25_RESTART_REQUEST_TIMEOUT=1,
 NET_X25_CALL_REQUEST_TIMEOUT=2,
 NET_X25_RESET_REQUEST_TIMEOUT=3,
 NET_X25_CLEAR_REQUEST_TIMEOUT=4,
 NET_X25_ACK_HOLD_BACK_TIMEOUT=5
};


enum
{
 NET_TR_RIF_TIMEOUT=1
};


enum {
 NET_DECNET_NODE_TYPE = 1,
 NET_DECNET_NODE_ADDRESS = 2,
 NET_DECNET_NODE_NAME = 3,
 NET_DECNET_DEFAULT_DEVICE = 4,
 NET_DECNET_TIME_WAIT = 5,
 NET_DECNET_DN_COUNT = 6,
 NET_DECNET_DI_COUNT = 7,
 NET_DECNET_DR_COUNT = 8,
 NET_DECNET_DST_GC_INTERVAL = 9,
 NET_DECNET_CONF = 10,
 NET_DECNET_NO_FC_MAX_CWND = 11,
 NET_DECNET_MEM = 12,
 NET_DECNET_RMEM = 13,
 NET_DECNET_WMEM = 14,
 NET_DECNET_DEBUG_LEVEL = 255
};


enum {
 NET_DECNET_CONF_LOOPBACK = -2,
 NET_DECNET_CONF_DDCMP = -3,
 NET_DECNET_CONF_PPP = -4,
 NET_DECNET_CONF_X25 = -5,
 NET_DECNET_CONF_GRE = -6,
 NET_DECNET_CONF_ETHER = -7


};


enum {
 NET_DECNET_CONF_DEV_PRIORITY = 1,
 NET_DECNET_CONF_DEV_T1 = 2,
 NET_DECNET_CONF_DEV_T2 = 3,
 NET_DECNET_CONF_DEV_T3 = 4,
 NET_DECNET_CONF_DEV_FORWARDING = 5,
 NET_DECNET_CONF_DEV_BLKSIZE = 6,
 NET_DECNET_CONF_DEV_STATE = 7
};


enum {
 NET_SCTP_RTO_INITIAL = 1,
 NET_SCTP_RTO_MIN = 2,
 NET_SCTP_RTO_MAX = 3,
 NET_SCTP_RTO_ALPHA = 4,
 NET_SCTP_RTO_BETA = 5,
 NET_SCTP_VALID_COOKIE_LIFE = 6,
 NET_SCTP_ASSOCIATION_MAX_RETRANS = 7,
 NET_SCTP_PATH_MAX_RETRANS = 8,
 NET_SCTP_MAX_INIT_RETRANSMITS = 9,
 NET_SCTP_HB_INTERVAL = 10,
 NET_SCTP_PRESERVE_ENABLE = 11,
 NET_SCTP_MAX_BURST = 12,
 NET_SCTP_ADDIP_ENABLE = 13,
 NET_SCTP_PRSCTP_ENABLE = 14,
 NET_SCTP_SNDBUF_POLICY = 15,
 NET_SCTP_SACK_TIMEOUT = 16,
 NET_SCTP_RCVBUF_POLICY = 17,
 NET_SCTP_BUF_MEM = 18,
 NET_SCTP_BUF_RMEM = 19,
 NET_SCTP_BUF_WMEM = 20,
};


enum {
 NET_BRIDGE_NF_CALL_ARPTABLES = 1,
 NET_BRIDGE_NF_CALL_IPTABLES = 2,
 NET_BRIDGE_NF_CALL_IP6TABLES = 3,
 NET_BRIDGE_NF_FILTER_VLAN_TAGGED = 4,
};


enum {
 NET_TUX_DOCROOT = 1,
 NET_TUX_LOGFILE = 2,
 NET_TUX_EXTCGI = 3,
 NET_TUX_STOP = 4,
 NET_TUX_CLIENTPORT = 5,
 NET_TUX_LOGGING = 6,
 NET_TUX_SERVERPORT = 7,
 NET_TUX_THREADS = 8,
 NET_TUX_KEEPALIVE_TIMEOUT = 9,
 NET_TUX_MAX_KEEPALIVE_BW = 10,
 NET_TUX_DEFER_ACCEPT = 11,
 NET_TUX_MAX_FREE_REQUESTS = 12,
 NET_TUX_MAX_CONNECT = 13,
 NET_TUX_MAX_BACKLOG = 14,
 NET_TUX_MODE_FORBIDDEN = 15,
 NET_TUX_MODE_ALLOWED = 16,
 NET_TUX_MODE_USERSPACE = 17,
 NET_TUX_MODE_CGI = 18,
 NET_TUX_CGI_UID = 19,
 NET_TUX_CGI_GID = 20,
 NET_TUX_CGIROOT = 21,
 NET_TUX_LOGENTRY_ALIGN_ORDER = 22,
 NET_TUX_NONAGLE = 23,
 NET_TUX_ACK_PINGPONG = 24,
 NET_TUX_PUSH_ALL = 25,
 NET_TUX_ZEROCOPY_PARSE = 26,
 NET_CONFIG_TUX_DEBUG_BLOCKING = 27,
 NET_TUX_PAGE_AGE_START = 28,
 NET_TUX_PAGE_AGE_ADV = 29,
 NET_TUX_PAGE_AGE_MAX = 30,
 NET_TUX_VIRTUAL_SERVER = 31,
 NET_TUX_MAX_OBJECT_SIZE = 32,
 NET_TUX_COMPRESSION = 33,
 NET_TUX_NOID = 34,
 NET_TUX_CGI_INHERIT_CPU = 35,
 NET_TUX_CGI_CPU_MASK = 36,
 NET_TUX_ZEROCOPY_HEADER = 37,
 NET_TUX_ZEROCOPY_SENDFILE = 38,
 NET_TUX_ALL_USERSPACE = 39,
 NET_TUX_REDIRECT_LOGGING = 40,
 NET_TUX_REFERER_LOGGING = 41,
 NET_TUX_MAX_HEADER_LEN = 42,
 NET_TUX_404_PAGE = 43,
 NET_TUX_MAX_KEEPALIVES = 44,
 NET_TUX_IGNORE_QUERY = 45,
};


enum
{
 FS_NRINODE=1,
 FS_STATINODE=2,
 FS_MAXINODE=3,
 FS_NRDQUOT=4,
 FS_MAXDQUOT=5,
 FS_NRFILE=6,
 FS_MAXFILE=7,
 FS_DENTRY=8,
 FS_NRSUPER=9,
 FS_MAXSUPER=10,
 FS_OVERFLOWUID=11,
 FS_OVERFLOWGID=12,
 FS_LEASES=13,
 FS_DIR_NOTIFY=14,
 FS_LEASE_TIME=15,
 FS_DQSTATS=16,
 FS_XFS=17,
 FS_AIO_NR=18,
 FS_AIO_MAX_NR=19,
 FS_INOTIFY=20,
};


enum {
 FS_DQ_LOOKUPS = 1,
 FS_DQ_DROPS = 2,
 FS_DQ_READS = 3,
 FS_DQ_WRITES = 4,
 FS_DQ_CACHE_HITS = 5,
 FS_DQ_ALLOCATED = 6,
 FS_DQ_FREE = 7,
 FS_DQ_SYNCS = 8,
 FS_DQ_WARNINGS = 9,
};


enum {
 DEBUG_KPROBES_OPTIMIZE = 1,
};


enum {
 DEV_CDROM=1,
 DEV_HWMON=2,
 DEV_PARPORT=3,
 DEV_RAID=4,
 DEV_MAC_HID=5,
 DEV_SCSI=6,
 DEV_IPMI=7,
};


enum {
 DEV_CDROM_INFO=1,
 DEV_CDROM_AUTOCLOSE=2,
 DEV_CDROM_AUTOEJECT=3,
 DEV_CDROM_DEBUG=4,
 DEV_CDROM_LOCK=5,
 DEV_CDROM_CHECK_MEDIA=6
};


enum {
 DEV_PARPORT_DEFAULT=-3
};


enum {
 DEV_RAID_SPEED_LIMIT_MIN=1,
 DEV_RAID_SPEED_LIMIT_MAX=2
};


enum {
 DEV_PARPORT_DEFAULT_TIMESLICE=1,
 DEV_PARPORT_DEFAULT_SPINTIME=2
};


enum {
 DEV_PARPORT_SPINTIME=1,
 DEV_PARPORT_BASE_ADDR=2,
 DEV_PARPORT_IRQ=3,
 DEV_PARPORT_DMA=4,
 DEV_PARPORT_MODES=5,
 DEV_PARPORT_DEVICES=6,
 DEV_PARPORT_AUTOPROBE=16
};


enum {
 DEV_PARPORT_DEVICES_ACTIVE=-3,
};


enum {
 DEV_PARPORT_DEVICE_TIMESLICE=1,
};


enum {
 DEV_MAC_HID_KEYBOARD_SENDS_LINUX_KEYCODES=1,
 DEV_MAC_HID_KEYBOARD_LOCK_KEYCODES=2,
 DEV_MAC_HID_MOUSE_BUTTON_EMULATION=3,
 DEV_MAC_HID_MOUSE_BUTTON2_KEYCODE=4,
 DEV_MAC_HID_MOUSE_BUTTON3_KEYCODE=5,
 DEV_MAC_HID_ADB_MOUSE_SENDS_KEYCODES=6
};


enum {
 DEV_SCSI_LOGGING_LEVEL=1,
};


enum {
 DEV_IPMI_POWEROFF_POWERCYCLE=1,
};


enum
{
 ABI_DEFHANDLER_COFF=1,
 ABI_DEFHANDLER_ELF=2,
 ABI_DEFHANDLER_LCALL7=3,
 ABI_DEFHANDLER_LIBCSO=4,
 ABI_TRACE=5,
 ABI_FAKE_UTSNAME=6,
};
# 45 "/usr/include/sys/sysctl.h" 2 3 4
# 64 "/usr/include/sys/sysctl.h" 3 4



extern int sysctl (int *__name, int __nlen, void *__oldval,
     size_t *__oldlenp, void *__newval, size_t __newlen) __attribute__ ((__nothrow__));


# 55 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/crypt.h" 1 3 4
# 30 "/usr/include/crypt.h" 3 4



extern char *crypt (__const char *__key, __const char *__salt)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void setkey (__const char *__key) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void encrypt (char *__block, int __edflag) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));




struct crypt_data
  {
    char keysched[16 * 8];
    char sb0[32768];
    char sb1[32768];
    char sb2[32768];
    char sb3[32768];

    char crypt_3_buf[14];
    char current_salt[2];
    long int current_saltbits;
    int direction, initialized;
  };

extern char *crypt_r (__const char *__key, __const char *__salt,
        struct crypt_data * __restrict __data)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern void setkey_r (__const char *__key,
        struct crypt_data * __restrict __data)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern void encrypt_r (char *__block, int __edflag,
         struct crypt_data * __restrict __data)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 3)));



# 56 "src/os/unix/ngx_linux_config.h" 2
# 1 "/usr/include/sys/utsname.h" 1 3 4
# 28 "/usr/include/sys/utsname.h" 3 4


# 1 "/usr/include/bits/utsname.h" 1 3 4
# 31 "/usr/include/sys/utsname.h" 2 3 4
# 49 "/usr/include/sys/utsname.h" 3 4
struct utsname
  {

    char sysname[65];


    char nodename[65];


    char release[65];

    char version[65];


    char machine[65];




    char domainname[65];




  };
# 82 "/usr/include/sys/utsname.h" 3 4
extern int uname (struct utsname *__name) __attribute__ ((__nothrow__));



# 57 "src/os/unix/ngx_linux_config.h" 2


# 1 "objs/ngx_auto_config.h" 1
# 60 "src/os/unix/ngx_linux_config.h" 2



# 1 "/usr/include/semaphore.h" 1 3 4
# 30 "/usr/include/semaphore.h" 3 4
# 1 "/usr/include/bits/semaphore.h" 1 3 4
# 24 "/usr/include/bits/semaphore.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 25 "/usr/include/bits/semaphore.h" 2 3 4
# 40 "/usr/include/bits/semaphore.h" 3 4
typedef union
{
  char __size[32];
  long int __align;
} sem_t;
# 31 "/usr/include/semaphore.h" 2 3 4






extern int sem_init (sem_t *__sem, int __pshared, unsigned int __value)
     __attribute__ ((__nothrow__));

extern int sem_destroy (sem_t *__sem) __attribute__ ((__nothrow__));


extern sem_t *sem_open (__const char *__name, int __oflag, ...) __attribute__ ((__nothrow__));


extern int sem_close (sem_t *__sem) __attribute__ ((__nothrow__));


extern int sem_unlink (__const char *__name) __attribute__ ((__nothrow__));





extern int sem_wait (sem_t *__sem);






extern int sem_timedwait (sem_t *__restrict __sem,
     __const struct timespec *__restrict __abstime);



extern int sem_trywait (sem_t *__sem) __attribute__ ((__nothrow__));


extern int sem_post (sem_t *__sem) __attribute__ ((__nothrow__));


extern int sem_getvalue (sem_t *__restrict __sem, int *__restrict __sval)
     __attribute__ ((__nothrow__));



# 64 "src/os/unix/ngx_linux_config.h" 2




# 1 "/usr/include/sys/prctl.h" 1 3 4
# 23 "/usr/include/sys/prctl.h" 3 4
# 1 "/usr/include/linux/prctl.h" 1 3 4
# 24 "/usr/include/sys/prctl.h" 2 3 4




extern int prctl (int __option, ...) __attribute__ ((__nothrow__));


# 69 "src/os/unix/ngx_linux_config.h" 2




# 1 "/usr/include/sys/sendfile.h" 1 3 4
# 26 "/usr/include/sys/sendfile.h" 3 4

# 38 "/usr/include/sys/sendfile.h" 3 4
extern ssize_t sendfile (int __out_fd, int __in_fd, __off64_t *__offset, size_t __count) __asm__ ("" "sendfile64") __attribute__ ((__nothrow__));







extern ssize_t sendfile64 (int __out_fd, int __in_fd, __off64_t *__offset,
      size_t __count) __attribute__ ((__nothrow__));



# 74 "src/os/unix/ngx_linux_config.h" 2
# 86 "src/os/unix/ngx_linux_config.h"
# 1 "/usr/include/sys/epoll.h" 1 3 4
# 26 "/usr/include/sys/epoll.h" 3 4
enum EPOLL_EVENTS
  {
    EPOLLIN = 0x001,

    EPOLLPRI = 0x002,

    EPOLLOUT = 0x004,

    EPOLLRDNORM = 0x040,

    EPOLLRDBAND = 0x080,

    EPOLLWRNORM = 0x100,

    EPOLLWRBAND = 0x200,

    EPOLLMSG = 0x400,

    EPOLLERR = 0x008,

    EPOLLHUP = 0x010,

    EPOLLONESHOT = (1 << 30),

    EPOLLET = (1 << 31)

  };
# 61 "/usr/include/sys/epoll.h" 3 4
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  uint32_t events;
  epoll_data_t data;
} __attribute__ ((__packed__));








extern int epoll_create (int __size) __attribute__ ((__nothrow__));
# 91 "/usr/include/sys/epoll.h" 3 4
extern int epoll_ctl (int __epfd, int __op, int __fd,
        struct epoll_event *__event) __attribute__ ((__nothrow__));
# 105 "/usr/include/sys/epoll.h" 3 4
extern int epoll_wait (int __epfd, struct epoll_event *__events,
         int __maxevents, int __timeout);


# 87 "src/os/unix/ngx_linux_config.h" 2
# 115 "src/os/unix/ngx_linux_config.h"
extern char **environ;
# 27 "src/core/ngx_config.h" 2
# 78 "src/core/ngx_config.h"
typedef intptr_t ngx_int_t;
typedef uintptr_t ngx_uint_t;
typedef intptr_t ngx_flag_t;
# 9 "src/http/ngx_http_upstream.c" 2
# 1 "src/core/ngx_core.h" 1
# 12 "src/core/ngx_core.h"
typedef struct ngx_module_s ngx_module_t;
typedef struct ngx_conf_s ngx_conf_t;
typedef struct ngx_cycle_s ngx_cycle_t;
typedef struct ngx_pool_s ngx_pool_t;
typedef struct ngx_chain_s ngx_chain_t;
typedef struct ngx_log_s ngx_log_t;
typedef struct ngx_array_s ngx_array_t;
typedef struct ngx_open_file_s ngx_open_file_t;
typedef struct ngx_command_s ngx_command_t;
typedef struct ngx_file_s ngx_file_t;
typedef struct ngx_event_s ngx_event_t;
typedef struct ngx_event_aio_s ngx_event_aio_t;
typedef struct ngx_connection_s ngx_connection_t;

typedef void (*ngx_event_handler_pt)(ngx_event_t *ev);
typedef void (*ngx_connection_handler_pt)(ngx_connection_t *c);
# 39 "src/core/ngx_core.h"
# 1 "src/os/unix/ngx_errno.h" 1
# 13 "src/os/unix/ngx_errno.h"
# 1 "src/core/ngx_core.h" 1
# 14 "src/os/unix/ngx_errno.h" 2


typedef int ngx_err_t;
# 71 "src/os/unix/ngx_errno.h"
u_char *ngx_strerror(ngx_err_t err, u_char *errstr, size_t size);
ngx_int_t ngx_strerror_init(void);
# 40 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_atomic.h" 1
# 97 "src/os/unix/ngx_atomic.h"
typedef long ngx_atomic_int_t;
typedef unsigned long ngx_atomic_uint_t;







typedef volatile ngx_atomic_uint_t ngx_atomic_t;
# 307 "src/os/unix/ngx_atomic.h"
void ngx_spinlock(ngx_atomic_t *lock, ngx_atomic_int_t value, ngx_uint_t spin);
# 41 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_thread.h" 1
# 42 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_rbtree.h" 1
# 16 "src/core/ngx_rbtree.h"
typedef ngx_uint_t ngx_rbtree_key_t;
typedef ngx_int_t ngx_rbtree_key_int_t;


typedef struct ngx_rbtree_node_s ngx_rbtree_node_t;

struct ngx_rbtree_node_s {
    ngx_rbtree_key_t key;
    ngx_rbtree_node_t *left;
    ngx_rbtree_node_t *right;
    ngx_rbtree_node_t *parent;
    u_char color;
    u_char data;
};


typedef struct ngx_rbtree_s ngx_rbtree_t;

typedef void (*ngx_rbtree_insert_pt) (ngx_rbtree_node_t *root,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);

struct ngx_rbtree_s {
    ngx_rbtree_node_t *root;
    ngx_rbtree_node_t *sentinel;
    ngx_rbtree_insert_pt insert;
};
# 51 "src/core/ngx_rbtree.h"
void ngx_rbtree_insert( ngx_rbtree_t *tree,
    ngx_rbtree_node_t *node);
void ngx_rbtree_delete( ngx_rbtree_t *tree,
    ngx_rbtree_node_t *node);
void ngx_rbtree_insert_value(ngx_rbtree_node_t *root, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel);
void ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *root,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
# 73 "src/core/ngx_rbtree.h"
static inline ngx_rbtree_node_t *
ngx_rbtree_min(ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel)
{
    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}
# 43 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_time.h" 1
# 16 "src/os/unix/ngx_time.h"
typedef ngx_rbtree_key_t ngx_msec_t;
typedef ngx_rbtree_key_int_t ngx_msec_int_t;

typedef struct tm ngx_tm_t;
# 56 "src/os/unix/ngx_time.h"
void ngx_timezone_update(void);
void ngx_localtime(time_t s, ngx_tm_t *tm);
void ngx_libc_localtime(time_t s, struct tm *tm);
void ngx_libc_gmtime(time_t s, struct tm *tm);
# 44 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_socket.h" 1
# 17 "src/os/unix/ngx_socket.h"
typedef int ngx_socket_t;







int ngx_nonblocking(ngx_socket_t s);
int ngx_blocking(ngx_socket_t s);
# 41 "src/os/unix/ngx_socket.h"
int ngx_tcp_nopush(ngx_socket_t s);
int ngx_tcp_push(ngx_socket_t s);
# 45 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_string.h" 1
# 16 "src/core/ngx_string.h"
typedef struct {
    size_t len;
    u_char *data;
} ngx_str_t;


typedef struct {
    ngx_str_t key;
    ngx_str_t value;
} ngx_keyval_t;


typedef struct {
    unsigned len:28;

    unsigned valid:1;
    unsigned no_cacheable:1;
    unsigned not_found:1;
    unsigned escape:1;

    u_char *data;
} ngx_variable_value_t;
# 50 "src/core/ngx_string.h"
void ngx_strlow(u_char *dst, u_char *src, size_t n);
# 65 "src/core/ngx_string.h"
static inline u_char *
ngx_strlchr(u_char *p, u_char *last, u_char c)
{
    while (p < last) {

        if (*p == c) {
            return p;
        }

        p++;
    }

    return ((void *)0);
}
# 147 "src/core/ngx_string.h"
u_char *ngx_cpystrn(u_char *dst, u_char *src, size_t n);
u_char *ngx_pstrdup(ngx_pool_t *pool, ngx_str_t *src);
u_char * ngx_sprintf(u_char *buf, const char *fmt, ...);
u_char * ngx_snprintf(u_char *buf, size_t max, const char *fmt, ...);
u_char * ngx_slprintf(u_char *buf, u_char *last, const char *fmt,
    ...);
u_char *ngx_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);



ngx_int_t ngx_strcasecmp(u_char *s1, u_char *s2);
ngx_int_t ngx_strncasecmp(u_char *s1, u_char *s2, size_t n);

u_char *ngx_strnstr(u_char *s1, char *s2, size_t n);

u_char *ngx_strstrn(u_char *s1, char *s2, size_t n);
u_char *ngx_strcasestrn(u_char *s1, char *s2, size_t n);
u_char *ngx_strlcasestrn(u_char *s1, u_char *last, u_char *s2, size_t n);

ngx_int_t ngx_rstrncmp(u_char *s1, u_char *s2, size_t n);
ngx_int_t ngx_rstrncasecmp(u_char *s1, u_char *s2, size_t n);
ngx_int_t ngx_memn2cmp(u_char *s1, u_char *s2, size_t n1, size_t n2);
ngx_int_t ngx_dns_strcmp(u_char *s1, u_char *s2);

ngx_int_t ngx_atoi(u_char *line, size_t n);
ngx_int_t ngx_atofp(u_char *line, size_t n, size_t point);
ssize_t ngx_atosz(u_char *line, size_t n);
off_t ngx_atoof(u_char *line, size_t n);
time_t ngx_atotm(u_char *line, size_t n);
ngx_int_t ngx_hextoi(u_char *line, size_t n);

u_char *ngx_hex_dump(u_char *dst, u_char *src, size_t len);





void ngx_encode_base64(ngx_str_t *dst, ngx_str_t *src);
ngx_int_t ngx_decode_base64(ngx_str_t *dst, ngx_str_t *src);
ngx_int_t ngx_decode_base64url(ngx_str_t *dst, ngx_str_t *src);

uint32_t ngx_utf8_decode(u_char **p, size_t n);
size_t ngx_utf8_length(u_char *p, size_t n);
u_char *ngx_utf8_cpystrn(u_char *dst, u_char *src, size_t n, size_t len);
# 204 "src/core/ngx_string.h"
uintptr_t ngx_escape_uri(u_char *dst, u_char *src, size_t size,
    ngx_uint_t type);
void ngx_unescape_uri(u_char **dst, u_char **src, size_t size, ngx_uint_t type);
uintptr_t ngx_escape_html(u_char *dst, u_char *src, size_t size);


typedef struct {
    ngx_rbtree_node_t node;
    ngx_str_t str;
} ngx_str_node_t;


void ngx_str_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
ngx_str_node_t *ngx_str_rbtree_lookup(ngx_rbtree_t *rbtree, ngx_str_t *name,
    uint32_t hash);


void ngx_sort(void *base, size_t n, size_t size,
    ngx_int_t (*cmp)(const void *, const void *));
# 46 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_files.h" 1
# 16 "src/os/unix/ngx_files.h"
typedef int ngx_fd_t;
typedef struct stat ngx_file_info_t;
typedef ino_t ngx_file_uniq_t;


typedef struct {
    u_char *name;
    size_t size;
    void *addr;
    ngx_fd_t fd;
    ngx_log_t *log;
} ngx_file_mapping_t;


typedef struct {
    DIR *dir;
    struct dirent *de;
    struct stat info;

    unsigned type:8;
    unsigned valid_info:1;
} ngx_dir_t;


typedef struct {
    size_t n;
    glob_t pglob;
    u_char *pattern;
    ngx_log_t *log;
    ngx_uint_t test;
} ngx_glob_t;
# 112 "src/os/unix/ngx_files.h"
ngx_fd_t ngx_open_tempfile(u_char *name, ngx_uint_t persistent,
    ngx_uint_t access);



ssize_t ngx_read_file(ngx_file_t *file, u_char *buf, size_t size, off_t offset);






ssize_t ngx_write_file(ngx_file_t *file, u_char *buf, size_t size,
    off_t offset);

ssize_t ngx_write_chain_to_file(ngx_file_t *file, ngx_chain_t *ce,
    off_t offset, ngx_pool_t *pool);
# 139 "src/os/unix/ngx_files.h"
static inline ssize_t
ngx_write_fd(ngx_fd_t fd, void *buf, size_t n)
{
    return write(fd, buf, n);
}
# 164 "src/os/unix/ngx_files.h"
ngx_int_t ngx_set_file_time(u_char *name, ngx_fd_t fd, time_t s);
# 188 "src/os/unix/ngx_files.h"
ngx_int_t ngx_create_file_mapping(ngx_file_mapping_t *fm);
void ngx_close_file_mapping(ngx_file_mapping_t *fm);
# 225 "src/os/unix/ngx_files.h"
ngx_int_t ngx_open_dir(ngx_str_t *name, ngx_dir_t *dir);







ngx_int_t ngx_read_dir(ngx_dir_t *dir);
# 255 "src/os/unix/ngx_files.h"
static inline ngx_int_t
ngx_de_info(u_char *name, ngx_dir_t *dir)
{
    dir->type = 0;
    return stat((const char *) name, &dir->info);
}
# 295 "src/os/unix/ngx_files.h"
ngx_int_t ngx_open_glob(ngx_glob_t *gl);

ngx_int_t ngx_read_glob(ngx_glob_t *gl, ngx_str_t *name);
void ngx_close_glob(ngx_glob_t *gl);


ngx_err_t ngx_trylock_fd(ngx_fd_t fd);
ngx_err_t ngx_lock_fd(ngx_fd_t fd);
ngx_err_t ngx_unlock_fd(ngx_fd_t fd);
# 321 "src/os/unix/ngx_files.h"
ngx_int_t ngx_read_ahead(ngx_fd_t fd, size_t n);
# 334 "src/os/unix/ngx_files.h"
ngx_int_t ngx_directio_on(ngx_fd_t fd);


ngx_int_t ngx_directio_off(ngx_fd_t fd);
# 357 "src/os/unix/ngx_files.h"
size_t ngx_fs_bsize(u_char *name);
# 47 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_shmem.h" 1
# 16 "src/os/unix/ngx_shmem.h"
typedef struct {
    u_char *addr;
    size_t size;
    ngx_str_t name;
    ngx_log_t *log;
    ngx_uint_t exists;
} ngx_shm_t;


ngx_int_t ngx_shm_alloc(ngx_shm_t *shm);
void ngx_shm_free(ngx_shm_t *shm);
# 48 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_process.h" 1
# 12 "src/os/unix/ngx_process.h"
# 1 "src/os/unix/ngx_setaffinity.h" 1
# 14 "src/os/unix/ngx_setaffinity.h"
void ngx_setaffinity(uint64_t cpu_affinity, ngx_log_t *log);
# 13 "src/os/unix/ngx_process.h" 2
# 1 "src/os/unix/ngx_setproctitle.h" 1
# 37 "src/os/unix/ngx_setproctitle.h"
ngx_int_t ngx_init_setproctitle(ngx_log_t *log);
void ngx_setproctitle(char *title);
# 14 "src/os/unix/ngx_process.h" 2


typedef pid_t ngx_pid_t;



typedef void (*ngx_spawn_proc_pt) (ngx_cycle_t *cycle, void *data);

typedef struct {
    ngx_pid_t pid;
    int status;
    ngx_socket_t channel[2];

    ngx_spawn_proc_pt proc;
    void *data;
    char *name;

    unsigned respawn:1;
    unsigned just_spawn:1;
    unsigned detached:1;
    unsigned exiting:1;
    unsigned exited:1;
} ngx_process_t;


typedef struct {
    char *path;
    char *name;
    char *const *argv;
    char *const *envp;
} ngx_exec_ctx_t;
# 63 "src/os/unix/ngx_process.h"
ngx_pid_t ngx_spawn_process(ngx_cycle_t *cycle,
    ngx_spawn_proc_pt proc, void *data, char *name, ngx_int_t respawn);
ngx_pid_t ngx_execute(ngx_cycle_t *cycle, ngx_exec_ctx_t *ctx);
ngx_int_t ngx_init_signals(ngx_log_t *log);
void ngx_debug_point(void);
# 77 "src/os/unix/ngx_process.h"
extern int ngx_argc;
extern char **ngx_argv;
extern char **ngx_os_argv;

extern ngx_pid_t ngx_pid;
extern ngx_socket_t ngx_channel;
extern ngx_int_t ngx_process_slot;
extern ngx_int_t ngx_last_process;
extern ngx_process_t ngx_processes[1024];
# 49 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_user.h" 1
# 16 "src/os/unix/ngx_user.h"
typedef uid_t ngx_uid_t;
typedef gid_t ngx_gid_t;


ngx_int_t ngx_libc_crypt(ngx_pool_t *pool, u_char *key, u_char *salt,
    u_char **encrypted);
# 50 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_parse.h" 1
# 16 "src/core/ngx_parse.h"
ssize_t ngx_parse_size(ngx_str_t *line);
off_t ngx_parse_offset(ngx_str_t *line);
ngx_int_t ngx_parse_time(ngx_str_t *line, ngx_uint_t is_sec);
# 51 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_log.h" 1
# 45 "src/core/ngx_log.h"
typedef u_char *(*ngx_log_handler_pt) (ngx_log_t *log, u_char *buf, size_t len);


struct ngx_log_s {
    ngx_uint_t log_level;
    ngx_open_file_t *file;

    ngx_atomic_uint_t connection;

    ngx_log_handler_pt handler;
    void *data;







    char *action;
};
# 79 "src/core/ngx_log.h"
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...);
# 222 "src/core/ngx_log.h"
ngx_log_t *ngx_log_init(u_char *prefix);
ngx_log_t *ngx_log_create(ngx_cycle_t *cycle, ngx_str_t *name);
char *ngx_log_set_levels(ngx_conf_t *cf, ngx_log_t *log);
void ngx_log_abort(ngx_err_t err, const char *fmt, ...);
void ngx_log_stderr(ngx_err_t err, const char *fmt, ...);
u_char *ngx_log_errno(u_char *buf, u_char *last, ngx_err_t err);
# 239 "src/core/ngx_log.h"
static inline void
ngx_write_stderr(char *text)
{
    (void) ngx_write_fd(2, text, strlen(text));
}


extern ngx_module_t ngx_errlog_module;
extern ngx_uint_t ngx_use_stderr;
# 52 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_alloc.h" 1
# 16 "src/os/unix/ngx_alloc.h"
void *ngx_alloc(size_t size, ngx_log_t *log);
void *ngx_calloc(size_t size, ngx_log_t *log);
# 31 "src/os/unix/ngx_alloc.h"
void *ngx_memalign(size_t alignment, size_t size, ngx_log_t *log);
# 40 "src/os/unix/ngx_alloc.h"
extern ngx_uint_t ngx_pagesize;
extern ngx_uint_t ngx_pagesize_shift;
extern ngx_uint_t ngx_cacheline_size;
# 53 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_palloc.h" 1
# 30 "src/core/ngx_palloc.h"
typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt handler;
    void *data;
    ngx_pool_cleanup_t *next;
};


typedef struct ngx_pool_large_s ngx_pool_large_t;

struct ngx_pool_large_s {
    ngx_pool_large_t *next;
    void *alloc;
};


typedef struct {
    u_char *last;
    u_char *end;
    ngx_pool_t *next;
    ngx_uint_t failed;
} ngx_pool_data_t;


struct ngx_pool_s {
    ngx_pool_data_t d;
    size_t max;
    ngx_pool_t *current;
    ngx_chain_t *chain;
    ngx_pool_large_t *large;
    ngx_pool_cleanup_t *cleanup;
    ngx_log_t *log;
};


typedef struct {
    ngx_fd_t fd;
    u_char *name;
    ngx_log_t *log;
} ngx_pool_cleanup_file_t;


void *ngx_alloc(size_t size, ngx_log_t *log);
void *ngx_calloc(size_t size, ngx_log_t *log);

ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);
void ngx_reset_pool(ngx_pool_t *pool);

void *ngx_palloc(ngx_pool_t *pool, size_t size);
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void *data);
void ngx_pool_delete_file(void *data);
# 54 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_buf.h" 1
# 16 "src/core/ngx_buf.h"
typedef void * ngx_buf_tag_t;

typedef struct ngx_buf_s ngx_buf_t;

struct ngx_buf_s {
    u_char *pos;
    u_char *last;
    off_t file_pos;
    off_t file_last;

    u_char *start;
    u_char *end;
    ngx_buf_tag_t tag;
    ngx_file_t *file;
    ngx_buf_t *shadow;



    unsigned temporary:1;





    unsigned memory:1;


    unsigned mmap:1;

    unsigned recycled:1;
    unsigned in_file:1;
    unsigned flush:1;
    unsigned sync:1;
    unsigned last_buf:1;
    unsigned last_in_chain:1;

    unsigned last_shadow:1;
    unsigned temp_file:1;

               int num;
};


struct ngx_chain_s {
    ngx_buf_t *buf;
    ngx_chain_t *next;
};


typedef struct {
    ngx_int_t num;
    size_t size;
} ngx_bufs_t;


typedef struct ngx_output_chain_ctx_s ngx_output_chain_ctx_t;

typedef ngx_int_t (*ngx_output_chain_filter_pt)(void *ctx, ngx_chain_t *in);






struct ngx_output_chain_ctx_s {
    ngx_buf_t *buf;
    ngx_chain_t *in;
    ngx_chain_t *free;
    ngx_chain_t *busy;

    unsigned sendfile:1;
    unsigned directio:1;

    unsigned unaligned:1;

    unsigned need_in_memory:1;
    unsigned need_in_temp:1;






    off_t alignment;

    ngx_pool_t *pool;
    ngx_int_t allocated;
    ngx_bufs_t bufs;
    ngx_buf_tag_t tag;

    ngx_output_chain_filter_pt output_filter;
    void *filter_ctx;
};


typedef struct {
    ngx_chain_t *out;
    ngx_chain_t **last;
    ngx_connection_t *connection;
    ngx_pool_t *pool;
    off_t limit;
} ngx_chain_writer_ctx_t;
# 138 "src/core/ngx_buf.h"
ngx_buf_t *ngx_create_temp_buf(ngx_pool_t *pool, size_t size);
ngx_chain_t *ngx_create_chain_of_bufs(ngx_pool_t *pool, ngx_bufs_t *bufs);





ngx_chain_t *ngx_alloc_chain_link(ngx_pool_t *pool);






ngx_int_t ngx_output_chain(ngx_output_chain_ctx_t *ctx, ngx_chain_t *in);
ngx_int_t ngx_chain_writer(void *ctx, ngx_chain_t *in);

ngx_int_t ngx_chain_add_copy(ngx_pool_t *pool, ngx_chain_t **chain,
    ngx_chain_t *in);
ngx_chain_t *ngx_chain_get_free_buf(ngx_pool_t *p, ngx_chain_t **free);
void ngx_chain_update_chains(ngx_pool_t *p, ngx_chain_t **free,
    ngx_chain_t **busy, ngx_chain_t **out, ngx_buf_tag_t tag);
# 55 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_queue.h" 1
# 16 "src/core/ngx_queue.h"
typedef struct ngx_queue_s ngx_queue_t;

struct ngx_queue_s {
    ngx_queue_t *prev;
    ngx_queue_t *next;
};
# 107 "src/core/ngx_queue.h"
ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue);
void ngx_queue_sort(ngx_queue_t *queue,
    ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *));
# 56 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_array.h" 1
# 16 "src/core/ngx_array.h"
struct ngx_array_s {
    void *elts;
    ngx_uint_t nelts;
    size_t size;
    ngx_uint_t nalloc;
    ngx_pool_t *pool;
};


ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);
void ngx_array_destroy(ngx_array_t *a);
void *ngx_array_push(ngx_array_t *a);
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);


static inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{





    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == ((void *)0)) {
        return -1;
    }

    return 0;
}
# 57 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_list.h" 1
# 16 "src/core/ngx_list.h"
typedef struct ngx_list_part_s ngx_list_part_t;

struct ngx_list_part_s {
    void *elts;
    ngx_uint_t nelts;
    ngx_list_part_t *next;
};


typedef struct {
    ngx_list_part_t *last;
    ngx_list_part_t part;
    size_t size;
    ngx_uint_t nalloc;
    ngx_pool_t *pool;
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

static inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == ((void *)0)) {
        return -1;
    }

    list->part.nelts = 0;
    list->part.next = ((void *)0);
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return 0;
}
# 80 "src/core/ngx_list.h"
void *ngx_list_push(ngx_list_t *list);
# 58 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_hash.h" 1
# 16 "src/core/ngx_hash.h"
typedef struct {
    void *value;
    u_short len;
    u_char name[1];
} ngx_hash_elt_t;


typedef struct {
    ngx_hash_elt_t **buckets;
    ngx_uint_t size;
} ngx_hash_t;


typedef struct {
    ngx_hash_t hash;
    void *value;
} ngx_hash_wildcard_t;


typedef struct {
    ngx_str_t key;
    ngx_uint_t key_hash;
    void *value;
} ngx_hash_key_t;


typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t hash;
    ngx_hash_wildcard_t *wc_head;
    ngx_hash_wildcard_t *wc_tail;
} ngx_hash_combined_t;


typedef struct {
    ngx_hash_t *hash;
    ngx_hash_key_pt key;

    ngx_uint_t max_size;
    ngx_uint_t bucket_size;

    char *name;
    ngx_pool_t *pool;
    ngx_pool_t *temp_pool;
} ngx_hash_init_t;
# 75 "src/core/ngx_hash.h"
typedef struct {
    ngx_uint_t hsize;

    ngx_pool_t *pool;
    ngx_pool_t *temp_pool;

    ngx_array_t keys;
    ngx_array_t *keys_hash;

    ngx_array_t dns_wc_head;
    ngx_array_t *dns_wc_head_hash;

    ngx_array_t dns_wc_tail;
    ngx_array_t *dns_wc_tail_hash;
} ngx_hash_keys_arrays_t;


typedef struct {
    ngx_uint_t hash;
    ngx_str_t key;
    ngx_str_t value;
    u_char *lowcase_key;
} ngx_table_elt_t;


void *ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len);
void *ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,
    u_char *name, size_t len);

ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);


ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);
# 59 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_file.h" 1
# 16 "src/core/ngx_file.h"
struct ngx_file_s {
    ngx_fd_t fd;
    ngx_str_t name;
    ngx_file_info_t info;

    off_t offset;
    off_t sys_offset;

    ngx_log_t *log;





    unsigned valid_info:1;
    unsigned directio:1;
};





typedef time_t (*ngx_path_manager_pt) (void *data);
typedef void (*ngx_path_loader_pt) (void *data);


typedef struct {
    ngx_str_t name;
    size_t len;
    size_t level[3];

    ngx_path_manager_pt manager;
    ngx_path_loader_pt loader;
    void *data;

    u_char *conf_file;
    ngx_uint_t line;
} ngx_path_t;


typedef struct {
    ngx_str_t name;
    size_t level[3];
} ngx_path_init_t;


typedef struct {
    ngx_file_t file;
    off_t offset;
    ngx_path_t *path;
    ngx_pool_t *pool;
    char *warn;

    ngx_uint_t access;

    unsigned log_level:8;
    unsigned persistent:1;
    unsigned clean:1;
} ngx_temp_file_t;


typedef struct {
    ngx_uint_t access;
    ngx_uint_t path_access;
    time_t time;
    ngx_fd_t fd;

    unsigned create_path:1;
    unsigned delete_file:1;

    ngx_log_t *log;
} ngx_ext_rename_file_t;


typedef struct {
    off_t size;
    size_t buf_size;

    ngx_uint_t access;
    time_t time;

    ngx_log_t *log;
} ngx_copy_file_t;


typedef struct ngx_tree_ctx_s ngx_tree_ctx_t;

typedef ngx_int_t (*ngx_tree_init_handler_pt) (void *ctx, void *prev);
typedef ngx_int_t (*ngx_tree_handler_pt) (ngx_tree_ctx_t *ctx, ngx_str_t *name);

struct ngx_tree_ctx_s {
    off_t size;
    off_t fs_size;
    ngx_uint_t access;
    time_t mtime;

    ngx_tree_init_handler_pt init_handler;
    ngx_tree_handler_pt file_handler;
    ngx_tree_handler_pt pre_tree_handler;
    ngx_tree_handler_pt post_tree_handler;
    ngx_tree_handler_pt spec_handler;

    void *data;
    size_t alloc;

    ngx_log_t *log;
};


ssize_t ngx_write_chain_to_temp_file(ngx_temp_file_t *tf, ngx_chain_t *chain);
ngx_int_t ngx_create_temp_file(ngx_file_t *file, ngx_path_t *path,
    ngx_pool_t *pool, ngx_uint_t persistent, ngx_uint_t clean,
    ngx_uint_t access);
void ngx_create_hashed_filename(ngx_path_t *path, u_char *file, size_t len);
ngx_int_t ngx_create_path(ngx_file_t *file, ngx_path_t *path);
ngx_err_t ngx_create_full_path(u_char *dir, ngx_uint_t access);
ngx_int_t ngx_add_path(ngx_conf_t *cf, ngx_path_t **slot);
ngx_int_t ngx_create_pathes(ngx_cycle_t *cycle, ngx_uid_t user);
ngx_int_t ngx_ext_rename_file(ngx_str_t *src, ngx_str_t *to,
    ngx_ext_rename_file_t *ext);
ngx_int_t ngx_copy_file(u_char *from, u_char *to, ngx_copy_file_t *cf);
ngx_int_t ngx_walk_tree(ngx_tree_ctx_t *ctx, ngx_str_t *tree);

ngx_atomic_uint_t ngx_next_temp_number(ngx_uint_t collision);

char *ngx_conf_set_path_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_merge_path_value(ngx_conf_t *cf, ngx_path_t **path,
    ngx_path_t *prev, ngx_path_init_t *init);
char *ngx_conf_set_access_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


extern ngx_atomic_t *ngx_temp_number;
extern ngx_atomic_int_t ngx_random_number;
# 60 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_crc.h" 1
# 18 "src/core/ngx_crc.h"
static inline uint32_t
ngx_crc(u_char *data, size_t len)
{
    uint32_t sum;

    for (sum = 0; len; len--) {






        sum = sum >> 1 | sum << 31;

        sum += *data++;
    }

    return sum;
}
# 61 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_crc32.h" 1
# 16 "src/core/ngx_crc32.h"
extern uint32_t *ngx_crc32_table_short;
extern uint32_t ngx_crc32_table256[];


static inline uint32_t
ngx_crc32_short(u_char *p, size_t len)
{
    u_char c;
    uint32_t crc;

    crc = 0xffffffff;

    while (len--) {
        c = *p++;
        crc = ngx_crc32_table_short[(crc ^ (c & 0xf)) & 0xf] ^ (crc >> 4);
        crc = ngx_crc32_table_short[(crc ^ (c >> 4)) & 0xf] ^ (crc >> 4);
    }

    return crc ^ 0xffffffff;
}


static inline uint32_t
ngx_crc32_long(u_char *p, size_t len)
{
    uint32_t crc;

    crc = 0xffffffff;

    while (len--) {
        crc = ngx_crc32_table256[(crc ^ *p++) & 0xff] ^ (crc >> 8);
    }

    return crc ^ 0xffffffff;
}






static inline void
ngx_crc32_update(uint32_t *crc, u_char *p, size_t len)
{
    uint32_t c;

    c = *crc;

    while (len--) {
        c = ngx_crc32_table256[(c ^ *p++) & 0xff] ^ (c >> 8);
    }

    *crc = c;
}






ngx_int_t ngx_crc32_table_init(void);
# 62 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_murmurhash.h" 1
# 16 "src/core/ngx_murmurhash.h"
uint32_t ngx_murmur_hash2(u_char *data, size_t len);
# 63 "src/core/ngx_core.h" 2

# 1 "src/core/ngx_regex.h" 1
# 15 "src/core/ngx_regex.h"
# 1 "/usr/include/pcre.h" 1 3 4
# 181 "/usr/include/pcre.h" 3 4
struct real_pcre;
typedef struct real_pcre pcre;
# 196 "/usr/include/pcre.h" 3 4
typedef struct pcre_extra {
  unsigned long int flags;
  void *study_data;
  unsigned long int match_limit;
  void *callout_data;
  const unsigned char *tables;
  unsigned long int match_limit_recursion;
} pcre_extra;






typedef struct pcre_callout_block {
  int version;

  int callout_number;
  int *offset_vector;
  const char * subject;
  int subject_length;
  int start_match;
  int current_position;
  int capture_top;
  int capture_last;
  void *callout_data;

  int pattern_position;
  int next_item_length;

} pcre_callout_block;
# 235 "/usr/include/pcre.h" 3 4
extern void *(*pcre_malloc)(size_t);
extern void (*pcre_free)(void *);
extern void *(*pcre_stack_malloc)(size_t);
extern void (*pcre_stack_free)(void *);
extern int (*pcre_callout)(pcre_callout_block *);
# 250 "/usr/include/pcre.h" 3 4
extern pcre *pcre_compile(const char *, int, const char **, int *,
                  const unsigned char *);
extern pcre *pcre_compile2(const char *, int, int *, const char **,
                  int *, const unsigned char *);
extern int pcre_config(int, void *);
extern int pcre_copy_named_substring(const pcre *, const char *,
                  int *, int, const char *, char *, int);
extern int pcre_copy_substring(const char *, int *, int, int, char *,
                  int);
extern int pcre_dfa_exec(const pcre *, const pcre_extra *,
                  const char *, int, int, int, int *, int , int *, int);
extern int pcre_exec(const pcre *, const pcre_extra *, const char *,
                   int, int, int, int *, int);
extern void pcre_free_substring(const char *);
extern void pcre_free_substring_list(const char **);
extern int pcre_fullinfo(const pcre *, const pcre_extra *, int,
                  void *);
extern int pcre_get_named_substring(const pcre *, const char *,
                  int *, int, const char *, const char **);
extern int pcre_get_stringnumber(const pcre *, const char *);
extern int pcre_get_substring(const char *, int *, int, int,
                  const char **);
extern int pcre_get_substring_list(const char *, int *, int,
                  const char ***);
extern int pcre_info(const pcre *, int *, int *);
extern const unsigned char *pcre_maketables(void);
extern int pcre_refcount(pcre *, int);
extern pcre_extra *pcre_study(const pcre *, int, const char **);
extern const char *pcre_version(void);
# 16 "src/core/ngx_regex.h" 2







typedef struct {
    pcre *code;
    pcre_extra *extra;
} ngx_regex_t;


typedef struct {
    ngx_str_t pattern;
    ngx_pool_t *pool;
    ngx_int_t options;

    ngx_regex_t *regex;
    int captures;
    int named_captures;
    int name_size;
    u_char *names;
    ngx_str_t err;
} ngx_regex_compile_t;


typedef struct {
    ngx_regex_t *regex;
    u_char *name;
} ngx_regex_elt_t;


void ngx_regex_init(void);
ngx_int_t ngx_regex_compile(ngx_regex_compile_t *rc);






ngx_int_t ngx_regex_exec_array(ngx_array_t *a, ngx_str_t *s, ngx_log_t *log);
# 65 "src/core/ngx_core.h" 2

# 1 "src/core/ngx_radix_tree.h" 1
# 18 "src/core/ngx_radix_tree.h"
typedef struct ngx_radix_node_s ngx_radix_node_t;

struct ngx_radix_node_s {
    ngx_radix_node_t *right;
    ngx_radix_node_t *left;
    ngx_radix_node_t *parent;
    uintptr_t value;
};


typedef struct {
    ngx_radix_node_t *root;
    ngx_pool_t *pool;
    ngx_radix_node_t *free;
    char *start;
    size_t size;
} ngx_radix_tree_t;


ngx_radix_tree_t *ngx_radix_tree_create(ngx_pool_t *pool,
    ngx_int_t preallocate);
ngx_int_t ngx_radix32tree_insert(ngx_radix_tree_t *tree,
    uint32_t key, uint32_t mask, uintptr_t value);
ngx_int_t ngx_radix32tree_delete(ngx_radix_tree_t *tree,
    uint32_t key, uint32_t mask);
uintptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, uint32_t key);
# 67 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_times.h" 1
# 16 "src/core/ngx_times.h"
typedef struct {
    time_t sec;
    ngx_uint_t msec;
    ngx_int_t gmtoff;
} ngx_time_t;


void ngx_time_init(void);
void ngx_time_update(void);
void ngx_time_sigsafe_update(void);
u_char *ngx_http_time(u_char *buf, time_t t);
u_char *ngx_http_cookie_time(u_char *buf, time_t t);
void ngx_gmtime(time_t t, ngx_tm_t *tp);

time_t ngx_next_time(time_t when);



extern volatile ngx_time_t *ngx_cached_time;




extern volatile ngx_str_t ngx_cached_err_log_time;
extern volatile ngx_str_t ngx_cached_http_time;
extern volatile ngx_str_t ngx_cached_http_log_time;
extern volatile ngx_str_t ngx_cached_http_log_iso8601;





extern volatile ngx_msec_t ngx_current_msec;
# 68 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_shmtx.h" 1
# 16 "src/core/ngx_shmtx.h"
typedef struct {
    ngx_atomic_t lock;

    ngx_atomic_t wait;

} ngx_shmtx_sh_t;


typedef struct {

    ngx_atomic_t *lock;

    ngx_atomic_t *wait;
    ngx_uint_t semaphore;
    sem_t sem;





    ngx_uint_t spin;
} ngx_shmtx_t;


ngx_int_t ngx_shmtx_create(ngx_shmtx_t *mtx, ngx_shmtx_sh_t *addr,
    u_char *name);
void ngx_shmtx_destroy(ngx_shmtx_t *mtx);
ngx_uint_t ngx_shmtx_trylock(ngx_shmtx_t *mtx);
void ngx_shmtx_lock(ngx_shmtx_t *mtx);
void ngx_shmtx_unlock(ngx_shmtx_t *mtx);
ngx_uint_t ngx_shmtx_force_unlock(ngx_shmtx_t *mtx, ngx_pid_t pid);
# 69 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_slab.h" 1
# 16 "src/core/ngx_slab.h"
typedef struct ngx_slab_page_s ngx_slab_page_t;

struct ngx_slab_page_s {
    uintptr_t slab;
    ngx_slab_page_t *next;
    uintptr_t prev;
};


typedef struct {
    ngx_shmtx_sh_t lock;

    size_t min_size;
    size_t min_shift;

    ngx_slab_page_t *pages;
    ngx_slab_page_t free;

    u_char *start;
    u_char *end;

    ngx_shmtx_t mutex;

    u_char *log_ctx;
    u_char zero;

    void *data;
    void *addr;
} ngx_slab_pool_t;


void ngx_slab_init(ngx_slab_pool_t *pool);
void *ngx_slab_alloc(ngx_slab_pool_t *pool, size_t size);
void *ngx_slab_alloc_locked(ngx_slab_pool_t *pool, size_t size);
void ngx_slab_free(ngx_slab_pool_t *pool, void *p);
void ngx_slab_free_locked(ngx_slab_pool_t *pool, void *p);
# 70 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_inet.h" 1
# 43 "src/core/ngx_inet.h"
typedef struct {
    in_addr_t addr;
    in_addr_t mask;
} ngx_in_cidr_t;
# 59 "src/core/ngx_inet.h"
typedef struct {
    ngx_uint_t family;
    union {
        ngx_in_cidr_t in;



    } u;
} ngx_cidr_t;


typedef struct {
    struct sockaddr *sockaddr;
    socklen_t socklen;
    ngx_str_t name;
} ngx_addr_t;


typedef struct {
    ngx_str_t url;
    ngx_str_t host;
    ngx_str_t port_text;
    ngx_str_t uri;

    in_port_t port;
    in_port_t default_port;
    int family;

    unsigned listen:1;
    unsigned uri_part:1;
    unsigned no_resolve:1;
    unsigned one_addr:1;

    unsigned no_port:1;
    unsigned wildcard:1;

    socklen_t socklen;
    u_char sockaddr[sizeof(struct sockaddr_un)];

    ngx_addr_t *addrs;
    ngx_uint_t naddrs;

    char *err;
} ngx_url_t;


in_addr_t ngx_inet_addr(u_char *text, size_t len);




size_t ngx_sock_ntop(struct sockaddr *sa, u_char *text, size_t len,
    ngx_uint_t port);
size_t ngx_inet_ntop(int family, void *addr, u_char *text, size_t len);
ngx_int_t ngx_ptocidr(ngx_str_t *text, ngx_cidr_t *cidr);
ngx_int_t ngx_parse_addr(ngx_pool_t *pool, ngx_addr_t *addr, u_char *text,
    size_t len);
ngx_int_t ngx_parse_url(ngx_pool_t *pool, ngx_url_t *u);
ngx_int_t ngx_inet_resolve_host(ngx_pool_t *pool, ngx_url_t *u);
# 71 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_cycle.h" 1
# 25 "src/core/ngx_cycle.h"
typedef struct ngx_shm_zone_s ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s {
    void *data;
    ngx_shm_t shm;
    ngx_shm_zone_init_pt init;
    void *tag;
};


struct ngx_cycle_s {
    void ****conf_ctx;
    ngx_pool_t *pool;

    ngx_log_t *log;
    ngx_log_t new_log;

    ngx_connection_t **files;
    ngx_connection_t *free_connections;
    ngx_uint_t free_connection_n;

    ngx_queue_t reusable_connections_queue;

    ngx_array_t listening;
    ngx_array_t pathes;
    ngx_list_t open_files;
    ngx_list_t shared_memory;

    ngx_uint_t connection_n;
    ngx_uint_t files_n;

    ngx_connection_t *connections;
    ngx_event_t *read_events;
    ngx_event_t *write_events;

    ngx_cycle_t *old_cycle;

    ngx_str_t conf_file;
    ngx_str_t conf_param;
    ngx_str_t conf_prefix;
    ngx_str_t prefix;
    ngx_str_t lock_file;
    ngx_str_t hostname;
};


typedef struct {
     ngx_flag_t daemon;
     ngx_flag_t master;

     ngx_msec_t timer_resolution;

     ngx_int_t worker_processes;
     ngx_int_t debug_points;

     ngx_int_t rlimit_nofile;
     ngx_int_t rlimit_sigpending;
     off_t rlimit_core;

     int priority;

     ngx_uint_t cpu_affinity_n;
     uint64_t *cpu_affinity;

     char *username;
     ngx_uid_t user;
     ngx_gid_t group;

     ngx_str_t working_directory;
     ngx_str_t lock_file;

     ngx_str_t pid;
     ngx_str_t oldpid;

     ngx_array_t env;
     char **environment;






} ngx_core_conf_t;


typedef struct {
     ngx_pool_t *pool;
} ngx_core_tls_t;





ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
uint64_t ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);


extern volatile ngx_cycle_t *ngx_cycle;
extern ngx_array_t ngx_old_cycles;
extern ngx_module_t ngx_core_module;
extern ngx_uint_t ngx_test_config;
extern ngx_uint_t ngx_quiet_mode;
# 72 "src/core/ngx_core.h" 2



# 1 "src/os/unix/ngx_process_cycle.h" 1
# 30 "src/os/unix/ngx_process_cycle.h"
typedef struct {
    ngx_event_handler_pt handler;
    char *name;
    ngx_msec_t delay;
} ngx_cache_manager_ctx_t;


void ngx_master_process_cycle(ngx_cycle_t *cycle);
void ngx_single_process_cycle(ngx_cycle_t *cycle);


extern ngx_uint_t ngx_process;
extern ngx_pid_t ngx_pid;
extern ngx_pid_t ngx_new_binary;
extern ngx_uint_t ngx_inherited;
extern ngx_uint_t ngx_daemonized;
extern ngx_uint_t ngx_threaded;
extern ngx_uint_t ngx_exiting;

extern sig_atomic_t ngx_reap;
extern sig_atomic_t ngx_sigio;
extern sig_atomic_t ngx_sigalrm;
extern sig_atomic_t ngx_quit;
extern sig_atomic_t ngx_debug_quit;
extern sig_atomic_t ngx_terminate;
extern sig_atomic_t ngx_noaccept;
extern sig_atomic_t ngx_reconfigure;
extern sig_atomic_t ngx_reopen;
extern sig_atomic_t ngx_change_binary;
# 76 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_conf_file.h" 1
# 78 "src/core/ngx_conf_file.h"
struct ngx_command_s {
    ngx_str_t name;
    ngx_uint_t type;
    char *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    ngx_uint_t conf;
    ngx_uint_t offset;
    void *post;
};




struct ngx_open_file_s {
    ngx_fd_t fd;
    ngx_str_t name;

    u_char *buffer;
    u_char *pos;
    u_char *last;
# 105 "src/core/ngx_conf_file.h"
};





struct ngx_module_s {
    ngx_uint_t ctx_index;
    ngx_uint_t index;

    ngx_uint_t spare0;
    ngx_uint_t spare1;
    ngx_uint_t spare2;
    ngx_uint_t spare3;

    ngx_uint_t version;

    void *ctx;
    ngx_command_t *commands;
    ngx_uint_t type;

    ngx_int_t (*init_master)(ngx_log_t *log);

    ngx_int_t (*init_module)(ngx_cycle_t *cycle);

    ngx_int_t (*init_process)(ngx_cycle_t *cycle);
    ngx_int_t (*init_thread)(ngx_cycle_t *cycle);
    void (*exit_thread)(ngx_cycle_t *cycle);
    void (*exit_process)(ngx_cycle_t *cycle);

    void (*exit_master)(ngx_cycle_t *cycle);

    uintptr_t spare_hook0;
    uintptr_t spare_hook1;
    uintptr_t spare_hook2;
    uintptr_t spare_hook3;
    uintptr_t spare_hook4;
    uintptr_t spare_hook5;
    uintptr_t spare_hook6;
    uintptr_t spare_hook7;
};


typedef struct {
    ngx_str_t name;
    void *(*create_conf)(ngx_cycle_t *cycle);
    char *(*init_conf)(ngx_cycle_t *cycle, void *conf);
} ngx_core_module_t;


typedef struct {
    ngx_file_t file;
    ngx_buf_t *buffer;
    ngx_uint_t line;
} ngx_conf_file_t;


typedef char *(*ngx_conf_handler_pt)(ngx_conf_t *cf,
    ngx_command_t *dummy, void *conf);


struct ngx_conf_s {
    char *name;
    ngx_array_t *args;

    ngx_cycle_t *cycle;
    ngx_pool_t *pool;
    ngx_pool_t *temp_pool;
    ngx_conf_file_t *conf_file;
    ngx_log_t *log;

    void *ctx;
    ngx_uint_t module_type;
    ngx_uint_t cmd_type;

    ngx_conf_handler_pt handler;
    char *handler_conf;
};


typedef char *(*ngx_conf_post_handler_pt) (ngx_conf_t *cf,
    void *data, void *conf);

typedef struct {
    ngx_conf_post_handler_pt post_handler;
} ngx_conf_post_t;


typedef struct {
    ngx_conf_post_handler_pt post_handler;
    char *old_name;
    char *new_name;
} ngx_conf_deprecated_t;


typedef struct {
    ngx_conf_post_handler_pt post_handler;
    ngx_int_t low;
    ngx_int_t high;
} ngx_conf_num_bounds_t;


typedef struct {
    ngx_str_t name;
    ngx_uint_t value;
} ngx_conf_enum_t;




typedef struct {
    ngx_str_t name;
    ngx_uint_t mask;
} ngx_conf_bitmask_t;



char * ngx_conf_deprecated(ngx_conf_t *cf, void *post, void *data);
char *ngx_conf_check_num_bounds(ngx_conf_t *cf, void *post, void *data);
# 318 "src/core/ngx_conf_file.h"
char *ngx_conf_param(ngx_conf_t *cf);
char *ngx_conf_parse(ngx_conf_t *cf, ngx_str_t *filename);


ngx_int_t ngx_conf_full_name(ngx_cycle_t *cycle, ngx_str_t *name,
    ngx_uint_t conf_prefix);
ngx_open_file_t *ngx_conf_open_file(ngx_cycle_t *cycle, ngx_str_t *name);
void ngx_conf_log_error(ngx_uint_t level, ngx_conf_t *cf,
    ngx_err_t err, const char *fmt, ...);


char *ngx_conf_set_flag_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_str_array_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_conf_set_keyval_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_num_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_size_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_off_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_msec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_sec_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bufs_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_enum_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_conf_set_bitmask_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


extern ngx_uint_t ngx_max_module;
extern ngx_module_t *ngx_modules[];
# 77 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_resolver.h" 1
# 36 "src/core/ngx_resolver.h"
typedef struct {
    ngx_connection_t *connection;
    struct sockaddr *sockaddr;
    socklen_t socklen;
    ngx_str_t server;
    ngx_log_t log;
} ngx_udp_connection_t;


typedef struct ngx_resolver_ctx_s ngx_resolver_ctx_t;

typedef void (*ngx_resolver_handler_pt)(ngx_resolver_ctx_t *ctx);


typedef struct {
    ngx_rbtree_node_t node;
    ngx_queue_t queue;


    u_char *name;

    u_short nlen;
    u_short qlen;

    u_char *query;

    union {
        in_addr_t addr;
        in_addr_t *addrs;
        u_char *cname;
    } u;

    u_short naddrs;
    u_short cnlen;

    time_t expire;
    time_t valid;

    ngx_resolver_ctx_t *waiting;
} ngx_resolver_node_t;


typedef struct {

    ngx_event_t *event;
    void *dummy;
    ngx_log_t *log;


    ngx_int_t ident;


    ngx_array_t udp_connections;
    ngx_uint_t last_connection;

    ngx_rbtree_t name_rbtree;
    ngx_rbtree_node_t name_sentinel;

    ngx_rbtree_t addr_rbtree;
    ngx_rbtree_node_t addr_sentinel;

    ngx_queue_t name_resend_queue;
    ngx_queue_t addr_resend_queue;

    ngx_queue_t name_expire_queue;
    ngx_queue_t addr_expire_queue;

    time_t resend_timeout;
    time_t expire;
    time_t valid;

    ngx_uint_t log_level;
} ngx_resolver_t;


struct ngx_resolver_ctx_s {
    ngx_resolver_ctx_t *next;
    ngx_resolver_t *resolver;
    ngx_udp_connection_t *udp_connection;


    ngx_int_t ident;

    ngx_int_t state;
    ngx_int_t type;
    ngx_str_t name;

    ngx_uint_t naddrs;
    in_addr_t *addrs;
    in_addr_t addr;

    ngx_resolver_handler_pt handler;
    void *data;
    ngx_msec_t timeout;

    ngx_uint_t quick;
    ngx_uint_t recursion;
    ngx_event_t *event;
};


ngx_resolver_t *ngx_resolver_create(ngx_conf_t *cf, ngx_str_t *names,
    ngx_uint_t n);
ngx_resolver_ctx_t *ngx_resolve_start(ngx_resolver_t *r,
    ngx_resolver_ctx_t *temp);
ngx_int_t ngx_resolve_name(ngx_resolver_ctx_t *ctx);
void ngx_resolve_name_done(ngx_resolver_ctx_t *ctx);
ngx_int_t ngx_resolve_addr(ngx_resolver_ctx_t *ctx);
void ngx_resolve_addr_done(ngx_resolver_ctx_t *ctx);
char *ngx_resolver_strerror(ngx_int_t err);
# 78 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_open_file_cache.h" 1
# 19 "src/core/ngx_open_file_cache.h"
typedef struct {
    ngx_fd_t fd;
    ngx_file_uniq_t uniq;
    time_t mtime;
    off_t size;
    off_t fs_size;
    off_t directio;
    size_t read_ahead;

    ngx_err_t err;
    char *failed;

    time_t valid;

    ngx_uint_t min_uses;


    size_t disable_symlinks_from;
    unsigned disable_symlinks:2;


    unsigned test_dir:1;
    unsigned test_only:1;
    unsigned log:1;
    unsigned errors:1;
    unsigned events:1;

    unsigned is_dir:1;
    unsigned is_file:1;
    unsigned is_link:1;
    unsigned is_exec:1;
    unsigned is_directio:1;
} ngx_open_file_info_t;


typedef struct ngx_cached_open_file_s ngx_cached_open_file_t;

struct ngx_cached_open_file_s {
    ngx_rbtree_node_t node;
    ngx_queue_t queue;

    u_char *name;
    time_t created;
    time_t accessed;

    ngx_fd_t fd;
    ngx_file_uniq_t uniq;
    time_t mtime;
    off_t size;
    ngx_err_t err;

    uint32_t uses;


    size_t disable_symlinks_from;
    unsigned disable_symlinks:2;


    unsigned count:24;
    unsigned close:1;
    unsigned use_event:1;

    unsigned is_dir:1;
    unsigned is_file:1;
    unsigned is_link:1;
    unsigned is_exec:1;
    unsigned is_directio:1;

    ngx_event_t *event;
};


typedef struct {
    ngx_rbtree_t rbtree;
    ngx_rbtree_node_t sentinel;
    ngx_queue_t expire_queue;

    ngx_uint_t current;
    ngx_uint_t max;
    time_t inactive;
} ngx_open_file_cache_t;


typedef struct {
    ngx_open_file_cache_t *cache;
    ngx_cached_open_file_t *file;
    ngx_uint_t min_uses;
    ngx_log_t *log;
} ngx_open_file_cache_cleanup_t;


typedef struct {


    void *data;
    ngx_event_t *read;
    ngx_event_t *write;
    ngx_fd_t fd;

    ngx_cached_open_file_t *file;
    ngx_open_file_cache_t *cache;
} ngx_open_file_cache_event_t;


ngx_open_file_cache_t *ngx_open_file_cache_init(ngx_pool_t *pool,
    ngx_uint_t max, time_t inactive);
ngx_int_t ngx_open_cached_file(ngx_open_file_cache_t *cache, ngx_str_t *name,
    ngx_open_file_info_t *of, ngx_pool_t *pool);
# 79 "src/core/ngx_core.h" 2
# 1 "src/os/unix/ngx_os.h" 1
# 19 "src/os/unix/ngx_os.h"
typedef ssize_t (*ngx_recv_pt)(ngx_connection_t *c, u_char *buf, size_t size);
typedef ssize_t (*ngx_recv_chain_pt)(ngx_connection_t *c, ngx_chain_t *in);
typedef ssize_t (*ngx_send_pt)(ngx_connection_t *c, u_char *buf, size_t size);
typedef ngx_chain_t *(*ngx_send_chain_pt)(ngx_connection_t *c, ngx_chain_t *in,
    off_t limit);

typedef struct {
    ngx_recv_pt recv;
    ngx_recv_chain_pt recv_chain;
    ngx_recv_pt udp_recv;
    ngx_send_pt send;
    ngx_send_chain_pt send_chain;
    ngx_uint_t flags;
} ngx_os_io_t;


ngx_int_t ngx_os_init(ngx_log_t *log);
void ngx_os_status(ngx_log_t *log);
ngx_int_t ngx_os_specific_init(ngx_log_t *log);
void ngx_os_specific_status(ngx_log_t *log);
ngx_int_t ngx_daemon(ngx_log_t *log);
ngx_int_t ngx_os_signal_process(ngx_cycle_t *cycle, char *sig, ngx_int_t pid);


ssize_t ngx_unix_recv(ngx_connection_t *c, u_char *buf, size_t size);
ssize_t ngx_readv_chain(ngx_connection_t *c, ngx_chain_t *entry);
ssize_t ngx_udp_unix_recv(ngx_connection_t *c, u_char *buf, size_t size);
ssize_t ngx_unix_send(ngx_connection_t *c, u_char *buf, size_t size);
ngx_chain_t *ngx_writev_chain(ngx_connection_t *c, ngx_chain_t *in,
    off_t limit);
# 59 "src/os/unix/ngx_os.h"
extern ngx_os_io_t ngx_os_io;
extern ngx_int_t ngx_ncpu;
extern ngx_int_t ngx_max_sockets;
extern ngx_uint_t ngx_inherited_nonblocking;
extern ngx_uint_t ngx_tcp_nodelay_and_tcp_nopush;







# 1 "src/os/unix/ngx_linux.h" 1
# 12 "src/os/unix/ngx_linux.h"
ngx_chain_t *ngx_linux_sendfile_chain(ngx_connection_t *c, ngx_chain_t *in,
    off_t limit);

extern int ngx_linux_rtsig_max;
# 72 "src/os/unix/ngx_os.h" 2
# 80 "src/core/ngx_core.h" 2
# 1 "src/core/ngx_connection.h" 1
# 16 "src/core/ngx_connection.h"
typedef struct ngx_listening_s ngx_listening_t;

struct ngx_listening_s {
    ngx_socket_t fd;

    struct sockaddr *sockaddr;
    socklen_t socklen;
    size_t addr_text_max_len;
    ngx_str_t addr_text;

    int type;

    int backlog;
    int rcvbuf;
    int sndbuf;

    int keepidle;
    int keepintvl;
    int keepcnt;



    ngx_connection_handler_pt handler;

    void *servers;

    ngx_log_t log;
    ngx_log_t *logp;

    size_t pool_size;

    size_t post_accept_buffer_size;

    ngx_msec_t post_accept_timeout;

    ngx_listening_t *previous;
    ngx_connection_t *connection;

    unsigned open:1;
    unsigned remain:1;
    unsigned ignore:1;

    unsigned bound:1;
    unsigned inherited:1;
    unsigned nonblocking_accept:1;
    unsigned listen:1;
    unsigned nonblocking:1;
    unsigned shared:1;
    unsigned addr_ntop:1;




    unsigned keepalive:2;


    unsigned deferred_accept:1;
    unsigned delete_deferred:1;
    unsigned add_deferred:1;
# 83 "src/core/ngx_connection.h"
};


typedef enum {
     NGX_ERROR_ALERT = 0,
     NGX_ERROR_ERR,
     NGX_ERROR_INFO,
     NGX_ERROR_IGNORE_ECONNRESET,
     NGX_ERROR_IGNORE_EINVAL
} ngx_connection_log_error_e;


typedef enum {
     NGX_TCP_NODELAY_UNSET = 0,
     NGX_TCP_NODELAY_SET,
     NGX_TCP_NODELAY_DISABLED
} ngx_connection_tcp_nodelay_e;


typedef enum {
     NGX_TCP_NOPUSH_UNSET = 0,
     NGX_TCP_NOPUSH_SET,
     NGX_TCP_NOPUSH_DISABLED
} ngx_connection_tcp_nopush_e;






struct ngx_connection_s {
    void *data;
    ngx_event_t *read;
    ngx_event_t *write;

    ngx_socket_t fd;

    ngx_recv_pt recv;
    ngx_send_pt send;
    ngx_recv_chain_pt recv_chain;
    ngx_send_chain_pt send_chain;

    ngx_listening_t *listening;

    off_t sent;

    ngx_log_t *log;

    ngx_pool_t *pool;

    struct sockaddr *sockaddr;
    socklen_t socklen;
    ngx_str_t addr_text;





    struct sockaddr *local_sockaddr;

    ngx_buf_t *buffer;

    ngx_queue_t queue;

    ngx_atomic_uint_t number;

    ngx_uint_t requests;

    unsigned buffered:8;

    unsigned log_error:3;

    unsigned single_connection:1;
    unsigned unexpected_eof:1;
    unsigned timedout:1;
    unsigned error:1;
    unsigned destroyed:1;

    unsigned idle:1;
    unsigned reusable:1;
    unsigned close:1;

    unsigned sendfile:1;
    unsigned sndlowat:1;
    unsigned tcp_nodelay:2;
    unsigned tcp_nopush:2;
# 182 "src/core/ngx_connection.h"
};


ngx_listening_t *ngx_create_listening(ngx_conf_t *cf, void *sockaddr,
    socklen_t socklen);
ngx_int_t ngx_set_inherited_sockets(ngx_cycle_t *cycle);
ngx_int_t ngx_open_listening_sockets(ngx_cycle_t *cycle);
void ngx_configure_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_connection(ngx_connection_t *c);
ngx_int_t ngx_connection_local_sockaddr(ngx_connection_t *c, ngx_str_t *s,
    ngx_uint_t port);
ngx_int_t ngx_connection_error(ngx_connection_t *c, ngx_err_t err, char *text);

ngx_connection_t *ngx_get_connection(ngx_socket_t s, ngx_log_t *log);
void ngx_free_connection(ngx_connection_t *c);

void ngx_reusable_connection(ngx_connection_t *c, ngx_uint_t reusable);
# 81 "src/core/ngx_core.h" 2
# 92 "src/core/ngx_core.h"
void ngx_cpuinfo(void);
# 10 "src/http/ngx_http_upstream.c" 2
# 1 "src/http/ngx_http.h" 1
# 16 "src/http/ngx_http.h"
typedef struct ngx_http_request_s ngx_http_request_t;
typedef struct ngx_http_upstream_s ngx_http_upstream_t;
typedef struct ngx_http_cache_s ngx_http_cache_t;
typedef struct ngx_http_file_cache_s ngx_http_file_cache_t;
typedef struct ngx_http_log_ctx_s ngx_http_log_ctx_t;

typedef ngx_int_t (*ngx_http_header_handler_pt)(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
typedef u_char *(*ngx_http_log_handler_pt)(ngx_http_request_t *r,
    ngx_http_request_t *sr, u_char *buf, size_t len);


# 1 "src/http/ngx_http_variables.h" 1
# 14 "src/http/ngx_http_variables.h"
# 1 "src/http/ngx_http.h" 1
# 15 "src/http/ngx_http_variables.h" 2


typedef ngx_variable_value_t ngx_http_variable_value_t;



typedef struct ngx_http_variable_s ngx_http_variable_t;

typedef void (*ngx_http_set_variable_pt) (ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
typedef ngx_int_t (*ngx_http_get_variable_pt) (ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
# 35 "src/http/ngx_http_variables.h"
struct ngx_http_variable_s {
    ngx_str_t name;
    ngx_http_set_variable_pt set_handler;
    ngx_http_get_variable_pt get_handler;
    uintptr_t data;
    ngx_uint_t flags;
    ngx_uint_t index;
};


ngx_http_variable_t *ngx_http_add_variable(ngx_conf_t *cf, ngx_str_t *name,
    ngx_uint_t flags);
ngx_int_t ngx_http_get_variable_index(ngx_conf_t *cf, ngx_str_t *name);
ngx_http_variable_value_t *ngx_http_get_indexed_variable(ngx_http_request_t *r,
    ngx_uint_t index);
ngx_http_variable_value_t *ngx_http_get_flushed_variable(ngx_http_request_t *r,
    ngx_uint_t index);

ngx_http_variable_value_t *ngx_http_get_variable(ngx_http_request_t *r,
    ngx_str_t *name, ngx_uint_t key);

ngx_int_t ngx_http_variable_unknown_header(ngx_http_variable_value_t *v,
    ngx_str_t *var, ngx_list_part_t *part, size_t prefix);







typedef struct {
    ngx_uint_t capture;
    ngx_int_t index;
} ngx_http_regex_variable_t;


typedef struct {
    ngx_regex_t *regex;
    ngx_uint_t ncaptures;
    ngx_http_regex_variable_t *variables;
    ngx_uint_t nvariables;
    ngx_str_t name;
} ngx_http_regex_t;


typedef struct {
    ngx_http_regex_t *regex;
    void *value;
} ngx_http_map_regex_t;


ngx_http_regex_t *ngx_http_regex_compile(ngx_conf_t *cf,
    ngx_regex_compile_t *rc);
ngx_int_t ngx_http_regex_exec(ngx_http_request_t *r, ngx_http_regex_t *re,
    ngx_str_t *s);




typedef struct {
    ngx_hash_combined_t hash;

    ngx_http_map_regex_t *regex;
    ngx_uint_t nregex;

} ngx_http_map_t;


void *ngx_http_map_find(ngx_http_request_t *r, ngx_http_map_t *map,
    ngx_str_t *match);


ngx_int_t ngx_http_variables_add_core_vars(ngx_conf_t *cf);
ngx_int_t ngx_http_variables_init_vars(ngx_conf_t *cf);


extern ngx_http_variable_value_t ngx_http_variable_null_value;
extern ngx_http_variable_value_t ngx_http_variable_true_value;
# 29 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_request.h" 1
# 147 "src/http/ngx_http_request.h"
typedef enum {
    NGX_HTTP_INITING_REQUEST_STATE = 0,
    NGX_HTTP_READING_REQUEST_STATE,
    NGX_HTTP_PROCESS_REQUEST_STATE,

    NGX_HTTP_CONNECT_UPSTREAM_STATE,
    NGX_HTTP_WRITING_UPSTREAM_STATE,
    NGX_HTTP_READING_UPSTREAM_STATE,

    NGX_HTTP_WRITING_REQUEST_STATE,
    NGX_HTTP_LINGERING_CLOSE_STATE,
    NGX_HTTP_KEEPALIVE_STATE
} ngx_http_state_e;


typedef struct {
    ngx_str_t name;
    ngx_uint_t offset;
    ngx_http_header_handler_pt handler;
} ngx_http_header_t;


typedef struct {
    ngx_str_t name;
    ngx_uint_t offset;
} ngx_http_header_out_t;


typedef struct {
    ngx_list_t headers;

    ngx_table_elt_t *host;
    ngx_table_elt_t *connection;
    ngx_table_elt_t *if_modified_since;
    ngx_table_elt_t *if_unmodified_since;
    ngx_table_elt_t *user_agent;
    ngx_table_elt_t *referer;
    ngx_table_elt_t *content_length;
    ngx_table_elt_t *content_type;

    ngx_table_elt_t *range;
    ngx_table_elt_t *if_range;

    ngx_table_elt_t *transfer_encoding;
    ngx_table_elt_t *expect;


    ngx_table_elt_t *accept_encoding;
    ngx_table_elt_t *via;


    ngx_table_elt_t *authorization;

    ngx_table_elt_t *keep_alive;


    ngx_table_elt_t *x_forwarded_for;
# 222 "src/http/ngx_http_request.h"
    ngx_str_t user;
    ngx_str_t passwd;

    ngx_array_t cookies;

    ngx_str_t server;
    off_t content_length_n;
    time_t keep_alive_n;

    unsigned connection_type:2;
    unsigned msie:1;
    unsigned msie6:1;
    unsigned opera:1;
    unsigned gecko:1;
    unsigned chrome:1;
    unsigned safari:1;
    unsigned konqueror:1;
} ngx_http_headers_in_t;


typedef struct {
    ngx_list_t headers;

    ngx_uint_t status;
    ngx_str_t status_line;

    ngx_table_elt_t *server;
    ngx_table_elt_t *date;
    ngx_table_elt_t *content_length;
    ngx_table_elt_t *content_encoding;
    ngx_table_elt_t *location;
    ngx_table_elt_t *refresh;
    ngx_table_elt_t *last_modified;
    ngx_table_elt_t *content_range;
    ngx_table_elt_t *accept_ranges;
    ngx_table_elt_t *www_authenticate;
    ngx_table_elt_t *expires;
    ngx_table_elt_t *etag;

    ngx_str_t *override_charset;

    size_t content_type_len;
    ngx_str_t content_type;
    ngx_str_t charset;
    u_char *content_type_lowcase;
    ngx_uint_t content_type_hash;

    ngx_array_t cache_control;

    off_t content_length_n;
    time_t date_time;
    time_t last_modified_time;
} ngx_http_headers_out_t;


typedef void (*ngx_http_client_body_handler_pt)(ngx_http_request_t *r);

typedef struct {
    ngx_temp_file_t *temp_file;
    ngx_chain_t *bufs;
    ngx_buf_t *buf;
    off_t rest;
    ngx_chain_t *to_write;
    ngx_http_client_body_handler_pt post_handler;
} ngx_http_request_body_t;


typedef struct {
    ngx_http_request_t *request;

    ngx_buf_t **busy;
    ngx_int_t nbusy;

    ngx_buf_t **free;
    ngx_int_t nfree;

    ngx_uint_t pipeline;
} ngx_http_connection_t;


typedef struct ngx_http_server_name_s ngx_http_server_name_t;


typedef struct {
     ngx_hash_combined_t names;

     ngx_uint_t nregex;
     ngx_http_server_name_t *regex;
} ngx_http_virtual_names_t;


typedef void (*ngx_http_cleanup_pt)(void *data);

typedef struct ngx_http_cleanup_s ngx_http_cleanup_t;

struct ngx_http_cleanup_s {
    ngx_http_cleanup_pt handler;
    void *data;
    ngx_http_cleanup_t *next;
};


typedef ngx_int_t (*ngx_http_post_subrequest_pt)(ngx_http_request_t *r,
    void *data, ngx_int_t rc);

typedef struct {
    ngx_http_post_subrequest_pt handler;
    void *data;
} ngx_http_post_subrequest_t;


typedef struct ngx_http_postponed_request_s ngx_http_postponed_request_t;

struct ngx_http_postponed_request_s {
    ngx_http_request_t *request;
    ngx_chain_t *out;
    ngx_http_postponed_request_t *next;
};


typedef struct ngx_http_posted_request_s ngx_http_posted_request_t;

struct ngx_http_posted_request_s {
    ngx_http_request_t *request;
    ngx_http_posted_request_t *next;
};


typedef ngx_int_t (*ngx_http_handler_pt)(ngx_http_request_t *r);
typedef void (*ngx_http_event_handler_pt)(ngx_http_request_t *r);


struct ngx_http_request_s {
    uint32_t signature;

    ngx_connection_t *connection;

    void **ctx;
    void **main_conf;
    void **srv_conf;
    void **loc_conf;

    ngx_http_event_handler_pt read_event_handler;
    ngx_http_event_handler_pt write_event_handler;


    ngx_http_cache_t *cache;


    ngx_http_upstream_t *upstream;
    ngx_array_t *upstream_states;


    ngx_pool_t *pool;
    ngx_buf_t *header_in;

    ngx_http_headers_in_t headers_in;
    ngx_http_headers_out_t headers_out;

    ngx_http_request_body_t *request_body;

    time_t lingering_time;
    time_t start_sec;
    ngx_msec_t start_msec;

    ngx_uint_t method;
    ngx_uint_t http_version;

    ngx_str_t request_line;
    ngx_str_t uri;
    ngx_str_t args;
    ngx_str_t exten;
    ngx_str_t unparsed_uri;

    ngx_str_t method_name;
    ngx_str_t http_protocol;

    ngx_chain_t *out;
    ngx_http_request_t *main;
    ngx_http_request_t *parent;
    ngx_http_postponed_request_t *postponed;
    ngx_http_post_subrequest_t *post_subrequest;
    ngx_http_posted_request_t *posted_requests;

    ngx_http_virtual_names_t *virtual_names;

    ngx_int_t phase_handler;
    ngx_http_handler_pt content_handler;
    ngx_uint_t access_code;

    ngx_http_variable_value_t *variables;


    ngx_uint_t ncaptures;
    int *captures;
    u_char *captures_data;


    size_t limit_rate;


    size_t header_size;

    off_t request_length;

    ngx_uint_t err_status;

    ngx_http_connection_t *http_connection;

    ngx_http_log_handler_pt log_handler;

    ngx_http_cleanup_t *cleanup;

    unsigned subrequests:8;
    unsigned count:8;
    unsigned blocked:8;

    unsigned aio:1;

    unsigned http_state:4;


    unsigned complex_uri:1;


    unsigned quoted_uri:1;


    unsigned plus_in_uri:1;


    unsigned space_in_uri:1;

    unsigned invalid_header:1;

    unsigned add_uri_to_alias:1;
    unsigned valid_location:1;
    unsigned valid_unparsed_uri:1;
    unsigned uri_changed:1;
    unsigned uri_changes:4;

    unsigned request_body_in_single_buf:1;
    unsigned request_body_in_file_only:1;
    unsigned request_body_in_persistent_file:1;
    unsigned request_body_in_clean_file:1;
    unsigned request_body_file_group_access:1;
    unsigned request_body_file_log_level:3;

    unsigned subrequest_in_memory:1;
    unsigned waited:1;


    unsigned cached:1;



    unsigned gzip_tested:1;
    unsigned gzip_ok:1;
    unsigned gzip_vary:1;


    unsigned proxy:1;
    unsigned bypass_cache:1;
    unsigned no_cache:1;






    unsigned limit_conn_set:1;
    unsigned limit_req_set:1;





    unsigned pipeline:1;
    unsigned plain_http:1;
    unsigned chunked:1;
    unsigned header_only:1;
    unsigned keepalive:1;
    unsigned lingering_close:1;
    unsigned discard_body:1;
    unsigned internal:1;
    unsigned error_page:1;
    unsigned ignore_content_encoding:1;
    unsigned filter_finalize:1;
    unsigned post_action:1;
    unsigned request_complete:1;
    unsigned request_output:1;
    unsigned header_sent:1;
    unsigned expect_tested:1;
    unsigned root_tested:1;
    unsigned done:1;
    unsigned logged:1;

    unsigned buffered:4;

    unsigned main_filter_need_in_memory:1;
    unsigned filter_need_in_memory:1;
    unsigned filter_need_temporary:1;
    unsigned allow_ranges:1;
# 533 "src/http/ngx_http_request.h"
    ngx_uint_t state;

    ngx_uint_t header_hash;
    ngx_uint_t lowcase_index;
    u_char lowcase_header[32];

    u_char *header_name_start;
    u_char *header_name_end;
    u_char *header_start;
    u_char *header_end;






    u_char *uri_start;
    u_char *uri_end;
    u_char *uri_ext;
    u_char *args_start;
    u_char *request_start;
    u_char *request_end;
    u_char *method_end;
    u_char *schema_start;
    u_char *schema_end;
    u_char *host_start;
    u_char *host_end;
    u_char *port_start;
    u_char *port_end;

    unsigned http_minor:16;
    unsigned http_major:16;
};


typedef struct {
    ngx_http_posted_request_t terminal_posted_request;



} ngx_http_ephemeral_t;


extern ngx_http_header_t ngx_http_headers_in[];
extern ngx_http_header_out_t ngx_http_headers_out[];
# 30 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_upstream.h" 1
# 14 "src/http/ngx_http_upstream.h"
# 1 "src/event/ngx_event.h" 1
# 30 "src/event/ngx_event.h"
typedef struct {
    ngx_uint_t lock;

    ngx_event_t *events;
    ngx_event_t *last;
} ngx_event_mutex_t;


struct ngx_event_s {
    void *data;

    unsigned write:1;

    unsigned accept:1;


    unsigned instance:1;





    unsigned active:1;

    unsigned disabled:1;


    unsigned ready:1;

    unsigned oneshot:1;


    unsigned complete:1;

    unsigned eof:1;
    unsigned error:1;

    unsigned timedout:1;
    unsigned timer_set:1;

    unsigned delayed:1;

    unsigned read_discarded:1;

    unsigned unexpected_eof:1;

    unsigned deferred_accept:1;


    unsigned pending_eof:1;


    unsigned posted_ready:1;
# 114 "src/event/ngx_event.h"
    unsigned available:1;


    ngx_event_handler_pt handler;
# 130 "src/event/ngx_event.h"
    ngx_uint_t index;

    ngx_log_t *log;

    ngx_rbtree_node_t timer;

    unsigned closed:1;


    unsigned channel:1;
    unsigned resolver:1;
# 167 "src/event/ngx_event.h"
    ngx_event_t *next;
    ngx_event_t **prev;
# 190 "src/event/ngx_event.h"
};
# 224 "src/event/ngx_event.h"
typedef struct {
    ngx_int_t (*add)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);
    ngx_int_t (*del)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    ngx_int_t (*enable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);
    ngx_int_t (*disable)(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags);

    ngx_int_t (*add_conn)(ngx_connection_t *c);
    ngx_int_t (*del_conn)(ngx_connection_t *c, ngx_uint_t flags);

    ngx_int_t (*process_changes)(ngx_cycle_t *cycle, ngx_uint_t nowait);
    ngx_int_t (*process_events)(ngx_cycle_t *cycle, ngx_msec_t timer,
                   ngx_uint_t flags);

    ngx_int_t (*init)(ngx_cycle_t *cycle, ngx_msec_t timer);
    void (*done)(ngx_cycle_t *cycle);
} ngx_event_actions_t;


extern ngx_event_actions_t ngx_event_actions;
# 455 "src/event/ngx_event.h"
extern ngx_os_io_t ngx_io;
# 468 "src/event/ngx_event.h"
typedef struct {
    ngx_uint_t connections;
    ngx_uint_t use;

    ngx_flag_t multi_accept;
    ngx_flag_t accept_mutex;

    ngx_msec_t accept_mutex_delay;

    u_char *name;




} ngx_event_conf_t;


typedef struct {
    ngx_str_t *name;

    void *(*create_conf)(ngx_cycle_t *cycle);
    char *(*init_conf)(ngx_cycle_t *cycle, void *conf);

    ngx_event_actions_t actions;
} ngx_event_module_t;


extern ngx_atomic_t *ngx_connection_counter;

extern ngx_atomic_t *ngx_accept_mutex_ptr;
extern ngx_shmtx_t ngx_accept_mutex;
extern ngx_uint_t ngx_use_accept_mutex;
extern ngx_uint_t ngx_accept_events;
extern ngx_uint_t ngx_accept_mutex_held;
extern ngx_msec_t ngx_accept_mutex_delay;
extern ngx_int_t ngx_accept_disabled;
# 523 "src/event/ngx_event.h"
extern sig_atomic_t ngx_event_timer_alarm;
extern ngx_uint_t ngx_event_flags;
extern ngx_module_t ngx_events_module;
extern ngx_module_t ngx_event_core_module;







void ngx_event_accept(ngx_event_t *ev);
ngx_int_t ngx_trylock_accept_mutex(ngx_cycle_t *cycle);
u_char *ngx_accept_log_error(ngx_log_t *log, u_char *buf, size_t len);


void ngx_process_events_and_timers(ngx_cycle_t *cycle);
ngx_int_t ngx_handle_read_event(ngx_event_t *rev, ngx_uint_t flags);
ngx_int_t ngx_handle_write_event(ngx_event_t *wev, size_t lowat);
# 551 "src/event/ngx_event.h"
ngx_int_t ngx_send_lowat(ngx_connection_t *c, size_t lowat);






# 1 "src/event/ngx_event_timer.h" 1
# 14 "src/event/ngx_event_timer.h"
# 1 "src/event/ngx_event.h" 1
# 15 "src/event/ngx_event_timer.h" 2







ngx_int_t ngx_event_timer_init(ngx_log_t *log);
ngx_msec_t ngx_event_find_timer(void);
void ngx_event_expire_timers(void);







extern ngx_rbtree_t ngx_event_timer_rbtree;


static inline void
ngx_event_del_timer(ngx_event_t *ev)
{
    ;



    ;

    ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

    ;







    ev->timer_set = 0;
}


static inline void
ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer)
{
    ngx_msec_t key;
    ngx_msec_int_t diff;

    key = ngx_current_msec + timer;

    if (ev->timer_set) {







        diff = (ngx_msec_int_t) (key - ev->timer.key);

        if ((((diff) >= 0) ? (diff) : - (diff)) < 300) {
            ;


            return;
        }

        ngx_event_del_timer(ev);
    }

    ev->timer.key = key;

    ;



    ;

    ngx_rbtree_insert(&ngx_event_timer_rbtree, &ev->timer);

    ;

    ev->timer_set = 1;
}
# 559 "src/event/ngx_event.h" 2
# 1 "src/event/ngx_event_posted.h" 1
# 62 "src/event/ngx_event_posted.h"
void ngx_event_process_posted(ngx_cycle_t *cycle,
    ngx_event_t **posted);
void ngx_wakeup_worker_thread(ngx_cycle_t *cycle);






extern ngx_event_t *ngx_posted_accept_events;
extern ngx_event_t *ngx_posted_events;
# 560 "src/event/ngx_event.h" 2
# 1 "src/event/ngx_event_busy_lock.h" 1
# 16 "src/event/ngx_event_busy_lock.h"
typedef struct ngx_event_busy_lock_ctx_s ngx_event_busy_lock_ctx_t;

struct ngx_event_busy_lock_ctx_s {
    ngx_event_t *event;
    ngx_event_handler_pt handler;
    void *data;
    ngx_msec_t timer;

    unsigned locked:1;
    unsigned waiting:1;
    unsigned cache_updated:1;

    char *md5;
    ngx_int_t slot;

    ngx_event_busy_lock_ctx_t *next;
};


typedef struct {
    u_char *md5_mask;
    char *md5;
    ngx_uint_t cacheable;

    ngx_uint_t busy;
    ngx_uint_t max_busy;

    ngx_uint_t waiting;
    ngx_uint_t max_waiting;

    ngx_event_busy_lock_ctx_t *events;
    ngx_event_busy_lock_ctx_t *last;




} ngx_event_busy_lock_t;


ngx_int_t ngx_event_busy_lock(ngx_event_busy_lock_t *bl,
    ngx_event_busy_lock_ctx_t *ctx);
ngx_int_t ngx_event_busy_lock_cacheable(ngx_event_busy_lock_t *bl,
    ngx_event_busy_lock_ctx_t *ctx);
void ngx_event_busy_unlock(ngx_event_busy_lock_t *bl,
    ngx_event_busy_lock_ctx_t *ctx);
void ngx_event_busy_lock_cancel(ngx_event_busy_lock_t *bl,
    ngx_event_busy_lock_ctx_t *ctx);
# 561 "src/event/ngx_event.h" 2
# 15 "src/http/ngx_http_upstream.h" 2
# 1 "src/event/ngx_event_connect.h" 1
# 22 "src/event/ngx_event_connect.h"
typedef struct ngx_peer_connection_s ngx_peer_connection_t;

typedef ngx_int_t (*ngx_event_get_peer_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_free_peer_pt)(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state);
# 37 "src/event/ngx_event_connect.h"
struct ngx_peer_connection_s {
    ngx_connection_t *connection;

    struct sockaddr *sockaddr;
    socklen_t socklen;
    ngx_str_t *name;

    ngx_uint_t tries;

    ngx_event_get_peer_pt get;
    ngx_event_free_peer_pt free;
    void *data;
# 59 "src/event/ngx_event_connect.h"
    ngx_addr_t *local;

    int rcvbuf;

    ngx_log_t *log;

    unsigned cached:1;


    unsigned log_error:2;
};


ngx_int_t ngx_event_connect_peer(ngx_peer_connection_t *pc);
ngx_int_t ngx_event_get_peer(ngx_peer_connection_t *pc, void *data);
# 16 "src/http/ngx_http_upstream.h" 2
# 1 "src/event/ngx_event_pipe.h" 1
# 17 "src/event/ngx_event_pipe.h"
typedef struct ngx_event_pipe_s ngx_event_pipe_t;

typedef ngx_int_t (*ngx_event_pipe_input_filter_pt)(ngx_event_pipe_t *p,
                                                    ngx_buf_t *buf);
typedef ngx_int_t (*ngx_event_pipe_output_filter_pt)(void *data,
                                                     ngx_chain_t *chain);


struct ngx_event_pipe_s {
    ngx_connection_t *upstream;
    ngx_connection_t *downstream;

    ngx_chain_t *free_raw_bufs;
    ngx_chain_t *in;
    ngx_chain_t **last_in;

    ngx_chain_t *out;
    ngx_chain_t *free;
    ngx_chain_t *busy;






    ngx_event_pipe_input_filter_pt input_filter;
    void *input_ctx;

    ngx_event_pipe_output_filter_pt output_filter;
    void *output_ctx;

    unsigned read:1;
    unsigned cacheable:1;
    unsigned single_buf:1;
    unsigned free_bufs:1;
    unsigned upstream_done:1;
    unsigned upstream_error:1;
    unsigned upstream_eof:1;
    unsigned upstream_blocked:1;
    unsigned downstream_done:1;
    unsigned downstream_error:1;
    unsigned cyclic_temp_file:1;

    ngx_int_t allocated;
    ngx_bufs_t bufs;
    ngx_buf_tag_t tag;

    ssize_t busy_size;

    off_t read_length;
    off_t length;

    off_t max_temp_file_size;
    ssize_t temp_file_write_size;

    ngx_msec_t read_timeout;
    ngx_msec_t send_timeout;
    ssize_t send_lowat;

    ngx_pool_t *pool;
    ngx_log_t *log;

    ngx_chain_t *preread_bufs;
    size_t preread_size;
    ngx_buf_t *buf_to_file;

    ngx_temp_file_t *temp_file;

               int num;
};


ngx_int_t ngx_event_pipe(ngx_event_pipe_t *p, ngx_int_t do_write);
ngx_int_t ngx_event_pipe_copy_input_filter(ngx_event_pipe_t *p, ngx_buf_t *buf);
ngx_int_t ngx_event_pipe_add_free_buf(ngx_event_pipe_t *p, ngx_buf_t *b);
# 17 "src/http/ngx_http_upstream.h" 2
# 53 "src/http/ngx_http_upstream.h"
typedef struct {
    ngx_msec_t bl_time;
    ngx_uint_t bl_state;

    ngx_uint_t status;
    time_t response_sec;
    ngx_uint_t response_msec;
    off_t response_length;

    ngx_str_t *peer;
} ngx_http_upstream_state_t;


typedef struct {
    ngx_hash_t headers_in_hash;
    ngx_array_t upstreams;

} ngx_http_upstream_main_conf_t;

typedef struct ngx_http_upstream_srv_conf_s ngx_http_upstream_srv_conf_t;

typedef ngx_int_t (*ngx_http_upstream_init_pt)(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
typedef ngx_int_t (*ngx_http_upstream_init_peer_pt)(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);


typedef struct {
    ngx_http_upstream_init_pt init_upstream;
    ngx_http_upstream_init_peer_pt init;
    void *data;
} ngx_http_upstream_peer_t;


typedef struct {
    ngx_addr_t *addrs;
    ngx_uint_t naddrs;
    ngx_uint_t weight;
    ngx_uint_t max_fails;
    time_t fail_timeout;

    unsigned down:1;
    unsigned backup:1;
} ngx_http_upstream_server_t;
# 107 "src/http/ngx_http_upstream.h"
struct ngx_http_upstream_srv_conf_s {
    ngx_http_upstream_peer_t peer;
    void **srv_conf;

    ngx_array_t *servers;

    ngx_uint_t flags;
    ngx_str_t host;
    u_char *file_name;
    ngx_uint_t line;
    in_port_t port;
    in_port_t default_port;
};


typedef struct {
    ngx_http_upstream_srv_conf_t *upstream;

    ngx_msec_t connect_timeout;
    ngx_msec_t send_timeout;
    ngx_msec_t read_timeout;
    ngx_msec_t timeout;

    size_t send_lowat;
    size_t buffer_size;

    size_t busy_buffers_size;
    size_t max_temp_file_size;
    size_t temp_file_write_size;

    size_t busy_buffers_size_conf;
    size_t max_temp_file_size_conf;
    size_t temp_file_write_size_conf;

    ngx_bufs_t bufs;

    ngx_uint_t ignore_headers;
    ngx_uint_t next_upstream;
    ngx_uint_t store_access;
    ngx_flag_t buffering;
    ngx_flag_t pass_request_headers;
    ngx_flag_t pass_request_body;

    ngx_flag_t ignore_client_abort;
    ngx_flag_t intercept_errors;
    ngx_flag_t cyclic_temp_file;

    ngx_path_t *temp_path;

    ngx_hash_t hide_headers_hash;
    ngx_array_t *hide_headers;
    ngx_array_t *pass_headers;

    ngx_addr_t *local;


    ngx_shm_zone_t *cache;

    ngx_uint_t cache_min_uses;
    ngx_uint_t cache_use_stale;
    ngx_uint_t cache_methods;

    ngx_flag_t cache_lock;
    ngx_msec_t cache_lock_timeout;

    ngx_array_t *cache_valid;
    ngx_array_t *cache_bypass;
    ngx_array_t *no_cache;


    ngx_array_t *store_lengths;
    ngx_array_t *store_values;

    signed store:2;
    unsigned intercept_404:1;
    unsigned change_buffering:1;






    ngx_str_t module;
} ngx_http_upstream_conf_t;


typedef struct {
    ngx_str_t name;
    ngx_http_header_handler_pt handler;
    ngx_uint_t offset;
    ngx_http_header_handler_pt copy_handler;
    ngx_uint_t conf;
    ngx_uint_t redirect;
} ngx_http_upstream_header_t;


typedef struct {
    ngx_list_t headers;

    ngx_uint_t status_n;
    ngx_str_t status_line;

    ngx_table_elt_t *status;
    ngx_table_elt_t *date;
    ngx_table_elt_t *server;
    ngx_table_elt_t *connection;

    ngx_table_elt_t *expires;
    ngx_table_elt_t *etag;
    ngx_table_elt_t *x_accel_expires;
    ngx_table_elt_t *x_accel_redirect;
    ngx_table_elt_t *x_accel_limit_rate;

    ngx_table_elt_t *content_type;
    ngx_table_elt_t *content_length;

    ngx_table_elt_t *last_modified;
    ngx_table_elt_t *location;
    ngx_table_elt_t *accept_ranges;
    ngx_table_elt_t *www_authenticate;
    ngx_table_elt_t *transfer_encoding;


    ngx_table_elt_t *content_encoding;


    off_t content_length_n;

    ngx_array_t cache_control;

    unsigned connection_close:1;
    unsigned chunked:1;
} ngx_http_upstream_headers_in_t;


typedef struct {
    ngx_str_t host;
    in_port_t port;
    ngx_uint_t no_port;

    ngx_uint_t naddrs;
    in_addr_t *addrs;

    struct sockaddr *sockaddr;
    socklen_t socklen;

    ngx_resolver_ctx_t *ctx;
} ngx_http_upstream_resolved_t;


typedef void (*ngx_http_upstream_handler_pt)(ngx_http_request_t *r,
    ngx_http_upstream_t *u);


struct ngx_http_upstream_s {
    ngx_http_upstream_handler_pt read_event_handler;
    ngx_http_upstream_handler_pt write_event_handler;

    ngx_peer_connection_t peer;

    ngx_event_pipe_t *pipe;

    ngx_chain_t *request_bufs;

    ngx_output_chain_ctx_t output;
    ngx_chain_writer_ctx_t writer;

    ngx_http_upstream_conf_t *conf;

    ngx_http_upstream_headers_in_t headers_in;

    ngx_http_upstream_resolved_t *resolved;

    ngx_buf_t buffer;
    off_t length;

    ngx_chain_t *out_bufs;
    ngx_chain_t *busy_bufs;
    ngx_chain_t *free_bufs;

    ngx_int_t (*input_filter_init)(void *data);
    ngx_int_t (*input_filter)(void *data, ssize_t bytes);
    void *input_filter_ctx;


    ngx_int_t (*create_key)(ngx_http_request_t *r);

    ngx_int_t (*create_request)(ngx_http_request_t *r);
    ngx_int_t (*reinit_request)(ngx_http_request_t *r);
    ngx_int_t (*process_header)(ngx_http_request_t *r);
    void (*abort_request)(ngx_http_request_t *r);
    void (*finalize_request)(ngx_http_request_t *r,
                                         ngx_int_t rc);
    ngx_int_t (*rewrite_redirect)(ngx_http_request_t *r,
                                         ngx_table_elt_t *h, size_t prefix);
    ngx_int_t (*rewrite_cookie)(ngx_http_request_t *r,
                                         ngx_table_elt_t *h);

    ngx_msec_t timeout;

    ngx_http_upstream_state_t *state;

    ngx_str_t method;
    ngx_str_t schema;
    ngx_str_t uri;

    ngx_http_cleanup_pt *cleanup;

    unsigned store:1;
    unsigned cacheable:1;
    unsigned accel:1;
    unsigned ssl:1;

    unsigned cache_status:3;


    unsigned buffering:1;
    unsigned keepalive:1;

    unsigned request_sent:1;
    unsigned header_sent:1;
};


typedef struct {
    ngx_uint_t status;
    ngx_uint_t mask;
} ngx_http_upstream_next_t;


typedef struct {
    ngx_str_t key;
    ngx_str_t value;
    ngx_uint_t skip_empty;
} ngx_http_upstream_param_t;


ngx_int_t ngx_http_upstream_header_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

ngx_int_t ngx_http_upstream_create(ngx_http_request_t *r);
void ngx_http_upstream_init(ngx_http_request_t *r);
ngx_http_upstream_srv_conf_t *ngx_http_upstream_add(ngx_conf_t *cf,
    ngx_url_t *u, ngx_uint_t flags);
char *ngx_http_upstream_bind_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_http_upstream_param_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
ngx_int_t ngx_http_upstream_hide_headers_hash(ngx_conf_t *cf,
    ngx_http_upstream_conf_t *conf, ngx_http_upstream_conf_t *prev,
    ngx_str_t *default_hide_headers, ngx_hash_init_t *hash);






extern ngx_module_t ngx_http_upstream_module;
extern ngx_conf_bitmask_t ngx_http_upstream_cache_method_mask[];
extern ngx_conf_bitmask_t ngx_http_upstream_ignore_headers_masks[];
# 31 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_upstream_round_robin.h" 1
# 17 "src/http/ngx_http_upstream_round_robin.h"
typedef struct {
    struct sockaddr *sockaddr;
    socklen_t socklen;
    ngx_str_t name;

    ngx_int_t current_weight;
    ngx_int_t effective_weight;
    ngx_int_t weight;

    ngx_uint_t fails;
    time_t accessed;
    time_t checked;

    ngx_uint_t max_fails;
    time_t fail_timeout;

    ngx_uint_t down;




} ngx_http_upstream_rr_peer_t;


typedef struct ngx_http_upstream_rr_peers_s ngx_http_upstream_rr_peers_t;

struct ngx_http_upstream_rr_peers_s {
    ngx_uint_t number;
    ngx_uint_t last_cached;


    ngx_connection_t **cached;

    ngx_uint_t total_weight;

    unsigned single:1;
    unsigned weighted:1;

    ngx_str_t *name;

    ngx_http_upstream_rr_peers_t *next;

    ngx_http_upstream_rr_peer_t peer[1];
};


typedef struct {
    ngx_http_upstream_rr_peers_t *peers;
    ngx_uint_t current;
    uintptr_t *tried;
    uintptr_t data;
} ngx_http_upstream_rr_peer_data_t;


ngx_int_t ngx_http_upstream_init_round_robin(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_init_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_create_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_resolved_t *ur);
ngx_int_t ngx_http_upstream_get_round_robin_peer(ngx_peer_connection_t *pc,
    void *data);
void ngx_http_upstream_free_round_robin_peer(ngx_peer_connection_t *pc,
    void *data, ngx_uint_t state);
# 32 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_config.h" 1
# 17 "src/http/ngx_http_config.h"
typedef struct {
    void **main_conf;
    void **srv_conf;
    void **loc_conf;
} ngx_http_conf_ctx_t;


typedef struct {
    ngx_int_t (*preconfiguration)(ngx_conf_t *cf);
    ngx_int_t (*postconfiguration)(ngx_conf_t *cf);

    void *(*create_main_conf)(ngx_conf_t *cf);
    char *(*init_main_conf)(ngx_conf_t *cf, void *conf);

    void *(*create_srv_conf)(ngx_conf_t *cf);
    char *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);

    void *(*create_loc_conf)(ngx_conf_t *cf);
    char *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
} ngx_http_module_t;
# 33 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_busy_lock.h" 1
# 18 "src/http/ngx_http_busy_lock.h"
typedef struct {
    u_char *md5_mask;
    char *md5;
    int cacheable;

    int busy;
    int max_busy;

    int waiting;
    int max_waiting;

    time_t timeout;

    ngx_event_mutex_t *mutex;
} ngx_http_busy_lock_t;


typedef struct {
    time_t time;
    ngx_event_t *event;
    void (*event_handler)(ngx_event_t *ev);
    u_char *md5;
    int slot;
} ngx_http_busy_lock_ctx_t;


int ngx_http_busy_lock(ngx_http_busy_lock_t *bl, ngx_http_busy_lock_ctx_t *bc);
int ngx_http_busy_lock_cacheable(ngx_http_busy_lock_t *bl,
                                 ngx_http_busy_lock_ctx_t *bc, int lock);
void ngx_http_busy_unlock(ngx_http_busy_lock_t *bl,
                          ngx_http_busy_lock_ctx_t *bc);

char *ngx_http_set_busy_lock_slot(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf);
# 34 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_script.h" 1
# 17 "src/http/ngx_http_script.h"
typedef struct {
    u_char *ip;
    u_char *pos;
    ngx_http_variable_value_t *sp;

    ngx_str_t buf;
    ngx_str_t line;


    u_char *args;

    unsigned flushed:1;
    unsigned skip:1;
    unsigned quote:1;
    unsigned is_args:1;
    unsigned log:1;

    ngx_int_t status;
    ngx_http_request_t *request;
} ngx_http_script_engine_t;


typedef struct {
    ngx_conf_t *cf;
    ngx_str_t *source;

    ngx_array_t **flushes;
    ngx_array_t **lengths;
    ngx_array_t **values;

    ngx_uint_t variables;
    ngx_uint_t ncaptures;
    ngx_uint_t captures_mask;
    ngx_uint_t size;

    void *main;

    unsigned compile_args:1;
    unsigned complete_lengths:1;
    unsigned complete_values:1;
    unsigned zero:1;
    unsigned conf_prefix:1;
    unsigned root_prefix:1;

    unsigned dup_capture:1;
    unsigned args:1;
} ngx_http_script_compile_t;


typedef struct {
    ngx_str_t value;
    ngx_uint_t *flushes;
    void *lengths;
    void *values;
} ngx_http_complex_value_t;


typedef struct {
    ngx_conf_t *cf;
    ngx_str_t *value;
    ngx_http_complex_value_t *complex_value;

    unsigned zero:1;
    unsigned conf_prefix:1;
    unsigned root_prefix:1;
} ngx_http_compile_complex_value_t;


typedef void (*ngx_http_script_code_pt) (ngx_http_script_engine_t *e);
typedef size_t (*ngx_http_script_len_code_pt) (ngx_http_script_engine_t *e);


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t len;
} ngx_http_script_copy_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t index;
} ngx_http_script_var_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    ngx_http_set_variable_pt handler;
    uintptr_t data;
} ngx_http_script_var_handler_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t n;
} ngx_http_script_copy_capture_code_t;




typedef struct {
    ngx_http_script_code_pt code;
    ngx_http_regex_t *regex;
    ngx_array_t *lengths;
    uintptr_t size;
    uintptr_t status;
    uintptr_t next;

    uintptr_t test:1;
    uintptr_t negative_test:1;
    uintptr_t uri:1;
    uintptr_t args:1;


    uintptr_t add_args:1;

    uintptr_t redirect:1;
    uintptr_t break_cycle:1;

    ngx_str_t name;
} ngx_http_script_regex_code_t;


typedef struct {
    ngx_http_script_code_pt code;

    uintptr_t uri:1;
    uintptr_t args:1;


    uintptr_t add_args:1;

    uintptr_t redirect:1;
} ngx_http_script_regex_end_code_t;




typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t conf_prefix;
} ngx_http_script_full_name_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t status;
    ngx_http_complex_value_t text;
} ngx_http_script_return_code_t;


typedef enum {
    ngx_http_script_file_plain = 0,
    ngx_http_script_file_not_plain,
    ngx_http_script_file_dir,
    ngx_http_script_file_not_dir,
    ngx_http_script_file_exists,
    ngx_http_script_file_not_exists,
    ngx_http_script_file_exec,
    ngx_http_script_file_not_exec
} ngx_http_script_file_op_e;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t op;
} ngx_http_script_file_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t next;
    void **loc_conf;
} ngx_http_script_if_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    ngx_array_t *lengths;
} ngx_http_script_complex_value_code_t;


typedef struct {
    ngx_http_script_code_pt code;
    uintptr_t value;
    uintptr_t text_len;
    uintptr_t text_data;
} ngx_http_script_value_code_t;


void ngx_http_script_flush_complex_value(ngx_http_request_t *r,
    ngx_http_complex_value_t *val);
ngx_int_t ngx_http_complex_value(ngx_http_request_t *r,
    ngx_http_complex_value_t *val, ngx_str_t *value);
ngx_int_t ngx_http_compile_complex_value(ngx_http_compile_complex_value_t *ccv);
char *ngx_http_set_complex_value_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


ngx_int_t ngx_http_test_predicates(ngx_http_request_t *r,
    ngx_array_t *predicates);
char *ngx_http_set_predicate_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

ngx_uint_t ngx_http_script_variables_count(ngx_str_t *value);
ngx_int_t ngx_http_script_compile(ngx_http_script_compile_t *sc);
u_char *ngx_http_script_run(ngx_http_request_t *r, ngx_str_t *value,
    void *code_lengths, size_t reserved, void *code_values);
void ngx_http_script_flush_no_cacheable_variables(ngx_http_request_t *r,
    ngx_array_t *indices);

void *ngx_http_script_start_code(ngx_pool_t *pool, ngx_array_t **codes,
    size_t size);
void *ngx_http_script_add_code(ngx_array_t *codes, size_t size, void *code);

size_t ngx_http_script_copy_len_code(ngx_http_script_engine_t *e);
void ngx_http_script_copy_code(ngx_http_script_engine_t *e);
size_t ngx_http_script_copy_var_len_code(ngx_http_script_engine_t *e);
void ngx_http_script_copy_var_code(ngx_http_script_engine_t *e);
size_t ngx_http_script_copy_capture_len_code(ngx_http_script_engine_t *e);
void ngx_http_script_copy_capture_code(ngx_http_script_engine_t *e);
size_t ngx_http_script_mark_args_code(ngx_http_script_engine_t *e);
void ngx_http_script_start_args_code(ngx_http_script_engine_t *e);

void ngx_http_script_regex_start_code(ngx_http_script_engine_t *e);
void ngx_http_script_regex_end_code(ngx_http_script_engine_t *e);

void ngx_http_script_return_code(ngx_http_script_engine_t *e);
void ngx_http_script_break_code(ngx_http_script_engine_t *e);
void ngx_http_script_if_code(ngx_http_script_engine_t *e);
void ngx_http_script_equal_code(ngx_http_script_engine_t *e);
void ngx_http_script_not_equal_code(ngx_http_script_engine_t *e);
void ngx_http_script_file_code(ngx_http_script_engine_t *e);
void ngx_http_script_complex_value_code(ngx_http_script_engine_t *e);
void ngx_http_script_value_code(ngx_http_script_engine_t *e);
void ngx_http_script_set_var_code(ngx_http_script_engine_t *e);
void ngx_http_script_var_set_handler_code(ngx_http_script_engine_t *e);
void ngx_http_script_var_code(ngx_http_script_engine_t *e);
void ngx_http_script_nop_code(ngx_http_script_engine_t *e);
# 35 "src/http/ngx_http.h" 2
# 1 "src/http/ngx_http_core_module.h" 1
# 52 "src/http/ngx_http_core_module.h"
typedef struct ngx_http_location_tree_node_s ngx_http_location_tree_node_t;
typedef struct ngx_http_core_loc_conf_s ngx_http_core_loc_conf_t;


typedef struct {
    union {
        struct sockaddr sockaddr;
        struct sockaddr_in sockaddr_in;




        struct sockaddr_un sockaddr_un;

        u_char sockaddr_data[sizeof(struct sockaddr_un)];
    } u;

    socklen_t socklen;

    unsigned set:1;
    unsigned default_server:1;
    unsigned bind:1;
    unsigned wildcard:1;






    unsigned so_keepalive:2;

    int backlog;
    int rcvbuf;
    int sndbuf;




    int tcp_keepidle;
    int tcp_keepintvl;
    int tcp_keepcnt;






    ngx_uint_t deferred_accept;


    u_char addr[(sizeof("unix:") - 1 + (sizeof(struct sockaddr_un) - __builtin_offsetof (struct sockaddr_un, sun_path))) + 1];
} ngx_http_listen_opt_t;


typedef enum {
    NGX_HTTP_POST_READ_PHASE = 0,

    NGX_HTTP_SERVER_REWRITE_PHASE,

    NGX_HTTP_FIND_CONFIG_PHASE,
    NGX_HTTP_REWRITE_PHASE,
    NGX_HTTP_POST_REWRITE_PHASE,

    NGX_HTTP_PREACCESS_PHASE,

    NGX_HTTP_ACCESS_PHASE,
    NGX_HTTP_POST_ACCESS_PHASE,

    NGX_HTTP_TRY_FILES_PHASE,
    NGX_HTTP_CONTENT_PHASE,

    NGX_HTTP_LOG_PHASE
} ngx_http_phases;

typedef struct ngx_http_phase_handler_s ngx_http_phase_handler_t;

typedef ngx_int_t (*ngx_http_phase_handler_pt)(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);

struct ngx_http_phase_handler_s {
    ngx_http_phase_handler_pt checker;
    ngx_http_handler_pt handler;
    ngx_uint_t next;
};


typedef struct {
    ngx_http_phase_handler_t *handlers;
    ngx_uint_t server_rewrite_index;
    ngx_uint_t location_rewrite_index;
} ngx_http_phase_engine_t;


typedef struct {
    ngx_array_t handlers;
} ngx_http_phase_t;


typedef struct {
    ngx_array_t servers;

    ngx_http_phase_engine_t phase_engine;

    ngx_hash_t headers_in_hash;

    ngx_hash_t variables_hash;

    ngx_array_t variables;
    ngx_uint_t ncaptures;

    ngx_uint_t server_names_hash_max_size;
    ngx_uint_t server_names_hash_bucket_size;

    ngx_uint_t variables_hash_max_size;
    ngx_uint_t variables_hash_bucket_size;

    ngx_hash_keys_arrays_t *variables_keys;

    ngx_array_t *ports;

    ngx_uint_t try_files;

    ngx_http_phase_t phases[NGX_HTTP_LOG_PHASE + 1];
} ngx_http_core_main_conf_t;


typedef struct {

    ngx_array_t server_names;


    ngx_http_conf_ctx_t *ctx;

    ngx_str_t server_name;

    size_t connection_pool_size;
    size_t request_pool_size;
    size_t client_header_buffer_size;

    ngx_bufs_t large_client_header_buffers;

    ngx_msec_t client_header_timeout;

    ngx_flag_t ignore_invalid_headers;
    ngx_flag_t merge_slashes;
    ngx_flag_t underscores_in_headers;

    unsigned listen:1;

    unsigned captures:1;


    ngx_http_core_loc_conf_t **named_locations;
} ngx_http_core_srv_conf_t;





typedef struct {

    ngx_http_core_srv_conf_t *default_server;

    ngx_http_virtual_names_t *virtual_names;




} ngx_http_addr_conf_t;


typedef struct {
    in_addr_t addr;
    ngx_http_addr_conf_t conf;
} ngx_http_in_addr_t;
# 239 "src/http/ngx_http_core_module.h"
typedef struct {

    void *addrs;
    ngx_uint_t naddrs;
} ngx_http_port_t;


typedef struct {
    ngx_int_t family;
    in_port_t port;
    ngx_array_t addrs;
} ngx_http_conf_port_t;


typedef struct {
    ngx_http_listen_opt_t opt;

    ngx_hash_t hash;
    ngx_hash_wildcard_t *wc_head;
    ngx_hash_wildcard_t *wc_tail;


    ngx_uint_t nregex;
    ngx_http_server_name_t *regex;



    ngx_http_core_srv_conf_t *default_server;
    ngx_array_t servers;
} ngx_http_conf_addr_t;


struct ngx_http_server_name_s {

    ngx_http_regex_t *regex;

    ngx_http_core_srv_conf_t *server;
    ngx_str_t name;
};


typedef struct {
    ngx_int_t status;
    ngx_int_t overwrite;
    ngx_http_complex_value_t value;
    ngx_str_t args;
} ngx_http_err_page_t;


typedef struct {
    ngx_array_t *lengths;
    ngx_array_t *values;
    ngx_str_t name;

    unsigned code:10;
    unsigned test_dir:1;
} ngx_http_try_file_t;


struct ngx_http_core_loc_conf_s {
    ngx_str_t name;


    ngx_http_regex_t *regex;


    unsigned noname:1;
    unsigned lmt_excpt:1;
    unsigned named:1;

    unsigned exact_match:1;
    unsigned noregex:1;

    unsigned auto_redirect:1;

    unsigned gzip_disable_msie6:2;





    ngx_http_location_tree_node_t *static_locations;

    ngx_http_core_loc_conf_t **regex_locations;



    void **loc_conf;

    uint32_t limit_except;
    void **limit_except_loc_conf;

    ngx_http_handler_pt handler;


    size_t alias;
    ngx_str_t root;
    ngx_str_t post_action;

    ngx_array_t *root_lengths;
    ngx_array_t *root_values;

    ngx_array_t *types;
    ngx_hash_t types_hash;
    ngx_str_t default_type;

    off_t client_max_body_size;
    off_t directio;
    off_t directio_alignment;

    size_t client_body_buffer_size;
    size_t send_lowat;
    size_t postpone_output;
    size_t limit_rate;
    size_t limit_rate_after;
    size_t sendfile_max_chunk;
    size_t read_ahead;

    ngx_msec_t client_body_timeout;
    ngx_msec_t send_timeout;
    ngx_msec_t keepalive_timeout;
    ngx_msec_t lingering_time;
    ngx_msec_t lingering_timeout;
    ngx_msec_t resolver_timeout;

    ngx_resolver_t *resolver;

    time_t keepalive_header;

    ngx_uint_t keepalive_requests;
    ngx_uint_t keepalive_disable;
    ngx_uint_t satisfy;
    ngx_uint_t lingering_close;
    ngx_uint_t if_modified_since;
    ngx_uint_t max_ranges;
    ngx_uint_t client_body_in_file_only;

    ngx_flag_t client_body_in_single_buffer;

    ngx_flag_t internal;
    ngx_flag_t sendfile;



    ngx_flag_t tcp_nopush;
    ngx_flag_t tcp_nodelay;
    ngx_flag_t reset_timedout_connection;
    ngx_flag_t server_name_in_redirect;
    ngx_flag_t port_in_redirect;
    ngx_flag_t msie_padding;
    ngx_flag_t msie_refresh;
    ngx_flag_t log_not_found;
    ngx_flag_t log_subrequest;
    ngx_flag_t recursive_error_pages;
    ngx_flag_t server_tokens;
    ngx_flag_t chunked_transfer_encoding;


    ngx_flag_t gzip_vary;

    ngx_uint_t gzip_http_version;
    ngx_uint_t gzip_proxied;


    ngx_array_t *gzip_disable;




    ngx_uint_t disable_symlinks;
    ngx_http_complex_value_t *disable_symlinks_from;


    ngx_array_t *error_pages;
    ngx_http_try_file_t *try_files;

    ngx_path_t *client_body_temp_path;

    ngx_open_file_cache_t *open_file_cache;
    time_t open_file_cache_valid;
    ngx_uint_t open_file_cache_min_uses;
    ngx_flag_t open_file_cache_errors;
    ngx_flag_t open_file_cache_events;

    ngx_log_t *error_log;

    ngx_uint_t types_hash_max_size;
    ngx_uint_t types_hash_bucket_size;

    ngx_queue_t *locations;




};


typedef struct {
    ngx_queue_t queue;
    ngx_http_core_loc_conf_t *exact;
    ngx_http_core_loc_conf_t *inclusive;
    ngx_str_t *name;
    u_char *file_name;
    ngx_uint_t line;
    ngx_queue_t list;
} ngx_http_location_queue_t;


struct ngx_http_location_tree_node_s {
    ngx_http_location_tree_node_t *left;
    ngx_http_location_tree_node_t *right;
    ngx_http_location_tree_node_t *tree;

    ngx_http_core_loc_conf_t *exact;
    ngx_http_core_loc_conf_t *inclusive;

    u_char auto_redirect;
    u_char len;
    u_char name[1];
};


void ngx_http_core_run_phases(ngx_http_request_t *r);
ngx_int_t ngx_http_core_generic_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_rewrite_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_find_config_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_post_rewrite_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_access_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_post_access_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_try_files_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);
ngx_int_t ngx_http_core_content_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph);


void *ngx_http_test_content_type(ngx_http_request_t *r, ngx_hash_t *types_hash);
ngx_int_t ngx_http_set_content_type(ngx_http_request_t *r);
void ngx_http_set_exten(ngx_http_request_t *r);
ngx_int_t ngx_http_send_response(ngx_http_request_t *r, ngx_uint_t status,
    ngx_str_t *ct, ngx_http_complex_value_t *cv);
u_char *ngx_http_map_uri_to_path(ngx_http_request_t *r, ngx_str_t *name,
    size_t *root_length, size_t reserved);
ngx_int_t ngx_http_auth_basic_user(ngx_http_request_t *r);

ngx_int_t ngx_http_gzip_ok(ngx_http_request_t *r);



ngx_int_t ngx_http_subrequest(ngx_http_request_t *r,
    ngx_str_t *uri, ngx_str_t *args, ngx_http_request_t **sr,
    ngx_http_post_subrequest_t *psr, ngx_uint_t flags);
ngx_int_t ngx_http_internal_redirect(ngx_http_request_t *r,
    ngx_str_t *uri, ngx_str_t *args);
ngx_int_t ngx_http_named_location(ngx_http_request_t *r, ngx_str_t *name);


ngx_http_cleanup_t *ngx_http_cleanup_add(ngx_http_request_t *r, size_t size);


typedef ngx_int_t (*ngx_http_output_header_filter_pt)(ngx_http_request_t *r);
typedef ngx_int_t (*ngx_http_output_body_filter_pt)
    (ngx_http_request_t *r, ngx_chain_t *chain);


ngx_int_t ngx_http_output_filter(ngx_http_request_t *r, ngx_chain_t *chain);
ngx_int_t ngx_http_write_filter(ngx_http_request_t *r, ngx_chain_t *chain);


ngx_int_t ngx_http_set_disable_symlinks(ngx_http_request_t *r,
    ngx_http_core_loc_conf_t *clcf, ngx_str_t *path, ngx_open_file_info_t *of);

ngx_int_t ngx_http_get_forwarded_addr(ngx_http_request_t *r, ngx_addr_t *addr,
    u_char *xff, size_t xfflen, ngx_array_t *proxies, int recursive);


extern ngx_module_t ngx_http_core_module;

extern ngx_uint_t ngx_http_max_module;

extern ngx_str_t ngx_http_core_get_method;
# 36 "src/http/ngx_http.h" 2


# 1 "src/http/ngx_http_cache.h" 1
# 28 "src/http/ngx_http_cache.h"
typedef struct {
    ngx_uint_t status;
    time_t valid;
} ngx_http_cache_valid_t;


typedef struct {
    ngx_rbtree_node_t node;
    ngx_queue_t queue;

    u_char key[16
                                         - sizeof(ngx_rbtree_key_t)];

    unsigned count:20;
    unsigned uses:10;
    unsigned valid_msec:10;
    unsigned error:10;
    unsigned exists:1;
    unsigned updating:1;
    unsigned deleting:1;


    ngx_file_uniq_t uniq;
    time_t expire;
    time_t valid_sec;
    size_t body_start;
    off_t fs_size;
} ngx_http_file_cache_node_t;


struct ngx_http_cache_s {
    ngx_file_t file;
    ngx_array_t keys;
    uint32_t crc32;
    u_char key[16];

    ngx_file_uniq_t uniq;
    time_t valid_sec;
    time_t last_modified;
    time_t date;

    size_t header_start;
    size_t body_start;
    off_t length;
    off_t fs_size;

    ngx_uint_t min_uses;
    ngx_uint_t error;
    ngx_uint_t valid_msec;

    ngx_buf_t *buf;

    ngx_http_file_cache_t *file_cache;
    ngx_http_file_cache_node_t *node;

    ngx_msec_t lock_timeout;
    ngx_msec_t wait_time;

    ngx_event_t wait_event;

    unsigned lock:1;
    unsigned waiting:1;

    unsigned updated:1;
    unsigned updating:1;
    unsigned exists:1;
    unsigned temp_file:1;
};


typedef struct {
    time_t valid_sec;
    time_t last_modified;
    time_t date;
    uint32_t crc32;
    u_short valid_msec;
    u_short header_start;
    u_short body_start;
} ngx_http_file_cache_header_t;


typedef struct {
    ngx_rbtree_t rbtree;
    ngx_rbtree_node_t sentinel;
    ngx_queue_t queue;
    ngx_atomic_t cold;
    ngx_atomic_t loading;
    off_t size;
} ngx_http_file_cache_sh_t;


struct ngx_http_file_cache_s {
    ngx_http_file_cache_sh_t *sh;
    ngx_slab_pool_t *shpool;

    ngx_path_t *path;

    off_t max_size;
    size_t bsize;

    time_t inactive;

    ngx_uint_t files;
    ngx_uint_t loader_files;
    ngx_msec_t last;
    ngx_msec_t loader_sleep;
    ngx_msec_t loader_threshold;

    ngx_shm_zone_t *shm_zone;
};


ngx_int_t ngx_http_file_cache_new(ngx_http_request_t *r);
ngx_int_t ngx_http_file_cache_create(ngx_http_request_t *r);
void ngx_http_file_cache_create_key(ngx_http_request_t *r);
ngx_int_t ngx_http_file_cache_open(ngx_http_request_t *r);
void ngx_http_file_cache_set_header(ngx_http_request_t *r, u_char *buf);
void ngx_http_file_cache_update(ngx_http_request_t *r, ngx_temp_file_t *tf);
ngx_int_t ngx_http_cache_send(ngx_http_request_t *);
void ngx_http_file_cache_free(ngx_http_cache_t *c, ngx_temp_file_t *tf);
time_t ngx_http_file_cache_valid(ngx_array_t *cache_valid, ngx_uint_t status);

char *ngx_http_file_cache_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_http_file_cache_valid_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


extern ngx_str_t ngx_http_cache_status[];
# 39 "src/http/ngx_http.h" 2


# 1 "src/http/modules/ngx_http_ssi_filter_module.h" 1
# 33 "src/http/modules/ngx_http_ssi_filter_module.h"
typedef struct {
    ngx_hash_t hash;
    ngx_hash_keys_arrays_t commands;
} ngx_http_ssi_main_conf_t;


typedef struct {
    ngx_buf_t *buf;

    u_char *pos;
    u_char *copy_start;
    u_char *copy_end;

    ngx_uint_t key;
    ngx_str_t command;
    ngx_array_t params;
    ngx_table_elt_t *param;
    ngx_table_elt_t params_array[4];

    ngx_chain_t *in;
    ngx_chain_t *out;
    ngx_chain_t **last_out;
    ngx_chain_t *busy;
    ngx_chain_t *free;

    ngx_uint_t state;
    ngx_uint_t saved_state;
    size_t saved;
    size_t looked;

    size_t value_len;

    ngx_list_t *variables;
    ngx_array_t *blocks;


    ngx_uint_t ncaptures;
    int *captures;
    u_char *captures_data;


    unsigned conditional:2;
    unsigned encoding:2;
    unsigned block:1;
    unsigned output:1;
    unsigned output_chosen:1;

    ngx_http_request_t *wait;
    void *value_buf;
    ngx_str_t timefmt;
    ngx_str_t errmsg;
} ngx_http_ssi_ctx_t;


typedef ngx_int_t (*ngx_http_ssi_command_pt) (ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **);


typedef struct {
    ngx_str_t name;
    ngx_uint_t index;

    unsigned mandatory:1;
    unsigned multiple:1;
} ngx_http_ssi_param_t;


typedef struct {
    ngx_str_t name;
    ngx_http_ssi_command_pt handler;
    ngx_http_ssi_param_t *params;

    unsigned conditional:2;
    unsigned block:1;
    unsigned flush:1;
} ngx_http_ssi_command_t;


extern ngx_module_t ngx_http_ssi_filter_module;
# 42 "src/http/ngx_http.h" 2






struct ngx_http_log_ctx_s {
    ngx_connection_t *connection;
    ngx_http_request_t *request;
    ngx_http_request_t *current_request;
};


typedef struct {
    ngx_uint_t http_version;
    ngx_uint_t code;
    ngx_uint_t count;
    u_char *start;
    u_char *end;
} ngx_http_status_t;






ngx_int_t ngx_http_add_location(ngx_conf_t *cf, ngx_queue_t **locations,
    ngx_http_core_loc_conf_t *clcf);
ngx_int_t ngx_http_add_listen(ngx_conf_t *cf, ngx_http_core_srv_conf_t *cscf,
    ngx_http_listen_opt_t *lsopt);


void ngx_http_init_connection(ngx_connection_t *c);





ngx_int_t ngx_http_parse_request_line(ngx_http_request_t *r, ngx_buf_t *b);
ngx_int_t ngx_http_parse_complex_uri(ngx_http_request_t *r,
    ngx_uint_t merge_slashes);
ngx_int_t ngx_http_parse_status_line(ngx_http_request_t *r, ngx_buf_t *b,
    ngx_http_status_t *status);
ngx_int_t ngx_http_parse_unsafe_uri(ngx_http_request_t *r, ngx_str_t *uri,
    ngx_str_t *args, ngx_uint_t *flags);
ngx_int_t ngx_http_parse_header_line(ngx_http_request_t *r, ngx_buf_t *b,
    ngx_uint_t allow_underscores);
ngx_int_t ngx_http_parse_multi_header_lines(ngx_array_t *headers,
    ngx_str_t *name, ngx_str_t *value);
ngx_int_t ngx_http_arg(ngx_http_request_t *r, u_char *name, size_t len,
    ngx_str_t *value);
void ngx_http_split_args(ngx_http_request_t *r, ngx_str_t *uri,
    ngx_str_t *args);


ngx_int_t ngx_http_find_server_conf(ngx_http_request_t *r);
void ngx_http_update_location_config(ngx_http_request_t *r);
void ngx_http_handler(ngx_http_request_t *r);
void ngx_http_run_posted_requests(ngx_connection_t *c);
ngx_int_t ngx_http_post_request(ngx_http_request_t *r,
    ngx_http_posted_request_t *pr);
void ngx_http_finalize_request(ngx_http_request_t *r, ngx_int_t rc);

void ngx_http_empty_handler(ngx_event_t *wev);
void ngx_http_request_empty_handler(ngx_http_request_t *r);
# 115 "src/http/ngx_http.h"
ngx_int_t ngx_http_send_special(ngx_http_request_t *r, ngx_uint_t flags);


ngx_int_t ngx_http_read_client_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler);

ngx_int_t ngx_http_send_header(ngx_http_request_t *r);
ngx_int_t ngx_http_special_response_handler(ngx_http_request_t *r,
    ngx_int_t error);
ngx_int_t ngx_http_filter_finalize_request(ngx_http_request_t *r,
    ngx_module_t *m, ngx_int_t error);
void ngx_http_clean_header(ngx_http_request_t *r);


time_t ngx_http_parse_time(u_char *value, size_t len);
size_t ngx_http_get_time(char *buf, time_t t);



ngx_int_t ngx_http_discard_request_body(ngx_http_request_t *r);
void ngx_http_discarded_request_body_handler(ngx_http_request_t *r);
void ngx_http_block_reading(ngx_http_request_t *r);
void ngx_http_test_reading(ngx_http_request_t *r);


char *ngx_http_types_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
char *ngx_http_merge_types(ngx_conf_t *cf, ngx_array_t **keys,
    ngx_hash_t *types_hash, ngx_array_t **prev_keys,
    ngx_hash_t *prev_types_hash, ngx_str_t *default_types);
ngx_int_t ngx_http_set_default_types(ngx_conf_t *cf, ngx_array_t **types,
    ngx_str_t *default_type);






extern ngx_module_t ngx_http_module;

extern ngx_str_t ngx_http_html_default_types[];


extern ngx_http_output_header_filter_pt ngx_http_top_header_filter;
extern ngx_http_output_body_filter_pt ngx_http_top_body_filter;
# 11 "src/http/ngx_http_upstream.c" 2



static ngx_int_t ngx_http_upstream_cache(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_cache_send(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_cache_status(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);


static void ngx_http_upstream_init_request(ngx_http_request_t *r);
static void ngx_http_upstream_resolve_handler(ngx_resolver_ctx_t *ctx);
static void ngx_http_upstream_rd_check_broken_connection(ngx_http_request_t *r);
static void ngx_http_upstream_wr_check_broken_connection(ngx_http_request_t *r);
static void ngx_http_upstream_check_broken_connection(ngx_http_request_t *r,
    ngx_event_t *ev);
static void ngx_http_upstream_connect(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_reinit(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_send_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_send_request_handler(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_header(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_test_next(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_intercept_errors(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_test_connect(ngx_connection_t *c);
static ngx_int_t ngx_http_upstream_process_headers(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_body_in_memory(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_send_response(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void
    ngx_http_upstream_process_non_buffered_downstream(ngx_http_request_t *r);
static void
    ngx_http_upstream_process_non_buffered_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void
    ngx_http_upstream_process_non_buffered_request(ngx_http_request_t *r,
    ngx_uint_t do_write);
static ngx_int_t ngx_http_upstream_non_buffered_filter_init(void *data);
static ngx_int_t ngx_http_upstream_non_buffered_filter(void *data,
    ssize_t bytes);
static void ngx_http_upstream_process_downstream(ngx_http_request_t *r);
static void ngx_http_upstream_process_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_request(ngx_http_request_t *r);
static void ngx_http_upstream_store(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_dummy_handler(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_next(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_uint_t ft_type);
static void ngx_http_upstream_cleanup(void *data);
static void ngx_http_upstream_finalize_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_int_t rc);

static ngx_int_t ngx_http_upstream_process_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_content_length(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_set_cookie(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_process_cache_control(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_ignore_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_expires(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_accel_expires(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_limit_rate(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_buffering(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_charset(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_connection(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_process_transfer_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_copy_multi_header_lines(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_content_type(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_last_modified(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_location(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_refresh(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_set_cookie(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_allow_ranges(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);


static ngx_int_t ngx_http_upstream_copy_content_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);


static ngx_int_t ngx_http_upstream_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_upstream_addr_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_response_time_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_response_length_variable(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

static char *ngx_http_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *dummy);
static char *ngx_http_upstream_server(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

static void *ngx_http_upstream_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_upstream_init_main_conf(ngx_conf_t *cf, void *conf);
# 147 "src/http/ngx_http_upstream.c"
ngx_http_upstream_header_t ngx_http_upstream_headers_in[] = {

    { { sizeof("Status") - 1, (u_char *) "Status" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, status),
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("Content-Type") - 1, (u_char *) "Content-Type" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, content_type),
                 ngx_http_upstream_copy_content_type, 0, 1 },

    { { sizeof("Content-Length") - 1, (u_char *) "Content-Length" },
                 ngx_http_upstream_process_content_length,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, content_length),
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { { sizeof("Date") - 1, (u_char *) "Date" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, date),
                 ngx_http_upstream_copy_header_line,
                 __builtin_offsetof (ngx_http_headers_out_t, date), 0 },

    { { sizeof("Last-Modified") - 1, (u_char *) "Last-Modified" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, last_modified),
                 ngx_http_upstream_copy_last_modified, 0, 0 },

    { { sizeof("ETag") - 1, (u_char *) "ETag" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, etag),
                 ngx_http_upstream_copy_header_line,
                 __builtin_offsetof (ngx_http_headers_out_t, etag), 0 },

    { { sizeof("Server") - 1, (u_char *) "Server" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, server),
                 ngx_http_upstream_copy_header_line,
                 __builtin_offsetof (ngx_http_headers_out_t, server), 0 },

    { { sizeof("WWW-Authenticate") - 1, (u_char *) "WWW-Authenticate" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, www_authenticate),
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("Location") - 1, (u_char *) "Location" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, location),
                 ngx_http_upstream_rewrite_location, 0, 0 },

    { { sizeof("Refresh") - 1, (u_char *) "Refresh" },
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_rewrite_refresh, 0, 0 },

    { { sizeof("Set-Cookie") - 1, (u_char *) "Set-Cookie" },
                 ngx_http_upstream_process_set_cookie, 0,
                 ngx_http_upstream_rewrite_set_cookie, 0, 1 },

    { { sizeof("Content-Disposition") - 1, (u_char *) "Content-Disposition" },
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_copy_header_line, 0, 1 },

    { { sizeof("Cache-Control") - 1, (u_char *) "Cache-Control" },
                 ngx_http_upstream_process_cache_control, 0,
                 ngx_http_upstream_copy_multi_header_lines,
                 __builtin_offsetof (ngx_http_headers_out_t, cache_control), 1 },

    { { sizeof("Expires") - 1, (u_char *) "Expires" },
                 ngx_http_upstream_process_expires, 0,
                 ngx_http_upstream_copy_header_line,
                 __builtin_offsetof (ngx_http_headers_out_t, expires), 1 },

    { { sizeof("Accept-Ranges") - 1, (u_char *) "Accept-Ranges" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, accept_ranges),
                 ngx_http_upstream_copy_allow_ranges,
                 __builtin_offsetof (ngx_http_headers_out_t, accept_ranges), 1 },

    { { sizeof("Connection") - 1, (u_char *) "Connection" },
                 ngx_http_upstream_process_connection, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { { sizeof("Keep-Alive") - 1, (u_char *) "Keep-Alive" },
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { { sizeof("X-Powered-By") - 1, (u_char *) "X-Powered-By" },
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("X-Accel-Expires") - 1, (u_char *) "X-Accel-Expires" },
                 ngx_http_upstream_process_accel_expires, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("X-Accel-Redirect") - 1, (u_char *) "X-Accel-Redirect" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, x_accel_redirect),
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("X-Accel-Limit-Rate") - 1, (u_char *) "X-Accel-Limit-Rate" },
                 ngx_http_upstream_process_limit_rate, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("X-Accel-Buffering") - 1, (u_char *) "X-Accel-Buffering" },
                 ngx_http_upstream_process_buffering, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("X-Accel-Charset") - 1, (u_char *) "X-Accel-Charset" },
                 ngx_http_upstream_process_charset, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { { sizeof("Transfer-Encoding") - 1, (u_char *) "Transfer-Encoding" },
                 ngx_http_upstream_process_transfer_encoding, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },


    { { sizeof("Content-Encoding") - 1, (u_char *) "Content-Encoding" },
                 ngx_http_upstream_process_header_line,
                 __builtin_offsetof (ngx_http_upstream_headers_in_t, content_encoding),
                 ngx_http_upstream_copy_content_encoding, 0, 0 },


    { { 0, ((void *)0) }, ((void *)0), 0, ((void *)0), 0, 0 }
};


static ngx_command_t ngx_http_upstream_commands[] = {

    { { sizeof("upstream") - 1, (u_char *) "upstream" },
      0x02000000|0x00000100|0x00000002,
      ngx_http_upstream,
      0,
      0,
      ((void *)0) },

    { { sizeof("server") - 1, (u_char *) "server" },
      0x10000000|0x00000800,
      ngx_http_upstream_server,
      __builtin_offsetof (ngx_http_conf_ctx_t, srv_conf),
      0,
      ((void *)0) },

      { { 0, ((void *)0) }, 0, ((void *)0), 0, 0, ((void *)0) }
};


static ngx_http_module_t ngx_http_upstream_module_ctx = {
    ngx_http_upstream_add_variables,
    ((void *)0),

    ngx_http_upstream_create_main_conf,
    ngx_http_upstream_init_main_conf,

    ((void *)0),
    ((void *)0),

    ((void *)0),
    ((void *)0)
};


ngx_module_t ngx_http_upstream_module = {
    0, 0, 0, 0, 0, 0, 1,
    &ngx_http_upstream_module_ctx,
    ngx_http_upstream_commands,
    0x50545448,
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    0, 0, 0, 0, 0, 0, 0, 0
};


static ngx_http_variable_t ngx_http_upstream_vars[] = {

    { { sizeof("upstream_addr") - 1, (u_char *) "upstream_addr" }, ((void *)0),
      ngx_http_upstream_addr_variable, 0,
      2, 0 },

    { { sizeof("upstream_status") - 1, (u_char *) "upstream_status" }, ((void *)0),
      ngx_http_upstream_status_variable, 0,
      2, 0 },

    { { sizeof("upstream_response_time") - 1, (u_char *) "upstream_response_time" }, ((void *)0),
      ngx_http_upstream_response_time_variable, 0,
      2, 0 },

    { { sizeof("upstream_response_length") - 1, (u_char *) "upstream_response_length" }, ((void *)0),
      ngx_http_upstream_response_length_variable, 0,
      2, 0 },



    { { sizeof("upstream_cache_status") - 1, (u_char *) "upstream_cache_status" }, ((void *)0),
      ngx_http_upstream_cache_status, 0,
      2, 0 },



    { { 0, ((void *)0) }, ((void *)0), ((void *)0), 0, 0, 0 }
};


static ngx_http_upstream_next_t ngx_http_upstream_next_errors[] = {
    { 500, 0x00000010 },
    { 502, 0x00000020 },
    { 503, 0x00000040 },
    { 504, 0x00000080 },
    { 404, 0x00000100 },
    { 0, 0 }
};


ngx_conf_bitmask_t ngx_http_upstream_cache_method_mask[] = {
   { { sizeof("GET") - 1, (u_char *) "GET" }, 0x0002},
   { { sizeof("HEAD") - 1, (u_char *) "HEAD" }, 0x0004 },
   { { sizeof("POST") - 1, (u_char *) "POST" }, 0x0008 },
   { { 0, ((void *)0) }, 0 }
};


ngx_conf_bitmask_t ngx_http_upstream_ignore_headers_masks[] = {
    { { sizeof("X-Accel-Redirect") - 1, (u_char *) "X-Accel-Redirect" }, 0x00000002 },
    { { sizeof("X-Accel-Expires") - 1, (u_char *) "X-Accel-Expires" }, 0x00000004 },
    { { sizeof("X-Accel-Limit-Rate") - 1, (u_char *) "X-Accel-Limit-Rate" }, 0x00000040 },
    { { sizeof("X-Accel-Buffering") - 1, (u_char *) "X-Accel-Buffering" }, 0x00000080 },
    { { sizeof("X-Accel-Charset") - 1, (u_char *) "X-Accel-Charset" }, 0x00000100 },
    { { sizeof("Expires") - 1, (u_char *) "Expires" }, 0x00000008 },
    { { sizeof("Cache-Control") - 1, (u_char *) "Cache-Control" }, 0x00000010 },
    { { sizeof("Set-Cookie") - 1, (u_char *) "Set-Cookie" }, 0x00000020 },
    { { 0, ((void *)0) }, 0 }
};


ngx_int_t
ngx_http_upstream_create(ngx_http_request_t *r)
{
    ngx_http_upstream_t *u;

    u = r->upstream;

    if (u && u->cleanup) {
        r->main->count++;
        ngx_http_upstream_cleanup(r);
    }

    u = ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_t));
    if (u == ((void *)0)) {
        return -1;
    }

    r->upstream = u;

    u->peer.log = r->connection->log;
    u->peer.log_error = NGX_ERROR_ERR;





    r->cache = ((void *)0);


    u->headers_in.content_length_n = -1;

    return 0;
}


void
ngx_http_upstream_init(ngx_http_request_t *r)
{
    ngx_connection_t *c;

    c = r->connection;

    ;


    if (c->read->timer_set) {
        ngx_event_del_timer(c->read);
    }

    if (ngx_event_flags & 0x00000004) {

        if (!c->write->active) {
            if (ngx_event_actions.add(c->write, EPOLLOUT, EPOLLET)
                == -1)
            {
                ngx_http_finalize_request(r, 500);
                return;
            }
        }
    }

    ngx_http_upstream_init_request(r);
}


static void
ngx_http_upstream_init_request(ngx_http_request_t *r)
{
    ngx_str_t *host;
    ngx_uint_t i;
    ngx_resolver_ctx_t *ctx, temp;
    ngx_http_cleanup_t *cln;
    ngx_http_upstream_t *u;
    ngx_http_core_loc_conf_t *clcf;
    ngx_http_upstream_srv_conf_t *uscf, **uscfp;
    ngx_http_upstream_main_conf_t *umcf;

    if (r->aio) {
        return;
    }

    u = r->upstream;



    if (u->conf->cache) {
        ngx_int_t rc;

        rc = ngx_http_upstream_cache(r, u);

        if (rc == -3) {
            r->write_event_handler = ngx_http_upstream_init_request;
            return;
        }

        r->write_event_handler = ngx_http_request_empty_handler;

        if (rc == -4) {
            return;
        }

        if (rc != -5) {
            ngx_http_finalize_request(r, rc);
            return;
        }
    }



    u->store = (u->conf->store || u->conf->store_lengths);

    if (!u->store && !r->post_action && !u->conf->ignore_client_abort) {
        r->read_event_handler = ngx_http_upstream_rd_check_broken_connection;
        r->write_event_handler = ngx_http_upstream_wr_check_broken_connection;
    }

    if (r->request_body) {
        u->request_bufs = r->request_body->bufs;
    }

    if (u->create_request(r) != 0) {
        ngx_http_finalize_request(r, 500);
        return;
    }

    u->peer.local = u->conf->local;

    clcf = (r)->loc_conf[ngx_http_core_module.ctx_index];

    u->output.alignment = clcf->directio_alignment;
    u->output.pool = r->pool;
    u->output.bufs.num = 1;
    u->output.bufs.size = clcf->client_body_buffer_size;
    u->output.output_filter = ngx_chain_writer;
    u->output.filter_ctx = &u->writer;

    u->writer.pool = r->pool;

    if (r->upstream_states == ((void *)0)) {

        r->upstream_states = ngx_array_create(r->pool, 1,
                                            sizeof(ngx_http_upstream_state_t));
        if (r->upstream_states == ((void *)0)) {
            ngx_http_finalize_request(r, 500);
            return;
        }

    } else {

        u->state = ngx_array_push(r->upstream_states);
        if (u->state == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        (void) memset(u->state, 0, sizeof(ngx_http_upstream_state_t));
    }

    cln = ngx_http_cleanup_add(r, 0);
    if (cln == ((void *)0)) {
        ngx_http_finalize_request(r, 500);
        return;
    }

    cln->handler = ngx_http_upstream_cleanup;
    cln->data = r;
    u->cleanup = &cln->handler;

    if (u->resolved == ((void *)0)) {

        uscf = u->conf->upstream;

    } else {

        if (u->resolved->sockaddr) {

            if (ngx_http_upstream_create_round_robin_peer(r, u->resolved)
                != 0)
            {
                ngx_http_upstream_finalize_request(r, u,
                                               500);
                return;
            }

            ngx_http_upstream_connect(r, u);

            return;
        }

        host = &u->resolved->host;

        umcf = (r)->main_conf[ngx_http_upstream_module.ctx_index];

        uscfp = umcf->upstreams.elts;

        for (i = 0; i < umcf->upstreams.nelts; i++) {

            uscf = uscfp[i];

            if (uscf->host.len == host->len
                && ((uscf->port == 0 && u->resolved->no_port)
                     || uscf->port == u->resolved->port)
                && memcmp((const char *) uscf->host.data, (const char *) host->data, host->len) == 0)
            {
                goto found;
            }
        }

        if (u->resolved->port == 0) {
            if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 0, "no port in upstream \"%V\"", host);

            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        temp.name = *host;

        ctx = ngx_resolve_start(clcf->resolver, &temp);
        if (ctx == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        if (ctx == (void *) -1) {
            if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 0, "no resolver defined to resolve %V", host);


            ngx_http_upstream_finalize_request(r, u, 502);
            return;
        }

        ctx->name = *host;
        ctx->type = 1;
        ctx->handler = ngx_http_upstream_resolve_handler;
        ctx->data = r;
        ctx->timeout = clcf->resolver_timeout;

        u->resolved->ctx = ctx;

        if (ngx_resolve_name(ctx) != 0) {
            u->resolved->ctx = ((void *)0);
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        return;
    }

found:

    if (uscf->peer.init(r, uscf) != 0) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }

    ngx_http_upstream_connect(r, u);
}




static ngx_int_t
ngx_http_upstream_cache(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t rc;
    ngx_http_cache_t *c;

    c = r->cache;

    if (c == ((void *)0)) {

        if (!(r->method & u->conf->cache_methods)) {
            return -5;
        }

        if (r->method & 0x0004) {
            u->method = ngx_http_core_get_method;
        }

        if (ngx_http_file_cache_new(r) != 0) {
            return -1;
        }

        if (u->create_key(r) != 0) {
            return -1;
        }



        ngx_http_file_cache_create_key(r);

        if (r->cache->header_start + 256 >= u->conf->buffer_size) {
            if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 0, "%V_buffer_size %uz is not enough for cache key, " "it should increased at least to %uz", &u->conf->module, u->conf->buffer_size, (((r->cache->header_start + 256) + (1024 - 1)) & ~(1024 - 1)));





            r->cache = ((void *)0);
            return -5;
        }

        u->cacheable = 1;

        switch (ngx_http_test_predicates(r, u->conf->cache_bypass)) {

        case -1:
            return -1;

        case -5:
            u->cache_status = 2;
            return -5;

        default:
            break;
        }

        c = r->cache;

        c->min_uses = u->conf->cache_min_uses;
        c->body_start = u->conf->buffer_size;
        c->file_cache = u->conf->cache->data;

        c->lock = u->conf->cache_lock;
        c->lock_timeout = u->conf->cache_lock_timeout;

        u->cache_status = 1;
    }

    rc = ngx_http_file_cache_open(r);

    ;


    switch (rc) {

    case 5:

        if (u->conf->cache_use_stale & 0x00000200) {
            u->cache_status = rc;
            rc = 0;

        } else {
            rc = 4;
        }

        break;

    case 0:
        u->cache_status = 6;
    }

    switch (rc) {

    case 0:

        rc = ngx_http_upstream_cache_send(r, u);

        if (rc != 40) {
            return rc;
        }

        break;

    case 4:

        c->valid_sec = 0;
        u->buffer.start = ((void *)0);
        u->cache_status = 3;

        break;

    case -5:

        if ((size_t) (u->buffer.end - u->buffer.start) < u->conf->buffer_size) {
            u->buffer.start = ((void *)0);

        } else {
            u->buffer.pos = u->buffer.start + c->header_start;
            u->buffer.last = u->buffer.pos;
        }

        break;

    case 7:

        u->cacheable = 0;

        break;

    case -2:

        return -3;

    case -1:

        return -1;

    default:



        u->cache_status = 6;

        return rc;
    }

    r->cached = 0;

    return -5;
}


static ngx_int_t
ngx_http_upstream_cache_send(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t rc;
    ngx_http_cache_t *c;

    r->cached = 1;
    c = r->cache;

    if (c->header_start == c->body_start) {
        r->http_version = 9;
        return ngx_http_cache_send(r);
    }



    u->buffer = *c->buf;
    u->buffer.pos += c->header_start;

    (void) memset(&u->headers_in, 0, sizeof(ngx_http_upstream_headers_in_t));
    u->headers_in.content_length_n = -1;

    if (ngx_list_init(&u->headers_in.headers, r->pool, 8,
                      sizeof(ngx_table_elt_t))
        != 0)
    {
        return -1;
    }

    rc = u->process_header(r);

    if (rc == 0) {

        if (ngx_http_upstream_process_headers(r, u) != 0) {
            return -4;
        }

        return ngx_http_cache_send(r);
    }

    if (rc == -1) {
        return -1;
    }





    return rc;
}




static void
ngx_http_upstream_resolve_handler(ngx_resolver_ctx_t *ctx)
{
    ngx_http_request_t *r;
    ngx_http_upstream_t *u;
    ngx_http_upstream_resolved_t *ur;

    r = ctx->data;

    u = r->upstream;
    ur = u->resolved;

    if (ctx->state) {
        if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 0, "%V could not be resolved (%i: %s)", &ctx->name, ctx->state, ngx_resolver_strerror(ctx->state));




        ngx_http_upstream_finalize_request(r, u, 502);
        return;
    }

    ur->naddrs = ctx->naddrs;
    ur->addrs = ctx->addrs;
# 898 "src/http/ngx_http_upstream.c"
    if (ngx_http_upstream_create_round_robin_peer(r, ur) != 0) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }

    ngx_resolve_name_done(ctx);
    ur->ctx = ((void *)0);

    ngx_http_upstream_connect(r, u);
}


static void
ngx_http_upstream_handler(ngx_event_t *ev)
{
    ngx_connection_t *c;
    ngx_http_request_t *r;
    ngx_http_log_ctx_t *ctx;
    ngx_http_upstream_t *u;

    c = ev->data;
    r = c->data;

    u = r->upstream;
    c = r->connection;

    ctx = c->log->data;
    ctx->current_request = r;

    ;


    if (ev->write) {
        u->write_event_handler(r, u);

    } else {
        u->read_event_handler(r, u);
    }

    ngx_http_run_posted_requests(c);
}


static void
ngx_http_upstream_rd_check_broken_connection(ngx_http_request_t *r)
{
    ngx_http_upstream_check_broken_connection(r, r->connection->read);
}


static void
ngx_http_upstream_wr_check_broken_connection(ngx_http_request_t *r)
{
    ngx_http_upstream_check_broken_connection(r, r->connection->write);
}


static void
ngx_http_upstream_check_broken_connection(ngx_http_request_t *r,
    ngx_event_t *ev)
{
    int n;
    char buf[1];
    ngx_err_t err;
    ngx_int_t event;
    ngx_connection_t *c;
    ngx_http_upstream_t *u;

    ;



    c = r->connection;
    u = r->upstream;

    if (c->error) {
        if ((ngx_event_flags & 0x00000001) && ev->active) {

            event = ev->write ? EPOLLOUT : EPOLLIN;

            if (ngx_event_actions.del(ev, event, 0) != 0) {
                ngx_http_upstream_finalize_request(r, u,
                                               500);
                return;
            }
        }

        if (!u->cacheable) {
            ngx_http_upstream_finalize_request(r, u,
                                               499);
        }

        return;
    }
# 1032 "src/http/ngx_http_upstream.c"
    n = recv(c->fd, buf, 1, MSG_PEEK);

    err = (*__errno_location ());

    ;


    if (ev->write && (n >= 0 || err == 11)) {
        return;
    }

    if ((ngx_event_flags & 0x00000001) && ev->active) {

        event = ev->write ? EPOLLOUT : EPOLLIN;

        if (ngx_event_actions.del(ev, event, 0) != 0) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }
    }

    if (n > 0) {
        return;
    }

    if (n == -1) {
        if (err == 11) {
            return;
        }

        ev->error = 1;

    } else {
        err = 0;
    }

    ev->eof = 1;
    c->error = 1;

    if (!u->cacheable && u->peer.connection) {
        if ((ev->log)->log_level >= 7) ngx_log_error_core(7, ev->log, err, "client prematurely closed connection, " "so upstream connection is closed too");


        ngx_http_upstream_finalize_request(r, u,
                                           499);
        return;
    }

    if ((ev->log)->log_level >= 7) ngx_log_error_core(7, ev->log, err, "client prematurely closed connection");


    if (u->peer.connection == ((void *)0)) {
        ngx_http_upstream_finalize_request(r, u,
                                           499);
    }
}


static void
ngx_http_upstream_connect(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t rc;
    ngx_time_t *tp;
    ngx_connection_t *c;

    r->connection->log->action = "connecting to upstream";

    r->connection->single_connection = 0;

    if (u->state && u->state->response_sec) {
        tp = (ngx_time_t *) ngx_cached_time;
        u->state->response_sec = tp->sec - u->state->response_sec;
        u->state->response_msec = tp->msec - u->state->response_msec;
    }

    u->state = ngx_array_push(r->upstream_states);
    if (u->state == ((void *)0)) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }

    (void) memset(u->state, 0, sizeof(ngx_http_upstream_state_t));

    tp = (ngx_time_t *) ngx_cached_time;
    u->state->response_sec = tp->sec;
    u->state->response_msec = tp->msec;

    rc = ngx_event_connect_peer(&u->peer);

    ;


    if (rc == -1) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }

    u->state->peer = u->peer.name;

    if (rc == -3) {
        if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 0, "no live upstreams");
        ngx_http_upstream_next(r, u, 0x40000000);
        return;
    }

    if (rc == -5) {
        ngx_http_upstream_next(r, u, 0x00000002);
        return;
    }



    c = u->peer.connection;

    c->data = r;

    c->write->handler = ngx_http_upstream_handler;
    c->read->handler = ngx_http_upstream_handler;

    u->write_event_handler = ngx_http_upstream_send_request_handler;
    u->read_event_handler = ngx_http_upstream_process_header;

    c->sendfile &= r->connection->sendfile;
    u->output.sendfile = c->sendfile;

    if (c->pool == ((void *)0)) {



        c->pool = ngx_create_pool(128, r->connection->log);
        if (c->pool == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }
    }

    c->log = r->connection->log;
    c->pool->log = c->log;
    c->read->log = c->log;
    c->write->log = c->log;



    u->writer.out = ((void *)0);
    u->writer.last = &u->writer.out;
    u->writer.connection = c;
    u->writer.limit = 0;

    if (u->request_sent) {
        if (ngx_http_upstream_reinit(r, u) != 0) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }
    }

    if (r->request_body
        && r->request_body->buf
        && r->request_body->temp_file
        && r == r->main)
    {





        u->output.free = ngx_alloc_chain_link(r->pool);
        if (u->output.free == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        u->output.free->buf = r->request_body->buf;
        u->output.free->next = ((void *)0);
        u->output.allocated = 1;

        r->request_body->buf->pos = r->request_body->buf->start;
        r->request_body->buf->last = r->request_body->buf->start;
        r->request_body->buf->tag = u->output.tag;
    }

    u->request_sent = 0;

    if (rc == -2) {
        ngx_event_add_timer(c->write, u->conf->connect_timeout);
        return;
    }
# 1234 "src/http/ngx_http_upstream.c"
    ngx_http_upstream_send_request(r, u);
}
# 1309 "src/http/ngx_http_upstream.c"
static ngx_int_t
ngx_http_upstream_reinit(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_chain_t *cl;

    if (u->reinit_request(r) != 0) {
        return -1;
    }

    u->keepalive = 0;

    (void) memset(&u->headers_in, 0, sizeof(ngx_http_upstream_headers_in_t));
    u->headers_in.content_length_n = -1;

    if (ngx_list_init(&u->headers_in.headers, r->pool, 8,
                      sizeof(ngx_table_elt_t))
        != 0)
    {
        return -1;
    }



    for (cl = u->request_bufs; cl; cl = cl->next) {
        cl->buf->pos = cl->buf->start;
        cl->buf->file_pos = 0;
    }



    if (r->request_body && r->request_body->temp_file
        && r != r->main && u->output.buf)
    {
        u->output.free = ngx_alloc_chain_link(r->pool);
        if (u->output.free == ((void *)0)) {
            return -1;
        }

        u->output.free->buf = u->output.buf;
        u->output.free->next = ((void *)0);

        u->output.buf->pos = u->output.buf->start;
        u->output.buf->last = u->output.buf->start;
    }

    u->output.buf = ((void *)0);
    u->output.in = ((void *)0);
    u->output.busy = ((void *)0);



    u->buffer.pos = u->buffer.start;



    if (r->cache) {
        u->buffer.pos += r->cache->header_start;
    }



    u->buffer.last = u->buffer.pos;

    return 0;
}


static void
ngx_http_upstream_send_request(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t rc;
    ngx_connection_t *c;

    c = u->peer.connection;

    ;


    if (!u->request_sent && ngx_http_upstream_test_connect(c) != 0) {
        ngx_http_upstream_next(r, u, 0x00000002);
        return;
    }

    c->log->action = "sending request to upstream";

    rc = ngx_output_chain(&u->output, u->request_sent ? ((void *)0) : u->request_bufs);

    u->request_sent = 1;

    if (rc == -1) {
        ngx_http_upstream_next(r, u, 0x00000002);
        return;
    }

    if (c->write->timer_set) {
        ngx_event_del_timer(c->write);
    }

    if (rc == -2) {
        ngx_event_add_timer(c->write, u->conf->send_timeout);

        if (ngx_handle_write_event(c->write, u->conf->send_lowat) != 0) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        return;
    }



    if (c->tcp_nopush == NGX_TCP_NOPUSH_SET) {
        if (ngx_tcp_push(c->fd) == -1) {
            if ((c->log)->log_level >= 3) ngx_log_error_core(3, c->log, (*__errno_location ()), "setsockopt(!TCP_CORK)" " failed");

            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        c->tcp_nopush = NGX_TCP_NOPUSH_UNSET;
    }

    ngx_event_add_timer(c->read, u->conf->read_timeout);


    if (c->read->ready) {
# 1447 "src/http/ngx_http_upstream.c"
        ngx_http_upstream_process_header(r, u);
        return;
    }


    u->write_event_handler = ngx_http_upstream_dummy_handler;

    if (ngx_handle_write_event(c->write, 0) != 0) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }
}


static void
ngx_http_upstream_send_request_handler(ngx_http_request_t *r,
    ngx_http_upstream_t *u)
{
    ngx_connection_t *c;

    c = u->peer.connection;

    ;


    if (c->write->timedout) {
        ngx_http_upstream_next(r, u, 0x00000004);
        return;
    }
# 1487 "src/http/ngx_http_upstream.c"
    if (u->header_sent) {
        u->write_event_handler = ngx_http_upstream_dummy_handler;

        (void) ngx_handle_write_event(c->write, 0);

        return;
    }

    ngx_http_upstream_send_request(r, u);
}


static void
ngx_http_upstream_process_header(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ssize_t n;
    ngx_int_t rc;
    ngx_connection_t *c;

    c = u->peer.connection;

    ;


    c->log->action = "reading response header from upstream";

    if (c->read->timedout) {
        ngx_http_upstream_next(r, u, 0x00000004);
        return;
    }

    if (!u->request_sent && ngx_http_upstream_test_connect(c) != 0) {
        ngx_http_upstream_next(r, u, 0x00000002);
        return;
    }

    if (u->buffer.start == ((void *)0)) {
        u->buffer.start = ngx_palloc(r->pool, u->conf->buffer_size);
        if (u->buffer.start == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }

        u->buffer.pos = u->buffer.start;
        u->buffer.last = u->buffer.start;
        u->buffer.end = u->buffer.start + u->conf->buffer_size;
        u->buffer.temporary = 1;

        u->buffer.tag = u->output.tag;

        if (ngx_list_init(&u->headers_in.headers, r->pool, 8,
                          sizeof(ngx_table_elt_t))
            != 0)
        {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return;
        }



        if (r->cache) {
            u->buffer.pos += r->cache->header_start;
            u->buffer.last = u->buffer.pos;
        }

    }

    for ( ;; ) {

        n = c->recv(c, u->buffer.last, u->buffer.end - u->buffer.last);

        if (n == -2) {




            if (ngx_handle_read_event(c->read, 0) != 0) {
                ngx_http_upstream_finalize_request(r, u,
                                               500);
                return;
            }

            return;
        }

        if (n == 0) {
            if ((c->log)->log_level >= 4) ngx_log_error_core(4, c->log, 0, "upstream prematurely closed connection");

        }

        if (n == -1 || n == 0) {
            ngx_http_upstream_next(r, u, 0x00000002);
            return;
        }

        u->buffer.last += n;







        rc = u->process_header(r);

        if (rc == -2) {

            if (u->buffer.last == u->buffer.end) {
                if ((c->log)->log_level >= 4) ngx_log_error_core(4, c->log, 0, "upstream sent too big header");


                ngx_http_upstream_next(r, u,
                                       0x00000008);
                return;
            }

            continue;
        }

        break;
    }

    if (rc == 40) {
        ngx_http_upstream_next(r, u, 0x00000008);
        return;
    }

    if (rc == -1) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }



    if (u->headers_in.status_n > 300) {

        if (r->subrequest_in_memory) {
            u->buffer.last = u->buffer.pos;
        }

        if (ngx_http_upstream_test_next(r, u) == 0) {
            return;
        }

        if (ngx_http_upstream_intercept_errors(r, u) == 0) {
            return;
        }
    }

    if (ngx_http_upstream_process_headers(r, u) != 0) {
        return;
    }

    if (!r->subrequest_in_memory) {
        ngx_http_upstream_send_response(r, u);
        return;
    }



    if (u->input_filter == ((void *)0)) {
        u->input_filter_init = ngx_http_upstream_non_buffered_filter_init;
        u->input_filter = ngx_http_upstream_non_buffered_filter;
        u->input_filter_ctx = r;
    }

    if (u->input_filter_init(u->input_filter_ctx) == -1) {
        ngx_http_upstream_finalize_request(r, u,
                                           500);
        return;
    }

    n = u->buffer.last - u->buffer.pos;

    if (n) {
        u->buffer.last -= n;

        u->state->response_length += n;

        if (u->input_filter(u->input_filter_ctx, n) == -1) {
            ngx_http_upstream_finalize_request(r, u, -1);
            return;
        }

        if (u->length == 0) {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }
    }

    u->read_event_handler = ngx_http_upstream_process_body_in_memory;

    ngx_http_upstream_process_body_in_memory(r, u);
}


static ngx_int_t
ngx_http_upstream_test_next(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_uint_t status;
    ngx_http_upstream_next_t *un;

    status = u->headers_in.status_n;

    for (un = ngx_http_upstream_next_errors; un->status; un++) {

        if (status != un->status) {
            continue;
        }

        if (u->peer.tries > 1 && (u->conf->next_upstream & un->mask)) {
            ngx_http_upstream_next(r, u, un->mask);
            return 0;
        }



        if (u->cache_status == 3
            && (u->conf->cache_use_stale & un->mask))
        {
            ngx_int_t rc;

            rc = u->reinit_request(r);

            if (rc == 0) {
                u->cache_status = 4;
                rc = ngx_http_upstream_cache_send(r, u);
            }

            ngx_http_upstream_finalize_request(r, u, rc);
            return 0;
        }


    }

    return -5;
}


static ngx_int_t
ngx_http_upstream_intercept_errors(ngx_http_request_t *r,
    ngx_http_upstream_t *u)
{
    ngx_int_t status;
    ngx_uint_t i;
    ngx_table_elt_t *h;
    ngx_http_err_page_t *err_page;
    ngx_http_core_loc_conf_t *clcf;

    status = u->headers_in.status_n;

    if (status == 404 && u->conf->intercept_404) {
        ngx_http_upstream_finalize_request(r, u, 404);
        return 0;
    }

    if (!u->conf->intercept_errors) {
        return -5;
    }

    clcf = (r)->loc_conf[ngx_http_core_module.ctx_index];

    if (clcf->error_pages == ((void *)0)) {
        return -5;
    }

    err_page = clcf->error_pages->elts;
    for (i = 0; i < clcf->error_pages->nelts; i++) {

        if (err_page[i].status == status) {

            if (status == 401
                && u->headers_in.www_authenticate)
            {
                h = ngx_list_push(&r->headers_out.headers);

                if (h == ((void *)0)) {
                    ngx_http_upstream_finalize_request(r, u,
                                               500);
                    return 0;
                }

                *h = *u->headers_in.www_authenticate;

                r->headers_out.www_authenticate = h;
            }



            if (r->cache) {
                time_t valid;

                valid = ngx_http_file_cache_valid(u->conf->cache_valid, status);

                if (valid) {
                    r->cache->valid_sec = ngx_cached_time->sec + valid;
                    r->cache->error = status;
                }

                ngx_http_file_cache_free(r->cache, u->pipe->temp_file);
            }

            ngx_http_upstream_finalize_request(r, u, status);

            return 0;
        }
    }

    return -5;
}


static ngx_int_t
ngx_http_upstream_test_connect(ngx_connection_t *c)
{
    int err;
    socklen_t len;
# 1821 "src/http/ngx_http_upstream.c"
    {
        err = 0;
        len = sizeof(int);






        if (getsockopt(c->fd, 1, 4, (void *) &err, &len)
            == -1)
        {
            err = (*__errno_location ());
        }

        if (err) {
            c->log->action = "connecting to upstream";
            (void) ngx_connection_error(c, err, "connect() failed");
            return -1;
        }
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_headers(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_str_t *uri, args;
    ngx_uint_t i, flags;
    ngx_list_part_t *part;
    ngx_table_elt_t *h;
    ngx_http_upstream_header_t *hh;
    ngx_http_upstream_main_conf_t *umcf;

    umcf = (r)->main_conf[ngx_http_upstream_module.ctx_index];

    if (u->headers_in.x_accel_redirect
        && !(u->conf->ignore_headers & 0x00000002))
    {
        ngx_http_upstream_finalize_request(r, u, -5);

        part = &u->headers_in.headers.part;
        h = part->elts;

        for (i = 0; ; i++) {

            if (i >= part->nelts) {
                if (part->next == ((void *)0)) {
                    break;
                }

                part = part->next;
                h = part->elts;
                i = 0;
            }

            hh = ngx_hash_find(&umcf->headers_in_hash, h[i].hash,
                               h[i].lowcase_key, h[i].key.len);

            if (hh && hh->redirect) {
                if (hh->copy_handler(r, &h[i], hh->conf) != 0) {
                    ngx_http_finalize_request(r,
                                              500);
                    return -4;
                }
            }
        }

        uri = &u->headers_in.x_accel_redirect->value;
        (&args)->len = 0; (&args)->data = ((void *)0);
        flags = 8;

        if (ngx_http_parse_unsafe_uri(r, uri, &args, &flags) != 0) {
            ngx_http_finalize_request(r, 404);
            return -4;
        }

        if (r->method != 0x0004) {
            r->method = 0x0002;
        }

        ngx_http_internal_redirect(r, uri, &args);
        ngx_http_finalize_request(r, -4);
        return -4;
    }

    part = &u->headers_in.headers.part;
    h = part->elts;

    for (i = 0; ; i++) {

        if (i >= part->nelts) {
            if (part->next == ((void *)0)) {
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        if (ngx_hash_find(&u->conf->hide_headers_hash, h[i].hash,
                          h[i].lowcase_key, h[i].key.len))
        {
            continue;
        }

        hh = ngx_hash_find(&umcf->headers_in_hash, h[i].hash,
                           h[i].lowcase_key, h[i].key.len);

        if (hh) {
            if (hh->copy_handler(r, &h[i], hh->conf) != 0) {
                ngx_http_upstream_finalize_request(r, u,
                                               500);
                return -4;
            }

            continue;
        }

        if (ngx_http_upstream_copy_header_line(r, &h[i], 0) != 0) {
            ngx_http_upstream_finalize_request(r, u,
                                               500);
            return -4;
        }
    }

    if (r->headers_out.server && r->headers_out.server->value.data == ((void *)0)) {
        r->headers_out.server->hash = 0;
    }

    if (r->headers_out.date && r->headers_out.date->value.data == ((void *)0)) {
        r->headers_out.date->hash = 0;
    }

    r->headers_out.status = u->headers_in.status_n;
    r->headers_out.status_line = u->headers_in.status_line;

    r->headers_out.content_length_n = u->headers_in.content_length_n;

    u->length = u->headers_in.content_length_n;

    return 0;
}


static void
ngx_http_upstream_process_body_in_memory(ngx_http_request_t *r,
    ngx_http_upstream_t *u)
{
    size_t size;
    ssize_t n;
    ngx_buf_t *b;
    ngx_event_t *rev;
    ngx_connection_t *c;

    c = u->peer.connection;
    rev = c->read;

    ;


    if (rev->timedout) {
        ngx_connection_error(c, 110, "upstream timed out");
        ngx_http_upstream_finalize_request(r, u, 110);
        return;
    }

    b = &u->buffer;

    for ( ;; ) {

        size = b->end - b->last;

        if (size == 0) {
            if ((c->log)->log_level >= 2) ngx_log_error_core(2, c->log, 0, "upstream buffer is too small to read response");

            ngx_http_upstream_finalize_request(r, u, -1);
            return;
        }

        n = c->recv(c, b->last, size);

        if (n == -2) {
            break;
        }

        if (n == 0 || n == -1) {
            ngx_http_upstream_finalize_request(r, u, n);
            return;
        }

        u->state->response_length += n;

        if (u->input_filter(u->input_filter_ctx, n) == -1) {
            ngx_http_upstream_finalize_request(r, u, -1);
            return;
        }

        if (!rev->ready) {
            break;
        }
    }

    if (u->length == 0) {
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    if (ngx_handle_read_event(rev, 0) != 0) {
        ngx_http_upstream_finalize_request(r, u, -1);
        return;
    }

    if (rev->active) {
        ngx_event_add_timer(rev, u->conf->read_timeout);

    } else if (rev->timer_set) {
        ngx_event_del_timer(rev);
    }
}


static void
ngx_http_upstream_send_response(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    int tcp_nodelay;
    ssize_t n;
    ngx_int_t rc;
    ngx_event_pipe_t *p;
    ngx_connection_t *c;
    ngx_http_core_loc_conf_t *clcf;

    rc = ngx_http_send_header(r);

    if (rc == -1 || rc > 0 || r->post_action) {
        ngx_http_upstream_finalize_request(r, u, rc);
        return;
    }

    c = r->connection;

    if (r->header_only) {

        if (u->cacheable || u->store) {

            if (shutdown(c->fd, SHUT_WR) == -1) {
                ngx_connection_error(c, (*__errno_location ()),
                                     "shutdown()" " failed");
            }

            r->read_event_handler = ngx_http_request_empty_handler;
            r->write_event_handler = ngx_http_request_empty_handler;
            c->error = 1;

        } else {
            ngx_http_upstream_finalize_request(r, u, rc);
            return;
        }
    }

    u->header_sent = 1;

    if (r->request_body && r->request_body->temp_file) {
        ngx_pool_run_cleanup_file(r->pool, r->request_body->temp_file->file.fd);
        r->request_body->temp_file->file.fd = -1;
    }

    clcf = (r)->loc_conf[ngx_http_core_module.ctx_index];

    if (!u->buffering) {

        if (u->input_filter == ((void *)0)) {
            u->input_filter_init = ngx_http_upstream_non_buffered_filter_init;
            u->input_filter = ngx_http_upstream_non_buffered_filter;
            u->input_filter_ctx = r;
        }

        u->read_event_handler = ngx_http_upstream_process_non_buffered_upstream;
        r->write_event_handler =
                             ngx_http_upstream_process_non_buffered_downstream;

        r->limit_rate = 0;

        if (u->input_filter_init(u->input_filter_ctx) == -1) {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }

        if (clcf->tcp_nodelay && c->tcp_nodelay == NGX_TCP_NODELAY_UNSET) {
            ;

            tcp_nodelay = 1;

            if (setsockopt(c->fd, IPPROTO_TCP, 1,
                               (const void *) &tcp_nodelay, sizeof(int)) == -1)
            {
                ngx_connection_error(c, (*__errno_location ()),
                                     "setsockopt(TCP_NODELAY) failed");
                ngx_http_upstream_finalize_request(r, u, 0);
                return;
            }

            c->tcp_nodelay = NGX_TCP_NODELAY_SET;
        }

        n = u->buffer.last - u->buffer.pos;

        if (n) {
            u->buffer.last = u->buffer.pos;

            u->state->response_length += n;

            if (u->input_filter(u->input_filter_ctx, n) == -1) {
                ngx_http_upstream_finalize_request(r, u, 0);
                return;
            }

            ngx_http_upstream_process_non_buffered_downstream(r);

        } else {
            u->buffer.pos = u->buffer.start;
            u->buffer.last = u->buffer.start;

            if (ngx_http_send_special(r, 2) == -1) {
                ngx_http_upstream_finalize_request(r, u, 0);
                return;
            }

            if (u->peer.connection->read->ready || u->length == 0) {
                ngx_http_upstream_process_non_buffered_upstream(r, u);
            }
        }

        return;
    }





    if (r->cache && r->cache->file.fd != -1) {
        ngx_pool_run_cleanup_file(r->pool, r->cache->file.fd);
        r->cache->file.fd = -1;
    }

    switch (ngx_http_test_predicates(r, u->conf->no_cache)) {

    case -1:
        ngx_http_upstream_finalize_request(r, u, 0);
        return;

    case -5:
        u->cacheable = 0;
        break;

    default:

        if (u->cache_status == 2) {

            r->cache->min_uses = u->conf->cache_min_uses;
            r->cache->body_start = u->conf->buffer_size;
            r->cache->file_cache = u->conf->cache->data;

            if (ngx_http_file_cache_create(r) != 0) {
                ngx_http_upstream_finalize_request(r, u, 0);
                return;
            }
        }

        break;
    }

    if (u->cacheable) {
        time_t now, valid;

        now = ngx_cached_time->sec;

        valid = r->cache->valid_sec;

        if (valid == 0) {
            valid = ngx_http_file_cache_valid(u->conf->cache_valid,
                                              u->headers_in.status_n);
            if (valid) {
                r->cache->valid_sec = now + valid;
            }
        }

        if (valid) {
            r->cache->last_modified = r->headers_out.last_modified_time;
            r->cache->date = now;
            r->cache->body_start = (u_short) (u->buffer.pos - u->buffer.start);

            ngx_http_file_cache_set_header(r, u->buffer.start);

        } else {
            u->cacheable = 0;
            r->headers_out.last_modified_time = -1;
        }
    }

    ;


    if (u->cacheable == 0 && r->cache) {
        ngx_http_file_cache_free(r->cache, u->pipe->temp_file);
    }



    p = u->pipe;

    p->output_filter = (ngx_event_pipe_output_filter_pt) ngx_http_output_filter;
    p->output_ctx = r;
    p->tag = u->output.tag;
    p->bufs = u->conf->bufs;
    p->busy_size = u->conf->busy_buffers_size;
    p->upstream = u->peer.connection;
    p->downstream = c;
    p->pool = r->pool;
    p->log = c->log;

    p->cacheable = u->cacheable || u->store;

    p->temp_file = ngx_pcalloc(r->pool, sizeof(ngx_temp_file_t));
    if (p->temp_file == ((void *)0)) {
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    p->temp_file->file.fd = -1;
    p->temp_file->file.log = c->log;
    p->temp_file->path = u->conf->temp_path;
    p->temp_file->pool = r->pool;

    if (p->cacheable) {
        p->temp_file->persistent = 1;

    } else {
        p->temp_file->log_level = 5;
        p->temp_file->warn = "an upstream response is buffered "
                             "to a temporary file";
    }

    p->max_temp_file_size = u->conf->max_temp_file_size;
    p->temp_file_write_size = u->conf->temp_file_write_size;

    p->preread_bufs = ngx_alloc_chain_link(r->pool);
    if (p->preread_bufs == ((void *)0)) {
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    p->preread_bufs->buf = &u->buffer;
    p->preread_bufs->next = ((void *)0);
    u->buffer.recycled = 1;

    p->preread_size = u->buffer.last - u->buffer.pos;

    if (u->cacheable) {

        p->buf_to_file = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
        if (p->buf_to_file == ((void *)0)) {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }

        p->buf_to_file->pos = u->buffer.start;
        p->buf_to_file->last = u->buffer.pos;
        p->buf_to_file->temporary = 1;
    }

    if (ngx_event_flags & 0x00000100) {

        p->single_buf = 1;
    }


    p->free_bufs = 1;





    u->buffer.last = u->buffer.pos;

    if (u->conf->cyclic_temp_file) {







        p->cyclic_temp_file = 1;
        c->sendfile = 0;

    } else {
        p->cyclic_temp_file = 0;
    }

    p->read_timeout = u->conf->read_timeout;
    p->send_timeout = clcf->send_timeout;
    p->send_lowat = clcf->send_lowat;

    p->length = -1;

    if (u->input_filter_init
        && u->input_filter_init(p->input_ctx) != 0)
    {
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    u->read_event_handler = ngx_http_upstream_process_upstream;
    r->write_event_handler = ngx_http_upstream_process_downstream;

    ngx_http_upstream_process_upstream(r, u);
}


static void
ngx_http_upstream_process_non_buffered_downstream(ngx_http_request_t *r)
{
    ngx_event_t *wev;
    ngx_connection_t *c;
    ngx_http_upstream_t *u;

    c = r->connection;
    u = r->upstream;
    wev = c->write;

    ;


    c->log->action = "sending to client";

    if (wev->timedout) {
        c->timedout = 1;
        ngx_connection_error(c, 110, "client timed out");
        ngx_http_upstream_finalize_request(r, u, 408);
        return;
    }

    ngx_http_upstream_process_non_buffered_request(r, 1);
}


static void
ngx_http_upstream_process_non_buffered_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u)
{
    ngx_connection_t *c;

    c = u->peer.connection;

    ;


    c->log->action = "reading upstream";

    if (c->read->timedout) {
        ngx_connection_error(c, 110, "upstream timed out");
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    ngx_http_upstream_process_non_buffered_request(r, 0);
}


static void
ngx_http_upstream_process_non_buffered_request(ngx_http_request_t *r,
    ngx_uint_t do_write)
{
    size_t size;
    ssize_t n;
    ngx_buf_t *b;
    ngx_int_t rc;
    ngx_connection_t *downstream, *upstream;
    ngx_http_upstream_t *u;
    ngx_http_core_loc_conf_t *clcf;

    u = r->upstream;
    downstream = r->connection;
    upstream = u->peer.connection;

    b = &u->buffer;

    do_write = do_write || u->length == 0;

    for ( ;; ) {

        if (do_write) {

            if (u->out_bufs || u->busy_bufs) {
                rc = ngx_http_output_filter(r, u->out_bufs);

                if (rc == -1) {
                    ngx_http_upstream_finalize_request(r, u, 0);
                    return;
                }

                ngx_chain_update_chains(r->pool, &u->free_bufs, &u->busy_bufs,
                                        &u->out_bufs, u->output.tag);
            }

            if (u->busy_bufs == ((void *)0)) {

                if (u->length == 0
                    || upstream->read->eof
                    || upstream->read->error)
                {
                    ngx_http_upstream_finalize_request(r, u, 0);
                    return;
                }

                b->pos = b->start;
                b->last = b->start;
            }
        }

        size = b->end - b->last;

        if (size && upstream->read->ready) {

            n = upstream->recv(upstream, b->last, size);

            if (n == -2) {
                break;
            }

            if (n > 0) {
                u->state->response_length += n;

                if (u->input_filter(u->input_filter_ctx, n) == -1) {
                    ngx_http_upstream_finalize_request(r, u, 0);
                    return;
                }
            }

            do_write = 1;

            continue;
        }

        break;
    }

    clcf = (r)->loc_conf[ngx_http_core_module.ctx_index];

    if (downstream->data == r) {
        if (ngx_handle_write_event(downstream->write, clcf->send_lowat)
            != 0)
        {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }
    }

    if (downstream->write->active && !downstream->write->ready) {
        ngx_event_add_timer(downstream->write, clcf->send_timeout);

    } else if (downstream->write->timer_set) {
        ngx_event_del_timer(downstream->write);
    }

    if (ngx_handle_read_event(upstream->read, 0) != 0) {
        ngx_http_upstream_finalize_request(r, u, 0);
        return;
    }

    if (upstream->read->active && !upstream->read->ready) {
        ngx_event_add_timer(upstream->read, u->conf->read_timeout);

    } else if (upstream->read->timer_set) {
        ngx_event_del_timer(upstream->read);
    }
}


static ngx_int_t
ngx_http_upstream_non_buffered_filter_init(void *data)
{
    return 0;
}


static ngx_int_t
ngx_http_upstream_non_buffered_filter(void *data, ssize_t bytes)
{
    ngx_http_request_t *r = data;

    ngx_buf_t *b;
    ngx_chain_t *cl, **ll;
    ngx_http_upstream_t *u;

    u = r->upstream;

    for (cl = u->out_bufs, ll = &u->out_bufs; cl; cl = cl->next) {
        ll = &cl->next;
    }

    cl = ngx_chain_get_free_buf(r->pool, &u->free_bufs);
    if (cl == ((void *)0)) {
        return -1;
    }

    *ll = cl;

    cl->buf->flush = 1;
    cl->buf->memory = 1;

    b = &u->buffer;

    cl->buf->pos = b->last;
    b->last += bytes;
    cl->buf->last = b->last;
    cl->buf->tag = u->output.tag;

    if (u->length == -1) {
        return 0;
    }

    u->length -= bytes;

    return 0;
}


static void
ngx_http_upstream_process_downstream(ngx_http_request_t *r)
{
    ngx_event_t *wev;
    ngx_connection_t *c;
    ngx_event_pipe_t *p;
    ngx_http_upstream_t *u;

    c = r->connection;
    u = r->upstream;
    p = u->pipe;
    wev = c->write;

    ;


    c->log->action = "sending to client";

    if (wev->timedout) {

        if (wev->delayed) {

            wev->timedout = 0;
            wev->delayed = 0;

            if (!wev->ready) {
                ngx_event_add_timer(wev, p->send_timeout);

                if (ngx_handle_write_event(wev, p->send_lowat) != 0) {
                    ngx_http_upstream_finalize_request(r, u, 0);
                }

                return;
            }

            if (ngx_event_pipe(p, wev->write) == -6) {
                ngx_http_upstream_finalize_request(r, u, 0);
                return;
            }

        } else {
            p->downstream_error = 1;
            c->timedout = 1;
            ngx_connection_error(c, 110, "client timed out");
        }

    } else {

        if (wev->delayed) {

            ;


            if (ngx_handle_write_event(wev, p->send_lowat) != 0) {
                ngx_http_upstream_finalize_request(r, u, 0);
            }

            return;
        }

        if (ngx_event_pipe(p, 1) == -6) {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }
    }

    ngx_http_upstream_process_request(r);
}


static void
ngx_http_upstream_process_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u)
{
    ngx_connection_t *c;

    c = u->peer.connection;

    ;


    c->log->action = "reading upstream";

    if (c->read->timedout) {
        u->pipe->upstream_error = 1;
        ngx_connection_error(c, 110, "upstream timed out");

    } else {
        if (ngx_event_pipe(u->pipe, 0) == -6) {
            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }
    }

    ngx_http_upstream_process_request(r);
}


static void
ngx_http_upstream_process_request(ngx_http_request_t *r)
{
    ngx_temp_file_t *tf;
    ngx_event_pipe_t *p;
    ngx_http_upstream_t *u;

    u = r->upstream;
    p = u->pipe;

    if (u->peer.connection) {

        if (u->store) {

            if (p->upstream_eof || p->upstream_done) {

                tf = u->pipe->temp_file;

                if (u->headers_in.status_n == 200
                    && (u->headers_in.content_length_n == -1
                        || (u->headers_in.content_length_n == tf->offset)))
                {
                    ngx_http_upstream_store(r, u);
                    u->store = 0;
                }
            }
        }



        if (u->cacheable) {

            if (p->upstream_done) {
                ngx_http_file_cache_update(r, u->pipe->temp_file);

            } else if (p->upstream_eof) {

                tf = u->pipe->temp_file;

                if (u->headers_in.content_length_n == -1
                    || u->headers_in.content_length_n
                       == tf->offset - (off_t) r->cache->body_start)
                {
                    ngx_http_file_cache_update(r, tf);

                } else {
                    ngx_http_file_cache_free(r->cache, tf);
                }

            } else if (p->upstream_error) {
                ngx_http_file_cache_free(r->cache, u->pipe->temp_file);
            }
        }



        if (p->upstream_done || p->upstream_eof || p->upstream_error) {
            ;




            ngx_http_upstream_finalize_request(r, u, 0);
            return;
        }
    }

    if (p->downstream_error) {
        ;


        if (!u->cacheable && !u->store && u->peer.connection) {
            ngx_http_upstream_finalize_request(r, u, 0);
        }
    }
}


static void
ngx_http_upstream_store(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    size_t root;
    time_t lm;
    ngx_str_t path;
    ngx_temp_file_t *tf;
    ngx_ext_rename_file_t ext;

    tf = u->pipe->temp_file;

    if (tf->file.fd == -1) {



        tf = ngx_pcalloc(r->pool, sizeof(ngx_temp_file_t));
        if (tf == ((void *)0)) {
            return;
        }

        tf->file.fd = -1;
        tf->file.log = r->connection->log;
        tf->path = u->conf->temp_path;
        tf->pool = r->pool;
        tf->persistent = 1;

        if (ngx_create_temp_file(&tf->file, tf->path, tf->pool,
                                 tf->persistent, tf->clean, tf->access)
            != 0)
        {
            return;
        }

        u->pipe->temp_file = tf;
    }

    ext.access = u->conf->store_access;
    ext.path_access = u->conf->store_access;
    ext.time = -1;
    ext.create_path = 1;
    ext.delete_file = 1;
    ext.log = r->connection->log;

    if (u->headers_in.last_modified) {

        lm = ngx_http_parse_time(u->headers_in.last_modified->value.data,
                                 u->headers_in.last_modified->value.len);

        if (lm != -1) {
            ext.time = lm;
            ext.fd = tf->file.fd;
        }
    }

    if (u->conf->store_lengths == ((void *)0)) {

        ngx_http_map_uri_to_path(r, &path, &root, 0);

    } else {
        if (ngx_http_script_run(r, &path, u->conf->store_lengths->elts, 0,
                                u->conf->store_values->elts)
            == ((void *)0))
        {
            return;
        }
    }

    path.len--;

    ;



    (void) ngx_ext_rename_file(&tf->file.name, &path, &ext);
}


static void
ngx_http_upstream_dummy_handler(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ;

}


static void
ngx_http_upstream_next(ngx_http_request_t *r, ngx_http_upstream_t *u,
    ngx_uint_t ft_type)
{
    ngx_uint_t status, state;

    ;






    if (ft_type == 0x00000100) {
        state = 2;
    } else {
        state = 4;
    }

    if (ft_type != 0x40000000) {
        u->peer.free(&u->peer, u->peer.data, state);
    }

    if (ft_type == 0x00000004) {
        if ((r->connection->log)->log_level >= 4) ngx_log_error_core(4, r->connection->log, 110, "upstream timed out");

    }

    if (u->peer.cached && ft_type == 0x00000002) {
        status = 0;



        u->peer.tries++;

    } else {
        switch(ft_type) {

        case 0x00000004:
            status = 504;
            break;

        case 0x00000010:
            status = 500;
            break;

        case 0x00000100:
            status = 404;
            break;






        default:
            status = 502;
        }
    }

    if (r->connection->error) {
        ngx_http_upstream_finalize_request(r, u,
                                           499);
        return;
    }

    if (status) {
        u->state->status = status;

        if (u->peer.tries == 0 || !(u->conf->next_upstream & ft_type)) {



            if (u->cache_status == 3
                && (u->conf->cache_use_stale & ft_type))
            {
                ngx_int_t rc;

                rc = u->reinit_request(r);

                if (rc == 0) {
                    u->cache_status = 4;
                    rc = ngx_http_upstream_cache_send(r, u);
                }

                ngx_http_upstream_finalize_request(r, u, rc);
                return;
            }


            ngx_http_upstream_finalize_request(r, u, status);
            return;
        }
    }

    if (u->peer.connection) {
        ;
# 2923 "src/http/ngx_http_upstream.c"
        if (u->peer.connection->pool) {
            ngx_destroy_pool(u->peer.connection->pool);
        }

        ngx_close_connection(u->peer.connection);
        u->peer.connection = ((void *)0);
    }
# 2938 "src/http/ngx_http_upstream.c"
    ngx_http_upstream_connect(r, u);
}


static void
ngx_http_upstream_cleanup(void *data)
{
    ngx_http_request_t *r = data;

    ngx_http_upstream_t *u;

    ;


    u = r->upstream;

    if (u->resolved && u->resolved->ctx) {
        ngx_resolve_name_done(u->resolved->ctx);
        u->resolved->ctx = ((void *)0);
    }

    ngx_http_upstream_finalize_request(r, u, -4);
}


static void
ngx_http_upstream_finalize_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_int_t rc)
{
    ngx_time_t *tp;

    ;


    if (u->cleanup) {
        *u->cleanup = ((void *)0);
        u->cleanup = ((void *)0);
    }

    if (u->resolved && u->resolved->ctx) {
        ngx_resolve_name_done(u->resolved->ctx);
        u->resolved->ctx = ((void *)0);
    }

    if (u->state && u->state->response_sec) {
        tp = (ngx_time_t *) ngx_cached_time;
        u->state->response_sec = tp->sec - u->state->response_sec;
        u->state->response_msec = tp->msec - u->state->response_msec;

        if (u->pipe) {
            u->state->response_length = u->pipe->read_length;
        }
    }

    u->finalize_request(r, rc);

    if (u->peer.free) {
        u->peer.free(&u->peer, u->peer.data, 0);
    }

    if (u->peer.connection) {
# 3018 "src/http/ngx_http_upstream.c"
        ;



        if (u->peer.connection->pool) {
            ngx_destroy_pool(u->peer.connection->pool);
        }

        ngx_close_connection(u->peer.connection);
    }

    u->peer.connection = ((void *)0);

    if (u->pipe && u->pipe->temp_file) {
        ;


    }

    if (u->store && u->pipe && u->pipe->temp_file
        && u->pipe->temp_file->file.fd != -1)
    {
        if (unlink((const char *) u->pipe->temp_file->file.name.data)
            == -1)
        {
            if ((r->connection->log)->log_level >= 3) ngx_log_error_core(3, r->connection->log, (*__errno_location ()), "unlink()" " \"%s\" failed", u->pipe->temp_file->file.name.data);


        }
    }



    if (r->cache) {

        if (u->cacheable) {

            if (rc == 502 || rc == 504) {
                time_t valid;

                valid = ngx_http_file_cache_valid(u->conf->cache_valid, rc);

                if (valid) {
                    r->cache->valid_sec = ngx_cached_time->sec + valid;
                    r->cache->error = rc;
                }
            }
        }

        ngx_http_file_cache_free(r->cache, u->pipe->temp_file);
    }



    if (u->header_sent
        && rc != 408
        && (rc == -1 || rc >= 300))
    {
        rc = 0;
    }

    if (rc == -5) {
        return;
    }

    r->connection->log->action = "sending to client";

    if (rc == 0

        && !r->cached

       )
    {
        rc = ngx_http_send_special(r, 1);
    }

    ngx_http_finalize_request(r, rc);
}


static ngx_int_t
ngx_http_upstream_process_header_line(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_table_elt_t **ph;

    ph = (ngx_table_elt_t **) ((char *) &r->upstream->headers_in + offset);

    if (*ph == ((void *)0)) {
        *ph = h;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_ignore_header_line(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    return 0;
}


static ngx_int_t
ngx_http_upstream_process_content_length(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_http_upstream_t *u;

    u = r->upstream;

    u->headers_in.content_length = h;
    u->headers_in.content_length_n = ngx_atoof(h->value.data, h->value.len);

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_set_cookie(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{

    ngx_http_upstream_t *u;

    u = r->upstream;

    if (!(u->conf->ignore_headers & 0x00000020)) {
        u->cacheable = 0;
    }


    return 0;
}


static ngx_int_t
ngx_http_upstream_process_cache_control(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_array_t *pa;
    ngx_table_elt_t **ph;
    ngx_http_upstream_t *u;

    u = r->upstream;
    pa = &u->headers_in.cache_control;

    if (pa->elts == ((void *)0)) {
       if (ngx_array_init(pa, r->pool, 2, sizeof(ngx_table_elt_t *)) != 0)
       {
           return -1;
       }
    }

    ph = ngx_array_push(pa);
    if (ph == ((void *)0)) {
        return -1;
    }

    *ph = h;


    {
    u_char *p, *last;
    ngx_int_t n;

    if (u->conf->ignore_headers & 0x00000010) {
        return 0;
    }

    if (r->cache == ((void *)0)) {
        return 0;
    }

    if (r->cache->valid_sec != 0) {
        return 0;
    }

    p = h->value.data;
    last = p + h->value.len;

    if (ngx_strlcasestrn(p, last, (u_char *) "no-cache", 8 - 1) != ((void *)0)
        || ngx_strlcasestrn(p, last, (u_char *) "no-store", 8 - 1) != ((void *)0)
        || ngx_strlcasestrn(p, last, (u_char *) "private", 7 - 1) != ((void *)0))
    {
        u->cacheable = 0;
        return 0;
    }

    p = ngx_strlcasestrn(p, last, (u_char *) "max-age=", 8 - 1);

    if (p == ((void *)0)) {
        return 0;
    }

    n = 0;

    for (p += 8; p < last; p++) {
        if (*p == ',' || *p == ';' || *p == ' ') {
            break;
        }

        if (*p >= '0' && *p <= '9') {
            n = n * 10 + *p - '0';
            continue;
        }

        u->cacheable = 0;
        return 0;
    }

    if (n == 0) {
        u->cacheable = 0;
        return 0;
    }

    r->cache->valid_sec = ngx_cached_time->sec + n;
    }


    return 0;
}


static ngx_int_t
ngx_http_upstream_process_expires(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_http_upstream_t *u;

    u = r->upstream;
    u->headers_in.expires = h;


    {
    time_t expires;

    if (u->conf->ignore_headers & 0x00000008) {
        return 0;
    }

    if (r->cache == ((void *)0)) {
        return 0;
    }

    if (r->cache->valid_sec != 0) {
        return 0;
    }

    expires = ngx_http_parse_time(h->value.data, h->value.len);

    if (expires == -1 || expires < ngx_cached_time->sec) {
        u->cacheable = 0;
        return 0;
    }

    r->cache->valid_sec = expires;
    }


    return 0;
}


static ngx_int_t
ngx_http_upstream_process_accel_expires(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_http_upstream_t *u;

    u = r->upstream;
    u->headers_in.x_accel_expires = h;


    {
    u_char *p;
    size_t len;
    ngx_int_t n;

    if (u->conf->ignore_headers & 0x00000004) {
        return 0;
    }

    if (r->cache == ((void *)0)) {
        return 0;
    }

    len = h->value.len;
    p = h->value.data;

    if (p[0] != '@') {
        n = ngx_atoi(p, len);

        switch (n) {
        case 0:
            u->cacheable = 0;


        case -1:
            return 0;

        default:
            r->cache->valid_sec = ngx_cached_time->sec + n;
            return 0;
        }
    }

    p++;
    len--;

    n = ngx_atoi(p, len);

    if (n != -1) {
        r->cache->valid_sec = n;
    }
    }


    return 0;
}


static ngx_int_t
ngx_http_upstream_process_limit_rate(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_int_t n;
    ngx_http_upstream_t *u;

    u = r->upstream;
    u->headers_in.x_accel_limit_rate = h;

    if (u->conf->ignore_headers & 0x00000040) {
        return 0;
    }

    n = ngx_atoi(h->value.data, h->value.len);

    if (n != -1) {
        r->limit_rate = (size_t) n;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_buffering(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    u_char c0, c1, c2;
    ngx_http_upstream_t *u;

    u = r->upstream;

    if (u->conf->ignore_headers & 0x00000080) {
        return 0;
    }

    if (u->conf->change_buffering) {

        if (h->value.len == 2) {
            c0 = (u_char) ((h->value.data[0] >= 'A' && h->value.data[0] <= 'Z') ? (h->value.data[0] | 0x20) : h->value.data[0]);
            c1 = (u_char) ((h->value.data[1] >= 'A' && h->value.data[1] <= 'Z') ? (h->value.data[1] | 0x20) : h->value.data[1]);

            if (c0 == 'n' && c1 == 'o') {
                u->buffering = 0;
            }

        } else if (h->value.len == 3) {
            c0 = (u_char) ((h->value.data[0] >= 'A' && h->value.data[0] <= 'Z') ? (h->value.data[0] | 0x20) : h->value.data[0]);
            c1 = (u_char) ((h->value.data[1] >= 'A' && h->value.data[1] <= 'Z') ? (h->value.data[1] | 0x20) : h->value.data[1]);
            c2 = (u_char) ((h->value.data[2] >= 'A' && h->value.data[2] <= 'Z') ? (h->value.data[2] | 0x20) : h->value.data[2]);

            if (c0 == 'y' && c1 == 'e' && c2 == 's') {
                u->buffering = 1;
            }
        }
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_charset(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    if (r->upstream->conf->ignore_headers & 0x00000100) {
        return 0;
    }

    r->headers_out.override_charset = &h->value;

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_connection(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    r->upstream->headers_in.connection = h;

    if (ngx_strlcasestrn(h->value.data, h->value.data + h->value.len,
                         (u_char *) "close", 5 - 1)
        != ((void *)0))
    {
        r->upstream->headers_in.connection_close = 1;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_process_transfer_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    r->upstream->headers_in.transfer_encoding = h;

    if (ngx_strlcasestrn(h->value.data, h->value.data + h->value.len,
                         (u_char *) "chunked", 7 - 1)
        != ((void *)0))
    {
        r->upstream->headers_in.chunked = 1;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_copy_header_line(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_table_elt_t *ho, **ph;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    if (offset) {
        ph = (ngx_table_elt_t **) ((char *) &r->headers_out + offset);
        *ph = ho;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_copy_multi_header_lines(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_array_t *pa;
    ngx_table_elt_t *ho, **ph;

    pa = (ngx_array_t *) ((char *) &r->headers_out + offset);

    if (pa->elts == ((void *)0)) {
        if (ngx_array_init(pa, r->pool, 2, sizeof(ngx_table_elt_t *)) != 0)
        {
            return -1;
        }
    }

    ph = ngx_array_push(pa);
    if (ph == ((void *)0)) {
        return -1;
    }

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;
    *ph = ho;

    return 0;
}


static ngx_int_t
ngx_http_upstream_copy_content_type(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    u_char *p, *last;

    r->headers_out.content_type_len = h->value.len;
    r->headers_out.content_type = h->value;
    r->headers_out.content_type_lowcase = ((void *)0);

    for (p = h->value.data; *p; p++) {

        if (*p != ';') {
            continue;
        }

        last = p;

        while (*++p == ' ') { }

        if (*p == '\0') {
            return 0;
        }

        if (ngx_strncasecmp(p, (u_char *) "charset=", 8) != 0) {
            continue;
        }

        p += 8;

        r->headers_out.content_type_len = last - h->value.data;

        if (*p == '"') {
            p++;
        }

        last = h->value.data + h->value.len;

        if (*(last - 1) == '"') {
            last--;
        }

        r->headers_out.charset.len = last - p;
        r->headers_out.charset.data = p;

        return 0;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_copy_last_modified(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_table_elt_t *ho;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    r->headers_out.last_modified = ho;



    if (r->upstream->cacheable) {
        r->headers_out.last_modified_time = ngx_http_parse_time(h->value.data,
                                                                h->value.len);
    }



    return 0;
}


static ngx_int_t
ngx_http_upstream_rewrite_location(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_int_t rc;
    ngx_table_elt_t *ho;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    if (r->upstream->rewrite_redirect) {
        rc = r->upstream->rewrite_redirect(r, ho, 0);

        if (rc == -5) {
            return 0;
        }

        if (rc == 0) {
            r->headers_out.location = ho;

            ;

        }

        return rc;
    }

    if (ho->value.data[0] != '/') {
        r->headers_out.location = ho;
    }






    return 0;
}


static ngx_int_t
ngx_http_upstream_rewrite_refresh(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    u_char *p;
    ngx_int_t rc;
    ngx_table_elt_t *ho;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    if (r->upstream->rewrite_redirect) {

        p = ngx_strcasestrn(ho->value.data, "url=", 4 - 1);

        if (p) {
            rc = r->upstream->rewrite_redirect(r, ho, p + 4 - ho->value.data);

        } else {
            return 0;
        }

        if (rc == -5) {
            return 0;
        }

        if (rc == 0) {
            r->headers_out.refresh = ho;

            ;

        }

        return rc;
    }

    r->headers_out.refresh = ho;

    return 0;
}


static ngx_int_t
ngx_http_upstream_rewrite_set_cookie(ngx_http_request_t *r, ngx_table_elt_t *h,
    ngx_uint_t offset)
{
    ngx_int_t rc;
    ngx_table_elt_t *ho;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    if (r->upstream->rewrite_cookie) {
        rc = r->upstream->rewrite_cookie(r, ho);

        if (rc == -5) {
            return 0;
        }
# 3704 "src/http/ngx_http_upstream.c"
        return rc;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_copy_allow_ranges(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_table_elt_t *ho;



    if (r->cached) {
        r->allow_ranges = 1;
        return 0;

    }



    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    r->headers_out.accept_ranges = ho;

    return 0;
}




static ngx_int_t
ngx_http_upstream_copy_content_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset)
{
    ngx_table_elt_t *ho;

    ho = ngx_list_push(&r->headers_out.headers);
    if (ho == ((void *)0)) {
        return -1;
    }

    *ho = *h;

    r->headers_out.content_encoding = ho;

    return 0;
}




static ngx_int_t
ngx_http_upstream_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t *var, *v;

    for (v = ngx_http_upstream_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == ((void *)0)) {
            return -1;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return 0;
}


static ngx_int_t
ngx_http_upstream_addr_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;
    size_t len;
    ngx_uint_t i;
    ngx_http_upstream_state_t *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == ((void *)0) || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return 0;
    }

    len = 0;
    state = r->upstream_states->elts;

    for (i = 0; i < r->upstream_states->nelts; i++) {
        if (state[i].peer) {
            len += state[i].peer->len + 2;

        } else {
            len += 3;
        }
    }

    p = ngx_pnalloc(r->pool, len);
    if (p == ((void *)0)) {
        return -1;
    }

    v->data = p;

    i = 0;

    for ( ;; ) {
        if (state[i].peer) {
            p = (((u_char *) memcpy(p, state[i].peer->data, state[i].peer->len)) + (state[i].peer->len));
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return 0;
}


static ngx_int_t
ngx_http_upstream_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;
    size_t len;
    ngx_uint_t i;
    ngx_http_upstream_state_t *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == ((void *)0) || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return 0;
    }

    len = r->upstream_states->nelts * (3 + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == ((void *)0)) {
        return -1;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    for ( ;; ) {
        if (state[i].status) {
            p = ngx_sprintf(p, "%ui", state[i].status);

        } else {
            *p++ = '-';
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return 0;
}


static ngx_int_t
ngx_http_upstream_response_time_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;
    size_t len;
    ngx_uint_t i;
    ngx_msec_int_t ms;
    ngx_http_upstream_state_t *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == ((void *)0) || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return 0;
    }

    len = r->upstream_states->nelts * ((sizeof("-9223372036854775808") - 1) + 4 + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == ((void *)0)) {
        return -1;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    for ( ;; ) {
        if (state[i].status) {
            ms = (ngx_msec_int_t)
                     (state[i].response_sec * 1000 + state[i].response_msec);
            ms = ((ms < 0) ? (0) : (ms));
            p = ngx_sprintf(p, "%d.%03d", ms / 1000, ms % 1000);

        } else {
            *p++ = '-';
        }

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return 0;
}


static ngx_int_t
ngx_http_upstream_response_length_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *p;
    size_t len;
    ngx_uint_t i;
    ngx_http_upstream_state_t *state;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    if (r->upstream_states == ((void *)0) || r->upstream_states->nelts == 0) {
        v->not_found = 1;
        return 0;
    }

    len = r->upstream_states->nelts * ((sizeof("-9223372036854775808") - 1) + 2);

    p = ngx_pnalloc(r->pool, len);
    if (p == ((void *)0)) {
        return -1;
    }

    v->data = p;

    i = 0;
    state = r->upstream_states->elts;

    for ( ;; ) {
        p = ngx_sprintf(p, "%O", state[i].response_length);

        if (++i == r->upstream_states->nelts) {
            break;
        }

        if (state[i].peer) {
            *p++ = ',';
            *p++ = ' ';

        } else {
            *p++ = ' ';
            *p++ = ':';
            *p++ = ' ';

            if (++i == r->upstream_states->nelts) {
                break;
            }

            continue;
        }
    }

    v->len = p - v->data;

    return 0;
}


ngx_int_t
ngx_http_upstream_header_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->upstream == ((void *)0)) {
        v->not_found = 1;
        return 0;
    }

    return ngx_http_variable_unknown_header(v, (ngx_str_t *) data,
                                         &r->upstream->headers_in.headers.part,
                                         sizeof("upstream_http_") - 1);
}




ngx_int_t
ngx_http_upstream_cache_status(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_uint_t n;

    if (r->upstream == ((void *)0) || r->upstream->cache_status == 0) {
        v->not_found = 1;
        return 0;
    }

    n = r->upstream->cache_status - 1;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->len = ngx_http_cache_status[n].len;
    v->data = ngx_http_cache_status[n].data;

    return 0;
}




static char *
ngx_http_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *dummy)
{
    char *rv;
    void *mconf;
    ngx_str_t *value;
    ngx_url_t u;
    ngx_uint_t m;
    ngx_conf_t pcf;
    ngx_http_module_t *module;
    ngx_http_conf_ctx_t *ctx, *http_ctx;
    ngx_http_upstream_srv_conf_t *uscf;

    (void) memset(&u, 0, sizeof(ngx_url_t));

    value = cf->args->elts;
    u.host = value[1];
    u.no_resolve = 1;

    uscf = ngx_http_upstream_add(cf, &u, 0x0001
                                         |0x0002
                                         |0x0004
                                         |0x0008
                                         |0x0010
                                         |0x0020);
    if (uscf == ((void *)0)) {
        return (void *) -1;
    }


    ctx = ngx_pcalloc(cf->pool, sizeof(ngx_http_conf_ctx_t));
    if (ctx == ((void *)0)) {
        return (void *) -1;
    }

    http_ctx = cf->ctx;
    ctx->main_conf = http_ctx->main_conf;



    ctx->srv_conf = ngx_pcalloc(cf->pool, sizeof(void *) * ngx_http_max_module);
    if (ctx->srv_conf == ((void *)0)) {
        return (void *) -1;
    }

    ctx->srv_conf[ngx_http_upstream_module.ctx_index] = uscf;

    uscf->srv_conf = ctx->srv_conf;




    ctx->loc_conf = ngx_pcalloc(cf->pool, sizeof(void *) * ngx_http_max_module);
    if (ctx->loc_conf == ((void *)0)) {
        return (void *) -1;
    }

    for (m = 0; ngx_modules[m]; m++) {
        if (ngx_modules[m]->type != 0x50545448) {
            continue;
        }

        module = ngx_modules[m]->ctx;

        if (module->create_srv_conf) {
            mconf = module->create_srv_conf(cf);
            if (mconf == ((void *)0)) {
                return (void *) -1;
            }

            ctx->srv_conf[ngx_modules[m]->ctx_index] = mconf;
        }

        if (module->create_loc_conf) {
            mconf = module->create_loc_conf(cf);
            if (mconf == ((void *)0)) {
                return (void *) -1;
            }

            ctx->loc_conf[ngx_modules[m]->ctx_index] = mconf;
        }
    }




    pcf = *cf;
    cf->ctx = ctx;
    cf->cmd_type = 0x10000000;

    rv = ngx_conf_parse(cf, ((void *)0));

    *cf = pcf;

    if (rv != ((void *)0)) {
        return rv;
    }

    if (uscf->servers == ((void *)0)) {
        ngx_conf_log_error(1, cf, 0,
                           "no servers are inside upstream");
        return (void *) -1;
    }

    return rv;
}


static char *
ngx_http_upstream_server(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_upstream_srv_conf_t *uscf = conf;

    time_t fail_timeout;
    ngx_str_t *value, s;
    ngx_url_t u;
    ngx_int_t weight, max_fails;
    ngx_uint_t i;
    ngx_http_upstream_server_t *us;

    if (uscf->servers == ((void *)0)) {
        uscf->servers = ngx_array_create(cf->pool, 4,
                                         sizeof(ngx_http_upstream_server_t));
        if (uscf->servers == ((void *)0)) {
            return (void *) -1;
        }
    }

    us = ngx_array_push(uscf->servers);
    if (us == ((void *)0)) {
        return (void *) -1;
    }

    (void) memset(us, 0, sizeof(ngx_http_upstream_server_t));

    value = cf->args->elts;

    (void) memset(&u, 0, sizeof(ngx_url_t));

    u.url = value[1];
    u.default_port = 80;

    if (ngx_parse_url(cf->pool, &u) != 0) {
        if (u.err) {
            ngx_conf_log_error(1, cf, 0,
                               "%s in upstream \"%V\"", u.err, &u.url);
        }

        return (void *) -1;
    }

    weight = 1;
    max_fails = 1;
    fail_timeout = 10;

    for (i = 2; i < cf->args->nelts; i++) {

        if (strncmp((const char *) value[i].data, (const char *) "weight=", 7) == 0) {

            if (!(uscf->flags & 0x0002)) {
                goto invalid;
            }

            weight = ngx_atoi(&value[i].data[7], value[i].len - 7);

            if (weight == -1 || weight == 0) {
                goto invalid;
            }

            continue;
        }

        if (strncmp((const char *) value[i].data, (const char *) "max_fails=", 10) == 0) {

            if (!(uscf->flags & 0x0004)) {
                goto invalid;
            }

            max_fails = ngx_atoi(&value[i].data[10], value[i].len - 10);

            if (max_fails == -1) {
                goto invalid;
            }

            continue;
        }

        if (strncmp((const char *) value[i].data, (const char *) "fail_timeout=", 13) == 0) {

            if (!(uscf->flags & 0x0008)) {
                goto invalid;
            }

            s.len = value[i].len - 13;
            s.data = &value[i].data[13];

            fail_timeout = ngx_parse_time(&s, 1);

            if (fail_timeout == (time_t) -1) {
                goto invalid;
            }

            continue;
        }

        if (strncmp((const char *) value[i].data, (const char *) "backup", 6) == 0) {

            if (!(uscf->flags & 0x0020)) {
                goto invalid;
            }

            us->backup = 1;

            continue;
        }

        if (strncmp((const char *) value[i].data, (const char *) "down", 4) == 0) {

            if (!(uscf->flags & 0x0010)) {
                goto invalid;
            }

            us->down = 1;

            continue;
        }

        goto invalid;
    }

    us->addrs = u.addrs;
    us->naddrs = u.naddrs;
    us->weight = weight;
    us->max_fails = max_fails;
    us->fail_timeout = fail_timeout;

    return ((void *)0);

invalid:

    ngx_conf_log_error(1, cf, 0,
                       "invalid parameter \"%V\"", &value[i]);

    return (void *) -1;
}


ngx_http_upstream_srv_conf_t *
ngx_http_upstream_add(ngx_conf_t *cf, ngx_url_t *u, ngx_uint_t flags)
{
    ngx_uint_t i;
    ngx_http_upstream_server_t *us;
    ngx_http_upstream_srv_conf_t *uscf, **uscfp;
    ngx_http_upstream_main_conf_t *umcf;

    if (!(flags & 0x0001)) {

        if (ngx_parse_url(cf->pool, u) != 0) {
            if (u->err) {
                ngx_conf_log_error(1, cf, 0,
                                   "%s in upstream \"%V\"", u->err, &u->url);
            }

            return ((void *)0);
        }
    }

    umcf = ((ngx_http_conf_ctx_t *) cf->ctx)->main_conf[ngx_http_upstream_module.ctx_index];

    uscfp = umcf->upstreams.elts;

    for (i = 0; i < umcf->upstreams.nelts; i++) {

        if (uscfp[i]->host.len != u->host.len
            || ngx_strncasecmp(uscfp[i]->host.data, u->host.data, u->host.len)
               != 0)
        {
            continue;
        }

        if ((flags & 0x0001)
             && (uscfp[i]->flags & 0x0001))
        {
            ngx_conf_log_error(1, cf, 0,
                               "duplicate upstream \"%V\"", &u->host);
            return ((void *)0);
        }

        if ((uscfp[i]->flags & 0x0001) && u->port) {
            ngx_conf_log_error(5, cf, 0,
                               "upstream \"%V\" may not have port %d",
                               &u->host, u->port);
            return ((void *)0);
        }

        if ((flags & 0x0001) && uscfp[i]->port) {
            if ((cf->log)->log_level >= 5) ngx_log_error_core(5, cf->log, 0, "upstream \"%V\" may not have port %d in %s:%ui", &u->host, uscfp[i]->port, uscfp[i]->file_name, uscfp[i]->line);



            return ((void *)0);
        }

        if (uscfp[i]->port != u->port) {
            continue;
        }

        if (uscfp[i]->default_port && u->default_port
            && uscfp[i]->default_port != u->default_port)
        {
            continue;
        }

        if (flags & 0x0001) {
            uscfp[i]->flags = flags;
        }

        return uscfp[i];
    }

    uscf = ngx_pcalloc(cf->pool, sizeof(ngx_http_upstream_srv_conf_t));
    if (uscf == ((void *)0)) {
        return ((void *)0);
    }

    uscf->flags = flags;
    uscf->host = u->host;
    uscf->file_name = cf->conf_file->file.name.data;
    uscf->line = cf->conf_file->line;
    uscf->port = u->port;
    uscf->default_port = u->default_port;

    if (u->naddrs == 1) {
        uscf->servers = ngx_array_create(cf->pool, 1,
                                         sizeof(ngx_http_upstream_server_t));
        if (uscf->servers == ((void *)0)) {
            return ((void *)0);
        }

        us = ngx_array_push(uscf->servers);
        if (us == ((void *)0)) {
            return ((void *)0);
        }

        (void) memset(us, 0, sizeof(ngx_http_upstream_server_t));

        us->addrs = u->addrs;
        us->naddrs = 1;
    }

    uscfp = ngx_array_push(&umcf->upstreams);
    if (uscfp == ((void *)0)) {
        return ((void *)0);
    }

    *uscfp = uscf;

    return uscf;
}


char *
ngx_http_upstream_bind_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf)
{
    char *p = conf;

    ngx_int_t rc;
    ngx_str_t *value;
    ngx_addr_t **paddr;

    paddr = (ngx_addr_t **) (p + cmd->offset);

    *paddr = ngx_palloc(cf->pool, sizeof(ngx_addr_t));
    if (*paddr == ((void *)0)) {
        return (void *) -1;
    }

    value = cf->args->elts;

    rc = ngx_parse_addr(cf->pool, *paddr, value[1].data, value[1].len);

    switch (rc) {
    case 0:
        (*paddr)->name = value[1];
        return ((void *)0);

    case -5:
        ngx_conf_log_error(1, cf, 0,
                           "invalid address \"%V\"", &value[1]);


    default:
        return (void *) -1;
    }
}


char *
ngx_http_upstream_param_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf)
{
    char *p = conf;

    ngx_str_t *value;
    ngx_array_t **a;
    ngx_http_upstream_param_t *param;

    a = (ngx_array_t **) (p + cmd->offset);

    if (*a == ((void *)0)) {
        *a = ngx_array_create(cf->pool, 4, sizeof(ngx_http_upstream_param_t));
        if (*a == ((void *)0)) {
            return (void *) -1;
        }
    }

    param = ngx_array_push(*a);
    if (param == ((void *)0)) {
        return (void *) -1;
    }

    value = cf->args->elts;

    param->key = value[1];
    param->value = value[2];
    param->skip_empty = 0;

    if (cf->args->nelts == 4) {
        if (strcmp((const char *) value[3].data, (const char *) "if_not_empty") != 0) {
            ngx_conf_log_error(1, cf, 0,
                               "invalid parameter \"%V\"", &value[3]);
            return (void *) -1;
        }

        param->skip_empty = 1;
    }

    return ((void *)0);
}


ngx_int_t
ngx_http_upstream_hide_headers_hash(ngx_conf_t *cf,
    ngx_http_upstream_conf_t *conf, ngx_http_upstream_conf_t *prev,
    ngx_str_t *default_hide_headers, ngx_hash_init_t *hash)
{
    ngx_str_t *h;
    ngx_uint_t i, j;
    ngx_array_t hide_headers;
    ngx_hash_key_t *hk;

    if (conf->hide_headers == (void *) -1
        && conf->pass_headers == (void *) -1)
    {
        conf->hide_headers = prev->hide_headers;
        conf->pass_headers = prev->pass_headers;

        conf->hide_headers_hash = prev->hide_headers_hash;

        if (conf->hide_headers_hash.buckets

            && ((conf->cache == ((void *)0)) == (prev->cache == ((void *)0)))

           )
        {
            return 0;
        }

    } else {
        if (conf->hide_headers == (void *) -1) {
            conf->hide_headers = prev->hide_headers;
        }

        if (conf->pass_headers == (void *) -1) {
            conf->pass_headers = prev->pass_headers;
        }
    }

    if (ngx_array_init(&hide_headers, cf->temp_pool, 4, sizeof(ngx_hash_key_t))
        != 0)
    {
        return -1;
    }

    for (h = default_hide_headers; h->len; h++) {
        hk = ngx_array_push(&hide_headers);
        if (hk == ((void *)0)) {
            return -1;
        }

        hk->key = *h;
        hk->key_hash = ngx_hash_key_lc(h->data, h->len);
        hk->value = (void *) 1;
    }

    if (conf->hide_headers != (void *) -1) {

        h = conf->hide_headers->elts;

        for (i = 0; i < conf->hide_headers->nelts; i++) {

            hk = hide_headers.elts;

            for (j = 0; j < hide_headers.nelts; j++) {
                if (ngx_strcasecmp(h[i].data, hk[j].key.data) == 0) {
                    goto exist;
                }
            }

            hk = ngx_array_push(&hide_headers);
            if (hk == ((void *)0)) {
                return -1;
            }

            hk->key = h[i];
            hk->key_hash = ngx_hash_key_lc(h[i].data, h[i].len);
            hk->value = (void *) 1;

        exist:

            continue;
        }
    }

    if (conf->pass_headers != (void *) -1) {

        h = conf->pass_headers->elts;
        hk = hide_headers.elts;

        for (i = 0; i < conf->pass_headers->nelts; i++) {
            for (j = 0; j < hide_headers.nelts; j++) {

                if (hk[j].key.data == ((void *)0)) {
                    continue;
                }

                if (ngx_strcasecmp(h[i].data, hk[j].key.data) == 0) {
                    hk[j].key.data = ((void *)0);
                    break;
                }
            }
        }
    }

    hash->hash = &conf->hide_headers_hash;
    hash->key = ngx_hash_key_lc;
    hash->pool = cf->pool;
    hash->temp_pool = ((void *)0);

    return ngx_hash_init(hash, hide_headers.elts, hide_headers.nelts);
}


static void *
ngx_http_upstream_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_upstream_main_conf_t *umcf;

    umcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_upstream_main_conf_t));
    if (umcf == ((void *)0)) {
        return ((void *)0);
    }

    if (ngx_array_init(&umcf->upstreams, cf->pool, 4,
                       sizeof(ngx_http_upstream_srv_conf_t *))
        != 0)
    {
        return ((void *)0);
    }

    return umcf;
}


static char *
ngx_http_upstream_init_main_conf(ngx_conf_t *cf, void *conf)
{
    ngx_http_upstream_main_conf_t *umcf = conf;

    ngx_uint_t i;
    ngx_array_t headers_in;
    ngx_hash_key_t *hk;
    ngx_hash_init_t hash;
    ngx_http_upstream_init_pt init;
    ngx_http_upstream_header_t *header;
    ngx_http_upstream_srv_conf_t **uscfp;

    uscfp = umcf->upstreams.elts;

    for (i = 0; i < umcf->upstreams.nelts; i++) {

        init = uscfp[i]->peer.init_upstream ? uscfp[i]->peer.init_upstream:
                                            ngx_http_upstream_init_round_robin;

        if (init(cf, uscfp[i]) != 0) {
            return (void *) -1;
        }
    }




    if (ngx_array_init(&headers_in, cf->temp_pool, 32, sizeof(ngx_hash_key_t))
        != 0)
    {
        return (void *) -1;
    }

    for (header = ngx_http_upstream_headers_in; header->name.len; header++) {
        hk = ngx_array_push(&headers_in);
        if (hk == ((void *)0)) {
            return (void *) -1;
        }

        hk->key = header->name;
        hk->key_hash = ngx_hash_key_lc(header->name.data, header->name.len);
        hk->value = header;
    }

    hash.hash = &umcf->headers_in_hash;
    hash.key = ngx_hash_key_lc;
    hash.max_size = 512;
    hash.bucket_size = (((64) + (ngx_cacheline_size - 1)) & ~(ngx_cacheline_size - 1));
    hash.name = "upstream_headers_in_hash";
    hash.pool = cf->pool;
    hash.temp_pool = ((void *)0);

    if (ngx_hash_init(&hash, headers_in.elts, headers_in.nelts) != 0) {
        return (void *) -1;
    }

    return ((void *)0);
}
