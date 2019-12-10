/* This file contains Linux-amd64 system call emulation.  Most of these functions are callbacks and have names like:
 *
 *    RSIM_Linux64::syscall_FOO_enter           -- prints syscall tracing info when the call is entered
 *    RSIM_Linux64::syscall_FOO                 -- implements the system call
 *    RSIM_Linux64::syscall_FOO_leave           -- prints syscall tracing info when the call returns
 */
#include <rose.h>
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux64.h"
#include <BinaryDebugger.h>                             // rose
#include <Diagnostics.h>                                // rose
#include <FileSystem.h>                                 // rose

#include <asm/prctl.h>                                  // for the arch_prctl syscall
#include <sys/prctl.h>                                  // for the arch_prctl syscall
#include <sys/syscall.h>                                // SYS_xxx definitions
#include <sys/vfs.h>                                    // for the statfs syscalls
#include <sys/wait.h>                                   // for the wait4 syscall

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

void
RSIM_Linux64::init() {
    vdsoName("linux-vdso.so");

    if (interpreterBaseVa() == 0) {
        // Linux seems to ignore the alignment constraints (0x200000) in the ld-linux-x86-64.so interpreter and uses 0x1000
        // instead. Unfortunately ROSE's BinaryLoader can't do that (2015-06-02) so we use a different load address instead.
        // interpreterBaseVa(0x00007ffff7fe1000ull); -- unsolvable alignment constraints against 0x200000
        interpreterBaseVa(0x00007ffff7000000ull);
    }

    // System call registers.
    syscallReturnRegister(RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_di,  0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_si,  0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_dx,  0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_r10, 0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_r8,  0, 64));
    syscallArgumentRegisters().push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_r9,  0, 64));

#   define SC_REG(NUM, NAME, LEAVE)                                                                                            \
        syscall_define((NUM), syscall_##NAME##_enter, syscall_##NAME##_body, syscall_##LEAVE##_leave);

    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     * according to whether ROSE is compiled for 32- or 64-bit.  We always want the 64-bit syscall numbers here. */
    SC_REG(0,   read,                           read);
    SC_REG(1,   write,                          default);
    SC_REG(2,   open,                           default);
    SC_REG(3,   close,                          default);
    SC_REG(4,   stat,                           stat);
    SC_REG(5,   stat,                           stat);  // actually fstat
    SC_REG(6,   stat,                           stat);  // actually lstat
    SC_REG(8,   lseek,                          default);
    SC_REG(9,   mmap,                           mmap);
    SC_REG(10,  mprotect,                       mprotect);
    SC_REG(11,  munmap,                         default);
    SC_REG(12,  brk,                            brk);
    SC_REG(13,  rt_sigaction,                   rt_sigaction);
    SC_REG(14,  rt_sigprocmask,                 rt_sigprocmask);
    SC_REG(16,  ioctl,                          default);
    SC_REG(20,  writev,                         default);
    SC_REG(21,  access,                         default);
    SC_REG(22,  pipe,                           pipe);
    SC_REG(24,  sched_yield,                    default);
    SC_REG(28,  madvise,                        default);
    SC_REG(32,  dup,                            default);
    SC_REG(33,  dup2,                           default);
    SC_REG(34,  pause,                          pause);
    SC_REG(35,  nanosleep,                      nanosleep);
    SC_REG(37,  alarm,                          default);
    SC_REG(39,  getpid,                         default);
    SC_REG(41,  socket,                         default);
    SC_REG(42,  connect,                        default);
    SC_REG(43,  accept,                         accept);
    SC_REG(49,  bind,                           default);
    SC_REG(50,  listen,                         default);
    SC_REG(54,  setsockopt,                     default);
    SC_REG(59,  execve,                         default);
    SC_REG(60,  exit,                           exit);
    SC_REG(61,  wait4,                          wait4);
    SC_REG(62,  kill,                           default);
    SC_REG(63,  uname,                          uname);
    SC_REG(72,  fcntl,                          fcntl);
    SC_REG(74,  fsync,                          default);
    SC_REG(77,  ftruncate,                      default);
    SC_REG(78,  getdents,                       getdents);
    SC_REG(79,  getcwd,                         getcwd);
    SC_REG(80,  chdir,                          default);
    SC_REG(81,  fchdir,                         default);
    SC_REG(82,  rename,                         default);
    SC_REG(83,  mkdir,                          default);
    SC_REG(84,  rmdir,                          default);
    SC_REG(85,  creat,                          default);
    SC_REG(86,  link,                           default);
    SC_REG(87,  unlink,                         default);
    SC_REG(88,  symlink,                        default);
    SC_REG(89,  readlink,                       default);
    SC_REG(90,  chmod,                          default);
    SC_REG(91,  fchmod,                         default);
    SC_REG(92,  chown,                          default);
    SC_REG(93,  fchown,                         default);
    SC_REG(95,  umask,                          default);
    SC_REG(96,  gettimeofday,                   gettimeofday);
    SC_REG(97,  getrlimit,                      getrlimit);
    SC_REG(99,  sysinfo,                        sysinfo);
    SC_REG(102, getuid,                         default);
    SC_REG(104, getgid,                         default);
    SC_REG(107, geteuid,                        default);
    SC_REG(108, getegid,                        default);
    SC_REG(109, setpgid,                        default);
    SC_REG(110, getppid,                        default);
    SC_REG(111, getpgrp,                        default);
    SC_REG(133, mknod,                          default);
    SC_REG(137, statfs,                         statfs);
    SC_REG(138, fstatfs,                        fstatfs);
    SC_REG(145, sched_getscheduler,             sched_getscheduler);
    SC_REG(145, sched_get_priority_max,         default);
    SC_REG(147, sched_get_priority_min,         default);
    SC_REG(158, arch_prctl,                     arch_prctl);
    SC_REG(162, sync,                           default);
    SC_REG(186, gettid,                         default);
    SC_REG(201, time,                           time);
    SC_REG(202, futex,                          futex);
    SC_REG(218, set_tid_address,                default);
    SC_REG(231, exit_group,                     exit_group);
    SC_REG(234, tgkill,                         default);
    SC_REG(268, fchmodat,                       default);
    SC_REG(273, set_robust_list,                default);
    SC_REG(293, pipe2,                          pipe2);

#   undef SC_REG
}

