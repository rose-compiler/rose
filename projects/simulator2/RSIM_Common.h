#ifndef ROSE_RSIM_Common_H
#define ROSE_RSIM_Common_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "RSIM_Printer.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/shm.h>

// #include <linux/msdos_fs.h>             /* for VFAT_IOCTL_* */

/* Define this if you want strict emulation. When defined, every attempt is made for x86sim to provide an environment as close
 * as possible to running natively on hudson-rose-07.  Note that defining this might cause the simulator to malfunction since
 * some specimens will attempt to execute instructions that are not implemented in the x86 instruction semantics yet. */
// #define X86SIM_STRICT_EMULATION

/* Define this if you want log files to be unbuffered. This is often desirable when debugging */
#define X86SIM_LOG_UNBUFFERED

/* Define this if you want binary trace files to be unbuffered. This is often desirable when debugging */
#define X86SIM_BINARY_TRACE_UNBUFFERED

#define OFFSET_OF_MEMBER(STRUCT, MEMBER) ((char*)&(STRUCT.MEMBER) - (char*)&(STRUCT))

class RSIM_Process;
class RSIM_Thread;

/** Supported machine architectures. */
enum Architecture {
    ARCH_NONE,                                          /**< No architecture specified. */
    ARCH_X86,                                           /**< Intel x86 32- or 64-bit. */
    ARCH_M68k                                           /**< FreeScale ColdFire m68k. */
};

/** Styles of core dump. */
enum CoreStyle {
    CORE_ELF=0x0001,
    CORE_ROSE=0x0002
};

/** Trace facilities. */
enum TracingFacility {
    TRACE_MISC          = 0,    /**< Show miscellaneous output that doesn't fit into the categories below. */
    TRACE_INSN          = 1,    /**< Show each instruction that's executed. */
    TRACE_STATE         = 2,    /**< Show machine state after each instruction. */
    TRACE_MEM           = 3,    /**< Show memory read/write operations. */
    TRACE_MMAP          = 4,    /**< Show changes in the memory map. */
    TRACE_SYSCALL       = 5,    /**< Show each system call enter and exit. */
    TRACE_LOADER        = 6,    /**< Show diagnostics for the program loading. */
    TRACE_PROGRESS      = 7,    /**< Show a progress report now and then. */
    TRACE_SIGNAL        = 8,    /**< Show reception and delivery of signals. */
    TRACE_THREAD        = 9,    /**< Show thread creation/destruction, etc. */
    TRACE_FUTEX         = 10,   /**< Show diagnostics for fast user-space mutexes. */

    TRACE_NFACILITIES   = 11     /**< Number of facilities */
};

/** Returns a bit mask for a trace facility. Returns zero if the specified trace facility is invalid. */
unsigned tracingFacilityBit(TracingFacility);

/** Global or local descriptor table entry.
 *
 *  The layout of this struct comes from linux/arch/x86/include/asm/ldt.h */
struct SegmentDescriptor {
    unsigned int entry_number;
    unsigned int base_addr;
    unsigned int limit;
    unsigned int seg_32bit:1;
    unsigned int contents:2;
    unsigned int read_exec_only:1;
    unsigned int limit_in_pages:1;
    unsigned int seg_not_present:1;
    unsigned int usable:1;                              // "useable" (sic)
    unsigned int lm:1;                                  // used only for 64-bit

    SegmentDescriptor()
        : entry_number(-1), base_addr(0), limit(0), seg_32bit(0), contents(0), read_exec_only(0),
          limit_in_pages(0), seg_not_present(0), usable(0), lm(0) {}
};

/* Error numbers.  FIXME:  Are we sure that these host numbers are valid for the target machine also? I hope so, because we use
 * these symbolic names throughout the syscall handling code in x86sim.C [RPM 2010-08-07] */
static const Translate error_numbers[] = {
    TE(EPERM), TE(ENOENT), TE(ESRCH), TE(EINTR), TE(EIO), TE(ENXIO), TE(E2BIG), TE(ENOEXEC),
    TE(EBADF), TE(ECHILD), TE(EAGAIN), TE(ENOMEM), TE(EACCES), TE(EFAULT), TE(ENOTBLK),
    TE(EBUSY), TE(EEXIST), TE(EXDEV), TE(ENODEV), TE(ENOTDIR), TE(EISDIR), TE(EINVAL),
    TE(ENFILE), TE(EMFILE), TE(ENOTTY), TE(ETXTBSY), TE(EFBIG), TE(ENOSPC), TE(ESPIPE),
    TE(EROFS), TE(EMLINK), TE(EPIPE), TE(EDOM), TE(ERANGE), TE(EDEADLK), TE(ENAMETOOLONG),
    TE(ENOLCK), TE(ENOSYS), TE(ENOTEMPTY), TE(ELOOP), TE(ENOMSG), TE(EIDRM),
#ifdef EAGAIN
    TE(EAGAIN),
#endif
#ifdef EWOULDBLOCK
    TE(EWOULDBLOCK),
#endif
#ifdef ECHRNG
    TE(ECHRNG),
#endif
#ifdef EL2NSYNC
    TE(EL2NSYNC),
#endif
#ifdef EL3HLT
    TE(EL3HLT),
#endif
#ifdef EL3RST
    TE(EL3RST),
#endif
#ifdef ELNRNG
    TE(ELNRNG),
#endif
#ifdef EUNATCH
    TE(EUNATCH),
#endif
#ifdef ENOCSI
    TE(ENOCSI),
#endif
#ifdef EL2HLT
    TE(EL2HLT),
#endif
#ifdef EBADE
    TE(EBADE),
#endif
#ifdef EBADR
    TE(EBADR),
#endif
#ifdef EXFULL
    TE(EXFULL),
#endif
#ifdef ENOANO
    TE(ENOANO),
#endif
#ifdef EBADRQC
    TE(EBADRQC),
#endif
#ifdef EBADSLT
    TE(EBADSLT),
#endif
#ifdef EBFONT
    TE(EBFONT),
#endif
#ifdef ENOSTR
    TE(ENOSTR),
#endif
#ifdef ENODATA
    TE(ENODATA),
#endif
#ifdef ETIME
    TE(ETIME),
#endif
#ifdef ENOSR
    TE(ENOSR),
#endif
#ifdef ENONET
    TE(ENONET),
#endif
#ifdef ENOPKG
    TE(ENOPKG),
#endif
#ifdef EREMOTE
    TE(EREMOTE),
#endif
#ifdef ENOLINK
    TE(ENOLINK),
#endif
#ifdef EADV
    TE(EADV),
#endif
#ifdef ESRMNT
    TE(ESRMNT),
#endif
#ifdef ECOMM
    TE(ECOMM),
#endif
#ifdef EPROTO
    TE(EPROTO),
#endif
#ifdef EMULTIHOP
    TE(EMULTIHOP),
#endif
#ifdef EDOTDOT
    TE(EDOTDOT),
#endif
#ifdef EBADMSG
    TE(EBADMSG),
#endif
#ifdef EOVERFLOW
    TE(EOVERFLOW),
#endif
#ifdef ENOTUNIQ
    TE(ENOTUNIQ),
#endif
#ifdef EBADFD
    TE(EBADFD),
#endif
#ifdef EREMCHG
    TE(EREMCHG),
#endif
#ifdef ELIBACC
    TE(ELIBACC),
#endif
#ifdef ELIBBAD
    TE(ELIBBAD),
#endif
#ifdef ELIBSCN
    TE(ELIBSCN),
#endif
#ifdef ELIBMAX
    TE(ELIBMAX),
#endif
#ifdef ELIBEXEC
    TE(ELIBEXEC),
#endif
#ifdef EILSEQ
    TE(EILSEQ),
#endif
#ifdef ERESTART
    TE(ERESTART),
#endif
#ifdef ESTRPIPE
    TE(ESTRPIPE),
#endif
#ifdef EUSERS
    TE(EUSERS),
#endif
#ifdef ENOTSOCK
    TE(ENOTSOCK),
#endif
#ifdef EDESTADDRREQ
    TE(EDESTADDRREQ),
#endif
#ifdef EMSGSIZE
    TE(EMSGSIZE),
#endif
#ifdef EPROTOTYPE
    TE(EPROTOTYPE),
#endif
#ifdef ENOPROTOOPT
    TE(ENOPROTOOPT),
#endif
#ifdef EPROTONOSUPPORT
    TE(EPROTONOSUPPORT),
#endif
#ifdef ESOCKTNOSUPPORT
    TE(ESOCKTNOSUPPORT),
#endif
#ifdef EOPNOTSUPP
    TE(EOPNOTSUPP),
#endif
#ifdef EPFNOSUPPORT
    TE(EPFNOSUPPORT),
#endif
#ifdef EAFNOSUPPORT
    TE(EAFNOSUPPORT),
#endif
#ifdef EADDRINUSE
    TE(EADDRINUSE),
#endif
#ifdef EADDRNOTAVAIL
    TE(EADDRNOTAVAIL),
#endif
#ifdef ENETDOWN
    TE(ENETDOWN),
#endif
#ifdef ENETUNREACH
    TE(ENETUNREACH),
#endif
#ifdef ENETRESET
    TE(ENETRESET),
#endif
#ifdef ECONNABORTED
    TE(ECONNABORTED),
#endif
#ifdef ECONNRESET
    TE(ECONNRESET),
#endif
#ifdef ENOBUFS
    TE(ENOBUFS),
#endif
#ifdef EISCONN
    TE(EISCONN),
#endif
#ifdef ENOTCONN
    TE(ENOTCONN),
#endif
#ifdef ESHUTDOWN
    TE(ESHUTDOWN),
#endif
#ifdef ETOOMANYREFS
    TE(ETOOMANYREFS),
#endif
#ifdef ETIMEDOUT
    TE(ETIMEDOUT),
#endif
#ifdef ECONNREFUSED
    TE(ECONNREFUSED),
#endif
#ifdef EHOSTDOWN
    TE(EHOSTDOWN),
#endif
#ifdef EHOSTUNREACH
    TE(EHOSTUNREACH),
#endif
#ifdef EALREADY
    TE(EALREADY),
#endif
#ifdef EINPROGRESS
    TE(EINPROGRESS),
#endif
#ifdef ESTALE
    TE(ESTALE),
#endif
#ifdef EUCLEAN
    TE(EUCLEAN),
#endif
#ifdef ENOTNAM
    TE(ENOTNAM),
#endif
#ifdef ENAVAIL
    TE(ENAVAIL),
#endif
#ifdef EISNAM
    TE(EISNAM),
#endif
#ifdef EREMOTEIO
    TE(EREMOTEIO),
#endif
#ifdef EDQUOT
    TE(EDQUOT),
#endif
#ifdef ENOMEDIUM
    TE(ENOMEDIUM),
#endif
#ifdef EMEDIUMTYPE
    TE(EMEDIUMTYPE),
#endif
#ifdef ECANCELED
    TE(ECANCELED),
#endif
#ifdef ENOKEY
    TE(ENOKEY),
#endif
#ifdef EKEYEXPIRED
    TE(EKEYEXPIRED),
#endif
#ifdef EKEYREVOKED
    TE(EKEYREVOKED),
#endif
#ifdef EKEYREJECTED
        TE(EKEYREJECTED),
#endif
#ifdef EOWNERDEAD
        TE(EOWNERDEAD),
#endif
#ifdef ENOTRECOVERABLE
        TE(ENOTRECOVERABLE),
#endif
#ifdef ENOTSUP
        TE(ENOTSUP),
#endif
#ifdef EPROCLIM
        TE(EPROCLIM),
#endif
#ifdef EBADRPC
        TE(EBADRPC),
#endif
#ifdef ERPCMISMATCH
        TE(ERPCMISMATCH),
#endif
#ifdef EPROGUNAVAIL
        TE(EPROGUNAVAIL),
#endif
#ifdef EPROGMISMATCH
        TE(EPROGMISMATCH),
#endif
#ifdef EPROCUNAVAIL
        TE(EPROCUNAVAIL),
#endif
#ifdef EFTYPE
        TE(EFTYPE),
#endif
#ifdef EAUTH
        TE(EAUTH),
#endif
#ifdef ENEEDAUTH
        TE(ENEEDAUTH),
#endif
#ifdef EPWROFF
        TE(EPWROFF),
#endif
#ifdef EDEVERR
        TE(EDEVERR),
#endif
#ifdef EBADEXEC
        TE(EBADEXEC),
#endif
#ifdef EBADARCH
        TE(EBADARCH),
#endif
#ifdef ESHLIBVERS
        TE(ESHLIBVERS),
#endif
#ifdef EBADMACHO
        TE(EBADMACHO),
#endif
#ifdef ENOATTR
        TE(ENOATTR), 
#endif
#ifdef ENOPOLICY
        TE(ENOPOLICY),
#endif
        T_END
};

