#ifndef ROSE_RSIM_Common_H
#define ROSE_RSIM_Common_H

#define __STDC_FORMAT_MACROS

#include "x86print.h"

#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/shm.h>

class RSIM_Process;
class RSIM_Thread;


/** Styles of core dump. */
enum CoreStyle {
    CORE_ELF=0x0001,
    CORE_ROSE=0x0002
};

/* Bit flags that control what to trace */
enum TraceFlags {
    TRACE_INSN          = 0x0001,               /**< Show each instruction that's executed. */
    TRACE_STATE         = 0x0002,               /**< Show machine state after each instruction. */
    TRACE_MEM           = 0x0004,               /**< Show memory read/write operations. */
    TRACE_MMAP          = 0x0008,               /**< Show changes in the memory map. */
    TRACE_SYSCALL       = 0x0010,               /**< Show each system call enter and exit. */
    TRACE_LOADER        = 0x0020,               /**< Show diagnostics for the program loading. */
    TRACE_PROGRESS      = 0x0040,               /**< Show a progress report now and then. */
    TRACE_SIGNAL        = 0x0080,               /**< Show reception and delivery of signals. */

    TRACE_ALL           = 0xffff                /**< Turn on all tracing bits. */
};

/* From linux/arch/x86/include/asm/ldt.h */
struct user_desc_32 {
    unsigned int  entry_number;
    unsigned int  base_addr;
    unsigned int  limit;
    unsigned int  seg_32bit:1;
    unsigned int  contents:2;
    unsigned int  read_exec_only:1;
    unsigned int  limit_in_pages:1;
    unsigned int  seg_not_present:1;
    unsigned int  useable:1;
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

struct timeval_32 {
    uint32_t tv_sec;        /* seconds */
    uint32_t tv_usec;       /* microseconds */
} __attribute__((packed));

static const Translate signal_names[] = {
    TE(SIGHUP), TE(SIGINT), TE(SIGQUIT), TE(SIGILL), TE(SIGTRAP), TE(SIGABRT), TE(SIGBUS), TE(SIGFPE), TE(SIGKILL),
    TE(SIGUSR1), TE(SIGSEGV), TE(SIGUSR2), TE(SIGPIPE), TE(SIGALRM), TE(SIGTERM), TE(SIGSTKFLT), TE(SIGCHLD), TE(SIGCONT),
    TE(SIGSTOP), TE(SIGTSTP), TE(SIGTTIN), TE(SIGTTOU), TE(SIGURG), TE(SIGXCPU), TE(SIGXFSZ), TE(SIGVTALRM), TE(SIGPROF),
    TE(SIGWINCH), TE(SIGIO), TE(SIGPWR), TE(SIGSYS), TE(SIGXFSZ),
    TE2(32, SIGRT32), TE2(33, SIGRT33), TE2(34, SIGRT34), TE2(35, SIGRT35), TE2(36, SIGRT36), TE2(37, SIGRT37),
    TE2(38, SIGRT38), TE2(39, SIGRT39), TE2(40, SIGRT40), TE2(41, SIGRT41), TE2(42, SIGRT42), TE2(43, SIGRT43),
    TE2(44, SIGRT44), TE2(45, SIGRT45), TE2(46, SIGRT46), TE2(47, SIGRT47), TE2(48, SIGRT48), TE2(49, SIGRT49),
    TE2(50, SIGRT50), TE2(51, SIGRT51), TE2(52, SIGRT52), TE2(53, SIGRT53), TE2(54, SIGRT54), TE2(55, SIGRT55),
    TE2(56, SIGRT56), TE2(57, SIGRT57), TE2(58, SIGRT58), TE2(59, SIGRT59), TE2(60, SIGRT60), TE2(61, SIGRT61),
    TE2(62, SIGRT62), TE2(63, SIGRT63),
    T_END};

static const Translate signal_flags[] = {
    TF(SA_NOCLDSTOP), TF(SA_NOCLDWAIT), TF(SA_NODEFER), TF(SA_ONSTACK), TF(SA_RESETHAND), TF(SA_RESTART),
    TF(SA_SIGINFO), T_END};

struct sigaction_32 {
    uint32_t handler_va;
    uint32_t flags;
    uint32_t restorer_va;
    uint64_t mask;
} __attribute__((packed));

static const Translate open_flags[] = { TF(O_RDWR), TF(O_RDONLY), TF(O_WRONLY),
                                        TF(O_CREAT), TF(O_EXCL), TF(O_NONBLOCK), TF(O_NOCTTY), TF(O_TRUNC),
                                        TF(O_APPEND), TF(O_NDELAY), TF(O_ASYNC), TF(O_FSYNC), TF(O_SYNC), TF(O_NOATIME),
                                        T_END };

/* Types for getdents syscalls */
struct dirent32_t {
    uint32_t d_ino;
    uint32_t d_off;
    uint16_t d_reclen;
} __attribute__((__packed__));

struct dirent64_t {
    uint64_t d_ino;
    uint64_t d_off;
    uint16_t d_reclen;
    uint8_t d_type;
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

/* kernels struct statfs on i686 */
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

struct statfs_64_native {
    unsigned f_type;
    unsigned f_bsize;
    uint64_t f_blocks;
    uint64_t f_bfree;
    uint64_t f_bavail;
    uint64_t f_files;
    uint64_t f_ffree;
    unsigned f_fsid[2];
    unsigned f_namelen;
    unsigned f_frsize;
    unsigned f_flags;
    unsigned f_spare[4];
};

struct robust_list_head_32 {
    uint32_t next;              /* virtual address of next entry in list; points to self if list is empty */
    uint32_t futex_va;          /* address of futex in user space */
    uint32_t pending_va;        /* address of list element being added while it is being added */
} __attribute__((packed));

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
    TF(CLONE_STOPPED),                  /* 0x02000000  Start in stopped state */
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

/* command values for the ioctl syscall */
static const Translate ioctl_commands[] = {
    TE(TCGETS), TE(TCSETS), TE(TCSETSW), TE(TCGETA), TE(TIOCGPGRP), TE(TIOCSPGRP), TE(TIOCSWINSZ), TE(TIOCGWINSZ),
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





/* Functions to print various data structures */
int print_user_desc_32(FILE *f, const uint8_t *_ud, size_t sz);
int print_int_32(FILE *f, const uint8_t *ptr, size_t sz);
int print_rlimit(FILE *f, const uint8_t *ptr, size_t sz);
int print_kernel_stat_32(FILE *f, const uint8_t *_sb, size_t sz);
int print_timespec_32(FILE *f, const uint8_t *_ts, size_t sz);
int print_timeval_32(FILE *f, const uint8_t *_tv, size_t sz);
int print_sigaction_32(FILE *f, const uint8_t *_sa, size_t sz);
int print_dentries_helper(FILE *f, const uint8_t *_sa, size_t sz, size_t wordsize);
int print_dentries_32(FILE *f, const uint8_t *sa, size_t sz);
int print_dentries_64(FILE *f, const uint8_t *sa, size_t sz);
int print_bitvec(FILE *f, const uint8_t *vec, size_t sz);
int print_sigmask(FILE *f, const uint8_t *vec, size_t sz);
int print_stack_32(FILE *f, const uint8_t *_v, size_t sz);
int print_flock_32(FILE *f, const uint8_t *_v, size_t sz);
int print_flock64_32(FILE *f, const uint8_t *_v, size_t sz);
int print_statfs_32(FILE *f, const uint8_t *_v, size_t sz);
int print_statfs64_32(FILE *f, const uint8_t *_v, size_t sz);
int print_robust_list_head_32(FILE *f, const uint8_t *_v, size_t sz);
int print_ipc64_perm_32(FILE *f, const uint8_t *_v, size_t sz);
int print_msqid64_ds_32(FILE *f, const uint8_t *_v, size_t sz);
int print_ipc_kludge_32(FILE *f, const uint8_t *_v, size_t sz);
int print_shmid64_ds_32(FILE *f, const uint8_t *_v, size_t sz);
int print_shm_info_32(FILE *f, const uint8_t *_v, size_t sz);
int print_shminfo64_32(FILE *f, const uint8_t *_v, size_t sz);
int print_pt_regs_32(FILE *f, const uint8_t *_v, size_t sz);
int print_termios_32(FILE *f, const uint8_t *_v, size_t sz);
int print_winsize_32(FILE *f, const uint8_t *_v, size_t sz);

#endif /* ROSE_RSIM_Common_H */