void
RSIM_Linux64::initializeSimulatedOs(RSIM_Process *process, SgAsmGenericHeader *hdr) {
    RSIM_Linux::initializeSimulatedOs(process, hdr);
    process->mmapNextVa(0x00007ffff7fff000ull);
    process->mmapGrowsDown(true); //  ^^-- is a maximum address
    process->mmapRecycle(true);
}

bool
RSIM_Linux64::isSupportedArch(SgAsmGenericHeader *fhdr) {
    return isSgAsmElfFileHeader(fhdr) && fhdr->get_word_size()==8;
}

void
RSIM_Linux64::loadVsyscalls(RSIM_Process *process) {
    if (process->wordSize() != 64)
        return;
    
    std::vector<std::string> paths = settings().vsyscallPaths;
    if (paths.empty()) {
        paths.push_back(".");
        paths.push_back(ROSE_AUTOMAKE_TOP_BUILDDIR + "/projects/simulator2");
        paths.push_back(ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/simulator2");
        paths.push_back(ROSE_AUTOMAKE_DATADIR + "/projects/simulator2");
    }
    AddressInterval loaded;
    std::string found;
    BOOST_FOREACH (const std::string &vsyscallPath, paths) {
        FileSystem::Path path = vsyscallPath;
        FileSystem::Path name = path / "vsyscall-amd64";
        if (FileSystem::isFile(name)) {
            found = FileSystem::toString(name);
            loaded = process->get_memory()->insertFile(":0xffffffffff600000+0x1000=rx::" + found);
            break;
        } else if (FileSystem::isFile(path)) {
            found = FileSystem::toString(name);
            loaded = process->get_memory()->insertFile(":0xffffffffff600000+0x1000=rx::" + found);
            break;
        }
    }

    // Change the name from just a file name to "[vsyscall] ..."
    BOOST_FOREACH (MemoryMap::Segment &segment, process->get_memory()->within(loaded).segments())
        segment.name("[vsyscall] " + found);
}

void
RSIM_Linux64::loadSpecimenNative(RSIM_Process *process, Disassembler *disassembler, int existingPid/*=-1*/) {
    process->mem_transaction_start("specimen main memory");
    Debugger::Ptr debugger = Debugger::instance();
    if (existingPid != -1) {
        debugger->attach(existingPid);
    } else {
        debugger->attach(exeArgs());
    }

    process->get_memory()->insertProcess(debugger->isAttached(), MemoryMap::Attach::NO);

    const RegisterDictionary *regs = disassembler->registerDictionary();
    initialRegs_.ax = debugger->readRegister(*regs->lookup("rax")).toInteger();
    initialRegs_.bx = debugger->readRegister(*regs->lookup("rbx")).toInteger();
    initialRegs_.cx = debugger->readRegister(*regs->lookup("rcx")).toInteger();
    initialRegs_.dx = debugger->readRegister(*regs->lookup("rdx")).toInteger();
    initialRegs_.si = debugger->readRegister(*regs->lookup("rsi")).toInteger();
    initialRegs_.di = debugger->readRegister(*regs->lookup("rdi")).toInteger();
    initialRegs_.flags = debugger->readRegister(*regs->lookup("rflags")).toInteger();
    initialRegs_.orig_ax = debugger->readRegister(*regs->lookup("rax")).toInteger();
    initialRegs_.ip = debugger->readRegister(*regs->lookup("rip")).toInteger();
    initialRegs_.sp = debugger->readRegister(*regs->lookup("rsp")).toInteger();
    initialRegs_.bp = debugger->readRegister(*regs->lookup("rbp")).toInteger();
    initialRegs_.cs = debugger->readRegister(*regs->lookup("cs")).toInteger();
    initialRegs_.ss = debugger->readRegister(*regs->lookup("ss")).toInteger();
    initialRegs_.r8 = debugger->readRegister(*regs->lookup("r8")).toInteger();
    initialRegs_.r9 = debugger->readRegister(*regs->lookup("r9")).toInteger();
    initialRegs_.r10 = debugger->readRegister(*regs->lookup("r10")).toInteger();
    initialRegs_.r11 = debugger->readRegister(*regs->lookup("r11")).toInteger();
    initialRegs_.r12 = debugger->readRegister(*regs->lookup("r12")).toInteger();
    initialRegs_.r13 = debugger->readRegister(*regs->lookup("r13")).toInteger();
    initialRegs_.r14 = debugger->readRegister(*regs->lookup("r14")).toInteger();
    initialRegs_.r15 = debugger->readRegister(*regs->lookup("r15")).toInteger();

    if (existingPid != -1) {
        debugger->detach();
    } else {
        debugger->terminate();
    }
    return;
}
    
PtRegs
RSIM_Linux64::initialRegistersArch(RSIM_Process*) {
    if (settings().nativeLoad)
        return initialRegs_;

    PtRegs regs;
    memset(&regs, 0, sizeof regs);
    regs.sp = 0x00007ffffffff000ull;                    // high end of stack, exclusive
    regs.flags = 2;                                     // flag bit 1 is set, although this is a reserved bit
    return regs;
}

rose_addr_t
RSIM_Linux64::pushAuxVector(RSIM_Process *process, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader *fhdr,
                            FILE *trace) {
    static const char *platform = "x86_64";
    sp -= strlen(platform)+1;
    uint64_t platformVa = sp;
    process->mem_write(platform, platformVa, strlen(platform)+1);

    // 16-bytes of random data. We use hard-coded values for reproducibility with a pattern one might recognize when debugging.
    static const uint8_t randomData[] = {
        0x00, 0x11, 0x22, 0x33,
        0xff, 0xee, 0xdd, 0xcc,
        0x88, 0x99, 0xaa, 0xbb,
        0x77, 0x66, 0x55, 0x44
    };
    sp -= sizeof randomData;
    uint64_t randomDataVa = sp;
    process->mem_write(randomData, randomDataVa, sizeof randomData);

    // Initialize the stack with auxv, where each is two words. The order and values were determined by running the simulator
    // as "x86_64 -R x86sim --show-auxv a.out" on my devel machine, Linux 2.6.32-5-amd64. [Robb P. Matzke 2015-06-02]
    auxv_.clear();
    if (vdsoMappedVa() != 0) {
        auxv_.push_back(33);
        auxv_.push_back(vdsoMappedVa());
        if (trace)
            fprintf(trace, "AT_SYSINFO_PHDR(0x21):  0x%016" PRIx64"\n", auxv_.back());
    }

    auxv_.push_back(0x10);
    auxv_.push_back(0xbfebfbfful);
    if (trace)
        fprintf(trace, "AT_HWCAP(0x10):         0x%016" PRIx64"\n", auxv_.back());

    auxv_.push_back(0x06);
    auxv_.push_back(4096);
    if (trace)
        fprintf(trace, "AT_PAGESZ(0x06):        %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x11);
    auxv_.push_back(100);
    if (trace)
        fprintf(trace, "AT_CLKTCK(0x11):        %" PRId64"\n", auxv_.back());

    auxv_.push_back(3);
    auxv_.push_back(segmentTableVa(fhdr));
    if (trace)
        fprintf(trace, "AT_PHDR(0x03):          0x%016" PRIx64"\n", auxv_.back());

    auxv_.push_back(4);
    auxv_.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk));
    if (trace)
        fprintf(trace, "AT_PHENT(0x04):         0x%" PRId64"\n", auxv_.back());

    auxv_.push_back(5);
    auxv_.push_back(fhdr->get_e_phnum());
    if (trace)
        fprintf(trace, "AT_PHNUM(0x05):         %" PRId64"\n", auxv_.back());

    auxv_.push_back(7);
    auxv_.push_back(fhdr->get_section_by_name(".interp") ? interpreterBaseVa() : 0);
    if (trace)
        fprintf(trace, "AT_BASE(0x07):          0x%016" PRIx64"\n", auxv_.back());
        
    auxv_.push_back(8);
    auxv_.push_back(0);
    if (trace)
        fprintf(trace, "AT_FLAGS(0x08):         0x%016" PRIx64"\n", auxv_.back());

    auxv_.push_back(9);
    auxv_.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
    if (trace)
        fprintf(trace, "AT_ENTRY(0x09):         0x%016" PRIx64"\n", auxv_.back());

    auxv_.push_back(0x0b);
    auxv_.push_back(getuid());
    if (trace)
        fprintf(trace, "AT_UID(0x0b):           %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x0c);
    auxv_.push_back(geteuid());
    if (trace)
        fprintf(trace, "AT_EUID(0x0c):          %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x0d);
    auxv_.push_back(getgid());
    if (trace)
        fprintf(trace, "AT_GID(0x0d):           %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x0e);
    auxv_.push_back(getegid());
    if (trace)
        fprintf(trace, "AT_EGID(0x0e):          %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x17);
    auxv_.push_back(false);
    if (trace)
        fprintf(trace, "AT_SECURE(0x17):        %" PRId64"\n", auxv_.back());

    auxv_.push_back(0x19);
    auxv_.push_back(randomDataVa);
    if (trace)
        fprintf(trace, "AT_RANDOM(0x19):        0x%016" PRIx64"\n", auxv_.back());

    auxv_.push_back(0x1f);
    auxv_.push_back(execfn_va);
    if (trace)
        fprintf(trace, "AT_EXECFN(0x1f):        0x%016" PRIx64" (%s)\n", auxv_.back(), exeArgs()[0].c_str());

    auxv_.push_back(0x0f);
    auxv_.push_back(platformVa);
    if (trace)
        fprintf(trace, "AT_PLATFORM(0x0f):      0x%016" PRIx64" (%s)\n", auxv_.back(), platform);

    // AT_NULL
    auxv_.push_back(0);
    auxv_.push_back(0);

    // Write auxv pairs
    ASSERT_require(8 == sizeof(auxv_[0]));
    sp -= 8 * auxv_.size();
    sp &= ~0xf;                                         // align to 16 byte boundary
    process->mem_write(&auxv_[0], sp, 8*auxv_.size());

    return sp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      System calls
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_accept_enter(RSIM_Thread *t, int callno) {
    ASSERT_require(sizeof(socklen_t)==4);
    t->syscall_enter("accept").d().p().P(4, print_int_32).d();
}

void
RSIM_Linux64::syscall_accept_body(RSIM_Thread *t, int callno) {
    int guestFd = t->syscall_arg(0);
    rose_addr_t addrVa = t->syscall_arg(1);
    rose_addr_t addrLenVa = t->syscall_arg(2);
    unsigned flags = t->syscall_arg(3);
    syscall_accept_helper(t, guestFd, addrVa, addrLenVa, flags);
}

void
RSIM_Linux64::syscall_accept_leave(RSIM_Thread *t, int callno) {
    rose_addr_t addrlenVa = t->syscall_arg(2);
    ASSERT_require(sizeof(socklen_t)==4);
    socklen_t addrlen = 0;
    if (sizeof(addrlen) == t->get_process()->mem_read(&addrlen, addrlenVa, sizeof(addrlen))) {
        t->syscall_leave().ret().arg(1).P(addrlen, print_sockaddr);
    } else {
        t->syscall_leave().ret();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_arch_prctl_enter(RSIM_Thread *t, int callno) {
    static const Translate code[] = { TE(ARCH_SET_FS), TE(ARCH_GET_FS), TE(ARCH_SET_GS), TE(ARCH_GET_GS), T_END };
    switch (t->syscall_arg(0)) {
        case ARCH_SET_FS:
        case ARCH_SET_GS:
            t->syscall_enter("arch_prctl").e(code).P(8, print_hex_64);
            break;
        default:
            t->syscall_enter("arch_prctl").e(code).p();
            break;
    }
}

void
RSIM_Linux64::syscall_arch_prctl_body(RSIM_Thread *t, int callno) {
    rose_addr_t va = t->syscall_arg(1);
    int retval = 0;
    switch (t->syscall_arg(0)) {
        case ARCH_SET_FS: {
            uint64_t val;
            if (t->get_process()->mem_read((uint8_t*)&val, va, sizeof val) != sizeof val) {
                retval = -EFAULT;
            } else {
                t->operators()->segmentInfo(Rose::BinaryAnalysis::x86_segreg_fs).base = val;
            }
            break;
        }
        case ARCH_SET_GS: {
            uint64_t val;
            if (t->get_process()->mem_read((uint8_t*)&val, va, sizeof val) != sizeof val) {
                retval = -EFAULT;
            } else {
                t->operators()->segmentInfo(Rose::BinaryAnalysis::x86_segreg_gs).base = val;
            }
            break;
        }
        case ARCH_GET_FS: {
            uint64_t val = t->operators()->segmentInfo(Rose::BinaryAnalysis::x86_segreg_fs).base;
            if (t->get_process()->mem_write((uint8_t*)&val, va, sizeof val) != sizeof val)
                retval = -EFAULT;
            break;
        }
        case ARCH_GET_GS: {
            uint64_t val = t->operators()->segmentInfo(Rose::BinaryAnalysis::x86_segreg_gs).base;
            if (t->get_process()->mem_write((uint8_t*)&val, va, sizeof val) != sizeof val)
                retval = -EFAULT;
            break;
        }
        default:
            retval = -ENOSYS;
            break;
    }
    t->syscall_return(retval);
}

void
RSIM_Linux64::syscall_arch_prctl_leave(RSIM_Thread *t, int callno) {
    switch (t->syscall_arg(0)) {
        case ARCH_GET_FS:
        case ARCH_GET_GS:
            t->syscall_leave().ret().arg(1).P(8, print_hex_64);
            break;
        default:
            t->syscall_leave().ret();
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_bind_enter(RSIM_Thread *t, int callno) {
    socklen_t addrlen = t->syscall_arg(2);
    t->syscall_enter("bind").d().P(addrlen, print_sockaddr).d();
}

void
RSIM_Linux64::syscall_bind_body(RSIM_Thread *t, int callno) {
    int guestFd = t->syscall_arg(0);
    rose_addr_t addrVa = t->syscall_arg(1);
    size_t addrSize = t->syscall_arg(2);
    syscall_bind_helper(t, guestFd, addrVa, addrSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_connect_enter(RSIM_Thread *t, int callno) {
    socklen_t addrlen = t->syscall_arg(2);
    t->syscall_enter("connect").d().P(addrlen, print_sockaddr).d();
}

void
RSIM_Linux64::syscall_connect_body(RSIM_Thread *t, int callno) {
    int guestFd = t->syscall_arg(0);
    rose_addr_t addrVa = t->syscall_arg(1);
    size_t addrSize = t->syscall_arg(2);
    syscall_connect_helper(t, guestFd, addrVa, addrSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_fstatfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fstatfs").d().p();
}

void
RSIM_Linux64::syscall_fstatfs_body(RSIM_Thread *t, int callno)
{
    int guestFd      = t->syscall_arg(0);
    rose_addr_t sbVa = t->syscall_arg(1);

    int hostFd = t->get_process()->hostFileDescriptor(guestFd);

    struct statfs_64 host_statfs;
    int result = syscall(SYS_fstatfs, hostFd, &host_statfs);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }
    if (sizeof(host_statfs) != t->get_process()->mem_write(&host_statfs, sbVa, sizeof host_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_fstatfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(struct statfs_64), print_statfs_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_futex_enter(RSIM_Thread *t, int callno)
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

    unsigned op = t->syscall_arg(1);
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
RSIM_Linux64::syscall_futex_body(RSIM_Thread *t, int callno)
{
    /* Variable arguments */
    rose_addr_t futex1_va = t->syscall_arg(0);
    int op = t->syscall_arg(1);
    int val1 = t->syscall_arg(2);
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
            assert(0); // NOT HANDLED YET
            break;
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
            ASSERT_always_require(0==timeout_va); // NOT HANDLED YET
            val3 = t->syscall_arg(5);
            result = t->futex_wait(futex1_va, val1, val3/*bitset*/);
            break;
        default: {
#if 0 // [Robb Matzke 2019-04-09]
            timeout_va = t->syscall_arg(3);
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
RSIM_Linux64::syscall_futex_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(4, print_int_32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_getdents_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getdents").d().p().d();
}

void
RSIM_Linux64::syscall_getdents_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0), sz = t->syscall_arg(2);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    rose_addr_t dirent_va = t->syscall_arg(1);
    int status = getdents_syscall<dirent_64, dirent_64>(t, SYS_getdents, hostFd, dirent_va, sz);
    t->syscall_return(status);
}

void
RSIM_Linux64::syscall_getdents_leave(RSIM_Thread *t, int callno)
{
    int status = t->syscall_arg(-1);
    t->syscall_leave().ret().arg(1).P(status>0?status:0, print_dentries_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_getrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getrlimit").f(rlimit_resources).p();
}

void
RSIM_Linux64::syscall_getrlimit_body(RSIM_Thread *t, int callno)
{
    int resource = t->syscall_arg(0);
    rose_addr_t rlimit_va = t->syscall_arg(1);
    struct rlimit rlimit_native;
    ASSERT_require(sizeof(rlimit_native)==16);
    int result = getrlimit(resource, &rlimit_native);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(rlimit_native) != t->get_process()->mem_write(&rlimit_native, rlimit_va, sizeof(rlimit_native))) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_getrlimit_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(16, print_rlimit_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_gettimeofday_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("gettimeofday").p();
}

void
RSIM_Linux64::syscall_gettimeofday_body(RSIM_Thread *t, int callno)
{
    rose_addr_t tp = t->syscall_arg(0);
    ASSERT_require(8 == sizeof(long));
    struct timeval host_time;

    int result = gettimeofday(&host_time, NULL);
    if (result == -1) {
        result = -errno;
    } else if (sizeof(host_time) != t->get_process()->mem_write(&host_time, tp, sizeof host_time)) {
        result = -EFAULT;
    }

    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_gettimeofday_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(timeval), print_timeval);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_ioctl_enter(RSIM_Thread *t, int callno) {
    t->syscall_enter("ioctl").str("...[not supported yet]...");
}

void
RSIM_Linux64::syscall_ioctl_body(RSIM_Thread *t, int callno) {
    t->syscall_return(-ENOTTY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_listen_enter(RSIM_Thread *t, int callno) {
    t->syscall_enter("listen").d().d();
}

void
RSIM_Linux64::syscall_listen_body(RSIM_Thread *t, int callno) {
    int guestFd = t->syscall_arg(0);
    int backlog = t->syscall_arg(1);
    syscall_listen_helper(t, guestFd, backlog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_mmap_enter(RSIM_Thread *t, int callno) {
    t->syscall_enter("mmap").p().d().f(mmap_pflags).f(mmap_mflags).d().d();
}

void
RSIM_Linux64::syscall_mmap_body(RSIM_Thread *t, int callno) {
    rose_addr_t addr = t->syscall_arg(0);
    rose_addr_t len = t->syscall_arg(1);
    unsigned linux_perms = t->syscall_arg(2);
    unsigned linux_flags = t->syscall_arg(3);
    int guestFd = t->syscall_arg(4);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    rose_addr_t offset = t->syscall_arg(5);

    unsigned rose_perms = 0;
    if (0 != (linux_perms & PROT_READ))
        rose_perms |= MemoryMap::READABLE;
    if (0 != (linux_perms & PROT_WRITE))
        rose_perms |= MemoryMap::WRITABLE;
    if (0 != (linux_perms & PROT_EXEC))
        rose_perms |= MemoryMap::EXECUTABLE;

    rose_addr_t result = t->get_process()->mem_map(addr, len, rose_perms, linux_flags, offset, hostFd);
    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_mmap_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().eret().p();
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mmap syscall:\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_setsockopt_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("setsockopt").d().d().d().p().d();
}

void
RSIM_Linux64::syscall_setsockopt_body(RSIM_Thread *t, int callno) {
    int guestFd          = t->syscall_arg(0);
    int level            = t->syscall_arg(1);
    int optName          = t->syscall_arg(2);
    rose_addr_t optvalVa = t->syscall_arg(3);
    size_t optSize       = t->syscall_arg(4);
    syscall_setsockopt_helper(t, guestFd, level, optName, optvalVa, optSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_socket_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("socket").f(protocol_families).f(socket_types).f(socket_protocols);
}

void
RSIM_Linux64::syscall_socket_body(RSIM_Thread *t, int callno)
{
    int family = t->syscall_arg(0);
    int type = t->syscall_arg(1);
    int proto = t->syscall_arg(2);
    syscall_socket_helper(t, family, type, proto);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_stat_enter(RSIM_Thread *t, int callno)
{
    switch (callno) {
        case 4:
            t->syscall_enter("stat").s().p();
            break;
        case 5:
            t->syscall_enter("fstat").d().p();
            break;
        case 6:                                         // lstat
            t->syscall_enter("lstat").s().p();
            break;
        default:
            ASSERT_not_reachable("invalid syscall number for stat family");
    }
}

void
RSIM_Linux64::syscall_stat_body(RSIM_Thread *t, int callno)
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
    ROSE_ASSERT(144==sizeof(kernel_stat_64));
    ROSE_ASSERT(8==sizeof(long));
    int host_callno = 0;
    switch (callno) {
        case 4: host_callno = SYS_stat; break;
        case 5: host_callno = SYS_fstat; break;
        case 6: host_callno = SYS_lstat; break;
        default:
            ASSERT_not_reachable("invalid syscall number for stat family");
    }
    static const size_t kernel_stat_size = sizeof(kernel_stat_64);

    static uint8_t kernel_stat[kernel_stat_size+100];
    memset(kernel_stat, 0xff, sizeof kernel_stat);
    int result = 0xdeadbeef;

    /* Make the system call without going through the C library. Well, we go through syscall(), but nothing else. */
    if (4 /*stat*/ == callno || 6 /*lstat*/ == callno) {
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
        if (-1 == hostFd) {
            result = -1;
            errno = EBADF;
        } else {
            result = syscall(host_callno, (unsigned long)hostFd, (unsigned long)kernel_stat);
        }
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

    t->get_process()->mem_write(kernel_stat, t->syscall_arg(1), kernel_stat_size);
    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_stat_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().ret().arg(1).P(sizeof(kernel_stat_64), print_kernel_stat_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_statfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("statfs").d().p();
}

void
RSIM_Linux64::syscall_statfs_body(RSIM_Thread *t, int callno)
{
    bool error;
    std::string name = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    rose_addr_t sbVa = t->syscall_arg(1);

    struct statfs_64 host_statfs;
    int result = syscall(SYS_statfs, name.c_str(), &host_statfs);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }
    if (sizeof(host_statfs) != t->get_process()->mem_write(&host_statfs, sbVa, sizeof host_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_statfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(sizeof(struct statfs_64), print_statfs_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_sysinfo_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sysinfo").p();
}

void
RSIM_Linux64::syscall_sysinfo_body(RSIM_Thread *t, int callno)
{
    sysinfo_native hostBuf;
    int result  = syscall(SYS_sysinfo, &hostBuf);

    if (-1==result) {
        t->syscall_return(-errno);
    } else {
        sysinfo_64 guestBuf(hostBuf);
        if (sizeof guestBuf != t->get_process()->mem_write(&guestBuf, t->syscall_arg(0), sizeof guestBuf)) {
            t->syscall_return(-EFAULT);
        } else {
            t->syscall_return(result);
        }
    }
}

void
RSIM_Linux64::syscall_sysinfo_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().ret().P(sizeof(sysinfo_64), print_sysinfo_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_time_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("time").p();
}

void
RSIM_Linux64::syscall_time_body(RSIM_Thread *t, int callno)
{
    time_t result = time(NULL);
    if (t->syscall_arg(0)) {
        ASSERT_require(8 == sizeof(result));
        if (8 != t->get_process()->mem_write(&result, t->syscall_arg(0), 8))
            result = -EFAULT;
    }
    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_time_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().t();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_uname_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("uname").p();
}

void
RSIM_Linux64::syscall_uname_body(RSIM_Thread *t, int callno)
{
    rose_addr_t dest_va = t->syscall_arg(0);
    new_utsname_64 buf;
    memset(&buf, ' ', sizeof buf);
    if (-1 == syscall(SYS_uname, &buf)) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(buf) != t->get_process()->mem_write(&buf, dest_va, sizeof buf)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(0);
}

void
RSIM_Linux64::syscall_uname_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(sizeof(new_utsname_64), print_new_utsname_64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_wait4_enter(RSIM_Thread *t, int callno)
{
    static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
    t->syscall_enter("wait4").d().p().f(wflags).p();
}

void
RSIM_Linux64::syscall_wait4_body(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    rose_addr_t status_va = t->syscall_arg(1), rusage_va = t->syscall_arg(3);
    int options = t->syscall_arg(2);
    int status;
    struct rusage rusage;

    // check that we can write to status_va and rusage before we wait for anything
    if (status_va && 4 != t->get_process()->mem_write(&status, status_va, 4)) {
        t->syscall_return(-EFAULT);
        return;
    }
    if (rusage_va && sizeof(rusage) != t->get_process()->mem_write(&rusage, rusage_va, sizeof rusage)) {
        t->syscall_return(-EFAULT);
        return;
    }
    
    // Now do the wait
    int result = wait4(pid, &status, options, &rusage);
    if( result == -1) {
        t->syscall_return(-errno);
        return;
    }

    // Write results to specimen memory
    if (status_va != 0)
        t->get_process()->mem_write(&status, status_va, 4);
    if (rusage_va != 0)
        t->get_process()->mem_write(&rusage, rusage_va, sizeof rusage);

    t->syscall_return(result);
}

void
RSIM_Linux64::syscall_wait4_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(1).P(4, print_exit_status_32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux64::syscall_writev_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("writev").d().p().d();
}

void
RSIM_Linux64::syscall_writev_body(RSIM_Thread *t, int callno)
{
    Sawyer::Message::Stream strace(t->tracing(TRACE_SYSCALL));
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    rose_addr_t iov_va = t->syscall_arg(1);
    int niov = t->syscall_arg(2), idx = 0;
    int retval = 0;
    if (niov<0 || niov>1024) {
        retval = -EINVAL;
    } else {
        if (niov>0)
            strace <<"\n";
        for (idx=0; idx<niov; idx++) {
            /* Obtain buffer address and size */
            mfprintf(strace)("    iov %d: ", idx);

            iovec_64 iov;
            if (sizeof(iov)!=t->get_process()->mem_read(&iov, iov_va+idx*sizeof(iov), sizeof(iov))) {
                if (0==idx)
                    retval = -EFAULT;
                strace <<"<segfault reading iovec>\n";
                break;
            }

            /* Make sure total size doesn't overflow a ssize_t */
            if ((iov.iov_len & 0x80000000) || ((uint64_t)retval+iov.iov_len) & 0x8000000000000000ull) {
                if (0==idx)
                    retval = -EINVAL;
                strace <<"<size overflow>\n";
                break;
            }

            /* Copy data from guest to host because guest memory might not be contiguous in the host. Perhaps a more
             * efficient way to do this would be to copy chunks of host-contiguous data in a loop instead. */
            uint8_t *buf = new uint8_t[iov.iov_len];
            if (iov.iov_len != t->get_process()->mem_read(buf, iov.iov_base, iov.iov_len)) {
                if (0==idx)
                    retval = -EFAULT;
                strace <<"<seg fault reading buffer>\n";
                break;
            }
            Printer::print_buffer(strace, iov.iov_base, buf, iov.iov_len, 1024);
            strace <<" (size=" <<iov.iov_len <<")";

            /* Write data to the file */
            ssize_t nwritten = write(hostFd, buf, iov.iov_len);
            delete[] buf; buf = NULL;
            if (-1==nwritten) {
                if (0==idx)
                    retval = -errno;
                mfprintf(strace)(" <write failed (%s)>\n", strerror(errno));
                break;
            }
            retval += nwritten;
            if ((uint64_t)nwritten<iov.iov_len) {
                strace <<" <short write of " <<nwritten <<" bytes>\n";
                break;
            }
            strace <<"\n";
        }
    }
    t->syscall_return(retval);
    if (niov>0 && niov<=1024)
        strace <<"writev return";
}



#endif