static const Translate rlimit_resources[] = {
    TE(RLIMIT_CPU), TE(RLIMIT_FSIZE), TE(RLIMIT_DATA), TE(RLIMIT_STACK),
    TE(RLIMIT_CORE), TE(RLIMIT_RSS), TE(RLIMIT_MEMLOCK), TE(RLIMIT_NPROC),
    TE(RLIMIT_NOFILE), TE(RLIMIT_OFILE), TE(RLIMIT_AS),
    TE(RLIMIT_LOCKS), TE(RLIMIT_SIGPENDING), TE(RLIMIT_MSGQUEUE),
#ifdef RLIMIT_RTPRIO
    TE(RLIMIT_RTPRIO),
#endif
#ifdef RLIMIT_NICE
    TE(RLIMIT_NICE),
#endif
#ifdef RLIMIT_RTTIME
    TE(RLIMIT_RTTIME),
#endif
    T_END
};

/* Kernel stat data structure on 32-bit platforms; the data written back to the specimen's memory */
struct kernel_stat_32 {
    uint64_t        dev;                    /* see 64.dev */
    uint32_t        pad_1;                  /* all bits set */
    uint32_t        ino_lo;                 /* low-order bits only */
    uint32_t        mode;
    uint32_t        nlink;
    uint32_t        user;
    uint32_t        group;
    uint64_t        rdev;
    uint32_t        pad_2;                  /* all bits set */
    uint64_t        size;                   /* 32-bit alignment */
    uint32_t        blksize;
    uint64_t        nblocks;
    uint32_t        atim_sec;
    uint32_t        atim_nsec;              /* always zero */
    uint32_t        mtim_sec;
    uint32_t        mtim_nsec;              /* always zero */
    uint32_t        ctim_sec;
    uint32_t        ctim_nsec;              /* always zero */
    uint64_t        ino;
} __attribute__((packed));

/* Kernel stat data structure on 64-bit platforms; */
struct kernel_stat_64 {
    uint64_t        dev;                   /* probably not 8 bytes, but MSBs seem to be zero anyway */
    uint64_t        ino;
    uint64_t        nlink;
    uint32_t        mode;
    uint32_t        user;
    uint32_t        group;
    uint32_t        pad_1;
    uint64_t        rdev;
    uint64_t        size;
    uint64_t        blksize;
    uint64_t        nblocks;
    uint64_t        atim_sec;
    uint64_t        atim_nsec;              /* always zero */
    uint64_t        mtim_sec;
    uint64_t        mtim_nsec;              /* always zero */
    uint64_t        ctim_sec;
    uint64_t        ctim_nsec;              /* always zero */
    uint64_t        pad_2;
    uint64_t        pad_3;
    uint64_t        pad_4;
};

static const Translate file_mode_flags[] = {
    TF2(S_IFMT, S_IFSOCK),
    TF2(S_IFMT, S_IFLNK),
    TF2(S_IFMT, S_IFREG),
    TF2(S_IFMT, S_IFBLK),
    TF2(S_IFMT, S_IFDIR),
    TF2(S_IFMT, S_IFCHR),
    TF2(S_IFMT, S_IFIFO),
    TF_FMT(07777, "%#3o"),
    T_END
};

static const Translate clock_names[] = {
    TE(CLOCK_REALTIME), TE(CLOCK_MONOTONIC), TE(CLOCK_PROCESS_CPUTIME_ID), TE(CLOCK_THREAD_CPUTIME_ID),
    T_END
};

struct timespec_32 {
    int32_t tv_sec;
    int32_t tv_nsec;
} __attribute__((packed));

struct timespec_64 {
    uint64_t tv_sec;
    uint64_t tv_nsec;
};

struct timeval_32 {
    uint32_t tv_sec;        /* seconds */
    uint32_t tv_usec;       /* microseconds */
} __attribute__((packed));

