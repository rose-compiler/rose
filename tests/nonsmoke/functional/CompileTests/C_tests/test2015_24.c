// # 1 "zftp.c"
// # 1 "<built-in>"
// # 1 "<command-line>"
// # 1 "zftp.c"
// # 45 "zftp.c"
struct hostent;
struct in_addr;
struct sockaddr_in;
struct sockaddr_in6;
struct zftp_session;
typedef struct zftp_session *Zftp_session;

// # 1 "tcp.h" 1
// # 36 "tcp.h"
// # 1 "../../config.h" 1
// # 37 "tcp.h" 2

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



// # 39 "tcp.h" 2
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
// # 32 "/usr/include/bits/socket.h" 2 3 4




typedef __socklen_t socklen_t;




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

// # 40 "tcp.h" 2
// # 48 "tcp.h"
// # 1 "/usr/include/netdb.h" 1 3 4
// # 28 "/usr/include/netdb.h" 3 4
// # 1 "/usr/include/netinet/in.h" 1 3 4
// # 23 "/usr/include/netinet/in.h" 3 4
// # 1 "/usr/include/stdint.h" 1 3 4
// # 27 "/usr/include/stdint.h" 3 4
// # 1 "/usr/include/bits/wchar.h" 1 3 4
// # 28 "/usr/include/stdint.h" 2 3 4
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
// # 120 "/usr/include/stdint.h" 3 4
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
// # 135 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
// # 24 "/usr/include/netinet/in.h" 2 3 4







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

// # 29 "/usr/include/netdb.h" 2 3 4




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

// # 49 "tcp.h" 2
// # 61 "tcp.h"
// # 1 "/usr/include/netinet/in_systm.h" 1 3 4
// # 26 "/usr/include/netinet/in_systm.h" 3 4

// # 35 "/usr/include/netinet/in_systm.h" 3 4
typedef u_int16_t n_short;
typedef u_int32_t n_long;
typedef u_int32_t n_time;


// # 62 "tcp.h" 2


// # 1 "/usr/include/netinet/ip.h" 1 3 4
// # 27 "/usr/include/netinet/ip.h" 3 4


struct timestamp
  {
    u_int8_t len;
    u_int8_t ptr;

    unsigned int flags:4;
    unsigned int overflow:4;






    u_int32_t data[9];
  };

struct iphdr
  {

    unsigned int ihl:4;
    unsigned int version:4;






    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;

  };
// # 108 "/usr/include/netinet/ip.h" 3 4
struct ip
  {

    unsigned int ip_hl:4;
    unsigned int ip_v:4;





    u_int8_t ip_tos;
    u_short ip_len;
    u_short ip_id;
    u_short ip_off;




    u_int8_t ip_ttl;
    u_int8_t ip_p;
    u_short ip_sum;
    struct in_addr ip_src, ip_dst;
  };




struct ip_timestamp
  {
    u_int8_t ipt_code;
    u_int8_t ipt_len;
    u_int8_t ipt_ptr;

    unsigned int ipt_flg:4;
    unsigned int ipt_oflw:4;





    u_int32_t data[9];
  };
// # 247 "/usr/include/netinet/ip.h" 3 4

// # 65 "tcp.h" 2
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



// # 66 "tcp.h" 2
// # 74 "tcp.h"
union tcp_sockaddr {
    struct sockaddr a;
    struct sockaddr_in in;

    struct sockaddr_in6 in6;

};

typedef struct tcp_session *Tcp_session;





struct tcp_session {
    int fd;
    union tcp_sockaddr sock;
    union tcp_sockaddr peer;
    int flags;
};

// # 1 "tcp.pro" 1
// # 96 "tcp.h" 2
// # 53 "zftp.c" 2
// # 1 "tcp.mdh" 1
// # 17 "tcp.mdh"
// # 1 "../../Src/zsh.mdh" 1
// # 16 "../../Src/zsh.mdh"
// # 1 "../../Src/../config.h" 1
// # 17 "../../Src/zsh.mdh" 2
// # 1 "../../Src/zsh_system.h" 1
// # 127 "../../Src/zsh_system.h"
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

// # 128 "../../Src/zsh_system.h" 2
// # 137 "../../Src/zsh_system.h"
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 152 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef long int ptrdiff_t;
// # 326 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 3 4
typedef int wchar_t;
// # 138 "../../Src/zsh_system.h" 2


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
// # 1 "/usr/include/wchar.h" 1 3 4
// # 48 "/usr/include/wchar.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 49 "/usr/include/wchar.h" 2 3 4
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

// # 141 "../../Src/zsh_system.h" 2
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

// # 142 "../../Src/zsh_system.h" 2
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

// # 143 "../../Src/zsh_system.h" 2
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




// # 144 "../../Src/zsh_system.h" 2
// # 1 "/usr/include/setjmp.h" 1 3 4
// # 28 "/usr/include/setjmp.h" 3 4


// # 1 "/usr/include/bits/setjmp.h" 1 3 4
// # 27 "/usr/include/bits/setjmp.h" 3 4
// # 1 "/usr/include/bits/wordsize.h" 1 3 4
// # 28 "/usr/include/bits/setjmp.h" 2 3 4




typedef long int __jmp_buf[8];
// # 31 "/usr/include/setjmp.h" 2 3 4
// # 1 "/usr/include/bits/sigset.h" 1 3 4
// # 32 "/usr/include/setjmp.h" 2 3 4




typedef struct __jmp_buf_tag
  {




    __jmp_buf __jmpbuf;
    int __mask_was_saved;
    __sigset_t __saved_mask;
  } jmp_buf[1];




extern int setjmp (jmp_buf __env) __attribute__ ((__nothrow__));







extern int __sigsetjmp (struct __jmp_buf_tag __env[1], int __savemask) __attribute__ ((__nothrow__));




extern int _setjmp (struct __jmp_buf_tag __env[1]) __attribute__ ((__nothrow__));
// # 76 "/usr/include/setjmp.h" 3 4




extern void longjmp (struct __jmp_buf_tag __env[1], int __val)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));







extern void _longjmp (struct __jmp_buf_tag __env[1], int __val)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));







typedef struct __jmp_buf_tag sigjmp_buf[1];
// # 108 "/usr/include/setjmp.h" 3 4
extern void siglongjmp (sigjmp_buf __env, int __val)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



// # 145 "../../Src/zsh_system.h" 2


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

// # 148 "../../Src/zsh_system.h" 2



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




// # 152 "../../Src/zsh_system.h" 2



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

// # 156 "../../Src/zsh_system.h" 2
// # 183 "../../Src/zsh_system.h"
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

// # 184 "../../Src/zsh_system.h" 2
// # 199 "../../Src/zsh_system.h"
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 1 3 4
// # 105 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
// # 200 "../../Src/zsh_system.h" 2
// # 231 "../../Src/zsh_system.h"
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

// # 232 "../../Src/zsh_system.h" 2



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

// # 236 "../../Src/zsh_system.h" 2
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

// # 237 "../../Src/zsh_system.h" 2
// # 267 "../../Src/zsh_system.h"
// # 1 "/usr/include/sys/times.h" 1 3 4
// # 32 "/usr/include/sys/times.h" 3 4



struct tms
  {
    clock_t tms_utime;
    clock_t tms_stime;

    clock_t tms_cutime;
    clock_t tms_cstime;
  };






extern clock_t times (struct tms *__buffer) __attribute__ ((__nothrow__));


// # 268 "../../Src/zsh_system.h" 2



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

// # 272 "../../Src/zsh_system.h" 2
// # 282 "../../Src/zsh_system.h"
// # 1 "/usr/include/locale.h" 1 3 4
// # 29 "/usr/include/locale.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 30 "/usr/include/locale.h" 2 3 4
// # 1 "/usr/include/bits/locale.h" 1 3 4
// # 27 "/usr/include/bits/locale.h" 3 4
enum
{
  __LC_CTYPE = 0,
  __LC_NUMERIC = 1,
  __LC_TIME = 2,
  __LC_COLLATE = 3,
  __LC_MONETARY = 4,
  __LC_MESSAGES = 5,
  __LC_ALL = 6,
  __LC_PAPER = 7,
  __LC_NAME = 8,
  __LC_ADDRESS = 9,
  __LC_TELEPHONE = 10,
  __LC_MEASUREMENT = 11,
  __LC_IDENTIFICATION = 12
};
// # 31 "/usr/include/locale.h" 2 3 4


// # 52 "/usr/include/locale.h" 3 4
struct lconv
{


  char *decimal_point;
  char *thousands_sep;





  char *grouping;





  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;

  char p_cs_precedes;

  char p_sep_by_space;

  char n_cs_precedes;

  char n_sep_by_space;






  char p_sign_posn;
  char n_sign_posn;
// # 112 "/usr/include/locale.h" 3 4
  char __int_p_cs_precedes;
  char __int_p_sep_by_space;
  char __int_n_cs_precedes;
  char __int_n_sep_by_space;
  char __int_p_sign_posn;
  char __int_n_sign_posn;

};





extern char *setlocale (int __category, __const char *__locale) __attribute__ ((__nothrow__));


extern struct lconv *localeconv (void) __attribute__ ((__nothrow__));


// # 208 "/usr/include/locale.h" 3 4

// # 283 "../../Src/zsh_system.h" 2



// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 287 "../../Src/zsh_system.h" 2
// # 329 "../../Src/zsh_system.h"
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

// # 330 "../../Src/zsh_system.h" 2
// # 341 "../../Src/zsh_system.h"
// # 1 "/usr/include/sys/wait.h" 1 3 4
// # 29 "/usr/include/sys/wait.h" 3 4



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




// # 342 "../../Src/zsh_system.h" 2
// # 402 "../../Src/zsh_system.h"
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



// # 403 "../../Src/zsh_system.h" 2
// # 421 "../../Src/zsh_system.h"
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


// # 422 "../../Src/zsh_system.h" 2







// # 1 "/usr/include/sys/param.h" 1 3 4
// # 22 "/usr/include/sys/param.h" 3 4
// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/limits.h" 1 3 4
// # 23 "/usr/include/sys/param.h" 2 3 4

// # 1 "/usr/include/linux/param.h" 1 3 4



// # 1 "/usr/include/asm/param.h" 1 3 4




// # 1 "/usr/include/asm-x86_64/param.h" 1 3 4
// # 6 "/usr/include/asm/param.h" 2 3 4
// # 5 "/usr/include/linux/param.h" 2 3 4
// # 25 "/usr/include/sys/param.h" 2 3 4
// # 430 "../../Src/zsh_system.h" 2



// # 1 "/usr/include/sys/utsname.h" 1 3 4
// # 28 "/usr/include/sys/utsname.h" 3 4


// # 1 "/usr/include/bits/utsname.h" 1 3 4
// # 31 "/usr/include/sys/utsname.h" 2 3 4
// # 49 "/usr/include/sys/utsname.h" 3 4
struct utsname
  {

    char sysname[65];


    char nodename[65];


    char release[65];

    char version[65];


    char machine[65];






    char __domainname[65];


  };
// # 82 "/usr/include/sys/utsname.h" 3 4
extern int uname (struct utsname *__name) __attribute__ ((__nothrow__));



// # 434 "../../Src/zsh_system.h" 2
// # 510 "../../Src/zsh_system.h"
// # 1 "/usr/include/sys/capability.h" 1 3 4
// # 32 "/usr/include/sys/capability.h" 3 4
typedef unsigned int __u32;

// # 1 "/usr/include/linux/capability.h" 1 3 4
// # 16 "/usr/include/linux/capability.h" 3 4
// # 1 "/usr/include/linux/types.h" 1 3 4
// # 17 "/usr/include/linux/capability.h" 2 3 4
// # 31 "/usr/include/linux/capability.h" 3 4
typedef struct __user_cap_header_struct {
 __u32 version;
 int pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
        __u32 effective;
        __u32 permitted;
        __u32 inheritable;
} *cap_user_data_t;
// # 35 "/usr/include/sys/capability.h" 2 3 4
// # 44 "/usr/include/sys/capability.h" 3 4
typedef struct _cap_struct *cap_t;







typedef int cap_value_t;




typedef enum {
    CAP_EFFECTIVE=0,
    CAP_PERMITTED=1,
    CAP_INHERITABLE=2
} cap_flag_t;




typedef enum {
    CAP_CLEAR=0,
    CAP_SET=1
} cap_flag_value_t;






cap_t cap_dup(cap_t);
int cap_free(void *);
cap_t cap_init(void);


int cap_get_flag(cap_t, cap_value_t, cap_flag_t, cap_flag_value_t *);
int cap_set_flag(cap_t, cap_flag_t, int, cap_value_t *, cap_flag_value_t);
int cap_clear(cap_t);


cap_t cap_get_fd(int);
cap_t cap_get_file(const char *);
int cap_set_fd(int, cap_t);
int cap_set_file(const char *, cap_t);


cap_t cap_get_proc(void);
int cap_set_proc(cap_t);


ssize_t cap_size(cap_t);
ssize_t cap_copy_ext(void *, cap_t, ssize_t);
cap_t cap_copy_int(const void *);


cap_t cap_from_text(const char *);
char * cap_to_text(cap_t, ssize_t *);
// # 511 "../../Src/zsh_system.h" 2
// # 718 "../../Src/zsh_system.h"
extern char **environ;
// # 798 "../../Src/zsh_system.h"
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

// # 799 "../../Src/zsh_system.h" 2
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

// # 800 "../../Src/zsh_system.h" 2


// # 1 "/usr/include/langinfo.h" 1 3 4
// # 24 "/usr/include/langinfo.h" 3 4
// # 1 "/usr/include/nl_types.h" 1 3 4
// # 31 "/usr/include/nl_types.h" 3 4



typedef void *nl_catd;


typedef int nl_item;





extern nl_catd catopen (__const char *__cat_name, int __flag) __attribute__ ((__nonnull__ (1)));



