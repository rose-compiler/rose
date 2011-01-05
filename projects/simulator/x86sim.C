/* Emulates an executable. */
#include "rose.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

/* Define one CPP symbol to determine whether this simulator can be compiled.  The definition of this one symbol depends on
 * all the header file prerequisites. */
#if defined(HAVE_ASM_LDT_H) && defined(HAVE_ELF_H) &&                                                                          \
    defined(HAVE_LINUX_TYPES_H) && defined(HAVE_LINUX_DIRENT_H) && defined(HAVE_LINUX_UNISTD_H)
#  define ROSE_ENABLE_SIMULATOR
#else
#  undef ROSE_ENABLE_SIMULATOR
#endif

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */


#include "x86print.h"
#include "VirtualMachineSemantics.h"
#include "BinaryLoaderElf.h"
#include <stdarg.h>
#include <boost/regex.hpp>

/* These are necessary for the system call emulation */
#include <asm/ldt.h>
#include <errno.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <utime.h>


/* AS extra required headers for system call simulation */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/types.h>
#include <linux/dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <asm/ldt.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h> 
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/socket.h>

/* Define this if you want strict emulation. When defined, every attempt is made for x86sim to provide an environment as close
 * as possible to running natively on hudson-rose-07.  Note that defining this might cause the simulator to malfunction since
 * some specimens will attempt to execute instructions that are not implemented in the x86 instruction semantics yet. */
// #define X86SIM_STRICT_EMULATION


enum CoreStyle { CORE_ELF=0x0001, CORE_ROSE=0x0002 }; /*bit vector*/

#define CONV_FIELD(var1, var2, field) var1.field = var2.field
#ifndef HAVE_USER_DESC
typedef modify_ldt_ldt_s user_desc;
#endif

static bool did_fork = false;

static const int PROGRESS_INTERVAL = 10; /* seconds */
static int had_alarm = 0;
static void alarm_handler(int) {
    had_alarm = 1;
}

static int
print_user_desc(FILE *f, const uint8_t *_ud, size_t sz)
{
    const user_desc *ud = (const user_desc*)_ud;
    assert(sizeof(*ud)==sz);

    const char *content_type = "unknown";
    switch (ud->contents) {
        case 0: content_type = "data"; break;
        case 1: content_type = "stack"; break;
        case 2: content_type = "code"; break;
    }

    return fprintf(f, "entry=%d, base=0x%08lx, limit=0x%08lx, %s, %s, %s, %s, %s, %s",
                   (int)ud->entry_number, (unsigned long)ud->base_addr, (unsigned long)ud->limit,
                   ud->seg_32bit ? "32bit" : "16bit",
                   content_type, ud->read_exec_only ? "read_exec" : "writable",
                   ud->limit_in_pages ? "page_gran" : "byte_gran",
                   ud->seg_not_present ? "not_present" : "present",
                   ud->useable ? "usable" : "not_usable");
}

static int
print_int_32(FILE *f, const uint8_t *ptr, size_t sz)
{
    assert(4==sz);
    return fprintf(f, "%"PRId32, *(const int32_t*)ptr);
}

static const Translate rlimit_resources[] = {TE(RLIMIT_CPU), TE(RLIMIT_FSIZE), TE(RLIMIT_DATA), TE(RLIMIT_STACK),
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
                                             T_END};

static int
print_rlimit(FILE *f, const uint8_t *ptr, size_t sz)
{
    assert(8==sz); /* two 32-bit unsigned integers */
    int retval = 0;
    if (0==~((const uint32_t*)ptr)[0]) {
        retval += fprintf(f, "rlim_cur=unlimited");
    } else {
        retval += fprintf(f, "rlim_cur=%"PRIu32, ((const uint32_t*)ptr)[0]);
    }
    if (0==~((const uint32_t*)ptr)[1]) {
        retval += fprintf(f, ", rlim_max=unlimited");
    } else {
        retval += fprintf(f, ", rlim_max=%"PRIu32, ((const uint32_t*)ptr)[1]);
    }
    return retval;
}

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

static int
print_kernel_stat_32(FILE *f, const uint8_t *_sb, size_t sz)
{
    assert(sz==sizeof(kernel_stat_32));
    const kernel_stat_32 *sb = (const kernel_stat_32*)_sb;
    return fprintf(f, "dev=%"PRIu64", ino=%"PRIu64", mode=0%03"PRIo32", nlink=%"PRIu32", uid=%"PRIu32", gid=%"PRIu32
                   ", rdev=%"PRIu64", size=%"PRIu64", blksz=%"PRIu32", blocks=%"PRIu64", ...",
                   sb->dev, sb->ino, sb->mode, sb->nlink, sb->user, sb->group,
                   sb->rdev, sb->size, sb->blksize, sb->nblocks);
}

static const Translate clock_names[] = {
    TE(CLOCK_REALTIME), TE(CLOCK_MONOTONIC), TE(CLOCK_PROCESS_CPUTIME_ID), TE(CLOCK_THREAD_CPUTIME_ID),
    T_END
};

struct timespec_32 {
    uint32_t tv_sec;
    uint32_t tv_nsec;
} __attribute__((packed));

static int
print_timespec_32(FILE *f, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_32 *ts = (const timespec_32*)_ts;
    return fprintf(f, "sec=%"PRIu32", nsec=%"PRIu32, ts->tv_sec, ts->tv_nsec);
}

struct timeval_32 {
    uint32_t tv_sec;        /* seconds */
    uint32_t tv_usec;       /* microseconds */
} __attribute__((packed));

static int
print_timeval_32(FILE *f, const uint8_t *_tv, size_t sz)
{
    assert(sz==sizeof(timeval_32));
    const timeval_32 *tv = (const timeval_32*)_tv;
    return fprintf(f, "sec=%"PRIu32", usec=%"PRIu32, tv->tv_sec, tv->tv_usec);
}

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

static int
print_sigaction_32(FILE *f, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_32));
    const sigaction_32 *sa = (const sigaction_32*)_sa;
    return (fprintf(f, "handler=0x%08"PRIx32", flags=", sa->handler_va) +
            print_flags(f, signal_flags, sa->flags) +
            fprintf(f, ", restorer=0x%08"PRIx32", mask=0x%016"PRIx64, sa->restorer_va, sa->mask));
}

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

static int
print_dentries_helper(FILE *f, const uint8_t *_sa, size_t sz, size_t wordsize)
{
    if (0==sz)
        return fprintf(f, "empty");

    int retval = fprintf(f, "\n");
    size_t offset = 0;
    for (size_t i=0; offset<sz; i++) {
        uint64_t d_ino, d_off;
        int d_reclen, d_type;
        const char *d_name;
        if (4==wordsize) {
            const dirent32_t *sa = (const dirent32_t*)(_sa+offset);
            d_ino = sa->d_ino;
            d_off = sa->d_off;
            d_reclen = sa->d_reclen;
            d_type = _sa[offset+sa->d_reclen-1];
            d_name = (const char*)_sa + offset + sizeof(*sa);
        } else {
            ROSE_ASSERT(8==wordsize);
            const dirent64_t *sa = (const dirent64_t*)(_sa+offset);
            d_ino = sa->d_ino;
            d_off = sa->d_off;
            d_reclen = sa->d_reclen;
            d_type = sa->d_type;
            d_name = (const char*)_sa + offset + sizeof(*sa);
        }
        retval += fprintf(f,
                          "        dentry[%3zu]: ino=%-8"PRIu64" next_offset=%-8"PRIu64" reclen=%-3d"
                          " type=%-3d name=\"%s\"\n", i, d_ino, d_off, d_reclen, d_type, d_name);
        offset = d_off;
        if (0==offset) break;
    }
    return retval + fprintf(f, "    ");
}

static int
print_dentries_32(FILE *f, const uint8_t *sa, size_t sz)
{
    return print_dentries_helper(f, sa, sz, 4);
}

static int
print_dentries_64(FILE *f, const uint8_t *sa, size_t sz)
{
    return print_dentries_helper(f, sa, sz, 8);
}

static int
print_bitvec(FILE *f, const uint8_t *vec, size_t sz)
{
    int result=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                result += fprintf(f, "%s%zu", result?",":"", i*8+j);
            }
        }
    }
    return result;
}

static int
print_sigmask(FILE *f, const uint8_t *vec, size_t sz)
{
    int result=0, nsigs=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                uint32_t signo = i*8+j + 1;
                if (nsigs++) result += fprintf(f, ",");
                result += print_enum(f, signal_names, signo);
            }
        }
    }
    return result;
}

static const Translate stack_flags[] = {TF(SS_DISABLE), TF(SS_ONSTACK), T_END};

struct stack_32 {
    uint32_t ss_sp;
    uint32_t ss_flags;
    uint32_t ss_size;
} __attribute__((packed));

static int
print_stack_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(stack_32)==sz);
    const stack_32 *v = (const stack_32*)_v;
    int result = fprintf(f, "sp=0x%08"PRIx32", flags=", v->ss_sp);
    result += print_flags(f, stack_flags, v->ss_flags);
    result += fprintf(f, ", sz=%"PRIu32, v->ss_size);
    return result;
}

static const Translate seek_whence[] = {TE(SEEK_SET), TE(SEEK_CUR), TE(SEEK_END), T_END};

static const Translate flock_types[] = {TE(F_RDLCK), TE(F_WRLCK), TE(F_UNLCK), T_END};

/* The kernel struct flock for a 32-bit guest on a 32- or 64-bit host */
struct flock_32 {
    uint16_t l_type;
    uint16_t l_whence;
    int32_t l_start;
    int32_t l_len;
    int32_t l_pid;
} __attribute__((packed));

/* The kernel struct flock for a 32-bit guest using 64-bit files on a 32- or 64-bit host */
struct flock_64 {
    uint16_t l_type;
    uint16_t l_whence;
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
} __attribute__((packed));

/* The kernel struct flock on the host */
struct flock_64_native {
    short l_type;
    short l_whence;
    long long l_start;
    long long l_len;
    int l_pid;
};

#if 0 /* not used currently, but will be for syscall 143 when implemented */
static int
print_flock_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(flock_32)==sz);
    const flock_32 *v = (const flock_32*)_v;
    int retval = fprintf(f, "type=");
    retval += print_enum(f, flock_types, v->l_type);
    retval += fprintf(f, ", whence=");
    retval += print_enum(f, seek_whence, v->l_whence);
    retval += fprintf(f, ", start=%"PRId32", len=%"PRId32", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
    return retval;
}
#endif

static int
print_flock_64(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(flock_64)==sz);
    const flock_64 *v = (const flock_64*)_v;
    int retval = fprintf(f, "type=");
    retval += print_enum(f, flock_types, v->l_type);
    retval += fprintf(f, ", whence=");
    retval += print_enum(f, seek_whence, v->l_whence);
    retval += fprintf(f, ", start=%"PRId64", len=%"PRId64", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
    return retval;
}

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

static int
print_statfs_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(statfs_32)==sz);
    const statfs_32 *v = (const statfs_32*)_v;
    return fprintf(f, "type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu32", bfree=%"PRIu32", bavail=%"PRIu32", files=%"PRIu32
                   ", ffree=%"PRIu32", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
                   ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
                   v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
                   v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
                   v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

struct robust_list_head_32 {
    uint32_t next;              /* virtual address of next entry in list; points to self if list is empty */
    uint32_t futex_va;          /* address of futex in user space */
    uint32_t pending_va;        /* address of list element being added while it is being added */
} __attribute__((packed));

static int
print_robust_list_head_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(robust_list_head_32)==sz);
    const robust_list_head_32 *v = (const robust_list_head_32*)_v;
    return fprintf(f, "next=0x%08"PRIx32", futex=0x%08"PRIx32", pending=0x%08"PRIx32,
                   v->next, v->futex_va, v->pending_va);
}

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

static const Translate ipc_control[] = {
    TF3(0x00ff, IPC_RMID, IPC_RMID),
    TF3(0x00ff, IPC_SET,  IPC_SET),
    TF3(0x00ff, IPC_STAT, IPC_STAT),
    TF3(0x00ff, IPC_INFO, IPC_INFO),
    TF3(0x00ff, MSG_STAT, MSG_STAT),
    TF3(0x00ff, MSG_INFO, MSG_INFO),
    TF3(0x00ff, SHM_INFO, SHM_INFO),
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

static int
print_ipc64_perm_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(ipc64_perm_32)==sz);
    const ipc64_perm_32 *v = (const ipc64_perm_32*)_v;
    return fprintf(f,
                   "key=%"PRId32", uid=%"PRId32", gid=%"PRId32", cuid=%"PRId32", cgid=%"PRId32
                   ", mode=0%03"PRIo16", seq=%"PRIu16,
                   v->key, v->uid, v->gid, v->cuid, v->cgid, v->mode, v->seq);
}

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

static int
print_msqid64_ds_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(msqid64_ds_32)==sz);
    const msqid64_ds_32 *v = (const msqid64_ds_32*)_v;
    
    int retval = fprintf(f, "msg_perm={");
    retval += print_ipc64_perm_32(f, _v, sizeof(v->msg_perm));
    retval += fprintf(f, "}");

    retval += fprintf(f,
                      ", stime=%"PRId32", rtime=%"PRId32", ctime=%"PRId32
                      ", cbytes=%"PRIu32", qnum=%"PRIu32", qbytes=%"PRIu32
                      ", lspid=%"PRId32", lrpid=%"PRId32,
                      v->msg_stime, v->msg_rtime, v->msg_ctime,
                      v->msg_cbytes, v->msg_qnum, v->msg_qbytes,
                      v->msg_lspid, v->msg_lrpid);
    return retval;
}

/* See <linux/ipc.h>; used by old version of msgrcv variety of syscall 117, ipc() */
struct ipc_kludge_32 {
    uint32_t msgp;
    uint32_t msgtyp;
} __attribute__((packed));

static int
print_ipc_kludge_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(ipc_kludge_32)==sz);
    const ipc_kludge_32 *v = (const ipc_kludge_32*)_v;
    return fprintf(f, "msgp=0x%08"PRIx32", msgtype=%"PRId32, v->msgp, v->msgtyp);
}

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

static int
print_shmid64_ds_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(shmid64_ds_32)==sz);
    const shmid64_ds_32 *v = (const shmid64_ds_32*)_v;

    int retval = fprintf(f, "shm_perm={");
    retval += print_ipc64_perm_32(f, _v, sizeof(v->shm_perm));
    retval += fprintf(f, "}");

    retval += fprintf(f,
                      ", segsz=%"PRIu32
                      ", atime=%"PRId32", dtime=%"PRId32", ctime=%"PRId32
                      ", cpid=%"PRId32", lpid=%"PRId32,
                      v->shm_segsz,
                      v->shm_atime, v->shm_dtime, v->shm_ctime,
                      v->shm_cpid, v->shm_lpid);
    return retval;
}

/* The shm_info struct as layed out in the 32-bit specimen */
struct shm_info_32 {
    int32_t used_ids;
    uint32_t shm_tot;  /* total allocated shm */
    uint32_t shm_rss;  /* total resident shm */
    uint32_t shm_swp;  /* total swapped shm */
    uint32_t swap_attempts;
    uint32_t swap_successes;
} __attribute__((packed));

static int
print_shm_info_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(shm_info_32)==sz);
    const shm_info_32 *v = (const shm_info_32*)_v;

    return fprintf(f, "used_ids=%"PRId32", shm_tot=%"PRIu32", shm_rss=%"PRIu32", shm_swp=%"PRIu32
                   ", swap_attempts=%"PRIu32", swap_successes=%"PRIu32,
                   v->used_ids, v->shm_tot, v->shm_rss, v->shm_swp,
                   v->swap_attempts, v->swap_successes);
}

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

static int
print_pt_regs_32(FILE *f, const uint8_t *_v, size_t sz)
{
    ROSE_ASSERT(sizeof(pt_regs_32)==sz);
    const pt_regs_32 *v = (const pt_regs_32*)_v;
    return fprintf(f, "bx=0x%"PRIx32", cx=0x%"PRIx32", dx=0x%"PRIx32", si=0x%"PRIx32", di=0x%"PRIx32
                   ", bp=0x%"PRIx32", ax=0x%"PRIx32", ds=0x%"PRIx32", es=0x%"PRIx32", fs=0x%"PRIx32
                   ", gs=0x%"PRIx32", orig_ax=0x%"PRIx32", ip=0x%"PRIx32", cs=0x%"PRIx32", flags=0x%"PRIx32
                   ", sp=0x%"PRIx32", ss=0x%"PRIx32,
                   v->bx, v->cx, v->dx, v->si, v->di,
                   v->bp, v->ax, v->ds, v->es, v->fs,
                   v->gs, v->orig_ax, v->ip, v->cs, v->flags,
                   v->sp, v->ss);
}

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








/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that it's symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class EmulationPolicy: public VirtualMachineSemantics::Policy {
public:
    struct SegmentInfo {
        uint32_t base, limit;
        bool present;
        SegmentInfo(): base(0), limit(0), present(false) {}
        SegmentInfo(const user_desc &ud) {
            base = ud.base_addr;
            limit = ud.limit_in_pages ? (ud.limit << 12) | 0xfff : ud.limit;
            present = !ud.seg_not_present && ud.useable;
        }
    };

    /* Thrown by exit system calls. */
    struct Exit {
        explicit Exit(int status): status(status) {}
        int status;                             /* same value as returned by waitpid() */
    };

    /* Thrown for signals. */
    struct Signal {
        explicit Signal(int signo): signo(signo) {}
        int signo;
    };

public:
    std::string exename;                        /* Name of executable without any path components */
    std::string interpname;                     /* Name of interpreter from ".interp" section or "--interp=" switch */
    std::vector<std::string> exeargs;           /* Specimen argv with PATH-resolved argv[0] */
    MemoryMap *map;                             /* Describes how specimen's memory is mapped to simulator memory */
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /* Cache of disassembled instructions */
    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    static const size_t n_gdt=8192;             /* Number of global descriptor table entries */
    user_desc gdt[n_gdt];                       /* Global descriptor table */
    SegmentInfo segreg_shadow[6];               /* Shadow values of segment registers from GDT */
    uint32_t mmap_start;                        /* Minimum address to use when looking for mmap free space */
    bool mmap_recycle;                          /* If false, then never reuse mmap addresses */
    sigaction_32 signal_action[_NSIG+1];        /* Simulated actions for signal handling */
    uint64_t signal_mask;                       /* Set by sigsetmask(), specifies signals that are not delivered */
    std::queue<int> signal_queue;               /* List of pending signals in the order they arrived provided not masked */
    bool signal_reprocess;                      /* Set to true if we might need to reprocess the signal_queue */
    stack_32 signal_stack;                      /* Stack to use for signal handlers specifying SA_ONSTACK flag */
    uint32_t signal_oldstack;                   /* Stack pointer before currently executing signal handler, or zero */
    uint32_t signal_return;                     /* Return address for currently executing signal handler, or zero */
    std::vector<uint32_t> auxv;                 /* Auxv vector pushed onto initial stack; also used when dumping core */
    static const uint32_t brk_base=0x08000000;  /* Lowest possible brk() value */
    std::string vdso_name;                      /* Optional base name of virtual dynamic shared object from kernel */
    std::vector<std::string> vdso_paths;        /* Directories and/or filenames to search for vdso */
    rose_addr_t vdso_mapped_va;                 /* Address where vdso is mapped into specimen, or zero */
    rose_addr_t vdso_entry_va;                  /* Entry address for vdso, or zero */
    unsigned core_styles;                       /* What kind of core dump(s) to make for dump_core() */
    std::string core_base_name;                 /* Name to use for core files ("core") */
    rose_addr_t ld_linux_base_va;               /* Base address for ld-linux.so if no preferred addresss for "LOAD#0" */
    uint32_t robust_list_head_va;               /* Address of robust futex list head. See set_robust_list() syscall */

    static const uint32_t SIGHANDLER_RETURN = 0xdeceaced;

#if 0
    uint32_t gsOffset;
    void (*eipShadow)();
    uint32_t signalStack;
    std::vector<user_desc> thread_areas;