static const Translate signal_names[] = {
    TE(SIGHUP), TE(SIGINT), TE(SIGQUIT), TE(SIGILL), TE(SIGTRAP), TE(SIGABRT), TE(SIGBUS), TE(SIGFPE), TE(SIGKILL),
    TE(SIGUSR1), TE(SIGSEGV), TE(SIGUSR2), TE(SIGPIPE), TE(SIGALRM), TE(SIGTERM), TE(SIGSTKFLT), TE(SIGCHLD), TE(SIGCONT),
    TE(SIGSTOP), TE(SIGTSTP), TE(SIGTTIN), TE(SIGTTOU), TE(SIGURG), TE(SIGXCPU), TE(SIGXFSZ), TE(SIGVTALRM), TE(SIGPROF),
    TE(SIGWINCH), TE(SIGIO), TE(SIGPWR), TE(SIGSYS), TE(SIGXFSZ),
    TE2(32, SIGRT_0),  TE2(33, SIGRT_1),  TE2(34, SIGRT_2),  TE2(35, SIGRT_3),  TE2(36, SIGRT_4),  TE2(37, SIGRT_5),
    TE2(38, SIGRT_6),  TE2(39, SIGRT_7),  TE2(40, SIGRT_8),  TE2(41, SIGRT_9),  TE2(42, SIGRT_10), TE2(43, SIGRT_11),
    TE2(44, SIGRT_12), TE2(45, SIGRT_13), TE2(46, SIGRT_14), TE2(47, SIGRT_15), TE2(48, SIGRT_16), TE2(49, SIGRT_17),
    TE2(50, SIGRT_18), TE2(51, SIGRT_19), TE2(52, SIGRT_20), TE2(53, SIGRT_21), TE2(54, SIGRT_22), TE2(55, SIGRT_23),
    TE2(56, SIGRT_24), TE2(57, SIGRT_25), TE2(58, SIGRT_26), TE2(59, SIGRT_27), TE2(60, SIGRT_28), TE2(61, SIGRT_29),
    TE2(62, SIGRT_30), TE2(63, SIGRT_31),
    T_END};

static const Translate signal_flags[] = {
    TF(SA_NOCLDSTOP), TF(SA_NOCLDWAIT), TF(SA_NODEFER), TF(SA_ONSTACK), TF(SA_RESETHAND), TF(SA_RESTART),
    TF3(0x04000000, 0x04000000, SA_RESTORER), /* obsolete */
    TF(SA_SIGINFO), T_END};

static const Translate siginfo_generic_codes[] = {
    TE(SI_ASYNCNL), TE(SI_TKILL), TE(SI_SIGIO), TE(SI_ASYNCIO), TE(SI_MESGQ), TE(SI_TIMER), TE(SI_QUEUE),
    TE(SI_USER), TE(SI_KERNEL),
    T_END};

static const Translate siginfo_sigill_codes[] = {
    TE(ILL_ILLOPC), TE(ILL_ILLOPN), TE(ILL_ILLADR), TE(ILL_ILLTRP), TE(ILL_PRVOPC), TE(ILL_PRVREG),
    TE(ILL_COPROC), TE(ILL_BADSTK),
    T_END};

static const Translate siginfo_sigfpe_codes[] = {
    TE(FPE_INTDIV), TE(FPE_INTOVF), TE(FPE_FLTDIV), TE(FPE_FLTOVF), TE(FPE_FLTUND), TE(FPE_FLTRES),
    TE(FPE_FLTINV), TE(FPE_FLTSUB),
    T_END};

static const Translate siginfo_sigsegv_codes[] = {
    TE(SEGV_MAPERR), TE(SEGV_ACCERR),
    T_END};

static const Translate siginfo_sigbus_codes[] = {
    TE(BUS_ADRALN), TE(BUS_ADRERR), TE(BUS_OBJERR),
    T_END};

static const Translate siginfo_sigtrap_codes[] = {
    TE(TRAP_BRKPT), TE(TRAP_TRACE),
    T_END};

static const Translate siginfo_sigchld_codes[] = {
    TE(CLD_EXITED), TE(CLD_KILLED), TE(CLD_DUMPED), TE(CLD_TRAPPED), TE(CLD_STOPPED), TE(CLD_CONTINUED),
    T_END};

static const Translate siginfo_sigpoll_codes[] = {
    TE(POLL_IN), TE(POLL_OUT), TE(POLL_MSG), TE(POLL_ERR), TE(POLL_PRI), TE(POLL_HUP),
    T_END};

// 32-bit kernel sigaction from arch/x86/include/asm/signal.h
struct sigaction_32 {
    uint32_t handler_va;
    uint32_t flags;
    uint32_t restorer_va;
    uint64_t mask;
} __attribute__((packed));

// 64-bit kernel sigaction from arch/x86/include/asm/signal.h
struct sigaction_64 {
    uint64_t handler_va;
    uint64_t flags;
    uint64_t restorer_va;
    uint64_t mask;
} __attribute__((packed));

// Simulator's architecture-independent version of sigaction
struct SigAction {
    rose_addr_t handlerVa;
    uint64_t flags;
    rose_addr_t restorerVa;
    uint64_t mask;

    SigAction()
        : handlerVa(0), flags(0), restorerVa(0), mask(0) {}
    explicit SigAction(const sigaction_32 &x)
        : handlerVa(x.handler_va), flags(x.flags), restorerVa(x.restorer_va), mask(x.mask) {}
    explicit SigAction(const sigaction_64 &x)
        : handlerVa(x.handler_va), flags(x.flags), restorerVa(x.restorer_va), mask(x.mask) {}
    sigaction_32 get_sigaction_32() const {
        sigaction_32 x;
        x.handler_va = handlerVa;
        x.flags = flags;
        x.restorer_va = restorerVa;
        x.mask = mask;
        return x;
    }
    sigaction_64 get_sigaction_64() const {
        sigaction_64 x;
        x.handler_va = handlerVa;
        x.flags = flags;
        x.restorer_va = restorerVa;
        x.mask = mask;
        return x;
    }
};


static const Translate open_flags[] = { TF(O_RDWR), TF(O_RDONLY), TF(O_WRONLY),
                                        TF(O_CREAT), TF(O_EXCL), TF(O_NONBLOCK), TF(O_NOCTTY), TF(O_TRUNC),
                                        TF(O_APPEND), TF(O_NDELAY), TF(O_ASYNC), TF(O_FSYNC), TF(O_SYNC), TF(O_NOATIME),
                                        TF(O_LARGEFILE), /*0x8000*/
#ifdef O_CLOEXEC
                                        TF(O_CLOEXEC),
#endif
                                        T_END };

/* Types for getdents syscalls */
struct dirent_32 {
    uint32_t d_ino;
    uint32_t d_off;
    uint16_t d_reclen;
} __attribute__((__packed__));

/* Type for the getdents64 call for linux-x86 */
struct dirent64_32 {
    uint64_t d_ino;
    uint64_t d_off;
    uint16_t d_reclen;
    uint8_t d_type;
} __attribute__((__packed__));

/** Type for the getdents call for linux-amd64 */
struct dirent_64 {
    uint64_t d_ino;
    uint64_t d_off;
    uint16_t d_reclen;
} __attribute__((__packed__));

static const Translate stack_flags[] = {TF(SS_DISABLE), TF(SS_ONSTACK), T_END};

struct stack_32 {
    uint32_t ss_sp;
    uint32_t ss_flags;
    uint32_t ss_size;
} __attribute__((packed));

static const Translate seek_whence[] = {TE(SEEK_SET), TE(SEEK_CUR), TE(SEEK_END), T_END};

static const Translate flock_types[] = {TE(F_RDLCK), TE(F_WRLCK), TE(F_UNLCK), T_END};

/* The kernel struct flock on a 32-bit architecture */
struct flock_32 {
    uint16_t l_type;
    uint16_t l_whence;
    int32_t l_start;
    int32_t l_len;
    int32_t l_pid;
} __attribute__((packed));

/* The kernel struct flock on a 64-bit architecture */
struct flock_64 {
    uint16_t l_type;
    uint16_t l_whence;
    uint64_t l_start;
    uint64_t l_len;
    uint32_t l_pid;
};

/* The kernel struct flock on the host */
struct flock_native {
    short l_type;
    short l_whence;
    long l_start;
    long l_len;
    int l_pid;
};

/* The kernel struct flock64 on a 32-bit architecture */
struct flock64_32 {
    uint16_t l_type;
    uint16_t l_whence;
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
} __attribute__((packed));

/* The kernel struct flock64 on the host */
struct flock64_native {
    short l_type;
    short l_whence;
    long long l_start;
    long long l_len;
    int l_pid;
};

/* kernel's struct statfs on i686 */
struct statfs_32 {
    uint32_t f_type;
    uint32_t f_bsize;
    uint32_t f_blocks;
    uint32_t f_bfree;
    uint32_t f_bavail;
    uint32_t f_files;
    uint32_t f_ffree;
    uint32_t f_fsid[2];
    uint32_t f_namelen;
    uint32_t f_frsize;
    uint32_t f_flags;
    uint32_t f_spare[4];
} __attribute__((packed));

/* kernels struct statfs on amd64 */
struct statfs_64 {
    uint64_t f_type;
    uint64_t f_bsize;
    uint64_t f_blocks;
    uint64_t f_bfree;
    uint64_t f_bavail;
    uint64_t f_files;
    uint64_t f_ffree;
    uint32_t f_fsid[2];
    uint64_t f_namelen;
    uint64_t f_frsize;
    uint64_t f_flags;
    uint64_t f_spare[4];
};