extern char *catgets (nl_catd __catalog, int __set, int __number,
        __const char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int catclose (nl_catd __catalog) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


// # 25 "/usr/include/langinfo.h" 2 3 4

// # 1 "/usr/include/bits/locale.h" 1 3 4
// # 27 "/usr/include/langinfo.h" 2 3 4



// # 42 "/usr/include/langinfo.h" 3 4
enum
{



  ABDAY_1 = (((__LC_TIME) << 16) | (0)),

  ABDAY_2,

  ABDAY_3,

  ABDAY_4,

  ABDAY_5,

  ABDAY_6,

  ABDAY_7,



  DAY_1,

  DAY_2,

  DAY_3,

  DAY_4,

  DAY_5,

  DAY_6,

  DAY_7,



  ABMON_1,

  ABMON_2,

  ABMON_3,

  ABMON_4,

  ABMON_5,

  ABMON_6,

  ABMON_7,

  ABMON_8,

  ABMON_9,

  ABMON_10,

  ABMON_11,

  ABMON_12,



  MON_1,

  MON_2,

  MON_3,

  MON_4,

  MON_5,

  MON_6,

  MON_7,

  MON_8,

  MON_9,

  MON_10,

  MON_11,

  MON_12,


  AM_STR,

  PM_STR,


  D_T_FMT,

  D_FMT,

  T_FMT,

  T_FMT_AMPM,


  ERA,

  __ERA_YEAR,



  ERA_D_FMT,

  ALT_DIGITS,

  ERA_D_T_FMT,

  ERA_T_FMT,


  _NL_TIME_ERA_NUM_ENTRIES,
  _NL_TIME_ERA_ENTRIES,

  _NL_WABDAY_1,
  _NL_WABDAY_2,
  _NL_WABDAY_3,
  _NL_WABDAY_4,
  _NL_WABDAY_5,
  _NL_WABDAY_6,
  _NL_WABDAY_7,


  _NL_WDAY_1,
  _NL_WDAY_2,
  _NL_WDAY_3,
  _NL_WDAY_4,
  _NL_WDAY_5,
  _NL_WDAY_6,
  _NL_WDAY_7,


  _NL_WABMON_1,
  _NL_WABMON_2,
  _NL_WABMON_3,
  _NL_WABMON_4,
  _NL_WABMON_5,
  _NL_WABMON_6,
  _NL_WABMON_7,
  _NL_WABMON_8,
  _NL_WABMON_9,
  _NL_WABMON_10,
  _NL_WABMON_11,
  _NL_WABMON_12,


  _NL_WMON_1,
  _NL_WMON_2,
  _NL_WMON_3,
  _NL_WMON_4,
  _NL_WMON_5,
  _NL_WMON_6,
  _NL_WMON_7,
  _NL_WMON_8,
  _NL_WMON_9,
  _NL_WMON_10,
  _NL_WMON_11,
  _NL_WMON_12,

  _NL_WAM_STR,
  _NL_WPM_STR,

  _NL_WD_T_FMT,
  _NL_WD_FMT,
  _NL_WT_FMT,
  _NL_WT_FMT_AMPM,

  _NL_WERA_YEAR,
  _NL_WERA_D_FMT,
  _NL_WALT_DIGITS,
  _NL_WERA_D_T_FMT,
  _NL_WERA_T_FMT,

  _NL_TIME_WEEK_NDAYS,
  _NL_TIME_WEEK_1STDAY,
  _NL_TIME_WEEK_1STWEEK,
  _NL_TIME_FIRST_WEEKDAY,
  _NL_TIME_FIRST_WORKDAY,
  _NL_TIME_CAL_DIRECTION,
  _NL_TIME_TIMEZONE,

  _DATE_FMT,

  _NL_W_DATE_FMT,

  _NL_TIME_CODESET,

  _NL_NUM_LC_TIME,




  _NL_COLLATE_NRULES = (((__LC_COLLATE) << 16) | (0)),
  _NL_COLLATE_RULESETS,
  _NL_COLLATE_TABLEMB,
  _NL_COLLATE_WEIGHTMB,
  _NL_COLLATE_EXTRAMB,
  _NL_COLLATE_INDIRECTMB,
  _NL_COLLATE_GAP1,
  _NL_COLLATE_GAP2,
  _NL_COLLATE_GAP3,
  _NL_COLLATE_TABLEWC,
  _NL_COLLATE_WEIGHTWC,
  _NL_COLLATE_EXTRAWC,
  _NL_COLLATE_INDIRECTWC,
  _NL_COLLATE_SYMB_HASH_SIZEMB,
  _NL_COLLATE_SYMB_TABLEMB,
  _NL_COLLATE_SYMB_EXTRAMB,
  _NL_COLLATE_COLLSEQMB,
  _NL_COLLATE_COLLSEQWC,
  _NL_COLLATE_CODESET,
  _NL_NUM_LC_COLLATE,




  _NL_CTYPE_CLASS = (((__LC_CTYPE) << 16) | (0)),
  _NL_CTYPE_TOUPPER,
  _NL_CTYPE_GAP1,
  _NL_CTYPE_TOLOWER,
  _NL_CTYPE_GAP2,
  _NL_CTYPE_CLASS32,
  _NL_CTYPE_GAP3,
  _NL_CTYPE_GAP4,
  _NL_CTYPE_GAP5,
  _NL_CTYPE_GAP6,
  _NL_CTYPE_CLASS_NAMES,
  _NL_CTYPE_MAP_NAMES,
  _NL_CTYPE_WIDTH,
  _NL_CTYPE_MB_CUR_MAX,
  _NL_CTYPE_CODESET_NAME,
  CODESET = _NL_CTYPE_CODESET_NAME,

  _NL_CTYPE_TOUPPER32,
  _NL_CTYPE_TOLOWER32,
  _NL_CTYPE_CLASS_OFFSET,
  _NL_CTYPE_MAP_OFFSET,
  _NL_CTYPE_INDIGITS_MB_LEN,
  _NL_CTYPE_INDIGITS0_MB,
  _NL_CTYPE_INDIGITS1_MB,
  _NL_CTYPE_INDIGITS2_MB,
  _NL_CTYPE_INDIGITS3_MB,
  _NL_CTYPE_INDIGITS4_MB,
  _NL_CTYPE_INDIGITS5_MB,
  _NL_CTYPE_INDIGITS6_MB,
  _NL_CTYPE_INDIGITS7_MB,
  _NL_CTYPE_INDIGITS8_MB,
  _NL_CTYPE_INDIGITS9_MB,
  _NL_CTYPE_INDIGITS_WC_LEN,
  _NL_CTYPE_INDIGITS0_WC,
  _NL_CTYPE_INDIGITS1_WC,
  _NL_CTYPE_INDIGITS2_WC,
  _NL_CTYPE_INDIGITS3_WC,
  _NL_CTYPE_INDIGITS4_WC,
  _NL_CTYPE_INDIGITS5_WC,
  _NL_CTYPE_INDIGITS6_WC,
  _NL_CTYPE_INDIGITS7_WC,
  _NL_CTYPE_INDIGITS8_WC,
  _NL_CTYPE_INDIGITS9_WC,
  _NL_CTYPE_OUTDIGIT0_MB,
  _NL_CTYPE_OUTDIGIT1_MB,
  _NL_CTYPE_OUTDIGIT2_MB,
  _NL_CTYPE_OUTDIGIT3_MB,
  _NL_CTYPE_OUTDIGIT4_MB,
  _NL_CTYPE_OUTDIGIT5_MB,
  _NL_CTYPE_OUTDIGIT6_MB,
  _NL_CTYPE_OUTDIGIT7_MB,
  _NL_CTYPE_OUTDIGIT8_MB,
  _NL_CTYPE_OUTDIGIT9_MB,
  _NL_CTYPE_OUTDIGIT0_WC,
  _NL_CTYPE_OUTDIGIT1_WC,
  _NL_CTYPE_OUTDIGIT2_WC,
  _NL_CTYPE_OUTDIGIT3_WC,
  _NL_CTYPE_OUTDIGIT4_WC,
  _NL_CTYPE_OUTDIGIT5_WC,
  _NL_CTYPE_OUTDIGIT6_WC,
  _NL_CTYPE_OUTDIGIT7_WC,
  _NL_CTYPE_OUTDIGIT8_WC,
  _NL_CTYPE_OUTDIGIT9_WC,
  _NL_CTYPE_TRANSLIT_TAB_SIZE,
  _NL_CTYPE_TRANSLIT_FROM_IDX,
  _NL_CTYPE_TRANSLIT_FROM_TBL,
  _NL_CTYPE_TRANSLIT_TO_IDX,
  _NL_CTYPE_TRANSLIT_TO_TBL,
  _NL_CTYPE_TRANSLIT_DEFAULT_MISSING_LEN,
  _NL_CTYPE_TRANSLIT_DEFAULT_MISSING,
  _NL_CTYPE_TRANSLIT_IGNORE_LEN,
  _NL_CTYPE_TRANSLIT_IGNORE,
  _NL_CTYPE_MAP_TO_NONASCII,
  _NL_CTYPE_EXTRA_MAP_1,
  _NL_CTYPE_EXTRA_MAP_2,
  _NL_CTYPE_EXTRA_MAP_3,
  _NL_CTYPE_EXTRA_MAP_4,
  _NL_CTYPE_EXTRA_MAP_5,
  _NL_CTYPE_EXTRA_MAP_6,
  _NL_CTYPE_EXTRA_MAP_7,
  _NL_CTYPE_EXTRA_MAP_8,
  _NL_CTYPE_EXTRA_MAP_9,
  _NL_CTYPE_EXTRA_MAP_10,
  _NL_CTYPE_EXTRA_MAP_11,
  _NL_CTYPE_EXTRA_MAP_12,
  _NL_CTYPE_EXTRA_MAP_13,
  _NL_CTYPE_EXTRA_MAP_14,
  _NL_NUM_LC_CTYPE,




  __INT_CURR_SYMBOL = (((__LC_MONETARY) << 16) | (0)),



  __CURRENCY_SYMBOL,



  __MON_DECIMAL_POINT,



  __MON_THOUSANDS_SEP,



  __MON_GROUPING,



  __POSITIVE_SIGN,



  __NEGATIVE_SIGN,



  __INT_FRAC_DIGITS,



  __FRAC_DIGITS,



  __P_CS_PRECEDES,



  __P_SEP_BY_SPACE,



  __N_CS_PRECEDES,



  __N_SEP_BY_SPACE,



  __P_SIGN_POSN,



  __N_SIGN_POSN,



  _NL_MONETARY_CRNCYSTR,

  __INT_P_CS_PRECEDES,



  __INT_P_SEP_BY_SPACE,



  __INT_N_CS_PRECEDES,



  __INT_N_SEP_BY_SPACE,



  __INT_P_SIGN_POSN,



  __INT_N_SIGN_POSN,



  _NL_MONETARY_DUO_INT_CURR_SYMBOL,
  _NL_MONETARY_DUO_CURRENCY_SYMBOL,
  _NL_MONETARY_DUO_INT_FRAC_DIGITS,
  _NL_MONETARY_DUO_FRAC_DIGITS,
  _NL_MONETARY_DUO_P_CS_PRECEDES,
  _NL_MONETARY_DUO_P_SEP_BY_SPACE,
  _NL_MONETARY_DUO_N_CS_PRECEDES,
  _NL_MONETARY_DUO_N_SEP_BY_SPACE,
  _NL_MONETARY_DUO_INT_P_CS_PRECEDES,
  _NL_MONETARY_DUO_INT_P_SEP_BY_SPACE,
  _NL_MONETARY_DUO_INT_N_CS_PRECEDES,
  _NL_MONETARY_DUO_INT_N_SEP_BY_SPACE,
  _NL_MONETARY_DUO_P_SIGN_POSN,
  _NL_MONETARY_DUO_N_SIGN_POSN,
  _NL_MONETARY_DUO_INT_P_SIGN_POSN,
  _NL_MONETARY_DUO_INT_N_SIGN_POSN,
  _NL_MONETARY_UNO_VALID_FROM,
  _NL_MONETARY_UNO_VALID_TO,
  _NL_MONETARY_DUO_VALID_FROM,
  _NL_MONETARY_DUO_VALID_TO,
  _NL_MONETARY_CONVERSION_RATE,
  _NL_MONETARY_DECIMAL_POINT_WC,
  _NL_MONETARY_THOUSANDS_SEP_WC,
  _NL_MONETARY_CODESET,
  _NL_NUM_LC_MONETARY,



  __DECIMAL_POINT = (((__LC_NUMERIC) << 16) | (0)),



  RADIXCHAR = __DECIMAL_POINT,

  __THOUSANDS_SEP,



  THOUSEP = __THOUSANDS_SEP,

  __GROUPING,



  _NL_NUMERIC_DECIMAL_POINT_WC,
  _NL_NUMERIC_THOUSANDS_SEP_WC,
  _NL_NUMERIC_CODESET,
  _NL_NUM_LC_NUMERIC,

  __YESEXPR = (((__LC_MESSAGES) << 16) | (0)),

  __NOEXPR,

  __YESSTR,



  __NOSTR,



  _NL_MESSAGES_CODESET,
  _NL_NUM_LC_MESSAGES,

  _NL_PAPER_HEIGHT = (((__LC_PAPER) << 16) | (0)),
  _NL_PAPER_WIDTH,
  _NL_PAPER_CODESET,
  _NL_NUM_LC_PAPER,

  _NL_NAME_NAME_FMT = (((__LC_NAME) << 16) | (0)),
  _NL_NAME_NAME_GEN,
  _NL_NAME_NAME_MR,
  _NL_NAME_NAME_MRS,
  _NL_NAME_NAME_MISS,
  _NL_NAME_NAME_MS,
  _NL_NAME_CODESET,
  _NL_NUM_LC_NAME,

  _NL_ADDRESS_POSTAL_FMT = (((__LC_ADDRESS) << 16) | (0)),
  _NL_ADDRESS_COUNTRY_NAME,
  _NL_ADDRESS_COUNTRY_POST,
  _NL_ADDRESS_COUNTRY_AB2,
  _NL_ADDRESS_COUNTRY_AB3,
  _NL_ADDRESS_COUNTRY_CAR,
  _NL_ADDRESS_COUNTRY_NUM,
  _NL_ADDRESS_COUNTRY_ISBN,
  _NL_ADDRESS_LANG_NAME,
  _NL_ADDRESS_LANG_AB,
  _NL_ADDRESS_LANG_TERM,
  _NL_ADDRESS_LANG_LIB,
  _NL_ADDRESS_CODESET,
  _NL_NUM_LC_ADDRESS,

  _NL_TELEPHONE_TEL_INT_FMT = (((__LC_TELEPHONE) << 16) | (0)),
  _NL_TELEPHONE_TEL_DOM_FMT,
  _NL_TELEPHONE_INT_SELECT,
  _NL_TELEPHONE_INT_PREFIX,
  _NL_TELEPHONE_CODESET,
  _NL_NUM_LC_TELEPHONE,

  _NL_MEASUREMENT_MEASUREMENT = (((__LC_MEASUREMENT) << 16) | (0)),
  _NL_MEASUREMENT_CODESET,
  _NL_NUM_LC_MEASUREMENT,

  _NL_IDENTIFICATION_TITLE = (((__LC_IDENTIFICATION) << 16) | (0)),
  _NL_IDENTIFICATION_SOURCE,
  _NL_IDENTIFICATION_ADDRESS,
  _NL_IDENTIFICATION_CONTACT,
  _NL_IDENTIFICATION_EMAIL,
  _NL_IDENTIFICATION_TEL,
  _NL_IDENTIFICATION_FAX,
  _NL_IDENTIFICATION_LANGUAGE,
  _NL_IDENTIFICATION_TERRITORY,
  _NL_IDENTIFICATION_AUDIENCE,
  _NL_IDENTIFICATION_APPLICATION,
  _NL_IDENTIFICATION_ABBREVIATION,
  _NL_IDENTIFICATION_REVISION,
  _NL_IDENTIFICATION_DATE,
  _NL_IDENTIFICATION_CATEGORY,
  _NL_IDENTIFICATION_CODESET,
  _NL_NUM_LC_IDENTIFICATION,


  _NL_NUM
};
// # 582 "/usr/include/langinfo.h" 3 4
extern char *nl_langinfo (nl_item __item) __attribute__ ((__nothrow__));
// # 596 "/usr/include/langinfo.h" 3 4

// # 803 "../../Src/zsh_system.h" 2

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


// # 805 "../../Src/zsh_system.h" 2
// # 865 "../../Src/zsh_system.h"
// # 1 "/usr/include/termio.h" 1 3 4
// # 866 "../../Src/zsh_system.h" 2


// # 1 "../../Src/zshcurses.h" 1
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


// # 1 "/nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/stddef.h" 1 3 4
// # 131 "/usr/include/ncursesw/ncurses.h" 2 3 4
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
// # 337 "/usr/include/ncursesw/ncurses.h" 3 4
typedef struct
{
    attr_t attr;
    wchar_t chars[5];



}
cchar_t;



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


 cchar_t _bkgrnd;




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
// # 1204 "/usr/include/ncursesw/ncurses.h" 3 4
extern cchar_t * _nc_wacs;
// # 1267 "/usr/include/ncursesw/ncurses.h" 3 4
extern int add_wch (const cchar_t *);
extern int add_wchnstr (const cchar_t *, int);
extern int add_wchstr (const cchar_t *);
extern int addnwstr (const wchar_t *, int);
extern int addwstr (const wchar_t *);
extern int bkgrnd (const cchar_t *);
extern void bkgrndset (const cchar_t *);
extern int border_set (const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*);
extern int box_set (WINDOW *, const cchar_t *, const cchar_t *);
extern int echo_wchar (const cchar_t *);
extern int erasewchar (wchar_t*);
extern int get_wch (wint_t *);
extern int get_wstr (wint_t *);
extern int getbkgrnd (cchar_t *);
extern int getcchar (const cchar_t *, wchar_t*, attr_t*, short*, void*);
extern int getn_wstr (wint_t *, int);
extern int hline_set (const cchar_t *, int);
extern int in_wch (cchar_t *);
extern int in_wchnstr (cchar_t *, int);
extern int in_wchstr (cchar_t *);
extern int innwstr (wchar_t *, int);
extern int ins_nwstr (const wchar_t *, int);
extern int ins_wch (const cchar_t *);
extern int ins_wstr (const wchar_t *);
extern int inwstr (wchar_t *);
extern char* key_name (wchar_t);
extern int killwchar (wchar_t *);
extern int mvadd_wch (int, int, const cchar_t *);
extern int mvadd_wchnstr (int, int, const cchar_t *, int);
extern int mvadd_wchstr (int, int, const cchar_t *);
extern int mvaddnwstr (int, int, const wchar_t *, int);
extern int mvaddwstr (int, int, const wchar_t *);
extern int mvget_wch (int, int, wint_t *);
extern int mvget_wstr (int, int, wint_t *);
extern int mvgetn_wstr (int, int, wint_t *, int);
extern int mvhline_set (int, int, const cchar_t *, int);
extern int mvin_wch (int, int, cchar_t *);
extern int mvin_wchnstr (int, int, cchar_t *, int);
extern int mvin_wchstr (int, int, cchar_t *);
extern int mvinnwstr (int, int, wchar_t *, int);
extern int mvins_nwstr (int, int, const wchar_t *, int);
extern int mvins_wch (int, int, const cchar_t *);
extern int mvins_wstr (int, int, const wchar_t *);
extern int mvinwstr (int, int, wchar_t *);
extern int mvvline_set (int, int, const cchar_t *, int);
extern int mvwadd_wch (WINDOW *, int, int, const cchar_t *);
extern int mvwadd_wchnstr (WINDOW *, int, int, const cchar_t *, int);
extern int mvwadd_wchstr (WINDOW *, int, int, const cchar_t *);
extern int mvwaddnwstr (WINDOW *, int, int, const wchar_t *, int);
extern int mvwaddwstr (WINDOW *, int, int, const wchar_t *);
extern int mvwget_wch (WINDOW *, int, int, wint_t *);
extern int mvwget_wstr (WINDOW *, int, int, wint_t *);
extern int mvwgetn_wstr (WINDOW *, int, int, wint_t *, int);
extern int mvwhline_set (WINDOW *, int, int, const cchar_t *, int);
extern int mvwin_wch (WINDOW *, int, int, cchar_t *);
extern int mvwin_wchnstr (WINDOW *, int,int, cchar_t *,int);
extern int mvwin_wchstr (WINDOW *, int, int, cchar_t *);
extern int mvwinnwstr (WINDOW *, int, int, wchar_t *, int);
extern int mvwins_nwstr (WINDOW *, int,int, const wchar_t *,int);
extern int mvwins_wch (WINDOW *, int, int, const cchar_t *);
extern int mvwins_wstr (WINDOW *, int, int, const wchar_t *);
extern int mvwinwstr (WINDOW *, int, int, wchar_t *);
extern int mvwvline_set (WINDOW *, int,int, const cchar_t *,int);
extern int pecho_wchar (WINDOW *, const cchar_t *);
extern int setcchar (cchar_t *, const wchar_t *, const attr_t, short, const void *);
extern int slk_wset (int, const wchar_t *, int);
extern attr_t term_attrs (void);
extern int unget_wch (const wchar_t);
extern int vid_attr (attr_t, short, void *);
extern int vid_puts (attr_t, short, void *, int (*)(int));
extern int vline_set (const cchar_t *, int);
extern int wadd_wch (WINDOW *,const cchar_t *);
extern int wadd_wchnstr (WINDOW *,const cchar_t *,int);
extern int wadd_wchstr (WINDOW *,const cchar_t *);
extern int waddnwstr (WINDOW *,const wchar_t *,int);
extern int waddwstr (WINDOW *,const wchar_t *);
extern int wbkgrnd (WINDOW *,const cchar_t *);
extern void wbkgrndset (WINDOW *,const cchar_t *);
extern int wborder_set (WINDOW *,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*,const cchar_t*);
extern int wecho_wchar (WINDOW *, const cchar_t *);
extern int wget_wch (WINDOW *, wint_t *);
extern int wget_wstr (WINDOW *, wint_t *);
extern int wgetbkgrnd (WINDOW *, cchar_t *);
extern int wgetn_wstr (WINDOW *,wint_t *, int);
extern int whline_set (WINDOW *, const cchar_t *, int);
extern int win_wch (WINDOW *, cchar_t *);
extern int win_wchnstr (WINDOW *, cchar_t *, int);
extern int win_wchstr (WINDOW *, cchar_t *);
extern int winnwstr (WINDOW *, wchar_t *, int);
extern int wins_nwstr (WINDOW *, const wchar_t *, int);
extern int wins_wch (WINDOW *, const cchar_t *);
extern int wins_wstr (WINDOW *, const wchar_t *);
extern int winwstr (WINDOW *, wchar_t *);
extern wchar_t* wunctrl (cchar_t *);
extern int wvline_set (WINDOW *, const cchar_t *, int);
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


extern char * _tracecchar_t (const cchar_t *);

extern char * _tracecchar_t2 (int, const cchar_t *);




extern char * _tracemouse (const MEVENT *);
extern void trace (const unsigned int);
// # 1 "../../Src/zshcurses.h" 2
// # 869 "../../Src/zsh_system.h" 2

// # 1 "../../Src/zshterm.h" 1
// # 1 "/usr/include/ncursesw/term.h" 1 3 4
// # 673 "/usr/include/ncursesw/term.h" 3 4
typedef struct termtype {
    char *term_names;
    char *str_table;
    char *Booleans;
    short *Numbers;
    char **Strings;


    char *ext_str_table;
    char **ext_Names;

    unsigned short num_Booleans;
    unsigned short num_Numbers;
    unsigned short num_Strings;

    unsigned short ext_Booleans;
    unsigned short ext_Numbers;
    unsigned short ext_Strings;


} TERMTYPE;

typedef struct term {
    TERMTYPE type;
    short Filedes;
    struct termios Ottyb,
  Nttyb;
    int _baudrate;
    char * _termname;
} TERMINAL;

extern TERMINAL * cur_term;
// # 729 "/usr/include/ncursesw/term.h" 3 4
extern char * const boolnames[];
extern char * const boolcodes[];
extern char * const boolfnames[];
extern char * const numnames[];
extern char * const numcodes[];
extern char * const numfnames[];
extern char * const strnames[];
extern char * const strcodes[];
extern char * const strfnames[];




extern int _nc_set_tty_mode (struct termios *buf);
extern int _nc_get_tty_mode (struct termios *buf);
extern int _nc_read_entry (const char * const, char * const, TERMTYPE *const);
extern int _nc_read_file_entry (const char *const, TERMTYPE *);
extern char * _nc_first_name (const char *const);
extern int _nc_name_match (const char *const, const char *const, const char *const);
extern int _nc_read_termcap_entry (const char *const, TERMTYPE *const);
extern const TERMTYPE * _nc_fallback (const char *);


extern TERMINAL * set_curterm (TERMINAL *);
extern int del_curterm (TERMINAL *);


extern int restartterm ( char *, int, int *);
extern int setupterm ( char *,int,int *);
// # 771 "/usr/include/ncursesw/term.h" 3 4
extern char * tgetstr ( char *, char **);
extern char * tgoto (const char *, int, int);
extern int tgetent (char *, const char *);
extern int tgetflag ( char *);
extern int tgetnum ( char *);
extern int tputs (const char *, int, int (*)(int));
// # 1 "../../Src/zshterm.h" 2
// # 871 "../../Src/zsh_system.h" 2
// # 18 "../../Src/zsh.mdh" 2
// # 1 "../../Src/zsh.h" 1
// # 48 "../../Src/zsh.h"
typedef long zlong;
typedef unsigned long zulong;
// # 61 "../../Src/zsh.h"
typedef struct {
    union {
 zlong l;
 double d;
    } u;
    int type;
} mnumber;





typedef struct mathfunc *MathFunc;
typedef mnumber (*NumMathFunc)(char *, int, mnumber *, int);
typedef mnumber (*StrMathFunc)(char *, char *, int);

struct mathfunc {
    MathFunc next;
    char *name;
    int flags;
    NumMathFunc nfunc;
    StrMathFunc sfunc;
    char *module;
    int minargs;
    int maxargs;
    int funcid;
};
// # 197 "../../Src/zsh.h"
enum {




    QT_NONE,

    QT_BACKSLASH,

    QT_SINGLE,

    QT_DOUBLE,

    QT_DOLLARS,





    QT_BACKTICK,




    QT_SINGLE_OPTIONAL,



    QT_BACKSLASH_SHOWNULL
};
// # 235 "../../Src/zsh.h"
enum {
    NULLTOK,
    SEPER,
    NEWLIN,
    SEMI,
    DSEMI,
    AMPER,
    INPAR,
    OUTPAR,
    DBAR,
    DAMPER,
    OUTANG,
    OUTANGBANG,
    DOUTANG,
    DOUTANGBANG,
    INANG,
    INOUTANG,
    DINANG,
    DINANGDASH,
    INANGAMP,
    OUTANGAMP,
    AMPOUTANG,
    OUTANGAMPBANG,
    DOUTANGAMP,
    DOUTANGAMPBANG,
    TRINANG,
    BAR,
    BARAMP,
    INOUTPAR,
    DINPAR,
    DOUTPAR,
    AMPERBANG,
    SEMIAMP,
    SEMIBAR,
    DOUTBRACK,
    STRING,
    ENVSTRING,
    ENVARRAY,
    ENDINPUT,
    LEXERR,


    BANG,
    DINBRACK,
    INBRACE,
    OUTBRACE,
    CASE,
    COPROC,
    DOLOOP,
    DONE,
    ELIF,
    ELSE,
    ZEND,
    ESAC,
    FI,
    FOR,
    FOREACH,
    FUNC,
    IF,
    NOCORRECT,
    REPEAT,
    SELECT,
    THEN,
    TIME,
    UNTIL,
    WHILE
};





enum {
    REDIR_WRITE,
    REDIR_WRITENOW,
    REDIR_APP,
    REDIR_APPNOW,
    REDIR_ERRWRITE,
    REDIR_ERRWRITENOW,
    REDIR_ERRAPP,
    REDIR_ERRAPPNOW,
    REDIR_READWRITE,
    REDIR_READ,
    REDIR_HEREDOC,
    REDIR_HEREDOCDASH,
    REDIR_HERESTR,
    REDIR_MERGEIN,
    REDIR_MERGEOUT,
    REDIR_CLOSE,
    REDIR_INPIPE,
    REDIR_OUTPIPE
};
// # 403 "../../Src/zsh.h"
typedef struct alias *Alias;
typedef struct asgment *Asgment;
typedef struct builtin *Builtin;
typedef struct cmdnam *Cmdnam;
typedef struct complist *Complist;
typedef struct conddef *Conddef;
typedef struct dirsav *Dirsav;
typedef struct features *Features;
typedef struct feature_enables *Feature_enables;
typedef struct funcstack *Funcstack;
typedef struct funcwrap *FuncWrap;
typedef struct hashnode *HashNode;
typedef struct hashtable *HashTable;
typedef struct heap *Heap;
typedef struct heapstack *Heapstack;
typedef struct histent *Histent;
typedef struct hookdef *Hookdef;
typedef struct job *Job;
typedef struct linkedmod *Linkedmod;
typedef struct linknode *LinkNode;
typedef union linkroot *LinkList;
typedef struct module *Module;
typedef struct nameddir *Nameddir;
typedef struct options *Options;
typedef struct optname *Optname;
typedef struct param *Param;
typedef struct paramdef *Paramdef;
typedef struct patprog *Patprog;
typedef struct prepromptfn *Prepromptfn;
typedef struct process *Process;
typedef struct redir *Redir;
typedef struct reswd *Reswd;
typedef struct shfunc *Shfunc;
typedef struct timedfn *Timedfn;
typedef struct value *Value;







struct linknode {
    LinkNode next;
    LinkNode prev;
    void *dat;
};

struct linklist {
    LinkNode first;
    LinkNode last;
    int flags;
};

union linkroot {
    struct linklist list;
    struct linknode node;
};
// # 509 "../../Src/zsh.h"
typedef void (*voidvoidfnptr_t) (void);




struct prepromptfn {
    voidvoidfnptr_t func;
};





struct timedfn {
    voidvoidfnptr_t func;
    time_t when;
};
// # 562 "../../Src/zsh.h"
typedef int (*CondHandler) (char **, int);

struct conddef {
    Conddef next;
    char *name;
    int flags;
    CondHandler handler;
    int min;
    int max;
    int condid;
    char *module;
};
// # 587 "../../Src/zsh.h"
enum {

    REDIRF_FROM_HEREDOC = 1
};



struct redir {
    int type;
    int flags;
    int fd1, fd2;
    char *name;
    char *varid;
    char *here_terminator;
    char *munged_here_terminator;
};
// # 616 "../../Src/zsh.h"
struct multio {
    int ct;
    int rflag;
    int pipe;
    int fds[8];
};



struct asgment {
    struct asgment *next;
    char *name;
    char *value;
};



struct value {
    int isarr;
    Param pm;
    int flags;
    int start;
    int end;
    char **arr;
};

enum {
    VALFLAG_INV = 0x0001,
    VALFLAG_EMPTY = 0x0002,
    VALFLAG_SUBST = 0x0004
};







typedef unsigned int wordcode;
typedef wordcode *Wordcode;

typedef struct funcdump *FuncDump;
typedef struct eprog *Eprog;

struct funcdump {
    FuncDump next;
    dev_t dev;
    ino_t ino;
    int fd;
    Wordcode map;
    Wordcode addr;
    int len;
    int count;
    char *filename;
};
// # 689 "../../Src/zsh.h"
struct eprog {
    int flags;
    int len;
    int npats;
    int nref;
    Patprog *pats;
    Wordcode prog;
    char *strs;
    Shfunc shf;
    FuncDump dump;
};






typedef struct estate *Estate;

struct estate {
    Eprog prog;
    Wordcode pc;
    char *strs;
};

typedef struct eccstr *Eccstr;

struct eccstr {
    Eccstr left, right;
    char *str;
    wordcode offs, aoffs;
    int nfunc;
};
// # 878 "../../Src/zsh.h"
struct job {
    pid_t gleader;
    pid_t other;
    int stat;
    char *pwd;

    struct process *procs;
    struct process *auxprocs;
    LinkList filelist;
    int stty_in_env;
    struct ttyinfo *ty;
};
// # 914 "../../Src/zsh.h"
struct timeinfo {
    long ut;
    long st;
};
// # 927 "../../Src/zsh.h"
typedef struct rusage child_times_t;




struct process {
    struct process *next;
    pid_t pid;
    char text[80];
    int status;
    child_times_t ti;
    struct timeval bgtime;
    struct timeval endtime;
};

struct execstack {
    struct execstack *next;

    pid_t list_pipe_pid;
    int nowait;
    int pline_level;
    int list_pipe_child;
    int list_pipe_job;
    char list_pipe_text[80];
    int lastval;
    int noeval;
    int badcshglob;
    pid_t cmdoutpid;
    int cmdoutval;
    int use_cmdoutval;
    int trap_return;
    int trap_state;
    int trapisfunc;
    int traplocallevel;
    int noerrs;
    int subsh_close;
    char *underscore;
};

struct heredocs {
    struct heredocs *next;
    int type;
    int pc;
    char *str;
};

struct dirsav {
    int dirfd, level;
    char *dirname;
    dev_t dev;
    ino_t ino;
};







typedef void *(*VFunc) (void *);
typedef void (*FreeFunc) (void *);

typedef unsigned (*HashFunc) (const char *);
typedef void (*TableFunc) (HashTable);





typedef void (*AddNodeFunc) (HashTable, char *, void *);
typedef HashNode (*GetNodeFunc) (HashTable, const char *);
typedef HashNode (*RemoveNodeFunc) (HashTable, const char *);
typedef void (*FreeNodeFunc) (HashNode);
typedef int (*CompareFunc) (const char *, const char *);



typedef void (*ScanFunc) (HashNode, int);
typedef void (*ScanTabFunc) (HashTable, ScanFunc, int);

typedef void (*PrintTableStats) (HashTable);



struct hashtable {

    int hsize;
    int ct;
    HashNode *nodes;
    void *tmpdata;


    HashFunc hash;
    TableFunc emptytable;
    TableFunc filltable;
    CompareFunc cmpnodes;
    AddNodeFunc addnode;
    GetNodeFunc getnode;
    GetNodeFunc getnode2;

    RemoveNodeFunc removenode;
    ScanFunc disablenode;
    ScanFunc enablenode;
    FreeNodeFunc freenode;
    ScanFunc printnode;
    ScanTabFunc scantab;




};



struct hashnode {
    HashNode next;
    char *nam;
    int flags;
};
// # 1054 "../../Src/zsh.h"
struct optname {
    struct hashnode node;
    int optno;
};



struct reswd {
    struct hashnode node;
    int token;
};



struct alias {
    struct hashnode node;
    char *text;
    int inuse;
};
// # 1082 "../../Src/zsh.h"
struct cmdnam {
    struct hashnode node;
    union {
 char **name;
 char *cmd;
    }
    u;
};







struct shfunc {
    struct hashnode node;
    char *filename;
    zlong lineno;
    Eprog funcdef;
    int emulation;
};
// # 1118 "../../Src/zsh.h"
enum {
    FS_SOURCE,
    FS_FUNC,
    FS_EVAL
};



struct funcstack {
    Funcstack prev;
    char *name;
    char *filename;
    char *caller;
    zlong flineno;
    zlong lineno;
    int tp;
};



typedef int (*WrapFunc) (Eprog, FuncWrap, char *);

struct funcwrap {
    FuncWrap next;
    int flags;
    WrapFunc handler;
    Module module;
};
// # 1194 "../../Src/zsh.h"
struct options {
    unsigned char ind[128];
    char **args;
    int argscount, argsalloc;
};







typedef int (*HandlerFunc) (char *, char **, Options, int);


struct builtin {
    struct hashnode node;
    HandlerFunc handlerfunc;
    int minargs;
    int maxargs;
    int funcid;
    char *optstr;
    char *defopts;
};
// # 1251 "../../Src/zsh.h"
struct module {
    struct hashnode node;
    union {
 void *handle;
 Linkedmod linked;
 char *alias;
    } u;
    LinkList autoloads;
    LinkList deps;
    int wrapper;
};
// # 1282 "../../Src/zsh.h"
typedef int (*Module_generic_func) (void);
typedef int (*Module_void_func) (Module);
typedef int (*Module_features_func) (Module, char ***);
typedef int (*Module_enables_func) (Module, int **);

struct linkedmod {
    char *name;
    Module_void_func setup;
    Module_features_func features;
    Module_enables_func enables;
    Module_void_func boot;
    Module_void_func cleanup;
    Module_void_func finish;
};





struct features {

    Builtin bn_list;
    int bn_size;

    Conddef cd_list;
    int cd_size;

    MathFunc mf_list;
    int mf_size;

    Paramdef pd_list;
    int pd_size;

    int n_abstract;
};




struct feature_enables {

    char *str;

    Patprog pat;
};



typedef int (*Hookfn) (Hookdef, void *);

struct hookdef {
    Hookdef next;
    char *name;
    Hookfn def;
    int flags;
    LinkList funcs;
};
// # 1349 "../../Src/zsh.h"
struct patprog {
    long startoff;
    long size;
    long mustoff;
    long patmlen;
    int globflags;
    int globend;
    int flags;
    int patnpar;
    char patstartch;
};
// # 1438 "../../Src/zsh.h"
typedef const struct gsu_scalar *GsuScalar;
typedef const struct gsu_integer *GsuInteger;
typedef const struct gsu_float *GsuFloat;
typedef const struct gsu_array *GsuArray;
typedef const struct gsu_hash *GsuHash;

struct gsu_scalar {
    char *(*getfn) (Param);
    void (*setfn) (Param, char *);
    void (*unsetfn) (Param, int);
};

struct gsu_integer {
    zlong (*getfn) (Param);
    void (*setfn) (Param, zlong);
    void (*unsetfn) (Param, int);
};

struct gsu_float {
    double (*getfn) (Param);
    void (*setfn) (Param, double);
    void (*unsetfn) (Param, int);
};

struct gsu_array {
    char **(*getfn) (Param);
    void (*setfn) (Param, char **);
    void (*unsetfn) (Param, int);
};

struct gsu_hash {
    HashTable (*getfn) (Param);
    void (*setfn) (Param, HashTable);
    void (*unsetfn) (Param, int);
};




struct param {
    struct hashnode node;


    union {
 void *data;
 char **arr;
 char *str;
 zlong val;
 zlong *valptr;
 double dval;

        HashTable hash;
    } u;
// # 1500 "../../Src/zsh.h"
    union {
 GsuScalar s;
 GsuInteger i;
 GsuFloat f;
 GsuArray a;
 GsuHash h;
    } gsu;

    int base;
    int width;
    char *env;
    char *ename;
    Param old;
    int level;
};


struct tieddata {
    char ***arrptr;
    int joinchar;
};
// # 1631 "../../Src/zsh.h"
struct repldata {
    int b, e;
    char *replstr;
};
typedef struct repldata *Repldata;




enum {

    ZSHTOK_SUBST = 0x0001,

    ZSHTOK_SHGLOB = 0x0002
};
// # 1676 "../../Src/zsh.h"
struct paramdef {
    char *name;
    int flags;
    void *var;
    const void *gsu;
    GetNodeFunc getnfn;
    ScanTabFunc scantfn;
    Param pm;
};
// # 1727 "../../Src/zsh.h"
enum {
    ASSPM_AUGMENT = 1 << 0,
    ASSPM_WARN_CREATE = 1 << 1
};



struct nameddir {
    struct hashnode node;
    char *dir;
    int diff;
};







typedef struct {

    char *name;

    gid_t gid;
} groupmap;
typedef groupmap *Groupmap;


typedef struct {

    Groupmap array;

    int num;
} groupset;
typedef groupset *Groupset;
// # 1780 "../../Src/zsh.h"
enum loop_return {

    LOOP_OK,

    LOOP_EMPTY,

    LOOP_ERROR
};



enum source_return {

    SOURCE_OK = 0,

    SOURCE_NOT_FOUND = 1,

    SOURCE_ERROR = 2
};







struct histent {
    struct hashnode node;

    Histent up;
    Histent down;
    char *zle_text;




    time_t stim;
    time_t ftim;
    short *words;

    int nwords;
    zlong histnum;
};
// # 1933 "../../Src/zsh.h"
enum {
    OPT_INVALID,
    ALIASESOPT,
    ALLEXPORT,
    ALWAYSLASTPROMPT,
    ALWAYSTOEND,
    APPENDHISTORY,
    AUTOCD,
    AUTOCONTINUE,
    AUTOLIST,
    AUTOMENU,
    AUTONAMEDIRS,
    AUTOPARAMKEYS,
    AUTOPARAMSLASH,
    AUTOPUSHD,
    AUTOREMOVESLASH,
    AUTORESUME,
    BADPATTERN,
    BANGHIST,
    BAREGLOBQUAL,
    BASHAUTOLIST,
    BASHREMATCH,
    BEEP,
    BGNICE,
    BRACECCL,
    BSDECHO,
    CASEGLOB,
    CASEMATCH,
    CBASES,
    CDABLEVARS,
    CHASEDOTS,
    CHASELINKS,
    CHECKJOBS,
    CLOBBER,
    COMBININGCHARS,
    COMPLETEALIASES,
    COMPLETEINWORD,
    CORRECT,
    CORRECTALL,
    CPRECEDENCES,
    CSHJUNKIEHISTORY,
    CSHJUNKIELOOPS,
    CSHJUNKIEQUOTES,
    CSHNULLCMD,
    CSHNULLGLOB,
    DEBUGBEFORECMD,
    EMACSMODE,
    EQUALS,
    ERREXIT,
    ERRRETURN,
    EXECOPT,
    EXTENDEDGLOB,
    EXTENDEDHISTORY,
    EVALLINENO,
    FLOWCONTROL,
    FUNCTIONARGZERO,
    GLOBOPT,
    GLOBALEXPORT,
    GLOBALRCS,
    GLOBASSIGN,
    GLOBCOMPLETE,
    GLOBDOTS,
    GLOBSUBST,
    HASHCMDS,
    HASHDIRS,
    HASHEXECUTABLESONLY,
    HASHLISTALL,
    HISTALLOWCLOBBER,
    HISTBEEP,
    HISTEXPIREDUPSFIRST,
    HISTFCNTLLOCK,
    HISTFINDNODUPS,
    HISTIGNOREALLDUPS,
    HISTIGNOREDUPS,
    HISTIGNORESPACE,
    HISTLEXWORDS,
    HISTNOFUNCTIONS,
    HISTNOSTORE,
    HISTREDUCEBLANKS,
    HISTSAVEBYCOPY,
    HISTSAVENODUPS,
    HISTSUBSTPATTERN,
    HISTVERIFY,
    HUP,
    IGNOREBRACES,
    IGNORECLOSEBRACES,
    IGNOREEOF,
    INCAPPENDHISTORY,
    INTERACTIVE,
    INTERACTIVECOMMENTS,
    KSHARRAYS,
    KSHAUTOLOAD,
    KSHGLOB,
    KSHOPTIONPRINT,
    KSHTYPESET,
    KSHZEROSUBSCRIPT,
    LISTAMBIGUOUS,
    LISTBEEP,
    LISTPACKED,
    LISTROWSFIRST,
    LISTTYPES,
    LOCALOPTIONS,
    LOCALTRAPS,
    LOGINSHELL,
    LONGLISTJOBS,
    MAGICEQUALSUBST,
    MAILWARNING,
    MARKDIRS,
    MENUCOMPLETE,
    MONITOR,
    MULTIBYTE,
    MULTIFUNCDEF,
    MULTIOS,
    NOMATCH,
    NOTIFY,
    NULLGLOB,
    NUMERICGLOBSORT,
    OCTALZEROES,
    OVERSTRIKE,
    PATHDIRS,
    PATHSCRIPT,
    POSIXALIASES,
    POSIXBUILTINS,
    POSIXCD,
    POSIXIDENTIFIERS,
    POSIXJOBS,
    POSIXSTRINGS,
    POSIXTRAPS,
    PRINTEIGHTBIT,
    PRINTEXITVALUE,
    PRIVILEGED,
    PROMPTBANG,
    PROMPTCR,
    PROMPTPERCENT,
    PROMPTSP,
    PROMPTSUBST,
    PUSHDIGNOREDUPS,
    PUSHDMINUS,
    PUSHDSILENT,
    PUSHDTOHOME,
    RCEXPANDPARAM,
    RCQUOTES,
    RCS,
    RECEXACT,
    REMATCHPCRE,
    RESTRICTED,
    RMSTARSILENT,
    RMSTARWAIT,
    SHAREHISTORY,
    SHFILEEXPANSION,
    SHGLOB,
    SHINSTDIN,
    SHNULLCMD,
    SHOPTIONLETTERS,
    SHORTLOOPS,
    SHWORDSPLIT,
    SINGLECOMMAND,
    SINGLELINEZLE,
    SOURCETRACE,
    SUNKEYBOARDHACK,
    TRANSIENTRPROMPT,
    TRAPSASYNC,
    TYPESETSILENT,
    UNSET,
    VERBOSE,
    VIMODE,
    WARNCREATEGLOBAL,
    XTRACE,
    USEZLE,
    DVORAK,
    OPT_SIZE
};
// # 2120 "../../Src/zsh.h"
struct ttyinfo {

    struct termios tio;
// # 2134 "../../Src/zsh.h"
    struct winsize winsize;

};
// # 2286 "../../Src/zsh.h"
enum {

    TSC_RAW = 0x0001,

    TSC_PROMPT = 0x0002,

    TSC_OUTPUT_MASK = 0x0003,

    TSC_DIRTY = 0x0004
};
// # 2354 "../../Src/zsh.h"
typedef unsigned int Heapid;
// # 2399 "../../Src/zsh.h"
struct heapstack {
    struct heapstack *next;
    size_t used;



};



struct heap {
    struct heap *next;
    size_t size;
    size_t used;
    struct heapstack *sp;
// # 2426 "../../Src/zsh.h"
}



;
// # 2475 "../../Src/zsh.h"
enum trap_state {

    TRAP_STATE_INACTIVE,




    TRAP_STATE_PRIMED,




    TRAP_STATE_FORCE_RETURN
};
// # 2497 "../../Src/zsh.h"
typedef int (*CompareFn) (const void *, const void *);

enum {
    SORTIT_ANYOLDHOW = 0,
    SORTIT_IGNORING_CASE = 1,
    SORTIT_NUMERICALLY = 2,
    SORTIT_BACKWARDS = 4,




    SORTIT_IGNORING_BACKSLASHES = 8,




    SORTIT_SOMEHOW = 16,
};




struct sortelt {

    char *orig;

    const char *cmp;




    int origlen;




    int len;
};

typedef struct sortelt *SortElt;





enum {
    CASMOD_NONE,
    CASMOD_UPPER,
    CASMOD_LOWER,
    CASMOD_CAPS
};
// # 2557 "../../Src/zsh.h"
enum {





    GETKEY_OCTAL_ESC = (1 << 0),






    GETKEY_EMACS = (1 << 1),

    GETKEY_CTRL = (1 << 2),

    GETKEY_BACKSLASH_C = (1 << 3),

    GETKEY_DOLLAR_QUOTE = (1 << 4),

    GETKEY_BACKSLASH_MINUS = (1 << 5),

    GETKEY_SINGLE_CHAR = (1 << 6),





    GETKEY_UPDATE_OFFSET = (1 << 7),



    GETKEY_PRINTF_PERCENT = (1 << 8)
};
// # 2631 "../../Src/zsh.h"
enum {
    ZLCON_LINE_START,
    ZLCON_LINE_CONT,
    ZLCON_SELECT,
    ZLCON_VARED
};







typedef int (*CompctlReadFn) (char *, char **, Options, char *);



typedef char * (*ZleEntryPoint)(int cmd, va_list ap);



enum {
    ZLE_CMD_GET_LINE,
    ZLE_CMD_READ,
    ZLE_CMD_ADD_TO_LINE,
    ZLE_CMD_TRASH,
    ZLE_CMD_RESET_PROMPT,
    ZLE_CMD_REFRESH,
    ZLE_CMD_SET_KEYMAP,
    ZLE_CMD_GET_KEY
};
// # 2674 "../../Src/zsh.h"
typedef wint_t convchar_t;
// # 19 "../../Src/zsh.mdh" 2
// # 1 "../../Src/sigcount.h" 1
// # 20 "../../Src/zsh.mdh" 2
// # 1 "../../Src/signals.h" 1
// # 106 "../../Src/signals.h"
extern sigset_t signal_block (sigset_t);


extern sigset_t signal_unblock (sigset_t);
// # 21 "../../Src/zsh.mdh" 2
// # 1 "../../Src/prototypes.h" 1
// # 22 "../../Src/zsh.mdh" 2
// # 1 "../../Src/hashtable.h" 1
// # 23 "../../Src/zsh.mdh" 2
// # 1 "../../Src/ztype.h" 1
// # 24 "../../Src/zsh.mdh" 2
// # 34 "../../Src/zsh.mdh"
// # 1 "../../Src/builtin.epro" 1




extern HashTable builtintab;
extern void createbuiltintable (void);
extern void init_builtins (void);
extern int execbuiltin (LinkList args,Builtin bn);
extern int bin_enable (char*name,char**argv,Options ops,int func);
extern int bin_set (char*nam,char**args,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int doprintdir;
extern int bin_pwd (char*name __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops,int func __attribute__((__unused__)));
extern LinkList dirstack;
extern int bin_dirs (char*name __attribute__((__unused__)),char**argv,Options ops,int func __attribute__((__unused__)));
extern void set_pwd_env (void);
extern int bin_cd (char*nam,char**argv,Options ops,int func);
extern char*cd_able_vars (char*s);
extern int fixdir (char*src);
extern void printqt (char*str);
extern void printif (char*str,int c);
extern int bin_fc (char*nam,char**argv,Options ops,int func);
extern int bin_typeset (char*name,char**argv,Options ops,int func);
extern int eval_autoload (Shfunc shf,char*name,Options ops,int func);
extern int bin_functions (char*name,char**argv,Options ops,int func);
extern Eprog mkautofn (Shfunc shf);
extern int bin_unset (char*name,char**argv,Options ops,int func);
extern int bin_whence (char*nam,char**argv,Options ops,int func);
extern int bin_hash (char*name,char**argv,Options ops,int func __attribute__((__unused__)));
extern int bin_unhash (char*name,char**argv,Options ops,int func __attribute__((__unused__)));
extern int bin_alias (char*name,char**argv,Options ops,int func __attribute__((__unused__)));
extern int bin_true (char*name __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_false (char*name __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern LinkList bufstack;
extern int bin_print (char*name,char**args,Options ops,int func);
extern int bin_shift (char*name,char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int optcind;
extern int bin_getopts (char*name __attribute__((__unused__)),char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int exit_pending;
extern int bin_break (char*name,char**argv,Options ops __attribute__((__unused__)),int func);
extern int stopmsg;
extern void zexit (int val,int from_where);
extern int bin_dot (char*name,char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_emulate (char*nam __attribute__((__unused__)),char**argv,Options ops,int func __attribute__((__unused__)));
extern int ineval;
extern int bin_eval (char*nam __attribute__((__unused__)),char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_read (char*name,char**args,Options ops,int func __attribute__((__unused__)));
extern char**testargs;
extern char**curtestarg;
extern void testlex (void);
extern int bin_test (char*name,char**argv,Options ops __attribute__((__unused__)),int func);
extern int bin_times (char*name __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_trap (char*name,char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_ttyctl (char*name __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops,int func __attribute__((__unused__)));
extern int bin_let (char*name __attribute__((__unused__)),char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int bin_umask (char*nam,char**args,Options ops,int func __attribute__((__unused__)));
extern int bin_notavail (char*nam,char**argv __attribute__((__unused__)),Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
// # 35 "../../Src/zsh.mdh" 2
// # 1 "../../Src/compat.epro" 1




extern long zpathmax (char*dir);
extern long zopenmax (void);
extern char*zgetdir (struct dirsav*d);
extern char*zgetcwd (void);
extern int zchdir (char*dir);
// # 36 "../../Src/zsh.mdh" 2
// # 1 "../../Src/cond.epro" 1




extern int evalcond (Estate state,char*fromtest);
extern char*cond_str (char**args,int num,int raw);
extern zlong cond_val (char**args,int num);
extern int cond_match (char**args,int num,char*str);
// # 37 "../../Src/zsh.mdh" 2
// # 1 "../../Src/exec.epro" 1




extern int noerrexit;
extern int noerrs;
extern int nohistsave;
extern int errflag;
extern int trap_state;
extern int trap_return;
extern int subsh;
extern int retflag;
extern long lastval2;
extern unsigned char*fdtable;
extern int fdtable_size;
extern int max_zsh_fd;
extern int coprocin;
extern int coprocout;
extern int fdtable_flocks;
extern int zleactive;
extern pid_t cmdoutpid;
extern int cmdoutval;
extern int use_cmdoutval;
extern int sfcontext;
extern struct execstack*exstack;
extern Funcstack funcstack;
extern Eprog parse_string (char*s,int reset_lineno);

extern struct rlimit current_limits[__RLIM_NLIMITS];
extern struct rlimit limits[__RLIM_NLIMITS];
extern int zsetlimit (int limnum,char*nam);
extern int setlimits (char*nam);

extern int list_pipe;
extern int simple_pline;
extern char*findcmd (char*arg0,int docopy);
extern int iscom (char*s);
extern int isreallycom (Cmdnam cn);
extern int isrelative (char*s);
extern Cmdnam hashcmd (char*arg0,char**pp);
extern int forklevel;
extern void execstring (char*s,int dont_change_job,int exiting,char*context);
extern void execode (Eprog p,int dont_change_job,int exiting,char*context);
extern void execlist (Estate state,int dont_change_job,int exiting);
extern void untokenize (char*s);
extern void quote_tokenized_output (char*str,FILE*file);
extern void setunderscore (char*str);
extern void execsubst (LinkList strs);
extern void closem (int how);
extern char*gethere (char**strp,int typ);
extern LinkList getoutput (char*cmd,int qt);
extern LinkList readoutput (int in,int qt);
extern char*getoutputfile (char*cmd,char**eptr);
extern char*getproc (char*cmd,char**eptr);
extern Shfunc loadautofn (Shfunc shf,int fksh,int autol);
extern int doshfunc (Shfunc shfunc,LinkList doshargs,int noreturnval);
extern void runshfunc (Eprog prog,FuncWrap wrap,char*name);
extern Eprog getfpfunc (char*s,int*ksh,char**fname);
extern Eprog stripkshdef (Eprog prog,char*name);
extern void execsave (void);
extern void execrestore (void);
// # 38 "../../Src/zsh.mdh" 2
// # 1 "../../Src/glob.epro" 1




extern int badcshglob;
extern int pathpos;
extern char*pathbuf;
extern char*glob_pre;
extern char*glob_suf;
extern int haswilds (char*str);
extern void zglob (LinkList list,LinkNode np,int nountok);
extern char file_type (mode_t filemode);
extern int hasbraces (char*str);
extern int xpandredir (struct redir*fn,LinkList redirtab);
extern void xpandbraces (LinkList list,LinkNode*np);
extern int matchpat (char*a,char*b);
extern int getmatch (char**sp,char*pat,int fl,int n,char*replstr);
extern void getmatcharr (char***ap,char*pat,int fl,int n,char*replstr);
extern int getmatchlist (char*str,Patprog p,LinkList*repllistp);
extern void freematchlist (LinkList repllist);



extern void tokenize (char*s);
extern void shtokenize (char*s);
extern void remnulargs (char*s);
// # 39 "../../Src/zsh.mdh" 2
// # 1 "../../Src/hashtable.epro" 1




extern unsigned hasher (const char*str);
extern HashTable newhashtable (int size,char const*name __attribute__((__unused__)),PrintTableStats printinfo __attribute__((__unused__)));
extern void deletehashtable (HashTable ht);
extern void addhashnode (HashTable ht,char*nam,void*nodeptr);
extern HashNode addhashnode2 (HashTable ht,char*nam,void*nodeptr);
extern HashNode gethashnode (HashTable ht,const char*nam);
extern HashNode gethashnode2 (HashTable ht,const char*nam);
extern HashNode removehashnode (HashTable ht,const char*nam);
extern void disablehashnode (HashNode hn,int flags __attribute__((__unused__)));
extern void enablehashnode (HashNode hn,int flags __attribute__((__unused__)));
extern int scanmatchtable (HashTable ht,Patprog pprog,int sorted,int flags1,int flags2,ScanFunc scanfunc,int scanflags);
extern int scanhashtable (HashTable ht,int sorted,int flags1,int flags2,ScanFunc scanfunc,int scanflags);
extern void emptyhashtable (HashTable ht);



extern HashTable cmdnamtab;
extern char**pathchecked;
extern void createcmdnamtable (void);
extern void hashdir (char**dirp);
extern HashTable shfunctab;
extern void createshfunctable (void);
extern HashTable reswdtab;
extern void createreswdtable (void);
extern HashTable aliastab;
extern HashTable sufaliastab;
extern void createaliastable (HashTable ht);
extern void createaliastables (void);
extern Alias createaliasnode (char*txt,int flags);
extern void createhisttable (void);
extern unsigned histhasher (const char*str);
extern void emptyhisttable (HashTable ht);
extern int histstrcmp (const char*str1,const char*str2);
extern void addhistnode (HashTable ht,char*nam,void*nodeptr);
extern void freehistnode (HashNode nodeptr);
extern void freehistdata (Histent he,int unlink);
// # 40 "../../Src/zsh.mdh" 2
// # 1 "../../Src/hashnameddir.epro" 1




extern HashTable nameddirtab;
extern void createnameddirtable (void);
// # 41 "../../Src/zsh.mdh" 2
// # 1 "../../Src/hist.epro" 1




extern int(*hgetc)(void);
extern void(*hungetc)(int);
extern void(*hwaddc)(int);
extern void(*hwbegin)(int);
extern void(*hwend)(void);
extern void(*addtoline)(int);
extern int stophist;
extern int expanding;
extern int excs;
extern int exlast;
extern zlong curhist;
extern struct histent curline;
extern zlong histlinect;
extern HashTable histtab;
extern Histent hist_ring;
extern zlong histsiz;
extern zlong savehistsiz;
extern int histdone;
extern int histactive;
extern int hist_ignore_all_dups;
extern int hist_skip_flags;
extern short*chwords;
extern int chwordlen;
extern int chwordpos;
extern char*hsubl;
extern char*hsubr;
extern char*hptr;
extern char*chline;
extern char*zle_chline;
extern int qbang;
extern int hlinesz;
extern void iaddtoline (int c);
extern void herrflush (void);
extern void strinbeg (int dohist);
extern void strinend (void);
extern void hbegin (int dohist);
extern void histreduceblanks (void);
extern void histremovedups (void);
extern zlong addhistnum (zlong hl,int n,int xflags);
extern Histent movehistent (Histent he,int n,int xflags);
extern Histent up_histent (Histent he);
extern Histent down_histent (Histent he);
extern Histent gethistent (zlong ev,int nearmatch);
extern Histent prepnexthistent (void);
extern int hend (Eprog prog);
extern int hwgetword;
extern void ihwbegin (int offset);
extern void ihwend (void);
extern void histbackword (void);
extern void hwrep (char*rep);
extern char*hgetline (void);
extern zlong hcomsearch (char*str);
extern int chabspath (char**junkptr);
extern int chrealpath (char**junkptr);
extern int remtpath (char**junkptr);
extern int remtext (char**junkptr);
extern int rembutext (char**junkptr);
extern int remlpaths (char**junkptr);
extern char*casemodify (char*str,int how);
extern int subst (char**strptr,char*in,char*out,int gbal);
extern void checkcurline (Histent he);
extern Histent quietgethist (int ev);
extern int quote (char**tr);
extern void inithist (void);
extern void resizehistents (void);
extern void readhistfile (char*fn,int err,int readflags);
extern void savehistfile (char*fn,int err,int writeflags);
extern int lockhistfile (char*fn,int keep_trying);
extern void unlockhistfile (char*fn);
extern int histfileIsLocked (void);
extern LinkList bufferwords (LinkList list,char*buf,int*index,int flags);
extern void histsplitwords (char*lineptr,short**wordsp,int*nwordsp,int*nwordposp,int uselex);
extern int pushhiststack (char*hf,zlong hs,zlong shs,int level);
extern int pophiststack (void);
extern int saveandpophiststack (int pop_through,int writeflags);
// # 42 "../../Src/zsh.mdh" 2
// # 1 "../../Src/init.epro" 1




extern int noexitct;
extern char*zunderscore;
extern int underscorelen;
extern int underscoreused;
extern int sourcelevel;
extern int SHTTY;
extern FILE*shout;
extern char*tcstr[34];
extern int tclen[34];
extern int tclines;
extern int tccolumns;
extern int hasam;
extern int hasxn;
extern int tccolours;
extern sigset_t sigchld_mask;
extern struct hookdef zshhooks[];
extern enum loop_return loop (int toplevel,int justonce);
extern char*parseopts (char*nam,char***argvp);
extern void init_io (void);
extern void init_shout (void);
extern char*tccap_get_name (int cap);
extern int init_term (void);
extern void setupvals (void);
extern void init_signals (void);
extern void run_init_scripts (void);
extern void init_misc (void);
extern enum source_return source (char*s);
extern void sourcehome (char*s);
extern void init_bltinmods (void);
extern void noop_function (void);
extern void noop_function_int (int nothing __attribute__((__unused__)));
extern ZleEntryPoint zle_entry_ptr;
extern int zle_load_state;
extern char * zleentry(int cmd, ...);
extern CompctlReadFn compctlreadptr;
extern int fallback_compctlread (char*name,char**args __attribute__((__unused__)),Options ops __attribute__((__unused__)),char*reply __attribute__((__unused__)));
extern int use_exit_printed;
extern int zsh_main (int argc __attribute__((__unused__)),char**argv);
// # 43 "../../Src/zsh.mdh" 2
// # 1 "../../Src/input.epro" 1




extern int SHIN;
extern FILE*bshin;
extern int strin;
extern int inbufct;
extern int inbufflags;
extern char*shingetline (void);
extern int ingetc (void);
extern void inungetc (int c);
extern int stuff (char*fn);
extern void inerrflush (void);
extern void inpush (char*str,int flags,Alias inalias);
extern void inpop (void);
extern void inpopalias (void);
extern char*ingetptr (void);
// # 44 "../../Src/zsh.mdh" 2
// # 1 "../../Src/jobs.epro" 1




extern pid_t origpgrp;
extern pid_t mypgrp;
extern int thisjob;
extern int curjob;
extern int prevjob;
extern struct job*jobtab;
extern int jobtabsize;
extern int maxjob;



extern int ttyfrozen;
extern int prev_errflag;
extern int prev_breaks;
extern int errbrk_saved;
extern int numpipestats;
extern int pipestats[256];
extern long lastpid_status;
extern void makerunning (Job jn);
extern int findproc (pid_t pid,Job*jptr,Process*pptr,int aux);
extern int hasprocs (int job);
extern void get_usage (void);
extern void update_process (Process pn,int status);
extern void check_cursh_sig (int sig);
extern void update_job (Job jn);


extern int printjob (Job jn,int lng,int synch);
extern void deletefilelist (LinkList file_list,int disowning);
extern void freejob (Job jn,int deleting);
extern void deletejob (Job jn,int disowning);
extern void addproc (pid_t pid,char*text,int aux,struct timeval*bgtime);
extern int havefiles (void);
extern int waitforpid (pid_t pid,int wait_cmd);
extern void waitjobs (void);
extern void clearjobtab (int monitor);
extern int initjob (void);
extern void setjobpwd (void);
extern void spawnjob (void);
extern void shelltime (void);
extern void scanjobs (void);
extern int getjob (const char*s,const char*prog);
extern void init_jobs (char**argv,char**envp);
extern int expandjobtab (void);
extern void maybeshrinkjobtab (void);
extern int bin_fg (char*name,char**argv,Options ops,int func);
extern int bin_kill (char*nam,char**argv,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern int getsignum (const char*s);
extern const char*getsigname (int sig);
extern HashNode gettrapnode (int sig,int ignoredisable);
extern void removetrapnode (int sig);
extern int bin_suspend (char*name,char**argv __attribute__((__unused__)),Options ops,int func __attribute__((__unused__)));
extern int findjobnam (const char*s);
extern void acquire_pgrp (void);
extern void release_pgrp (void);
// # 45 "../../Src/zsh.mdh" 2
// # 1 "../../Src/lex.epro" 1




extern char ztokens[];
extern char*zshlextext;
extern char*tokstr;
extern int tok;
extern int tokfd;
extern zlong toklineno;
extern int lexstop;
extern int isfirstln;
extern int isfirstch;
extern int inalmore;
extern int nocorrect;
extern int zlemetacs;
extern int zlemetall;
extern int inwhat;
extern int addedx;
extern int wb;
extern int we;
extern int noaliases;
extern int lexflags;
extern int wordbeg;
extern int parbegin;
extern int parend;
extern int nocomments;
extern char*tokstrings[WHILE+1];
extern void lexsave (void);
extern void lexrestore (void);
extern void zshlex (void);
extern void ctxtlex (void);
extern void initlextabs (void);
extern void lexinit (void);
extern void add (int c);
extern int parsestr (char*s);
extern int parsestrnoerr (char*s);
extern char*parse_subscript (char*s,int sub,int endchar);
extern int parse_subst_string (char*s);
extern void gotword (void);
extern int exalias (void);
// # 46 "../../Src/zsh.mdh" 2
// # 1 "../../Src/linklist.epro" 1




extern LinkList newlinklist (void);
extern LinkList znewlinklist (void);
extern LinkNode insertlinknode (LinkList list,LinkNode node,void*dat);
extern LinkNode zinsertlinknode (LinkList list,LinkNode node,void*dat);
extern LinkNode uinsertlinknode (LinkList list,LinkNode node,LinkNode new);
extern void insertlinklist (LinkList l,LinkNode where,LinkList x);
extern void*getlinknode (LinkList list);
extern void*ugetnode (LinkList list);
extern void*remnode (LinkList list,LinkNode nd);
extern void*uremnode (LinkList list,LinkNode nd);
extern void freelinklist (LinkList list,FreeFunc freefunc);
extern int countlinknodes (LinkList list);
extern void rolllist (LinkList l,LinkNode nd);
extern LinkList newsizedlist (int size);
extern LinkNode linknodebydatum (LinkList list,void*dat);
extern LinkNode linknodebystring (LinkList list,char*dat);
extern char**hlinklist2array (LinkList list,int copy);
extern char**zlinklist2array (LinkList list);
// # 47 "../../Src/zsh.mdh" 2
// # 1 "../../Src/loop.epro" 1




extern int loops;
extern int contflag;
extern int breaks;
extern int execfor (Estate state,int do_exec);
extern int execselect (Estate state,int do_exec __attribute__((__unused__)));
extern size_t selectlist (LinkList l,size_t start);
extern int execwhile (Estate state,int do_exec __attribute__((__unused__)));
extern int execrepeat (Estate state,int do_exec __attribute__((__unused__)));
extern int execif (Estate state,int do_exec);
extern int execcase (Estate state,int do_exec);
extern zlong try_errflag;
extern zlong try_tryflag;
extern int exectry (Estate state,int do_exec);
// # 48 "../../Src/zsh.mdh" 2
// # 1 "../../Src/math.epro" 1




extern int noeval;
extern mnumber zero_mnumber;
extern mnumber lastmathval;
extern int lastbase;
extern int outputradix;
extern void op (int what);
extern mnumber matheval (char*s);
extern zlong mathevali (char*s);
extern zlong mathevalarg (char*s,char**ss);
// # 49 "../../Src/zsh.mdh" 2
// # 1 "../../Src/mem.epro" 1







extern Heap new_heaps (void);
extern void old_heaps (Heap old);
extern Heap switch_heaps (Heap new);
extern void pushheap (void);
extern void freeheap (void);
extern void popheap (void);
extern void*zheapptr (void*p);
extern void*zhalloc (size_t size);
extern void*hrealloc (char*p,size_t old,size_t new);



extern void*hcalloc (size_t size);
extern void*zalloc (size_t size);
extern void*zshcalloc (size_t size);
extern void*zrealloc (void*ptr,size_t size);





extern void zfree (void*p,int sz __attribute__((__unused__)));
extern void zsfree (char*p);
// # 50 "../../Src/zsh.mdh" 2
// # 1 "../../Src/module.epro" 1




extern LinkList linkedmodules;
extern char**module_path;
extern HashTable modulestab;
extern HashTable newmoduletable (int size,char const*name);
extern int setup_ (Module m __attribute__((__unused__)));
extern int features_ (Module m __attribute__((__unused__)),char***features __attribute__((__unused__)));
extern int enables_ (Module m __attribute__((__unused__)),int**enables __attribute__((__unused__)));
extern int boot_ (Module m __attribute__((__unused__)));
extern int cleanup_ (Module m __attribute__((__unused__)));
extern int finish_ (Module m __attribute__((__unused__)));
extern void register_module (char*n,Module_void_func setup,Module_features_func features,Module_enables_func enables,Module_void_func boot,Module_void_func cleanup,Module_void_func finish);
extern Linkedmod module_linked (char const*name);
extern int deletebuiltin (const char*nam);
extern int addbuiltins (char const*nam,Builtin binl,int size);
extern FuncWrap wrappers;
extern int addwrapper (Module m,FuncWrap w);
extern int deletewrapper (Module m,FuncWrap w);
extern Conddef condtab;
extern Conddef getconddef (int inf,const char*name,int autol);
extern int deleteconddef (Conddef c);
extern Hookdef hooktab;
extern Hookdef gethookdef (char*n);
extern int addhookdef (Hookdef h);
extern int addhookdefs (Module m,Hookdef h,int size);
extern int deletehookdef (Hookdef h);
extern int deletehookdefs (Module m __attribute__((__unused__)),Hookdef h,int size);
extern int addhookdeffunc (Hookdef h,Hookfn f);
extern int addhookfunc (char*n,Hookfn f);
extern int deletehookdeffunc (Hookdef h,Hookfn f);
extern int deletehookfunc (char*n,Hookfn f);
extern int runhookdef (Hookdef h,void*d);
extern int addparamdef (Paramdef d);
extern int deleteparamdef (Paramdef d);
extern MathFunc mathfuncs;
extern void removemathfunc (MathFunc previous,MathFunc current);
extern MathFunc getmathfunc (const char*name,int autol);
extern int deletemathfunc (MathFunc f);
// # 50 "../../Src/module.epro"
extern int module_loaded (const char*name);






extern int load_module (char const*name,Feature_enables enablesarr,int silent);
extern int require_module (const char*module,Feature_enables features);
extern void add_dep (const char*name,char*from);
extern int bin_zmodload (char*nam,char**args,Options ops,int func __attribute__((__unused__)));
extern int unload_module (Module m);
extern int unload_named_module (char*modname,char*nam,int silent);
extern char**featuresarray (Module m __attribute__((__unused__)),Features f);
extern int*getfeatureenables (Module m __attribute__((__unused__)),Features f);
extern int setfeatureenables (Module m,Features f,int*e);
extern int handlefeatures (Module m,Features f,int**enables);
extern int ensurefeature (const char*modname,const char*prefix,const char*feature);
extern int autofeatures (const char*cmdnam,const char*module,char**features,int prefchar,int defflags);
// # 51 "../../Src/zsh.mdh" 2
// # 1 "../../Src/options.epro" 1




extern int emulation;
extern int sticky_emulation;
extern char opts[OPT_SIZE];
extern HashTable optiontab;
extern void createoptiontable (void);
extern void installemulation (void);
extern void emulate (const char*zsh_name,int fully);
extern int bin_setopt (char*nam,char**args,Options ops __attribute__((__unused__)),int isun);
extern int optlookup (char const*name);
extern int optlookupc (char c);
extern int dosetopt (int optno,int value,int force);
extern char*dashgetfn (Param pm __attribute__((__unused__)));
extern void printoptionstates (int hadplus);
extern void printoptionlist (void);
// # 52 "../../Src/zsh.mdh" 2
// # 1 "../../Src/params.epro" 1




extern int locallevel;
extern char**pparams;
extern char**cdpath;
extern char**fpath;
extern char**mailpath;
extern char**manpath;
extern char**psvar;
extern char**watch;
extern char**zsh_eval_context;
extern char**path;
extern char**fignore;
extern char*argzero;
extern char*home;
extern char*nullcmd;
extern char*oldpwd;
extern char*zoptarg;
extern char*prompt;
extern char*prompt2;
extern char*prompt3;
extern char*prompt4;
extern char*readnullcmd;
extern char*rprompt;
extern char*rprompt2;
extern char*sprompt;
extern char*wordchars;
extern char*zsh_name;
extern char*ifs;
extern char*postedit;
extern char*term;
extern char*zsh_terminfo;
extern char*ttystrname;
extern char*pwd;
extern zlong lastval;
extern zlong mypid;
extern zlong lastpid;
extern zlong zterm_columns;
extern zlong zterm_lines;
extern zlong ppid;
extern zlong zsh_subshell;
extern zlong lineno;
extern zlong zoptind;
extern zlong shlvl;
extern unsigned char bangchar;
extern unsigned char hatchar;
extern unsigned char hashchar;
extern unsigned char keyboardhackchar;
extern struct timeval shtimer;
extern int termflags;
extern const struct gsu_scalar stdscalar_gsu;
extern const struct gsu_scalar varscalar_gsu;
extern const struct gsu_scalar nullsetscalar_gsu;
extern const struct gsu_integer stdinteger_gsu;
extern const struct gsu_integer varinteger_gsu;
extern const struct gsu_integer nullsetinteger_gsu;
extern const struct gsu_float stdfloat_gsu;
extern const struct gsu_array stdarray_gsu;
extern const struct gsu_array vararray_gsu;
extern const struct gsu_hash stdhash_gsu;
extern const struct gsu_hash nullsethash_gsu;
extern HashTable paramtab;
extern HashTable realparamtab;
extern HashTable newparamtable (int size,char const*name);
extern HashTable copyparamtable (HashTable ht,char*name);
extern void deleteparamtable (HashTable t);
extern void scancountparams (HashNode hn __attribute__((__unused__)),int flags);
extern char**paramvalarr (HashTable ht,int flags);
extern void createparamtable (void);
extern void assigngetset (Param pm);
extern Param createparam (char*name,int flags);
extern Param createspecialhash (char*name,GetNodeFunc get,ScanTabFunc scan,int flags);
extern void copyparam (Param tpm,Param pm,int fakecopy);
extern int isident (char*s);
extern int getindex (char**pptr,Value v,int flags);
extern Value getvalue (Value v,char**pptr,int bracks);
extern Value fetchvalue (Value v,char**pptr,int bracks,int flags);
extern char*getstrvalue (Value v);
extern char**getarrvalue (Value v);
extern zlong getintvalue (Value v);
extern mnumber getnumvalue (Value v);
extern void export_param (Param pm);
extern void setstrvalue (Value v,char*val);
extern void setnumvalue (Value v,mnumber val);
extern void setarrvalue (Value v,char**val);
extern zlong getiparam (char*s);
extern mnumber getnparam (char*s);
extern char*getsparam (char*s);
extern char**getaparam (char*s);
extern char**gethparam (char*s);
extern char**gethkparam (char*s);
extern Param assignsparam (char*s,char*val,int flags);
extern Param assignaparam (char*s,char**val,int flags);
extern Param sethparam (char*s,char**val);
extern Param setnparam (char*s,mnumber val);
extern Param setiparam (char*s,zlong val);
extern void unsetparam (char*s);
extern int unsetparam_pm (Param pm,int altflag,int exp);
extern void stdunsetfn (Param pm,int exp __attribute__((__unused__)));
extern zlong intgetfn (Param pm);
extern char*strgetfn (Param pm);
extern void strsetfn (Param pm,char*x);
extern char**arrgetfn (Param pm);
extern void arrsetfn (Param pm,char**x);
extern HashTable hashgetfn (Param pm);
extern void hashsetfn (Param pm,HashTable x);
extern void nullsethashfn (Param pm __attribute__((__unused__)),HashTable x);
extern void arrhashsetfn (Param pm,char**val,int augment);
extern void nullstrsetfn (Param pm __attribute__((__unused__)),char*x);
extern void nullintsetfn (Param pm __attribute__((__unused__)),zlong x __attribute__((__unused__)));
extern void nullunsetfn (Param pm __attribute__((__unused__)),int exp __attribute__((__unused__)));
extern zlong intvargetfn (Param pm);
extern void intvarsetfn (Param pm,zlong x);
extern void zlevarsetfn (Param pm,zlong x);
extern void strvarsetfn (Param pm,char*x);
extern char*strvargetfn (Param pm);
extern char**arrvargetfn (Param pm);
extern void arrvarsetfn (Param pm,char**x);
extern char*colonarrgetfn (Param pm);
extern void colonarrsetfn (Param pm,char*x);
extern char*tiedarrgetfn (Param pm);
extern void tiedarrsetfn (Param pm,char*x);
extern void tiedarrunsetfn (Param pm,int exp __attribute__((__unused__)));
extern HashTable newuniqtable (zlong size);
extern void uniqarray (char**x);
extern void zhuniqarray (char**x);
extern zlong poundgetfn (Param pm __attribute__((__unused__)));
extern zlong randomgetfn (Param pm __attribute__((__unused__)));
extern void randomsetfn (Param pm __attribute__((__unused__)),zlong v);
extern zlong intsecondsgetfn (Param pm __attribute__((__unused__)));
extern void intsecondssetfn (Param pm __attribute__((__unused__)),zlong x);
extern double floatsecondsgetfn (Param pm __attribute__((__unused__)));
extern void floatsecondssetfn (Param pm __attribute__((__unused__)),double x);
extern double getrawseconds (void);
extern void setrawseconds (double x);
extern int setsecondstype (Param pm,int on,int off);
extern char*usernamegetfn (Param pm __attribute__((__unused__)));
extern void usernamesetfn (Param pm __attribute__((__unused__)),char*x);
extern zlong uidgetfn (Param pm __attribute__((__unused__)));
extern void uidsetfn (Param pm __attribute__((__unused__)),zlong x);
extern zlong euidgetfn (Param pm __attribute__((__unused__)));
extern void euidsetfn (Param pm __attribute__((__unused__)),zlong x);
extern zlong gidgetfn (Param pm __attribute__((__unused__)));
extern void gidsetfn (Param pm __attribute__((__unused__)),zlong x);
extern zlong egidgetfn (Param pm __attribute__((__unused__)));
extern void egidsetfn (Param pm __attribute__((__unused__)),zlong x);
extern zlong ttyidlegetfn (Param pm __attribute__((__unused__)));
extern char*ifsgetfn (Param pm __attribute__((__unused__)));
extern void ifssetfn (Param pm __attribute__((__unused__)),char*x);
extern void lc_allsetfn (Param pm,char*x);
extern void langsetfn (Param pm,char*x);
extern void lcsetfn (Param pm,char*x);
extern zlong histsizegetfn (Param pm __attribute__((__unused__)));
extern void histsizesetfn (Param pm __attribute__((__unused__)),zlong v);
extern zlong savehistsizegetfn (Param pm __attribute__((__unused__)));
extern void savehistsizesetfn (Param pm __attribute__((__unused__)),zlong v);
extern void errnosetfn (Param pm __attribute__((__unused__)),zlong x);
extern zlong errnogetfn (Param pm __attribute__((__unused__)));
extern char*keyboardhackgetfn (Param pm __attribute__((__unused__)));
extern void keyboardhacksetfn (Param pm __attribute__((__unused__)),char*x);
extern char*histcharsgetfn (Param pm __attribute__((__unused__)));
extern void histcharssetfn (Param pm __attribute__((__unused__)),char*x);
extern char*homegetfn (Param pm __attribute__((__unused__)));
extern void homesetfn (Param pm __attribute__((__unused__)),char*x);
extern char*wordcharsgetfn (Param pm __attribute__((__unused__)));
extern void wordcharssetfn (Param pm __attribute__((__unused__)),char*x);
extern char*underscoregetfn (Param pm __attribute__((__unused__)));
extern char*termgetfn (Param pm __attribute__((__unused__)));
extern void termsetfn (Param pm __attribute__((__unused__)),char*x);
extern char*terminfogetfn (Param pm __attribute__((__unused__)));
extern void terminfosetfn (Param pm,char*x);
extern void arrfixenv (char*s,char**t);
extern int zputenv (char*str);


extern char*zgetenv (char*name);
extern void addenv (Param pm,char*value);
extern void delenvvalue (char*x);
extern void delenv (Param pm);
extern void convbase (char*s,zlong v,int base);
extern char*convfloat (double dval,int digits,int flags,FILE*fout);
extern void startparamscope (void);
extern void endparamscope (void);
extern void freeparamnode (HashNode hn);
extern void printparamnode (HashNode hn,int printflags);
// # 53 "../../Src/zsh.mdh" 2
// # 1 "../../Src/parse.epro" 1




extern int incmdpos;
extern int aliasspaceflag;
extern int incond;
extern int inredir;
extern int incasepat;
extern int isnewlin;
extern int infor;
extern struct heredocs*hdocs;
extern int eclen;
extern int ecused;
extern int ecnpats;
extern Wordcode ecbuf;
extern Eccstr ecstrs;
extern int ecsoffs;
extern int ecssub;
extern int ecnfunc;
extern int empty_eprog (Eprog p);
extern Eprog parse_event (void);
extern Eprog parse_list (void);
extern Eprog parse_cond (void);
extern void setheredoc (int pc,int type,char*str,char*termstr,char*munged_termstr);
extern void(*condlex)(void);
extern Eprog dupeprog (Eprog p,int heap);
extern void useeprog (Eprog p);
extern void freeeprog (Eprog p);
extern char*ecgetstr (Estate s,int dup,int*tok);
extern char*ecrawstr (Eprog p,Wordcode pc,int*tok);
extern char**ecgetarr (Estate s,int num,int dup,int*tok);
extern LinkList ecgetlist (Estate s,int num,int dup,int*tok);
extern LinkList ecgetredirs (Estate s);
extern struct eprog dummy_eprog;
extern void init_eprog (void);
extern int bin_zcompile (char*nam,char**args,Options ops,int func __attribute__((__unused__)));







extern Eprog try_dump_file (char*path,char*name,char*file,int*ksh);
extern Eprog try_source_file (char*file);
extern void incrdumpcount (FuncDump f);
extern void decrdumpcount (FuncDump f);
extern void closedumps (void);
extern void closedumps (void);
extern int dump_autoload (char*nam,char*file,int on,Options ops,int func);
// # 54 "../../Src/zsh.mdh" 2
// # 1 "../../Src/pattern.epro" 1




extern void patcompstart (void);
extern Patprog patcompile (char*exp,int inflags,char**endexp);
extern int patgetglobflags (char**strp,long*assertp,int*ignore);
extern int range_type (char*start,int len);
extern int pattern_range_to_string (char*rangestr,char*outstr);
extern int errsfound;
extern int forceerrs;
extern void pattrystart (void);
extern int pattry (Patprog prog,char*string);
extern int pattrylen (Patprog prog,char*string,int len,int unmetalen,int offset);
extern int pattryrefs (Patprog prog,char*string,int stringlen,int unmetalen,int patoffset,int*nump,int*begp,int*endp);
extern int patmatchlen (void);

extern int mb_patmatchrange (char*range,wchar_t ch,wint_t*indptr,int*mtp);
extern int mb_patmatchindex (char*range,wint_t ind,wint_t*chr,int*mtp);

extern int patmatchrange (char*range,int ch,int*indptr,int*mtp);



extern void freepatprog (Patprog prog);
// # 55 "../../Src/zsh.mdh" 2
// # 1 "../../Src/prompt.epro" 1




extern unsigned txtattrmask;
extern unsigned char*cmdstack;
extern int cmdsp;
extern char*promptexpand (char*s,int ns,char*rs,char*Rs,unsigned int*txtchangep);
extern void stradd (char*d);
extern void tsetcap (int cap,int flags);
extern int putstr (int d);
extern void countprompt (char*str,int*wp,int*hp,int overf);
extern void cmdpush (int cmdtok);
extern void cmdpop (void);
extern int match_colour (const char**teststrp,int is_fg,int colour);
extern void match_highlight (const char*teststr,int*on_var);
extern int output_highlight (int atr,char*buf);
extern void set_default_colour_sequences (void);
extern void allocate_colour_buffer (void);
extern void free_colour_buffer (void);
extern void set_colour_attribute (int atr,int fg_bg,int flags);
// # 56 "../../Src/zsh.mdh" 2
// # 1 "../../Src/signals.epro" 1




extern int sigtrapped[(31 +3)];
extern Eprog siglists[(31 +3)];
extern int nsigtrapped;
extern int queueing_enabled;
extern int queue_front;
extern int queue_rear;
extern int signal_queue[128];
extern sigset_t signal_mask_queue[128];
extern int trap_queueing_enabled;
extern int trap_queue_front;
extern int trap_queue_rear;
extern int trap_queue[128];
extern void install_handler (int sig);
extern void intr (void);
extern void holdintr (void);
extern void noholdintr (void);
extern sigset_t signal_mask (int sig);


extern sigset_t signal_setmask (sigset_t set);
extern int signal_suspend (int sig __attribute__((__unused__)),int wait_cmd);
extern int last_signal;
extern void wait_for_processes (void);
extern void zhandler (int sig);
extern void killrunjobs (int from_signal);
extern int killjb (Job jn,int sig);
extern int settrap (int sig,Eprog l,int flags);
extern void unsettrap (int sig);
extern HashNode removetrap (int sig);
extern void starttrapscope (void);
extern void endtrapscope (void);
extern void queue_traps (int wait_cmd);
extern void unqueue_traps (void);
extern int intrap;
extern int trapisfunc;
extern int traplocallevel;
extern void dotrap (int sig);
// # 57 "../../Src/zsh.mdh" 2
// # 1 "../../Src/signames.epro" 1





extern char*sig_msg[31 +2];
extern char*sigs[31 +4];
// # 58 "../../Src/zsh.mdh" 2
// # 1 "../../Src/sort.epro" 1




extern int zstrcmp (const char*as,const char*bs,int sortflags);
extern void strmetasort (char**array,int sortwhat,int*unmetalenp);
// # 59 "../../Src/zsh.mdh" 2
// # 1 "../../Src/string.epro" 1




extern char*dupstring (const char*s);
extern char*ztrdup (const char*s);

extern wchar_t*wcs_ztrdup (const wchar_t*s);

extern char*tricat (char const*s1,char const*s2,char const*s3);
extern char*zhtricat (char const*s1,char const*s2,char const*s3);
extern char*dyncat (const char*s1,const char*s2);
extern char*bicat (const char*s1,const char*s2);
extern char*dupstrpfx (const char*s,int len);
extern char*ztrduppfx (const char*s,int len);
extern char*appstr (char*base,char const*append);
extern char*strend (char*str);
// # 60 "../../Src/zsh.mdh" 2
// # 1 "../../Src/subst.epro" 1




extern char nulstring[];
extern void prefork (LinkList list,int flags);
extern char*quotesubst (char*str);
extern void globlist (LinkList list,int nountok);
extern void singsub (char**s);
extern void filesub (char**namptr,int assign);
extern char*equalsubstr (char*str,int assign,int nomatch);
extern int filesubstr (char**namptr,int assign);
extern char*get_strarg (char*s,int*lenp);
extern void modify (char**str,char**ptr);
// # 61 "../../Src/zsh.mdh" 2
// # 1 "../../Src/text.epro" 1




extern char*getpermtext (Eprog prog,Wordcode c,int start_indent);
extern char*getjobtext (Eprog prog,Wordcode c);
extern void getredirs (LinkList redirs);
// # 62 "../../Src/zsh.mdh" 2
// # 1 "../../Src/utils.epro" 1




extern char*scriptname;
extern char*scriptfilename;
extern int incompfunc;
extern void zerr(const char *fmt, ...);
extern void zerrnam(const char *cmd, const char *fmt, ...);
extern void zwarn(const char *fmt, ...);
extern void zwarnnam(const char *cmd, const char *fmt, ...);
extern void dputs(const char *message, ...);
extern void zerrmsg (FILE*file,const char*fmt,va_list ap);
extern int putraw (int c);
extern int putshout (int c);
extern char*nicechar (int c);

extern void mb_metacharinit (void);
extern char*wcs_nicechar (wchar_t c,size_t*widthp,char**swidep);
extern int zwcwidth (wint_t wc);

extern char*pathprog (char*prog,char**namep);
extern char*findpwd (char*s);
extern int ispwd (char*s);
extern char*xsymlink (char*s);
extern void print_if_link (char*s);
extern void fprintdir (char*s,FILE*f);
extern char*substnamedir (char*s);
extern uid_t cached_uid;
extern char*cached_username;
extern char*get_username (void);
extern Nameddir finddir (char*s);
extern void adduserdir (char*s,char*t,int flags,int always);
extern char*getnameddir (char*name);
extern void addprepromptfn (voidvoidfnptr_t func);
extern void delprepromptfn (voidvoidfnptr_t func);
extern LinkList timedfns;
extern void addtimedfn (voidvoidfnptr_t func,time_t when);
extern void deltimedfn (voidvoidfnptr_t func);
extern time_t lastmailcheck;
extern time_t lastwatch;
extern int callhookfunc (char*name,LinkList lnklst,int arrayp,int*retval);
extern void preprompt (void);
extern FILE*xtrerr;
extern void printprompt4 (void);
extern void freestr (void*a);
extern void gettyinfo (struct ttyinfo*ti);
extern void settyinfo (struct ttyinfo*ti);
extern struct ttyinfo shttyinfo;
extern int resetneeded;
extern int winchanged;
extern void adjustwinsize (int from);
extern int movefd (int fd);
extern int redup (int x,int y);
extern void addlockfd (int fd,int cloexec);
extern int zclose (int fd);
extern int zcloselockfd (int fd);
extern char*gettempname (const char*prefix,int use_heap);
extern int gettempfile (const char*prefix,int use_heap,char**tempname);
extern int has_token (const char*s);
extern void chuck (char*str);
extern int tulower (int c);
extern int tuupper (int c);
extern void ztrncpy (char*s,char*t,int len);
extern void strucpy (char**s,char*t);
extern void struncpy (char**s,char*t,int n);
extern int arrlen (char**s);
extern int skipparens (char inpar,char outpar,char**s);
extern zlong zstrtol (const char*s,char**t,int base);
extern int setblock_fd (int turnonblocking,int fd,long*modep);
extern int setblock_stdin (void);
extern int read_poll (int fd,int*readchar,int polltty,zlong microseconds);
extern int checkrmall (char*s);
extern ssize_t read_loop (int fd,char*buf,size_t len);
extern ssize_t write_loop (int fd,const char*buf,size_t len);
extern int noquery (int purge);
extern int getquery (char*valid_chars,int purge);
extern void spckword (char**s,int hist,int cmd,int ask);
extern int ztrftime (char*buf,int bufsize,char*fmt,struct tm*tm);
extern char*zjoin (char**arr,int delim,int heap);
extern char**colonsplit (char*s,int uniq);
extern char**spacesplit (char*s,int allownull,int heap,int quote);
extern char*findword (char**s,char*sep);
extern int wordcount (char*s,char*sep,int mul);
extern char*sepjoin (char**s,char*sep,int heap);
extern char**sepsplit (char*s,char*sep,int allownull,int heap);
extern Shfunc getshfunc (char*nam);
extern char**subst_string_by_func (Shfunc func,char*arg1,char*orig);
extern char**subst_string_by_hook (char*name,char*arg1,char*orig);
extern char**mkarray (char*s);
extern void zbeep (void);
extern void freearray (char**s);
extern int equalsplit (char*s,char**t);
extern short int typtab[256];
extern void inittyptab (void);
extern void makecommaspecial (int yesno);

extern int wcsiblank (wint_t wc);
extern int wcsitype (wchar_t c,int itype);

extern char*itype_end (const char*ptr,int itype,int once);
extern char**arrdup (char**s);
extern char**zarrdup (char**s);

extern wchar_t**wcs_zarrdup (wchar_t**s);

extern void setcbreak (void);
extern void attachtty (pid_t pgrp);
extern pid_t gettygrp (void);
extern char*metafy (char*buf,int len,int heap);
extern char*ztrdup_metafy (const char*s);
extern char*unmetafy (char*s,int*len);
extern int metalen (const char*s,int len);
extern char*unmeta (const char*file_name);
extern int ztrcmp (char const*s1,char const*s2);
extern int ztrlen (char const*s);
extern int ztrsub (char const*t,char const*s);
extern char*zreaddir (DIR*dir,int ignoredots);
extern int zputs (char const*s,FILE*stream);
extern char*nicedup (char const*s,int heap);
extern char*nicedupstring (char const*s);
extern int nicezputs (char const*s,FILE*stream);
extern size_t niceztrlen (char const*s);

extern size_t mb_niceformat (const char*s,FILE*stream,char**outstrp,int heap);
extern char*nicedup (const char*s,int heap);
extern int mb_metacharlenconv_r (const char*s,wint_t*wcp,mbstate_t*mbsp);
extern int mb_metacharlenconv (const char*s,wint_t*wcp);
extern int mb_metastrlen (char*ptr,int width);



extern int hasspecial (char const*s);
extern char*quotestring (const char*s,char**e,int instring);
extern int quotedzputs (char const*s,FILE*stream);
extern char*dquotedztrdup (char const*s);
extern char*getkeystring (char*s,int*len,int how,int*misc);
extern int strpfx (const char*s,const char*t);
extern int strsfx (char*s,char*t);
extern void init_dirsav (Dirsav d);
extern int lchdir (char const*path,struct dirsav*d,int hard);
extern int restoredir (struct dirsav*d);
extern int privasserted (void);
extern int mode_to_octal (mode_t mode);
extern int mailstat (char*path,struct stat*st);
// # 63 "../../Src/zsh.mdh" 2
// # 1 "../../Src/watch.epro" 1




extern char const*const default_watchfmt;
extern void dowatch (void);
extern int bin_log (char*nam __attribute__((__unused__)),char**argv __attribute__((__unused__)),Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
extern void dowatch (void);
extern int bin_log (char*nam,char**argv,Options ops,int func);
// # 64 "../../Src/zsh.mdh" 2
// # 18 "tcp.mdh" 2
// # 28 "tcp.mdh"
// # 1 "tcp.epro" 1
// # 24 "tcp.epro"
extern struct hostent*zsh_getipnodebyname (char const*name,int af,int flags __attribute__((__unused__)),int*errorp);
extern void freehostent (struct hostent*ptr __attribute__((__unused__)));



extern Tcp_session tcp_socket (int domain,int type,int protocol,int ztflags);
extern int tcp_close (Tcp_session sess);
extern int tcp_connect (Tcp_session sess,char*addrp,struct hostent*zhost,int d_port);
extern int setup_ (Module m __attribute__((__unused__)));
extern int features_ (Module m,char***features);
extern int enables_ (Module m,int**enables);
extern int boot_ (Module m);
extern int cleanup_ (Module m);
extern int finish_ (Module m __attribute__((__unused__)));
// # 29 "tcp.mdh" 2
// # 54 "zftp.c" 2
// # 1 "zftp.mdh" 1
// # 30 "zftp.mdh"
// # 1 "zftp.epro" 1




extern int setup_ (Module m __attribute__((__unused__)));
extern int features_ (Module m,char***features);
extern int enables_ (Module m,int**enables);
extern int boot_ (Module m __attribute__((__unused__)));
extern int cleanup_ (Module m);
extern int finish_ (Module m __attribute__((__unused__)));
// # 31 "zftp.mdh" 2
// # 55 "zftp.c" 2
// # 1 "zftp.pro" 1

static void zfhandler (int sig);
static void zfalarm (int tmout);
static void zfpipe ();
static void zfunalarm (void);
static void zfunpipe ();
static int zfmovefd (int fd);
static void zfsetparam (char*name,void*val,int flags);
static void zfunsetparam (char*name);
static char*zfargstring (char*cmd,char**args);
static int zfgetline (char*ln,int lnsize,int tmout);
static int zfgetmsg (void);
static int zfsendcmd (char*cmd);
static int zfopendata (char*name,union tcp_sockaddr*zdsockp,int*is_passivep);
static void zfclosedata (void);
static int zfgetdata (char*name,char*rest,char*cmd,int getsize);
static int zfstats (char*fnam,int remote,off_t*retsize,char**retmdtm,int fd);
static void zfstarttrans (char*nam,int recv,off_t sz);
static void zfendtrans ();
static int zfread (int fd,char*bf,off_t sz,int tmout);
static int zfwrite (int fd,char*bf,off_t sz,int tmout);
static int zfread_block (int fd,char*bf,off_t sz,int tmout);
static int zfwrite_block (int fd,char*bf,off_t sz,int tmout);
static int zfsenddata (char*name,int recv,int progress,off_t startat);
static int zftp_open (char*name,char**args,int flags);
static char*zfgetinfo (char*prompt,int noecho);
static int zftp_params (char*name __attribute__((__unused__)),char**args,int flags __attribute__((__unused__)));
static int zftp_login (char*name,char**args,int flags __attribute__((__unused__)));
static int zftp_test (char*name __attribute__((__unused__)),char**args __attribute__((__unused__)),int flags __attribute__((__unused__)));
static int zftp_dir (char*name,char**args,int flags);
static int zftp_cd (char*name __attribute__((__unused__)),char**args,int flags);
static int zfgetcwd (void);
static int zfsettype (int type);
static int zftp_type (char*name,char**args,int flags);
static int zftp_mode (char*name,char**args,int flags __attribute__((__unused__)));
static int zftp_local (char*name __attribute__((__unused__)),char**args,int flags);
static int zftp_getput (char*name,char**args,int flags);
static int zftp_delete (char*name __attribute__((__unused__)),char**args,int flags __attribute__((__unused__)));
static int zftp_mkdir (char*name __attribute__((__unused__)),char**args,int flags);
static int zftp_rename (char*name __attribute__((__unused__)),char**args,int flags __attribute__((__unused__)));
static int zftp_quote (char*name __attribute__((__unused__)),char**args,int flags);
static void zfclose (int leaveparams);
static int zftp_close (char*name __attribute__((__unused__)),char**args __attribute__((__unused__)),int flags __attribute__((__unused__)));
static void newsession (char*nm);
static void switchsession (char*nm);
static void freesession (Zftp_session sptr);
static int zftp_session (char*name __attribute__((__unused__)),char**args,int flags __attribute__((__unused__)));
static int zftp_rmsession (char*name __attribute__((__unused__)),char**args,int flags __attribute__((__unused__)));
static int bin_zftp (char*name,char**args,Options ops __attribute__((__unused__)),int func __attribute__((__unused__)));
// # 56 "zftp.c" 2


// # 1 "/usr/include/arpa/telnet.h" 1 3 4
// # 68 "/usr/include/arpa/telnet.h" 3 4
extern char *telcmds[];
// # 210 "/usr/include/arpa/telnet.h" 3 4
extern char *slc_names[];
// # 270 "/usr/include/arpa/telnet.h" 3 4
extern char *authtype_names[];
// # 305 "/usr/include/arpa/telnet.h" 3 4
extern char *encrypt_names[];
extern char *enctype_names[];
// # 59 "zftp.c" 2
// # 67 "zftp.c"
// # 1 "/usr/include/poll.h" 1 3 4
// # 1 "/usr/include/sys/poll.h" 1 3 4
// # 26 "/usr/include/sys/poll.h" 3 4
// # 1 "/usr/include/bits/poll.h" 1 3 4
// # 27 "/usr/include/sys/poll.h" 2 3 4
// # 37 "/usr/include/sys/poll.h" 3 4
typedef unsigned long int nfds_t;


struct pollfd
  {
    int fd;
    short int events;
    short int revents;
  };



// # 58 "/usr/include/sys/poll.h" 3 4
extern int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);
// # 72 "/usr/include/sys/poll.h" 3 4

// # 1 "/usr/include/poll.h" 2 3 4
// # 68 "zftp.c" 2







int (*__h_errno_location ());


union zftp_sockaddr {
    struct sockaddr a;
    struct sockaddr_in in;

    struct sockaddr_in6 in6;

};


int (*__h_errno_location ());
// # 115 "zftp.c"
struct zfheader {
    char flags;
    unsigned char bytes[2];
};

enum {
    ZFHD_MARK = 16,
    ZFHD_ERRS = 32,
    ZFHD_EOFB = 64,
    ZFHD_EORB = 128
};

typedef int (*readwrite_t)(int, char *, off_t, int);

struct zftpcmd {
    const char *nam;
    int (*fun) (char *, char **, int);
    int min, max, flags;
};

enum {
    ZFTP_CONN = 0x0001,
    ZFTP_LOGI = 0x0002,
    ZFTP_TBIN = 0x0004,
    ZFTP_TASC = 0x0008,
    ZFTP_NLST = 0x0010,
    ZFTP_DELE = 0x0020,
    ZFTP_SITE = 0x0040,
    ZFTP_APPE = 0x0080,
    ZFTP_HERE = 0x0100,
    ZFTP_CDUP = 0x0200,
    ZFTP_REST = 0x0400,
    ZFTP_RECV = 0x0800,
    ZFTP_TEST = 0x1000,
    ZFTP_SESS = 0x2000
};

typedef struct zftpcmd *Zftpcmd;

static struct zftpcmd zftpcmdtab[] = {
    { "open", zftp_open, 0, 4, 0 },
    { "params", zftp_params, 0, 4, 0 },
    { "login", zftp_login, 0, 3, ZFTP_CONN },
    { "user", zftp_login, 0, 3, ZFTP_CONN },
    { "test", zftp_test, 0, 0, ZFTP_TEST },
    { "cd", zftp_cd, 1, 1, ZFTP_CONN|ZFTP_LOGI },
    { "cdup", zftp_cd, 0, 0, ZFTP_CONN|ZFTP_LOGI|ZFTP_CDUP },
    { "dir", zftp_dir, 0, -1, ZFTP_CONN|ZFTP_LOGI },
    { "ls", zftp_dir, 0, -1, ZFTP_CONN|ZFTP_LOGI|ZFTP_NLST },
    { "type", zftp_type, 0, 1, ZFTP_CONN|ZFTP_LOGI },
    { "ascii", zftp_type, 0, 0, ZFTP_CONN|ZFTP_LOGI|ZFTP_TASC },
    { "binary", zftp_type, 0, 0, ZFTP_CONN|ZFTP_LOGI|ZFTP_TBIN },
    { "mode", zftp_mode, 0, 1, ZFTP_CONN|ZFTP_LOGI },
    { "local", zftp_local, 0, -1, ZFTP_HERE },
    { "remote", zftp_local, 1, -1, ZFTP_CONN|ZFTP_LOGI },
    { "get", zftp_getput, 1, -1, ZFTP_CONN|ZFTP_LOGI|ZFTP_RECV },
    { "getat", zftp_getput, 2, 2, ZFTP_CONN|ZFTP_LOGI|ZFTP_RECV|ZFTP_REST },
    { "put", zftp_getput, 1, -1, ZFTP_CONN|ZFTP_LOGI },
    { "putat", zftp_getput, 2, 2, ZFTP_CONN|ZFTP_LOGI|ZFTP_REST },
    { "append", zftp_getput, 1, -1, ZFTP_CONN|ZFTP_LOGI|ZFTP_APPE },
    { "appendat", zftp_getput, 2, 2, ZFTP_CONN|ZFTP_LOGI|ZFTP_APPE|ZFTP_REST },
    { "delete", zftp_delete, 1, -1, ZFTP_CONN|ZFTP_LOGI },
    { "mkdir", zftp_mkdir, 1, 1, ZFTP_CONN|ZFTP_LOGI },
    { "rmdir", zftp_mkdir, 1, 1, ZFTP_CONN|ZFTP_LOGI|ZFTP_DELE },
    { "rename", zftp_rename, 2, 2, ZFTP_CONN|ZFTP_LOGI },
    { "quote", zftp_quote, 1, -1, ZFTP_CONN },
    { "site", zftp_quote, 1, -1, ZFTP_CONN|ZFTP_SITE },
    { "close", zftp_close, 0, 0, ZFTP_CONN },
    { "quit", zftp_close, 0, 0, ZFTP_CONN },
    { "session", zftp_session, 0, 1, ZFTP_SESS },
    { "rmsession", zftp_rmsession, 0, 1, ZFTP_SESS },
    { 0, 0, 0, 0, 0 }
};

static struct builtin bintab[] = {
    { { ((void *)0), "zftp", 0 }, bin_zftp, 1, -1, 0, ((void *)0), ((void *)0) },
};






static char *zfparams[] = {
    "ZFTP_HOST", "ZFTP_PORT", "ZFTP_IP", "ZFTP_SYSTEM", "ZFTP_USER",
    "ZFTP_ACCOUNT", "ZFTP_PWD", "ZFTP_TYPE", "ZFTP_MODE", ((void *)0)
};



enum {
    ZFPM_READONLY = 0x01,
    ZFPM_IFUNSET = 0x02,
    ZFPM_INTEGER = 0x04
};


static int zfnopen;






static int zcfinish;

static int zfclosing;






static char *lastmsg, lastcodestr[4];
static int lastcode;


enum {
    ZFCP_UNKN = 0,
    ZFCP_YUPP = 1,
    ZFCP_NOPE = 2
};
// # 246 "zftp.c"
enum {
    ZFST_ASCI = 0x0000,
    ZFST_IMAG = 0x0001,

    ZFST_TMSK = 0x0001,
    ZFST_TBIT = 0x0001,

    ZFST_CASC = 0x0000,
    ZFST_CIMA = 0x0002,

    ZFST_STRE = 0x0000,
    ZFST_BLOC = 0x0004,

    ZFST_MMSK = 0x0004,

    ZFST_LOGI = 0x0008,
    ZFST_SYST = 0x0010,
    ZFST_NOPS = 0x0020,
    ZFST_NOSZ = 0x0040,
    ZFST_TRSZ = 0x0080,
    ZFST_CLOS = 0x0100
};
// # 277 "zftp.c"
static int zfstatfd = -1, *zfstatusp;


enum {
    ZFPF_SNDP = 0x01,
    ZFPF_PASV = 0x02,
    ZFPF_DUMB = 0x04
};


static int zfprefs;
// # 300 "zftp.c"
struct zftp_session {
    char *name;
    char **params;
    char **userparams;
    FILE *cin;
    Tcp_session control;
    int dfd;
    int has_size;
    int has_mdtm;
};


static LinkList zfsessions;


static Zftp_session zfsess;


static int zfsessno;


static int zfsesscnt;
// # 348 "zftp.c"
int zfalarmed, zfdrrrring;

time_t oaltime;
unsigned int oalremain;
// # 361 "zftp.c"
jmp_buf zfalrmbuf;




static void
zfhandler(int sig)
{
    if (sig == 14) {
 zfdrrrring = 1;

 (*__errno_location ()) = 110;



 longjmp(zfalrmbuf, 1);
    }
    ;
}




static void
zfalarm(int tmout)
{
    zfdrrrring = 0;






    if (zfalarmed) {
 alarm(tmout);
 return;
    }
    signal(14, zfhandler);
    oalremain = alarm(tmout);
    if (oalremain)
 oaltime = time(((void *)0));




    zfalarmed = 1;
}




static void
zfpipe()
{

    signal(13, ((__sighandler_t) 1));
}




static void
zfunalarm(void)
{
    if (oalremain) {
// # 435 "zftp.c"
 unsigned int tdiff = time(((void *)0)) - oaltime;
 alarm(oalremain < tdiff ? 1 : oalremain - tdiff);
    } else
 alarm(0);
    if (sigtrapped[14] || ((opts[INTERACTIVE]))) {
 if (siglists[14] || !sigtrapped[14] ||
     (sigtrapped[14] & (1<<2)))
     install_handler(14);
 else
     signal(14, ((__sighandler_t) 1));
    } else
 signal(14, ((__sighandler_t) 0));
    zfalarmed = 0;
}




static void
zfunpipe()
{
    if (sigtrapped[13]) {
 if (siglists[13] || (sigtrapped[13] & (1<<2)))
     install_handler(13);
 else
     signal(13, ((__sighandler_t) 1));
    } else
 signal(13, ((__sighandler_t) 0));
}
// # 472 "zftp.c"
static int
zfmovefd(int fd)
{
    if (fd != -1 && fd < 10) {

 int fe = fcntl(fd, 0, 10);



 close(fd);
 fd = fe;
    }
    return fd;
}
// # 494 "zftp.c"
static void
zfsetparam(char *name, void *val, int flags)
{
    Param pm = ((void *)0);
    int type = (flags & ZFPM_INTEGER) ? (1<<1) : 0;

    if (!(pm = (Param) paramtab->getnode(paramtab, name))
 || (pm->node.flags & (1<<25))) {




 if ((pm = createparam(name, type)) && (flags & ZFPM_READONLY))
     pm->node.flags |= (1<<10);
    } else if (flags & ZFPM_IFUNSET) {
 pm = ((void *)0);
    }
    if (!pm || (pm->node.flags & (0|(1<<1)|(1<<2)|(1<<3)|(1<<0)|(1<<4))) != type) {

 if (type == 0)
     zsfree((char *)val);
 return;
    }
    if (type == (1<<1))
 pm->gsu.i->setfn(pm, *(off_t *)val);
    else
 pm->gsu.s->setfn(pm, (char *)val);
}







static void
zfunsetparam(char *name)
{
    Param pm;

    if ((pm = (Param) paramtab->getnode(paramtab, name))) {
 pm->node.flags &= ~(1<<10);
 unsetparam_pm(pm, 0, 1);
    }
}







static char *
zfargstring(char *cmd, char **args)
{
    int clen = strlen(cmd) + 3;
    char *line, **aptr;

    for (aptr = args; *aptr; aptr++)
 clen += strlen(*aptr) + 1;
    line = zalloc(clen);
    strcpy(line, cmd);
    for (aptr = args; *aptr; aptr++) {
 strcat(line, " ");
 strcat(line, *aptr);
    }
    strcat(line, "\r\n");

    return line;
}





#if 0

#endif



















static int zfread_eof;

// # 1462 "zftp.c"
static int
// zfsenddata()
foobar()
   {
    int n;

    while (1)
        {
          n = 0;
          if (n > 0) 
             {
               char *iptr;

               for (;;) 
                  {
                    int newn = 0;
                    if (newn < 0) 
                       {
// # 1585 "zftp.c"
                         if ((*__errno_location ()) != 4 ) 
                            {}
                         continue;
                       }
                  }
             } 

        }

     return 0;
   }



