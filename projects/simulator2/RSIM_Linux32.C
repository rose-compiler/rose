/* This file contains Linux-32 system call emulation.  Most of these functions are callbacks and have names like:
 *
 *    RSIM_Linux32::syscall_FOO_enter           -- prints syscall tracing info when the call is entered
 *    RSIM_Linux32::syscall_FOO                 -- implements the system call
 *    RSIM_Linux32::syscall_FOO_leave           -- prints syscall tracing info when the call returns
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux32.h"
#include "Diagnostics.h"

#include <errno.h>
#include <syscall.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <utime.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <grp.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

void
RSIM_Linux32::init()
{
    vdsoName("linux-gate.so");

    if (interpreterBaseVa() == 0)
        interpreterBaseVa(0x40000000);

    // System call registers.
    syscallReturnRegister(RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_bx, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_cx, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_dx, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_si, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_di, 0, 32));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_bp, 0, 32));

#   define SC_REG(NUM, NAME, LEAVE)                                                                                            \
        syscall_define((NUM), syscall_##NAME##_enter, syscall_##NAME##_body, syscall_##LEAVE##_leave);

    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     * according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers here. */
    SC_REG(1,   exit,                           exit);
    SC_REG(3,   read,                           read);
    SC_REG(4,   write,                          default);
    SC_REG(5,   open,                           default);
    SC_REG(6,   close,                          default);
    SC_REG(7,   waitpid,                        waitpid);
    SC_REG(8,   creat,                          default);
    SC_REG(9,   link,                           default);
    SC_REG(10,  unlink,                         default);
    SC_REG(11,  execve,                         default);
    SC_REG(12,  chdir,                          default);
    SC_REG(13,  time,                           time);
    SC_REG(14,  mknod,                          default);
    SC_REG(15,  chmod,                          default);
    SC_REG(19,  lseek,                          default);
    SC_REG(20,  getpid,                         default);
    SC_REG(24,  getuid,                         default);
    SC_REG(27,  alarm,                          default);
    SC_REG(29,  pause,                          pause);
    SC_REG(30,  utime,                          default);
    SC_REG(33,  access,                         default);
    SC_REG(36,  sync,                           default);
    SC_REG(37,  kill,                           default);
    SC_REG(38,  rename,                         default);
    SC_REG(39,  mkdir,                          default);
    SC_REG(40,  rmdir,                          default);
    SC_REG(41,  dup,                            default);
    SC_REG(42,  pipe,                           pipe);
    SC_REG(45,  brk,                            brk);
    SC_REG(47,  getgid,                         default);
    SC_REG(49,  geteuid,                        default);
    SC_REG(50,  getegid,                        default);
    SC_REG(54,  ioctl,                          ioctl);
    SC_REG(57,  setpgid,                        default);
    SC_REG(60,  umask,                          default);
    SC_REG(63,  dup2,                           default);
    SC_REG(64,  getppid,                        default);
    SC_REG(65,  getpgrp,                        default);
    SC_REG(75,  setrlimit,                      default);
    SC_REG(76,  getrlimit,                      getrlimit);
    SC_REG(78,  gettimeofday,                   gettimeofday);
    SC_REG(83,  symlink,                        default);
    SC_REG(85,  readlink,                       default);               // FIXME: probably needs an explicit leave
    SC_REG(90,  mmap,                           mmap);
    SC_REG(91,  munmap,                         default);
    SC_REG(93,  ftruncate,                      default);
    SC_REG(94,  fchmod,                         default);
    SC_REG(95,  fchown,                         default);
    SC_REG(99,  statfs,                         statfs);
    SC_REG(100, fstatfs,                        fstatfs);
    SC_REG(102, socketcall,                     socketcall);
    SC_REG(114, wait4,                          wait4);
    SC_REG(116, sysinfo,                        sysinfo);
    SC_REG(117, ipc,                            ipc);
    SC_REG(118, fsync,                          default);
    SC_REG(119, sigreturn,                      sigreturn);
    SC_REG(120, clone,                          clone);
    SC_REG(122, uname,                          uname);
    SC_REG(123, modify_ldt,                     modify_ldt);
    SC_REG(125, mprotect,                       mprotect);
    SC_REG(133, fchdir,                         default);
    SC_REG(140, llseek,                         default);
    SC_REG(141, getdents,                       getdents);
    SC_REG(142, select,                         select);
    SC_REG(144, msync,                          default);
    SC_REG(146, writev,                         default);
    SC_REG(155, sched_setparam,                 default);
    SC_REG(156, sched_setscheduler,             default);
    SC_REG(157, sched_getscheduler,             sched_getscheduler);
    SC_REG(158, sched_yield,                    default);
    SC_REG(159, sched_get_priority_max,         default);
    SC_REG(160, sched_get_priority_min,         default);
    SC_REG(162, nanosleep,                      nanosleep);
    SC_REG(172, prctl,                          default);
    SC_REG(173, rt_sigreturn,                   rt_sigreturn);
    SC_REG(174, rt_sigaction,                   rt_sigaction);
    SC_REG(175, rt_sigprocmask,                 rt_sigprocmask);
    SC_REG(176, rt_sigpending,                  rt_sigpending);
    SC_REG(179, rt_sigsuspend,                  rt_sigsuspend);
    SC_REG(180, pread64,                        pread64);
    SC_REG(183, getcwd,                         getcwd);
    SC_REG(186, sigaltstack,                    sigaltstack);
    SC_REG(191, ugetrlimit,                     ugetrlimit);
    SC_REG(192, mmap2,                          mmap2);
    SC_REG(195, stat64,                         stat64);
    SC_REG(196, stat64,                         stat64);        // lstat64
    SC_REG(197, stat64,                         stat64);        // fstat64
    SC_REG(199, getuid32,                       default);
    SC_REG(200, getgid32,                       default);
    SC_REG(201, geteuid32,                      default);
    SC_REG(202, getegid32,                      default);
    SC_REG(205, getgroups32,                    getgroups32);
    SC_REG(206, setgroups32,                    default);
    SC_REG(207, fchown32,                       default);
    SC_REG(212, chown,                          default);
    SC_REG(219, madvise,                        default);
    SC_REG(220, getdents64,                     getdents64);
    SC_REG(221, fcntl,                          fcntl);
    SC_REG(224, gettid,                         default);
    SC_REG(240, futex,                          futex);
    SC_REG(242, sched_getaffinity,              sched_getaffinity);
    SC_REG(243, set_thread_area,                set_thread_area);
    SC_REG(252, exit_group,                     exit_group);
    SC_REG(258, set_tid_address,                default);
    SC_REG(264, clock_settime,                  default);
    SC_REG(265, clock_gettime,                  clock_gettime);
    SC_REG(266, clock_getres,                   clock_getres);
    SC_REG(268, statfs64,                       statfs64);
    SC_REG(269, fstatfs64,                      fstatfs64);
    SC_REG(270, tgkill,                         default);
    SC_REG(271, utimes,                         default);
    SC_REG(306, fchmodat,                       default);
    SC_REG(311, set_robust_list,                default);
    SC_REG(331, pipe2,                          pipe2);

#undef SC_REG
}

void
RSIM_Linux32::initializeSimulatedOs(RSIM_Process *process, SgAsmGenericHeader *hdr) {
    RSIM_Linux::initializeSimulatedOs(process, hdr);
    process->mmapNextVa(0x40000000);
    process->mmapGrowsDown(false); // ^^-- is a minimum address
    process->mmapRecycle(false);
}

bool
RSIM_Linux32::isSupportedArch(SgAsmGenericHeader *fhdr) {
    return isSgAsmElfFileHeader(fhdr) && fhdr->get_word_size()==4;
}

void
RSIM_Linux32::loadVsyscalls(RSIM_Process *process) {}

void
RSIM_Linux32::loadSpecimenNative(RSIM_Process *process, Disassembler *disassembler, int existingPid/*=-1*/) {
    TODO("[Robb P. Matzke 2015-06-03]");
}

PtRegs
RSIM_Linux32::initialRegistersArch(RSIM_Process*) {
    PtRegs regs;
    regs.sp = 0xc0000000ul;                             // high end of stack, exclusive
    regs.flags = 2;                                     // flag bit 1 is set, although this is a reserved bit
    regs.cs = 0x23;
    regs.ds = 0x2b;
    regs.es = 0x2b;
    regs.ss = 0x2b;
    return regs;
}