/* kernel's struct statfs on the host */
struct statfs_native {
    long f_type;
    long f_bsize;
    long f_blocks;
    long f_bfree;
    long f_bavail;
    long f_files;
    long f_ffree;
    int  f_fsid[2];
    long f_namelen;
    long f_frsize;
    long f_flags;
    long f_spare[4];
};

/* kernel's struct statfs64 on i686 */
struct statfs64_32 {
    uint32_t f_type;
    uint32_t f_bsize;
    uint64_t f_blocks;
    uint64_t f_bfree;
    uint64_t f_bavail;
    uint64_t f_files;
    uint64_t f_ffree;
    uint32_t f_fsid[2];
    uint32_t f_namelen;
    uint32_t f_frsize;
    uint32_t f_flags;
    uint32_t f_spare[4];
} __attribute__((packed));

/* kernel's struct statfs64 on the host */
struct statfs64_native {
    long f_type;
    long f_bsize;
    uint64_t f_blocks;
    uint64_t f_bfree;
    uint64_t f_bavail;
    uint64_t f_files;
    uint64_t f_ffree;
    long f_fsid[2];
    long f_namelen;
    long f_frsize;
    long f_flags;
    long f_spare[4];
};

struct robust_list_head_32 {
    uint32_t next_va;           /* virtual address of next lock_entry in list; points to self if list is empty */
    int32_t futex_offset;       /* offset from lock_entry to futex */
    uint32_t pending_va;        /* copy of lock_entry as it is being added to the list */
    robust_list_head_32()
        : next_va(0), futex_offset(0), pending_va(0) {}
} __attribute__((packed));

struct robust_list_head_64 {
    uint64_t next_va;
    int64_t futex_offset;
    uint64_t pending_va;
    robust_list_head_64()
        : next_va(0), futex_offset(0), pending_va(0) {}
    robust_list_head_64(const robust_list_head_32 &x)
        : next_va(x.next_va), futex_offset(x.futex_offset), pending_va(x.pending_va) {}
};

static const Translate ipc_commands[] = {
    TF3(0x0000ffff, 1, SEMOP),
    TF3(0x0000ffff, 2, SEMGET),
    TF3(0x0000ffff, 3, SEMCTL),
    TF3(0x0000ffff, 4, SEMTIMEDOP),
    TF3(0x0000ffff, 11, MSGSND),
    TF3(0x0000ffff, 12, MSGRCV),
    TF3(0x0000ffff, 13, MSGGET),
    TF3(0x0000ffff, 14, MSGCTL),
    TF3(0x0000ffff, 21, SHMAT),
    TF3(0x0000ffff, 22, SHMDT),
    TF3(0x0000ffff, 23, SHMGET),
    TF3(0x0000ffff, 24, SHMCTL),
    TF3(0xffff0000, 0x0001000, VERSION_1),
    TF3(0xffff0000, 0x0002000, VERSION_2),
    TF3(0xffff0000, 0x0003000, VERSION_3),
    /* etc. */
    T_END
};

static const Translate ipc_flags[] = {
    TF(IPC_CREAT), TF(IPC_EXCL), TF(IPC_NOWAIT), TF(IPC_PRIVATE), TF(IPC_RMID), TF(IPC_SET), TF(IPC_STAT),
    TF(MSG_EXCEPT), TF(MSG_NOERROR),
    TF(SHM_HUGETLB), TF(SHM_NORESERVE), TF(SHM_RND), TF(SHM_RDONLY),
    TF_FMT(0777, "0%03o"),
    T_END
};

static const Translate msg_control[] = {
    TF3(0x00ff, IPC_RMID, IPC_RMID),
    TF3(0x00ff, IPC_SET,  IPC_SET),
    TF3(0x00ff, IPC_STAT, IPC_STAT),
    TF3(0x00ff, IPC_INFO, IPC_INFO),
    TF3(0x00ff, MSG_STAT, MSG_STAT),
    TF3(0x00ff, MSG_INFO, MSG_INFO),
    TF3(0xff00, 0x0100, IPC_64),
    T_END
};

static const Translate shm_control[] = {
    TF3(0x00ff, IPC_RMID, IPC_RMID),
    TF3(0x00ff, IPC_INFO, IPC_INFO),
    TF3(0x00ff, SHM_INFO, SHM_INFO),
    TF3(0x00ff, IPC_STAT, IPC_STAT),
    TF3(0x00ff, SHM_STAT, SHM_STAT),
    TF3(0x00ff, SHM_LOCK, SHM_LOCK),
    TF3(0x00ff, SHM_UNLOCK, SHM_UNLOCK),
    TF3(0x00ff, IPC_SET,  IPC_SET),
    TF3(0xff00, 0x0100,   IPC_64),
    T_END
};

static const Translate sem_control[] = {
    TF3(0x00ff, IPC_RMID, IPC_RMID),
    TF3(0x00ff, IPC_INFO, IPC_INFO),
    TF3(0x00ff, SEM_INFO, SEM_INFO),
    TF3(0x00ff, IPC_STAT, IPC_STAT),
    TF3(0x00ff, SEM_STAT, SEM_STAT),
    TF3(0x00ff, GETALL,   GETALL),
    TF3(0x00ff, GETVAL,   GETVAL),
    TF3(0x00ff, GETPID,   GETPID),
    TF3(0x00ff, GETNCNT,  GETNCNT),
    TF3(0x00ff, GETZCNT,  GETZCNT),
    TF3(0x00ff, SETVAL,   SETVAL),
    TF3(0x00ff, SETALL,   SETALL),
    TF3(0x00ff, IPC_SET,  IPC_SET),
    TF3(0xff00, 0x0100, IPC_64),
    T_END
};

/* The ipc64_perm struct as layed out in the 32-bit specimen */
struct ipc64_perm_32 {
    int32_t key;
    int32_t uid;
    int32_t gid;
    int32_t cuid;
    int32_t cgid;
    uint16_t mode;
    uint16_t pad1;
    uint16_t seq;
    uint16_t pad2;
    uint32_t unused1;
    uint32_t unused2;
} __attribute__((packed));

/* The msqid64_ds struct as layed out in the 32-bit specimen */
struct msqid64_ds_32 {
    ipc64_perm_32 msg_perm;
    int32_t msg_stime;
    int32_t unused1;
    int32_t msg_rtime;
    int32_t unused2;
    int32_t msg_ctime;
    int32_t unused3;
    uint32_t msg_cbytes;
    uint32_t msg_qnum;
    uint32_t msg_qbytes;
    int32_t msg_lspid;
    int32_t msg_lrpid;
    uint32_t unused4;
    uint32_t unused5;
};

/* The semid64_ds struct as layed out in the 32-bit specimen */
struct semid64_ds_32 {
    ipc64_perm_32   sem_perm;
    int32_t sem_otime;
    uint32_t unused1;
    int32_t sem_ctime;
    uint32_t unused2;
    uint32_t sem_nsems;
    uint32_t unused3;
    uint32_t unused4;
};

/* See <linux/ipc.h>; used by old version of msgrcv variety of syscall 117, ipc() */
struct ipc_kludge_32 {
    uint32_t msgp;
    uint32_t msgtyp;
} __attribute__((packed));

/* The shmid64_ds struct as layed out in the 32-bit specimen */
struct shmid64_ds_32 {
    ipc64_perm_32 shm_perm;             /* operation perms */
    uint32_t shm_segsz;                 /* size of segment (bytes) */
    int32_t shm_atime;                  /* last attach time */
    int32_t unused1;
    int32_t shm_dtime;                  /* last detach time */
    int32_t unused2;
    int32_t shm_ctime;                  /* last change time */
    int32_t unused3;
    int32_t shm_cpid;                   /* pid of creator */
    int32_t shm_lpid;                   /* pid of last operator */
    uint32_t shm_nattch;                /* number of current attaches */
    int32_t unused4;
    int32_t unused5;
} __attribute__((packed));

/* The shm_info struct as layed out in the 32-bit specimen */
struct shm_info_32 {
    int32_t used_ids;
    uint32_t shm_tot;  /* total allocated shm */
    uint32_t shm_rss;  /* total resident shm */
    uint32_t shm_swp;  /* total swapped shm */
    uint32_t swap_attempts;
    uint32_t swap_successes;
} __attribute__((packed));

/* The seminfo struct as layed out in the 32-bit specimen */
struct seminfo_32 {
    int32_t        semmap;
    int32_t        semmni;
    int32_t        semmns;
    int32_t        semmnu;
    int32_t        semmsl;
    int32_t        semopm;
    int32_t        semume;
    int32_t        semusz;
    int32_t        semvmx;
    int32_t        semaem;
} __attribute__((packed));

/* kernel's struct shminfo64 on the host architecture */
struct shminfo64_native {
    unsigned long shmmax;
    unsigned long shmmin;
    unsigned long shmmni;
    unsigned long shmseg;
    unsigned long shmall;
    unsigned long unused1;
    unsigned long unused2;
    unsigned long unused3;
    unsigned long unused4;
};