#endif
    

    /* Debugging, tracing, etc. */
    FILE *debug;                                /* Stream to which debugging output is sent (or NULL to suppress it) */
    bool trace_insn;                            /* Show each instruction that's executed */
    bool trace_state;                           /* Show machine state after each instruction */
    bool trace_mem;                             /* Show memory read/write operations */
    bool trace_mmap;                            /* Show changes in the memory map */
    bool trace_syscall;                         /* Show each system call */
    bool trace_loader;                          /* Show diagnostics for the program loading */
    bool trace_progress;			/* Show progress now and then */
    bool trace_signal;                          /* Show reception and delivery of signals */

    EmulationPolicy()
        : map(NULL), disassembler(NULL), brk_va(0), mmap_start(0x40000000ul), mmap_recycle(false), signal_mask(0),
          signal_reprocess(true), signal_oldstack(0), signal_return(0), vdso_mapped_va(0), vdso_entry_va(0),
          core_styles(CORE_ELF), core_base_name("x-core.rose"), ld_linux_base_va(0x40000000), robust_list_head_va(0),
          debug(NULL), trace_insn(false), trace_state(false), trace_mem(false), trace_mmap(false), trace_syscall(false),
          trace_loader(false), trace_progress(false), trace_signal(false) {

        vdso_name = "x86vdso";
        vdso_paths.push_back(".");
#ifdef X86_VDSO_PATH_1
        vdso_paths.push_back(X86_VDSO_PATH_1);
#endif
#ifdef X86_VDSO_PATH_2
        vdso_paths.push_back(X86_VDSO_PATH_2);
#endif
        

        for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
            writeGPR((X86GeneralPurposeRegister)i, 0);
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
            writeFlag((X86Flag)i, 0);
        writeIP(0);
        writeFlag((X86Flag)1, true_());
        writeGPR(x86_gpr_sp, 0xbffff000ul);     /* high end of stack, exclusive */

        memset(gdt, 0, sizeof gdt);
        gdt[0x23>>3].entry_number = 0x23>>3;
        gdt[0x23>>3].limit = 0x000fffff;
        gdt[0x23>>3].seg_32bit = 1;
        gdt[0x23>>3].read_exec_only = 1;
        gdt[0x23>>3].limit_in_pages = 1;
        gdt[0x23>>3].useable = 1;
        gdt[0x2b>>3].entry_number = 0x2b>>3;
        gdt[0x2b>>3].limit = 0x000fffff;
        gdt[0x2b>>3].seg_32bit = 1;
        gdt[0x2b>>3].limit_in_pages = 1;
        gdt[0x2b>>3].useable = 1;

        writeSegreg(x86_segreg_cs, 0x23);
        writeSegreg(x86_segreg_ds, 0x2b);
        writeSegreg(x86_segreg_es, 0x2b);
        writeSegreg(x86_segreg_ss, 0x2b);
        writeSegreg(x86_segreg_fs, 0x2b);
        writeSegreg(x86_segreg_gs, 0x2b);

        memset(signal_action, 0, sizeof signal_action);
        signal_stack.ss_sp = 0;
        signal_stack.ss_size = 0;
        signal_stack.ss_flags = SS_DISABLE;
    }

    /* Print machine register state for debugging */
    void dump_registers(FILE *f) const {
        fprintf(f, "  Machine state:\n");
        fprintf(f, "    eax=0x%08"PRIx64" ebx=0x%08"PRIx64" ecx=0x%08"PRIx64" edx=0x%08"PRIx64"\n",
                readGPR(x86_gpr_ax).known_value(), readGPR(x86_gpr_bx).known_value(),
                readGPR(x86_gpr_cx).known_value(), readGPR(x86_gpr_dx).known_value());
        fprintf(f, "    esi=0x%08"PRIx64" edi=0x%08"PRIx64" ebp=0x%08"PRIx64" esp=0x%08"PRIx64" eip=0x%08"PRIx64"\n",
                readGPR(x86_gpr_si).known_value(), readGPR(x86_gpr_di).known_value(),
                readGPR(x86_gpr_bp).known_value(), readGPR(x86_gpr_sp).known_value(),
                get_ip().known_value());
        for (int i=0; i<6; i++) {
            X86SegmentRegister sr = (X86SegmentRegister)i;
            fprintf(f, "    %s=0x%04"PRIx64" base=0x%08"PRIx32" limit=0x%08"PRIx32" present=%s\n",
                    segregToString(sr), readSegreg(sr).known_value(), segreg_shadow[sr].base, segreg_shadow[sr].limit,
                    segreg_shadow[sr].present?"yes":"no");
        }

        uint32_t eflags = get_eflags();
        fprintf(f, "    flags: 0x%08"PRIx32":", eflags);
        static const char *flag_name[] = {"cf",  "#1",  "pf",   "#3",    "af",    "#5",  "zf",  "sf",
                                          "tf",  "if",  "df",   "of", "iopl0", "iopl1",  "nt", "#15",
                                          "rf",  "vm",  "ac",  "vif",   "vip",    "id", "#22", "#23",
                                          "#24", "#25", "#26", "#27",   "#28",   "#29", "#30", "#31"};
        for (uint32_t i=0; i<32; i++) {
            if (eflags & (1u<<i))
                fprintf(f, " %s", flag_name[i]);
        }
        fprintf(f, "\n");
    }

    uint32_t get_eflags() const {
        uint32_t eflags = 0;
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
            if (readFlag((X86Flag)i).is_known())
                eflags |= readFlag((X86Flag)i).known_value() ? 1u<<i : 0u;
        }
        return eflags;
    }

    /* Generate an ELF Core Dump on behalf of the specimen.  This is a real core dump that can be used with GDB and contains
     * the same information as if the specimen had been running natively and dumped its own core. In other words, the core
     * dump we generate here does not have references to the simulator even though it is being dumped by the simulator. */
    void dump_core(int signo, std::string base_name="");

    /* Recursively load an executable and its libraries libraries into memory, creating the MemoryMap object that describes
     * the mapping from the specimen's address space to the simulator's address space.
     *
     * There are two ways to load dynamic libraries:
     *   1. Load the dynamic linker (ld-linux.so) and simulate it in order to load the libraries.  This is the most accurate
     *      since it delegates the dynamic linking to the actual dynamic linker.  It thus allows different linkers to be
     *      used.
     *   2. Use Matt Brown's work to have ROSE itself resolve the dynamic linking issues.  This approach gives us better
     *      control over the finer details such as which directories are searched, etc. since we have total control over the
     *      linker.  However, Matt's work is not complete at this time [2010-07-20].
     *
     * We use the first approach. */
    SgAsmGenericHeader* load(const char *name);

    /* Initialize the stack for the specimen.  The argc and argv are the command-line of the specimen, not ROSE or the
     * simulator. */
    void initialize_stack(SgAsmGenericHeader*, int argc, char *argv[]);

    /* Returns instruction at current IP, disassembling it if necessary, and caching it. */
    SgAsmx86Instruction *current_insn();

    /* Returns an argument of a system call */
    uint32_t arg(int idx);

    /* Emulates a Linux system call from an INT 0x80 instruction. */
    void emulate_syscall();

    /* Print the name and arguments of a system call in a manner like strace using values in registers */
    void syscall_enter(const char *name, const char *fmt, ...);

    /* Print the name and arguments of a system call in a manner like strace using supplied valies */
    void syscall_enter(uint32_t *values, const char *name, const char *fmt, ...);

    /* Print the name and arguments of a system call in a manner like strace */
    void syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app);

    /* Print the return value of a system call in a manner like strace */
    void syscall_leave(const char *format, ...);

    /* Initializes an ArgInfo object to pass to syscall printing functions. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list *ap);

    /* Returns the memory address in ROSE where the specified specimen address is located. */
    void *my_addr(uint32_t va, size_t size);

    /* Does the opposite, more or less, of my_addr(). Return a specimen virtual address that maps to the specified address in
     * the simulator.  There may be more than one, in which case we return the lowest. */
    uint32_t guest_va(void *addr, size_t nbytes);

    /* Reads a NUL-terminated string from specimen memory. The NUL is not included in the string.  If a limit is specified then
     * the returned string will contain at most this many characters (a value of zero implies no limit).  If the string cannot
     * be read, then "error" (if non-null) will point to a true value and the returned string will include the characters up to
     * the error. */
    std::string read_string(uint32_t va, size_t limit=0, bool *error=NULL);

    /* Reads a vector of NUL-terminated strings from specimen memory. */
    std::vector<std::string> read_string_vector(uint32_t va);

    /* Conditionally adds a signal to the queue of pending signals */
    void signal_arrival(int signo);

    /* Dispatch all unmasked pending signals */
    void signal_dispatch();

    /* Dispatch a signal. That is, emulation the specimen's signal handler or default action. */
    void signal_dispatch(int signo);

    /* Handles return from a signal handler. */
    void signal_cleanup();

    /* Same as the x86_push instruction */
    void push(VirtualMachineSemantics::ValueType<32> n) {
        VirtualMachineSemantics::ValueType<32> new_sp = add(readGPR(x86_gpr_sp), number<32>(-4));
        writeMemory(x86_segreg_ss, new_sp, n, true_());
        writeGPR(x86_gpr_sp, new_sp);
    }

    /* Same as the x86_pop instruction */
    VirtualMachineSemantics::ValueType<32> pop() {
        VirtualMachineSemantics::ValueType<32> old_sp = readGPR(x86_gpr_sp);
        VirtualMachineSemantics::ValueType<32> retval = readMemory<32>(x86_segreg_ss, old_sp, true_());
        writeGPR(x86_gpr_sp, add(old_sp, number<32>(4)));
        return retval;
    }

    /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
    VirtualMachineSemantics::ValueType<1> undefined_() {
        return 1;
    }

    /* Called by X86InstructionSemantics for the HLT instruction */
    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    /* Called by X86InstructionSemantics for the INT instruction */
    void interrupt(uint8_t num) {
        if (num != 0x80) {
            fprintf(stderr, "Bad interrupt\n");
            abort();
        }
        emulate_syscall();
    }

    /* Called by X86InstructionSemantics for the SYSENTER instruction */
    void sysenter() {
        emulate_syscall();

        /* On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to
         * restart interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call
         * should return to the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER. */
        writeIP(add(readIP(), number<32>(9)));
    }

    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn) {
        if (debug && trace_insn) {
            if (isatty(fileno(debug))) {
                fprintf(debug, "\033[K\n[%07zu] %s\033[K\r\033[1A", get_ninsns(), unparseInstructionWithAddress(insn).c_str());
            } else {
                fprintf(debug, "[%07zu] 0x%08"PRIx64": %s\n", get_ninsns(), insn->get_address(), unparseInstruction(insn).c_str());
            }
        }
        VirtualMachineSemantics::Policy::startInstruction(insn);
    }

    /* Write value to a segment register and its shadow. */
    void writeSegreg(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<16> &val) {
        ROSE_ASSERT(3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
        VirtualMachineSemantics::Policy::writeSegreg(sr, val);
        segreg_shadow[sr] = gdt[val.known_value()>>3];
        ROSE_ASSERT(segreg_shadow[sr].present);
    }

    /* Reads memory from the memory map rather than the super class. */
    template <size_t Len> VirtualMachineSemantics::ValueType<Len>
    readMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
               const VirtualMachineSemantics::ValueType<1> cond) {
        ROSE_ASSERT(0==Len % 8 && Len<=64);
        uint32_t base = segreg_shadow[sr].base;
        uint32_t offset = addr.known_value();
        ROSE_ASSERT(offset <= segreg_shadow[sr].limit);
        ROSE_ASSERT(offset + (Len/8) - 1 <= segreg_shadow[sr].limit);

        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            uint8_t buf[Len/8];
            size_t nread = map->read(buf, base+offset, Len/8);
            if (nread!=Len/8)
                throw Signal(SIGSEGV);
            uint64_t result = 0;
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                result |= buf[j] << i;
            if (debug && trace_mem) {
                fprintf(debug, "  readMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32") -> 0x%08"PRIx64"\n",
                        Len, base, offset, base+offset, VirtualMachineSemantics::ValueType<Len>(result).known_value());
            }
            return result;
        } else {
            return 0;
        }
    }

    /* Writes memory to the memory map rather than the super class. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
                const VirtualMachineSemantics::ValueType<Len> &data,  VirtualMachineSemantics::ValueType<1> cond) {
        ROSE_ASSERT(0==Len % 8 && Len<=64);
        uint32_t base = segreg_shadow[sr].base;
        uint32_t offset = addr.known_value();
        ROSE_ASSERT(offset <= segreg_shadow[sr].limit);
        ROSE_ASSERT(offset + (Len/8) - 1 <= segreg_shadow[sr].limit);
        ROSE_ASSERT(data.is_known());
        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            if (debug && trace_mem) {
                fprintf(debug, "  writeMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32", 0x%08"PRIx64")\n",
                        Len, base, offset, base+offset, data.known_value());
            }
            uint8_t buf[Len/8];
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                buf[j] = (data.known_value() >> i) & 0xff;
            size_t nwritten = map->write(buf, base+offset, Len/8);
            if (nwritten!=Len/8) {
                /* Writing to mem that's not mapped results in SIGSEGV; writing to mem that's mapped without write permission
                 * results in SIGBUS. */
                if (map->find(base+offset)) {
                    throw Signal(SIGBUS);
                } else {
                    throw Signal(SIGSEGV);
                }
            }
        }
    }

    /* Helper functions for syscall 117, ipc() and related syscalls */
    void sys_semtimedop(uint32_t semid, uint32_t tsops_va, uint32_t nsops, uint32_t timeout_va);
    void sys_semget(uint32_t key, uint32_t nsems, uint32_t semflg);
    void sys_msgsnd(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg);
    void sys_msgrcv(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg);
    void sys_msgget(uint32_t key, uint32_t msgflg);
    void sys_msgctl(uint32_t msqid, uint32_t cmd, uint32_t buf_va);
    void sys_shmdt(uint32_t shmaddr_va);
    void sys_shmget(uint32_t key, uint32_t size, uint32_t shmflg);
    void sys_shmctl(uint32_t shmid, uint32_t cmd, uint32_t buf_va);
    void sys_shmat(uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t ptr);

    /* Helper function for syscall 102, socketcall() and related syscalls */
    void sys_socket(int family, int type, int protocol);
    void sys_bind(int fd, uint32_t addr_va, uint32_t addrlen);
    void sys_listen(int fd, int backlog);

    template<class guest_dirent_t> int getdents_syscall(int fd, uint32_t dirent_va, long sz);
};

/* Using the new interface is still about as complicated as the old interface because we need to perform only a partial link.
 * We want ROSE to link the interpreter (usually /lib/ld-linux.so) into the AST but not link in any other shared objects.
 * Then we want ROSE to map the interpreter (if present) and all main ELF Segments into the specimen address space but not
 * make any of the usual adjustments for ELF Sections that also specify a mapping. */

struct SimLoader: public BinaryLoaderElf {
public:
    SgAsmGenericHeader *interpreter;                    /* header linked into AST for .interp section */
    SgAsmGenericHeader *vdso;                           /* header for the vdso file, if any */
    rose_addr_t vdso_mapped_va;                         /* base address where vdso is mapped, or zero */
    rose_addr_t vdso_entry_va;                          /* entry address for the vdso, or zero */

    SimLoader(SgAsmInterpretation *interpretation, FILE *debug, std::string default_interpname)
        : interpreter(NULL), vdso(NULL), vdso_mapped_va(0), vdso_entry_va(0) {
        set_debug(debug);
        set_perform_dynamic_linking(false);             /* we explicitly link in the interpreter and nothing else */
        set_perform_remap(true);                        /* map interpreter and main binary into specimen memory */
        set_perform_relocations(false);                 /* allow simulated interpreter to perform relocation fixups */

        /* Link the interpreter into the AST */
        SgAsmGenericHeader *header = interpretation->get_headers()->get_headers().front();
        std::string interpreter_name = find_interpreter(header, default_interpname);
        if (!interpreter_name.empty()) {
            SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interpretation);
            ROSE_ASSERT(composite!=NULL);
            SgAsmGenericFile *ifile = createAsmAST(composite, interpreter_name);
            interpreter = ifile->get_headers()->get_headers().front();
        }
    }

    /* Finds the name of the interpreter (usually "/lib/ld-linux.so") if any. The name comes from the PT_INTERP section,
     * usually named ".interp".  If an interpreter name is supplied as an argument, then it will be used instead, but only
     * if a PT_INTERP section is present. */
    std::string find_interpreter(SgAsmGenericHeader *header, std::string default_interpname="") {
        struct: public SgSimpleProcessing {
            std::string interp_name;
            void visit(SgNode *node) {
                SgAsmElfSection *section = isSgAsmElfSection(node);
                SgAsmElfSegmentTableEntry *segment = section ? section->get_segment_entry() : NULL;
                if (segment && SgAsmElfSegmentTableEntry::PT_INTERP==segment->get_type()) {
                    char buf[section->get_size()];
                    section->read_content_local(0, buf, section->get_size());
                    interp_name = std::string(buf, section->get_size());
                }
            }
        } t1;
        t1.traverse(header, preorder);
        return (t1.interp_name.empty() || default_interpname.empty()) ? t1.interp_name : default_interpname;
    }

    /* Returns ELF PT_LOAD Segments in order by virtual address. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader *header) {
        SgAsmGenericSectionPtrList retval;
        SgAsmGenericSectionPtrList sections = BinaryLoaderElf::get_remap_sections(header);
        for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); si++) {
            SgAsmElfSection *section = isSgAsmElfSection(*si);
            SgAsmElfSegmentTableEntry *entry = section ? section->get_segment_entry() : NULL;
            if (entry && entry->get_type()==SgAsmElfSegmentTableEntry::PT_LOAD)
                retval.push_back(section);
        }
        return retval;
    }

    /* Load the specified file as a virtual dynamic shared object. Returns true if the vdso was found and mapped. The side
     * effect is that the "vdso", "vdso_mapped_va", and "vdso_entry_va" data members are initialized when the vdso is found and
     * mapped into memory. */
    bool map_vdso(const std::string &vdso_name, SgAsmInterpretation *interpretation, MemoryMap *map) {
        ROSE_ASSERT(vdso==NULL);
        ROSE_ASSERT(vdso_mapped_va==0);
        ROSE_ASSERT(vdso_entry_va==0);

        struct stat sb;
        if (stat(vdso_name.c_str(), &sb)<0 || !S_ISREG(sb.st_mode))
            return false;
        int fd = open(vdso_name.c_str(), O_RDONLY);
        if (fd<0)
            return false;
        
        SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interpretation);
        ROSE_ASSERT(composite!=NULL);
        SgAsmGenericFile *file = createAsmAST(composite, vdso_name);
        ROSE_ASSERT(file!=NULL);
        SgAsmGenericHeader *fhdr = file->get_headers()->get_headers()[0];
        ROSE_ASSERT(isSgAsmElfFileHeader(fhdr)!=NULL);
        rose_addr_t entry_rva = fhdr->get_entry_rva();

        uint8_t *buf = new uint8_t[sb.st_size];
        ssize_t nread = read(fd, buf, sb.st_size);
        ROSE_ASSERT(nread==sb.st_size);
        close(fd); fd=-1;

        vdso_mapped_va = ALIGN_UP(map->find_last_free(), PAGE_SIZE);
        vdso_mapped_va = std::max(vdso_mapped_va, (rose_addr_t)0x40000000); /* value used on hudson-rose-07 */
        MemoryMap::MapElement me(vdso_mapped_va, sb.st_size, buf, 0, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
        me.set_name("[vdso]");
        map->insert(me);

        if ((size_t)sb.st_size!=ALIGN_UP((size_t)sb.st_size, PAGE_SIZE)) {
            MemoryMap::MapElement me2(vdso_mapped_va+sb.st_size, ALIGN_UP(sb.st_size, PAGE_SIZE)-sb.st_size,
                                      MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
            me2.set_name(me.get_name());
            map->insert(me2);
        }

        vdso_entry_va = vdso_mapped_va + entry_rva;
        return true;
    }
};

SgAsmGenericHeader*
EmulationPolicy::load(const char *name)
{
    /* Find the executable by searching the PATH environment variable. The executable name and full path name are both saved
     * in the class (exename and exeargs[0]). */ 
    ROSE_ASSERT(exename.empty() && exeargs.empty());
    if (strchr(name, '/')) {
        if (access(name, R_OK)<0) {
            fprintf(stderr, "%s: %s\n", name, strerror(errno));
            exit(1);
        }
        exename = strrchr(name, '/')+1;
        exeargs.push_back(std::string(name));
    } else {
        const char *path_env = getenv("PATH");
        if (path_env) {
            std::string s = path_env;
            boost::regex re;
            re.assign("[:;]");
            boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
            boost::sregex_token_iterator iterEnd;
            for (; iter!=iterEnd; ++iter) {
                std::string fullname = *iter + "/" + name;
                if (access(fullname.c_str(), R_OK)>=0) {
                    exename = name;
                    exeargs.push_back(fullname);
                    break;
                }
            }
        }
    }
    if (exeargs.empty()) {
        fprintf(stderr, "%s: not found\n", name);
        exit(1);
    }
       
    /* Link the main binary into the AST without further linking, mapping, or relocating. */
    if (debug && trace_loader)
        fprintf(debug, "loading %s...\n", exeargs[0].c_str());
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeargs[0].c_str());
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    /* Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
     * (we'll use the PE interpretation). */
    SgAsmInterpretation *interpretation = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation).back();
    SgAsmGenericHeader *fhdr = interpretation->get_headers()->get_headers().front();
    writeIP(fhdr->get_entry_rva() + fhdr->get_base_va());

    /* Link the interpreter into the AST */
    SimLoader *loader = new SimLoader(interpretation, trace_loader ? debug : NULL, interpname);

    /* If we found an interpreter then use its entry address as the start of simulation.  When running the specimen directly
     * in Linux with "setarch i386 -LRB3", the ld-linux.so.2 gets mapped to 0x40000000 if it has no preferred address.  We can
     * accomplish the same thing simply by rebasing the library. */
    if (loader->interpreter) {
        SgAsmGenericSection *load0 = loader->interpreter->get_section_by_name("LOAD#0");
        if (load0 && load0->is_mapped() && load0->get_mapped_preferred_rva()==0 && load0->get_mapped_size()>0)
            loader->interpreter->set_base_va(ld_linux_base_va);
        writeIP(loader->interpreter->get_entry_rva() + loader->interpreter->get_base_va());
    }

    /* Sort the headers so they're in order by entry address. In other words, if the interpreter's entry address is below the
     * entry address of the main executable, then make sure the interpretter gets mapped first. */
    SgAsmGenericHeaderPtrList &headers = interpretation->get_headers()->get_headers();
    if (2==headers.size()) {
        if (headers[0]->get_base_va() + headers[0]->get_entry_rva() >
            headers[1]->get_base_va() + headers[1]->get_entry_rva())
            std::swap(headers[0], headers[1]);
    } else {
        ROSE_ASSERT(1==headers.size());
    }

    /* Map all segments into simulated memory */
    loader->load(interpretation);
    map = interpretation->get_map();

    /* Load and map the virtual dynamic shared library. */
    if (!loader->interpreter) {
        if (debug && trace_loader)
            fprintf(stderr, "warning: static executable; no vdso necessary\n");
    } else {
        bool vdso_loaded = false;
        for (size_t i=0; i<vdso_paths.size() && !vdso_loaded; i++) {
            for (int j=0; j<2 && !vdso_loaded; j++) {
                std::string vdso_name = vdso_paths[i] + (j ? "" : "/" + this->vdso_name);
                if (debug && trace_loader)
                    fprintf(debug, "looking for vdso: %s\n", vdso_name.c_str());
                if ((vdso_loaded = loader->map_vdso(vdso_name, interpretation, map))) {
                    vdso_mapped_va = loader->vdso_mapped_va;
                    vdso_entry_va = loader->vdso_entry_va;
                    if (debug && trace_loader) {
                        fprintf(debug, "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                                vdso_name.c_str(), vdso_mapped_va, vdso_entry_va);
                    }
                }
            }
        }
        if (!vdso_loaded && debug && trace_loader)
            fprintf(stderr, "warning: cannot find a virtual dynamic shared object\n");
    }

    /* Find a disassembler. */
    if (!disassembler) {
        disassembler = Disassembler::lookup(interpretation)->clone();
        disassembler->set_progress_reporting(NULL, 0); /* turn off progress reporting */
    }

    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address but
     * below 0x40000000 (the stack, and where ld-linux.so.2 might be loaded when loaded high). */
    rose_addr_t free_area = std::max(map->find_last_free(std::max(ld_linux_base_va, (rose_addr_t)0x40000000)),
                                     (rose_addr_t)brk_base);
    brk_va = ALIGN_UP(free_area, PAGE_SIZE);

    delete loader;
    return fhdr;
}