rose_addr_t
RSIM_Linux32::pushAuxVector(RSIM_Process *process, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader *fhdr,
                            FILE *trace) {
    static const char *platform = "i686";
    sp -= strlen(platform)+1;
    uint32_t platform_va = sp;
    process->mem_write(platform, platform_va, strlen(platform)+1);

    // This part of the stack is initialized with 16 bytes of random data. We use hard-coded values for
    // reproducibility, and follow a pattern that we might recognized if we see it later.
    static const uint8_t random_data[] = {
        0x00, 0x11, 0x22, 0x33,
        0xff, 0xee, 0xdd, 0xcc,
        0x88, 0x99, 0xaa, 0xbb,
        0x77, 0x66, 0x55, 0x44
    };
    sp -= sizeof random_data;
    uint32_t random_data_va = sp;
    process->mem_write(random_data, random_data_va, sizeof random_data);

    // Initialize stack with auxv, where each entry is two words.  The order and values were determined by running the
    // simulator with the "--show-auxv" switch on hudson-rose-07. These same values appear on my 64-bit development machine
    // when running a program with "i386 -LRB3" (this is Linux 2.6.32-5-amd64). [Robb P. Matzke 2015-06-02]
    auxv_.clear();
    if (vdsoMappedVa()!=0) {
        /* AT_SYSINFO */
        auxv_.push_back(0x20);
        auxv_.push_back(vdsoEntryVa());
        if (trace)
            fprintf(trace, "AT_SYSINFO(0x20):       0x%08" PRIx32"\n", auxv_.back());

        /* AT_SYSINFO_PHDR */
        auxv_.push_back(0x21);
        auxv_.push_back(vdsoMappedVa());
        if (trace)
            fprintf(trace, "AT_SYSINFO_PHDR(0x21):  0x%08" PRIx32"\n", auxv_.back());
    }
    
    /* AT_HWCAP (see linux <include/asm/cpufeature.h>). */
    auxv_.push_back(0x10);
    uint32_t hwcap = 0xbfebfbfful; /* value used by hudson-rose-07, and wortheni(Xeon X5680) */
    auxv_.push_back(hwcap);
    if (trace)
        fprintf(trace, "AT_HWCAP(0x10):         0x%08" PRIx32"\n", auxv_.back());

    /* AT_PAGESZ */
    auxv_.push_back(6);
    auxv_.push_back(PAGE_SIZE);
    if (trace)
        fprintf(trace, "AT_PAGESZ(0x06):        %" PRId32"\n", auxv_.back());

    /* AT_CLKTCK */
    auxv_.push_back(0x11);
    auxv_.push_back(100);
    if (trace)
        fprintf(trace, "AT_CLKTCK(0x11):        %" PRId32"\n", auxv_.back());

    /* AT_PHDR */
    auxv_.push_back(3); /*AT_PHDR*/
    auxv_.push_back(segmentTableVa(fhdr));
    if (trace)
        fprintf(trace, "AT_PHDR(0x03):          0x%08" PRIx32"\n", auxv_.back());

    /*AT_PHENT*/
    auxv_.push_back(4);
    auxv_.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
    if (trace)
        fprintf(trace, "AT_PHENT(0x04):         0x%" PRId32"\n", auxv_.back());

    /* AT_PHNUM */
    auxv_.push_back(5);
    auxv_.push_back(fhdr->get_e_phnum());
    if (trace)
        fprintf(trace, "AT_PHNUM(0x05):         %" PRId32"\n", auxv_.back());

    /* AT_BASE */
    auxv_.push_back(7);
    auxv_.push_back(fhdr->get_section_by_name(".interp") ? interpreterBaseVa() : 0);
    if (trace)
        fprintf(trace, "AT_BASE(0x07):          0x%08" PRIx32"\n", auxv_.back());
        
    /* AT_FLAGS */
    auxv_.push_back(8);
    auxv_.push_back(0);
    if (trace)
        fprintf(trace, "AT_FLAGS(0x08):         0x%08" PRIx32"\n", auxv_.back());

    /* AT_ENTRY */
    auxv_.push_back(9);
    auxv_.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
    if (trace)
        fprintf(trace, "AT_ENTRY(0x09):         0x%08" PRIx32"\n", auxv_.back());

    /* AT_UID */
    auxv_.push_back(0x0b);
    auxv_.push_back(getuid());
    if (trace)
        fprintf(trace, "AT_UID(0x0b):           %" PRId32"\n", auxv_.back());

    /* AT_EUID */
    auxv_.push_back(0x0c);
    auxv_.push_back(geteuid());
    if (trace)
        fprintf(trace, "AT_EUID(0x0c):          %" PRId32"\n", auxv_.back());

    /* AT_GID */
    auxv_.push_back(0x0d);
    auxv_.push_back(getgid());
    if (trace)
        fprintf(trace, "AT_GID(0x0d):           %" PRId32"\n", auxv_.back());

    /* AT_EGID */
    auxv_.push_back(0x0e);
    auxv_.push_back(getegid());
    if (trace)
        fprintf(trace, "AT_EGID(0x0e):          %" PRId32"\n", auxv_.back());

    /* AT_SECURE */
    auxv_.push_back(23); /* 0x17 */
    auxv_.push_back(false);
    if (trace)
        fprintf(trace, "AT_SECURE(0x17):        %" PRId32"\n", auxv_.back());

    /* AT_RANDOM */
    auxv_.push_back(0x19);
    auxv_.push_back(random_data_va);
    if (trace)
        fprintf(trace, "AT_RANDOM(0x19):        0x%08" PRIx32"\n", auxv_.back());

    /* AT_EXECFN */
    auxv_.push_back(0x1f);
    auxv_.push_back(execfn_va);
    if (trace)
        fprintf(trace, "AT_EXECFN(0x1f):        0x%08" PRIx32" (%s)\n", auxv_.back(), exeArgs()[0].c_str());

    /* AT_PLATFORM */
    auxv_.push_back(0x0f);
    auxv_.push_back(platform_va);
    if (trace)
        fprintf(trace, "AT_PLATFORM(0x0f):      0x%08" PRIx32" (%s)\n", auxv_.back(), platform);

    /* AT_NULL */
    auxv_.push_back(0);
    auxv_.push_back(0);

    // write auxv pairs
    ASSERT_require(4 == sizeof(auxv_[0]));
    sp -= 4 * auxv_.size();
    sp &= ~0xf; /*align to 16 bytes*/
    process->mem_write(&auxv_[0], sp, 4*auxv_.size());

    return sp;
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_waitpid_enter(RSIM_Thread *t, int callno)
{
    static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
    t->syscall_enter("waitpid").d().p().f(wflags);
}

void
RSIM_Linux32::syscall_waitpid_body(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    uint32_t status_va=t->syscall_arg(1);
    int options=t->syscall_arg(2);
    int sys_status;
    int result = waitpid(pid, &sys_status, options);
    if (result == -1) {
        result = -errno;
    } else if (status_va) {
        uint32_t status_le;
        ByteOrder::host_to_le(sys_status, &status_le);
        size_t nwritten = t->get_process()->mem_write(&status_le, status_va, 4);
        ROSE_ASSERT(4==nwritten);
    }
    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_waitpid_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(4, print_exit_status_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_time_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("time").p();
}

void
RSIM_Linux32::syscall_time_body(RSIM_Thread *t, int callno)
{
    time_t result = time(NULL);
    if (t->syscall_arg(0)) {
        uint32_t t_le;
        ByteOrder::host_to_le(result, &t_le);
        size_t nwritten = t->get_process()->mem_write(&t_le, t->syscall_arg(0), 4);
        ROSE_ASSERT(4==nwritten);
    }
    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_time_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().t();
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_utime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("utime").s().p();
}

void
RSIM_Linux32::syscall_utime_body(RSIM_Thread *t, int callno)
{
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    // Check to see if times is NULL
    uint8_t byte;
    size_t nread = t->get_process()->mem_read(&byte, t->syscall_arg(1), 1);
    ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

    int result;
    if( byte) {
        struct kernel_utimebuf {
            uint32_t actime;
            uint32_t modtime;
        };

        kernel_utimebuf ubuf;
        size_t nread = t->get_process()->mem_read(&ubuf, t->syscall_arg(1), sizeof(kernel_utimebuf));
        ROSE_ASSERT(nread == sizeof(kernel_utimebuf));

        utimbuf ubuf64;
        ubuf64.actime  = ubuf.actime;
        ubuf64.modtime = ubuf.modtime;

        result = utime(filename.c_str(), &ubuf64);

    } else {
        result = utime(filename.c_str(), NULL);
    }
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_ioctl_enter(RSIM_Thread *t, int callno)
{
    uint32_t cmd=t->syscall_arg(1);
    switch (cmd) {
        case TCGETS:
            t->syscall_enter("ioctl").d().f(ioctl_commands).p();
            break;
        case TCSETSW:
        case TCSETSF:
        case TCSETS:
            t->syscall_enter("ioctl").d().f(ioctl_commands).P(sizeof(termios_32), print_termios_32);
            break;
        case TCGETA:
        case TIOCGPGRP:
        case TIOCSPGRP:
        case TIOCGWINSZ:
            t->syscall_enter("ioctl").d().f(ioctl_commands).d();
            break;
        case TIOCSWINSZ:
            t->syscall_enter("ioctl").d().f(ioctl_commands).P(sizeof(winsize_32), print_winsize_32);
            break;
        default:
            t->syscall_enter("ioctl").d().f(ioctl_commands).d();
            break;
    }
}

void
RSIM_Linux32::syscall_ioctl_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint32_t cmd = t->syscall_arg(1);

    switch (cmd) {
        case TCGETS: { /* 0x00005401, tcgetattr*/
            termios_native host_ti;
            int result = syscall(SYS_ioctl, hostFd, cmd, &host_ti);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }
            termios_32 guest_ti;
            guest_ti.c_iflag = host_ti.c_iflag;
            guest_ti.c_oflag = host_ti.c_oflag;
            guest_ti.c_cflag = host_ti.c_cflag;
            guest_ti.c_lflag = host_ti.c_lflag;
            guest_ti.c_line = host_ti.c_line;
            for (int i=0; i<19; i++)
                guest_ti.c_cc[i] = host_ti.c_cc[i];
            if (sizeof(guest_ti)!=t->get_process()->mem_write(&guest_ti, t->syscall_arg(2), sizeof guest_ti)) {
                t->syscall_return(-EFAULT);
                break;
            }
            t->syscall_return(result);
            break;
        }

        case TCSETSW:   /* 0x00005403 */
        case TCSETSF:
        case TCSETS: {  /* 0x00005402 */
            termios_32 guest_ti;
            if (sizeof(guest_ti)!=t->get_process()->mem_read(&guest_ti, t->syscall_arg(2), sizeof guest_ti)) {
                t->syscall_return(-EFAULT);
                break;
            }
            termios_native host_ti;
            host_ti.c_iflag = guest_ti.c_iflag;
            host_ti.c_oflag = guest_ti.c_oflag;
            host_ti.c_cflag = guest_ti.c_cflag;
            host_ti.c_lflag = guest_ti.c_lflag;
            host_ti.c_line = guest_ti.c_line;
            for (int i=0; i<19; i++)
                host_ti.c_cc[i] = guest_ti.c_cc[i];
            int result = syscall(SYS_ioctl, hostFd, cmd, &host_ti);
            t->syscall_return(-1==result?-errno:result);
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

            int result = ioctl(hostFd, TCGETA, &to);
            if (-1==result) {
                result = -errno;
            } else {
                size_t nwritten = t->get_process()->mem_write(&to, t->syscall_arg(2), sizeof to);
                ROSE_ASSERT(nwritten==sizeof to);
            }

            t->syscall_return(result);
            break;
        }

        case TIOCGPGRP: { /* 0x0000540F, tcgetpgrp*/
            /* equivalent to 
                pid_t tcgetpgrp(int fd);
               The  function tcgetpgrp() returns the process group ID of the foreground process group 
               on the terminal associated to fd, which must be the controlling terminal of the calling 
               process.
            */

            pid_t pgrp = tcgetpgrp(hostFd);
            if (-1==pgrp) {
                t->syscall_return(-errno);
                break;
            }
            uint32_t pgrp_le;
            ByteOrder::host_to_le(pgrp, &pgrp_le);
            size_t nwritten = t->get_process()->mem_write(&pgrp_le, t->syscall_arg(2), 4);
            ROSE_ASSERT(4==nwritten);
            t->syscall_return(pgrp);
            break;
        }

        case TIOCSPGRP: { /* 0x5410, tcsetpgrp*/
            uint32_t pgid_le;
            size_t nread = t->get_process()->mem_read(&pgid_le, t->syscall_arg(2), 4);
            ROSE_ASSERT(4==nread);
            pid_t pgid = ByteOrder::le_to_host(pgid_le);
            int result = tcsetpgrp(hostFd, pgid);
            if (-1==result)
                result = -errno;
            t->syscall_return(result);
            break;
        }

        case TIOCSWINSZ: { /* 0x5413, the winsize is const */
            winsize_32 guest_ws;
            if (sizeof(guest_ws)!=t->get_process()->mem_read(&guest_ws, t->syscall_arg(2), sizeof guest_ws)) {
                t->syscall_return(-EFAULT);
                break;
            }

            winsize_native host_ws;
            host_ws.ws_row = guest_ws.ws_row;
            host_ws.ws_col = guest_ws.ws_col;
            host_ws.ws_xpixel = guest_ws.ws_xpixel;
            host_ws.ws_ypixel = guest_ws.ws_ypixel;

            int result = syscall(SYS_ioctl, hostFd, cmd, &host_ws);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case TIOCGWINSZ: /* 0x5414, */ {
            winsize_native host_ws;
            int result = syscall(SYS_ioctl, hostFd, cmd, &host_ws);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            winsize_32 guest_ws;
            guest_ws.ws_row = host_ws.ws_row;
            guest_ws.ws_col = host_ws.ws_col;
            guest_ws.ws_xpixel = host_ws.ws_xpixel;
            guest_ws.ws_ypixel = host_ws.ws_ypixel;
            if (sizeof(guest_ws)!=t->get_process()->mem_write(&guest_ws, t->syscall_arg(2), sizeof guest_ws)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 0x82187201: /* VFAT_IOCTL_READDIR_BOTH */
            /* FIXME: For now assume that we have no vfat filesystems mounted. */
            t->syscall_return(-ENOTTY);
            break;

        default: {
            fprintf(stderr, "  unhandled ioctl: %u\n", cmd);
            abort();
        }
    }
}

void
RSIM_Linux32::syscall_ioctl_leave(RSIM_Thread *t, int callno)
{
    uint32_t cmd=t->syscall_arg(1);
    switch (cmd) {
        case TCGETS:
            t->syscall_leave().ret().arg(2).P(sizeof(termios_32), print_termios_32);
            break;
        case TIOCGWINSZ:
            t->syscall_leave().ret().arg(2).P(sizeof(winsize_32), print_winsize_32);
            break;
        default:
            t->syscall_leave().ret();
            break;
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_setrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("setrlimit").f(rlimit_resources).P(8, print_rlimit_32);
}

void
RSIM_Linux32::syscall_setrlimit_body(RSIM_Thread *t, int callno)
{
    int resource = t->syscall_arg(0);
    uint32_t rlimit_va = t->syscall_arg(1);
    uint32_t rlimit_guest[2];
    size_t nread = t->get_process()->mem_read(rlimit_guest, rlimit_va, sizeof rlimit_guest);
    ROSE_ASSERT(nread==sizeof rlimit_guest);
    struct rlimit rlimit_native;
    rlimit_native.rlim_cur = rlimit_guest[0];
    rlimit_native.rlim_max = rlimit_guest[1];
    int result = setrlimit(resource, &rlimit_native);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_ugetrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("ugetrlimit").f(rlimit_resources).p();
}

void
RSIM_Linux32::syscall_ugetrlimit_body(RSIM_Thread *t, int callno)
{
    syscall_getrlimit_body(t, callno);
}

void
RSIM_Linux32::syscall_ugetrlimit_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(8, print_rlimit_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getrlimit").f(rlimit_resources).p();
}

void
RSIM_Linux32::syscall_getrlimit_body(RSIM_Thread *t, int callno)
{
    int resource = t->syscall_arg(0);
    uint32_t rlimit_va = t->syscall_arg(1);
    struct rlimit rlimit_native;
    int result = getrlimit(resource, &rlimit_native);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    uint32_t rlimit_guest[2];
    rlimit_guest[0] = rlimit_native.rlim_cur;
    rlimit_guest[1] = rlimit_native.rlim_max;
    if (8!=t->get_process()->mem_write(rlimit_guest, rlimit_va, 8)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_getrlimit_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(8, print_rlimit_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_gettimeofday_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("gettimeofday").p();
}

void
RSIM_Linux32::syscall_gettimeofday_body(RSIM_Thread *t, int callno)
{
    uint32_t tp = t->syscall_arg(0);
    struct timeval host_time;
    struct timeval_32 guest_time;

    int result = gettimeofday(&host_time, NULL);
    if (result == -1) {
        result = -errno;
    } else {
        guest_time.tv_sec = host_time.tv_sec;
        guest_time.tv_usec = host_time.tv_usec;
        if (sizeof(guest_time) != t->get_process()->mem_write(&guest_time, tp, sizeof guest_time))
            result = -EFAULT;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_gettimeofday_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(timeval_32), print_timeval_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_mmap_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mmap").P(sizeof(mmap_arg_struct_32), print_mmap_arg_struct_32);
}

/* See also: syscall_mmap2 */
void
RSIM_Linux32::syscall_mmap_body(RSIM_Thread *t, int callno)
{
    mmap_arg_struct_32 args;
    uint32_t args_va = t->syscall_arg(0);
    if (sizeof(args)!=t->get_process()->mem_read(&args, args_va, sizeof args)) {
        t->syscall_return(-EFAULT);
        return;
    }

    unsigned rose_perms = 0;
    if (0 != (args.prot & PROT_READ))
        rose_perms |= MemoryMap::READABLE;
    if (0 != (args.prot & PROT_WRITE))
        rose_perms |= MemoryMap::WRITABLE;
    if (0 != (args.prot & PROT_EXEC))
        rose_perms |= MemoryMap::EXECUTABLE;

    int hostFd = t->get_process()->hostFileDescriptor(args.fd);
    uint32_t result = t->get_process()->mem_map(args.addr, args.len, rose_perms, args.flags, args.offset, hostFd);
    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_mmap_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().eret().p();
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mmap syscall:\n");
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_statfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("statfs").s().p();
}

void
RSIM_Linux32::syscall_statfs_body(RSIM_Thread *t, int callno)
{
    int result;
    statfs_32 guest_statfs;
    bool error;
    std::string path = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    result = syscall(SYS_statfs64 , path.c_str(), sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else               /* host is 64-bit machine */
    static statfs_native host_statfs;
    result = syscall(SYS_statfs, path.c_str(), &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif

    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(1), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_statfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(statfs_32), print_statfs_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_fstatfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fstatfs").d().p();
}

void
RSIM_Linux32::syscall_fstatfs_body(RSIM_Thread *t, int callno)
{
    int result;
    statfs_32 guest_statfs;
#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    result = syscall(SYS_fstatfs64, hostFd, sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else               /* host is 64-bit machine */
    static statfs_native host_statfs;
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    result = syscall(SYS_fstatfs, hostFd, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif

    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(1), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_fstatfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(statfs_32), print_statfs_32);
}

/*******************************************************************************************************************************/

/** Returns the first control message header if the control message buffer is large enough to contain a header.  The returned
 * header is therefore always a complete header, but the control message buffer might not be large enough to contain the
 * ancillary data that follows the header.  */
template<class ControlHeader>
static ControlHeader *cmsg_first(void *control, size_t controllen) {
    assert(control!=NULL || 0==controllen);
    return sizeof(ControlHeader)<=controllen ? (ControlHeader*)control : NULL;
}
template<class ControlHeader>
static const ControlHeader *cmsg_first(const void *control, size_t controllen) {
    assert(control!=NULL || 0==controllen);
    return sizeof(ControlHeader)<=controllen ? (const ControlHeader*)control : NULL;
}

/** Returns the next control message header if the control message buffer is large enough to contain a header.  The returned
 * header is therefore always a complete header, but the control message buffer might not be large enough to contain the
 * ancillary data that follows the header. */
template<class ControlHeader>
static ControlHeader *cmsg_next(void *control, size_t controllen, ControlHeader *cur) {
    if (!cur) return NULL;
    assert(control!=NULL && (uint8_t*)cur>=(uint8_t*)control);
    assert(cur->cmsg_len >= sizeof(ControlHeader));
    size_t offset = (uint8_t*)cur - (uint8_t*)control + alignUp((size_t)cur->cmsg_len, sizeof(cur->cmsg_len));
    return offset+sizeof(ControlHeader)<=controllen ? (ControlHeader*)((uint8_t*)control+offset) : NULL;
}
template<class ControlHeader>
static const ControlHeader *cmsg_next(const void *control, size_t controllen, const ControlHeader *cur) {
    if (!cur) return NULL;
    assert(control!=NULL && (const uint8_t*)cur>=(const uint8_t*)control);
    assert(cur->cmsg_len >= sizeof(ControlHeader));
    size_t offset = (const uint8_t*)cur - (const uint8_t*)control + alignUp((size_t)cur->cmsg_len, sizeof(cur->cmsg_len));
    return offset+sizeof(ControlHeader)<=controllen ? (const ControlHeader*)((const uint8_t*)control+offset) : NULL;
}

/** Returns true if the control buffer is large enough to contain the control message's ancillary data. */
template<class ControlHeader>
static bool cmsg_payload_complete(const void *control, size_t controllen, const ControlHeader *cur) {
    assert(cur && (uint8_t*)cur>=(const uint8_t*)control);
    return (size_t)((uint8_t*)cur-(const uint8_t*)control) + cur->cmsg_len <= controllen;
}

/** Count the number of control message headers in the control buffer.  The buffer might not be large enough to contain all the
 *  ancillary data of the last mesage. */
template<class ControlHeader>
static size_t cmsg_nmessages(const void *control, size_t controllen) {
    size_t count=0;
    for (ControlHeader *ctl=cmsg_first<ControlHeader>(control, controllen);
         ctl!=NULL;
         ctl=cmsg_next<ControlHeader>(control, controllen, ctl)) {
        count++;
    }
    return count;
}

/** Returns the size of the ancillary data for a control message.  The payload might be larger than the control message
 *  buffer. */
template<class ControlHeader>
static size_t cmsg_payload_size(const ControlHeader *ctl) {
    assert(ctl!=NULL);
    assert(ctl->cmsg_len >= sizeof(ControlHeader));
    return ctl->cmsg_len - sizeof(ControlHeader);
}

/** Returns the size of the ancillary data for a control message, limited by the size of the control message buffer. */
template<class ControlHeader>
static size_t cmsg_payload_size(const void *control, size_t controllen, const ControlHeader *ctl) {
    assert(ctl!=NULL && control!=NULL);
    assert((uint8_t*)ctl >= (const uint8_t*)control && (uint8_t*)(ctl+1) <= (const uint8_t*)control+controllen);
    size_t payload_offset = (uint8_t*)(ctl+1) - (const uint8_t*)control;
    size_t payload_size = cmsg_payload_size(ctl);
    size_t payload_avail = controllen - payload_offset;
    return std::min(payload_size, payload_avail);
}

/** Return the sizeof the buffer needed to hold control messages.  If the payload of the last message is trunctated in the
 * source, then it will also be truncated in the destination. */
template<class SourceType, class DestinationType>
static size_t cmsg_needed(const void *control, size_t controllen)
{
    size_t retval = 0;
    static const DestinationType *dst = NULL; // only used for sizeof(dst->cmsg_len)
    for (const SourceType *ctl=cmsg_first<SourceType>(control, controllen);
         ctl!=NULL;
         ctl=cmsg_next<SourceType>(control, controllen, ctl)) {
        retval = alignUp(retval, sizeof(dst->cmsg_len));
        retval += sizeof(DestinationType) + cmsg_payload_size(control, controllen, ctl);
    }
    return retval;
}

template<class ControlHeader>
static void cmsg_dump(const void *control, size_t controllen, Sawyer::Message::Stream &f, const std::string prefix="")
{
    std::string hexdump_prefix = prefix + "            ";
    HexdumpFormat fmt;
    fmt.prefix = hexdump_prefix.c_str();
    fmt.addr_fmt = "0x%02x:";
    fmt.multiline = true;
    fmt.pad_chars = false;

    size_t i = 0;
    for (const ControlHeader *ctl=cmsg_first<ControlHeader>(control, controllen);
         ctl!=NULL;
         ctl=cmsg_next<ControlHeader>(control, controllen, ctl)) {
        size_t payload_advertised_len = cmsg_payload_size(ctl);
        size_t payload_actual_len = cmsg_payload_size(control, controllen, ctl);
        mfprintf(f)("%smessage #%zd: size: total=%zu; payload=%zu; actual_payload=%zu\n",
                    prefix.c_str(), i++, (size_t)ctl->cmsg_len, payload_advertised_len, payload_actual_len);
        SgAsmExecutableFileFormat::hexdump(f, 0, (const unsigned char*)(ctl+1), payload_actual_len, fmt);
    }
}

/** Copy the source control message to the destination control message. If the source payload is truncated then the destination
 *  payload will also be truncated. */
template<class SourceType, class DestinationType>
static void cmsg_copy1(const void *src_control, size_t src_controllen, const SourceType *src_ctl,
                       void *dst_control, size_t dst_controllen, DestinationType *dst_ctl)
{
    assert(src_ctl && src_control &&
           (const uint8_t*)src_ctl>=(const uint8_t*)src_control &&
           (const uint8_t*)(src_ctl+1)<=(const uint8_t*)src_control+src_controllen);
    assert(dst_ctl && dst_control &&
           (uint8_t*)dst_ctl>=(uint8_t*)dst_control &&
           (uint8_t*)(dst_ctl+1)<=(uint8_t*)dst_control+dst_controllen);
    dst_ctl->cmsg_len = sizeof(DestinationType) + cmsg_payload_size(src_ctl);
    dst_ctl->cmsg_level = src_ctl->cmsg_level;
    dst_ctl->cmsg_type = src_ctl->cmsg_type;

    size_t to_copy = std::min(cmsg_payload_size(src_control, src_controllen, src_ctl),
                              cmsg_payload_size(dst_control, dst_controllen, dst_ctl));
    memcpy(dst_ctl+1, src_ctl+1, to_copy);
}

/** Copy a guest msghdr_32.msg_control array to a host msghdr.msg_control array, allocating space and initializing the host's
 *  relevant members in its msghdr struct.  Returns zero on success, negative errno on failure. */
static int
cmsg_copy_all(RSIM_Thread *t, const msghdr_32 &guest, msghdr &host)
{
    int retval = 0;
    host.msg_control = NULL;
    host.msg_controllen = 0;

    /* Read the guest control messages */
    void *guest_control = new uint8_t[guest.msg_controllen];
    size_t nread = t->get_process()->mem_read(guest_control, guest.msg_control, guest.msg_controllen);
    if (nread !=guest.msg_controllen)
        retval = -EFAULT;

    /* Allocate the host buffer */
    host.msg_controllen = cmsg_needed<cmsghdr_32, cmsghdr>(guest_control, guest.msg_controllen);
    if (0==host.msg_controllen)
        return 0;
    host.msg_control = new uint8_t[host.msg_controllen];

    /* Copy each message from guest to host */
    cmsghdr_32 *guest_cmsg = cmsg_first<cmsghdr_32>(guest_control,    guest.msg_controllen);
    cmsghdr    *host_cmsg  = cmsg_first<cmsghdr   >(host.msg_control, host.msg_controllen);
    while (guest_cmsg) {
        if (!cmsg_payload_complete(guest_control, guest.msg_controllen, guest_cmsg)) {
            retval = -EFAULT;
            break;
        }
        cmsg_copy1(guest_control,    guest.msg_controllen, guest_cmsg,
                   host.msg_control, host.msg_controllen,  host_cmsg);
                  
        guest_cmsg = cmsg_next(guest_control,    guest.msg_controllen, guest_cmsg);
        host_cmsg  = cmsg_next(host.msg_control, host.msg_controllen,  host_cmsg);
    }

    if (retval) {
        delete[] (uint8_t*)guest_control;
        delete[] (uint8_t*)host.msg_control;
        host.msg_control = NULL;
        host.msg_controllen = 0;
    }
    return retval;
}

/** Copy host msghdr.msg_control to the guest.  Returns zero on success, negative errno on failure.  Truncation of a message
 *  due to the guest not supplying a large enough buffer is not an error, but rather results in setting the MSG_CTRUNC flag
 *  in the msghdr_32 struct. */
static int
cmsg_copy_all(RSIM_Thread *t, const msghdr &host, msghdr_32 &guest)
{
    int retval = 0;

    size_t guest_controllen = cmsg_needed<cmsghdr, cmsghdr_32>(host.msg_control, host.msg_controllen);
    if (0==guest_controllen) {
        guest.msg_controllen = 0;
        return 0;
    }
    
    guest_controllen = std::min(guest_controllen, (size_t)guest.msg_controllen);
    void *guest_control = new uint8_t[guest_controllen];

    cmsghdr_32 *guest_cmsg = cmsg_first<cmsghdr_32>(guest_control,    guest_controllen);
    cmsghdr    *host_cmsg  = cmsg_first<cmsghdr   >(host.msg_control, host.msg_controllen);
    while (host_cmsg) {
        assert(guest_cmsg);
        cmsg_copy1(host.msg_control, host.msg_controllen, host_cmsg,
                   guest_control,    guest_controllen,    guest_cmsg);
        if (!cmsg_payload_complete(guest_control, guest_controllen, guest_cmsg))
            guest.msg_flags |= MSG_CTRUNC;
        guest_cmsg = cmsg_next(guest_control, guest_controllen, guest_cmsg);
        host_cmsg = cmsg_next(host.msg_control, host.msg_controllen, host_cmsg);
    }

    size_t nwritten = t->get_process()->mem_write(guest_control, guest.msg_control, guest_controllen);
    guest.msg_controllen = nwritten;
    if (nwritten<guest_controllen) {
        guest.msg_flags |= MSG_CTRUNC;
        retval = -EFAULT;
    }

    delete[] (uint8_t*)guest_control;
    return retval;
}

void
RSIM_Linux32::syscall_socketcall_enter(RSIM_Thread *t, int callno)
{
    uint32_t a[6];
    switch (t->syscall_arg(0)) {
        case 1: /* SYS_SOCKET */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "socket").f(protocol_families).f(socket_types).f(socket_protocols);
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 2: /* SYS_BIND */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "bind").d().p().d(); // FIXME: we could do a better job printing the address RPM 2011-01-04
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 3: /* SYS_CONNECT */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "connect").d().p().d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 4: /* SYS_LISTEN */
            if (8==t->get_process()->mem_read(a, t->syscall_arg(1), 8)) {
                t->syscall_enter(a, "listen").d().d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 5:/*SYS_ACCEPT*/
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "accept").d().p().p();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 8: /* SYS_SOCKETPAIR */
            if (16==t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_enter(a, "socketpair").d().d().d().p();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 10: /* SYS_RECV */
            if (16==t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_enter(a, "recv").d().p().d().d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 14:/*SYS_SETSOCKOPT*/
            if (20==t->get_process()->mem_read(a, t->syscall_arg(1), 20)) {
                t->syscall_enter(a, "setsockopt").d().d().d().p().d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 16: /* SYS_SENDMSG */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "sendmsg").d().P(sizeof(msghdr_32), print_msghdr_32).d();
                Sawyer::Message::Stream trace(t->tracing(TRACE_SYSCALL));
                msghdr_32 msghdr;
                if (trace && sizeof(msghdr)==t->get_process()->mem_read(&msghdr, a[1], sizeof msghdr)) {
                    for (unsigned i=0; i<msghdr.msg_iovlen && i<100; i++) {
                        uint32_t vasz[2];
                        if (8==t->get_process()->mem_read(vasz, msghdr.msg_iov+i*8, 8)) {
                            uint8_t *buf = new uint8_t[vasz[1]];
                            if (vasz[1]==t->get_process()->mem_read(buf, vasz[0], vasz[1])) {
                                mfprintf(trace)("\n    iov #%u: ", i);
                                Printer::print_buffer(trace, vasz[0], buf, vasz[1], 1024);
                                mfprintf(trace)("; %" PRIu32" byte%s", vasz[1], 1==vasz[1]?"":"s");
                            } else {
                                mfprintf(trace)("\n    iov #%u: short read of data", i);
                            }
                            delete[] buf;
                        } else {
                            mfprintf(trace)("\n    iov #%u: short read of iov", i);
                            break;
                        }
                    }
                    if (msghdr.msg_iovlen>100)
                        mfprintf(trace)("\n    iov ... (%zu in total)\n", (size_t)msghdr.msg_iovlen);
                    if (msghdr.msg_iovlen>0)
                        trace <<"\n";

                    if (msghdr.msg_controllen>0 && msghdr.msg_controllen<4096) {
                        uint8_t control[msghdr.msg_controllen];
                        size_t nread = t->get_process()->mem_read(control, msghdr.msg_control, msghdr.msg_controllen);
                        if (nread==msghdr.msg_controllen)
                            cmsg_dump<cmsghdr_32>((void*)control, msghdr.msg_controllen, trace, std::string(16, ' '));
                    }
                }
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 17: /* SYS_RECVMSG */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "recvmsg").d().P(sizeof(msghdr_32), print_msghdr_32).d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        case 18:/*SYS_ACCEPT4*/
            if (16==t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_enter(a, "accept4").d().p().p().d();
            } else {
                t->syscall_enter("socketcall").f(socketcall_commands).p();
            }
            break;
        default:
            t->syscall_enter("socketcall").f(socketcall_commands).p();
            break;
    }
}

static void
sys_socketpair(RSIM_Thread *t, int family, int type, int protocol, uint32_t sockvec_va)
{
    int hostSockets[2];
    int guestSockets[2];
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[4];
    a[0] = family;
    a[1] = type;
    a[2] = protocol;
    a[3] = (int)hostSockets;
    int result = syscall(SYS_socketcall, 8/*SYS_SOCKETPAIR*/, a);
#else /* amd64 */
    int result = syscall(SYS_socketpair, family, type, protocol, hostSockets);
#endif
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    assert(8==sizeof guestSockets);
    guestSockets[0] = t->get_process()->allocateGuestFileDescriptor(hostSockets[0]);
    guestSockets[1] = t->get_process()->allocateGuestFileDescriptor(hostSockets[1]);
    if (sizeof(guestSockets)!=t->get_process()->mem_write(guestSockets, sockvec_va, sizeof guestSockets)) {
        close(hostSockets[0]);
        close(hostSockets[1]);
        t->get_process()->eraseGuestFileDescriptor(guestSockets[0]);
        t->get_process()->eraseGuestFileDescriptor(guestSockets[1]);
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
sys_recvfrom(RSIM_Thread *t, int guestFd, uint32_t buf_va, uint32_t buf_sz, int flags, uint32_t addr_va, uint32_t addr_sz)
{
    assert(0==addr_va); /* we don't handle recvfrom yet, only recv. */
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);

    char *buf = new char[buf_sz];
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[6];
    a[0] = hostFd;
    a[1] = (uint32_t)buf;
    a[2] = buf_sz;
    a[3] = flags;
    a[4] = 0;
    a[5] = 0;
    int result = syscall(SYS_socketcall, 10/*SYS_RECV*/, a);
#else /* amd64 */
    int result = syscall(SYS_recvfrom, hostFd, buf, buf_sz, flags, 0, 0);
#endif
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    size_t nbytes = (size_t)result;
    assert(nbytes <= buf_sz);
    if (nbytes!=t->get_process()->mem_write(buf, buf_va, nbytes)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
sys_sendmsg(RSIM_Thread *t, int guestFd, uint32_t msghdr_va, int flags)
{
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int retval = 0;
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));

    /* Read guest information */
    msghdr_32 guest;
    if (sizeof(msghdr_32)!=t->get_process()->mem_read(&guest, msghdr_va, sizeof guest)) {
        t->syscall_return(-EFAULT);
        return;
    }
    if (guest.msg_iovlen<0 || guest.msg_iovlen>1024) {
        t->syscall_return(-EINVAL);
        return;
    }
    assert(0==guest.msg_namelen);       /* FIXME: not implemented yet [RPM 2011-06-14] */

    if (0==guest.msg_iovlen) {
        t->syscall_return(0);
        return;
    }
    iovec_32 *guest_iov = new iovec_32[guest.msg_iovlen];
    size_t guest_iov_sz = guest.msg_iovlen * sizeof(iovec_32);
    if (guest_iov_sz!=t->get_process()->mem_read(guest_iov, guest.msg_iov, guest_iov_sz)) {
        strace <<"<segfault reading iov>";
        retval = -EFAULT;
    }

    /* Build the host data structure */
    msghdr host;
    memset(&host, 0, sizeof host);
    if (0==retval) {
        host.msg_name = NULL;
        host.msg_namelen = 0;
        host.msg_flags = guest.msg_flags;
        host.msg_iovlen = guest.msg_iovlen;
        host.msg_iov = new iovec[guest.msg_iovlen];
        memset(host.msg_iov, 0, guest.msg_iovlen*sizeof(iovec));
        for (unsigned i=0; i<guest.msg_iovlen; i++) {
            host.msg_iov[i].iov_len = guest_iov[i].iov_len;
            host.msg_iov[i].iov_base = new uint8_t[guest_iov[i].iov_len];
            size_t nread = t->get_process()->mem_read(host.msg_iov[i].iov_base, guest_iov[i].iov_base, guest_iov[i].iov_len);
            if (nread<guest_iov[i].iov_len) {
                retval = -EFAULT;
                break;
            }
        }
        cmsg_copy_all(t, guest, host);
    }

    /* The real syscall */
    if (0==retval) {
        retval = sendmsg(hostFd, &host, flags);
        if (-1==retval)
            retval = -errno;
    }

    /* Clean up */
    if (host.msg_iov) {
        for (unsigned i=0; i<host.msg_iovlen; i++)
            delete[] (uint8_t*)host.msg_iov[i].iov_base;
        delete[] host.msg_iov;
    }
    delete[] (uint8_t*)host.msg_control;
    delete[] guest_iov;

    t->syscall_return(retval);
}

static void
sys_recvmsg(RSIM_Thread *t, int guestFd, uint32_t msghdr_va, int flags)
{
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int retval = 0;
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));

    /* Read guest information */
    msghdr_32 guest;
    if (sizeof(msghdr_32)!=t->get_process()->mem_read(&guest, msghdr_va, sizeof guest)) {
        t->syscall_return(-EFAULT);
        return;
    }
    if (guest.msg_iovlen<0 || guest.msg_iovlen>1024) {
        t->syscall_return(-EINVAL);
        return;
    }
    assert(0==guest.msg_namelen); /* not implemented yet [RPM 2011-04-26] */
    iovec_32 *guest_iov = new iovec_32[guest.msg_iovlen];
    size_t guest_iov_sz = guest.msg_iovlen * sizeof(iovec_32);
    memset(guest_iov, 0, guest_iov_sz);
    if (guest_iov_sz!=t->get_process()->mem_read(guest_iov, guest.msg_iov, guest_iov_sz)) {
        strace <<"<segfault reading iov>";
        retval = -EFAULT;
    }

    /* Copy to host */
    msghdr host;
    memset(&host, 0, sizeof host);
    if (0==retval && guest.msg_controllen>0) {
        host.msg_controllen = guest.msg_controllen + (guest.msg_controllen/sizeof(cmsghdr_32))*4; // estimate
        host.msg_control = new uint8_t[host.msg_controllen];
        memset(host.msg_control, 0, host.msg_controllen);
    }
    if (0==retval && (host.msg_iovlen = guest.msg_iovlen) > 0) {
        host.msg_iov = new iovec[guest.msg_iovlen];
        for (unsigned i=0; i<host.msg_iovlen; i++) {
            unsigned n = host.msg_iov[i].iov_len = guest_iov[i].iov_len;
            host.msg_iov[i].iov_base = new uint8_t[n];
        }
    }

    /* Make the real call */
    if (0==retval) {
        retval = recvmsg(hostFd, &host, flags);
        if (-1==retval)
            retval = -errno;
    }

    /* Copy iov data back into the guest */
    if (retval>0) {
        size_t to_copy = retval;
        for (unsigned idx=0; idx<host.msg_iovlen && to_copy>0; idx++) {
            size_t n = std::min(to_copy, host.msg_iov[idx].iov_len);
            to_copy -= n;
            if (n!=t->get_process()->mem_write(host.msg_iov[idx].iov_base, guest_iov[idx].iov_base, n)) {
                retval = -EFAULT;
                break;
            }
        }
    }

    /* Copy into the guest */
    if (retval>=0) {
        guest.msg_flags = host.msg_flags;
        if (cmsg_copy_all(t, host, guest)<0)
            retval = -EFAULT;
        if (sizeof(guest)!=t->get_process()->mem_write(&guest, msghdr_va, sizeof guest))
            retval = -EFAULT;
    }

    /* Cleanup */
    for (unsigned idx=0; idx<host.msg_iovlen; idx++)
        delete[] (iovec*)host.msg_iov[idx].iov_base;
    delete[] host.msg_iov;
    delete[] (uint8_t*)host.msg_control;
    delete[] guest_iov;

    t->syscall_return(retval);
}

void
RSIM_Linux32::syscall_socketcall_body(RSIM_Thread *t, int callno)
{
    /* Return value is written to eax by these helper functions. The structure of this code closely follows that in the
     * Linux kernel. See linux/net/socket.c. */
    uint32_t a[6];
    switch (t->syscall_arg(0)) {
        case 1: { /* SYS_SOCKET */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_socket_helper(t, a[0], a[1], a[2]);
            }
            break;
        }
                    
        case 2: { /* SYS_BIND */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_bind_helper(t, a[0], a[1], a[2]);
            }
            break;
        }

        case 3: { /* SYS_CONNECT */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_connect_helper(t, a[0], a[1], a[2]);
            }
            break;
        }

        case 4: { /* SYS_LISTEN */
            if (8!=t->get_process()->mem_read(a, t->syscall_arg(1), 8)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_listen_helper(t, a[0], a[1]);
            }
            break;
        }

        case 5: { /* SYS_ACCEPT */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_accept_helper(t, a[0], a[1], a[2], 0);
            }
            break;
        }
            
        case 8: { /* SYS_SOCKETPAIR */
            if (16!=t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_socketpair(t, a[0], a[1], a[2], a[3]);
            }
            break;
        }

        case 10: { /* SYS_RECV */
            if (16!=t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_recvfrom(t, a[0], a[1], a[2], a[3], 0, 0);
            }
            break;
        }

        case 14: { /* SYS_SETSOCKOPT */
            if (20!=t->get_process()->mem_read(a, t->syscall_arg(1), 20)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_setsockopt_helper(t, a[0], a[1], a[2], a[3], a[4]);
            }
            break;
        }
            
        case 16: {/* SYS_SENDMSG */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_sendmsg(t, a[0], a[1], a[2]);
            }
            break;
        }
            
        case 17: { /* SYS_RECVMSG */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_recvmsg(t, a[0], a[1], a[2]);
            }
            break;
        }

        case 18: { /* SYS_ACCEPT4 */
            if (16!=t->get_process()->mem_read(a, t->syscall_arg(1), 16)) {
                t->syscall_return(-EFAULT);
            } else {
                syscall_accept_helper(t, a[0], a[1], a[2], a[3]);
            }
            break;
        }
            

        case 6: /* SYS_GETSOCKNAME */
        case 7: /* SYS_GETPEERNAME */
        case 9: /* SYS_SEND */
        case 11: /* SYS_SENDTO */
        case 12: /* SYS_RECVFROM */
        case 13: /* SYS_SHUTDOWN */
        case 15: /* SYS_GETSOCKOPT */
        case 19: /* SYS_RECVMMSG */
            t->syscall_return(-ENOSYS);
            break;
        default:
            t->syscall_return(-EINVAL);
            break;
    }
}

void
RSIM_Linux32::syscall_socketcall_leave(RSIM_Thread *t, int callno)
{
    uint32_t a[7];
    a[0] = t->syscall_arg(-1);

    switch (t->syscall_arg(0)) {
        case 8: /* SYS_SOCKETPAIR */
            if (16==t->get_process()->mem_read(a+1, t->syscall_arg(1), 16)) {
                t->syscall_leave(a).ret().arg(3).P(8, print_int_32);
                return;
            }
            break;
        case 17: /* SYS_RECVMSG */
            if (12==t->get_process()->mem_read(a+1, t->syscall_arg(1), 12)) {
                t->syscall_leave(a).ret().arg(1).P(sizeof(msghdr_32), print_msghdr_32);
                Sawyer::Message::Stream trace(t->tracing(TRACE_SYSCALL));
                msghdr_32 msghdr;
                if (trace &&
                    (int32_t)a[0] > 0 &&
                    sizeof(msghdr)==t->get_process()->mem_read(&msghdr, a[2], sizeof msghdr)) {
                    uint32_t nbytes = a[0];
                    for (unsigned i=0; i<msghdr.msg_iovlen && i<100 && nbytes>0; i++) {
                        uint32_t vasz[2];
                        if (8==t->get_process()->mem_read(vasz, msghdr.msg_iov+i*8, 8)) {
                            uint32_t nused = std::min(nbytes, vasz[1]);
                            nbytes -= nused;
                            uint8_t *buf = new uint8_t[nused];
                            if (vasz[1]==t->get_process()->mem_read(buf, vasz[0], nused)) {
                                mfprintf(trace)("    iov #%u: ", i);
                                Printer::print_buffer(trace, vasz[0], buf, nused, 1024);
                                mfprintf(trace)("; size total=%" PRIu32" used=%" PRIu32"\n", vasz[1], nused);
                            }
                            delete[] buf;
                        }
                    }
                    if (msghdr.msg_iovlen>100)
                        mfprintf(trace)("    iov... (%zu in total)\n", (size_t)msghdr.msg_iovlen);
                    if (msghdr.msg_controllen>0 && msghdr.msg_controllen<4096) {
                        uint8_t control[msghdr.msg_controllen];
                        size_t nread = t->get_process()->mem_read(control, msghdr.msg_control, msghdr.msg_controllen);
                        if (nread==msghdr.msg_controllen)
                            cmsg_dump<cmsghdr_32>((void*)control, msghdr.msg_controllen, trace, std::string(16, ' '));
                    }
                    trace <<"\n";
                }
                return;
            }
            break;
    }

    t->syscall_leave().ret();
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_wait4_enter(RSIM_Thread *t, int callno)
{
    static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
    t->syscall_enter("wait4").d().p().f(wflags).p();
}

void
RSIM_Linux32::syscall_wait4_body(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    uint32_t status_va=t->syscall_arg(1), rusage_va=t->syscall_arg(3);
    int options=t->syscall_arg(2);
    int status;
    struct rusage rusage;
    int result = wait4(pid, &status, options, &rusage);
    if( result == -1) {
        result = -errno;
    } else {
        if (status_va != 0) {
            size_t nwritten = t->get_process()->mem_write(&status, status_va, 4);
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
            size_t nwritten = t->get_process()->mem_write(&out, rusage_va, sizeof out);
            ROSE_ASSERT(nwritten == sizeof out);
        }
    }
    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_wait4_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(4, print_exit_status_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sysinfo_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sysinfo").p();
}

void
RSIM_Linux32::syscall_sysinfo_body(RSIM_Thread *t, int callno)
{
    sysinfo_native hostBuf;
    int result  = syscall(SYS_sysinfo, &hostBuf);

    if (-1==result) {
        t->syscall_return(-errno);
    } else {
        sysinfo_32 guestBuf(hostBuf);
        if (sizeof guestBuf != t->get_process()->mem_write(&guestBuf, t->syscall_arg(0), sizeof guestBuf)) {
            t->syscall_return(-EFAULT);
        } else {
            t->syscall_return(result);
        }
    }
}

void
RSIM_Linux32::syscall_sysinfo_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().ret().P(sizeof(sysinfo_32), print_sysinfo_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_ipc_enter(RSIM_Thread *t, int callno)
{
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t second=t->syscall_arg(2), third=t->syscall_arg(3);
    switch (call) {
        case 1: /*SEMOP*/
            t->syscall_enter("ipc").f(ipc_commands).d().d().unused().p();
            break;
        case 2: /*SEMGET*/
            t->syscall_enter("ipc").f(ipc_commands).d().d().f(ipc_flags);
            break;
        case 3: /*SEMCTL*/
            switch (third & 0xff) {
                case 16: /*SETVAL*/
                    t->syscall_enter("ipc").f(ipc_commands).d().d().f(sem_control).P((size_t)4, print_int_32);
                    break;
                default:
                    t->syscall_enter("ipc").f(ipc_commands).d().d().f(sem_control).p();
                    break;
            }
            break;
        case 4: /*SEMTIMEDOP*/
            t->syscall_enter("ipc").f(ipc_commands).d().d().unused().p().P(sizeof(timespec_32), print_timespec_32);
            break;
        case 11: /*MSGSND*/
            t->syscall_enter("ipc").f(ipc_commands).d().d().f(ipc_flags).b(4+t->syscall_arg(2));
            break;
        case 12: /*MSGRCV*/
            if (0==version) {
                t->syscall_enter("ipc").f(ipc_commands).d().d().f(ipc_flags).P(sizeof(ipc_kludge_32), print_ipc_kludge_32);
            } else {
                t->syscall_enter("ipc").f(ipc_commands).d().d().f(ipc_flags).p().d();
            }
            break;
        case 13: /*MSGGET*/
            t->syscall_enter("ipc").f(ipc_commands).p().f(ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
            break;
        case 14: /*MSGCTL*/
            switch (second & 0xff) {
                case 0: /* IPC_RMID */
                    t->syscall_enter("ipc").f(ipc_commands).d().f(msg_control);
                    break;
                case 1: /* IPC_SET */
                    t->syscall_enter("ipc").f(ipc_commands).d().f(msg_control).unused()
                        .P(sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    break;
                default:
                    t->syscall_enter("ipc").f(ipc_commands).d().f(msg_control).unused().p();
                    break;
            }
            break;
        case 21: /*SHMAT*/
            if (1==version) {
                t->syscall_enter("ipc").f(ipc_commands).d().d().d().p().d();
            } else {
                t->syscall_enter("ipc").f(ipc_commands).d().f(ipc_flags).p().p();
            }
            break;
        case 22: /*SHMDT*/
            t->syscall_enter("ipc").f(ipc_commands).unused().unused().unused().p();
            break;
        case 23: /*SHMGET*/
            t->syscall_enter("ipc").f(ipc_commands).p().d().f(ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
            break;
        case 24: /*SHMCTL*/
            switch (second & 0xff) {
                case 0:         /* IPC_RMID */
                    t->syscall_enter("ipc").f(ipc_commands).d().f(shm_control);
                    break;
                case 1:         /* IPC_SET */
                    t->syscall_enter("ipc").f(ipc_commands).d().f(shm_control).unused()
                        .P(sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    break;
                case 11:        /* SHM_LOCK */
                case 12:        /* SHM_UNLOCK */
                    t->syscall_enter("ipc").f(ipc_commands).d().f(shm_control);
                    break;
                default:
                    t->syscall_enter("ipc").f(ipc_commands).d().f(shm_control).unused().p();
                    break;
            }
            break;
        default:
            t->syscall_enter("ipc").f(ipc_commands).d().d().d().p().d();
            break;
    }
}

static void
sys_semtimedop(RSIM_Thread *t, uint32_t semid, uint32_t sops_va, uint32_t nsops, uint32_t timeout_va)
{
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));

    static const Translate sem_flags[] = {
        TF(IPC_NOWAIT), TF(SEM_UNDO), T_END
    };

    if (nsops<1) {
        t->syscall_return(-EINVAL);
        return;
    }

    /* struct sembuf is the same on both 32- and 64-bit platforms */
    sembuf sops[nsops * sizeof(sembuf)];
    if (nsops*sizeof(sembuf)!=t->get_process()->mem_read(sops, sops_va, nsops*sizeof(sembuf))) {
        t->syscall_return(-EFAULT);
        return;
    }
    for (uint32_t i=0; i<nsops; i++) {
        mfprintf(strace)("    sops[%" PRIu32"] = { num=%" PRIu16", op=%" PRId16", flg=",
                         i, sops[i].sem_num, sops[i].sem_op);
        Printer::print_flags(strace, sem_flags, sops[i].sem_flg);
        strace <<" }\n";
    }

    timespec host_timeout;
    if (timeout_va) {
        timespec_32 guest_timeout;
        if (sizeof(guest_timeout)!=t->get_process()->mem_read(&guest_timeout, timeout_va, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        }
        host_timeout.tv_sec = guest_timeout.tv_sec;
        host_timeout.tv_nsec = guest_timeout.tv_nsec;
    }

    int result = semtimedop(semid, sops, nsops, timeout_va?&host_timeout:NULL);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_semget(RSIM_Thread *t, uint32_t key, uint32_t nsems, uint32_t semflg)
{
#ifdef SYS_ipc /* i686 */
    int result = syscall(SYS_ipc, 2, key, nsems, semflg);
#else
    int result = syscall(SYS_semget, key, nsems, semflg);
#endif
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_semctl(RSIM_Thread *t, uint32_t semid, uint32_t semnum, uint32_t cmd, uint32_t semun_va)
{
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));

    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    ROSE_ASSERT(version!=0);

    union semun_32 {
        uint32_t val;
        uint32_t ptr;
    };

    union semun_native {
        int val;
        void *ptr;
    };

    semun_32 guest_semun;
    if (sizeof(guest_semun)!=t->get_process()->mem_read(&guest_semun, semun_va, sizeof guest_semun)) {
        t->syscall_return(-EFAULT);
        return;
    }
    

    switch (cmd) {
        case 3:         /* IPC_INFO */
        case 19: {      /* SEM_INFO */
            seminfo host_seminfo;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_seminfo;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_seminfo);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            seminfo_32 guest_seminfo;
            guest_seminfo.semmap = host_seminfo.semmap;
            guest_seminfo.semmni = host_seminfo.semmni;
            guest_seminfo.semmns = host_seminfo.semmns;
            guest_seminfo.semmnu = host_seminfo.semmnu;
            guest_seminfo.semmsl = host_seminfo.semmsl;
            guest_seminfo.semopm = host_seminfo.semopm;
            guest_seminfo.semume = host_seminfo.semume;
            guest_seminfo.semusz = host_seminfo.semusz;
            guest_seminfo.semvmx = host_seminfo.semvmx;
            guest_seminfo.semaem = host_seminfo.semaem;
            if (sizeof(guest_seminfo)!=t->get_process()->mem_write(&guest_seminfo, guest_semun.ptr, sizeof guest_seminfo)) {
                t->syscall_return(-EFAULT);
                return;
            }

            t->syscall_return(result);
            break;
        }

        case 2:         /* IPC_STAT */
        case 18: {      /* SEM_STAT */
            semid_ds host_ds;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            semid64_ds_32 guest_ds;
            memset(&guest_ds, 0, sizeof guest_ds);
            guest_ds.sem_perm.key = host_ds.sem_perm.__key;
            guest_ds.sem_perm.uid = host_ds.sem_perm.uid;
            guest_ds.sem_perm.gid = host_ds.sem_perm.gid;
            guest_ds.sem_perm.cuid = host_ds.sem_perm.cuid;
            guest_ds.sem_perm.cgid = host_ds.sem_perm.cgid;
            guest_ds.sem_perm.mode = host_ds.sem_perm.mode;
            guest_ds.sem_perm.pad1 = host_ds.sem_perm.__pad1;
            guest_ds.sem_perm.seq = host_ds.sem_perm.__seq;
            guest_ds.sem_perm.pad2 = host_ds.sem_perm.__pad2;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.sem_perm.unused1 = host_ds.sem_perm.__unused1;
            guest_ds.sem_perm.unused2 = host_ds.sem_perm.__unused1;
#endif
            guest_ds.sem_otime = host_ds.sem_otime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.sem_ctime = host_ds.sem_ctime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.sem_nsems = host_ds.sem_nsems;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused3 = host_ds.__unused3;
            guest_ds.unused4 = host_ds.__unused4;
#endif
            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, guest_semun.ptr, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                return;
            }
                        
            t->syscall_return(result);
            break;
        };

        case 1: {       /* IPC_SET */
            semid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, guest_semun.ptr, sizeof(guest_ds))) {
                t->syscall_return(-EFAULT);
                return;
            }
#ifdef SYS_ipc  /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native semun;
            semun.ptr = &guest_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else           /* amd64 */
            semid_ds host_ds;
            memset(&host_ds, 0, sizeof host_ds);
            host_ds.sem_perm.__key = guest_ds.sem_perm.key;
            host_ds.sem_perm.uid = guest_ds.sem_perm.uid;
            host_ds.sem_perm.gid = guest_ds.sem_perm.gid;
            host_ds.sem_perm.cuid = guest_ds.sem_perm.cuid;
            host_ds.sem_perm.cgid = guest_ds.sem_perm.cgid;
            host_ds.sem_perm.mode = guest_ds.sem_perm.mode;
            host_ds.sem_perm.__pad1 = guest_ds.sem_perm.pad1;
            host_ds.sem_perm.__seq = guest_ds.sem_perm.seq;
            host_ds.sem_perm.__pad2 = guest_ds.sem_perm.pad2;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused1;
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused2;
#endif
            host_ds.sem_otime = guest_ds.sem_otime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused1 = guest_ds.unused1;
#endif
            host_ds.sem_ctime = guest_ds.sem_ctime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused2 = guest_ds.unused2;
#endif
            host_ds.sem_nsems = guest_ds.sem_nsems;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused3 = guest_ds.unused3;
            host_ds.__unused4 = guest_ds.unused4;
#endif
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 13: {      /* GETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                t->syscall_return(-EINVAL);
                return;
            }
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (NULL==t->get_process()->my_addr(guest_semun.ptr, nbytes)) {
                t->syscall_return(-EFAULT);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            if (-1==result) {
                delete[] sem_values;
                t->syscall_return(-errno);
                return;
            }
            if (nbytes!=t->get_process()->mem_write(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                t->syscall_return(-EFAULT);
                return;
            }
            if (host_ds.sem_nsems>0) {
                for (size_t i=0; i<host_ds.sem_nsems; i++) {
                    mfprintf(strace)("    value[%zu] = %" PRId16"\n", i, sem_values[i]);
                }
            }
            delete[] sem_values;
            t->syscall_return(result);
            break;
        }
            
        case 17: {      /* SETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                t->syscall_return(-EINVAL);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (nbytes!=t->get_process()->mem_read(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                t->syscall_return(-EFAULT);
                return;
            }
            for (size_t i=0; i<host_ds.sem_nsems; i++)
                mfprintf(strace)("    value[%zu] = %" PRId16"\n", i, sem_values[i]);
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            t->syscall_return(-1==result?-errno:result);
            delete[] sem_values;
            break;
        }

        case 11:        /* GETPID */
        case 12:        /* GETVAL */
        case 15:        /* GETZCNT */
        case 14: {      /* GETNCNT */
            int result = semctl(semid, semnum, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 16: {      /* SETVAL */
#ifdef SYS_ipc  /* i686 */
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &guest_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, guest_semun.val);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 0: {       /* IPC_RMID */
#ifdef SYS_ipc /* i686 */
            semun_native host_semun;
            memset(&host_semun, 0, sizeof host_semun);
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = semctl(semid, semnum, cmd, NULL);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default:
            t->syscall_return(-EINVAL);
            return;
    }
}

static void
sys_msgsnd(RSIM_Thread *t, uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        t->syscall_return(-EINVAL);
        return;
    }

    /* Read the message buffer from the specimen. */
    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    if (!buf) {
        t->syscall_return(-ENOMEM);
        return;
    }
    if (4+msgsz!=t->get_process()->mem_read(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        t->syscall_return(-EFAULT);
        return;
    }

    /* Message type must be positive */
    if (*(int32_t*)buf <= 0) {
        delete[] buf;
        t->syscall_return(-EINVAL);
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
        t->syscall_return(-errno);
        return;
    }

    delete[] buf;
    t->syscall_return(result);
}

static void
sys_msgrcv(RSIM_Thread *t, uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        t->syscall_return(-EINVAL);
        return;
    }

    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    int result = msgrcv(msqid, buf, msgsz, msgtyp, msgflg);
    if (-1==result) {
        delete[] buf;
        t->syscall_return(-errno);
        return;
    }

    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        uint64_t type = *(uint64_t*)buf;
        ROSE_ASSERT(0 == (type >> 32));
        memmove(buf+4, buf+8, msgsz);
    }

    if (4+msgsz!=t->get_process()->mem_write(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        t->syscall_return(-EFAULT);
        return;
    }

    delete[] buf;
    t->syscall_return(result);
}

static void
sys_msgget(RSIM_Thread *t, uint32_t key, uint32_t msgflg)
{
    int result = msgget(key, msgflg);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_msgctl(RSIM_Thread *t, uint32_t msqid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 3:    /* IPC_INFO */
        case 12: { /* MSG_INFO */
            t->syscall_return(-ENOSYS);              /* FIXME */
            break;
        }

        case 2:    /* IPC_STAT */
        case 11: { /* MSG_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and msqid_ds from the kernel */
            static msqid_ds host_ds;
            int result = msgctl(msqid, cmd, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            msqid64_ds_32 guest_ds;
            memset(&guest_ds, 0, sizeof guest_ds);
            guest_ds.msg_perm.key = host_ds.msg_perm.__key;
            guest_ds.msg_perm.uid = host_ds.msg_perm.uid;
            guest_ds.msg_perm.gid = host_ds.msg_perm.gid;
            guest_ds.msg_perm.cuid = host_ds.msg_perm.cuid;
            guest_ds.msg_perm.cgid = host_ds.msg_perm.cgid;
            guest_ds.msg_perm.mode = host_ds.msg_perm.mode;
            guest_ds.msg_perm.pad1 = host_ds.msg_perm.__pad1;
            guest_ds.msg_perm.seq = host_ds.msg_perm.__seq;
            guest_ds.msg_perm.pad2 = host_ds.msg_perm.__pad2;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.msg_perm.unused1 = host_ds.msg_perm.__unused1;
            guest_ds.msg_perm.unused2 = host_ds.msg_perm.__unused2;
#endif
            guest_ds.msg_stime = host_ds.msg_stime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.msg_rtime = host_ds.msg_rtime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.msg_ctime = host_ds.msg_ctime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.msg_cbytes = host_ds.__msg_cbytes;
            guest_ds.msg_qnum = host_ds.msg_qnum;
            guest_ds.msg_qbytes = host_ds.msg_qbytes;
            guest_ds.msg_lspid = host_ds.msg_lspid;
            guest_ds.msg_lrpid = host_ds.msg_lrpid;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;
#endif

            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 0: { /* IPC_RMID */
            /* NOTE: syscall tracing will not show "IPC_RMID" if the IPC_64 flag is also present */
            int result = msgctl(msqid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            msqid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
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
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default: {
            t->syscall_return(-EINVAL);
            break;
        }
    }
}

static void
sys_shmdt(RSIM_Thread *t, uint32_t shmaddr_va)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(t->get_process()->rwlock());
    int result = -ENOSYS;

    do {
        if (!t->get_process()->get_memory()->at(shmaddr_va).exists()) {
            result = -EINVAL;
            break;
        }
        const MemoryMap::Node &me = *(t->get_process()->get_memory()->find(shmaddr_va));
        if (me.key().least()!=shmaddr_va ||
            me.value().offset()!=0 ||
            NULL==me.value().buffer()->data()) {
            result = -EINVAL;
            break;
        }

        result = shmdt(me.value().buffer()->data());
        if (-1==result) {
            result = -errno;
            break;
        }

        t->get_process()->mem_unmap(me.key().least(), me.key().size(), t->tracing(TRACE_MMAP));
        result = 0;
    } while (0);

    t->syscall_return(result);
}

static void
sys_shmget(RSIM_Thread *t, uint32_t key, uint32_t size, uint32_t shmflg)
{
    int result = shmget(key, size, shmflg);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_shmctl(RSIM_Thread *t, uint32_t shmid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 13:  /* SHM_STAT */
        case 2: { /* IPC_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and shmid_ds from the kernel */
            static shmid_ds host_ds;
            int result = shmctl(shmid, cmd, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            shmid64_ds_32 guest_ds;
            memset(&guest_ds, 0, sizeof guest_ds);
            guest_ds.shm_perm.key = host_ds.shm_perm.__key;
            guest_ds.shm_perm.uid = host_ds.shm_perm.uid;
            guest_ds.shm_perm.gid = host_ds.shm_perm.gid;
            guest_ds.shm_perm.cuid = host_ds.shm_perm.cuid;
            guest_ds.shm_perm.cgid = host_ds.shm_perm.cgid;
            guest_ds.shm_perm.mode = host_ds.shm_perm.mode;
            guest_ds.shm_perm.pad1 = host_ds.shm_perm.__pad1;
            guest_ds.shm_perm.seq = host_ds.shm_perm.__seq;
            guest_ds.shm_perm.pad2 = host_ds.shm_perm.__pad2;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.shm_perm.unused1 = host_ds.shm_perm.__unused1;
            guest_ds.shm_perm.unused2 = host_ds.shm_perm.__unused2;
#endif
            guest_ds.shm_segsz = host_ds.shm_segsz;
            guest_ds.shm_atime = host_ds.shm_atime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.shm_dtime = host_ds.shm_dtime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.shm_ctime = host_ds.shm_ctime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.shm_cpid = host_ds.shm_cpid;
            guest_ds.shm_lpid = host_ds.shm_lpid;
            guest_ds.shm_nattch = host_ds.shm_nattch;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;
#endif

            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 14: { /* SHM_INFO */
            shm_info host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            shm_info_32 guest_info;
            guest_info.used_ids = host_info.used_ids;
            guest_info.shm_tot = host_info.shm_tot;
            guest_info.shm_rss = host_info.shm_rss;
            guest_info.shm_swp = host_info.shm_swp;
            guest_info.swap_attempts = host_info.swap_attempts;
            guest_info.swap_successes = host_info.swap_successes;

            if (sizeof(guest_info)!=t->get_process()->mem_write(&guest_info, buf_va, sizeof guest_info)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 3: { /* IPC_INFO */
            shminfo64_native host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            shminfo64_32 guest_info;
            memset(&guest_info, 0, sizeof guest_info);
            guest_info.shmmax = host_info.shmmax;
            guest_info.shmmin = host_info.shmmin;
            guest_info.shmmni = host_info.shmmni;
            guest_info.shmseg = host_info.shmseg;
            guest_info.shmall = host_info.shmall;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            guest_info.unused1 = host_info.unused1;
            guest_info.unused2 = host_info.unused2;
            guest_info.unused3 = host_info.unused3;
            guest_info.unused4 = host_info.unused4;
#endif
            if (sizeof(guest_info)!=t->get_process()->mem_write(&guest_info, buf_va, sizeof guest_info)) {
                t->syscall_return(-EFAULT);
                return;
            }

            t->syscall_return(result);
            break;
        }

        case 11:   /* SHM_LOCK */
        case 12: { /* SHM_UNLOCK */
            int result = shmctl(shmid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            ROSE_ASSERT(version!=0);
            shmid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                return;
            }
            shmid_ds host_ds;
            memset(&host_ds, 0, sizeof host_ds);
            host_ds.shm_perm.__key = guest_ds.shm_perm.key;
            host_ds.shm_perm.uid = guest_ds.shm_perm.uid;
            host_ds.shm_perm.gid = guest_ds.shm_perm.gid;
            host_ds.shm_perm.cuid = guest_ds.shm_perm.cuid;
            host_ds.shm_perm.cgid = guest_ds.shm_perm.cgid;
            host_ds.shm_perm.mode = guest_ds.shm_perm.mode;
            host_ds.shm_perm.__pad1 = guest_ds.shm_perm.pad1;
            host_ds.shm_perm.__seq = guest_ds.shm_perm.seq;
            host_ds.shm_perm.__pad2 = guest_ds.shm_perm.pad2;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.shm_perm.__unused1 = guest_ds.shm_perm.unused1;
            host_ds.shm_perm.__unused2 = guest_ds.shm_perm.unused2;
#endif
            host_ds.shm_segsz = guest_ds.shm_segsz;
            host_ds.shm_atime = guest_ds.shm_atime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused1 = guest_ds.unused1;
#endif
            host_ds.shm_dtime = guest_ds.shm_dtime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused2 = guest_ds.unused2;
#endif
            host_ds.shm_ctime = guest_ds.shm_ctime;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused3 = guest_ds.unused3;
#endif
            host_ds.shm_cpid = guest_ds.shm_cpid;
            host_ds.shm_lpid = guest_ds.shm_lpid;
            host_ds.shm_nattch = guest_ds.shm_nattch;
#if 0 // [Robb P. Matzke 2015-08-12]: libc version doesn't always have the "unused" members
            host_ds.__unused4 = guest_ds.unused4;
            host_ds.__unused5 = guest_ds.unused5;
#endif

            int result = shmctl(shmid, cmd, &host_ds);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 0: { /* IPC_RMID */
            int result = shmctl(shmid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default: {
            t->syscall_return(-EINVAL);
            break;
        }
    }
}

static void
sys_shmat(RSIM_Thread *t, uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t shmaddr)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(t->get_process()->rwlock());
    int result = -ENOSYS;

    do {
        if (0==shmaddr) {
            MemoryMap::Ptr mm = t->get_process()->get_memory();
            AddressInterval freeArea = mm->unmapped(AddressInterval::whole().greatest(), Sawyer::Container::MATCH_BACKWARD);
            assert(!freeArea.isEmpty());
            shmaddr = freeArea.least();
        } else if (shmflg & SHM_RND) {
            shmaddr = alignDown(shmaddr, (uint32_t)SHMLBA);
        } else if (alignDown(shmaddr, (uint32_t)4096)!=shmaddr) {
            result = -EINVAL;
            break;
        }

        /* We don't handle SHM_REMAP */
        if (shmflg & SHM_REMAP) {
            result = -EINVAL;
            break;
        }

        /* Map shared memory into the simulator. It's OK to hold the write lock here because this syscall doesn't block. */
        uint8_t *buf = (uint8_t*)shmat(shmid, NULL, shmflg);
        if (!buf) {
            result = -errno;
            break;
        }

        /* Map simulator's shared memory into the specimen */
        shmid_ds ds;
        int status = shmctl(shmid, IPC_STAT, &ds); // does not block
        ROSE_ASSERT(status>=0);
        ROSE_ASSERT(ds.shm_segsz>0);
        unsigned perms = MemoryMap::READABLE | ((shmflg & SHM_RDONLY) ? 0 : MemoryMap::WRITABLE);

        MemoryMap::Buffer::Ptr buffer = MemoryMap::StaticBuffer::instance(buf, ds.shm_segsz);
        MemoryMap::Segment sgmt(buffer, 0, perms, "shmat("+StringUtility::numberToString(shmid)+")");
        t->get_process()->get_memory()->insert(AddressInterval::baseSize(shmaddr, ds.shm_segsz), sgmt);

        /* Return values */
        if (4!=t->get_process()->mem_write(&shmaddr, result_va, 4)) {
            result = -EFAULT;
            break;
        }
        t->syscall_return(shmaddr);
        result = 0;
    } while (0);
    if (result)
        t->syscall_return(result);
}

void
RSIM_Linux32::syscall_ipc_body(RSIM_Thread *t, int callno)
{
    /* Return value is written to eax by these helper functions. The structure of this code closely follows that in the
     * Linux kernel. */
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t first=t->syscall_arg(1), second=t->syscall_arg(2), third=t->syscall_arg(3);
    uint32_t ptr=t->syscall_arg(4), fifth=t->syscall_arg(5);
    switch (call) {
        case 1: /* SEMOP */
            sys_semtimedop(t, first, ptr, second, 0);
            break;
        case 2: /* SEMGET */
            sys_semget(t, first, second, third);
            break;
        case 3: /* SEMCTL */
            sys_semctl(t, first, second, third, ptr);
            break;
        case 4: /* SEMTIMEDOP */
            sys_semtimedop(t, first, ptr, second, fifth);
            break;
        case 11: /* MSGSND */
            sys_msgsnd(t, first, ptr, second, third);
            break;
        case 12: /* MSGRCV */
            if (0==version) {
                ipc_kludge_32 kludge;
                if (8!=t->get_process()->mem_read(&kludge, t->syscall_arg(4), 8)) {
                    t->syscall_return(-ENOSYS);
                } else {
                    sys_msgrcv(t, first, kludge.msgp, second, kludge.msgtyp, third);
                }
            } else {
                sys_msgrcv(t, first, ptr, second, fifth, third);
            }
            break;
        case 13: /* MSGGET */
            sys_msgget(t, first, second);
            break;
        case 14: /* MSGCTL */
            sys_msgctl(t, first, second, ptr);
            break;
        case 21: /* SHMAT */
            if (1==version) {
                /* This was the entry point for kernel-originating calls from iBCS2 in 2.2 days */
                t->syscall_return(-EINVAL);
            } else {
                sys_shmat(t, first, second, third, ptr);
            }
            break;
        case 22: /* SHMDT */
            sys_shmdt(t, ptr);
            break;
        case 23: /* SHMGET */
            sys_shmget(t, first, second, third);
            break;
        case 24: /* SHMCTL */
            sys_shmctl(t, first, second, ptr);
            break;
        default:
            t->syscall_return(-ENOSYS);
            break;
    }
}

void
RSIM_Linux32::syscall_ipc_leave(RSIM_Thread *t, int callno)
{
    Sawyer::Message::Stream mtrace(t->tracing(TRACE_MMAP));
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t second=t->syscall_arg(2);
    switch (call) {
        case 14: /* MSGCTL */ {
            switch (second & 0xff) {
                case 3:         /* IPC_INFO */
                case 12:        /* MSG_INFO */
                    ROSE_ASSERT(!"not handled");
                case 2:         /* IPC_STAT */
                case 11:        /* MSG_STAT */
                    t->syscall_leave().ret().arg(4).P(sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    break;
                default:
                    t->syscall_leave().ret();
                    break;
            }
            break;
        }
        case 21: /* SHMAT */
            if (1==version) {
                t->syscall_leave().ret();
            } else {
                t->syscall_leave().p();
                t->get_process()->mem_showmap(mtrace, "  memory map after shmat:\n");
            }
            break;
        case 22: /* SHMDT */
            t->syscall_leave().ret();
            t->get_process()->mem_showmap(mtrace, "  memory map after shmdt:\n");
            break;
        case 24: { /* SHMCTL */
            switch (second & 0xff) {
                case 2:         /* IPC_STAT */
                case 13:        /* SHM_STAT */
                    t->syscall_leave().ret().arg(4).P(sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    break;
                case 14:        /* SHM_INFO */
                    t->syscall_leave().ret().arg(4).P(sizeof(shm_info_32), print_shm_info_32);
                    break;
                case 3:         /* IPC_INFO */
                    t->syscall_leave().ret().arg(4).P(sizeof(shminfo64_32), print_shminfo64_32);
                    break;
                default:
                    t->syscall_leave().ret();
                    break;
            }
            break;
        }
        default:
            t->syscall_leave().ret();
            break;
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sigreturn_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sigreturn");
}

void
RSIM_Linux32::syscall_sigreturn_body(RSIM_Thread *t, int callno)
{
    int status = t->sys_sigreturn();
    if (status>=0) {
        t->tracing(TRACE_SYSCALL) <<" = <does not return>\n";
        throw RSIM_SignalHandling::mk_kill(0, 0);
    }
    t->syscall_return(status); /* ERROR; specimen will likely segfault shortly! */
}

void
RSIM_Linux32::syscall_sigreturn_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the sigreturn system call body was skipped over. */
    t->tracing(TRACE_SYSCALL) <<" = <should not have returned>\n";
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_clone_enter(RSIM_Thread *t, int callno)
{
    /* From linux arch/x86/kernel/process.c:
     *    long sys_clone(unsigned long clone_flags, unsigned long newsp,
     *                   void __user *parent_tid, void __user *child_tid, struct pt_regs *regs)
     *    {
     *        if (!newsp)
     *            newsp = regs->sp;
     *        return do_fork(clone_flags, newsp, regs, 0, parent_tid, child_tid);
     *    }
     *
     * The fourth argument, child_tid, varies depending on clone_flags. Linux doesn't appear to require that
     * these are mutually exclusive.  It appears as though the CLONE_SETTLS happens before CLONE_CHILD_SETTID.
     *   CLONE_CHILD_SETTID:  it is an address where the child's TID is written when the child is created
     *   CLONE_SETTLS:        it is an address of a user_desc_32 which will be loaded into the GDT.
     */
    unsigned flags = t->syscall_arg(0);
    if (flags & CLONE_SETTLS) {
        t->syscall_enter("clone")
            .f(clone_flags)
            .p()
            .p()
            .P(sizeof(SegmentDescriptor), print_SegmentDescriptor)
            .P(sizeof(pt_regs_32), print_pt_regs_32);
    } else {
        t->syscall_enter("clone")
            .f(clone_flags)
            .p()
            .p()
            .p()
            .P(sizeof(pt_regs_32), print_pt_regs_32);
    }
}

static int
sys_clone(RSIM_Thread *t, unsigned flags, uint32_t newsp, uint32_t parent_tid_va, uint32_t child_tls_va, uint32_t pt_regs_va)
{
    RSIM_Process *p = t->get_process();


    // Flags are documented at http://linux.die.net/man/2/clone and elsewhere
    if ((flags == (CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD)) || // linux 2.5.32 through 2.5.48
        (flags == (CLONE_PARENT_SETTID | SIGCHLD))) {                       // linux 2.5.49 and later

        /* Apply process pre-fork callbacks */
        p->get_callbacks().call_process_callbacks(RSIM_Callbacks::BEFORE, p, RSIM_Callbacks::ProcessCallback::FORK, true);

        /* We cannot use clone() because it's a wrapper around the clone system call and we'd need to provide a function for it
         * to execute. We want fork-like semantics.  The RSIM_Process::atfork_*() callbacks are invoked here, and among other
         * actions, wait and post the simulator's global semaphore. */
        t->atfork_prepare();
        pid_t pid = fork();
        bool isParent = pid != -1 && pid != 0;
        bool isChild = pid == 0;
        uint32_t childTid32 = isParent ? pid : getpid();

#if 1 // DEBUGGING [Robb P. Matzke 2015-02-09]
        if (isChild && 0 == access("./x86sim-stop-at-fork", F_OK)) {
            p->get_memory()->dump(std::cerr, "childmem: ");
            std::cerr <<"simulator: new process " <<childTid32 <<" stopped immediately after creation\n";
            raise(SIGSTOP);
        }
#endif

        if (isParent) {
            t->atfork_parent();
        } else if (isChild) {
            t->atfork_child();
        }

        // CLONE_CHILD_SETTID: Store child thread ID at location ctid in child memory.
        if (isChild && 0 != (flags & CLONE_CHILD_SETTID) && child_tls_va) {
            size_t nwritten = p->mem_write(&childTid32, child_tls_va, 4);
            ROSE_ASSERT(4==nwritten);
        }

        // CLONE_CHILD_CLEARTID: Erase child thread ID at location ctid in child memory when the child exits, and do a wakeup
        // on the futex at that address. The address involved may be changed by the set_tid_address(2) system call. This is
        // used by threading libraries.
        if (isChild && 0!=(flags & CLONE_CHILD_CLEARTID))
            t->clearChildTidVa(parent_tid_va);

        // CLONE_PARENT_SETTID: Store child thread ID at location ptid in parent and child memory. (In Linux 2.5.32-2.5.48
        // there was a flag CLONE_SETTID that did this.)
        if ((isParent || isChild) && 0 != (flags & CLONE_PARENT_SETTID) && parent_tid_va) {
            size_t nwritten = p->mem_write(&childTid32, parent_tid_va, 4);
            ROSE_ASSERT(4==nwritten);
        }

        // Return register values in child
        // does not work for linux 2.6.32; perhaps this was only necessary for 2.5.32-2.5.48? [Robb P. Matzke 2015-02-09]
        if (isChild && flags==(CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD)) {
            pt_regs_32 regs = t->get_regs().get_pt_regs_32();
            regs.ip = regs.ax = regs.bx = 0; // these three weren't in old initialization [Robb P. Matzke 2015-04-22]
            if (sizeof(regs)!=p->mem_write(&regs, pt_regs_va, sizeof regs))
                return -EFAULT;
        }

        // Callbacks
        if (isChild)
            p->get_callbacks().call_process_callbacks(RSIM_Callbacks::AFTER, p, RSIM_Callbacks::ProcessCallback::FORK, true);

        return -1==pid ? -errno : pid;
        
    } else if (flags == (CLONE_VM |
                         CLONE_FS |
                         CLONE_FILES |
                         CLONE_SIGHAND |
                         CLONE_THREAD |
                         CLONE_SYSVSEM |
                         CLONE_SETTLS |
                         CLONE_PARENT_SETTID |
                         CLONE_CHILD_CLEARTID)) {
        /* we are creating a new thread */
        PtRegs regs = t->get_regs();
        regs.sp = newsp;
        regs.ax = 0;

        pid_t tid = p->clone_thread(flags, parent_tid_va, child_tls_va, regs, true /*start_running*/);
        return tid;
    } else {
        return -EINVAL; /* can't handle this combination of flags */
    }
}

void
RSIM_Linux32::syscall_clone_body(RSIM_Thread *t, int callno)
{
    unsigned flags = t->syscall_arg(0);
    uint32_t newsp = t->syscall_arg(1);
    uint32_t parent_tid_va = t->syscall_arg(2);
    uint32_t child_tls_va = t->syscall_arg(3);
    uint32_t regs_va = t->syscall_arg(4);
    t->syscall_return(sys_clone(t, flags, newsp, parent_tid_va, child_tls_va, regs_va));
}

void
RSIM_Linux32::syscall_clone_leave(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(-1)) {
        /* Parent */
        t->syscall_leave().ret();
    } else {
        /* Child returns here for fork, but not for thread-clone */
        t->syscall_enter("child's clone").f(clone_flags).p().p().p().P(sizeof(pt_regs_32), print_pt_regs_32);
        t->syscall_leave().ret().arg(4).P(sizeof(pt_regs_32), print_pt_regs_32);
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_uname_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("uname").p();
}

void
RSIM_Linux32::syscall_uname_body(RSIM_Thread *t, int callno)
{
    uint32_t dest_va=t->syscall_arg(0);
    new_utsname_32 buf;
    memset(&buf, ' ', sizeof buf);
#if 0
    strcpy(buf.sysname,         "Linux");
    strcpy(buf.nodename,        "mymachine.example.com");
    strcpy(buf.release,         "2.6.18");
    strcpy(buf.version,         "#1 SMP Wed Jun 18 12:35:02 EDT 2008");
    strcpy(buf.macine,          "i386");
    strcpy(buf.domainname,      "example.com");
#else
    syscall(SYS_uname, &buf);
    char *s;
    if ((s=strstr(buf.release, "-amd64")))
        *s = '\0';
    strcpy(buf.machine, "i386");
#endif

    size_t nwritten = t->get_process()->mem_write(&buf, dest_va, sizeof buf);
    if( nwritten <= 0 ) {
        t->syscall_return(-EFAULT);
        return;
    }

    ROSE_ASSERT(nwritten==sizeof buf);
    t->syscall_return(0);
}

void
RSIM_Linux32::syscall_uname_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(new_utsname_32), print_new_utsname_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_modify_ldt_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("modify_ldt").d().p().d();
}

void
RSIM_Linux32::syscall_modify_ldt_body(RSIM_Thread *t, int callno)
{
    assert(!"not implemented yet");
}

void
RSIM_Linux32::syscall_modify_ldt_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret();                           // "---"; // FIXME
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_llseek_enter(RSIM_Thread *t, int callno)
{
    /* From the linux kernel, arguments are:
     *      unsigned int fd,                // file descriptor
     *      unsigned long offset_high,      // high 32 bits of 64-bit offset
     *      unsigned long offset_low,       // low 32 bits of 64-bit offset
     *      loff_t __user *result,          // 64-bit user area to write resulting position
     *      unsigned int origin             // whence specified offset is measured
     */
    t->syscall_enter("llseek").d().d().d().p().f(seek_whence);
}

void
RSIM_Linux32::syscall_llseek_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    off64_t offset = ((off64_t)t->syscall_arg(1) << 32) | t->syscall_arg(2);
    uint32_t result_va = t->syscall_arg(3);
    int whence = t->syscall_arg(4);

    off64_t result = lseek64(hostFd, offset, whence);
    if (-1==result) {
        t->syscall_return(-errno);
    } else {
        t->syscall_return(0);
        size_t nwritten = t->get_process()->mem_write(&result, result_va, sizeof result);
        ROSE_ASSERT(nwritten==sizeof result);
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getdents_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getdents").d().p().d();
}

void
RSIM_Linux32::syscall_getdents_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0), sz = t->syscall_arg(2);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint32_t dirent_va = t->syscall_arg(1);
    int status;
    if (4 == sizeof(long)) {
        status = getdents_syscall<dirent_32, dirent64_32>(t, SYS_getdents64, hostFd, dirent_va, sz);
    } else {
        status = getdents_syscall<dirent_32, dirent_64>(t, SYS_getdents, hostFd, dirent_va, sz);
    }

    t->syscall_return(status);
}

void
RSIM_Linux32::syscall_getdents_leave(RSIM_Thread *t, int callno)
{
    int status = t->syscall_arg(-1);
    t->syscall_leave().ret().arg(1).P(status>0?status:0, print_dentries_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_select_enter(RSIM_Thread *t, int callno)
{
    /* From the Linux kernel (fs/select.c):
     *    SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
     *                    fd_set __user *, exp, struct timeval __user *, tvp)
     * where:
     *    fd_set is enough "unsigned long" data to contain 1024 bits. Regardless of the size of "unsigned long",
     *    the file bits will be in the same order (when the host is little endian), and the fd_set is the same size. */
    t->syscall_enter("select")
        .d()
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(timeval_32), print_timeval_32);
}

void
RSIM_Linux32::syscall_select_body(RSIM_Thread *t, int callno)
{
    int nFileDescriptors = t->syscall_arg(0);
    if (nFileDescriptors < 0 || nFileDescriptors>1024) {
        t->syscall_return(-EINVAL);
        return;
    }

    uint32_t inVa=t->syscall_arg(1), outVa=t->syscall_arg(2), exVa=t->syscall_arg(3), tvVa=t->syscall_arg(4);

    // Read the guest file descriptor sets
    ROSE_ASSERT(128==sizeof(fd_set)); /* 128 bytes = 1024 file descriptor bits */
    fd_set hostIn, hostOut, hostEx;
    fd_set *hostInPtr=NULL, *hostOutPtr=NULL, *hostExPtr=NULL;

    if (inVa) {
        fd_set guestIn;
        if (sizeof(guestIn) != t->get_process()->mem_read(&guestIn, inVa, sizeof guestIn)) {
            t->syscall_return(-EFAULT);
            return;
        }
        FD_ZERO(&hostIn);
        for (int guestFd=0; guestFd<nFileDescriptors; ++guestFd) {
            if (FD_ISSET(guestFd, &guestIn)) {
                int hostFd = t->get_process()->hostFileDescriptor(guestFd);
                if (-1 == hostFd) {
                    t->syscall_return(-EBADF);
                    return;
                }
                FD_SET(hostFd, &hostIn);
            }
        }
        hostInPtr = &hostIn;
    }

    if (outVa) {
        fd_set guestOut;
        if (sizeof(guestOut) != t->get_process()->mem_read(&guestOut, outVa, sizeof guestOut)) {
            t->syscall_return(-EFAULT);
            return;
        }
        FD_ZERO(&hostOut);
        for (int guestFd=0; guestFd<nFileDescriptors; ++guestFd) {
            if (FD_ISSET(guestFd, &guestOut)) {
                int hostFd = t->get_process()->hostFileDescriptor(guestFd);
                if (-1 == hostFd) {
                    t->syscall_return(-EBADF);
                    return;
                }
                FD_SET(hostFd, &hostOut);
            }
        }
        hostOutPtr = &hostOut;
    }

    if (exVa) {
        fd_set guestEx;
        if (sizeof(guestEx) != t->get_process()->mem_read(&guestEx, exVa, sizeof guestEx)) {
            t->syscall_return(-EFAULT);
            return;
        }
        FD_ZERO(&hostEx);
        for (int guestFd=0; guestFd<nFileDescriptors; ++guestFd) {
            if (FD_ISSET(guestFd, &guestEx)) {
                int hostFd = t->get_process()->hostFileDescriptor(guestFd);
                if (-1 == hostFd) {
                    t->syscall_return(-EBADF);
                    return;
                }
                FD_SET(hostFd, &hostEx);
            }
        }
        hostExPtr = &hostEx;
    }

    // Read the timeout
    timeval_32 guest_timeout;
    timeval host_timeout, *tvp=NULL;
    if (tvVa) {
        if (sizeof(guest_timeout)!=t->get_process()->mem_read(&guest_timeout, tvVa, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        } else {
            host_timeout.tv_sec = guest_timeout.tv_sec;
            host_timeout.tv_usec = guest_timeout.tv_usec;
            tvp = &host_timeout;
        }
    }

    // Actual operation
    int result = select(nFileDescriptors, hostInPtr, hostOutPtr, hostExPtr, tvp);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    // Convert host descriptors to guest descriptors
    if (hostInPtr) {
        fd_set guest;
        FD_ZERO(&guest);
        for (int hostFd=0; hostFd<nFileDescriptors; ++hostFd) {
            if (FD_ISSET(hostFd, &hostIn)) {
                int guestFd = t->get_process()->guestFileDescriptor(hostFd);
                ASSERT_require(guestFd >= 0);
                FD_SET(guestFd, &guest);
            }
            if (sizeof(guest) != t->get_process()->mem_write(&guest, inVa, sizeof guest)) {
                t->syscall_return(-EFAULT);
                return;
            }
        }
    }

    if (hostOutPtr) {
        fd_set guest;
        FD_ZERO(&guest);
        for (int hostFd=0; hostFd<nFileDescriptors; ++hostFd) {
            if (FD_ISSET(hostFd, &hostOut)) {
                int guestFd = t->get_process()->guestFileDescriptor(hostFd);
                ASSERT_require(guestFd >= 0);
                FD_SET(guestFd, &guest);
            }
            if (sizeof(guest) != t->get_process()->mem_write(&guest, outVa, sizeof guest)) {
                t->syscall_return(-EFAULT);
                return;
            }
        }
    }

    if (hostExPtr) {
        fd_set guest;
        FD_ZERO(&guest);
        for (int hostFd=0; hostFd<nFileDescriptors; ++hostFd) {
            if (FD_ISSET(hostFd, &hostEx)) {
                int guestFd = t->get_process()->guestFileDescriptor(hostFd);
                ASSERT_require(guestFd >= 0);
                FD_SET(guestFd, &guest);
            }
            if (sizeof(guest) != t->get_process()->mem_write(&guest, exVa, sizeof guest)) {
                t->syscall_return(-EFAULT);
                return;
            }
        }
    }

    // Write time value back to guest
    if (tvp) {
        guest_timeout.tv_sec = tvp->tv_sec;
        guest_timeout.tv_usec = tvp->tv_usec;
        if (sizeof(guest_timeout)!=t->get_process()->mem_write(&guest_timeout, tvVa, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        }
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_select_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1)
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(fd_set), print_bitvec)
        .P(sizeof(timeval_32), print_timeval_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_msync_enter(RSIM_Thread *t, int callno)
{
    static const Translate msync_flags[] = { TF(MS_ASYNC), TF(MS_SYNC), TF(MS_INVALIDATE), T_END };
    t->syscall_enter("msync").p().d().f(msync_flags);
}

void
RSIM_Linux32::syscall_msync_body(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(0) % 4096) {
        t->syscall_return(-EINVAL);
        return;
    }

    void *addr = t->get_process()->my_addr(t->syscall_arg(0), t->syscall_arg(1));
    if (!addr) {
        t->syscall_return(-ENOMEM);
    } else if (-1==msync(addr, t->syscall_arg(1), t->syscall_arg(2))) {
        t->syscall_return(-errno);
    } else {
        t->syscall_return(0);
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_writev_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("writev").d().p().d();
}

void
RSIM_Linux32::syscall_writev_body(RSIM_Thread *t, int callno)
{
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint32_t iov_va=t->syscall_arg(1);
    int niov=t->syscall_arg(2), idx=0;
    int retval = 0;
    if (niov<0 || niov>1024) {
        retval = -EINVAL;
    } else {
        if (niov>0)
            strace <<"\n";
        for (idx=0; idx<niov; idx++) {
            /* Obtain buffer address and size */
            mfprintf(strace)("    iov %d: ", idx);

            iovec_32 iov;
            if (sizeof(iov)!=t->get_process()->mem_read(&iov, iov_va+idx*sizeof(iov), sizeof(iov))) {
                if (0==idx)
                    retval = -EFAULT;
                strace <<"<segfault reading iovec>\n";
                break;
            }

            /* Make sure total size doesn't overflow a ssize_t */
            if ((iov.iov_len & 0x80000000) || ((uint32_t)retval+iov.iov_len) & 0x80000000) {
                if (0==idx)
                    retval = -EINVAL;
                strace <<"<size overflow>\n";
                break;
            }

            /* Copy data from guest to host because guest memory might not be contiguous in the host. Perhaps a more
             * efficient way to do this would be to copy chunks of host-contiguous data in a loop instead. */
            uint8_t buf[iov.iov_len];
            if (iov.iov_len != t->get_process()->mem_read(buf, iov.iov_base, iov.iov_len)) {
                if (0==idx)
                    retval = -EFAULT;
                strace <<"<seg fault reading buffer>\n";
                break;
            }
            Printer::print_buffer(strace, iov.iov_base, buf, iov.iov_len, 1024);
            mfprintf(strace)(" (size=%" PRIu32")", iov.iov_len);

            /* Write data to the file */
            ssize_t nwritten = write(hostFd, buf, iov.iov_len);
            if (-1==nwritten) {
                if (0==idx)
                    retval = -errno;
                mfprintf(strace)(" <write failed (%s)>\n", strerror(errno));
                break;
            }
            retval += nwritten;
            if ((uint32_t)nwritten<iov.iov_len) {
                mfprintf(strace)(" <short write of %zd bytes>\n", nwritten);
                break;
            }
            strace <<"\n";
        }
    }
    t->syscall_return(retval);
    if (niov>0 && niov<=1024)
        strace <<"writev return";
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sched_setparam_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_setparam").d().P(sizeof(sched_param_32), print_sched_param_32);
}

void
RSIM_Linux32::syscall_sched_setparam_body(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    rose_addr_t params_va = t->syscall_arg(1);

    sched_param_32 guest;
    if (sizeof(guest)!=t->get_process()->mem_read(&guest, params_va, sizeof guest)) {
        t->syscall_return(-EFAULT);
        return;
    }

    sched_param host;
    host.sched_priority = guest.sched_priority;

    int result = sched_setparam(pid, &host);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sched_setscheduler_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_setscheduler").d().f(scheduler_policies).P(sizeof(sched_param_32), print_sched_param_32);
}

void
RSIM_Linux32::syscall_sched_setscheduler_body(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    int policy = t->syscall_arg(1);
    rose_addr_t params_va = t->syscall_arg(2);

    sched_param_32 guest;
    if (sizeof(guest)!=t->get_process()->mem_read(&guest, params_va, sizeof guest)) {
        t->syscall_return(-EFAULT);
        return;
    }

    sched_param host;
    host.sched_priority = guest.sched_priority;

    int result = sched_setscheduler(pid, policy, &host);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_prctl_enter(RSIM_Thread *t, int callno)
{
    switch (t->syscall_arg(0)) {
        case PR_SET_NAME:
            t->syscall_enter("prctl").e(prctl_options).s();
            break;
        default:
            t->syscall_enter("prctl").e(prctl_options).x().x().x().x();
            break;
    }
}

void
RSIM_Linux32::syscall_prctl_body(RSIM_Thread *t, int callno)
{
    int option = t->syscall_arg(0);
    switch (option) {
        case PR_SET_NAME: {
            bool error;
            std::string name = t->get_process()->read_string(t->syscall_arg(1), 17, &error);
            if (error) {
                t->syscall_return(-EFAULT);
                return;
            }
            int result = prctl(PR_SET_NAME, name.c_str());
            t->syscall_return(-1==result ? -errno : result);
            break;
        }
        default:
            t->syscall_return(-ENOSYS); // FIXME
            break;
    }
}


/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_rt_sigreturn_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigreturn");
}

void
RSIM_Linux32::syscall_rt_sigreturn_body(RSIM_Thread *t, int callno)
{
    int status = t->sys_rt_sigreturn();
    if (status>=0) {
        t->tracing(TRACE_SYSCALL) <<" = <does not return>\n";
        throw RSIM_SignalHandling::mk_kill(0, 0);
    }
    t->syscall_return(status); /* ERROR; specimen will likely segfault shortly! */
}

void
RSIM_Linux32::syscall_rt_sigreturn_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the rt_sigreturn system call body was skipped over. */
    t->tracing(TRACE_SYSCALL) <<" = <should not have returned>\n";
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_rt_sigpending_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigpending").p();
}

void
RSIM_Linux32::syscall_rt_sigpending_body(RSIM_Thread *t, int callno)
{
    uint32_t sigset_va=t->syscall_arg(0);
    RSIM_SignalHandling::SigSet pending;
    int result = t->sys_sigpending(&pending);
    t->syscall_return(result);
    if (result<0)
        return;

    if (sizeof(pending)!=t->get_process()->mem_write(&pending, sigset_va, sizeof pending)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

void
RSIM_Linux32::syscall_rt_sigpending_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(RSIM_SignalHandling::SigSet), print_SigSet);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_rt_sigsuspend_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigsuspend").P(sizeof(RSIM_SignalHandling::SigSet), print_SigSet).d();
}

void
RSIM_Linux32::syscall_rt_sigsuspend_body(RSIM_Thread *t, int callno)
{
    assert(sizeof(RSIM_SignalHandling::SigSet)==t->syscall_arg(1));
    RSIM_SignalHandling::SigSet new_mask;
    if (sizeof(new_mask)!=t->get_process()->mem_read(&new_mask, t->syscall_arg(0), sizeof new_mask)) {
        t->syscall_return(-EFAULT);
        return;
    }
    t->syscall_info.signo = t->sys_sigsuspend(&new_mask);
    t->syscall_return(-EINTR);
}

void
RSIM_Linux32::syscall_rt_sigsuspend_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret();
    if (t->syscall_info.signo>0) {
        t->tracing(TRACE_SYSCALL) <<"    retured due to ";
        Printer::print_enum(t->tracing(TRACE_SYSCALL), signal_names, t->syscall_info.signo);
        mfprintf(t->tracing(TRACE_SYSCALL))("(%d)\n", t->syscall_info.signo);
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_pread64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pread64").d().p().d().d();
}

void
RSIM_Linux32::syscall_pread64_body(RSIM_Thread *t, int callno)
{
    int guestFd         = t->syscall_arg(0);
    uint32_t buf_va     = t->syscall_arg(1);
    uint32_t size       = t->syscall_arg(2);
    uint32_t offset     = t->syscall_arg(3);

    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint8_t *buf = new uint8_t[size];
    ssize_t nread = pread64(hostFd, buf, size, offset);
    if (-1==nread) {
        t->syscall_return(-errno);
    } else if ((size_t)nread != t->get_process()->mem_write(buf, buf_va, (size_t)nread)) {
        t->syscall_return(-EFAULT);
    } else {
        t->syscall_return(nread);
    }

    delete[] buf;
}

void
RSIM_Linux32::syscall_pread64_leave(RSIM_Thread *t, int callno)
{
    ssize_t nread = t->syscall_arg(-1);
    t->syscall_leave().ret().arg(1).b(nread>0?nread:0);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sigaltstack_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sigaltstack").P(sizeof(stack_32), print_stack_32).p();
}

void
RSIM_Linux32::syscall_sigaltstack_body(RSIM_Thread *t, int callno)
{
    uint32_t in_va=t->syscall_arg(0), out_va=t->syscall_arg(1);

    stack_32 in_stack, out_stack;
    stack_32 *in_stack_p  = in_va  ? &in_stack  : NULL;
    stack_32 *out_stack_p = out_va ? &out_stack : NULL;

    if (in_stack_p && sizeof(in_stack)!=t->get_process()->mem_read(in_stack_p, in_va, sizeof in_stack)) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = t->sys_sigaltstack(in_stack_p, out_stack_p);
    t->syscall_return(result);
    if (result<0)
        return;

    if (out_stack_p && sizeof(out_stack)!=t->get_process()->mem_write(out_stack_p, out_va, sizeof out_stack)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

void
RSIM_Linux32::syscall_sigaltstack_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(stack_32), print_stack_32);
}
            
/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_mmap2_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mmap2").p().d().f(mmap_pflags).f(mmap_mflags).d().d();
}

void
RSIM_Linux32::syscall_mmap2_body(RSIM_Thread *t, int callno)
{
    uint32_t start=t->syscall_arg(0), size=t->syscall_arg(1), prot=t->syscall_arg(2), flags=t->syscall_arg(3);
    uint32_t offset=t->syscall_arg(5)*PAGE_SIZE;
    int guestFd=t->syscall_arg(4);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    unsigned rose_perms = 0;
    if (0 != (prot & PROT_READ))
        rose_perms |= MemoryMap::READABLE;
    if (0 != (prot & PROT_WRITE))
        rose_perms |= MemoryMap::WRITABLE;
    if (0 != (prot & PROT_EXEC))
        rose_perms |= MemoryMap::EXECUTABLE;

    uint32_t result = t->get_process()->mem_map(start, size, rose_perms, flags, offset, hostFd);
    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_mmap2_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().eret().p();
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mmap2 syscall:\n");
}

/*******************************************************************************************************************************/

/* Single function for syscalls 195 (stat64), 196 (lstat64), and 197 (fstat64) */
void
RSIM_Linux32::syscall_stat64_enter(RSIM_Thread *t, int callno)
{
    if (195==callno || 196==callno) {
        t->syscall_enter(195==callno?"stat64":"lstat64").s().p();
    } else {
        t->syscall_enter("fstat64").d().p();
    }
}

void
RSIM_Linux32::syscall_stat64_body(RSIM_Thread *t, int callno)
{
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
        std::string name = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
        if (error) {
            t->syscall_return(-EFAULT);
            return;
        }
        result = syscall(host_callno, (unsigned long)name.c_str(), (unsigned long)kernel_stat);
    } else {
        int guestFd = t->syscall_arg(0);
        int hostFd = t->get_process()->hostFileDescriptor(guestFd);
        result = syscall(host_callno, (unsigned long)hostFd, (unsigned long)kernel_stat);
    }
    if (-1==result) {
        t->syscall_return(-errno);
        return;
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
        t->tracing(TRACE_SYSCALL) <<"[64-to-32]";
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
        t->get_process()->mem_write(&out, t->syscall_arg(1), sizeof out);
    } else {
        t->get_process()->mem_write(kernel_stat, t->syscall_arg(1), kernel_stat_size);
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_stat64_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(kernel_stat_32), print_kernel_stat_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getuid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getuid32");
}

void
RSIM_Linux32::syscall_getuid32_body(RSIM_Thread *t, int callno)
{
    uid_t id = getuid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getgid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getgid32");
}

void
RSIM_Linux32::syscall_getgid32_body(RSIM_Thread *t, int callno)
{
    uid_t id = getgid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_geteuid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("geteuid32");
}

void
RSIM_Linux32::syscall_geteuid32_body(RSIM_Thread *t, int callno)
{
    uid_t id = geteuid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getegid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getegid32");
}

void
RSIM_Linux32::syscall_getegid32_body(RSIM_Thread *t, int callno)
{
    uid_t id = getegid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getgroups32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getgroups32").d().p();
}

void
RSIM_Linux32::syscall_getgroups32_body(RSIM_Thread *t, int callno)
{

    int nelmts = t->syscall_arg(0);
    uint32_t list_ptr = t->syscall_arg(1);
    int ngroups = 0;
    gid_t *list = NULL;

    /* Use the process mutex so we can figure out how much memory will be needed before we obtain the groups. */
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(t->get_process()->rwlock());
        if (-1==(ngroups = getgroups(0, NULL)))
            t->syscall_return(-errno); // capture errno before we lose it
        if (ngroups>0 && nelmts>0 && ngroups<=nelmts) {
            list = new gid_t[ngroups];
            int ngroups2 __attribute__((unused)) = getgroups(ngroups, list);
            assert(ngroups2==ngroups);
        }
    }

    if (-1==ngroups) {
        // errno already captured
    } else if (0==nelmts) {
        t->syscall_return(ngroups);
    } else if (ngroups>nelmts) {
        t->syscall_return(-EINVAL);
    } else {
        assert(list!=NULL);
        assert(sizeof(list[0])==4); // gid_t is 4 bytes on i386
        size_t nwrite = t->get_process()->mem_write(list, list_ptr, ngroups*sizeof(list[0]));
        if (nwrite!=ngroups*sizeof(list[0])) {
            t->syscall_return(-EFAULT);
        } else {
            t->syscall_return(ngroups);
        }
    }

    delete[] list;
}

void
RSIM_Linux32::syscall_getgroups32_leave(RSIM_Thread *t, int callno)
{
    int ngroups = t->syscall_arg(-1);
    int nreq = t->syscall_arg(0);
    if (nreq>0) {
        t->syscall_leave().ret().arg(1).P(std::min(ngroups, nreq)*sizeof(gid_t), print_int_32);
    } else {
        t->syscall_leave().ret();
    }
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_setgroups32_enter(RSIM_Thread *t, int callno)
{
    size_t ngroups = t->syscall_arg(0);
    size_t maxgroups = sysconf(_SC_NGROUPS_MAX);
    t->syscall_enter("setgroups32").d().P(std::min(ngroups, maxgroups)*sizeof(gid_t), print_int_32);
}

void
RSIM_Linux32::syscall_setgroups32_body(RSIM_Thread *t, int callno)
{
    int nelmts = t->syscall_arg(0);
    uint32_t list_ptr = t->syscall_arg(1);
    long maxgroups = sysconf(_SC_NGROUPS_MAX);

    if (nelmts>maxgroups) {
        t->syscall_return(-EINVAL);
        return;
    }
    
    gid_t *list = new gid_t[nelmts];
    size_t nread = t->get_process()->mem_read(list, list_ptr, nelmts*sizeof(*list));
    if (nread!=nelmts*sizeof(*list)) {
        t->syscall_return(-EFAULT);
        delete[] list;
        return;
    }

    /* The write lock is required by syscall_getgroups32() */
    int retval = 0;
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(t->get_process()->rwlock());
        retval = setgroups(nelmts, list);
        t->syscall_return(-1==retval ? -errno : retval); // capture errno before we lose it
    }

    delete[] list;
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_fchown32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchown32").d().d().d();
}

void
RSIM_Linux32::syscall_fchown32_body(RSIM_Thread *t, int callno)
{
    int guestFd=t->syscall_arg(0), user=t->syscall_arg(1), group=t->syscall_arg(2);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int result = syscall(SYS_fchown, hostFd, user, group);
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_getdents64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getdents64").d().p().d();
}

void
RSIM_Linux32::syscall_getdents64_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0), sz = t->syscall_arg(2);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint32_t dirent_va = t->syscall_arg(1);
    int status;
    if (4 == sizeof(long)) {
        status = getdents_syscall<dirent64_32, dirent64_32>(t, SYS_getdents64, hostFd, dirent_va, sz);
    } else {
        status = getdents_syscall<dirent64_32, dirent_64>(t, SYS_getdents, hostFd, dirent_va, sz);
    }
    t->syscall_return(status);
}

void
RSIM_Linux32::syscall_getdents64_leave(RSIM_Thread *t, int callno)
{
    int status = t->syscall_arg(-1);
    t->syscall_leave().ret().arg(1).P(status>0?status:0, print_dentries64_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_futex_enter(RSIM_Thread *t, int callno)
{
    /* We cannot include <linux/futex.h> portably across a variety of Linux machines. */
    static const Translate opflags[] = {
        TF3(0xff, 0x80, FUTEX_PRIVATE_FLAG|FUTEX_WAIT),
        TF3(0x80, 0x80, FUTEX_PRIVATE_FLAG),
        TF3(0x100, 0x100, FUTEX_CLOCK_REALTIME),
        TF3(0x7f, 0, FUTEX_WAIT),
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

    uint32_t op = t->syscall_arg(1);
    switch (op & 0x7f) {
        case 0: /*FUTEX_WAIT*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d().P(sizeof(timespec_32), print_timespec_32);
            break;
        case 1: /*FUTEX_WAKE*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d();
            break;
        case 2: /*FUTEX_FD*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d();
            break;
        case 3: /*FUTEX_REQUEUE*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d().unused().P(4, print_int_32);
            break;
        case 4: /*FUTEX_CMP_REQUEUE*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d().unused().P(4, print_int_32).d();
            break;
        case 9: /*FUTEX_WAIT_BITSET*/
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d().P(sizeof(timespec_32), print_timespec_32).unused().x();
            break;
        default:
            t->syscall_enter("futex").P(4, print_int_32).f(opflags).d().P(sizeof(timespec_32), print_timespec_32)
                .P(4, print_int_32).d();
            break;
    }
}

void
RSIM_Linux32::syscall_futex_body(RSIM_Thread *t, int callno)
{
    /* Variable arguments */
    uint32_t futex1_va = t->syscall_arg(0);
    uint32_t op = t->syscall_arg(1);
    uint32_t val1 = t->syscall_arg(2);
    uint32_t timeout_va = 0;                    /* arg 3 when present */
    uint32_t val3 = 0;                          /* arg 5 when present */

    int result = -ENOSYS;
    switch (op & 0x7f) {
        case 0: /*FUTEX_WAIT*/
            timeout_va = t->syscall_arg(3);
            assert(0==timeout_va); // NOT HANDLED YET
            result = t->futex_wait(futex1_va, val1);
            break;
        case 1: /*FUTEX_WAKE*/
            result = t->futex_wake(futex1_va, val1);
            break;
        case 2: /*FUTEX_FD*/
            abort(); // NOT HANDLED YET
        case 3: /*FUTEX_REQUEUE*/ {
#if 0 // [Robb Matzke 2019-04-09]
            uint32_t futex2_va = t->syscall_arg(4);
            break;
#else
            abort(); // NOT HANDLED YET
#endif
        }

        case 4: /*FUTEX_CMP_REQUEUE*/ {
#if 0 // [Robb Matzke 2019-04-09]
            uint32_t futex2_va = t->syscall_arg(4);
            val3 = t->syscall_arg(5);
            break;
#else
            abort(); // NOT HANDLED YET
#endif
        }

        case 9: /*FUTEX_WAIT_BITSET*/
            timeout_va = t->syscall_arg(3);
            ASSERT_always_require(0 == timeout_va); // NOT HANDLED YET
            val3 = t->syscall_arg(5);
            result = t->futex_wait(futex1_va, val1, val3/*bitset*/);
            break;
        default: {
#if 0 // [Robb Matzke 2019-04-09]
            timeout_va =t->syscall_arg(3);
            uint32_t futex2_va = t->syscall_arg(4);
            val3 = t->syscall_arg(5);
            break;
#else
            abort(); // NOT HANDLED YET
#endif
        }
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_futex_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(4, print_int_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_sched_getaffinity_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_getaffinity").d().d().p();
}

void
RSIM_Linux32::syscall_sched_getaffinity_body(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    
    size_t cpuset_nbits = t->syscall_arg(1);
    size_t cpuset_nbytes = (cpuset_nbits+7) / 8;
    uint8_t buf[cpuset_nbytes+sizeof(long)]; /* overallocated */

    int result = sched_getaffinity(pid, cpuset_nbits, (cpu_set_t*)buf);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    rose_addr_t cpuset_va = t->syscall_arg(2);
    if (cpuset_nbytes!=t->get_process()->mem_write(buf, cpuset_va, cpuset_nbytes)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_sched_getaffinity_leave(RSIM_Thread *t, int callno)
{
    size_t cpuset_nbits = t->syscall_arg(1);
    size_t cpuset_nbytes = (cpuset_nbits+7) / 8;
    t->syscall_leave().ret().arg(2).P(cpuset_nbytes, print_bitvec);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_set_thread_area_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("set_thread_area").P(sizeof(SegmentDescriptor), print_SegmentDescriptor);
}

void
RSIM_Linux32::syscall_set_thread_area_body(RSIM_Thread *t, int callno)
{
    SegmentDescriptor ud;
    if (sizeof(ud)!=t->get_process()->mem_read(&ud, t->syscall_arg(0), sizeof ud)) {
        t->syscall_return(-EFAULT);
        return;
    }
    int old_idx = ud.entry_number;
    int new_idx = t->set_thread_area(ud, true);
    if (new_idx<0) {
        t->syscall_return(new_idx);
        return;
    }
    if (old_idx!=new_idx && sizeof(ud)!=t->get_process()->mem_write(&ud, t->syscall_arg(0), sizeof ud)) {
        t->syscall_return(-EFAULT);
        return;
    }
    t->syscall_return(0);
}

void
RSIM_Linux32::syscall_set_thread_area_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(SegmentDescriptor), print_SegmentDescriptor);
}


/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_clock_settime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_settime").e(clock_names).P(sizeof(timespec_32), print_timespec_32);
}

void
RSIM_Linux32::syscall_clock_settime_body(RSIM_Thread *t, int callno)
{
    timespec_32 guest_ts;
    if (sizeof(guest_ts)!=t->get_process()->mem_read(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
        t->syscall_return(-EFAULT);
        return;
    }

    static timespec host_ts;
    host_ts.tv_sec = guest_ts.tv_sec;
    host_ts.tv_nsec = guest_ts.tv_nsec;
    int result = syscall(SYS_clock_settime, t->syscall_arg(0), &host_ts);
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_clock_gettime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_gettime").e(clock_names).p();
}

void
RSIM_Linux32::syscall_clock_gettime_body(RSIM_Thread *t, int callno)
{
    static timespec host_ts;
    int result = syscall(SYS_clock_gettime, t->syscall_arg(0), &host_ts);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    timespec_32 guest_ts;
    guest_ts.tv_sec = host_ts.tv_sec;
    guest_ts.tv_nsec = host_ts.tv_nsec;
    if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_clock_gettime_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(timespec_32), print_timespec_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_clock_getres_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_getres").e(clock_names).p();
}

void
RSIM_Linux32::syscall_clock_getres_body(RSIM_Thread *t, int callno)
{
    static timespec host_ts;
    timespec *host_tsp = t->syscall_arg(1) ? &host_ts : NULL;
    int result = syscall(SYS_clock_getres, t->syscall_arg(0), host_tsp);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (t->syscall_arg(1)) {
        timespec_32 guest_ts;
        guest_ts.tv_sec = host_ts.tv_sec;
        guest_ts.tv_nsec = host_ts.tv_nsec;
        if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_clock_getres_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(timespec_32), print_timespec_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_statfs64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("statfs64").s().d().p();
}

void
RSIM_Linux32::syscall_statfs64_body(RSIM_Thread *t, int callno)
{
    ROSE_ASSERT(t->syscall_arg(1)==sizeof(statfs64_32));
    bool error;
    std::string path = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    statfs64_32 guest_statfs;
#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    int result = syscall(SYS_statfs64, path.c_str(), sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else           /* host is 64-bit machine */
    static statfs_native host_statfs;
    int result = syscall(SYS_statfs, path.c_str(), &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }
    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(2), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_statfs64_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(2).P(sizeof(statfs64_32), print_statfs64_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_fstatfs64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fstatfs64").d().d().p();
}

void
RSIM_Linux32::syscall_fstatfs64_body(RSIM_Thread *t, int callno)
{
    int guestFd                          = t->syscall_arg(0);
    size_t sb_sz __attribute__((unused)) = t->syscall_arg(1);
    uint32_t sb_va                       = t->syscall_arg(2);

    assert(sb_sz==sizeof(statfs64_32));
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);

    statfs64_32 guest_statfs;
#ifdef SYS_statfs64 /* host is a 32-bit machine */
    static statfs64_native host_statfs;
    int result = syscall(SYS_fstatfs64, hostFd, sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else           /* host is 64-bit machine */
    static statfs_native host_statfs;
    int result = syscall(SYS_fstatfs, hostFd, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }
    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, sb_va, sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux32::syscall_fstatfs64_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(2).P(sizeof(statfs64_32), print_statfs64_32);
}

/*******************************************************************************************************************************/

void
RSIM_Linux32::syscall_utimes_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("utimes").s();
}

void
RSIM_Linux32::syscall_utimes_body(RSIM_Thread *t, int callno)
{
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
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    //Check to see if times is NULL
    uint8_t byte;
    size_t nread = t->get_process()->mem_read(&byte, t->syscall_arg(1), 1);
    ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

    int result;
    if( byte ) {

        size_t size_timeval_sample = sizeof(timeval_32)*2;

        timeval_32 ubuf[2];

        size_t nread = t->get_process()->mem_read(&ubuf, t->syscall_arg(1), size_timeval_sample);

        timeval timeval64[2];
        timeval64[0].tv_sec  = ubuf[0].tv_sec;
        timeval64[0].tv_usec = ubuf[0].tv_usec;
        timeval64[1].tv_sec  = ubuf[1].tv_sec;
        timeval64[1].tv_usec = ubuf[1].tv_usec;

        ROSE_ASSERT(nread == size_timeval_sample);

        result = utimes(filename.c_str(), timeval64);

    } else {
        result = utimes(filename.c_str(), NULL);
    }

    t->syscall_return(result);
}

/*******************************************************************************************************************************/


#endif /* ROSE_ENABLE_SIMULATOR */