/* kernel's struct shminfo64 on a 32-bit architecture */
struct shminfo64_32 {
    uint32_t shmmax;
    uint32_t shmmin;
    uint32_t shmmni;
    uint32_t shmseg;
    uint32_t shmall;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t unused4;
};

static const Translate clone_flags[] = {
    TF(CLONE_VM),                       /* 0x00000100  set if VM shared between processes */
    TF(CLONE_FS),                       /* 0x00000200  set if fs info shared between processes */
    TF(CLONE_FILES),                    /* 0x00000400  set if open files shared between processes */
    TF(CLONE_SIGHAND),                  /* 0x00000800  set if signal handlers and blocked signals shared */
    TF(CLONE_PTRACE),                   /* 0x00002000  set if we want to let tracing continue on the child too */
    TF(CLONE_VFORK),                    /* 0x00004000  set if the parent wants the child to wake it up on mm_release */
    TF(CLONE_PARENT),                   /* 0x00008000  set if we want to have the same parent as the cloner */
    TF(CLONE_THREAD),                   /* 0x00010000  Same thread group? */
    TF(CLONE_NEWNS),                    /* 0x00020000  New namespace group? */
    TF(CLONE_SYSVSEM),                  /* 0x00040000  share system V SEM_UNDO semantics */
    TF(CLONE_SETTLS),                   /* 0x00080000  create a new TLS for the child */
    TF(CLONE_PARENT_SETTID),            /* 0x00100000  set the TID in the parent */
    TF(CLONE_CHILD_CLEARTID),           /* 0x00200000  clear the TID in the child */
    TF(CLONE_UNTRACED),                 /* 0x00800000  set if the tracing process can't force CLONE_PTRACE on this clone */
    TF(CLONE_CHILD_SETTID),             /* 0x01000000  set the TID in the child */
#ifdef CLONE_STOPPED
    TF(CLONE_STOPPED),                  /* 0x02000000  Start in stopped state */
#endif
    TF_FMT(0xff, "(signal=0x%02x)"),    /* 0x000000ff  mask for signal to be sent at exit */
    /* stuff not defined by glibc */
    TF3(0x00400000, 0x00400000, CLONE_DETACHED), /* unused */
    TF3(0x04000000, 0x04000000, CLONE_NEWUTS),   /* New utsname group? */
    TF3(0x08000000, 0x08000000, CLONE_NEWIPC),   /* New ipcs */
    TF3(0x10000000, 0x10000000, CLONE_NEWUSER),  /* New user namespace */
    TF3(0x20000000, 0x20000000, CLONE_NEWPID),   /* New pid namespace */
    TF3(0x40000000, 0x40000000, CLONE_NEWNET),   /* New network namespace */
    TF3(0x80000000, 0x80000000, CLONE_IO),       /* Clone io context */
    T_END
};

/* The layout of the kernel struct pt_regs in a 32-bit specimen */
struct pt_regs_32 {
    uint32_t bx;
    uint32_t cx;
    uint32_t dx;
    uint32_t si;
    uint32_t di;
    uint32_t bp;
    uint32_t ax;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t orig_ax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
} __attribute__((packed));

// The layout of the kernel struct pt_regs in a 64-bit specimen
struct pt_regs_64 {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t bp;
    uint64_t bx;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t ax;
    uint64_t cx;
    uint64_t dx;
    uint64_t si;
    uint64_t di;
    uint64_t orig_ax;
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} __attribute__((packed));

// The architecture-independen version of pt_regs used by the simulator, a super-set of the architecture-specific versions.
struct PtRegs {
    // Common to all architectures
    uint64_t ip;                                        // instruction pointer, a.k.a. "pc"
    uint64_t flags;                                     // condition codes, a.k.a. "ccr"

    // Common to Intel x86 32 and 64 bit
    uint64_t ax;
    uint64_t bx;
    uint64_t cx;
    uint64_t dx;
    uint64_t si;
    uint64_t di;
    uint64_t orig_ax;
    uint64_t sp;
    uint64_t bp;
    uint64_t cs;
    uint64_t ss;

    // Only Intel x86 32-bit
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;

    // Only Intel x86 64-bit
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // Only ColdFire m68k
    uint32_t d0, d1, d2, d3, d4, d5, d6, d7;
    uint32_t a0, a1, a2, a3, a4, a5, a6, a7;
    uint16_t sr;

    PtRegs()
        : ip(0), flags(0), ax(0), bx(0), cx(0), dx(0), si(0), di(0), orig_ax(0), sp(0), bp(0), cs(0), ss(0),
          ds(0), es(0), fs(0), gs(0), r8(0), r9(0), r10(0), r11(0), r12(0), r13(0), r14(0), r15(0),
          d0(0), d1(0), d2(0), d3(0), d4(0), d5(0), d6(0), d7(0),
          a0(0), a1(0), a2(0), a3(0), a4(0), a5(0), a6(0), a7(0), sr(0) {}

    explicit PtRegs(const pt_regs_32 &x)
        : ip(x.ip), flags(x.flags), ax(x.ax), bx(x.bx), cx(x.cx), dx(x.dx), si(x.si), di(x.di), orig_ax(x.orig_ax),
          sp(x.sp), bp(x.bp), cs(x.cs), ss(x.ss), ds(x.ds), es(x.es), fs(x.fs), gs(x.gs),
          r8(0), r9(0), r10(0), r11(0), r12(0), r13(0), r14(0), r15(0),
          d0(0), d1(0), d2(0), d3(0), d4(0), d5(0), d6(0), d7(0),
          a0(0), a1(0), a2(0), a3(0), a4(0), a5(0), a6(0), a7(0), sr(0) {}

    explicit PtRegs(const pt_regs_64 &x)
        : ip(x.ip), flags(x.flags), ax(x.ax), bx(x.bx), cx(x.cx), dx(x.dx), si(x.si), di(x.di), orig_ax(x.orig_ax),
          sp(x.sp), bp(x.bp), cs(x.cs), ss(x.ss), ds(0), es(0), fs(0), gs(0), r8(x.r8), r9(x.r9),
          r10(x.r10), r11(x.r11), r12(x.r12), r13(x.r13), r14(x.r14), r15(x.r15),
          d0(0), d1(0), d2(0), d3(0), d4(0), d5(0), d6(0), d7(0),
          a0(0), a1(0), a2(0), a3(0), a4(0), a5(0), a6(0), a7(0), sr(0) {}

    pt_regs_32 get_pt_regs_32() const {
        pt_regs_32 x;
        x.ax = ax;
        x.bx = bx;
        x.cx = cx;
        x.dx = dx;
        x.si = si;
        x.di = di;
        x.flags = flags;
        x.orig_ax = orig_ax;
        x.ip = ip;
        x.sp = sp;
        x.bp = bp;
        x.cs = cs;
        x.ss = ss;
        x.ds = ds;
        x.es = es;
        x.fs = fs;
        x.gs = gs;
        ASSERT_require(sizeof(x) == 4 * 17); // did we omit an initialization?
        return x;
    }

    pt_regs_64 get_pt_regs_64() const {
        pt_regs_64 x;
        x.ax = ax;
        x.bx = bx;
        x.cx = cx;
        x.dx = dx;
        x.si = si;
        x.di = di;
        x.flags = flags;
        x.orig_ax = orig_ax;
        x.ip = ip;
        x.sp = sp;
        x.bp = bp;
        x.cs = cs;
        x.ss = ss;
        x.r8 = r8;
        x.r9 = r9;
        x.r10 = r10;
        x.r11 = r11;
        x.r12 = r12;
        x.r13 = r13;
        x.r14 = r14;
        x.r15 = r15;
        ASSERT_require(sizeof(x) == 8 * 21); // did we omit an initialization?
        return x;
    }
};

static const Translate protocol_families[] = {
    //TE2(1, PF_UNIX),
    TE2(1, PF_LOCAL),   /* POSIX name for AF_UNIX */
    TE2(2, PF_INET),
    TE2(3, PF_AX25),
    TE2(4, PF_IPX),
    TE2(5, PF_APPLETALK),
    TE2(6, PF_NETROM),
    TE2(7, PF_BRIDGE),
    TE2(8, PF_ATMPVC),
    TE2(9, PF_X25),
    TE2(10, PF_INET6),
    TE2(11, PF_ROSE),
    TE2(12, PF_DECnet),
    TE2(13, PF_NETBEUI),
    TE2(14, PF_SECURITY),
    TE2(15, PF_KEY),
    TE2(16, PF_NETLINK),
    //TE2(16, PF_ROUTE),        same as netlink, used by 4.4BSD
    TE2(17, PF_PACKET),
    TE2(18, PF_ASH),
    TE2(19, PF_ECONET),
    TE2(20, PF_ATMSVC),
    TE2(21, PF_RDS),
    TE2(22, PF_SNA),
    TE2(23, PF_IRDA),
    TE2(24, PF_PPPOX),
    TE2(25, PF_WANPIPE),
    TE2(26, PF_LLC),
    // 27, 28 unused
    TE2(29, PF_CAN),
    TE2(30, PF_TIPC),
    TE2(31, PF_BLUETOOTH),
    TE2(32, PF_IUCV),
    TE2(33, PF_RXRPC),
    TE2(34, PF_ISDN),
    TE2(35, PF_PHONET),
    TE2(36, PF_IEEE802154),
    TE2(37, PF_CAIF),
    T_END
};