void EmulationPolicy::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x00015000;
    size_t sp = readGPR(x86_gpr_sp).known_value();
    size_t stack_addr = sp - stack_size;
    MemoryMap::MapElement melmt(stack_addr, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
    melmt.set_name("[stack]");
    map->insert(melmt);

    /* Initialize the stack with specimen's argc and argv. Also save the arguments in the class. */
    ROSE_ASSERT(exeargs.size()==1);                     /* contains only the executable path */
    std::vector<uint32_t> pointers;                     /* pointers pushed onto stack at the end of initialization */
    pointers.push_back(argc);
    for (int i=0; i<argc; i++) {
        std::string arg;
        if (0==i) {
            arg = exeargs[0];
        } else {
            arg = argv[i];
            exeargs.push_back(arg);
        }
        size_t len = arg.size() + 1; /*inc. NUL termination*/
        sp -= len;
        map->write(arg.c_str(), sp, len);
        pointers.push_back(sp);
        if (debug && trace_loader)
            fprintf(debug, "argv[%d] %zu bytes at 0x%08zu = \"%s\"\n", i, len, sp, arg.c_str());
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that
     * value to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the
     * X86SIM_ overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is deleted
     * from the list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the name to FOO
     * and leave it at that location. We do all this so that variables are in the same order whether run natively or under the
     * simulator. */
    std::map<std::string, std::string> envvars;
    std::map<std::string, std::string>::iterator found;
    for (int i=0; environ[i]; i++) {
        char *eq = strchr(environ[i], '=');
        ROSE_ASSERT(eq!=NULL);
        std::string var(environ[i], eq-environ[i]);
        std::string val(eq+1);
        envvars.insert(std::make_pair(var, val));
    }
    for (int i=0, j=0; environ[i]; i++) {
        char *eq = strchr(environ[i], '=');
        ROSE_ASSERT(eq!=NULL);
        std::string var(environ[i], eq-environ[i]);
        std::string val(eq+1);
        if (!strncmp(var.c_str(), "X86SIM_", 7) && environ[i]+7!=eq) {
            std::string var_short = var.substr(7);
            if ((found=envvars.find(var_short))==envvars.end()) {
                var = var_short;
                val = eq+1;
            } else {
                continue;
            }
        } else {
            std::string var_long = "X86SIM_" + var;
            if ((found=envvars.find(var_long))!=envvars.end()) {
                val = found->second;
            }
        }
        std::string env = var + "=" + val;
        sp -= env.size() + 1;
        map->write(env.c_str(), sp, env.size()+1);
        pointers.push_back(sp);
        if (debug && trace_loader)
            fprintf(debug, "environ[%d] %zu bytes at 0x%08zu = \"%s\"\n", j++, env.size(), sp, env.c_str());
    }
    pointers.push_back(0); /*environment NULL terminator*/

    /* Initialize stack with auxv, where each entry is two words in the pointers vector. This information is only present for
     * dynamically linked executables. The order and values were determined by running the simulator with the "--showauxv"
     * switch on hudson-rose-07. */
    if (fhdr->get_section_by_name(".interp")) {
        struct T1: public SgSimpleProcessing {
            rose_addr_t phdr_rva;
            T1(): phdr_rva(0) {}
            void visit(SgNode *node) {
                SgAsmElfSection *section = isSgAsmElfSection(node);
                SgAsmElfSegmentTableEntry *entry = section ? section->get_segment_entry() : NULL;
                if (0==phdr_rva && entry && entry->get_type()==SgAsmElfSegmentTableEntry::PT_PHDR)
                    phdr_rva = section->get_mapped_preferred_rva();
            }
        } t1;
        t1.traverse(fhdr, preorder);
        auxv.clear();

        if (vdso_mapped_va!=0) {
            /* AT_SYSINFO */
            auxv.push_back(32);
            auxv.push_back(vdso_entry_va);
            if (debug && trace_loader)
                fprintf(debug, "AT_SYSINFO:       0x%08"PRIx32"\n", auxv.back());

            /* AT_SYSINFO_PHDR */
            auxv.push_back(33);
            auxv.push_back(vdso_mapped_va);
            if (debug && trace_loader)
                fprintf(debug, "AT_SYSINFO_PHDR:  0x%08"PRIx32"\n", auxv.back());
        }

        /* AT_HWCAP (see linux <include/asm/cpufeature.h>). */
        auxv.push_back(16);
        uint32_t hwcap = 0xbfebfbfful; /* value used by hudson-rose-07 */
        auxv.push_back(hwcap);

        if (debug && trace_loader)
            fprintf(debug, "AT_HWCAP:         0x%08"PRIx32"\n", auxv.back());

        /* AT_PAGESZ */
        auxv.push_back(6);
        auxv.push_back(PAGE_SIZE);
        if (debug && trace_loader)
            fprintf(debug, "AT_PAGESZ:        %"PRId32"\n", auxv.back());

        /* AT_CLKTCK */
        auxv.push_back(17);
        auxv.push_back(100);
        if (debug && trace_loader)
            fprintf(debug, "AT_CLKTCK:        %"PRId32"\n", auxv.back());

        /* AT_PHDR */
        auxv.push_back(3); /*AT_PHDR*/
        auxv.push_back(t1.phdr_rva + fhdr->get_base_va());
        if (debug && trace_loader)
            fprintf(debug, "AT_PHDR:          0x%08"PRIx32"\n", auxv.back());

        /*AT_PHENT*/
        auxv.push_back(4);
        auxv.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
        if (debug && trace_loader)
            fprintf(debug, "AT_PHENT:         %"PRId32"\n", auxv.back());

        /* AT_PHNUM */
        auxv.push_back(5);
        auxv.push_back(fhdr->get_e_phnum());
        if (debug && trace_loader)
            fprintf(debug, "AT_PHNUM:         %"PRId32"\n", auxv.back());

        /* AT_BASE */
        auxv.push_back(7);
        auxv.push_back(ld_linux_base_va);
        if (debug && trace_loader)
            fprintf(debug, "AT_BASE:          0x%08"PRIx32"\n", auxv.back());

        /* AT_FLAGS */
        auxv.push_back(8);
        auxv.push_back(0);
        if (debug && trace_loader)
            fprintf(debug, "AT_FLAGS:         0x%08"PRIx32"\n", auxv.back());

        /* AT_ENTRY */
        auxv.push_back(9);
        auxv.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
        if (debug && trace_loader)
            fprintf(debug, "AT_ENTRY:         0x%08"PRIx32"\n", auxv.back());

        /* AT_UID */
        auxv.push_back(11);
        auxv.push_back(getuid());
        if (debug && trace_loader)
            fprintf(debug, "AT_UID:           %"PRId32"\n", auxv.back());

        /* AT_EUID */
        auxv.push_back(12);
        auxv.push_back(geteuid());
        if (debug && trace_loader)
            fprintf(debug, "AT_EUID:          %"PRId32"\n", auxv.back());

        /* AT_GID */
        auxv.push_back(13);
        auxv.push_back(getgid());
        if (debug && trace_loader)
            fprintf(debug, "AT_GID:           %"PRId32"\n", auxv.back());

        /* AT_EGID */
        auxv.push_back(14);
        auxv.push_back(getegid());
        if (debug && trace_loader)
            fprintf(debug, "AT_EGID:          %"PRId32"\n", auxv.back());

        /* AT_SECURE */
        auxv.push_back(23);
        auxv.push_back(false);
        if (debug && trace_loader)
            fprintf(debug, "AT_SECURE:        %"PRId32"\n", auxv.back());

        /* AT_PLATFORM */
        {
            const char *platform = "i686";
            size_t len = strlen(platform)+1;
            sp -= len;
            map->write(platform, sp, len);
            auxv.push_back(15);
            auxv.push_back(sp);
            if (debug && trace_loader)
                fprintf(debug, "AT_PLATFORM:      0x%08"PRIx32" (%s)\n", auxv.back(), platform);
        }
    }

    /* AT_NULL */
    auxv.push_back(0);
    auxv.push_back(0);
    pointers.insert(pointers.end(), auxv.begin(), auxv.end());

    /* Finalize stack initialization by writing all the pointers to data we've pushed:
     *    argc
     *    argv with NULL terminator
     *    environment with NULL terminator
     *    auxv pairs terminated with (AT_NULL,0)
     */
    sp &= ~3U; /*align to four-bytes*/
    sp -= 4 * pointers.size();
    map->write(&(pointers[0]), sp, 4*pointers.size());

    writeGPR(x86_gpr_sp, sp);
}

SgAsmx86Instruction *
EmulationPolicy::current_insn()
{
    rose_addr_t ip = readIP().known_value();

    /* Use the cached instruction if possible. */
    Disassembler::InstructionMap::iterator found = icache.find(ip);
    if (found!=icache.end()) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(found->second);
        ROSE_ASSERT(insn!=NULL); /*shouldn't be possible due to check below*/
        size_t insn_sz = insn->get_raw_bytes().size();
        SgUnsignedCharList curmem(insn_sz);
        size_t nread = map->read(&curmem[0], ip, insn_sz);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
        icache.erase(found);
    }

    /* Disassemble (and cache) a new instruction */
    SgAsmx86Instruction *insn = NULL;
    try {
        insn = isSgAsmx86Instruction(disassembler->disassembleOne(map, ip));
    } catch (Disassembler::Exception &e) {
        std::cerr <<e <<"\n";
        dump_core(SIGSEGV);
        throw;
    }
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    icache.insert(std::make_pair(ip, insn));
    return insn;
}

void
EmulationPolicy::dump_core(int signo, std::string base_name)
{
    if (base_name.empty())
        base_name = core_base_name;

    fprintf(stderr, "dumping specimen core...\n");
    fprintf(stderr, "memory map at time of core dump:\n");
    map->dump(stderr, "  ");

    if (core_styles & CORE_ROSE)
        map->dump(base_name);
    if (0==(core_styles & CORE_ELF))
        return;

    /* Get current instruction pointer. We subtract the size of the current instruction if we're in the middle of processing
     * an instruction because it would have already been incremented by the semantics. */ 
    uint32_t eip = readIP().known_value();
    if (get_insn())
        eip -= get_insn()->get_raw_bytes().size();

    SgAsmGenericFile *ef = new SgAsmGenericFile;
    ef->set_truncate_zeros(false);

    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef);
    fhdr->get_exec_format()->set_purpose(SgAsmExecutableFileFormat::PURPOSE_CORE_DUMP);
    fhdr->add_entry_rva(rose_rva_t(0, NULL));

    SgAsmElfSegmentTable *segtab = new SgAsmElfSegmentTable(fhdr);
    
    /*========================================================================================================================
     * NOTE section
     *======================================================================================================================== */

    SgAsmElfNoteSection *notes = new SgAsmElfNoteSection(fhdr);
    segtab->add_section(notes);
    notes->get_segment_entry()->set_type(SgAsmElfSegmentTableEntry::PT_NOTE);
    
    /* Note CORE.PRSTATUS(1)              (144 bytes) */
    struct prstatus {
        uint32_t signo;                 /* signal number */
        uint32_t code;                  /* extra signal code */
        uint32_t err_num;
        uint32_t cursig;                /* current signal; 2-bytes followed by 2 bytes of zero padding */
        uint32_t sigpend;               /* pending signals */
        uint32_t sighold;               /* set of held signals */
        uint32_t pid;
        uint32_t ppid;
        uint32_t pgrp;
        uint32_t sid;
        uint32_t user_sec;              /* user time */
        uint32_t user_usec;
        uint32_t sys_sec;               /* system time */
        uint32_t sys_usec;
        uint32_t cuser_sec;             /* cummulative user time */
        uint32_t cuser_usec;
        uint32_t csys_sec;              /* cummulative system time */
        uint32_t csys_usec;
        uint32_t bx;                    /* general purpose registers */
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
        uint32_t orig_ax;               /* ax value before syscall? */
        uint32_t ip;
        uint32_t cs;
        uint32_t flags;
        uint32_t sp;
        uint32_t ss;
        uint32_t fpvalid;               /* is math coprocessor being used? */
    } __attribute__((packed));
    struct prstatus prstatus;
    ROSE_ASSERT(144==sizeof prstatus);
    memset(&prstatus, 0, sizeof prstatus);
    prstatus.signo = prstatus.cursig = signo;
    prstatus.pid = getpid();
    prstatus.ppid = getppid();
    prstatus.pgrp = getpgrp();
    prstatus.sid = getsid(0);
    prstatus.bx = readGPR(x86_gpr_bx).known_value();
    prstatus.cx = readGPR(x86_gpr_cx).known_value();
    prstatus.dx = readGPR(x86_gpr_dx).known_value();
    prstatus.si = readGPR(x86_gpr_si).known_value();
    prstatus.di = readGPR(x86_gpr_di).known_value();
    prstatus.bp = readGPR(x86_gpr_bp).known_value();
    prstatus.ax = readGPR(x86_gpr_ax).known_value();
    prstatus.ds = readSegreg(x86_segreg_ds).known_value();
    prstatus.es = readSegreg(x86_segreg_es).known_value();
    prstatus.fs = readSegreg(x86_segreg_fs).known_value();
    prstatus.gs = readSegreg(x86_segreg_gs).known_value();
    prstatus.orig_ax = readGPR(x86_gpr_ax).known_value();
    prstatus.ip = eip;
    prstatus.cs = readSegreg(x86_segreg_cs).known_value();
    prstatus.flags = get_eflags();
    prstatus.sp = readGPR(x86_gpr_sp).known_value();
    prstatus.ss = readSegreg(x86_segreg_ss).known_value();
    prstatus.fpvalid = 0;     /*ROSE doesn't support floating point yet*/

    SgAsmElfNoteEntry *prstatus_note = new SgAsmElfNoteEntry(notes);
    prstatus_note->get_name()->set_string("CORE");
    prstatus_note->set_type(1); /*ET_PRSTATUS*/
    prstatus_note->set_payload(&prstatus, sizeof prstatus);
            
    /* Note CORE.PRPSINFO(3)              (124 bytes) */
    struct {
        uint8_t state;                                          /* numeric process state */
        uint8_t sname;                                          /* char for state (is this what ps(1) prints? */
        uint8_t zombie;
        uint8_t nice;                                           /* nice value */
        uint32_t flags;
        uint16_t uid;
        uint16_t gid;
        uint32_t pid;
        uint32_t ppid;
        uint32_t pgrp;
        uint32_t sid;
        char fname[16];                                         /* filename of executable */
        char psargs[80];                                        /* initial part of arg list */
    } __attribute__((packed)) prpsinfo;
    ROSE_ASSERT(124==sizeof prpsinfo);
    memset(&prpsinfo, 0, sizeof prpsinfo);
    prpsinfo.state = 0;                                         /* runable */
    prpsinfo.sname = 'R';
    prpsinfo.zombie = 0;
    prpsinfo.nice = getpriority(PRIO_PROCESS, getpid());
    prpsinfo.flags = 0x2400; /* see linux/sched.h PF_* bits (0x2000=>dumped core; 0x400=>forked but not exec*/
    prpsinfo.uid = geteuid();
    prpsinfo.gid = getegid();
    prpsinfo.pid = getpid();
    prpsinfo.ppid = getppid();
    prpsinfo.pgrp = getpgrp();
    prpsinfo.sid = getsid(0);
    strncpy(prpsinfo.fname, exename.c_str(), sizeof(prpsinfo.fname));
    std::string all_args;
    for (size_t i=0; i<exeargs.size(); i++)
        all_args += exeargs[i] + " "; /*yes, there's an extra space at the end*/
    strncpy(prpsinfo.psargs, all_args.c_str(), sizeof(prpsinfo.psargs));
    
    SgAsmElfNoteEntry *prpsinfo_note = new SgAsmElfNoteEntry(notes);
    prpsinfo_note->get_name()->set_string("CORE");
    prpsinfo_note->set_type(3); /*ET_PRPSINFO*/
    prpsinfo_note->set_payload(&prpsinfo, sizeof prpsinfo);

    /* Note CORE.AUXV(6) */
    SgAsmElfNoteEntry *auxv_note = new SgAsmElfNoteEntry(notes);
    auxv_note->get_name()->set_string("CORE");
    auxv_note->set_type(6);
    auxv_note->set_payload(&auxv[0], 4*auxv.size());