static const Translate socket_types[] = {
    TE2(1, SOCK_STREAM),
    TE2(2, SOCK_DGRAM),
    TE2(3, SOCK_RAW),
    TE2(4, SOCK_RDM),
    TE2(5, SOCK_SEQPACKET),
    TE2(6, SOCK_DCCP),
    TE2(10, SOCK_PACKET),
    T_END
};

static const Translate socket_protocols[] = {
    TE2(1, IPPROTO_ICMP),
    TE2(2, IPPROTO_IGMP),
    TE2(4, IPPROTO_IPIP),
    TE2(6, IPPROTO_TCP),
    TE2(8, IPPROTO_EGP),
    TE2(12, IPPROTO_PUP),
    TE2(17, IPPROTO_UDP),
    TE2(22, IPPROTO_IDP),
    TE2(33, IPPROTO_DCCP),
    TE2(41, IPPROTO_IPV6),
    TE2(46, IPPROTO_RSVP),
    TE2(47, IPPROTO_GRE),
    TE2(50, IPPROTO_ESP),
    TE2(51, IPPROTO_AH),
    TE2(94, IPPROTO_BEETPH),
    TE2(103, IPPROTO_PIM),
    TE2(108, IPPROTO_COMP),
    TE2(132, IPPROTO_SCTP),
    TE2(136, IPPROTO_UDPLITE),
    TE2(255, IPPROTO_RAW), 
    TE2(0, IPPROTO_IP),
    T_END
};

struct sockaddr_32 {
    uint16_t    sa_family;
    char        sa_data[14];
} __attribute__((packed));

/* 32-bit kernel struct for sendmsg() and recvmsg(). */
struct msghdr_32 {
    uint32_t    msg_name;               /* optional ptr to source address if socket is unconnected */
    uint32_t    msg_namelen;            /* number of bytes pointed to by msg_name */
    uint32_t    msg_iov;                /* ptr to iovec scatter/gather array */
    uint32_t    msg_iovlen;             /* number of blocks */
    uint32_t    msg_control;            /* ptr to per-protocol magic (see kernel source code) */
    uint32_t    msg_controllen;         /* number of bytes pointed to by msg_control */
    uint32_t    msg_flags;
} __attribute__((packed));

/* 32-bit kernel struct to overlay onto parts of the msg_control member of msghdr_32 */
struct cmsghdr_32 {
    uint32_t    cmsg_len;               /* data byte count including this header */
    int32_t     cmsg_level;             /* originating protocol */
    int32_t     cmsg_type;              /* protocol-specific type */
} __attribute((packed));

/* Bit flags for msghdr_32.msg_flags */
static const Translate msghdr_flags[] = {
    TF3(0x1ffff, 0x00001, MSG_OOB),
    TF3(0x1ffff, 0x00002, MSG_PEEK),
    TF3(0x1ffff, 0x00004, MSG_DONTROUTE),
    TF3(0x1ffff, 0x00008, MSG_CTRUNC),
    TF3(0x1ffff, 0x00010, MSG_PROBE),
    TF3(0x1ffff, 0x00020, MSG_TRUNC),
    TF3(0x1ffff, 0x00040, MSG_DONTWAIT),
    TF3(0x1ffff, 0x00080, MSG_EOR),
    TF3(0x1ffff, 0x00100, MSG_WAITALL),
    TF3(0x1ffff, 0x00200, MSG_FIN),
    TF3(0x1ffff, 0x00400, MSG_SYN),
    TF3(0x1ffff, 0x00800, MSG_CONFIRM),
    TF3(0x1ffff, 0x01000, MSG_RST),
    TF3(0x1ffff, 0x02000, MSG_ERRQUEUE),
    TF3(0x1ffff, 0x04000, MSG_NOSIGNAL),
    TF3(0x1ffff, 0x08000, MSG_MORE),
    TF3(0x1ffff, 0x10000, MSG_WAITFORONEK),
    T_END
};

struct iovec_32 {
    uint32_t    iov_base;               /* address of buffer */
    uint32_t    iov_len;                /* size of buffer in bytes */
} __attribute__((packed));

struct iovec_64 {
    uint64_t    iov_base;                               // address of buffer
    uint64_t    iov_len;                                // size of buffer in bytes
} __attribute__((packed));

// /* Macros for creating ioctl command numbers. See the _IOC macro in linux source code. */
// extern unsigned invalid_size_arg_for_IOC; /* provoke linker error for invalid size arguments */
// #define LINUX_IOC_NONE 0u
// #define LINUX_IOC_WRITE 1u
// #define LINUX_IOC_READ 2u
// #define LINUX_IOC_TYPECHECK(t) ((sizeof(t) == sizeof(t[1]) && sizeof(t) < (1 << 14)) ? sizeof(t) : invalid_size_arg_for_IOC)
// #define LINUX_IOC(dir,type,nr,size) (((dir)<<30) | ((type)<<16) | ((nr)<<8) | (size))
// #define LINUX_IO(type,nr)            LINUX_IOC(LINUX_IOC_NONE,(type),(nr),0)
// #define LINUX_IOR(type,nr,size)      LINUX_IOC(LINUX_IOC_READ,(type),(nr),(LINUX_IOC_TYPECHECK(size)))
// #define LINUX_IOW(type,nr,size)      LINUX_IOC(LINUX_IOC_WRITE,(type),(nr),(LINUX_IOC_TYPECHECK(size)))
// #define LINUX_IOWR(type,nr,size)     LINUX_IOC(LINUX_IOC_READ|LINUX_IOC_WRITE,(type),(nr),(LINUX_IOC_TYPECHECK(size)))
// #define LINUX_IOR_BAD(type,nr,size)  LINUX_IOC(LINUX_IOC_READ,(type),(nr),sizeof(size))
// #define LINUX_IOW_BAD(type,nr,size)  LINUX_IOC(LINUX_IOC_WRITE,(type),(nr),sizeof(size))
// #define LINUX_IOWR_BAD(type,nr,size) LINUX_IOC(LINUX_IOC_READ|LINUX_IOC_WRITE,(type),(nr),sizeof(size))

/* command values for the ioctl syscall */
static const Translate ioctl_commands[] = {
    TE(TCGETS), TE(TCSETS), TE(TCSETSW), TE(TCGETA), TE(TIOCGPGRP), TE(TIOCSPGRP), TE(TIOCSWINSZ), TE(TIOCGWINSZ),
    TE2(0x82187201, VFAT_IOCTL_READDIR_BOTH),
    T_END
};
 
/* kernel struct termios on i686 */
struct termios_32 {
    uint32_t    c_iflag;                /* input mode flags */
    uint32_t    c_oflag;                /* output mode flags */
    uint32_t    c_cflag;                /* control mode flags */
    uint32_t    c_lflag;                /* local mode flags */
    uint8_t     c_line;                 /* line discipline */
    uint8_t     c_cc[19/*NCCS*/];       /* control characters */
} __attribute__((packed));

/* kernel struct termios on host architecture */
struct termios_native {
    unsigned int        c_iflag;
    unsigned int        c_oflag;
    unsigned int        c_cflag;
    unsigned int        c_lflag;
    uint8_t             c_line;
    uint8_t             c_cc[19/*NCCS*/];
};

static const Translate termios_iflags[] = {
    TF3(0000001, 0000001, IGNBRK),
    TF3(0000002, 0000002, BRKINT),
    TF3(0000004, 0000004, IGNPAR),
    TF3(0000010, 0000010, PARMRK),
    TF3(0000020, 0000020, INPCK),
    TF3(0000040, 0000040, ISTRIP),
    TF3(0000100, 0000100, INLCR),
    TF3(0000200, 0000200, IGNCR),
    TF3(0000400, 0000400, ICRNL),
    TF3(0001000, 0001000, IUCLC),
    TF3(0002000, 0002000, IXON),
    TF3(0004000, 0004000, IXANY),
    TF3(0010000, 0010000, IXOFF),
    TF3(0020000, 0020000, IMAXBEL),
    TF3(0040000, 0040000, IUTF8),
    T_END
};

static const Translate termios_oflags[] = {
    TF3(0000001, 0000001, OPOST),
    TF3(0000002, 0000002, OLCUC),
    TF3(0000004, 0000004, ONLCR),
    TF3(0000010, 0000010, OCRNL),
    TF3(0000020, 0000020, ONOCR),
    TF3(0000040, 0000040, ONLRET),
    TF3(0000100, 0000100, OFILL),
    TF3(0000200, 0000200, OFDEL),
    TF3(0000400, 0000400, NLDLY),
    TF3(0000000, 0000000, NL0),
    TF3(0000400, 0000400, NL1),
    TF3(0003000, 0003000, CRDLY),
    TF3(0000000, 0000000, CR0),
    TF3(0001000, 0001000, CR1),
    TF3(0002000, 0002000, CR2),
    TF3(0003000, 0003000, CR3),
    TF3(0014000, 0014000, TABDLY),
    TF3(0000000, 0000000, TAB0),
    TF3(0004000, 0004000, TAB1),
    TF3(0010000, 0010000, TAB2),
    TF3(0014000, 0014000, TAB3),
    TF3(0014000, 0014000, XTABS),
    TF3(0020000, 0020000, BSDLY),
    TF3(0000000, 0000000, BS0),
    TF3(0020000, 0020000, BS1),
    TF3(0040000, 0040000, VTDLY),
    TF3(0000000, 0000000, VT0),
    TF3(0040000, 0040000, VT1),
    TF3(0100000, 0100000, FFDLY),
    TF3(0000000, 0000000, FF0),
    TF3(0100000, 0100000, FF1),
    T_END
};

static const Translate termios_cflags[] = {
#define CBAUD     0010017
    
    TF3(0010017, 0000000, B0),
    TF3(0010017, 0000001, B50),
    TF3(0010017, 0000002, B75),
    TF3(0010017, 0000003, B110),
    TF3(0010017, 0000004, B134),
    TF3(0010017, 0000005, B150),
    TF3(0010017, 0000006, B200),
    TF3(0010017, 0000007, B300),
    TF3(0010017, 0000010, B600),
    TF3(0010017, 0000011, B1200),
    TF3(0010017, 0000012, B1800),
    TF3(0010017, 0000013, B2400),
    TF3(0010017, 0000014, B4800),
    TF3(0010017, 0000015, B9600),
    TF3(0010017, 0000016, B19200),
    TF3(0010017, 0000017, B38400),
    TF3(0010017, 0010000, BOTHER),
    TF3(0010017, 0010001, B57600),
    TF3(0010017, 0010002, B115200),
    TF3(0010017, 0010003, B230400),
    TF3(0010017, 0010004, B460800),
    TF3(0010017, 0010005, B500000),
    TF3(0010017, 0010006, B576000),
    TF3(0010017, 0010007, B921600),
    TF3(0010017, 0010010, B1000000),
    TF3(0010017, 0010011, B1152000),
    TF3(0010017, 0010012, B1500000),
    TF3(0010017, 0010013, B2000000),
    TF3(0010017, 0010014, B2500000),
    TF3(0010017, 0010015, B3000000),
    TF3(0010017, 0010016, B3500000),
    TF3(0010017, 0010017, B4000000),
    
    TF3(0000060, 0000060, CSIZE),
    TF3(0000000, 0000000, CS5),
    TF3(0000020, 0000020, CS6),
    TF3(0000040, 0000040, CS7),
    TF3(0000060, 0000060, CS8),
    TF3(0000100, 0000100, CSTOPB),
    TF3(0000200, 0000200, CREAD),
    TF3(0000400, 0000400, PARENB),
    TF3(0001000, 0001000, PARODD),
    TF3(0002000, 0002000, HUPCL),
    TF3(0004000, 0004000, CLOCAL),
    
    TF3(002003600000, 002003600000, CIBAUD),
    TF3(010000000000, 010000000000, CMSPAR),
    TF3(020000000000, 020000000000, CRTSCTS),

    T_END
};

static const Translate termios_lflags[] = {
    TF3(0000001, 0000001, ISIG),
    TF3(0000002, 0000002, ICANON),
    TF3(0000004, 0000004, XCASE),
    TF3(0000010, 0000010, ECHO),
    TF3(0000020, 0000020, ECHOE),
    TF3(0000040, 0000040, ECHOK),
    TF3(0000100, 0000100, ECHONL),
    TF3(0000200, 0000200, NOFLSH),
    TF3(0000400, 0000400, TOSTOP),
    TF3(0001000, 0001000, ECHOCTL),
    TF3(0002000, 0002000, ECHOPRT),
    TF3(0004000, 0004000, ECHOKE),
    TF3(0010000, 0010000, FLUSHO),
    TF3(0040000, 0040000, PENDIN),
    TF3(0100000, 0100000, IEXTEN),
    TF3(0200000, 0200000, EXTPROC),
    T_END
};

/* Kernel's struct winsize on i686 */
struct winsize_32 {
    uint16_t    ws_row;
    uint16_t    ws_col;
    uint16_t    ws_xpixel;
    uint16_t    ws_ypixel;
} __attribute__((packed));

/* struct winsize is same on 32 and 64 bit architectures */
typedef winsize_32 winsize_native;

static const Translate socketcall_commands[] = {
    TE2(1, SYS_SOCKET),
    TE2(2, SYS_BIND),
    TE2(3, SYS_CONNECT),
    TE2(4, SYS_LISTEN),
    TE2(5, SYS_ACCEPT),
    TE2(6, SYS_GETSOCKNAME),
    TE2(7, SYS_GETPEERNAME),
    TE2(8, SYS_SOCKETPAIR),
    TE2(9, SYS_SEND),
    TE2(10, SYS_RECV),
    TE2(11, SYS_SENDTO),
    TE2(12, SYS_RECVFROM),
    TE2(13, SYS_SHUTDOWN),
    TE2(14, SYS_SETSOCKOPT),
    TE2(15, SYS_GETSOCKOPT),
    TE2(16, SYS_SENDMSG),
    TE2(17, SYS_RECVMSG),
    TE2(18, SYS_ACCEPT4),
    TE2(19, SYS_RECVMMSG),
    T_END
};

/* Scheduling policy for sched_get_priority_max() et al. */
static const Translate scheduler_policies[] = {
    TF3(0x0fffffff, 0, SCHED_NORMAL),
    TF3(0x0fffffff, 1, SCHED_FIFO),
    TF3(0x0fffffff, 2, SCHED_RR),
    TF3(0x0fffffff, 3, SCHED_BATCH),
    TF3(0x0fffffff, 5, SCHED_IDLE),
    TF3(0x40000000, 0x40000000, SCHED_RESET_ON_FORK),
    T_END
};

/* Kernel's struct sched_param on i686 */
struct sched_param_32 {
    int32_t sched_priority;
} __attribute__((packed));

/* Kernel's struct new_utsname on i386 */
struct new_utsname_32 {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
} __attribute__((packed));

/* Kernel's struct new_utsname on amd64 */
struct new_utsname_64 {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

/* Third arg of madvise syscall */
static const Translate madvise_behaviors[] = {
    TE(MADV_NORMAL), TE(MADV_RANDOM), TE(MADV_SEQUENTIAL), TE(MADV_WILLNEED), TE(MADV_DONTNEED),
    TE(MADV_REMOVE), TE(MADV_DONTFORK), TE(MADV_DOFORK),
#ifdef MADV_HWPOISON
    TE(MADV_HWPOISON),
#endif
#ifdef MADV_SOFT_OFFLINE
    TE(MADV_SOFT_OFFLINE),
#endif
#ifdef MADV_MERGEABLE
    TE(MADV_MERGEABLE),
#endif
#ifdef MADV_UNMERGEABLE
    TE(MADV_UNMERGEABLE),
#endif
#ifdef MADV_HUGEPAGE
    TE(MADV_HUGEPAGE), TE(MADV_NOHUGEPAGE),
#endif
    T_END
};

/* Third arg of mmap syscall */
static const Translate mmap_pflags[] = {
    TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF3(-1, 0, PROT_NONE),
    T_END
};

/* Fourth argument of mmap syscall */
static const Translate mmap_mflags[] = {
    TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_ANONYMOUS), TF(MAP_DENYWRITE),
    TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), TF(MAP_GROWSDOWN),
    TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE), TF(MAP_POPULATE),
#ifdef MAP_32BIT
    TF(MAP_32BIT),
#endif
    T_END
};

/* Struct for argument of old mmap (syscall 90 in 32-bit x86) */
struct mmap_arg_struct_32 {
    uint32_t    addr;
    uint32_t    len;
    uint32_t    prot;
    uint32_t    flags;
    uint32_t    fd;
    uint32_t    offset;
} __attribute__((packed));