#if 0
    /* Note CORE.PRFPREG(2)               (108 bytes) */
    /* This was just copied straight from an actual core dump because we shouldn't need it here anyway. It's the
     * user_i387_struct defined in linux source code <include/asm/user_32.h> containing 27 doublewords. */
    uint8_t prfpreg[] = {
        0x7f, 0x03, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    SgAsmElfNoteEntry *prfpreg_note = new SgAsmElfNoteEntry(notes);
    prfpreg_note->get_name()->set_string("CORE");
    prfpreg_note->set_type(2);
    prfpreg_note->set_payload(prfpreg, sizeof prfpreg);
#endif
 
#if 0
    /* Note LINUX.PRXFPREG(0x46e62b7f)    (512 bytes) */
    /* FIXME: This was just copied straight from a real core dump. It's the user32_fxsr_struct defined in the linux source
     *        code <include/asm/user_32.h>.  I don't think we need it because we're not using floating point registers. */
    uint8_t prxfpreg[] = {
        0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    SgAsmElfNoteEntry *prxfpreg_note = new SgAsmElfNoteEntry(notes);
    prxfpreg_note->get_name()->set_string("LINUX");
    prxfpreg_note->set_type(0x46e62b7f);
    prxfpreg_note->set_payload(prxfpreg, sizeof prxfpreg);
#endif

    /* Note LINUX.386_TLS(0x200)          (48 bytes)  i386 TLS slots (struct user_desc)*/
    uint8_t i386_tls[] = {
        0x0c, 0x00, 0x00, 0x00, 0xb0, 0xd6, 0x18, 0x40, 0xff, 0xff, 0x0f, 0x00, 0x51, 0x00, 0x00, 0x00,
        0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00
    };
    SgAsmElfNoteEntry *i386_tls_note = new SgAsmElfNoteEntry(notes);
    i386_tls_note->get_name()->set_string("LINUX");
    i386_tls_note->set_type(0x200);
    i386_tls_note->set_payload(i386_tls, sizeof i386_tls);

    
    /*========================================================================================================================
     * LOAD sections
     *======================================================================================================================== */

    class SegmentBuilder: public SgAsmElfSection {
        MemoryMap       *map;                                   /* memory map for specimen's process address space */
    public:
        SegmentBuilder(SgAsmElfFileHeader *fhdr, MemoryMap *map, rose_addr_t va, rose_addr_t sz, unsigned perms)
            : SgAsmElfSection(fhdr), map(map) {
            set_purpose(SgAsmGenericSection::SP_PROGRAM);       /* Program-supplied text, data, etc. */
            set_offset(ALIGN_UP(get_offset(), 4096));
            set_size(sz);
            set_file_alignment(4096);
            set_mapped_alignment(4096);
            set_mapped_preferred_rva(va); /*va==rva for ELF*/
            set_mapped_size(sz);
            set_mapped_rperm(0!=(perms & MemoryMap::MM_PROT_READ));
            set_mapped_wperm(0!=(perms & MemoryMap::MM_PROT_WRITE));
            set_mapped_xperm(0!=(perms & MemoryMap::MM_PROT_EXEC));
        }
        virtual void unparse(std::ostream &f) const {
            if (0==get_size()) return;
            uint8_t buf[8192];
            rose_addr_t cur_va = get_mapped_preferred_va();     /* current virtual address */
            rose_addr_t nremain = get_mapped_size();            /* bytes remaining to be written to the file */
            rose_addr_t offset = 0;                             /* byte offset with respect to beginning of section */
            while (nremain>0) {
                rose_addr_t to_write = std::min(nremain, (rose_addr_t)sizeof buf);
                size_t nread = map->read(buf, cur_va, to_write);
#if 1
                memset(buf+nread, 0, to_write-nread);
#else
                ROSE_ASSERT(nread==to_write);
#endif
                offset = write(f, offset, to_write, buf);
                cur_va += to_write;
                nremain -= to_write;
            }
        }
    };

    /* We dump everything to the core file, although linux (by default) skips private and shared non-writable mappings that
     * have backing store. */
    const std::vector<MemoryMap::MapElement> &elmts = map->get_elements();
    std::vector<MemoryMap::MapElement>::const_iterator ei=elmts.begin();
    while (ei!=elmts.end()) {
        rose_addr_t va = ei->get_va();
        rose_addr_t sz = ei->get_size();
        unsigned perms = ei->get_mapperms();

        /* Combine elmts[i] with as many following elements as possible. */
        std::vector<MemoryMap::MapElement>::const_iterator ej=ei+1;
#if 0
        while (ej!=elmts.end() && va+sz==ej->get_va() && perms==ej->get_mapperms())
            sz += (ej++)->get_size();
#endif
        ei = ej;

        /* Create a segment */
        SgAsmElfSection *segment = new SegmentBuilder(fhdr, map, va, sz, perms);
        segtab->add_section(segment);
        segment->get_segment_entry()->set_type(SgAsmElfSegmentTableEntry::PT_LOAD);
    }

    /*========================================================================================================================
     * Generate the core file.
     *======================================================================================================================== */

    SgAsmExecutableFileFormat::unparseBinaryFormat(base_name, ef);
    //deleteAST(ef); /*FIXME [RPM 2010-09-18]*/
}

void *
EmulationPolicy::my_addr(uint32_t va, size_t nbytes)
{
    /* Obtain mapping information and check that the specified number of bytes are mapped. */
    const MemoryMap::MapElement *me = map->find(va);
    if (!me)
        return NULL;
    size_t offset = 0;
    try {
        offset = me->get_va_offset(va, nbytes);
    } catch (const MemoryMap::NotMapped) {
        return NULL;
    }

    return (uint8_t*)me->get_base() + offset;
}

uint32_t
EmulationPolicy::guest_va(void *addr, size_t nbytes)
{
    const std::vector<MemoryMap::MapElement> elmts = map->get_elements();
    for (std::vector<MemoryMap::MapElement>::const_iterator ei=elmts.begin(); ei!=elmts.end(); ++ei) {
        uint8_t *base = (uint8_t*)ei->get_base(false);
        rose_addr_t offset = ei->get_offset();
        size_t size = ei->get_size();
        if (base && addr>=base+offset && (uint8_t*)addr+nbytes<=base+offset+size)
            return ei->get_va() + ((uint8_t*)addr - (base+offset));
    }
    return 0;
}

std::string
EmulationPolicy::read_string(uint32_t va, size_t limit/*=0*/, bool *error/*=NULL*/)
{
    std::string retval;
    while (1) {
        uint8_t byte;
        size_t nread = map->read(&byte, va++, 1);
        if (1!=nread) {
            if (error)
                *error = true;
            return retval;
        }
        if (!byte)
            break;
        retval += byte;

        if (limit>0 && retval.size()>=limit)
            break;
    }
    if (error)
        *error = false;
    return retval;
}

std::vector<std::string>
EmulationPolicy::read_string_vector(uint32_t va)
{
    std::vector<std::string> vec;
    size_t size = 4;
    while(1) {
      char buf[size];
      size_t nread = map->read(buf, va, size);

      ROSE_ASSERT(nread == size);

      uint64_t result = 0;
      for (size_t i=0, j=0; i<size; i+=8, j++)
                result |= buf[j] << i;

      //FIXME (?) is this the correct test for memory being null?
      if ( result == 0 ) break;

      vec.push_back(read_string( result  ));
      
      va+=4;
    }
    return vec;
}

/* NOTE: not yet tested for guest_dirent_t == dirent64_t; i.e., the getdents64() syscall. [RPM 2010-11-17] */
template<class guest_dirent_t> /* either dirent32_t or dirent64_t */
int EmulationPolicy::getdents_syscall(int fd, uint32_t dirent_va, long sz)
{
    ROSE_ASSERT(sizeof(dirent64_t)>=sizeof(guest_dirent_t));

    /* Obtain guest's buffer and make sure we can write to it. The write check is necessary because we'll be accessing
     * the buffer directly below rather than going through the MemoryMap object. */
    long at = 0; /* position when filling specimen's buffer */
    uint8_t *guest_buf = (uint8_t*)my_addr(dirent_va, sz);
    char junk;
    if (NULL==guest_buf || 1!=map->read(&junk, dirent_va, 1))
        return -EFAULT;

    /* Read dentries from host kernel and copy to specimen's buffer. We must do this one dentry at a time because we don't want
     * to over read (there's no easy way to back up).  In other words, we read a dentry (but not more than what would fit in
     * the specimen) and if successful we copy to the specimen, translating from 64- to 32-bit.  The one-at-a-time requirement
     * is due to the return value being run-length encoded. */
    long status = -EINVAL; /* buffer too small */
    while (at+(long)sizeof(guest_dirent_t)<sz) {
        /* Read one dentry if possible */
        uint8_t host_buf[sz];
        dirent64_t *host_dirent = (dirent64_t*)host_buf;
        int limit = sizeof(*host_dirent);
        status = -EINVAL; /* buffer too small */
        while (limit<=sz-at && -EINVAL==status) {
            status = syscall(SYS_getdents64, fd, host_buf, limit++);
            if (-1==status) status = -errno;
        }

        /* Convert and copy the host dentry into the specimen memory. */
        if (status>0) {
            ROSE_ASSERT(status>(long)sizeof(*host_dirent));
            guest_dirent_t *guest_dirent = (guest_dirent_t*)(guest_buf+at);

            /* name */
            ROSE_ASSERT(host_dirent->d_reclen > sizeof(*host_dirent));
            char *name_src = (char*)host_dirent + sizeof(*host_dirent);
            char *name_dst = (char*)guest_dirent + sizeof(*guest_dirent);
            size_t name_sz = host_dirent->d_reclen - sizeof(*host_dirent);
            memcpy(name_dst, name_src, name_sz);
            
            /* inode */
            ROSE_ASSERT(host_dirent->d_ino == (host_dirent->d_ino & 0xffffffff));
            guest_dirent->d_ino = host_dirent->d_ino;

            /* record length */
            guest_dirent->d_reclen = host_dirent->d_reclen - sizeof(*host_dirent)
                                     + sizeof(*guest_dirent) + 2/*padding and d_type*/;

            /* type */
            if (sizeof(guest_dirent_t)==sizeof(dirent32_t)) {
                ROSE_ASSERT(host_dirent->d_type == (host_dirent->d_type & 0xff));
                ((uint8_t*)guest_dirent)[guest_dirent->d_reclen-1] = host_dirent->d_type;
            } else {
                ROSE_ASSERT(sizeof(guest_dirent_t)==sizeof(dirent64_t));
                ((uint8_t*)guest_dirent)[sizeof(*guest_dirent)-1] = host_dirent->d_type;
            }

            /* offset to next dentry */
            at += guest_dirent->d_reclen;
            guest_dirent->d_off = at;
        }
                
        /* Termination conditions */
        if (status<=0) break;
    }

    return at>0 ? at : status;
}

void
EmulationPolicy::sys_semtimedop(uint32_t semid, uint32_t tsops_va, uint32_t nsops, uint32_t timeout_va)
{
    writeGPR(x86_gpr_ax, -ENOSYS); /* FIXME */
}

void
EmulationPolicy::sys_semget(uint32_t key, uint32_t nsems, uint32_t semflg)
{
#ifdef SYS_ipc /* i686 */
    int result = syscall(SYS_ipc, 2, key, nsems, semflg);
#else
    int result = syscall(SYS_semget, key, nsems, semflg);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
EmulationPolicy::sys_msgsnd(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Read the message buffer from the specimen. */
    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    if (!buf) {
        writeGPR(x86_gpr_ax, -ENOMEM);
        return;
    }
    if (4+msgsz!=map->read(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }

    /* Message type must be positive */
    if (*(int32_t*)buf <= 0) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Convert message type from four to eight bytes if necessary */
    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        memmove(buf+8, buf+4, msgsz);
        memset(buf+4, 0, 4);
    }

    /* Try to send the message */
    int result = msgsnd(msqid, buf, msgsz, msgflg);
    if (-1==result) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    delete[] buf;
    writeGPR(x86_gpr_ax, result);
}

void
EmulationPolicy::sys_msgrcv(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    int result = msgrcv(msqid, buf, msgsz, msgtyp, msgflg);
    if (-1==result) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        uint64_t type = *(uint64_t*)buf;
        ROSE_ASSERT(0 == (type >> 32));
        memmove(buf+4, buf+8, msgsz);
    }

    if (4+msgsz!=map->write(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }

    delete[] buf;
    writeGPR(x86_gpr_ax, result);
}

void
EmulationPolicy::sys_msgget(uint32_t key, uint32_t msgflg)
{
    int result = msgget(key, msgflg);
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
EmulationPolicy::sys_msgctl(uint32_t msqid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case IPC_INFO:
        case MSG_INFO:
            writeGPR(x86_gpr_ax, -ENOSYS);              /* FIXME */
            break;

        case IPC_STAT:
        case MSG_STAT: {
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and msqid_ds from the kernel */
            static msqid_ds host_ds;
            int result = msgctl(msqid, cmd, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            msqid64_ds_32 guest_ds;
            guest_ds.msg_perm.key = host_ds.msg_perm.__key;
            guest_ds.msg_perm.uid = host_ds.msg_perm.uid;
            guest_ds.msg_perm.gid = host_ds.msg_perm.gid;
            guest_ds.msg_perm.cuid = host_ds.msg_perm.cuid;
            guest_ds.msg_perm.cgid = host_ds.msg_perm.cgid;
            guest_ds.msg_perm.mode = host_ds.msg_perm.mode;
            guest_ds.msg_perm.pad1 = host_ds.msg_perm.__pad1;
            guest_ds.msg_perm.seq = host_ds.msg_perm.__seq;
            guest_ds.msg_perm.pad2 = host_ds.msg_perm.__pad2;
            guest_ds.msg_perm.unused1 = host_ds.msg_perm.__unused1;
            guest_ds.msg_perm.unused2 = host_ds.msg_perm.__unused2;
            guest_ds.msg_stime = host_ds.msg_stime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.msg_rtime = host_ds.msg_rtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.msg_ctime = host_ds.msg_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.msg_cbytes = host_ds.__msg_cbytes;
            guest_ds.msg_qnum = host_ds.msg_qnum;
            guest_ds.msg_qbytes = host_ds.msg_qbytes;
            guest_ds.msg_lspid = host_ds.msg_lspid;
            guest_ds.msg_lrpid = host_ds.msg_lrpid;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=map->write(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case IPC_RMID: {
            /* NOTE: syscall tracing will not show "IPC_RMID" if the IPC_64 flag is also present */
            int result = msgctl(msqid, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case IPC_SET: {
            msqid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=map->read(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            static msqid_ds host_ds;
            host_ds.msg_perm.__key = guest_ds.msg_perm.key;
            host_ds.msg_perm.uid = guest_ds.msg_perm.uid;
            host_ds.msg_perm.gid = guest_ds.msg_perm.gid;
            host_ds.msg_perm.cuid = guest_ds.msg_perm.cuid;
            host_ds.msg_perm.cgid = guest_ds.msg_perm.cgid;
            host_ds.msg_perm.mode = guest_ds.msg_perm.mode;
            host_ds.msg_perm.__seq = guest_ds.msg_perm.seq;
            host_ds.msg_stime = guest_ds.msg_stime;
            host_ds.msg_rtime = guest_ds.msg_rtime;
            host_ds.msg_ctime = guest_ds.msg_ctime;
            host_ds.__msg_cbytes = guest_ds.msg_cbytes;
            host_ds.msg_qnum = guest_ds.msg_qnum;
            host_ds.msg_qbytes = guest_ds.msg_qbytes;
            host_ds.msg_lspid = guest_ds.msg_lspid;
            host_ds.msg_lrpid = guest_ds.msg_lrpid;

            int result = msgctl(msqid, cmd, &host_ds);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        default: {
            writeGPR(x86_gpr_ax, -EINVAL);
            break;
        }
    }
}

void
EmulationPolicy::sys_shmdt(uint32_t shmaddr_va)
{
    const MemoryMap::MapElement *me = map->find(shmaddr_va);
    if (!me || me->get_va()!=shmaddr_va || me->get_offset()!=0 || me->is_anonymous()) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    int result = shmdt(me->get_base());
    if (-1==result) {
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    map->erase(*me);
    writeGPR(x86_gpr_ax, result);
}

void
EmulationPolicy::sys_shmget(uint32_t key, uint32_t size, uint32_t shmflg)
{
    int result = shmget(key, size, shmflg);
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
EmulationPolicy::sys_shmctl(uint32_t shmid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case SHM_STAT:
        case IPC_STAT: {
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and shmid_ds from the kernel */
            static shmid_ds host_ds;
            int result = shmctl(shmid, cmd, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            shmid64_ds_32 guest_ds;
            guest_ds.shm_perm.key = host_ds.shm_perm.__key;
            guest_ds.shm_perm.uid = host_ds.shm_perm.uid;
            guest_ds.shm_perm.gid = host_ds.shm_perm.gid;
            guest_ds.shm_perm.cuid = host_ds.shm_perm.cuid;
            guest_ds.shm_perm.cgid = host_ds.shm_perm.cgid;
            guest_ds.shm_perm.mode = host_ds.shm_perm.mode;
            guest_ds.shm_perm.pad1 = host_ds.shm_perm.__pad1;
            guest_ds.shm_perm.seq = host_ds.shm_perm.__seq;
            guest_ds.shm_perm.pad2 = host_ds.shm_perm.__pad2;
            guest_ds.shm_perm.unused1 = host_ds.shm_perm.__unused1;
            guest_ds.shm_perm.unused2 = host_ds.shm_perm.__unused2;
            guest_ds.shm_segsz = host_ds.shm_segsz;
            guest_ds.shm_atime = host_ds.shm_atime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.shm_dtime = host_ds.shm_dtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.shm_ctime = host_ds.shm_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.shm_cpid = host_ds.shm_cpid;
            guest_ds.shm_lpid = host_ds.shm_lpid;
            guest_ds.shm_nattch = host_ds.shm_nattch;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=map->write(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case SHM_INFO: {
            shm_info host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            shm_info_32 guest_info;
            guest_info.used_ids = host_info.used_ids;
            guest_info.shm_tot = host_info.shm_tot;
            guest_info.shm_rss = host_info.shm_rss;
            guest_info.shm_swp = host_info.shm_swp;
            guest_info.swap_attempts = host_info.swap_attempts;
            guest_info.swap_successes = host_info.swap_successes;

            if (sizeof(guest_info)!=map->write(&guest_info, buf_va, sizeof guest_info)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case IPC_RMID: {
            int result = shmctl(shmid, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        default: {
            writeGPR(x86_gpr_ax, -EINVAL);
            break;
        }
    }
}

void
EmulationPolicy::sys_shmat(uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t shmaddr)
{
    if (0==shmaddr) {
        shmaddr = map->find_last_free();
    } else if (shmflg & SHM_RND) {
        shmaddr = ALIGN_DN(shmaddr, SHMLBA);
    } else if (ALIGN_DN(shmaddr, 4096)!=shmaddr) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* We don't handle SHM_REMAP */
    if (shmflg & SHM_REMAP) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Map shared memory into the simulator */
    void *buf = shmat(shmid, NULL, shmflg);
    if (!buf) {
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    /* Map simulator's shared memory into the specimen */
    shmid_ds ds;
    int status = shmctl(shmid, IPC_STAT, &ds);
    ROSE_ASSERT(status>=0);
    ROSE_ASSERT(ds.shm_segsz>0);
    unsigned perms = MemoryMap::MM_PROT_READ | ((shmflg & SHM_RDONLY) ? 0 : MemoryMap::MM_PROT_WRITE);
    MemoryMap::MapElement shm(shmaddr, ds.shm_segsz, buf, 0, perms);
    shm.set_name("shmat("+StringUtility::numberToString(shmid)+")");
    map->insert(shm);

    /* Return values */
    if (4!=map->write(&shmaddr, result_va, 4)) {
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }
    writeGPR(x86_gpr_ax, shmaddr);
}

void
EmulationPolicy::sys_socket(int family, int type, int protocol)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[3];
    a[0] = family;
    a[1] = type;
    a[2] = protocol;
    int result = syscall(SYS_socketcall, 1/*SYS_SOCKET*/, a);
#else /* amd64 */
    int result = syscall(SYS_socket, family, type, protocol);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
EmulationPolicy::sys_bind(int fd, uint32_t addr_va, uint32_t addrlen)
{
    if (addrlen<1 || addrlen>4096) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }
    uint8_t *addrbuf = new uint8_t[addrlen];
    if (addrlen!=map->read(addrbuf, addr_va, addrlen)) {
        writeGPR(x86_gpr_ax, -EFAULT);
        delete[] addrbuf;
        return;
    }

#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    ROSE_ASSERT(4==sizeof(void*));
    int a[3];
    a[0] = fd;
    a[1] = (int)addrbuf;
    a[2] = addrlen;
    int result = syscall(SYS_socketcall, 2/*SYS_BIND*/, a);
#else /* amd64 */
    int result = syscall(SYS_bind, fd, addrbuf, addrlen);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
    delete[] addrbuf;
}

void
EmulationPolicy::sys_listen(int fd, int backlog)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[2];
    a[0] = fd;
    a[1] = backlog;
    int result = syscall(SYS_socketcall, 4/*SYS_LISTEN*/, a);
#else /* amd64 */
    int result = syscall(SYS_listen, fd, backlog);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
EmulationPolicy::emulate_syscall()
{
    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     *          according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers. */
    unsigned callno = readGPR(x86_gpr_ax).known_value();




    switch (callno) {
        case 3: { /*read*/
            syscall_enter("read", "dpd");
            int fd=arg(0);
            uint32_t buf_va=arg(1), size=arg(2);
            char buf[size];
            ssize_t nread = read(fd, buf, size);
            if (-1==nread) {
                writeGPR(x86_gpr_ax, -errno);
            } else if (map->write(buf, buf_va, (size_t)nread)!=(size_t)nread) {
                writeGPR(x86_gpr_ax, -EFAULT);
            } else {
                writeGPR(x86_gpr_ax, nread);
            }
            syscall_leave("d-b", nread>0?nread:0);
            break;
        }

        case 4: { /*write*/
            syscall_enter("write", "dbd", arg(2));
            int fd=arg(0);
            uint32_t buf_va=arg(1);
            size_t size=arg(2);
            uint8_t buf[size];
            size_t nread = map->read(buf, buf_va, size);
            if (nread!=size) {
                writeGPR(x86_gpr_ax, -EFAULT);
            } else {
                ssize_t nwritten = write(fd, buf, size);
                if (-1==nwritten) {
                    writeGPR(x86_gpr_ax, -errno);
                } else {
                    writeGPR(x86_gpr_ax, nwritten);
                }
            }
            syscall_leave("d");
            break;
        }

        case 5: { /*open*/
            syscall_enter("open", "sf", open_flags);
            do {
                uint32_t filename_va=arg(0);
                bool error;
                std::string filename = read_string(filename_va, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                uint32_t flags=arg(1), mode=(flags & O_CREAT)?arg(2):0;
                int fd = open(filename.c_str(), flags, mode);
                if (-1==fd) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                writeGPR(x86_gpr_ax, fd);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 6: { /*close*/
            syscall_enter("close", "d");
            int fd=arg(0);
            if (1==fd || 2==fd) {
                /* ROSE is using these */
                writeGPR(x86_gpr_ax, -EPERM);
            } else {
                int status = close(fd);
                writeGPR(x86_gpr_ax, status<0 ? -errno : status);
            }
            syscall_leave("d");
            break;
        }

        case 7: { // waitpid
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("waitpid", "dpf", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1);
            int options=arg(2);
            int sys_status;
            int result = waitpid(pid, &sys_status, options);
            if (result == -1) {
                result = -errno;
            } else if (status_va) {
                uint32_t status_le;
                SgAsmExecutableFileFormat::host_to_le(sys_status, &status_le);
                size_t nwritten = map->write(&status_le, status_va, 4);
                ROSE_ASSERT(4==nwritten);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 8: { /* 0x8, creat */
            syscall_enter("creat", "sd");
            do {
                uint32_t filename = arg(0);
                bool error;
                std::string sys_filename = read_string(filename, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
	        mode_t mode = arg(1);

	        int result = creat(sys_filename.c_str(), mode);
                if (result == -1) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 10: { /*0xa, unlink*/
            syscall_enter("unlink", "s");
            do {
                uint32_t filename_va = arg(0);
                bool error;
                std::string filename = read_string(filename_va, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                int result = unlink(filename.c_str());
                if (result == -1) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }
                
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 11: { /* 0xb, execve */
            syscall_enter("execve", "spp");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                
                std::vector<std::string > argv = read_string_vector(arg(1));
                std::vector<std::string > envp = read_string_vector(arg(2));

                std::vector<char*> sys_argv;
                for (unsigned int i = 0; i < argv.size(); ++i)
                    sys_argv.push_back(&argv[i][0]);
                sys_argv.push_back(NULL);

                std::vector<char*> sys_envp;
                for (unsigned int i = 0; i < envp.size(); ++i)
                    sys_envp.push_back(&envp[i][0]);
                sys_envp.push_back(NULL);
                
                int result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
                ROSE_ASSERT(-1==result);
                writeGPR(x86_gpr_ax, -errno);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 12: { /* 0xc, chdir */
            syscall_enter("chdir", "s");
            do {
                uint32_t path = arg(0);
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                int result = chdir(sys_path.c_str());
                if (result == -1) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 13: { /*0xd, time */
            syscall_enter("time", "p");
            time_t result = time(NULL);
            if (arg(0)) {
                uint32_t t_le;
                SgAsmExecutableFileFormat::host_to_le(result, &t_le);
                size_t nwritten = map->write(&t_le, arg(0), 4);
                ROSE_ASSERT(4==nwritten);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("t");
            break;
        }

        case 14: { /*0xe, mknod*/
            syscall_enter("mknod", "sdd");
            do {
                uint32_t path_va = arg(0);
                int mode = arg(1);
                unsigned dev = arg(2);
                bool error;
                std::string path = read_string(path_va, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                int result = mknod(path.c_str(), mode, dev);
                writeGPR(x86_gpr_ax, -1==result ? -errno : result);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 15: { /* 0xf, chmod */
            syscall_enter("chmod", "sd");
            do {
                uint32_t filename = arg(0);
                bool error;
                std::string sys_filename = read_string(filename, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                mode_t mode = arg(1);
                int result = chmod(sys_filename.c_str(), mode);
                if (result == -1) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 19: { /* 0x13, lseek(int fd, off_t offset, int whence) */
            syscall_enter("lseek", "ddf", seek_whence);
            off_t result = lseek(arg(0), arg(1), arg(2));
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 20: { /*0x14, getpid*/
            syscall_enter("getpid", "");
            writeGPR(x86_gpr_ax, getpid());
            syscall_leave("d");
            break;
        }

        case 24: { /*0x18, getuid*/
            syscall_enter("getuid", "");
            writeGPR(x86_gpr_ax, getuid());
            syscall_leave("d");
            break;
        }

        case 27: { /* 0x1b, alarm */
            syscall_enter("alarm", "d");
            int result = alarm(arg(0));
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 30: { /* 0x1e, utime */

            /*
               int utime(const char *filename, const struct utimbuf *times);

               The utimbuf structure is:

               struct utimbuf {
               time_t actime;       // access time 
                   time_t modtime;  // modification time 
                 };

               The utime() system call changes the access and modification times of the inode
               specified by filename to the actime and modtime fields of times respectively.

               If times is NULL, then the access and modification times of the file are set
               to the current time.
            */
            syscall_enter("utime", "sp");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                //Check to see if times is NULL
                uint8_t byte;
                size_t nread = map->read(&byte, arg(1), 1);
                ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

                int result;
                if( byte) {
                    struct kernel_utimebuf {
                        uint32_t actime;
                        uint32_t modtime;
                    };

                    kernel_utimebuf ubuf;
                    size_t nread = map->read(&ubuf, arg(1), sizeof(kernel_utimebuf));
                    ROSE_ASSERT(nread == sizeof(kernel_utimebuf));

                    utimbuf ubuf64;
                    ubuf64.actime  = ubuf.actime;
                    ubuf64.modtime = ubuf.modtime;

                    result = utime(filename.c_str(), &ubuf64);

                } else {
                    result = utime(filename.c_str(), NULL);
                }
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        };

        case 33: { /*0x21, access*/
            static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
            syscall_enter("access", "sf", flags);
            do {
                uint32_t name_va=arg(0);
                bool error;
                std::string name = read_string(name_va, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                int mode=arg(1);
                int result = access(name.c_str(), mode);
                if (-1==result) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 37: { /* 0x25, kill */
            syscall_enter("kill", "df", signal_names);
            pid_t pid=arg(0);
            int sig=arg(1);
            int result = kill(pid, sig);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

	case 39: { /* 0x27, mkdir */
            syscall_enter("mkdir", "sd");
            do {
                uint32_t pathname = arg(0);
                bool error;
                std::string sys_pathname = read_string(pathname, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                mode_t mode = arg(1);

                int result = mkdir(sys_pathname.c_str(), mode);
                if (result == -1) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
	}

	case 40: { /* 0x28, rmdir */
            syscall_enter("rmdir", "s");
            do {
                uint32_t pathname = arg(0);
                bool error;
                std::string sys_pathname = read_string(pathname, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                int result = rmdir(sys_pathname.c_str());
                if (result == -1) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 41: { /*0x29, dup*/
            syscall_enter("dup", "d");
            uint32_t fd = arg(0);
            int result = dup(fd);
            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 42: { /*0x2a, pipe*/
            /*
               int pipe(int filedes[2]); 

               pipe() creates a pair of file descriptors, pointing to a pipe inode, and 
               places them in the array pointed to by filedes. filedes[0] is for reading, 
               filedes[1] is for writing. 

            */
            syscall_enter("pipe", "p");


            int32_t filedes_kernel[2];
            size_t  size_filedes = sizeof(int32_t)*2;


            int filedes[2];
            int result = pipe(filedes);

            filedes_kernel[0] = filedes[0];
            filedes_kernel[1] = filedes[1];

            map->write(filedes_kernel, arg(0), size_filedes);


            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 45: { /*0x2d, brk*/
            syscall_enter("brk", "x");
            uint32_t newbrk = arg(0);
            int retval = 0;

            if (newbrk >= 0xb0000000ul) {
                retval = -ENOMEM;
            } else {
                if (newbrk > brk_va) {
                    MemoryMap::MapElement melmt(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
                    melmt.set_name("[heap]");
                    map->insert(melmt);
                    brk_va = newbrk;
                } else if (newbrk>0 && newbrk<brk_va) {
                    map->erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
                    brk_va = newbrk;
                }
                retval= brk_va;
            }
            if (debug && newbrk!=0 && trace_mmap) {
                fprintf(debug, "  memory map after brk syscall:\n");
                map->dump(debug, "    ");
            }

            writeGPR(x86_gpr_ax, retval);
            syscall_leave("p");
            break;
        }

        case 47: { /*0x2f, getgid*/
            syscall_enter("getgid", "");
            writeGPR(x86_gpr_ax, getgid());
            syscall_leave("d");
            break;
        }

        case 49: { /*0x31, geteuid*/
            syscall_enter("geteuid", "");
            writeGPR(x86_gpr_ax, geteuid());
            syscall_leave("d");
            break;
        }

        case 50: { /*0x32, getegid*/
            syscall_enter("getegid", "");
            writeGPR(x86_gpr_ax, getegid());
            syscall_leave("d");
            break;
        }

        case 54: { /*0x36, ioctl*/
            static const Translate ioctl_cmd[] = {
                TE(TCGETS), TE(TCSETS), TE(TCSETSW), TE(TCGETA), TE(TIOCGPGRP), TE(TIOCSPGRP), TE(TIOCSWINSZ), TE(TIOCGWINSZ),
                T_END};
            syscall_enter("ioctl", "dfd", ioctl_cmd);
            
            int fd=arg(0);
            uint32_t cmd=arg(1), arg2=arg(2);
            int result = -ENOSYS;
            switch (cmd) {
                case TCGETS: { /* 0x00005401, tcgetattr*/
                    struct termios ti;
                    result = tcgetattr(fd, &ti);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        /* The Linux kernel and glibc have different definitions for termios, with very different sizes (39
                         * bytes vs 60) */                  
                        size_t nwritten = map->write(&ti, arg2, 39);
                        ROSE_ASSERT(39==nwritten);
                    }
                    break;
                }

                case TCSETS:  /* 0x,00005402,  tcsetattr */
                    /* TCSETS const struct termios *argp
                       Equivalent to
                          int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
                          tcsetattr(fd, TCSANOW, argp).
                       Set the current serial port settings. 
                    */
                case TCSETSW: { /* 0x00005403, tcsetattr  */
                    /* Equivalent to 
                         int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
                         tcsetattr(fd, TCSADRAIN, argp).
                       Allow the output buffer to drain, and set the current serial port settings. 

                       Value of second argument is the only difference between TCSETS and TCSETSW

                       typedef unsigned int     tcflag_t;
                       typedef unsigned char    cc_t;
                       typedef unsigned int     speed_t;

                       struct termios {
                         tcflag_t c_iflag;
                         tcflag_t c_oflag;
                         tcflag_t c_cflag;
                         tcflag_t c_lflag;
                         cc_t c_cc[NCCS];
                         speed_t c_ispeed;
                         speed_t c_ospeed;
                       };


                     */
                    uint32_t optional_actions = arg(2);

                    //Convert between 32 bit termios and whatever is
                    //used on this machine
                    struct termios_kernel {
                         uint32_t c_iflag;
                         uint32_t c_oflag;
                         uint32_t c_cflag;
                         uint32_t c_lflag;
                         unsigned char c_cc[NCCS];
                         uint32_t c_ispeed;
                         uint32_t c_ospeed;
                    };

                    termios_kernel tik;

                    size_t nread = map->read(&tik, arg(3), sizeof(termios_kernel));
                    ROSE_ASSERT(sizeof(termios_kernel) == nread);
 
                    struct termios ti;

                    CONV_FIELD(ti,tik,c_iflag);
                    CONV_FIELD(ti,tik,c_oflag);
                    CONV_FIELD(ti,tik,c_cflag);
                    CONV_FIELD(ti,tik,c_lflag);
                    CONV_FIELD(ti,tik,c_ispeed);

                    for (int i = 0 ; i < NCCS ; i++ )
                      ti.c_cc[i] = tik.c_cc[i];

                    CONV_FIELD(ti,tik,c_ospeed);

                    result = tcsetattr(fd, optional_actions, &ti);

                    if (result==-1) {
                        result = -errno;
                    } 
  
                    break;
                }

                case TCGETA: { /* 0x,00005405 */
                    /* gets a data structure of type 
                           struct termio * 

                       struct termio {
                         unsigned short c_iflag;     // input mode flags 
                         unsigned short c_oflag;     // output mode flags 
                         unsigned short c_cflag;     // control mode flags 
                         unsigned short c_lflag;     // local mode flags 
                         unsigned char c_line;       // line discipline 
                         unsigned char c_cc[NCC];    // control characters 
                       };

                     */

                    termio to;

                    result = ioctl(fd, TCGETA, &to);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = map->write(&to, arg2, sizeof to);
                        ROSE_ASSERT(nwritten==sizeof to);
                    }

                    break;
                }

                case TIOCGPGRP: { /* 0x0000540F, tcgetpgrp*/
                    /* equivalent to 
                        pid_t tcgetpgrp(int fd);
                       The  function tcgetpgrp() returns the process group ID of the foreground process group 
                       on the terminal associated to fd, which must be the controlling terminal of the calling 
                       process.
                    */

                    pid_t pgrp = tcgetpgrp(fd);
                    if (-1==pgrp) {
                        result = -errno;
                    } else {
                        uint32_t pgrp_le;
                        SgAsmExecutableFileFormat::host_to_le(pgrp, &pgrp_le);
                        size_t nwritten = map->write(&pgrp_le, arg2, 4);
                        ROSE_ASSERT(4==nwritten);
                        result = 0;
                    }
                    break;
                }
                    
                case TIOCSPGRP: { /* 0x5410, tcsetpgrp*/
                    uint32_t pgid_le;
                    size_t nread = map->read(&pgid_le, arg2, 4);
                    ROSE_ASSERT(4==nread);
                    pid_t pgid = SgAsmExecutableFileFormat::le_to_host(pgid_le);
                    result = tcsetpgrp(fd, pgid);
                    if (-1==result)
                        result = -errno;
                    break;
                }

                case TIOCSWINSZ: { /* 0x5413, the winsize is const */
                    /* Set window size.
                       struct winsize {
                          unsigned short ws_row;
                          unsigned short ws_col;
                          unsigned short ws_xpixel;   // unused 
                          unsigned short ws_ypixel;   // unused 
                       };
                    */
                    winsize ws;
                    size_t nread = map->read(&ws, arg(2), sizeof(winsize));
                    ROSE_ASSERT(sizeof(winsize) == nread);

                    result = ioctl(fd, TIOCSWINSZ, &ws);
                    if (-1==result)
                        result = -errno;
                    break;
                }

                case TIOCGWINSZ: /* 0x5414, */ {
                    struct winsize ws;
                    result = ioctl(fd, TIOCGWINSZ, &ws);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = map->write(&ws, arg2, sizeof ws);
                        ROSE_ASSERT(nwritten==sizeof ws);
                    }
                    break;
                }
                default:
                    fprintf(stderr, "  unhandled ioctl: %u\n", cmd);
                    abort();
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 57: { /*0x39, setpgid*/
            syscall_enter("setpgid", "dd");
            pid_t pid=arg(0), pgid=arg(1);
            int result = setpgid(pid, pgid);
            if (-1==result) { result = -errno; }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 60: { /* 0x3C, umask */
            /* mode_t umask(mode_t mask);

               umask() sets the calling process' file mode creation mask (umask) to mask & 0777.
 
               This system call always succeeds and the previous value of the mask is returned.
            */
            syscall_enter("umask", "d");
	    mode_t mode = arg(0);

	    int result = syscall(SYS_umask, mode); 
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	    }  

        case 63: { /* 0x3f, dup2 */
            syscall_enter("dup2", "dd");
            int result = dup2(arg(0), arg(1));
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            syscall_leave("d");
            break;
        }
            
        case 64: { /*0x40, getppid*/
            syscall_enter("getppid", "");
            writeGPR(x86_gpr_ax, getppid());
            syscall_leave("d");
            break;
        }

        case 65: { /*0x41, getpgrp*/
            syscall_enter("getpgrp", "");
            writeGPR(x86_gpr_ax, getpgrp());
            syscall_leave("d");
            break;
        }
        case 75: { /*0x4B, setrlimit */
            syscall_enter("setrlimit", "fP", rlimit_resources, 8, print_rlimit);
            int resource = arg(0);
            uint32_t rlimit_va = arg(1);
            uint32_t rlimit_guest[2];
            size_t nread = map->read(rlimit_guest, rlimit_va, sizeof rlimit_guest);
            ROSE_ASSERT(nread==sizeof rlimit_guest);
            struct rlimit rlimit_native;
            rlimit_native.rlim_cur = rlimit_guest[0];
            rlimit_native.rlim_max = rlimit_guest[1];
            int result = setrlimit(resource, &rlimit_native);
            writeGPR(x86_gpr_ax, -1==result ? -errno : result);
            syscall_leave("d");
            break;
        }

        case 191:
            syscall_enter("ugetrlimit", "fp", rlimit_resources);
            if (debug && trace_syscall)
                fputs("delegated to getrlimit (syscall 76); see next line\n", debug);
            /* fall through to 76; note that syscall trace will still show syscall 191 */
            
        case 76: {  /*0x4c, getrlimit*/
            syscall_enter("getrlimit", "fp", rlimit_resources);
            do {
                int resource = arg(0);
                uint32_t rlimit_va = arg(1);
                struct rlimit rlimit_native;
                int result = getrlimit(resource, &rlimit_native);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                uint32_t rlimit_guest[2];
                rlimit_guest[0] = rlimit_native.rlim_cur;
                rlimit_guest[1] = rlimit_native.rlim_max;
                if (8!=map->write(rlimit_guest, rlimit_va, 8)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d-P", 8, print_rlimit);
            break;
        }

        case 78: { /*0x4e, gettimeofday*/       
            syscall_enter("gettimeofday", "p");
            uint32_t tp = arg(0);
            struct timeval host_time;
            struct timeval_32 guest_time;

            int result = gettimeofday(&host_time, NULL);
            if (result == -1) {
                result = -errno;
            } else {
                guest_time.tv_sec = host_time.tv_sec;
                guest_time.tv_usec = host_time.tv_usec;
                if (sizeof(guest_time) != map->write(&guest_time, tp, sizeof guest_time))
                    result = -EFAULT;
            }

            writeGPR(x86_gpr_ax, result);
            syscall_leave("dP", sizeof guest_time, print_timeval_32);
            break;
        }

        case 83: { /*0x53, symlink*/
            syscall_enter("symlink", "ss");
            do {
                uint32_t oldpath=arg(0), newpath=arg(1);
                bool error;
                std::string sys_oldpath = read_string(oldpath, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                std::string sys_newpath = read_string(newpath, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                int result = symlink(sys_oldpath.c_str(),sys_newpath.c_str());
                if (result == -1) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 85: { /*0x55, readlink*/
            syscall_enter("readlink", "spd");
            do {
                uint32_t path=arg(0), buf_va=arg(1), bufsize=arg(2);
                char sys_buf[bufsize];
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                int result = readlink(sys_path.c_str(), sys_buf, bufsize);
                if (result == -1) {
                    result = -errno;
                } else {
                    size_t nwritten = map->write(sys_buf, buf_va, result);
                    ROSE_ASSERT(nwritten == (size_t)result);
                }
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }
            
        case 91: { /*0x5b, munmap*/
            syscall_enter("munmap", "pd");
            do {
                uint32_t va=arg(0);
                uint32_t sz=arg(1);
                uint32_t aligned_va = ALIGN_DN(va, PAGE_SIZE);
                uint32_t aligned_sz = ALIGN_UP(sz+va-aligned_va, PAGE_SIZE);
                void *rose_addr = my_addr(aligned_va, aligned_sz);

                /* Check ranges */
                if (aligned_va+aligned_sz <= aligned_va) { /* FIXME: not sure if sz==0 is an error */
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }

                /* Make sure that the specified memory range is actually mapped, or return -ENOMEM. */
                ExtentMap extents;
                extents.insert(ExtentPair(aligned_va, aligned_sz));
                extents.erase(map->va_extents());
                if (!extents.empty()) {
                    writeGPR(x86_gpr_ax, -ENOMEM);
                    break;
                }

                /* Erase the mapping from the simulation */
                map->erase(MemoryMap::MapElement(aligned_va, aligned_sz));

                /* Also unmap for real, because if we don't, and the mapping was not anonymous, and the file that was mapped is
                 * unlinked, and we're on NFS, an NFS temp file is created in place of the unlinked file. */
                if (rose_addr && ALIGN_UP((uint64_t)rose_addr, (uint64_t)PAGE_SIZE)==(uint64_t)rose_addr)
                    (void)munmap(rose_addr, aligned_sz);

                writeGPR(x86_gpr_ax, 0);
            } while (0);
            if (debug && trace_mmap) {
                fprintf(debug, " memory map after munmap syscall:\n");
                map->dump(debug, "    ");
            }
            syscall_leave("d");
            break;
        }

        case 93: { /* 0x5c, ftruncate */
            syscall_enter("ftruncate", "dd");
            int fd = arg(0);
            off_t len = arg(1);
            int result = ftruncate(fd, len);
            writeGPR(x86_gpr_ax, -1==result ? -errno : result);
            syscall_leave("d");
            break;
        }

        case 94: { /* 0x5d, fchmod */

            /*
                int fchmod(int fd, mode_t mode);

                fchmod() changes the permissions of the file referred to by the open file
                         descriptor fd.
            */
            syscall_enter("fchmod", "dd");
	        uint32_t fd = arg(0);
	        mode_t mode = arg(1);

	        int result = fchmod(fd, mode);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	    }

     	case 95: { /*0x5f, fchown */
            /* int fchown(int fd, uid_t owner, gid_t group);
             * typedef unsigned short  __kernel_old_uid_t;
             * typedef unsigned short  __kernel_old_gid_t;
             *
             * fchown() changes the ownership of the file referred to by the open file descriptor fd. */
            syscall_enter("fchown", "ddd");
            uint32_t fd = arg(0);
            int user = arg(1);
            int group = arg(2);
            int result = syscall(SYS_fchown, fd, user, group);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 99: { /* 0x63, statfs */
            syscall_enter("statfs", "sp");
            do {
                bool error;
                std::string path = read_string(arg(0), 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                statfs_32 guest_statfs;
                if (sizeof(guest_statfs)!=map->read(&guest_statfs, arg(1), sizeof guest_statfs)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                static statfs_64_native host_statfs;
                host_statfs.f_type = guest_statfs.f_type;
                host_statfs.f_bsize = guest_statfs.f_bsize;
                host_statfs.f_blocks = guest_statfs.f_blocks;
                host_statfs.f_bfree = guest_statfs.f_bfree;
                host_statfs.f_bavail = guest_statfs.f_bavail;
                host_statfs.f_files = guest_statfs.f_files;
                host_statfs.f_ffree = guest_statfs.f_ffree;
                host_statfs.f_fsid[0] = guest_statfs.f_fsid[0];
                host_statfs.f_fsid[1] = guest_statfs.f_fsid[1];
                host_statfs.f_namelen = guest_statfs.f_namelen;
                host_statfs.f_frsize = guest_statfs.f_frsize;
                host_statfs.f_flags = guest_statfs.f_flags;
                host_statfs.f_spare[0] = guest_statfs.f_spare[0];
                host_statfs.f_spare[1] = guest_statfs.f_spare[1];
                host_statfs.f_spare[2] = guest_statfs.f_spare[2];
                host_statfs.f_spare[3] = guest_statfs.f_spare[3];
#ifdef SYS_statfs64 /* host is 32-bit machine */
                int result = syscall(SYS_statfs64, path.c_str(), &host_statfs);
#else           /* host is 64-bit machine */
                int result = syscall(SYS_statfs, path.c_str(), &host_statfs);
#endif
                writeGPR(x86_gpr_ax, -1==result?-errno:result);
            } while (0);
            syscall_leave("d-P", sizeof(statfs_32), print_statfs_32);
            break;
        }

        case 102: { /* 0x66, socketcall */
            /* Return value is written to eax by these helper functions. The struction of this code closely follows that in the
             * Linux kernel. See linux/net/socket.c. */
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

            uint32_t a[6];
            switch (arg(0)) {
                case 1: { /* SYS_SOCKET */
                    if (12!=map->read(a, arg(1), 12)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "socket", "fff", protocol_families, socket_types, socket_protocols);
                    sys_socket(a[0], a[1], a[2]);
                    break;
                }
                    
                case 2: { /* SYS_BIND */
                    if (12!=map->read(a, arg(1), 12)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "bind", "dpd");    /* FIXME: we could do a better job printing the address [RPM 2011-01-04] */
                    sys_bind(a[0], a[1], a[2]);
                    break;
                }

                case 4: { /* SYS_LISTEN */
                    if (8!=map->read(a, arg(1), 8)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "listen", "dd");
                    sys_listen(a[0], a[1]);
                    break;
                }
                    
                case 3: /* SYS_CONNECT */
                case 5: /* SYS_ACCEPT */
                case 6: /* SYS_GETSOCKNAME */
                case 7: /* SYS_GETPEERNAME */
                case 8: /* SYS_SOCKETPAIR */
                case 9: /* SYS_SEND */
                case 10: /* SYS_RECV */
                case 11: /* SYS_SENDTO */
                case 12: /* SYS_RECVFROM */
                case 13: /* SYS_SHUTDOWN */
                case 14: /* SYS_SETSOCKOPT */
                case 15: /* SYS_GETSOCKOPT */
                case 16: /* SYS_SENDMSG */
                case 17: /* SYS_RECVMSG */
                case 18: /* SYS_ACCEPT4 */
                case 19: /* SYS_RECVMMSG */
                    writeGPR(x86_gpr_ax, -ENOSYS);
                    goto socketcall_error;
                default:
                    writeGPR(x86_gpr_ax, -EINVAL);
                    goto socketcall_error;
            }
            syscall_leave("d");
            break;

            socketcall_error:
            syscall_enter("socketcall", "fp", socketcall_commands);
            syscall_leave("d");
            break;
        }

        case 114: { /*0x72, wait4*/
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("wait4", "dpfp", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1), rusage_va=arg(3);
            int options=arg(2);
            int status;
            struct rusage rusage;
            int result = wait4(pid, &status, options, &rusage);
            if( result == -1) {
                result = -errno;
            } else {
                if (status_va != 0) {
                    size_t nwritten = map->write(&status, status_va, 4);
                    ROSE_ASSERT(nwritten == 4);
                }
                if (rusage_va != 0) {
                    struct rusage_32 {
                        uint32_t utime_sec;     /* user time used; seconds */
                        uint32_t utime_usec;    /* user time used; microseconds */
                        uint32_t stime_sec;     /* system time used; seconds */
                        uint32_t stime_usec;    /* system time used; microseconds */
                        uint32_t maxrss;        /* maximum resident set size */
                        uint32_t ixrss;         /* integral shared memory size */
                        uint32_t idrss;         /* integral unshared data size */
                        uint32_t isrss;         /* integral unshared stack size */
                        uint32_t minflt;        /* page reclaims */
                        uint32_t majflt;        /* page faults */
                        uint32_t nswap;         /* swaps */
                        uint32_t inblock;       /* block input operations */
                        uint32_t oublock;       /* block output operations */
                        uint32_t msgsnd;        /* messages sent */
                        uint32_t msgrcv;        /* messages received */
                        uint32_t nsignals;      /* signals received */
                        uint32_t nvcsw;         /* voluntary context switches */
                        uint32_t nivcsw;        /* involuntary " */
                    } __attribute__((packed));
                    struct rusage_32 out;
                    ROSE_ASSERT(18*4==sizeof(out));
                    out.utime_sec = rusage.ru_utime.tv_sec;
                    out.utime_usec = rusage.ru_utime.tv_usec;
                    out.stime_sec = rusage.ru_stime.tv_sec;
                    out.stime_usec = rusage.ru_stime.tv_usec;
                    out.maxrss = rusage.ru_maxrss;
                    out.ixrss = rusage.ru_ixrss;
                    out.idrss = rusage.ru_idrss;
                    out.isrss = rusage.ru_isrss;
                    out.minflt = rusage.ru_minflt;
                    out.majflt = rusage.ru_majflt;
                    out.nswap = rusage.ru_nswap;
                    out.inblock = rusage.ru_inblock;
                    out.msgsnd = rusage.ru_msgsnd;
                    out.msgrcv = rusage.ru_msgrcv;
                    out.nsignals = rusage.ru_nsignals;
                    out.nvcsw = rusage.ru_nvcsw;
                    out.nivcsw = rusage.ru_nivcsw;
                    size_t nwritten = map->write(&out, rusage_va, sizeof out);
                    ROSE_ASSERT(nwritten == sizeof out);
                }
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 116: { /* 0x74, sysinfo*/
            syscall_enter("sysinfo", "p");

            static const size_t guest_extra = 20 - 2*sizeof(uint32_t) - sizeof(int32_t);
            static const size_t host_extra  = 20 - 2*sizeof(long)     - sizeof(int);

            struct guest_sysinfo {      /* Sysinfo to be written into the specimen's memory */
                int32_t uptime;         /* Seconds since boot */
                uint32_t loads[3];      /* 1, 5, and 15 minute load averages */
                uint32_t totalram;      /* Total usable main memory size */
                uint32_t freeram;       /* Available memory size */
                uint32_t sharedram;     /* Amount of shared memory */
                uint32_t bufferram;     /* Memory used by buffers */
                uint32_t totalswap;     /* Total swap space size */
                uint32_t freeswap;      /* swap space still available */
                uint16_t procs;         /* Number of current processes */
                uint16_t pad;           /* explicit padding for m68k */
                uint32_t totalhigh;     /* Total high memory size */
                uint32_t freehigh;      /* Available high memory size */
                uint32_t mem_unit;      /* Memory unit size in bytes */
                char _f[guest_extra];   /* Padding for libc5 */
            } __attribute__((__packed__));

            struct host_sysinfo {
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
                char _f[host_extra];
            };

            host_sysinfo host_sys;
            int result  = syscall(SYS_sysinfo, &host_sys);

            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                guest_sysinfo guest_sys;
                guest_sys.uptime = host_sys.uptime;
                for(int i = 0 ; i < 3 ; i++)
                    guest_sys.loads[i] = host_sys.loads[i];
                guest_sys.totalram      = host_sys.totalram;
                guest_sys.freeram       = host_sys.freeram;
                guest_sys.sharedram     = host_sys.sharedram;
                guest_sys.bufferram     = host_sys.bufferram;
                guest_sys.totalswap     = host_sys.totalswap;
                guest_sys.freeswap      = host_sys.freeswap;
                guest_sys.procs         = host_sys.procs;
                guest_sys.pad           = host_sys.pad;
                guest_sys.totalhigh     = host_sys.totalhigh;
                guest_sys.mem_unit      = host_sys.mem_unit;
                memset(guest_sys._f, 0, sizeof(guest_sys._f));
                memcpy(guest_sys._f, host_sys._f, std::min(guest_extra, host_extra));

                size_t nwritten = map->write(&guest_sys, arg(0), sizeof(guest_sys));
                if (nwritten!=sizeof(guest_sys)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                } else {
                    writeGPR(x86_gpr_ax, result);
                }
            }
            syscall_leave("d");

            break;
        };

        case 117: { /* 0x75, ipc */
            /* Return value is written to eax by these helper functions. The structure of this code closely follows that in the
             * Linux kernel. */
            unsigned call = arg(0) & 0xffff;
            int version = arg(0) >> 16;
            uint32_t first=arg(1), second=arg(2), third=arg(3), ptr=arg(4), fifth=arg(5);
            switch (call) {
                case 1: /* SEMOP */
                    syscall_enter("ipc", "fdd-p", ipc_commands);
                    sys_semtimedop(first, ptr, second, 0);
                    syscall_leave("d");
                    break;
                case 2: /* SEMGET */
                    syscall_enter("ipc", "fddf", ipc_commands, ipc_flags);
                    sys_semget(first, second, third);
                    syscall_leave("d");
                    break;
                case 3: /* SEMCTL */
                    syscall_enter("ipc", "fdddpd", ipc_commands);
                    writeGPR(x86_gpr_ax, -ENOSYS); /* FIXME */
                    syscall_leave("d");
                    break;
                case 4: /* SEMTIMEDOP */
                    syscall_enter("ipc", "fdd-pP", ipc_commands, sizeof(timespec_32), print_timespec_32);
                    sys_semtimedop(first, ptr, second, fifth);
                    syscall_leave("d");
                    break;
                case 11: /* MSGSND */
                    syscall_enter("ipc", "fddfb", ipc_commands, ipc_flags, (size_t)(4+arg(2)));
                    sys_msgsnd(first, ptr, second, third);
                    syscall_leave("d");
                    break;
                case 12: /* MSGRCV */
                    if (0==version) {
                        syscall_enter("ipc", "fddfP", ipc_commands, ipc_flags, sizeof(ipc_kludge_32), print_ipc_kludge_32);
                        ipc_kludge_32 kludge;
                        if (8!=map->read(&kludge, arg(4), 8)) {
                            writeGPR(x86_gpr_ax, -ENOSYS);
                        } else {
                            sys_msgrcv(first, kludge.msgp, second, kludge.msgtyp, third);
                        }
                    } else {
                        syscall_enter("ipc", "fddfpd", ipc_commands, ipc_flags);
                        sys_msgrcv(first, ptr, second, fifth, third);
                    }
                    syscall_leave("d");
                    break;
                case 13: /* MSGGET */ {
                    syscall_enter("ipc", "fpf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
                    sys_msgget(first, second);
                    syscall_leave("d");
                    break;
                }
                case 14: /* MSGCTL */ {
                    bool set = (IPC_RMID==(second & ~0x0100) || IPC_SET==(second & ~0x0100));
                    if (set) {
                        syscall_enter("ipc", "fdf-P", ipc_commands, ipc_control, sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    } else {
                        syscall_enter("ipc", "fdf-p", ipc_commands, ipc_control);
                    }
                    sys_msgctl(first, second, ptr);
                    if (set) {
                        syscall_leave("d");
                    } else {
                        syscall_leave("d----P", sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    }
                    break;
                }
                case 21: /* SHMAT */
                    if (1==version) {
                        /* This was the entry point for kernel-originating calls from iBCS2 in 2.2 days */
                        syscall_enter("ipc", "fdddpd", ipc_commands);
                        writeGPR(x86_gpr_ax, -EINVAL);
                        syscall_leave("d");
                    } else {
                        syscall_enter("ipc", "fdfpp", ipc_commands, ipc_flags);
                        sys_shmat(first, second, third, ptr);
                        syscall_leave("p");
                        if (debug && trace_mmap) {
                            fprintf(debug, "  memory map after shmat:\n");
                            map->dump(debug, "    ");
                        }
                    }
                    break;
                case 22: /* SHMDT */
                    syscall_enter("ipc", "f---p", ipc_commands);
                    sys_shmdt(ptr);
                    syscall_leave("d");
                    if (debug && trace_mmap) {
                        fprintf(debug, "  memory map after shmdt:\n");
                        map->dump(debug, "    ");
                    }
                    break;
                case 23: /* SHMGET */
                    syscall_enter("ipc", "fpdf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
                    sys_shmget(first, second, third);
                    syscall_leave("d");
                    break;
                case 24: { /* SHMCTL */
                    bool set = (IPC_RMID==(second & ~0x0100) || IPC_SET==(second & ~0x0100));
                    if (set) {
                        syscall_enter("ipc", "fdf-P", ipc_commands, ipc_control, sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    } else {
                        syscall_enter("ipc", "fdf-p", ipc_commands, ipc_control);
                    }
                    sys_shmctl(first, second, ptr);
                    if (set) {
                        syscall_leave("d");
                    } else if (SHM_INFO==(second & ~0x0100)) {
                        syscall_leave("d----P", sizeof(shm_info_32), print_shm_info_32);
                    } else {
                        syscall_leave("d----P", sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    }
                    break;
                }
                default:
                    syscall_enter("ipc", "fdddpd", ipc_commands);
                    writeGPR(x86_gpr_ax, -ENOSYS);
                    syscall_leave("d");
                    break;
            }
            break;
        }

        case 120: { /* 0x78, clone */
            /* From linux arch/x86/kernel/process.c:
             *    long sys_clone(unsigned long clone_flags, unsigned long newsp,
             *                   void __user *parent_tid, void __user *child_tid, struct pt_regs *regs)
             */
            syscall_enter("clone", "fpppp", clone_flags);
            do {
                unsigned flags = arg(0);
                unsigned newsp = arg(1);
                unsigned parent_tid_va = arg(2);
                unsigned child_tid_va = arg(3);
                unsigned regs_va = arg(4);
                
                /* We cannot handle multiple threads yet. */
                if (newsp || parent_tid_va || child_tid_va || (flags & (CLONE_VM|CLONE_THREAD))) {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }

                /* ROSE simulates signal handling, therefore signal handlers cannot be shared. */
                if (flags & CLONE_SIGHAND) {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }

                /* We cannot use clone() because it's a wrapper around the clone system call and we'd need to provide a
                 * function for it to execute. We want fork-like semantics. */
                pid_t pid = fork();
                if (-1==pid) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }
                did_fork = true;

                /* Return register values in child */
                if (0==pid) {
                    pt_regs_32 regs;
                    regs.bx = readGPR(x86_gpr_bx).known_value();
                    regs.cx = readGPR(x86_gpr_cx).known_value();
                    regs.dx = readGPR(x86_gpr_dx).known_value();
                    regs.si = readGPR(x86_gpr_si).known_value();
                    regs.di = readGPR(x86_gpr_di).known_value();
                    regs.bp = readGPR(x86_gpr_bp).known_value();
                    regs.sp = readGPR(x86_gpr_sp).known_value();
                    regs.cs = readSegreg(x86_segreg_cs).known_value();
                    regs.ds = readSegreg(x86_segreg_ds).known_value();
                    regs.es = readSegreg(x86_segreg_es).known_value();
                    regs.fs = readSegreg(x86_segreg_fs).known_value();
                    regs.gs = readSegreg(x86_segreg_gs).known_value();
                    regs.ss = readSegreg(x86_segreg_ss).known_value();
                    uint32_t flags = 0;
                    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
                        if (readFlag((X86Flag)i).known_value()) {
                            flags |= (1u<<i);
                        }
                    }
                    if (sizeof(regs)!=map->write(&regs, regs_va, sizeof regs)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                }

                writeGPR(x86_gpr_ax, pid);
            } while (0);

            if (readGPR(x86_gpr_ax).known_value()) {
                syscall_leave("d");
            } else {
                /* Child */
                syscall_enter("child's clone", "fpppp");
                syscall_leave("d----P", sizeof(pt_regs_32), print_pt_regs_32);
            }
            break;
        }

        case 122: { /*0x7a, uname*/
            syscall_enter("uname", "p");
            uint32_t dest_va=arg(0);
            char buf[6*65];
            memset(buf, ' ', sizeof buf);
            strcpy(buf+0*65, "Linux");                                  /*sysname*/
            strcpy(buf+1*65, "mymachine.example.com");                  /*nodename*/
            strcpy(buf+2*65, "2.6.9");                                  /*release*/
            strcpy(buf+3*65, "#1 SMP Wed Jun 18 12:35:02 EDT 2008");    /*version*/
            strcpy(buf+4*65, "i386");                                   /*machine*/
            strcpy(buf+5*65, "example.com");                            /*domainname*/
            size_t nwritten = map->write(buf, dest_va, sizeof buf);
            if( nwritten <= 0 ) {
              writeGPR(x86_gpr_ax, -EFAULT);
              break;
            }

            ROSE_ASSERT(nwritten==sizeof buf);
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

	case 133: { /* 0x85, fchdir */
            syscall_enter("fchdir", "d");
	    uint32_t file_descriptor = arg(0);

	    int result = fchdir(file_descriptor);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

        case 125: { /*0x7d, mprotect*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            syscall_enter("mprotect", "pdf", pflags);
            uint32_t va=arg(0), size=arg(1), perms=arg(2);
            unsigned rose_perms = ((perms & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((perms & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((perms & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            if (va % PAGE_SIZE) {
                writeGPR(x86_gpr_ax, -EINVAL);
            } else {
                uint32_t aligned_sz = ALIGN_UP(size, PAGE_SIZE);

                /* Set protection in the underlying real memory (to catch things like trying to add write permission to memory
                 * that's mapped from a read-only file), then also set the protection in the simulated memory map so the simulator
                 * can make queries about memory access.  Some of the underlying memory points to parts of an ELF file that was
                 * read into ROSE's memory in such a way that segments are not aligned on page boundaries. We cannot change
                 * protections on these non-aligned sections. */
                if (-1==mprotect(my_addr(va, size), size, perms) && EINVAL!=errno) {
                    writeGPR(x86_gpr_ax, -errno);
                } else {
                    try {
                        map->mprotect(MemoryMap::MapElement(va, aligned_sz, rose_perms));
                        writeGPR(x86_gpr_ax, 0);
                    } catch (const MemoryMap::NotMapped &e) {
                        writeGPR(x86_gpr_ax, -ENOMEM);
                    }
                }
            }

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mprotect syscall:\n");
                map->dump(debug, "    ");
            }
            break;
        }

       case 140: { /* 0x8c, llseek */
            /* From the linux kernel, arguments are:
             *      unsigned int fd,                // file descriptor
             *      unsigned long offset_high,      // high 32 bits of 64-bit offset
             *      unsigned long offset_low,       // low 32 bits of 64-bit offset
             *      loff_t __user *result,          // 64-bit user area to write resulting position
             *      unsigned int origin             // whence specified offset is measured
             */
            syscall_enter("llseek","dddpf", seek_whence);
            int fd = arg(0);
            off64_t offset = ((off64_t)arg(1) << 32) | arg(2);
            uint32_t result_va = arg(3);
            int whence = arg(4);

            off64_t result = lseek64(fd, offset, whence);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                writeGPR(x86_gpr_ax, 0);
                size_t nwritten = map->write(&result, result_va, sizeof result);
                ROSE_ASSERT(nwritten==sizeof result);
            }
            syscall_leave("d");
            break;
        };
 	case 141: {     /* 0xdc, getdents(int fd, struct linux_dirent*, unsigned int count) */
            syscall_enter("getdents", "dpd");
            int fd = arg(0), sz = arg(2);
            uint32_t dirent_va = arg(1);
            int status = getdents_syscall<dirent32_t>(fd, dirent_va, sz);
            writeGPR(x86_gpr_ax, status);
            syscall_leave("d-P", status>0?status:0, print_dentries_32);
            break;
        }

        case 142: { /*0x8e , select */
            /* From the Linux kernel (fs/select.c):
             *    SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
             *                    fd_set __user *, exp, struct timeval __user *, tvp)
             * where:
             *    fd_set is enough "unsigned long" data to contain 1024 bits. Regardless of the size of "unsigned long",
             *    the file bits will be in the same order (we are the host is little endian), and the fd_set is the same size. */
            syscall_enter("select", "dPPPP",
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(timeval_32), print_timeval_32);
            do {
                int fd = arg(0);
                uint32_t in_va=arg(1), out_va=arg(2), ex_va=arg(3), tv_va=arg(4);

                fd_set in, out, ex;
                fd_set *inp=NULL, *outp=NULL, *exp=NULL;

                ROSE_ASSERT(128==sizeof(fd_set)); /* 128 bytes = 1024 file descriptor bits */
                if (in_va && sizeof(in)==map->read(&in, in_va, sizeof in))
                    inp = &in;
                if (out_va && sizeof(out)==map->read(&out, out_va, sizeof out))
                    outp = &out;
                if (ex_va && sizeof(ex)==map->read(&ex, ex_va, sizeof ex))
                    exp = &ex;

                timeval_32 guest_timeout;
                timeval host_timeout, *tvp=NULL;
                if (tv_va) {
                    if (sizeof(guest_timeout)!=map->read(&guest_timeout, tv_va, sizeof guest_timeout)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    } else {
                        host_timeout.tv_sec = guest_timeout.tv_sec;
                        host_timeout.tv_usec = guest_timeout.tv_usec;
                        tvp = &host_timeout;
                    }
                }

                int result = select(fd, inp, outp, exp, tvp);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                if ((in_va  && sizeof(in) !=map->write(inp,  in_va,  sizeof in))  ||
                    (out_va && sizeof(out)!=map->write(outp, out_va, sizeof out)) ||
                    (ex_va  && sizeof(ex) !=map->write(exp,  ex_va,  sizeof ex))) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                if (tvp) {
                    guest_timeout.tv_sec = tvp->tv_sec;
                    guest_timeout.tv_usec = tvp->tv_usec;
                    if (sizeof(guest_timeout)!=map->write(&guest_timeout, tv_va, sizeof guest_timeout)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                }
            } while (0);
            syscall_leave("d-PPPP",
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(timeval_32), print_timeval_32);
            break;
        }

        case 144: { /* 0x90, int msync(void *addr, size_t length, int flags) */
            static const Translate msync_flags[] = { TF(MS_ASYNC), TF(MS_SYNC), TF(MS_INVALIDATE), T_END };
            syscall_enter("msync", "pdf", msync_flags);
            void *addr = my_addr(arg(0), arg(1));
            if (!addr) {
                writeGPR(x86_gpr_ax, -ENOMEM);
            } else {
                int result = msync(addr, arg(1), arg(2));
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                } else {
                    writeGPR(x86_gpr_ax, result);
                }
            }
            syscall_leave("d");
            break;
        }

        case 146: { /*0x92, writev*/
            syscall_enter("writev", "dpd");
            uint32_t fd=arg(0), iov_va=arg(1);
            int niov=arg(2), idx=0;
            uint32_t retval = 0;
            if (niov<0 || niov>1024) {
                retval = -EINVAL;
            } else {
                for (idx=0; idx<niov; idx++) {
                    /* Obtain buffer address and size */
                    uint32_t buf_va;
                    if (4 != map->read(&buf_va, iov_va+idx*8+0, 4)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (debug && trace_syscall)
                            fprintf(debug, "    #%d: segmentation fault reading address\n", idx);
                        break;
                    }

                    uint32_t buf_sz;
                    if (4 != map->read(&buf_sz, iov_va+idx*8+4, 4)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (debug && trace_syscall)
                            fprintf(debug, "    #%d: segmentation fault reading size\n", idx);
                        break;
                    }

                    if (debug && trace_syscall) {
                        if (0==idx) fprintf(debug, "<see below>\n"); /* return value is delayed */
                        fprintf(debug, "    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32, idx, buf_va, buf_sz);
                    }

                    /* Make sure total size doesn't overflow a ssize_t */
                    if ((buf_sz & 0x80000000) || (retval+buf_sz) & 0x80000000) {
                        if (0==idx)
                            retval = -EINVAL;
                        if (debug && trace_syscall)
                            fprintf(debug, " size overflow\n");
                        break;
                    }

                    /* Copy data from guest to host because guest memory might not be contiguous in the host. Perhaps a more
                     * efficient way to do this would be to copy chunks of host-contiguous data in a loop instead. */
                    uint8_t buf[buf_sz];
                    if (buf_sz != map->read(buf, buf_va, buf_sz)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (debug && trace_syscall)
                            fprintf(debug, " segmentation fault\n");
                        break;
                    }

                    /* Write data to the file */
                    ssize_t nwritten = write(fd, buf, buf_sz);
                    if (-1==nwritten) {
                        if (0==idx)
                            retval = -errno;
                        if (debug && trace_syscall)
                            fprintf(debug, " write failed (%s)\n", strerror(errno));
                        break;
                    }
                    retval += nwritten;
                    if ((uint32_t)nwritten<buf_sz) {
                        if (debug && trace_syscall)
                            fprintf(debug, " short write (%zd bytes)\n", nwritten);
                        break;
                    }
                    if (debug && trace_syscall)
                        fputc('\n', debug);
                }
            }
            writeGPR(x86_gpr_ax, retval);
            if (debug && trace_syscall && niov>0 && niov<=1024)
                fprintf(debug, "%*s = ", 51, ""); /* align for return value */
            syscall_leave("d");
            break;
        }

        case 162: { /* 0xa2, nanosleep */
            syscall_enter("nanosleep", "Pp", sizeof(timespec_32), print_timespec_32);
            do {
                timespec_32 guest_ts;
                timespec host_ts_in, host_ts_out;
                if (sizeof(guest_ts)!=map->read(&guest_ts, arg(0), sizeof guest_ts)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                host_ts_in.tv_sec = guest_ts.tv_sec;
                host_ts_in.tv_nsec = guest_ts.tv_nsec;
                int result = nanosleep(&host_ts_in, &host_ts_out);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }
                if (arg(1)) {
                    guest_ts.tv_sec = host_ts_out.tv_sec;
                    guest_ts.tv_nsec = host_ts_out.tv_nsec;
                    if (sizeof(guest_ts)!=map->write(&guest_ts, arg(1), sizeof guest_ts)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                }
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 174: { /*0xae, rt_sigaction*/
            syscall_enter("rt_sigaction", "fPpd", signal_names, sizeof(sigaction_32), print_sigaction_32);
            do {
                int signum=arg(0);
                uint32_t action_va=arg(1), oldact_va=arg(2);
                size_t sigsetsize=arg(3);

                if (sigsetsize!=8 || signum<1 || signum>_NSIG) {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }


                sigaction_32 tmp;
                if (action_va && sizeof(tmp) != map->read(&tmp, action_va, sizeof tmp)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                if (oldact_va && sizeof(tmp) != map->write(signal_action+signum, oldact_va, sizeof tmp)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                signal_action[signum] = tmp;
                writeGPR(x86_gpr_ax, 0);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 175: { /*0xaf, rt_sigprocmask*/
            static const Translate flags[] = { TF(SIG_BLOCK), TF(SIG_UNBLOCK), TF(SIG_SETMASK), T_END };
            syscall_enter("rt_sigprocmask", "fPp", flags, (size_t)8, print_sigmask);

            int how=arg(0);
            uint32_t set_va=arg(1), get_va=arg(2);
            //size_t sigsetsize=arg(3);

            uint64_t saved=signal_mask, sigset=0;
            if ( set_va != 0 ) {

                size_t nread = map->read(&sigset, set_va, sizeof sigset);
                ROSE_ASSERT(nread==sizeof sigset);

                if (0==how) {
                    /* SIG_BLOCK */
                    signal_mask |= sigset;
                } else if (1==how) {
                    /* SIG_UNBLOCK */
                    signal_mask &= ~sigset;
                } else if (2==how) {
                    /* SIG_SETMASK */
                    signal_mask = sigset;
                } else {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }
                if (signal_mask!=saved)
                    signal_reprocess = true;
            }

            if (get_va) {
                size_t nwritten = map->write(&saved, get_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof saved);
            }
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d--P", (size_t)8, print_sigmask);
            break;
        }

        case 176: { /* 0xb0, rt_sigpending */
            syscall_enter("rt_sigpending", "p");
            uint32_t sigset_va=arg(0);
            uint64_t pending=0;
            std::queue<int> tmp;
            while (!signal_queue.empty()) {
                int signo = signal_queue.front();
                signal_queue.pop();
                tmp.push(signo);
                pending |= (1 << (signo - 1));
            }
            signal_queue = tmp;
            if (8!=map->write(&pending, sigset_va, 8)) {
                writeGPR(x86_gpr_ax, -EFAULT);
            } else {
                writeGPR(x86_gpr_ax, 0);
            }
            syscall_leave("dP", sizeof(uint64_t), print_sigmask);
            break;
        }

	case 183: { /* 0xb7, getcwd */
            syscall_enter("getcwd", "pd");
            do {
                static char buf[4096]; /* page size in kernel */
                int result = syscall(SYS_getcwd, buf, sizeof buf);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                size_t len = strlen(buf) + 1;
                if (len > arg(1)) {
                    writeGPR(x86_gpr_ax, -ERANGE);
                    break;
                }

                if (len!=map->write(buf, arg(0), len)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("ds");
            break;
        }

        case 186: { /* 0xba, sigaltstack*/
            syscall_enter("sigaltstack", "Pp", sizeof(stack_32), print_stack_32);
            do {
                uint32_t new_stack_va=arg(0), old_stack_va=arg(1);

                /* Are we currently executing on the alternate stack? */
                uint32_t sp = readGPR(x86_gpr_sp).known_value();
                bool on_stack = (0==(signal_stack.ss_flags & SS_DISABLE) &&
                                 sp >= signal_stack.ss_sp &&
                                 sp < signal_stack.ss_sp + signal_stack.ss_size);

                if (old_stack_va) {
                    stack_32 tmp = signal_stack;
                    tmp.ss_flags &= ~SS_ONSTACK;
                    if (on_stack) tmp.ss_flags |= SS_ONSTACK;
                    if (sizeof(tmp)!=map->write(&tmp, old_stack_va, sizeof tmp)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                }

                if (new_stack_va) {
                    stack_32 tmp;
                    tmp.ss_flags &= ~SS_ONSTACK;
                    if (sizeof(tmp)!=map->read(&tmp, new_stack_va, sizeof tmp)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                    if (on_stack) {
                        writeGPR(x86_gpr_ax, -EINVAL);  /* can't set alt stack while we're using it */
                        break;
                    } else if ((tmp.ss_flags & ~(SS_DISABLE|SS_ONSTACK))) {
                        writeGPR(x86_gpr_ax, -EINVAL);  /* invalid flags */
                        break;
                    } else if (0==(tmp.ss_flags & SS_DISABLE) && tmp.ss_size < 4096) {
                        writeGPR(x86_gpr_ax, -ENOMEM);  /* stack must be at least one page large */
                        break;
                    }
                    signal_stack = tmp;
                }

                writeGPR(x86_gpr_ax, 0);
            } while (0);
            syscall_leave("d-P", sizeof(stack_32), print_stack_32);
            break;
        }

        // case 191 (0xbf, ugetrlimit). See case 76. I think they're the same. [RPM 2010-11-12]

        case 192: { /*0xc0, mmap2*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            static const Translate mflags[] = { TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_ANONYMOUS), TF(MAP_DENYWRITE),
                                                TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), TF(MAP_GROWSDOWN),
                                                TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE),
#ifdef MAP_32BIT
                                                TF(MAP_32BIT),
#endif
                                                TF(MAP_POPULATE), T_END };
            syscall_enter("mmap2", "pdffdd", pflags, mflags);
            uint32_t start=arg(0), size=arg(1), prot=arg(2), flags=arg(3), offset=arg(5)*PAGE_SIZE;
            int fd=arg(4);
            size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
            void *buf = NULL;
            unsigned rose_perms = ((prot & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((prot & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((prot & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            //prot |= PROT_READ | PROT_WRITE | PROT_EXEC; /* ROSE takes care of permissions checking */

            if (!start) {
                try {
                    start = map->find_free(mmap_start, aligned_size, PAGE_SIZE);
                } catch (const MemoryMap::NoFreeSpace &e) {
                    writeGPR(x86_gpr_ax, -ENOMEM);
                    goto mmap2_done;
                }
            }
            if (!mmap_recycle)
                mmap_start = std::max(mmap_start, start);

            if (flags & MAP_ANONYMOUS) {
                buf = mmap(NULL, size, prot, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            } else {
                buf = mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, offset);
            }
            if (MAP_FAILED==buf) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                /* Try to figure out a reasonable name for the map element. If we're mapping a file, we can get the file name
                 * from the proc filesystem. The name is only used to aid debugging. */
                std::string melmt_name = "anonymous";
                if (fd>=0 && 0==(flags & MAP_ANONYMOUS)) {
                    char fd_namebuf[4096];
                    ssize_t nread = readlink(("/proc/self/fd/"+StringUtility::numberToString(fd)).c_str(),
                                             fd_namebuf, sizeof(fd_namebuf)-1);
                    if (nread>45) {
                        fd_namebuf[nread] = '\0';
                        char *slash = strrchr(fd_namebuf, '/');
                        melmt_name = slash ? slash+1 : fd_namebuf;
                    } else if (nread>0) {
                        fd_namebuf[nread] = '\0';
                        melmt_name = fd_namebuf;
                    } else {
                        melmt_name = "fd=" + StringUtility::numberToString(fd);
                    }
                }

                MemoryMap::MapElement melmt(start, aligned_size, buf, 0, rose_perms);
                melmt.set_name("mmap2("+melmt_name+")");
                map->erase(melmt); /*clear space space first to avoid MemoryMap::Inconsistent exception*/
                map->insert(melmt);
                writeGPR(x86_gpr_ax, start);
            }

        mmap2_done:
            syscall_leave("p");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mmap2 syscall:\n");
                map->dump(debug, "    ");
            }

            break;
        }

        case 195:       /*0xc3, stat64*/
        case 196:       /*0xc4, lstat64*/
        case 197: {     /*0xc5, fstat64*/
            /* We need to be a bit careful with xstat64 calls. The C library invokes one of the xstat64 system calls, which
             * writes a kernel data structure into a temporary buffer, and which the C library then massages into a struct
             * stat64. When simulating, we don't want the C library to monkey with the data returned from the system call
             * because the simulated C library will do the monkeying (it must only happen once).
             *
             * Therefore, we will invoke the system call directly, bypassing the C library, and then copy the result into
             * specimen memory. If the syscall is made on an amd64 host we need to convert it to an i386 host.
             *
             * For some unknown reason, if we invoke the system call with buf allocated on the stack we'll get -EFAULT (-14)
             * as the result; if we allocate it statically there's no problem.  Also, just in case the size is different than
             * we think, we'll allocate a guard area above the kernel_stat and check that the syscall didn't write into it. */
            if (195==callno || 196==callno) {
                syscall_enter(195==callno?"stat64":"lstat64", "sp");
            } else {
                syscall_enter("fstat64", "dp");
            }

            do {
                ROSE_ASSERT(96==sizeof(kernel_stat_32));
                ROSE_ASSERT(144==sizeof(kernel_stat_64));
#ifdef SYS_stat64       /* x86sim must be running on i386 */
                ROSE_ASSERT(4==sizeof(long));
                int host_callno = 195==callno ? SYS_stat64 : (196==callno ? SYS_lstat64 : SYS_fstat64);
                static const size_t kernel_stat_size = sizeof(kernel_stat_32);
#else                   /* x86sim must be running on amd64 */
                ROSE_ASSERT(8==sizeof(long));
                int host_callno = 195==callno ? SYS_stat : (196==callno ? SYS_lstat : SYS_fstat);
                static const size_t kernel_stat_size = sizeof(kernel_stat_64);
#endif

                static uint8_t kernel_stat[kernel_stat_size+100];
                memset(kernel_stat, 0xff, sizeof kernel_stat);
                int result = 0xdeadbeef;

            /* Make the system call without going through the C library. Well, we go through syscall(), but nothing else. */
                if (195==callno || 196==callno) {
                    bool error;
                    std::string name = read_string(arg(0), 0, &error);
                    if (error) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                    result = syscall(host_callno, (unsigned long)name.c_str(), (unsigned long)kernel_stat);
                } else {
                    result = syscall(host_callno, (unsigned long)arg(0), (unsigned long)kernel_stat);
                }
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                /* Check for overflow */
                for (size_t i=kernel_stat_size; i<sizeof kernel_stat; i++)
                    ROSE_ASSERT(0xff==kernel_stat[i]);

                /* Check for underflow.  Check that the kernel initialized as much data as we thought it should.  We
                 * initialized the kernel_stat to all 0xff bytes before making the system call.  The last data member of
                 * kernel_stat is either an 8-byte inode (i386) or zero (amd64), which in either case the high order byte is
                 * almost certainly not 0xff. */
                ROSE_ASSERT(0xff!=kernel_stat[kernel_stat_size-1]);

                /* On amd64 we need to translate the 64-bit struct that we got back from the host kernel to the 32-bit struct
                 * that the specimen should get back from the guest kernel. */           
                if (sizeof(kernel_stat_64)==kernel_stat_size) {
                    if (debug && trace_syscall)
                        fprintf(debug, "[64-to-32] ");
                    kernel_stat_64 *in = (kernel_stat_64*)kernel_stat;
                    kernel_stat_32 out;
                    out.dev = in->dev;
                    out.pad_1 = (uint32_t)(-1);
                    out.ino_lo = in->ino;
                    out.mode = in->mode;
                    out.nlink = in->nlink;
                    out.user = in->user;
                    out.group = in->group;
                    out.rdev = in->rdev;
                    out.pad_2 = (uint32_t)(-1);
                    out.size = in->size;
                    out.blksize = in->blksize;
                    out.nblocks = in->nblocks;
                    out.atim_sec = in->atim_sec;
                    out.atim_nsec = in->atim_nsec;
                    out.mtim_sec = in->mtim_sec;
                    out.mtim_nsec = in->mtim_nsec;
                    out.ctim_sec = in->ctim_sec;
                    out.ctim_nsec = in->ctim_nsec;
                    out.ino = in->ino;
                    map->write(&out, arg(1), sizeof out);
                } else {
                    map->write(kernel_stat, arg(1), kernel_stat_size);
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d-P", sizeof(kernel_stat_32), print_kernel_stat_32);
            break;
        }

	case 199: { /*0xc7, getuid32 */
            syscall_enter("getuid32", "");
            uid_t id = getuid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
	    break;
	}

	case 200: { /*0xc8, getgid32 */
            syscall_enter("getgid32", "");
            uid_t id = getgid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

	case 201: { /*0xc9, geteuid32 */
            syscall_enter("geteuid32", "");
            uid_t id = geteuid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

        case 202: { /*0xca, getegid32 */
            syscall_enter("getegid32", "");
            uid_t id = getegid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

        case 207: { /*0xcf, fchown32 */
            /* int fchown(int fd, uid_t owner, gid_t group);
             * typedef unsigned short  __kernel_old_uid_t;
             * typedef unsigned short  __kernel_old_gid_t;
             *
             * fchown() changes the ownership of the file referred to by the open file descriptor fd. */
            syscall_enter("fchown32", "ddd");
            uint32_t fd = arg(0);
            uid_t  user = arg(1);
            gid_t group = arg(2);
            int result = syscall(SYS_fchown, fd, user, group);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 212: { /*0xd4, chown */
            syscall_enter("chown", "sdd");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                uid_t user = arg(1);
                gid_t group = arg(2);
                int result = chown(filename.c_str(),user,group);
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
        }

 	case 220: {     /* 0xdc, getdents64(int fd, struct linux_dirent*, unsigned int count) */
            syscall_enter("getdents64", "dpd");
            int fd = arg(0), sz = arg(2);
            uint32_t dirent_va = arg(1);
            int status = getdents_syscall<dirent64_t>(fd, dirent_va, sz);
            writeGPR(x86_gpr_ax, status);
            syscall_leave("d-P", status>0?status:0, print_dentries_64);
            break;
        }

        case 221: { // 0xdd fcntl(int fd, int cmd, [long arg | struct flock*])
            static const Translate fcntl_cmds[] = { TE(F_DUPFD),
                                                    TE(F_GETFD), TE(F_SETFD),
                                                    TE(F_GETFL), TE(F_SETFL),
                                                    TE(F_GETLK), TE(F_GETLK64),
                                                    TE(F_SETLK), TE(F_SETLK64),
                                                    TE(F_SETLKW), TE(F_SETLKW64),
                                                    TE(F_SETOWN), TE(F_GETOWN),
                                                    TE(F_SETSIG), TE(F_GETSIG),
                                                    TE(F_SETLEASE), TE(F_GETLEASE),
                                                    TE(F_NOTIFY),
#ifdef F_DUPFD_CLOEXEC
                                                    TE(F_DUPFD_CLOEXEC),
#endif
                                                    T_END};
            int fd=arg(0), cmd=arg(1), other=arg(2), result=-EINVAL;
            switch (cmd) {
                case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
                case F_DUPFD_CLOEXEC:
#endif
                case F_GETFD:
                case F_GETFL:
                case F_GETOWN:
                case F_GETSIG: {
                    syscall_enter("fcntl64", "df", fcntl_cmds);
                    result = fcntl(fd, cmd, other);
                    writeGPR(x86_gpr_ax, -1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETFD:
                case F_SETOWN: {
                    syscall_enter("fcntl64", "dfd", fcntl_cmds);
                    result = fcntl(fd, cmd, other);
                    writeGPR(x86_gpr_ax, -1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETFL: {
                    syscall_enter("fcntl64", "dff", fcntl_cmds, open_flags);
                    result = fcntl(fd, cmd, other);
                    writeGPR(x86_gpr_ax, -1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETSIG: {
                    syscall_enter("fcntl64", "dff", fcntl_cmds, signal_names);
                    result = fcntl(fd, cmd, other);
                    writeGPR(x86_gpr_ax, -1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_GETLK: {
                    syscall_enter("fcntl64", "dfp", fcntl_cmds);
                    flock_64 guest_fl;
                    static flock_64_native host_fl;
#ifdef SYS_fcntl64  /* host is 32-bit */
                    result = syscall(SYS_fcntl64, fd, cmd, &host_fl);
#else               /* host is 64-bit */
                    result = syscall(SYS_fcntl, fd, cmd, &host_fl);
#endif
                    if (-1==result) {
                        result = -errno;
                    } else {
                        guest_fl.l_type = host_fl.l_type;
                        guest_fl.l_whence = host_fl.l_whence;
                        guest_fl.l_start = host_fl.l_start;
                        guest_fl.l_len = host_fl.l_len;
                        guest_fl.l_pid = host_fl.l_pid;
                        if (sizeof(guest_fl)!=map->write(&guest_fl, arg(2), sizeof guest_fl))
                            result = -EFAULT;
                    }
                    writeGPR(x86_gpr_ax, result);
                    syscall_leave("d--P", sizeof(flock_64), print_flock_64);
                    break;
                }
                case F_SETLK:
                case F_SETLKW: {
                    syscall_enter("fcntl64", "dfP", fcntl_cmds, sizeof(flock_64), print_flock_64);
                    flock_64 guest_fl;
                    static flock_64_native host_fl;
                    if (sizeof(guest_fl)!=map->read(&guest_fl, arg(2), sizeof guest_fl)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                    } else {
                        host_fl.l_type = guest_fl.l_type;
                        host_fl.l_whence = guest_fl.l_whence;
                        host_fl.l_start = guest_fl.l_start;
                        host_fl.l_len = guest_fl.l_len;
                        host_fl.l_pid = guest_fl.l_pid;
#ifdef SYS_fcntl64      /* host is 32-bit */
                        result = syscall(SYS_fcntl64, fd, cmd, &host_fl);
#else                   /* host is 64-bit */
                        result = syscall(SYS_fcntl, fd, cmd, &host_fl);
#endif
                        writeGPR(x86_gpr_ax, -1==result?-errno:result);
                    }
                    syscall_leave("d");
                    break;
                }
                default:
                    syscall_enter("fcntl64", "dfd", fcntl_cmds);
                    writeGPR(x86_gpr_ax, -EINVAL);
                    syscall_leave("d");
                    break;
            }
            break;
        }

        case 224: { /*0xe0, gettid*/
            // We have no concept of threads
            syscall_enter("gettid", "");
            writeGPR(x86_gpr_ax, getpid());
            syscall_leave("d");
            break;
       }

        case 240: { /*0xf0, futex*/
            /* We cannot include <linux/futex.h> portably across a variety of Linux machines. */
            static const Translate opflags[] = {
                TF3(0x80, 0x80, FUTEX_PRIVATE_FLAG),
                TF3(0x7f, 0, FUTEX_PRIVATE_FLAG),
                TF3(0x7f, 1, FUTEX_WAKE),
                TF3(0x7f, 2, FUTEX_FD),
                TF3(0x7f, 3, FUTEX_REQUEUE),
                TF3(0x7f, 4, FUTEX_CMP_REQUEUE),
                TF3(0x7f, 5, FUTEX_WAKE_OP),
                TF3(0x7f, 6, FUTEX_LOCK_PI),
                TF3(0x7f, 7, FUTEX_UNLOCK_PI),
                TF3(0x7f, 8, FUTEX_TRYLOCK_PI),
                TF3(0x7f, 9, FUTEX_WAIT_BITSET),
                TF3(0x7f, 10, FUTEX_WAKE_BITSET),
                T_END
            };

            /* Variable arguments */
            unsigned arg1 = arg(1);
            arg1 &= 0x7f;
            switch (arg1) {
                case 0: /*FUTEX_WAIT*/
                    syscall_enter("futex", "PfdP--", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32);
                    break;
                case 1: /*FUTEX_WAKE*/
                    syscall_enter("futex", "Pfd---", 4, print_int_32, opflags);
                    break;
                case 2: /*FUTEX_FD*/
                    syscall_enter("futex", "Pfd---", 4, print_int_32, opflags);
                    break;
                case 3: /*FUTEX_REQUEUE*/
                    syscall_enter("futex", "Pfd-P-", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                case 4: /*FUTEX_CMP_REQUEUE*/
                    syscall_enter("futex", "Pfd-Pd", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                default:
                    syscall_enter("futex", "PfdPPd", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32, 
                                  4, print_int_32);
                    break;
            }

            uint32_t futex1_va=arg(0), op=arg(1), val1=arg(2), timeout_va=arg(3), futex2_va=arg(4), val2=arg(5);
            int *futex1 = (int*)my_addr(futex1_va, sizeof(int));
            int *futex2 = (int*)my_addr(futex2_va, sizeof(int));

            struct timespec timespec_buf, *timespec=NULL;
            if (timeout_va) {
                timespec_32 ts;
                size_t nread = map->read(&ts, timeout_va, sizeof ts);
                ROSE_ASSERT(nread==sizeof ts);
                timespec_buf.tv_sec = ts.tv_sec;
                timespec_buf.tv_nsec = ts.tv_nsec;
                timespec = &timespec_buf;
            }

            int result = syscall(SYS_futex, futex1, op, val1, timespec, futex2, val2);
            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 243: { /*0xf3, set_thread_area*/
            syscall_enter("set_thread_area", "P", sizeof(user_desc), print_user_desc);
            user_desc ud;
            size_t nread = map->read(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nread==sizeof ud);
            if (ud.entry_number==(unsigned)-1) {
                for (ud.entry_number=0x33>>3; ud.entry_number<n_gdt; ud.entry_number++) {
                    if (!gdt[ud.entry_number].useable) break;
                }
                ROSE_ASSERT(ud.entry_number<8192);
                if (debug && trace_syscall)
                    fprintf(debug, "[entry #%d] ", (int)ud.entry_number);
            }
            gdt[ud.entry_number] = ud;
            size_t nwritten = map->write(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nwritten==sizeof ud);
            writeGPR(x86_gpr_ax, 0);
            /* Reload all the segreg shadow values from the (modified) descriptor table */
            for (size_t i=0; i<6; i++)
                writeSegreg((X86SegmentRegister)i, readSegreg((X86SegmentRegister)i));
            syscall_leave("d");
            break;
        }

        case 252: { /*0xfc, exit_group*/
            syscall_enter("exit_group", "d");
            if (debug && trace_syscall) fputs("(throwing...)\n", debug);
            int status=arg(0);
            throw Exit(__W_EXITCODE(status, 0));
        }

        case 258: { /*0x102, set_tid_address*/
            syscall_enter("set_tid_address", "p");
            uint32_t tid_va=arg(0);
            
            /* We want the 32-bit value to be updated by Linux, but if we're running on a 64-bit system then Linux will also
             * update the following 32-bits (probably initializing them to zero).  Therefore we'll create 64 bits memory for
             * Linux to update and map the low-order 32-bits into the specimen. */
            int *tidptr = NULL;
            if (sizeof(int)>4) {
                const MemoryMap::MapElement *orig = map->find(tid_va);
                if (orig->get_va()!=tid_va || orig->get_size()!=sizeof(int)) {
                    tidptr = new int;
                    *tidptr = 0;
                    size_t nread = map->read(tidptr, tid_va, 4); /*only low-order bytes*/
                    ROSE_ASSERT(4==nread);
                    MemoryMap::MapElement submap(tid_va, 4, tidptr, 0, orig->get_mapperms());
                    submap.set_name("set_tid_address");
                    map->insert(submap);
                }
            } else {
                tidptr = (int*)my_addr(tid_va, 4);
            }

            syscall(SYS_set_tid_address, tidptr);
            writeGPR(x86_gpr_ax, getpid());

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after set_tid_address syscall:\n");
                map->dump(debug, "    ");
            }
            break;
        }

        case 264: { /* 0x108, clock_settime */
            syscall_enter("clock_settime", "eP", clock_names, sizeof(timespec_32), print_timespec_32);
            do {
                timespec_32 guest_ts;
                if (sizeof(guest_ts)!=map->read(&guest_ts, arg(1), sizeof guest_ts)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                static timespec host_ts;
                host_ts.tv_sec = guest_ts.tv_sec;
                host_ts.tv_nsec = guest_ts.tv_nsec;
                int result = syscall(SYS_clock_settime, arg(0), &host_ts);
                writeGPR(x86_gpr_ax, -1==result?-errno:result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 265: { /* 0x109, clock_gettime */
            syscall_enter("clock_gettime", "ep", clock_names);
            do {
                static timespec host_ts;
                int result = syscall(SYS_clock_gettime, arg(0), &host_ts);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                timespec_32 guest_ts;
                guest_ts.tv_sec = host_ts.tv_sec;
                guest_ts.tv_nsec = host_ts.tv_nsec;
                if (sizeof(guest_ts)!=map->write(&guest_ts, arg(1), sizeof guest_ts)) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 266: { /* 0x10a, clock_getres */
            syscall_enter("clock_getres", "ep", clock_names);
            do {
                static timespec host_ts;
                timespec *host_tsp = arg(1) ? &host_ts : NULL;
                int result = syscall(SYS_clock_getres, arg(0), host_tsp);
                if (-1==result) {
                    writeGPR(x86_gpr_ax, -errno);
                    break;
                }

                if (arg(1)) {
                    timespec_32 guest_ts;
                    guest_ts.tv_sec = host_ts.tv_sec;
                    guest_ts.tv_nsec = host_ts.tv_nsec;
                    if (sizeof(guest_ts)!=map->write(&guest_ts, arg(1), sizeof guest_ts)) {
                        writeGPR(x86_gpr_ax, -EFAULT);
                        break;
                    }
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 270: { /*0x10e tgkill*/
            syscall_enter("tgkill", "ddf", signal_names);
            uint32_t /*tgid=arg(0), pid=arg(1),*/ sig=arg(2);
            // TODO: Actually check thread group and kill properly
            if (debug && trace_syscall) fputs("(throwing...)\n", debug);
            throw Exit(__W_EXITCODE(0, sig));
            break;

        }

        case 271: { /* 0x10f, utimes */
            /*
                int utimes(const char *filename, const struct timeval times[2]);

                struct timeval {
                    long tv_sec;        // seconds 
                    long tv_usec;   // microseconds 
                };


                The utimes() system call changes the access and modification times of the inode
                specified by filename to the actime and modtime fields of times respectively.

                times[0] specifies the new access time, and times[1] specifies the new
                modification time.  If times is NULL, then analogously to utime(), the access
                and modification times of the file are set to the current time.


            */
            syscall_enter("utimes", "s");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                //Check to see if times is NULL
                uint8_t byte;
                size_t nread = map->read(&byte, arg(1), 1);
                ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

                int result;
                if( byte ) {

                    size_t size_timeval_sample = sizeof(timeval_32)*2;

                    timeval_32 ubuf[1];

                    size_t nread = map->read(&ubuf, arg(1), size_timeval_sample);


                    timeval timeval64[1];
                    timeval64[0].tv_sec  = ubuf[0].tv_sec;
                    timeval64[0].tv_usec = ubuf[0].tv_usec;
                    timeval64[1].tv_sec  = ubuf[1].tv_sec;
                    timeval64[1].tv_usec = ubuf[1].tv_usec;

                    ROSE_ASSERT(nread == size_timeval_sample);

                    result = utimes(filename.c_str(), timeval64);

                } else {
                    result = utimes(filename.c_str(), NULL);
                }

                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;

        }

	case 306: { /* 0x132, fchmodat */
            syscall_enter("fchmodat", "dsdd");
            do {
                int dirfd = arg(0);
                uint32_t path = arg(1);
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }
                mode_t mode = arg(2);
                int flags = arg(3);

                int result = syscall( 306, dirfd, (long) sys_path.c_str(), mode, flags);
                if (result == -1) result = -errno;
                writeGPR(x86_gpr_ax, result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 311: { /*0x137, set_robust_list*/
            syscall_enter("set_robust_list", "Pd", sizeof(robust_list_head_32), print_robust_list_head_32);
            do {
                uint32_t head_va=arg(0), len=arg(1);
                if (len!=sizeof(robust_list_head_32)) {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }

                robust_list_head_32 guest_head;
                if (sizeof(guest_head)!=map->read(&guest_head, head_va, sizeof(guest_head))) {
                    writeGPR(x86_gpr_ax, -EFAULT);
                    break;
                }

                /* The robust list is maintained in user space and accessed by the kernel only when we a thread dies. Since the
                 * simulator handles thread death, we don't need to tell the kernel about the specimen's list until later. In
                 * fact, we can't tell the kernel because that would cause our own list (set by libc) to be removed from the
                 * kernel. */
                robust_list_head_va = head_va;
                writeGPR(x86_gpr_ax, 0);
            } while (0);
            syscall_leave("d");
            break;
        }

        default: {
            fprintf(stderr, "syscall_%u(", callno);
            for (int i=0; i<6; i++)
                fprintf(stderr, "%s0x%08"PRIx32, i?", ":"", arg(i));
            fprintf(stderr, ") is not implemented yet\n\n");
            dump_core(SIGSYS);
            abort();
        }
    }
    ROSE_ASSERT( this != NULL  );
}

void
EmulationPolicy::syscall_arginfo(char format, uint32_t val, ArgInfo *info, va_list *ap)
{
    ROSE_ASSERT(info!=NULL);
    info->val = val;
    switch (format) {
        case 'f':       /*flags*/
        case 'e':       /*enum*/
            info->xlate = va_arg(*ap, const Translate*);
            break;
        case 's': {     /*NUL-terminated string*/
            info->str = read_string(val, 4096, &(info->str_fault));
            info->str_trunc = (info->str.size() >= 4096);
            break;
        }
        case 'b': {     /* buffer */
            size_t advertised_size = va_arg(*ap, size_t);
            info->struct_buf = new uint8_t[advertised_size];
            info->struct_nread = map->read(info->struct_buf, info->val, advertised_size);
            info->struct_size = 64; /* max print width, measured in columns of output */
            break;
        }
        case 'P': {     /*ptr to a struct*/
            info->struct_size = va_arg(*ap, size_t);
            info->struct_printer = va_arg(*ap, ArgInfo::StructPrinter);
            info->struct_buf = new uint8_t[info->struct_size];
            info->struct_nread = map->read(info->struct_buf, info->val, info->struct_size);
            break;
        }
    }
}

void
EmulationPolicy::syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app)
{
    static timeval first_call;

    if (debug && trace_syscall) {
        timeval this_call;
        gettimeofday(&this_call, NULL);
        if (0==first_call.tv_sec)
            first_call = this_call;
        double elapsed = (this_call.tv_sec - first_call.tv_sec) + (this_call.tv_usec - first_call.tv_usec)/1e6;

        if (did_fork)
            fprintf(debug, "[pid %d] ", getpid());
        fprintf(debug, "0x%08"PRIx64" %8.4f: ", readIP().known_value(), elapsed);
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], values?values[i]:arg(i), args+i, app);
        print_enter(debug, name, arg(-1), format, args);
    }
}

void
EmulationPolicy::syscall_enter(uint32_t *values, const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(values, name, format, &ap);
    va_end(ap);
}

void
EmulationPolicy::syscall_enter(const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(NULL, name, format, &ap);
    va_end(ap);
}

void
EmulationPolicy::syscall_leave(const char *format, ...) 
{
    va_list ap;
    va_start(ap, format);

    ROSE_ASSERT(strlen(format)>=1);
    if (debug && trace_syscall) {
        /* System calls return an integer (negative error numbers, non-negative success) */
        ArgInfo info;
        uint32_t value = readGPR(x86_gpr_ax).known_value();
        syscall_arginfo(format[0], value, &info, &ap);
        print_leave(debug, format[0], &info);

        /* Additionally, output any other buffer values that were filled in by a successful system call. */
        if (format[0]!='d' || 0==(arg(-1) & 0x80000000)) {
            for (size_t i=1; format[i]; i++) {
                if ('-'!=format[i]) {
                    syscall_arginfo(format[i], arg(i-1), &info, &ap);
                    if ('P'!=format[i] || 0!=arg(i-1)) { /* no need to show null pointers */
                        fprintf(debug, "    arg%zu = ", i-1);
                        print_single(debug, format[i], &info);
                        fprintf(debug, "\n");
                    }
                }
            }
        }
    }
}

uint32_t
EmulationPolicy::arg(int idx)
{
    switch (idx) {
        case -1: return readGPR(x86_gpr_ax).known_value();      /* syscall return value */
        case 0: return readGPR(x86_gpr_bx).known_value();
        case 1: return readGPR(x86_gpr_cx).known_value();
        case 2: return readGPR(x86_gpr_dx).known_value();
        case 3: return readGPR(x86_gpr_si).known_value();
        case 4: return readGPR(x86_gpr_di).known_value();
        case 5: return readGPR(x86_gpr_bp).known_value();
        default: assert(!"invalid argument number"); abort();
    }
}

/* Place signal on queue of pending signals. */
void
EmulationPolicy::signal_arrival(int signo)
{
    ROSE_ASSERT(signo>0 && signo<_NSIG);
    bool is_masked = (0 != (signal_mask & (1<<(signo-1))));

    if (debug && trace_signal) {
        fprintf(debug, " [signal ");
        print_enum(debug, signal_names, signo);
        fprintf(debug, " (%s)", strsignal(signo));
    }

    
    if (signal_action[signo].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        if (debug && trace_signal)
            fputs(" ignored]", debug);
    } else if (is_masked) {
        if (debug && trace_signal)
            fputs(" masked]", debug);
        signal_queue.push(signo);
    } else {
        if (debug && trace_signal)
            fputs(" arrived]", debug);
        signal_queue.push(signo);
        signal_reprocess = true;
    }
}

/* Dispatch signals on the queue if they are not masked. */
void
EmulationPolicy::signal_dispatch()
{
    std::queue<int> pending;
    if (signal_reprocess) {
        signal_reprocess = false;
        while (!signal_queue.empty()) {
            int signo = signal_queue.front();
            signal_queue.pop();
            bool is_pending = (0 != (signal_mask & (1 << (signo-1))));
            if (is_pending) {
                pending.push(signo);
            } else {
                signal_dispatch(signo);
            }
        }
        signal_queue = pending;
    }
}

/* Dispatch the specified signal */
void
EmulationPolicy::signal_dispatch(int signo)
{
    if (debug && trace_signal) {
        fprintf(debug, "0x%08"PRIx64": dispatching signal ", readIP().known_value());
        print_enum(debug, signal_names, signo);
        fprintf(debug, " (%s)\n", strsignal(signo));
    }

    if (signal_action[signo].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        if (debug && trace_signal)
            fprintf(debug, "    signal is ignored\n");
    } else if (signal_action[signo].handler_va==(uint32_t)(uint64_t)SIG_DFL) {
        if (debug && trace_signal)
            fprintf(debug, "    signal causes default action\n");
        switch (signo) {
            case SIGFPE:
            case SIGILL:
            case SIGSEGV:
            case SIGBUS:
            case SIGABRT:
            case SIGTRAP:
            case SIGSYS:
                /* Exit with core dump */
                dump_core(signo);
                throw Exit((signo & 0x7f) | __WCOREFLAG);
            case SIGTERM:
            case SIGINT:
            case SIGQUIT:
            case SIGKILL:
            case SIGHUP:
            case SIGALRM:
            case SIGVTALRM:
            case SIGPROF:
            case SIGPIPE:
            case SIGXCPU:
            case SIGXFSZ:
            case SIGUSR1:
            case SIGUSR2:
                /* Exit without core dump */
                throw Exit(signo & 0x7f);
            case SIGIO:
            case SIGURG:
            case SIGCHLD:
            case SIGCONT:
            case SIGSTOP:
            case SIGTTIN:
            case SIGTTOU:
            case SIGWINCH:
                /* Signal is ignored by default */
                return;
            default:
                /* Exit without a core dump */
                throw Exit(signo & 0x7f);
        }

    } else if (signal_mask & ((uint64_t)1 << signo)) {
        if (debug && trace_signal)
            fprintf(debug, "    signal is currently masked (procmask)\n");
    } else {
        ROSE_ASSERT(0==signal_return); /* possible violation if specimen exits previous handler with longjmp */
        ROSE_ASSERT(0==signal_oldstack); /* ditto */
        signal_return = readIP().known_value();

        /* Switch to the alternate stack? */
        signal_oldstack = readGPR(x86_gpr_sp).known_value();
        if (0==(signal_stack.ss_flags & SS_ONSTACK) && 0!=(signal_action[signo].flags & SA_ONSTACK))
            writeGPR(x86_gpr_sp, number<32>(signal_stack.ss_sp + signal_stack.ss_size));

        /* Caller-saved registers */
        push(readGPR(x86_gpr_ax));
        push(readGPR(x86_gpr_bx));
        push(readGPR(x86_gpr_cx));
        push(readGPR(x86_gpr_dx));
        push(readGPR(x86_gpr_si));
        push(readGPR(x86_gpr_di));
        push(readGPR(x86_gpr_bp));

        /* Signal handler arguments */
        push(readIP());
        push(number<32>(signo));
        push(number<32>(SIGHANDLER_RETURN)); /* fake return address to trigger signal_cleanup() call */
        writeIP(number<32>(signal_action[signo].handler_va));
    }
}

/* Warning: if the specimen calls longjmp() or siglongjmp() from inside the signal handler in order to exit the handler, this
 * signal_cleanup() function will never be executed. */
void
EmulationPolicy::signal_cleanup()
{
    if (debug && trace_signal)
        fprintf(debug, "0x%08"PRIx32": returning from signal handler\n", signal_return);
    
    ROSE_ASSERT(signal_return!=0);
    ROSE_ASSERT(signal_oldstack!=0);

    /* Discard handler arguments */
    pop(); /* signal number */
    pop(); /* signal address */

    /* Restore caller-saved registers */
    writeGPR(x86_gpr_bp, pop());
    writeGPR(x86_gpr_di, pop());
    writeGPR(x86_gpr_si, pop());
    writeGPR(x86_gpr_dx, pop());
    writeGPR(x86_gpr_cx, pop());
    writeGPR(x86_gpr_bx, pop());
    writeGPR(x86_gpr_ax, pop());

    /* Restore normal stack */
    writeGPR(x86_gpr_sp, number<32>(signal_oldstack));
    signal_oldstack = 0;

    /* Resume execution at address where signal occurred */
    writeIP(number<32>(signal_return));
    signal_return = 0;
}

static EmulationPolicy *signal_deliver_to;
static void
signal_handler(int signo)
{
    if (signal_deliver_to) {
        signal_deliver_to->signal_arrival(signo);
    } else {
        fprintf(stderr, "received signal %d (%s); this signal cannot be delivered so we're taking it ourselves\n",
                signo, strsignal(signo));
        struct sigaction sa, old_sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(signo, &sa, &old_sa);
        sigset_t ss, old_ss;
        sigemptyset(&ss);
        sigaddset(&ss, signo);
        sigprocmask(SIG_UNBLOCK, &ss, &old_ss);
        raise(signo);
        sigprocmask(SIG_SETMASK, &old_ss, NULL);
        sigaction(signo, &old_sa, NULL);
    }
}

int
main(int argc, char *argv[], char *envp[])
{
    typedef X86InstructionSemantics<EmulationPolicy, VirtualMachineSemantics::ValueType> Semantics;
    EmulationPolicy policy;
    Semantics t(policy);
    uint32_t dump_at = 0;               /* dump core the first time we hit this address, before the instruction is executed */
    std::string dump_name = "dump";
    FILE *log_file = NULL;

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP,   &sa, NULL);
    sigaction(SIGINT,   &sa, NULL);
    sigaction(SIGQUIT,  &sa, NULL);
    sigaction(SIGILL,   &sa, NULL);
    sigaction(SIGTRAP,  &sa, NULL);
    sigaction(SIGABRT,  &sa, NULL);
    sigaction(SIGBUS,   &sa, NULL);
    sigaction(SIGFPE,   &sa, NULL);
    sigaction(SIGUSR1,  &sa, NULL);
    sigaction(SIGSEGV,  &sa, NULL);
    sigaction(SIGUSR2,  &sa, NULL);
    sigaction(SIGPIPE,  &sa, NULL);
    sigaction(SIGALRM,  &sa, NULL);
    sigaction(SIGTERM,  &sa, NULL);
    sigaction(SIGSTKFLT,&sa, NULL);
    sigaction(SIGCHLD,  &sa, NULL);
    sigaction(SIGTSTP,  &sa, NULL);
    sigaction(SIGTTIN,  &sa, NULL);
    sigaction(SIGTTOU,  &sa, NULL);
    sigaction(SIGURG,   &sa, NULL);
    sigaction(SIGXCPU,  &sa, NULL);
    sigaction(SIGXFSZ,  &sa, NULL);
    sigaction(SIGVTALRM,&sa, NULL);
    sigaction(SIGPROF,  &sa, NULL);
    sigaction(SIGWINCH, &sa, NULL);
    sigaction(SIGIO,    &sa, NULL);
    sigaction(SIGPWR,   &sa, NULL);
    sigaction(SIGSYS,   &sa, NULL);
    sigaction(SIGXFSZ,  &sa, NULL);
    for (int i=SIGRTMIN; i<=SIGRTMAX; i++)
        sigaction(i, &sa, NULL);

    /* Parse command-line */
    int argno = 1;
    while (argno<argc && '-'==argv[argno][0]) {
        if (!strcmp(argv[argno], "--")) {
            argno++;
            break;
        } else if (!strncmp(argv[argno], "--log=", 6)) {
            if (log_file)
                fclose(log_file);
            if (NULL==(log_file = fopen(argv[argno]+6, "w"))) {
                fprintf(stderr, "%s: %s: %s\n", argv[0], strerror(errno), argv[argno+6]);
                exit(1);
            }
            argno++;
        } else if (!strncmp(argv[argno], "--debug=", 8)) {
            policy.debug = stderr;
            char *s = argv[argno]+8;
            while (s && *s) {
                char *comma = strchr(s, ',');
                std::string word(s, comma?comma-s:strlen(s));
                s = comma ? comma+1 : NULL;
                if (word=="all") {
                    policy.trace_insn = true;
                    policy.trace_state = true;
                    policy.trace_mem = true;
                    policy.trace_mmap = true;
                    policy.trace_syscall = true;
                    policy.trace_loader = true;
                    policy.trace_progress = true;
                    policy.trace_signal = true;
                } else if (word=="insn") {
                    policy.trace_insn = true;
                } else if (word=="state") {
                    policy.trace_state = true;
                } else if (word=="mem") {
                    policy.trace_mem = true;
                } else if (word=="mmap") {
                    policy.trace_mmap = true;
                } else if (word=="signal") {
                    policy.trace_signal = true;
                } else if (word=="syscall") {
                    policy.trace_syscall = true;
                } else if (word=="loader") {
                    policy.trace_loader = true;
                } else if (word=="progress") {
                    policy.trace_progress = true;
                } else {
                    fprintf(stderr, "%s: debug words must be from the set: "
                            "all, insn, state, mem, mmap, syscall, signal, loader, progress\n",
                            argv[0]);
                    exit(1);
                }
            }
            argno++;
        } else if (!strcmp(argv[argno], "--debug")) {
            policy.debug = stderr;
            policy.trace_insn = true;
            policy.trace_syscall = true;
            argno++;
        } else if (!strncmp(argv[argno], "--core=", 7)) {
            policy.core_styles = 0;
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                if (!strncmp(s, "elf", 3)) {
                    s += 3;
                    policy.core_styles |= CORE_ELF;
                } else if (!strncmp(s, "rose", 4)) {
                    s += 4;
                    policy.core_styles |= CORE_ROSE;
                } else {
                    fprintf(stderr, "%s: unknown core dump type for %s\n", argv[0], argv[argno]);
                }
                while (','==*s) s++;
            }
            argno++;
        } else if (!strncmp(argv[argno], "--dump=", 7)) {
            char *rest;
            errno = 0;
            dump_at = strtoul(argv[argno]+7, &rest, 0);
            if (rest==argv[argno]+7 || errno!=0) {
                fprintf(stderr, "%s: --dump=N requires an address, N\n", argv[0]);
                exit(1);
            }
            if (','==rest[0] && rest[1])
                dump_name = rest+1;
            argno++;
        } else if (!strncmp(argv[argno], "--interp=", 9)) {
            policy.interpname = argv[argno++]+9;
        } else if (!strncmp(argv[argno], "--vdso=", 7)) {
            policy.vdso_paths.clear();
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                char *colon = strchr(s, ':');
                policy.vdso_paths.push_back(std::string(s, colon?colon-s:strlen(s)));
                s = colon ? colon+1 : NULL;
            }
            argno++;
        } else if (!strcmp(argv[argno], "--showauxv")) {
            fprintf(stderr, "showing the auxiliary vector for x86sim:\n");
            argno++;
            struct auxv_t {
                unsigned long type;
                unsigned long val;
            };
            char **p = envp;
            while (*p++);
            for (auxv_t *auxvp=(auxv_t*)p; 1; auxvp++) {
                switch (auxvp->type) {
                    case 0:  fprintf(stderr, "    0  AT_NULL         %lu\n", auxvp->val); break;
                    case 1:  fprintf(stderr, "    1  AT_IGNORE       %lu\n", auxvp->val); break;
                    case 2:  fprintf(stderr, "    2  AT_EXECFD       %lu\n", auxvp->val); break;
                    case 3:  fprintf(stderr, "    3  AT_PHDR         0x%lx\n", auxvp->val); break;
                    case 4:  fprintf(stderr, "    4  AT_PHENT        0x%lx\n", auxvp->val); break;
                    case 5:  fprintf(stderr, "    5  AT_PHNUM        %lu\n", auxvp->val); break;
                    case 6:  fprintf(stderr, "    6  AT_PAGESZ       %lu\n", auxvp->val); break;
                    case 7:  fprintf(stderr, "    7  AT_BASE         0x%lx\n", auxvp->val); break;
                    case 8:  fprintf(stderr, "    8  AT_FLAGS        0x%lx\n", auxvp->val); break;
                    case 9:  fprintf(stderr, "    9  AT_ENTRY        0x%lx\n", auxvp->val); break;
                    case 10: fprintf(stderr, "    10 AT_NOTELF       %lu\n", auxvp->val); break;
                    case 11: fprintf(stderr, "    11 AT_UID          %ld\n", auxvp->val); break;
                    case 12: fprintf(stderr, "    12 AT_EUID         %ld\n", auxvp->val); break;
                    case 13: fprintf(stderr, "    13 AT_GID          %ld\n", auxvp->val); break;
                    case 14: fprintf(stderr, "    14 AT_EGID         %ld\n", auxvp->val); break;
                    case 15: fprintf(stderr, "    15 AT_PLATFORM     0x%lx\n", auxvp->val); break;
                    case 16: fprintf(stderr, "    16 AT_HWCAP        0x%lx\n", auxvp->val); break;
                    case 17: fprintf(stderr, "    17 AT_CLKTCK       %lu\n", auxvp->val); break;
                    case 18: fprintf(stderr, "    18 AT_FPUCW        %lu\n", auxvp->val); break;
                    case 19: fprintf(stderr, "    19 AT_DCACHEBSIZE  %lu\n", auxvp->val); break;
                    case 20: fprintf(stderr, "    20 AT_ICACHEBSIZE  %lu\n", auxvp->val); break;
                    case 21: fprintf(stderr, "    21 AT_UCACHEBSIZE  %lu\n", auxvp->val); break;
                    case 22: fprintf(stderr, "    22 AT_IGNOREPPC    %lu\n", auxvp->val); break;
                    case 23: fprintf(stderr, "    23 AT_SECURE       %ld\n", auxvp->val); break;

                    case 32: fprintf(stderr, "    32 AT_SYSINFO      0x%lx\n", auxvp->val); break;
                    case 33: fprintf(stderr, "    33 AT_SYSINFO_PHDR 0x%lx\n", auxvp->val); break;
                    case 34: fprintf(stderr, "    34 AT_L1I_CACHESHAPE 0x%lx\n", auxvp->val); break;
                    case 35: fprintf(stderr, "    35 AT_L1D_CACHESHAPE 0x%lx\n", auxvp->val); break;
                    case 36: fprintf(stderr, "    36 AT_L2_CACHESHAPE  0x%lx\n", auxvp->val); break;
                    case 37: fprintf(stderr, "    37 AT_L3_CACHESHAPE  0x%lx\n", auxvp->val); break;

                    default: fprintf(stderr, "    %lu AT_(unknown)   0x%lx\n", auxvp->type, auxvp->val); break;
                }
                if (!auxvp->type)
                    break;
            }

        } else {
            fprintf(stderr, "usage: %s [--debug] PROGRAM ARGUMENTS...\n", argv[0]);
            exit(1);
        }
    }
    ROSE_ASSERT(argc-argno>=1); /* usage: executable name followed by executable's arguments */
    if (policy.debug && log_file)
        policy.debug = log_file;
    SgAsmGenericHeader *fhdr = policy.load(argv[argno]); /*header for main executable, not libraries*/
    policy.initialize_stack(fhdr, argc-argno, argv+argno);

    /* Debugging */
    if (policy.debug && policy.trace_mmap) {
        fprintf(policy.debug, "memory map after program load:\n");
        policy.map->dump(policy.debug, "  ");
    }
    if (policy.debug && policy.trace_state) {
        fprintf(policy.debug, "Initial state:\n");
        policy.dump_registers(policy.debug);
    }
    if (policy.debug && policy.trace_progress) {
        struct sigaction sa;
        sa.sa_handler = alarm_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGALRM, &sa, NULL);
        alarm(PROGRESS_INTERVAL);
    }

    /* Execute the program */
    signal_deliver_to = &policy;
    struct timeval sim_start_time;
    gettimeofday(&sim_start_time, NULL);
    bool seen_entry_va = false;
    while (true) {
        if (had_alarm) {
            had_alarm = 0;
            alarm(PROGRESS_INTERVAL);
            if (policy.debug && policy.trace_progress) {
                struct timeval cur_time;
                gettimeofday(&cur_time, NULL);
                double nsec = cur_time.tv_sec + cur_time.tv_usec/1e6 - (sim_start_time.tv_sec + sim_start_time.tv_usec/1e6);
                double insn_rate = nsec>0 ? policy.get_ninsns() / nsec : 0;
                fprintf(policy.debug, "x86sim: processed %zu insns in %d sec (%d insns/sec)\n",
                        policy.get_ninsns(), (int)(nsec+0.5), (int)(insn_rate+0.5));
            }
        }
        try {
            if (policy.readIP().known_value()==policy.SIGHANDLER_RETURN) {
                policy.signal_cleanup();
                if (dump_at!=0 && dump_at==policy.SIGHANDLER_RETURN) {
                    fprintf(stderr, "Reached dump point.\n");
                    policy.dump_core(SIGABRT, dump_name);
                    dump_at = 0;
                }
                continue;
            }

            if (dump_at!=0 && dump_at == policy.readIP().known_value()) {
                fprintf(stderr, "Reached dump point.\n");
                policy.dump_core(SIGABRT, dump_name);
                dump_at = 0;
            }

            SgAsmx86Instruction *insn = policy.current_insn();
            if (policy.debug && policy.trace_mmap &&
                !seen_entry_va && insn->get_address()==fhdr->get_base_va()+fhdr->get_entry_rva()) {
                fprintf(policy.debug, "memory map at program entry:\n");
                policy.map->dump(policy.debug, "  ");
                seen_entry_va = true;
            }
            t.processInstruction(insn);
            if (policy.debug && policy.trace_state)
                policy.dump_registers(policy.debug);

            policy.signal_dispatch();
        } catch (const Semantics::Exception &e) {
            std::cerr <<e <<"\n\n";
#ifdef X86SIM_STRICT_EMULATION
            policy.dump_core(SIGILL);
            abort();
#else
            std::cerr <<"Ignored. Continuing with a corrupt state...\n";
#endif
        } catch (const VirtualMachineSemantics::Policy::Exception &e) {
            std::cerr <<e <<"\n\n";
            policy.dump_core(SIGILL);
            abort();
        } catch (const EmulationPolicy::Exit &e) {
            /* specimen has exited */
            if (policy.robust_list_head_va)
                fprintf(stderr, "warning: robust_list not cleaned up\n"); /* FIXME: see set_robust_list() syscall */
            if (WIFEXITED(e.status)) {
                fprintf(stderr, "specimen exited with status %d\n", WEXITSTATUS(e.status));
		if (WEXITSTATUS(e.status))
                    exit(WEXITSTATUS(e.status));
            } else if (WIFSIGNALED(e.status)) {
                fprintf(stderr, "specimen exited due to signal ");
                print_enum(stderr, signal_names, WTERMSIG(e.status));
                fprintf(stderr, " (%s)%s\n", strsignal(WTERMSIG(e.status)), WCOREDUMP(e.status)?" core dumped":"");
                exit(1);
            } else if (WIFSTOPPED(e.status)) {
                fprintf(stderr, "specimen is stopped due to signal ");
                print_enum(stderr, signal_names, WSTOPSIG(e.status));
                fprintf(stderr, " (%s)\n", strsignal(WSTOPSIG(e.status)));
                exit(1);
            }
            break;
        } catch (const EmulationPolicy::Signal &e) {
            policy.signal_arrival(e.signo);
        }
    }
    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