static const Translate prctl_options[] = {
    TE(PR_SET_PDEATHSIG),
    TE(PR_GET_PDEATHSIG),
    TE(PR_GET_DUMPABLE),
    TE(PR_SET_DUMPABLE),
    TE(PR_GET_UNALIGN),
    TE(PR_SET_UNALIGN),
    TE(PR_GET_KEEPCAPS),
    TE(PR_SET_KEEPCAPS),
    TE(PR_GET_FPEMU),
    TE(PR_SET_FPEMU),
    TE(PR_GET_FPEXC),
    TE(PR_SET_FPEXC),
    TE(PR_GET_TIMING),
    TE(PR_SET_TIMING),
    TE(PR_SET_NAME),
    TE(PR_GET_NAME),
    TE(PR_GET_ENDIAN),
    TE(PR_SET_ENDIAN),
#ifdef PR_GET_SECCOMP
    TE(PR_GET_SECCOMP),
#endif
#ifdef PR_SET_SECCOMP
    TE(PR_SET_SECCOMP),
#endif
#ifdef PR_CAPBSET_READ
    TE(PR_CAPBSET_READ),
#endif
#ifdef PR_CAPBSET_DROP
    TE(PR_CAPBSET_DROP),
#endif
#ifdef PR_GET_TSC
    TE(PR_GET_TSC),
#endif
#ifdef PR_SET_TSC
    TE(PR_SET_TSC),
#endif
#ifdef PR_GET_SECUREBITS
    TE(PR_GET_SECUREBITS),
#endif
#ifdef PR_SET_SECUREBITS
    TE(PR_SET_SECUREBITS),
#endif
#ifdef PR_SET_TIMERSLACK
    TE(PR_SET_TIMERSLACK),
#endif
#ifdef PR_GET_TIMERSLACK
    TE(PR_GET_TIMERSLACK),
#endif
#ifdef PR_TASK_PERF_EVENTS_DISABLE
    TE(PR_TASK_PERF_EVENTS_DISABLE),
#endif
#ifdef PR_TASK_PERF_EVENTS_ENABLE
    TE(PR_TASK_PERF_EVENTS_ENABLE),
#endif
#ifdef PR_MCE_KILL
    TE(PR_MCE_KILL),
#endif
#ifdef PR_MCE_KILL_GET
    TE(PR_MCE_KILL_GET),
#endif
    T_END
};

static const Translate fchmod_flags[] = {
    TF(AT_SYMLINK_NOFOLLOW),
    T_END
};

// Kernel sysinfo struct for native (differs from libc's struct sysinfo)
struct sysinfo_native {
    long uptime;
    unsigned long loads[3];
    unsigned long totalram;
    unsigned long freeram;
    unsigned long sharedram;
    unsigned long bufferram;
    unsigned long totalswap;
    unsigned long freeswap;
    unsigned short procs;
    unsigned short pad;      
    unsigned long totalhigh;
    unsigned long freehigh;
    unsigned int mem_unit;
    char _f[20 - 2*sizeof(unsigned long) - sizeof(int)];
};

// Kernel sysinfo struct for x86
struct sysinfo_32 {
    int32_t uptime;                                 /* Seconds since boot */
    uint32_t loads[3];                              /* 1, 5, and 15 minute load averages */
    uint32_t totalram;                              /* Total usable main memory size */
    uint32_t freeram;                               /* Available memory size */
    uint32_t sharedram;                             /* Amount of shared memory */
    uint32_t bufferram;                             /* Memory used by buffers */
    uint32_t totalswap;                             /* Total swap space size */
    uint32_t freeswap;                              /* swap space still available */
    uint16_t procs;                                 /* Number of current processes */
    uint16_t pad;                                   /* explicit padding for m68k */
    uint32_t totalhigh;                             /* Total high memory size */
    uint32_t freehigh;                              /* Available high memory size */
    uint32_t mem_unit;                              /* Memory unit size in bytes */
    uint8_t _f[8];                                  /* Padding for libc5 */

    sysinfo_32(const sysinfo_native &x)
        : uptime(x.uptime), totalram(x.totalram), freeram(x.freeram), sharedram(x.sharedram), bufferram(x.bufferram),
          totalswap(x.totalswap), freeswap(x.freeswap), procs(x.procs), pad(x.pad), totalhigh(x.totalhigh),
          freehigh(x.freehigh), mem_unit(x.mem_unit) {
        loads[0] = x.loads[0];
        loads[1] = x.loads[1];
        loads[2] = x.loads[2];
        memset(_f, 0, sizeof _f);
        memcpy(_f, x._f, std::min(sizeof(_f), sizeof(x._f)));
    }
} __attribute__((__packed__));

// Kernel sysinfo struct for amd64
struct sysinfo_64 {
    int64_t uptime;                                 /* Seconds since boot */
    uint64_t loads[3];                              /* 1, 5, and 15 minute load averages */
    uint64_t totalram;                              /* Total usable main memory size */
    uint64_t freeram;                               /* Available memory size */
    uint64_t sharedram;                             /* Amount of shared memory */
    uint64_t bufferram;                             /* Memory used by buffers */
    uint64_t totalswap;                             /* Total swap space size */
    uint64_t freeswap;                              /* swap space still available */
    uint16_t procs;                                 /* Number of current processes */
    uint16_t pad;                                   /* explicit padding for m68k */
    uint64_t totalhigh;                             /* Total high memory size */
    uint64_t freehigh;                              /* Available high memory size */
    uint32_t mem_unit;                              /* Memory unit size in bytes */

    sysinfo_64(const sysinfo_native &x)
        : uptime(x.uptime), totalram(x.totalram), freeram(x.freeram), sharedram(x.sharedram), bufferram(x.bufferram),
          totalswap(x.totalswap), freeswap(x.freeswap), procs(x.procs), pad(x.pad), totalhigh(x.totalhigh),
          freehigh(x.freehigh), mem_unit(x.mem_unit) {
        loads[0] = x.loads[0];
        loads[1] = x.loads[1];
        loads[2] = x.loads[2];
    }
} __attribute__((__packed__));

/* Conversion functions */
void convert(statfs_32 *g, const statfs64_native *h);
void convert(statfs_32 *g, const statfs_native *h);
void convert(statfs64_32 *g, const statfs64_native *h);
void convert(statfs64_32 *g, const statfs_native *h);


/* Functions to print various data structures */
void print_SegmentDescriptor(Sawyer::Message::Stream &f, const uint8_t *_ud, size_t sz);
void print_int_32(Sawyer::Message::Stream &f, const uint8_t *ptr, size_t sz);
void print_hex_64(Sawyer::Message::Stream &f, const uint8_t *ptr, size_t sz);
void print_rlimit_32(Sawyer::Message::Stream &f, const uint8_t *ptr, size_t sz);
void print_rlimit_64(Sawyer::Message::Stream &f, const uint8_t *ptr, size_t sz);
void print_kernel_stat_32(Sawyer::Message::Stream &f, const uint8_t *_sb, size_t sz);
void print_kernel_stat_64(Sawyer::Message::Stream &f, const uint8_t *_sb, size_t sz);
void print_timespec_32(Sawyer::Message::Stream &f, const uint8_t *_ts, size_t sz);
void print_timespec_64(Sawyer::Message::Stream &f, const uint8_t *_ts, size_t sz);
void print_timeval_32(Sawyer::Message::Stream &f, const uint8_t *_tv, size_t sz);
void print_timeval(Sawyer::Message::Stream &f, const uint8_t *_tv, size_t sz);
void print_sigaction_32(Sawyer::Message::Stream &f, const uint8_t *_sa, size_t sz);
void print_sigaction_64(Sawyer::Message::Stream &f, const uint8_t *_sa, size_t sz);
void print_dentries_32(Sawyer::Message::Stream &f, const uint8_t *sa, size_t sz);
void print_dentries_64(Sawyer::Message::Stream &f, const uint8_t *sa, size_t sz);
void print_dentries64_32(Sawyer::Message::Stream &f, const uint8_t *sa, size_t sz);
void print_bitvec(Sawyer::Message::Stream &f, const uint8_t *vec, size_t sz);
void print_SigSet(Sawyer::Message::Stream &f, const uint8_t *vec, size_t sz);
void print_stack_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_flock_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_flock_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_flock64_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_statfs_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_statfs_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_statfs64_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_statfs(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_robust_list_head_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_robust_list_head_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_ipc64_perm_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_msqid64_ds_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_ipc_kludge_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_shmid64_ds_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_shm_info_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_shminfo64_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_pt_regs_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_pt_regs_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_termios_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_winsize_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_exit_status_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_SigInfo(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_sched_param_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_msghdr_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_new_utsname_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_new_utsname_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_mmap_arg_struct_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_sockaddr(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_sysinfo_32(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);
void print_sysinfo_64(Sawyer::Message::Stream &f, const uint8_t *_v, size_t sz);

#endif /* ROSE_RSIM_Common_H */
