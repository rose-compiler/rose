#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/I386Linux/Architecture.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryExit.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/StringUtility.h>

#include <boost/format.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

static const unsigned i386_NR_mmap   = 192;             //  | 0x40000000; // __X32_SYSCALL_BIT
static const unsigned i386_NR_munmap = 91;              //  | 0x40000000; // __X32_SYSCALL_BIT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
hashMemoryRegion(Combinatorics::Hasher &hasher, const MemoryMap::Ptr &map, AddressInterval where) {
    while (!where.isEmpty()) {
        uint8_t buffer[4096];
        size_t nToRead = std::min(where.size(), sizeof buffer);
        AddressInterval regionRead = map->at(where.least()).limit(nToRead).read(buffer);
        ASSERT_require(!regionRead.isEmpty() && regionRead.size() == nToRead);
        hasher.append(buffer, nToRead);
        if (regionRead.greatest() == where.greatest())
            break;                              // prevents overflow in next statement
        where = AddressInterval::hull(regionRead.greatest() + 1, where.greatest());
    }
}

static std::string
syscallName(int n) {
    switch (n) {
        case 1: return "exit";
        case 2: return "fork";
        case 3: return "read";
        case 4: return "write";
        case 5: return "open";
        case 6: return "close";
        case 7: return "waitpid";
        case 8: return "creat";
        case 9: return "link";
        case 10: return "unlink";
        case 11: return "execve";
        case 12: return "chdir";
        case 13: return "time";
        case 14: return "mknod";
        case 15: return "chmod";
        case 16: return "lchown";
        case 17: return "break";
        case 18: return "oldstat";
        case 19: return "lseek";
        case 20: return "getpid";
        case 21: return "mount";
        case 22: return "umount";
        case 23: return "setuid";
        case 24: return "getuid";
        case 25: return "stime";
        case 26: return "ptrace";
        case 27: return "alarm";
        case 28: return "oldfstat";
        case 29: return "pause";
        case 30: return "utime";
        case 31: return "stty";
        case 32: return "gtty";
        case 33: return "access";
        case 34: return "nice";
        case 35: return "ftime";
        case 36: return "sync";
        case 37: return "kill";
        case 38: return "rename";
        case 39: return "mkdir";
        case 40: return "rmdir";
        case 41: return "dup";
        case 42: return "pipe";
        case 43: return "times";
        case 44: return "prof";
        case 45: return "brk";
        case 46: return "setgid";
        case 47: return "getgid";
        case 48: return "signal";
        case 49: return "geteuid";
        case 50: return "getegid";
        case 51: return "acct";
        case 52: return "umount2";
        case 53: return "lock";
        case 54: return "ioctl";
        case 55: return "fcntl";
        case 56: return "mpx";
        case 57: return "setpgid";
        case 58: return "ulimit";
        case 59: return "oldolduname";
        case 60: return "umask";
        case 61: return "chroot";
        case 62: return "ustat";
        case 63: return "dup2";
        case 64: return "getppid";
        case 65: return "getpgrp";
        case 66: return "setsid";
        case 67: return "sigaction";
        case 68: return "sgetmask";
        case 69: return "ssetmask";
        case 70: return "setreuid";
        case 71: return "setregid";
        case 72: return "sigsuspend";
        case 73: return "sigpending";
        case 74: return "sethostname";
        case 75: return "setrlimit";
        case 76: return "getrlimit";
        case 77: return "getrusage";
        case 78: return "gettimeofday";
        case 79: return "settimeofday";
        case 80: return "getgroups";
        case 81: return "setgroups";
        case 82: return "select";
        case 83: return "symlink";
        case 84: return "oldlstat";
        case 85: return "readlink";
        case 86: return "uselib";
        case 87: return "swapon";
        case 88: return "reboot";
        case 89: return "readdir";
        case 90: return "mmap";
        case 91: return "munmap";
        case 92: return "truncate";
        case 93: return "ftruncate";
        case 94: return "fchmod";
        case 95: return "fchown";
        case 96: return "getpriority";
        case 97: return "setpriority";
        case 98: return "profil";
        case 99: return "statfs";
        case 100: return "fstatfs";
        case 101: return "ioperm";
        case 102: return "socketcall";
        case 103: return "syslog";
        case 104: return "setitimer";
        case 105: return "getitimer";
        case 106: return "stat";
        case 107: return "lstat";
        case 108: return "fstat";
        case 109: return "olduname";
        case 110: return "iopl";
        case 111: return "vhangup";
        case 112: return "idle";
        case 113: return "vm86old";
        case 114: return "wait4";
        case 115: return "swapoff";
        case 116: return "sysinfo";
        case 117: return "ipc";
        case 118: return "fsync";
        case 119: return "sigreturn";
        case 120: return "clone";
        case 121: return "setdomainname";
        case 122: return "uname";
        case 123: return "modify_ldt";
        case 124: return "adjtimex";
        case 125: return "mprotect";
        case 126: return "sigprocmask";
        case 127: return "create_module";
        case 128: return "init_module";
        case 129: return "delete_module";
        case 130: return "get_kernel_syms";
        case 131: return "quotactl";
        case 132: return "getpgid";
        case 133: return "fchdir";
        case 134: return "bdflush";
        case 135: return "sysfs";
        case 136: return "personality";
        case 137: return "afs_syscall";
        case 138: return "setfsuid";
        case 139: return "setfsgid";
        case 140: return "_llseek";
        case 141: return "getdents";
        case 142: return "_newselect";
        case 143: return "flock";
        case 144: return "msync";
        case 145: return "readv";
        case 146: return "writev";
        case 147: return "getsid";
        case 148: return "fdatasync";
        case 149: return "_sysctl";
        case 150: return "mlock";
        case 151: return "munlock";
        case 152: return "mlockall";
        case 153: return "munlockall";
        case 154: return "sched_setparam";
        case 155: return "sched_getparam";
        case 156: return "sched_setscheduler";
        case 157: return "sched_getscheduler";
        case 158: return "sched_yield";
        case 159: return "sched_get_priority_max";
        case 160: return "sched_get_priority_min";
        case 161: return "sched_rr_get_interval";
        case 162: return "nanosleep";
        case 163: return "mremap";
        case 164: return "setresuid";
        case 165: return "getresuid";
        case 166: return "vm86";
        case 167: return "query_module";
        case 168: return "poll";
        case 169: return "nfsservctl";
        case 170: return "setresgid";
        case 171: return "getresgid";
        case 172: return "prctl";
        case 173: return "rt_sigreturn";
        case 174: return "rt_sigaction";
        case 175: return "rt_sigprocmask";
        case 176: return "rt_sigpending";
        case 177: return "rt_sigtimedwait";
        case 178: return "rt_sigqueueinfo";
        case 179: return "rt_sigsuspend";
        case 180: return "pread64";
        case 181: return "pwrite64";
        case 182: return "chown";
        case 183: return "getcwd";
        case 184: return "capget";
        case 185: return "capset";
        case 186: return "sigaltstack";
        case 187: return "sendfile";
        case 188: return "getpmsg";
        case 189: return "putpmsg";
        case 190: return "vfork";
        case 191: return "ugetrlimit";
        case 192: return "mmap2";
        case 193: return "truncate64";
        case 194: return "ftruncate64";
        case 195: return "stat64";
        case 196: return "lstat64";
        case 197: return "fstat64";
        case 198: return "lchown32";
        case 199: return "getuid32";
        case 200: return "getgid32";
        case 201: return "geteuid32";
        case 202: return "getegid32";
        case 203: return "setreuid32";
        case 204: return "setregid32";
        case 205: return "getgroups32";
        case 206: return "setgroups32";
        case 207: return "fchown32";
        case 208: return "setresuid32";
        case 209: return "getresuid32";
        case 210: return "setresgid32";
        case 211: return "getresgid32";
        case 212: return "chown32";
        case 213: return "setuid32";
        case 214: return "setgid32";
        case 215: return "setfsuid32";
        case 216: return "setfsgid32";
        case 217: return "pivot_root";
        case 218: return "mincore";
        case 219: return "madvise";
        case 220: return "getdents64";
        case 221: return "fcntl64";
        case 224: return "gettid";
        case 225: return "readahead";
        case 226: return "setxattr";
        case 227: return "lsetxattr";
        case 228: return "fsetxattr";
        case 229: return "getxattr";
        case 230: return "lgetxattr";
        case 231: return "fgetxattr";
        case 232: return "listxattr";
        case 233: return "llistxattr";
        case 234: return "flistxattr";
        case 235: return "removexattr";
        case 236: return "lremovexattr";
        case 237: return "fremovexattr";
        case 238: return "tkill";
        case 239: return "sendfile64";
        case 240: return "futex";
        case 241: return "sched_setaffinity";
        case 242: return "sched_getaffinity";
        case 243: return "set_thread_area";
        case 244: return "get_thread_area";
        case 245: return "io_setup";
        case 246: return "io_destroy";
        case 247: return "io_getevents";
        case 248: return "io_submit";
        case 249: return "io_cancel";
        case 250: return "fadvise64";
        case 252: return "exit_group";
        case 253: return "lookup_dcookie";
        case 254: return "epoll_create";
        case 255: return "epoll_ctl";
        case 256: return "epoll_wait";
        case 257: return "remap_file_pages";
        case 258: return "set_tid_address";
        case 259: return "timer_create";
        case 260: return "timer_settime";
        case 261: return "timer_gettime";
        case 262: return "timer_getoverrun";
        case 263: return "timer_delete";
        case 264: return "clock_settime";
        case 265: return "clock_gettime";
        case 266: return "clock_getres";
        case 267: return "clock_nanosleep";
        case 268: return "statfs64";
        case 269: return "fstatfs64";
        case 270: return "tgkill";
        case 271: return "utimes";
        case 272: return "fadvise64_64";
        case 273: return "vserver";
        case 274: return "mbind";
        case 275: return "get_mempolicy";
        case 276: return "set_mempolicy";
        case 277: return "mq_open";
        case 278: return "mq_unlink";
        case 279: return "mq_timedsend";
        case 280: return "mq_timedreceive";
        case 281: return "mq_notify";
        case 282: return "mq_getsetattr";
        case 283: return "kexec_load";
        case 284: return "waitid";
        case 286: return "add_key";
        case 287: return "request_key";
        case 288: return "keyctl";
        case 289: return "ioprio_set";
        case 290: return "ioprio_get";
        case 291: return "inotify_init";
        case 292: return "inotify_add_watch";
        case 293: return "inotify_rm_watch";
        case 294: return "migrate_pages";
        case 295: return "openat";
        case 296: return "mkdirat";
        case 297: return "mknodat";
        case 298: return "fchownat";
        case 299: return "futimesat";
        case 300: return "fstatat64";
        case 301: return "unlinkat";
        case 302: return "renameat";
        case 303: return "linkat";
        case 304: return "symlinkat";
        case 305: return "readlinkat";
        case 306: return "fchmodat";
        case 307: return "faccessat";
        case 308: return "pselect6";
        case 309: return "ppoll";
        case 310: return "unshare";
        case 311: return "set_robust_list";
        case 312: return "get_robust_list";
        case 313: return "splice";
        case 314: return "sync_file_range";
        case 315: return "tee";
        case 316: return "vmsplice";
        case 317: return "move_pages";
        case 318: return "getcpu";
        case 319: return "epoll_pwait";
        case 320: return "utimensat";
        case 321: return "signalfd";
        case 322: return "timerfd_create";
        case 323: return "eventfd";
        case 324: return "fallocate";
        case 325: return "timerfd_settime";
        case 326: return "timerfd_gettime";
        case 327: return "signalfd4";
        case 328: return "eventfd2";
        case 329: return "epoll_create1";
        case 330: return "dup3";
        case 331: return "pipe2";
        case 332: return "inotify_init1";
        case 333: return "preadv";
        case 334: return "pwritev";
        case 335: return "rt_tgsigqueueinfo";
        case 336: return "perf_event_open";
        case 337: return "recvmmsg";
        case 338: return "fanotify_init";
        case 339: return "fanotify_mark";
        case 340: return "prlimit64";
        case 341: return "name_to_handle_at";
        case 342: return "open_by_handle_at";
        case 343: return "clock_adjtime";
        case 344: return "syncfs";
        case 345: return "sendmmsg";
        case 346: return "setns";
        case 347: return "process_vm_readv";
        case 348: return "process_vm_writev";
        case 349: return "kcmp";
        case 350: return "finit_module";
        case 351: return "sched_setattr";
        case 352: return "sched_getattr";
        case 353: return "renameat2";
        case 354: return "seccomp";
        case 355: return "getrandom";
        case 356: return "memfd_create";
        case 357: return "bpf";
        case 358: return "execveat";
        case 359: return "socket";
        case 360: return "socketpair";
        case 361: return "bind";
        case 362: return "connect";
        case 363: return "listen";
        case 364: return "accept4";
        case 365: return "getsockopt";
        case 366: return "setsockopt";
        case 367: return "getsockname";
        case 368: return "getpeername";
        case 369: return "sendto";
        case 370: return "sendmsg";
        case 371: return "recvfrom";
        case 372: return "recvmsg";
        case 373: return "shutdown";
        case 374: return "userfaultfd";
        case 375: return "membarrier";
        case 376: return "mlock2";
        case 377: return "copy_file_range";
        case 378: return "preadv2";
        case 379: return "pwritev2";
        case 380: return "pkey_mprotect";
        case 381: return "pkey_alloc";
        case 382: return "pkey_free";
        case 383: return "statx";
        case 384: return "arch_prctl";
    }

    return "sys" + boost::lexical_cast<std::string>(n);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscall callback base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallBase::SyscallBase() {}

SyscallBase::~SyscallBase() {}

bool
SyscallBase::operator()(bool /*handled*/, SyscallContext &ctx) {
    // Since a system call might have multiple callbacks, and since all of them inherit from this class, they will all end up
    // calling this code. Some things (such as creating new events and single stepping the concrete process) should only be
    // done once, so be careful!
    hello("linux i386 base class", ctx);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    auto i386 = as<Architecture>(ctx.architecture);
    ASSERT_not_null(i386);
    const RegisterDescriptor SYS_RET = i386->systemCallReturnRegister();
    penultimateReturnEvent_ = latestReturnEvent_;
    latestReturnEvent_ = ExecutionEvent::Ptr();

    if (ConcolicPhase::REPLAY == ctx.phase) {
        // Pick out the return value and save it for later. The return value will have been created as an event that describes
        // the side effect of writing the return value to the return register. When replaying, the return value will be
        // concrete.
        if (!ctx.returnEvent) {
            for (const ExecutionEvent::Ptr &relatedEvent: ctx.relatedEvents) {
                if (relatedEvent->action() == ExecutionEvent::Action::REGISTER_WRITE &&
                    relatedEvent->registerDescriptor() == SYS_RET) {
                    ctx.returnEvent = relatedEvent;
                    break;
                }
            }
        }
        ASSERT_not_null(ctx.returnEvent);
        latestReturnEvent_ = ctx.returnEvent;

        // Allow the child class to do its thing.
        playback(ctx);

    } else {
        // For non-replay (i.e., concolic execution) we should make sure that there's a return event if no other callback has
        // already created one. If we create one here, then also give it an input variable to affect the behavior of future
        // test cases for this same specimen (subsequent callbacks can remove this variable if desired).
        if (!ctx.returnEvent) {
            handlePreSyscall(ctx);

            // after this, we're in the syscall-exit-stop state
            i386->debugger()->stepIntoSystemCall(Debugger::ThreadId::unspecified());

            // If the syscall terminated the program, we should still allow the subclass to handle things even though we're
            // not creating a return event or input variable for the return.
            if (i386->debugger()->isTerminated()) {
                ASSERT_require(ctx.returnEvent == nullptr);
                handlePostSyscall(ctx);
                ASSERT_not_reachable("unexpected concrete termination in system call");
            }

            // Create the input variable and execution event for this return value.
            if (!ctx.symbolicReturn) {
                ctx.symbolicReturn = SymbolicExpression::makeIntegerVariable(SYS_RET.nBits(), ctx.syscallEvent->name() + "_return");
                SAWYER_MESG(debug) <<"  created input variable " <<*ctx.symbolicReturn <<"\n";
            } else {
                SAWYER_MESG(debug) <<"  using existing variable " <<*ctx.symbolicReturn <<"\n";
                ASSERT_require(ctx.symbolicReturn->nBits() == SYS_RET.nBits());
            }
            uint64_t retConcrete = i386->debugger()->readRegister(Debugger::ThreadId::unspecified(), SYS_RET).toInteger();
            SymbolicExpression::Ptr retValue = SymbolicExpression::makeIntegerConstant(SYS_RET.nBits(), retConcrete);
            SAWYER_MESG(debug) <<"  concrete return value: " <<*retValue <<"\n";
            ctx.returnEvent = ExecutionEvent::registerWrite(i386->testCase(), i386->nextEventLocation(When::POST),
                                                            ctx.syscallEvent->instructionPointer(), SYS_RET,
                                                            ctx.symbolicReturn, retValue, ctx.symbolicReturn);
            ctx.ops->inputVariables()->activate(ctx.returnEvent, InputType::SYSCALL_RET);
            SAWYER_MESG(debug) <<"  created " <<ctx.returnEvent->printableName(i386->database()) <<"\n";

            // Update the symbolic state
            BS::SValue::Ptr retSValue = ctx.ops->svalueExpr(ctx.symbolicReturn);
            ctx.ops->writeRegister(SYS_RET, retSValue);
            SAWYER_MESG(debug) <<"  return value saved in symbolic state: " <<*retSValue <<"\n";
        }

        latestReturnEvent_ = ctx.returnEvent;
        handlePostSyscall(ctx);
    }
    return true;                                        // handled
}

void
SyscallBase::hello(const std::string &name, const SyscallContext &ctx) const {
    if (name.empty()) {
        SyscallCallback::hello("", ctx);
    } else {
        SyscallCallback::hello(name + " for " + syscallName(ctx.syscallEvent->syscallFunction()) + " system call", ctx);
    }
}

ExecutionEvent::Ptr
SyscallBase::latestReturnEvent() const {
    return latestReturnEvent_;
}

ExecutionEvent::Ptr
SyscallBase::penultimateReturnEvent() const {
    return penultimateReturnEvent_;
}


SymbolicExpression::Ptr
SyscallBase::penultimateSymbolicReturn() const {
    if (!penultimateReturnEvent_) {
        return SymbolicExpression::Ptr();
    } else if (SymbolicExpression::Ptr variable = penultimateReturnEvent_->inputVariable()) {
        return variable;
    } else {
        ASSERT_require(penultimateReturnEvent_->action() == ExecutionEvent::Action::REGISTER_WRITE);
        return penultimateReturnEvent_->value();
    }
}

void
SyscallBase::showRecentReturnValues(std::ostream &out, const SyscallContext &ctx) const {
    Database::Ptr db = ctx.architecture->database();

    if (!latestReturnEvent_) {
        out <<"  this system call has not yet returned\n";
    } else {
        out <<"  latest return event: " <<latestReturnEvent_->printableName(db) <<"\n";
        out <<"  latest return concrete: "
            <<*latestReturnEvent_->calculateResult(ctx.ops->inputVariables()->bindings()) <<"\n";
    }

    if (!penultimateReturnEvent_) {
        out <<"  this is the first occurrence of this system call\n";
    } else {
        out <<"  penultimate return event: " <<penultimateReturnEvent_->printableName(db) <<"\n";
        if (penultimateReturnEvent_->variable())
            out <<"  penultimate return variable: " <<*penultimateReturnEvent_->variable() <<"\n";
        out <<"  penultimate return concrete: "
            <<*penultimateReturnEvent_->calculateResult(ctx.ops->inputVariables()->bindings()) <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System calls that are unimplemented
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallUnimplemented::SyscallUnimplemented() {}

SyscallUnimplemented::~SyscallUnimplemented() {}

SyscallCallback::Ptr
SyscallUnimplemented::instance() {
    return Ptr(new SyscallUnimplemented);
}

void
SyscallUnimplemented::playback(SyscallContext &ctx) {
    hello("not-implemented", ctx);
    mlog[ERROR] <<"  " <<syscallName(ctx.syscallEvent->syscallFunction()) <<" system call is not implemented\n";
}

void
SyscallUnimplemented::handlePostSyscall(SyscallContext &ctx) {
    hello("not-implemented", ctx);
    mlog[ERROR] <<"  " <<syscallName(ctx.syscallEvent->syscallFunction()) <<" system call is not implemented\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System calls whose only behavior is to return value that's a program input.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallReturnsInput::SyscallReturnsInput() {}

SyscallReturnsInput::~SyscallReturnsInput() {}

SyscallCallback::Ptr
SyscallReturnsInput::instance() {
    return Ptr(new SyscallReturnsInput);
}

void
SyscallReturnsInput::playback(SyscallContext &ctx) {
    hello("returns-input", ctx);
}

void
SyscallReturnsInput::handlePostSyscall(SyscallContext &ctx) {
    hello("returns-input", ctx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System calls that terminate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallTerminates::SyscallTerminates() {}

SyscallTerminates::~SyscallTerminates() {}

SyscallCallback::Ptr
SyscallTerminates::instance() {
    return Ptr(new SyscallTerminates);
}

void
SyscallTerminates::playback(SyscallContext &ctx) {
    hello("syscall-exits-process", ctx);
    ASSERT_not_reachable("cannot occur during startup phase");
}

void
SyscallTerminates::handlePostSyscall(SyscallContext &ctx) {
    hello("syscall-exits-process", ctx);
    auto ops = Emulation::RiscOperators::promote(ctx.ops);
    ops->doExit(ctx.argsConcrete[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System call return constraints.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallReturn::SyscallReturn() {}

SyscallReturn::~SyscallReturn() {}

void
SyscallReturn::handlePostSyscall(SyscallContext &ctx) {
    hello("syscall-return-constraint", ctx);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    auto i386 = as<Architecture>(ctx.architecture);
    ASSERT_not_null(i386);
    const RegisterDescriptor SYS_RET = i386->systemCallReturnRegister();
    showRecentReturnValues(debug, ctx);

    // If this is the first invocation of this syscall, then there's nothing we need to do.
    if (!penultimateReturnEvent())
        return;

    // Build the SMT solver constraint that the current return value must be equal to the previous return value.
    std::pair<SymbolicExpression::Ptr, Sawyer::Optional<uint64_t>> x = makeReturnConstraint(ctx);
    SymbolicExpression::Ptr constraint = x.first;
    Sawyer::Optional<uint64_t> concreteReturn = x.second;

    if (constraint) {
        SAWYER_MESG(mlog[DEBUG]) <<"  return value constraint: " <<*constraint <<"\n";
        ctx.ops->solver()->insert(constraint);
    } else {
        SAWYER_MESG(mlog[DEBUG]) <<"  return value constraint: none\n";
    }

    if (concreteReturn) {
        SAWYER_MESG(mlog[DEBUG]) <<"  modifying concrete state to have returned "
                                 <<StringUtility::toHex2(*concreteReturn, SYS_RET.nBits()) <<"\n";
        i386->debugger()->writeRegister(Debugger::ThreadId::unspecified(), SYS_RET, *concreteReturn);
    } else {
        SAWYER_MESG(mlog[DEBUG]) <<"  not modifying concrete return value\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System calls that are constant, always returning the same value.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallConstant::SyscallConstant() {}

SyscallConstant::~SyscallConstant() {}

SyscallCallback::Ptr
SyscallConstant::instance() {
    return Ptr(new SyscallConstant);
}

void
SyscallConstant::playback(SyscallContext&) {}

std::pair<SymbolicExpression::Ptr, Sawyer::Optional<uint64_t>>
SyscallConstant::makeReturnConstraint(SyscallContext &ctx) {
    SymbolicExpression::Ptr constraint;
    Sawyer::Optional<uint64_t> concreteReturn;

    if (penultimateReturnEvent()) {
        if (latestReturnEvent()->inputVariable()) {
            SymbolicExpression::Ptr curRet = latestReturnEvent()->inputVariable();
            SymbolicExpression::Ptr prevRet = penultimateSymbolicReturn();
            ASSERT_not_null(prevRet);
            constraint = SymbolicExpression::makeEq(curRet, prevRet);
        }

        concreteReturn = penultimateReturnEvent()->calculateResult(ctx.ops->inputVariables()->bindings())->toUnsigned();
    }

    return {constraint, concreteReturn};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System calls that return non-decreasing values, such as time
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallNondecreasing::SyscallNondecreasing() {}

SyscallNondecreasing::~SyscallNondecreasing() {}

SyscallCallback::Ptr
SyscallNondecreasing::instance() {
    return Ptr(new SyscallNondecreasing);
}

void
SyscallNondecreasing::playback(SyscallContext&) {}

std::pair<SymbolicExpression::Ptr, Sawyer::Optional<uint64_t>>
SyscallNondecreasing::makeReturnConstraint(SyscallContext &ctx) {
    SymbolicExpression::Ptr constraint;
    Sawyer::Optional<uint64_t> concreteReturn;

    if (penultimateReturnEvent()) {
        if (latestReturnEvent()->inputVariable()) {
            SymbolicExpression::Ptr curRet = latestReturnEvent()->inputVariable();
            SymbolicExpression::Ptr prevRet = penultimateSymbolicReturn();
            ASSERT_not_null(prevRet);
            constraint = SymbolicExpression::makeGe(curRet, prevRet);
        }

        concreteReturn = penultimateReturnEvent()->calculateResult(ctx.ops->inputVariables()->bindings())->toUnsigned();
    }

    return {constraint, concreteReturn};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access system call
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallAccess::SyscallAccess() {}

SyscallAccess::~SyscallAccess() {}

SyscallCallback::Ptr
SyscallAccess::instance() {
    return Ptr(new SyscallAccess);
}

void
SyscallAccess::playback(SyscallContext &ctx) {
    hello("I386Linux::SyscallAccess::playback", ctx);
}

void
SyscallAccess::handlePostSyscall(SyscallContext &ctx) {
    hello("I386Linux::SyscallAccess::handlePostSyscall", ctx);
    if (mlog[DEBUG]) {
        rose_addr_t fileNameVa = ctx.argsConcrete[0];
        std::string fileName = ctx.architecture->partitioner()->memoryMap()->readString(fileNameVa, 16384);
        mlog[DEBUG] <<"  pathname = \"" <<StringUtility::cEscape(fileName) <<"\"\n";
        mlog[DEBUG] <<"  mode     = " <<StringUtility::toHex(ctx.argsConcrete[1]) <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Brk system call
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallBrk::SyscallBrk() {}

SyscallBrk::~SyscallBrk() {}

SyscallCallback::Ptr
SyscallBrk::instance() {
    return Ptr(new SyscallBrk);
}

void
SyscallBrk::playback(SyscallContext &ctx) {
    hello("I386Linux::SyscallBrk::playback", ctx);
    ASSERT_not_implemented("[Robb Matzke 2021-12-16]");
}

void
SyscallBrk::handlePostSyscall(SyscallContext &ctx) {
    hello("I386Linux::SyscallBrk::handlePostSyscall", ctx);

    rose_addr_t endVa = 0;
    if (!ctx.returnEvent->calculateResult(ctx.ops->inputVariables()->bindings())->toUnsigned().assignTo(endVa)) {
        SAWYER_MESG(mlog[ERROR]) <<"no concrete return value from brk system call\n";
        return;
    }

    if (ctx.returnEvent)
        notAnInput(ctx, ctx.returnEvent);

    // The easy way to add more memory
    MemoryMap::Ptr map = ctx.ops->process()->partitioner()->memoryMap();
    auto events = ctx.ops->process()->createMemoryAdjustEvents(map, ctx.ip);
    ctx.relatedEvents.insert(ctx.relatedEvents.end(), events.begin(), events.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mmap2 system call
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallMmap2::SyscallMmap2() {}
SyscallMmap2::~SyscallMmap2() {}

SyscallCallback::Ptr
SyscallMmap2::instance() {
    return Ptr(new SyscallMmap2);
}

void
SyscallMmap2::playback(SyscallContext &ctx) {
    hello("I386Linux::SyscallMmap2::playback", ctx);
    ASSERT_not_implemented("[Robb Matzke 2021-12-20]");
}

void
SyscallMmap2::handlePostSyscall(SyscallContext &ctx) {
    hello("I386Linux::SyscallMmap2::handlePostSyscall", ctx);

    if (ctx.returnEvent)
        notAnInput(ctx, ctx.returnEvent);

    MemoryMap::Ptr map = ctx.ops->process()->partitioner()->memoryMap();
    auto events = ctx.ops->process()->createMemoryAdjustEvents(map, ctx.ip);
    ctx.relatedEvents.insert(ctx.relatedEvents.end(), events.begin(), events.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Openat system call
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallOpenat::SyscallOpenat() {}
SyscallOpenat::~SyscallOpenat() {}

SyscallCallback::Ptr
SyscallOpenat::instance() {
    return Ptr(new SyscallOpenat);
}

void
SyscallOpenat::playback(SyscallContext &ctx) {
    hello("I386Linux::SyscallOpenat::playback", ctx);
}

void
SyscallOpenat::handlePostSyscall(SyscallContext &ctx) {
    hello("I386Linux::SyscallOpenat::handlPostSyscall", ctx);
    if (mlog[DEBUG]) {
        rose_addr_t fileNameVa = ctx.argsConcrete[1];
        std::string fileName = ctx.architecture->partitioner()->memoryMap()->readString(fileNameVa, 16384);
        mlog[DEBUG] <<"  pathname = \"" <<StringUtility::cEscape(fileName) <<"\"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System call definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Architecture::configureSystemCalls() {
    // These are the generally useful configurations. Feel free to override these to accomplish whatever kind of testing you
    // need.

    // SYS_exit
    systemCalls(1, SyscallTerminates::instance());

    // SYS_time
    systemCalls(13, SyscallNondecreasing::instance());

    // SYS_getpid
    systemCalls(20, SyscallConstant::instance());

    // SYS_getuid: assumes SYS_setuid is never successfully called
    systemCalls(24, SyscallConstant::instance());

    // SYS_access
    systemCalls(33, SyscallAccess::instance());

    // SYS_brk
    systemCalls(45, SyscallBrk::instance());

    // SYS_getgid: assumes SYS_setgid is never successfully called
    systemCalls(47, SyscallConstant::instance());

    // SYS_geteuid: assumes SYS_setuid is never successfully called
    systemCalls(50, SyscallConstant::instance());

    // SYS_getppid
    systemCalls(64, SyscallConstant::instance());

    // SYS_getpgrp
    systemCalls(65, SyscallConstant::instance());

    // SYS_mmap2
    systemCalls(192, SyscallMmap2::instance());

    // SYS_exit_group
    systemCalls(252, SyscallTerminates::instance());

    // SYS_openat
    systemCalls(295, SyscallOpenat::instance());

    // SYS_arch_prctl
    systemCalls(384, SyscallReturnsInput::instance());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shared memory behaviors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Architecture::configureSharedMemory(const Yaml::Node &config) {
    Super::configureSharedMemory(config);

    // Null dereferences
    auto handler = Callback::MemoryExit::instance(AddressInterval::baseSize(0, 4096), 255);
    handler->name("null pointer dereference"),

    sharedMemory(handler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Architecture
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Architecture::Architecture(const std::string &name)
    : Concolic::Architecture(name) {}

Architecture::Architecture(const Database::Ptr &db, TestCaseId tcid)
    : Concolic::Architecture(db, tcid) {}

Architecture::~Architecture() {}

Architecture::Ptr
Architecture::factory() {
    return Ptr(new Architecture("I386Linux"));
}

Architecture::Ptr
Architecture::instance(const Database::Ptr &db, TestCaseId tcid, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_require(tcid);
    auto retval = Ptr(new Architecture(db, tcid));
    retval->configureSystemCalls();
    retval->configureSharedMemory(config);
    return retval;
}

Architecture::Ptr
Architecture::instance(const Database::Ptr &db, const TestCase::Ptr &tc, const Yaml::Node &config) {
    return instance(db, db->id(tc), config);
}

Concolic::Architecture::Ptr
Architecture::instanceFromFactory(const Database::Ptr &db, TestCaseId tcid, const Yaml::Node &config) const {
    ASSERT_require(isFactory());
    auto retval = instance(db, tcid, config);
    retval->name(name());
    return retval;
}

bool
Architecture::matchFactory(const Yaml::Node &config) const {
    return config["architecture"].as<std::string>() == name();
}

Debugger::Linux::Ptr
Architecture::debugger() const {
    ASSERT_not_null(Super::debugger());
    auto retval = as<Debugger::Linux>(Super::debugger());
    ASSERT_not_null(retval);
    return retval;
}

P2::Partitioner::Ptr
Architecture::partition(const P2::Engine::Ptr &engine, const std::string &specimenName) {
    const std::string specimenArg = "run:replace:" + specimenName;
    SAWYER_MESG(mlog[DEBUG]) <<"partitioning " <<specimenArg;
    return engine->partition(specimenArg);
}

BS::Dispatcher::Ptr
Architecture::makeDispatcher(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    auto arch = BinaryAnalysis::Architecture::findByName("intel-pentium4").orThrow();
    return IS::DispatcherX86::instance(arch, ops);
}

void
Architecture::load(const boost::filesystem::path &targetDir) {
    ASSERT_forbid(isFactory());

    // Extract the executable into the target temporary directory.
    auto exeName = boost::filesystem::path(testCase()->specimen()->name()).filename();
    if (exeName.empty())
        exeName = "a.out";
    exeName = targetDir / exeName;
    {
        std::ofstream executable(exeName.string().c_str(), std::ios_base::binary | std::ios_base::trunc);
        if (!executable) {
            mlog[ERROR] <<"cannot write to " <<exeName <<"\n";
        } else if (testCase()->specimen()->content().empty()) {
            mlog[ERROR] <<"specimen content is empty\n";
        } else {
            executable.write(reinterpret_cast<const char*>(&testCase()->specimen()->content()[0]),
                             testCase()->specimen()->content().size());
        }
    }
    boost::filesystem::permissions(exeName, boost::filesystem::owner_all);

    // Describe the process to be created from the executable. The TestCase arguments include argv[0], but we don't have any
    // control over that in general, so we discard it. Other executors might have more control, which is why the program name
    // is supplied in the first place.
    std::vector<std::string> args = testCase()->args();
    ASSERT_forbid(args.empty());
    args.erase(args.begin());
    Debugger::Linux::Specimen ds = exeName;
    ds.arguments(args);
    ds.workingDirectory(targetDir);
    ds.randomizedAddresses(false);
    ds.flags()
        .set(Debugger::Linux::Flag::REDIRECT_INPUT)
        .set(Debugger::Linux::Flag::REDIRECT_OUTPUT)
        .set(Debugger::Linux::Flag::REDIRECT_ERROR)
        .set(Debugger::Linux::Flag::CLOSE_FILES);

    // Create the process
    Super::debugger(Debugger::Linux::instance(ds));
    SAWYER_MESG(mlog[DEBUG]) <<"loaded pid=" <<*debugger()->processId() <<" " <<exeName <<"\n";
    mapScratchPage();
}

ByteOrder::Endianness
Architecture::memoryByteOrder() {
    return ByteOrder::ORDER_LSB;
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryRestoreEvents() {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[DEBUG]) <<"saving subordinate memory\n";
    auto map = MemoryMap::instance();
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    map->insertProcessPid(*debugger()->processId(), segments);
    return Super::createMemoryRestoreEvents(map);
}

std::vector<ExecutionEvent::Ptr>
Architecture::copyMemory(const MemoryMap::Ptr &srcMap, const MemoryMap::Ptr &dstMap, const AddressInterval &where,
                         rose_addr_t insnVa) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(srcMap);
    ASSERT_not_null(dstMap);
    std::vector<ExecutionEvent::Ptr> retval;

    if (!where.isEmpty()) {
        // Copy data from source to destination map, shared.
        auto srcNode = srcMap->find(where.least());
        ASSERT_forbid(srcNode == srcMap->nodes().end());
        MemoryMap::Segment segment = srcNode->value();
        segment.offset(segment.offset() + where.least() - srcNode->key().least());
        dstMap->insert(where, segment);

        // Create the map event
        auto eeMap = ExecutionEvent::bulkMemoryMap(testCase(), nextEventLocation(When::POST), insnVa, where, segment.accessibility());
        eeMap->name("map " + StringUtility::addrToString(where) + " " + segment.name());
        SAWYER_MESG(mlog[DEBUG]) <<"  created " <<eeMap->printableName(database()) <<"\n";
        retval.push_back(eeMap);

        // Create the initialization event
        std::vector<uint8_t> buf(where.size());
        size_t nRead = srcMap->at(where).read(buf).size();
        ASSERT_always_require(nRead == buf.size());
        auto eeInit = ExecutionEvent::bulkMemoryWrite(testCase(), nextEventLocation(When::POST), insnVa, where, buf);
        eeInit->name("init " + StringUtility::addrToString(where) + " " + segment.name());
        SAWYER_MESG(mlog[DEBUG]) <<"  created " <<eeInit->printableName(database()) <<"\n";
        retval.push_back(eeInit);
    }

    return retval;
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryAdjustEvents(const MemoryMap::Ptr &oldMap, rose_addr_t insnVa) {
    ASSERT_forbid(isFactory());
    std::vector<ExecutionEvent::Ptr> retval;
    SAWYER_MESG(mlog[DEBUG]) <<"saving memory map adjustments\n";
    std::vector<ExecutionEvent::Ptr> events;
    auto newMap = MemoryMap::instance();
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    newMap->insertProcessPid(*debugger()->processId(), segments);

#if 1 // DEBUGGING [Robb Matzke 2021-12-20]
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  old map:\n";
        oldMap->dump(mlog[DEBUG], "    ");
        mlog[DEBUG] <<"  new map:\n";
        newMap->dump(mlog[DEBUG], "    ");
    }
#endif

    // Addresses that exist in the old map but not the new map are removed from the old map and unmap events are generated.
    rose_addr_t oldVa = 0;
    while (AddressInterval oldWhere = oldMap->atOrAfter(oldVa).available()) {
        AddressInterval newWhere = newMap->atOrAfter(oldWhere.least()).available();
        rose_addr_t lastProcessedVa = 0;

        if (!newWhere) {
            // No addresses here or higher in new map, so delete the rest of the old map
            SAWYER_MESG(mlog[DEBUG]) <<"  erasing " <<StringUtility::addrToString(oldWhere)
                                     <<" " <<StringUtility::plural(oldWhere.size(), "bytes") <<"\n";
            oldMap->erase(oldWhere);
            lastProcessedVa = oldWhere.greatest();
            auto event = ExecutionEvent::bulkMemoryUnmap(testCase(), nextEventLocation(When::POST), insnVa, oldWhere);
            event->name("unmap " + StringUtility::addrToString(oldWhere));
            retval.push_back(event);

        } else if (newWhere.least() > oldWhere.least()) {
            // The next mapped interval in the new address map starts after the current address, so delete all parts of the old
            // map up to where the new map starts.
            auto toErase = AddressInterval::hull(oldWhere.least(), newWhere.least() - 1);
            SAWYER_MESG(mlog[DEBUG]) <<"  erasing " <<StringUtility::addrToString(toErase)
                                     <<" " <<StringUtility::plural(toErase.size(), "bytes") <<"\n";
            oldMap->erase(toErase);
            lastProcessedVa = toErase.greatest();
            auto event = ExecutionEvent::bulkMemoryUnmap(testCase(), nextEventLocation(When::POST), insnVa, toErase);
            event->name("unmap " + StringUtility::addrToString(toErase));
            retval.push_back(event);

        } else {
            // The new map has data at the old map's current address, so we can fast forward across the new addresses.
            ASSERT_require(newWhere.least() == oldWhere.least());
            lastProcessedVa = newWhere.greatest();
        }

        // Increment the address, but be careful of overflow.
        if (lastProcessedVa < AddressInterval::whole().greatest()) {
            oldVa = lastProcessedVa + 1;
        } else {
            break;
        }
    }

    // Addresses that exist in the new map but not the old map are added to the old map and map events are generated.
    rose_addr_t newVa = 0;
    while (AddressInterval newWhere = newMap->atOrAfter(newVa).singleSegment().available()) {
        AddressInterval oldWhere = oldMap->atOrAfter(newWhere.least()).singleSegment().available();
        rose_addr_t lastProcessedVa = 0;


        if (!oldWhere) {
            // Old map doesn't contain any subsequent address from the new map.
            SAWYER_MESG(mlog[DEBUG]) <<"  inserting " <<StringUtility::addrToString(newWhere) <<"\n";
            lastProcessedVa = newWhere.greatest();
            auto events = copyMemory(newMap, oldMap, newWhere, insnVa);
            retval.insert(retval.end(), events.begin(), events.end());

        } else if (oldWhere.least() > newWhere.least()) {
            // New map contains some addresses not in the old map.
            auto toInsert = AddressInterval::hull(newWhere.least(), std::min(newWhere.greatest(), oldWhere.least() - 1));
            SAWYER_MESG(mlog[DEBUG]) <<"  inserting " <<StringUtility::addrToString(toInsert) <<"\n";
            lastProcessedVa = toInsert.greatest();
            auto events = copyMemory(newMap, oldMap, toInsert, insnVa);
            retval.insert(retval.end(), events.begin(), events.end());

        } else {
            // Both maps have some of the same data.
            ASSERT_require(newWhere.least() == oldWhere.least());
            lastProcessedVa = oldWhere.least();
        }

        // Increment the address, but be careful of overflow.
        if (lastProcessedVa < AddressInterval::whole().greatest()) {
            newVa = lastProcessedVa + 1;
        } else {
            break;
        }
    }

#if 1 // DEBUGGING [Robb Matzke 2021-12-20]
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  adjusted map:\n";
        oldMap->dump(mlog[DEBUG], "    ");
    }
#endif
    return retval;
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryHashEvents() {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[DEBUG]) <<"hashing subordinate memory\n";
    std::vector<ExecutionEvent::Ptr> events;
    auto map = MemoryMap::instance();
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    map->insertProcessPid(*debugger()->processId(), segments);
    for (const MemoryMap::Node &node: map->nodes()) {
        SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(node.key())
                                 <<StringUtility::plural(node.key().size(), "bytes") <<"\n";
        Combinatorics::HasherSha256Builtin hasher;
        hashMemoryRegion(hasher, map, node.key());
        SAWYER_MESG(mlog[DEBUG]) <<"    hash = " <<hasher.toString() <<"\n";
        auto eeHash = ExecutionEvent::bulkMemoryHash(TestCase::Ptr(), ExecutionLocation(), ip(), node.key(), hasher.digest());
        events.push_back(eeHash);
    }
    return events;
}

bool
Architecture::playEvent(const ExecutionEvent::Ptr &event) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(event);
    bool handled = Super::playEvent(event);

    switch (event->action()) {
        case ExecutionEvent::Action::OS_SYSCALL:
            if (!handled) {
                // If it wasn't handled by Super, then it must be because there are no callbacks.
                const uint64_t functionNumber = event->syscallFunction();
                SyscallCallbacks callbacks = systemCalls().getOrDefault(functionNumber);
                ASSERT_require(callbacks.isEmpty());
                callbacks.append(SyscallUnimplemented::instance());
                SyscallContext ctx(sharedFromThis(), event, getRelatedEvents(event));
                return callbacks.apply(false, ctx);
            }
            return true;

        default:
            return handled;
    }
}

void
Architecture::mapMemory(const AddressInterval &where, unsigned permissions) {
    ASSERT_forbid(isFactory());
    ASSERT_forbid(where.isEmpty());
    SAWYER_MESG(mlog[DEBUG]) <<"map " <<StringUtility::plural(where.size(), "bytes") <<" ";
    unsigned prot = 0;
    if ((permissions & MemoryMap::READABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"r";
        prot |= PROT_READ;
    }
    if ((permissions & MemoryMap::WRITABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"w";
        prot |= PROT_WRITE;
    }
    if ((permissions & MemoryMap::EXECUTABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"x";
        prot |= PROT_EXEC;
    }
    SAWYER_MESG(mlog[DEBUG]) <<" at " <<StringUtility::addrToString(where) <<", flags=private|anonymous|fixed\n";
    int32_t status = debugger()->remoteSystemCall(Debugger::ThreadId::unspecified(), i386_NR_mmap, where.least(), where.size(),
                                                  prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (status < 0 && status > -4096) {
        mlog[ERROR] <<"MAP_MEMORY event failed to map memory (" <<strerror(-status) <<")\n";
    } else {
        ASSERT_require((uint64_t)(uint32_t)status == where.least());
    }
}

void
Architecture::unmapMemory(const AddressInterval &where) {
    ASSERT_forbid(isFactory());
    ASSERT_forbid(where.isEmpty());
    SAWYER_MESG(mlog[DEBUG]) <<"unmap " <<StringUtility::plural(where.size(), "bytes")
                             << " at " <<StringUtility::addrToString(where) <<"\n";
    int64_t status = debugger()->remoteSystemCall(Debugger::ThreadId::unspecified(), i386_NR_munmap, where.least(), where.size());
    if (status < 0)
        mlog[ERROR] <<"UNMAP_MEMORY event failed to unmap memory\n";
}

void
Architecture::mapScratchPage() {
    ASSERT_forbid(isFactory());
    ASSERT_require(debugger()->isAttached());

    // Create the scratch page
    int64_t status = debugger()->remoteSystemCall(Debugger::ThreadId::unspecified(), i386_NR_mmap, 0, 4096,
                                                  PROT_EXEC | PROT_READ | PROT_WRITE,
                                                  MAP_ANONYMOUS | MAP_PRIVATE,
                                                  -1, 0);
    if (status < 0 && status > -4096) {
        mlog[ERROR] <<"mmap system call failed for scratch page: " <<strerror(-status) <<"\n";
    } else {
        scratchVa((uint64_t)(uint32_t)status);
        SAWYER_MESG(mlog[DEBUG]) <<"scratch page mapped at " <<StringUtility::addrToString(scratchVa()) <<"\n";

        // Write an "INT 0x80" instruction to the beginning of the page.
        static const uint8_t int80[] = {0xcd, 0x80};
        size_t nWritten = debugger()->writeMemory(*scratchVa(), 2, int80);
        if (nWritten != 2)
            mlog[ERROR] <<"cannot write INT 0x80 instruction to scratch page\n";
    }
}

std::vector<MemoryMap::ProcessMapRecord>
Architecture::disposableMemory() {
    ASSERT_forbid(isFactory());
    std::vector<MemoryMap::ProcessMapRecord> segments = MemoryMap::readProcessMap(*debugger()->processId());
    for (auto segment = segments.begin(); segment != segments.end(); /*void*/) {
        ASSERT_forbid(segment->interval.isEmpty());
        if ("[vvar]" == segment->comment) {
            // Reading and writing to this memory segment doesn't work
            segment = segments.erase(segment);
        } else if ("[vdso]" == segment->comment) {
            // Pointless to read and write this segment -- its contents never changes
            segment = segments.erase(segment);
        } else if (scratchVa() && segment->interval.least() == *scratchVa()) {
            // This segment is for our own personal use
            segment = segments.erase(segment);
        } else {
            ++segment;
        }
    }
    return segments;
}

void
Architecture::unmapAllMemory() {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[DEBUG]) <<"unmapping memory\n";
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    for (const MemoryMap::ProcessMapRecord &segment: segments) {
        SAWYER_MESG(mlog[DEBUG]) <<"  at " <<StringUtility::addrToString(segment.interval) <<": " <<segment.comment <<"\n";
        int64_t status = debugger()->remoteSystemCall(Debugger::ThreadId::unspecified(), i386_NR_munmap, segment.interval.least(),
                                                      segment.interval.size());
        if (status < 0) {
            mlog[ERROR] <<"unamp memory failed at " <<StringUtility::addrToString(segment.interval)
                        <<" for " <<segment.comment <<"\n";
        }
    }
}

void
Architecture::createInputVariables(const P2::PartitionerConstPtr &partitioner, const Emulation::RiscOperators::Ptr &ops,
                                const SmtSolver::Ptr &solver) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    ASSERT_not_null(solver);

    // For Linux ELF x86, the argc and argv values are on the stack, not in registers. Also note that there is no return
    // address on the stack (i.e., the stack pointer is pointing at argc, not a return address). This means all the stack
    // argument offsets are four (or eight) bytes less than usual.
    //
    // The stack looks like this:
    //   +-------------------------------------------------+
    //   | argv[argc] 4-byte zero                          |
    //   +-------------------------------------------------+
    //   | ...                                             |
    //   +-------------------------------------------------+
    //   | argv[1], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argv[0], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argc value, 4-byte integer, e.g., 2             |  <--- ESP points here
    //   +-------------------------------------------------+
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"creating program arguments\n";
    const RegisterDescriptor SP = partitioner->instructionProvider().stackPointerRegister();
    size_t wordSizeBytes = SP.nBits() / 8;
    SymbolicExpression::Formatter fmt;
    fmt.show_comments = SymbolicExpression::Formatter::CMT_AFTER;

    //---------------------------------------------------------------------------------------------------------------------------
    // argc
    //---------------------------------------------------------------------------------------------------------------------------
    ASSERT_require(SP.nBits() == 32);                   // we only handle 32-bit for now
    ASSERT_require(ops->currentState()->memoryState()->get_byteOrder() == memoryByteOrder());
    rose_addr_t argcVa = readRegister(SP).toInteger();
    size_t argc = readMemoryUnsigned(argcVa, wordSizeBytes);

    // Event and input variable
    SymbolicExpression::Ptr argcVariable = SymbolicExpression::makeIntegerVariable(SP.nBits(), "argc");
    SymbolicExpression::Ptr argcValue = SymbolicExpression::makeIntegerConstant(SP.nBits(), argc);
    auto argcEvent = ExecutionEvent::memoryWrite(testCase(), nextEventLocation(When::PRE), ip(),
                                                 AddressInterval::baseSize(argcVa, SP.nBits()/8),
                                                 argcVariable, argcValue, argcVariable);
    inputVariables()->activate(argcEvent, InputType::ARGC);

    // Adjust symbolic state
    ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), argcVa), ops->svalueExpr(argcVariable), ops->boolean_(true));
    ExecutionEventId argcEventId = database()->id(argcEvent);
    SAWYER_MESG(debug) <<"  argc @" <<StringUtility::addrToString(argcVa) <<" = " <<argc
                       <<"; symbolic = " <<(*argcVariable + fmt)
                       <<"; event = " <<*argcEventId <<"\n";

    // The argc value cannot be less than 1 since it always points to at least the program name.
    SymbolicExpression::Ptr argcMinConstraint =
        SymbolicExpression::makeSignedGt(argcVariable, SymbolicExpression::makeIntegerConstant(SP.nBits(), 0));
    solver->insert(argcMinConstraint);

    // The argc value cannot be greater than its current concrete value since making it greater would mean that the address
    // of the environment variable list would need to change, potentially affecting many other things in the program.
    SymbolicExpression::Ptr argcMaxConstraint = SymbolicExpression::makeSignedLe(argcVariable, argcValue);
    solver->insert(argcMaxConstraint);

    //---------------------------------------------------------------------------------------------------------------------------
    // argv
    //---------------------------------------------------------------------------------------------------------------------------

    // The characters argv[0][...] are not inputs because we don't have much control over them.
    rose_addr_t argvVa = argcVa + wordSizeBytes;
    SAWYER_MESG(debug) <<"  argv @" <<StringUtility::addrToString(argvVa) <<" = [\n";
    for (size_t i = 1; i < argc; ++i) {
        rose_addr_t ptrVa = argvVa + i * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        std::string s = readCString(strVa);
        SAWYER_MESG(debug) <<"    " <<i <<": @" <<StringUtility::addrToString(strVa)
                           <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

        if (markingArgvAsInput_) {
            SymbolicExpression::Ptr anyPreviousCharIsNul;     // is any previous char of this argument an ASCII NUL character?
            for (size_t j = 0; j <= s.size(); ++j) {
                rose_addr_t charVa = strVa + j;

                // Event and input variable
                std::string name = (boost::format("argv_%d_%d") % i % j).str();
                SymbolicExpression::Ptr charValue = SymbolicExpression::makeIntegerConstant(8,  s.c_str()[j]);
                SymbolicExpression::Ptr charVariable = SymbolicExpression::makeIntegerVariable(8, name);
                auto charEvent = ExecutionEvent::memoryWrite(testCase(), nextEventLocation(When::PRE), ip(),
                                                             charVa, charVariable, charValue, charVariable);
                inputVariables()->activate(charEvent, InputType::ARGV, i, j);

                // Adjust symbolic state
                ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), charVa), ops->svalueExpr(charVariable),
                                 ops->boolean_(true));
                ExecutionEventId charEventId = database()->id(charEvent);
                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(charVa)
                                   <<"; symbolic = " <<(*charVariable + fmt)
                                   <<"; event = " <<*charEventId <<"\n";

                SymbolicExpression::Ptr currentCharIsNul =
                    SymbolicExpression::makeEq(charVariable, SymbolicExpression::makeIntegerConstant(8, 0));
                if (s.size() == j) {
                    // Final byte of the argument's buffer must always be NUL
                    solver->insert(currentCharIsNul);

                } else if (j > 0) {
                    // Linux doesn't allow NUL characters to appear inside program arguments. A NUL terminates the argument and
                    // the next argument starts immediately thereafter. For concolic testing, if an argument is shortened (by
                    // making one of it's non-ending bytes NUL) we don't want to have to adjust the addresses of all following
                    // arguments, environment variables, and the auxv vector because that would end up being a lot of changes
                    // to the the program. Instead, we reserve some amount of space for each argument (based on the root test
                    // case) and write NULs into the interior of arguments to shorten them.  In order to prevent some
                    // impossible inputs (arguments with interior NUL characters) we add solver constraints so that any
                    // character of an argument after a NUL is also a NUL.
                    ASSERT_not_null(anyPreviousCharIsNul);
                    auto bothNul = SymbolicExpression::makeAnd(anyPreviousCharIsNul, currentCharIsNul);
                    auto assertion = SymbolicExpression::makeOr(SymbolicExpression::makeInvert(anyPreviousCharIsNul), bothNul);
                    solver->insert(assertion);
                } else {
                    // argv[i] must be empty (i.e., first byte is NUL character) if argc <= i
                    auto argcGreaterThanI = SymbolicExpression::makeGt(argcVariable,
                                                                       SymbolicExpression::makeIntegerConstant(SP.nBits(), i));
                    auto assertion = SymbolicExpression::makeOr(argcGreaterThanI, currentCharIsNul);
                    solver->insert(assertion);
                }

                // Extend or create the expression for any previous character being NUL
                if (anyPreviousCharIsNul) {
                    anyPreviousCharIsNul = SymbolicExpression::makeOr(anyPreviousCharIsNul, currentCharIsNul);
                } else {
                    anyPreviousCharIsNul = currentCharIsNul;
                }
            }
        }
    }
    SAWYER_MESG(debug) <<"    " <<argc <<": @" <<StringUtility::addrToString(argvVa + argc * wordSizeBytes) <<" null\n"
                       <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // envp
    //---------------------------------------------------------------------------------------------------------------------------

    // Unlike for arguments, there's no count of the number of environment variables. We need a count, so the first thing we
    // do is scan the list of pointers to find the null pointer.
    const rose_addr_t envpVa = argvVa + (argc + 1) * wordSizeBytes;
    size_t envc = 0;
    while (true) {
        rose_addr_t ptrVa = envpVa + envc * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        if (0 == strVa)
            break;
        ++envc;
    }

    SAWYER_MESG(debug) <<"  envp @" <<StringUtility::addrToString(envpVa) <<" = [\n";
    for (size_t i = 0; i < envc; ++i) {
        rose_addr_t ptrVa = envpVa + i * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        std::string s = readCString(strVa);
        SAWYER_MESG(debug) <<"    " <<i <<": @" <<StringUtility::addrToString(strVa)
                           <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

        if (markingEnvpAsInput_) {
            SymbolicExpression::Ptr anyPreviousCharIsNul;     // is any previous char of this env an ASCII NUL character?
            for (size_t j = 0; j <= s.size(); ++j) {
                rose_addr_t charVa = strVa + j;

                // Event and input variable
                std::string name = (boost::format("envp_%d_%d") % i % j).str();
                SymbolicExpression::Ptr charValue = SymbolicExpression::makeIntegerConstant(8, s.c_str()[j]);
                SymbolicExpression::Ptr charVariable = SymbolicExpression::makeIntegerVariable(8, name);
                auto charEvent = ExecutionEvent::memoryWrite(testCase(), nextEventLocation(When::PRE), ip(),
                                                             charVa, charVariable, charValue, charVariable);
                inputVariables()->activate(charEvent, InputType::ENVP, i, j);

                // Adjust symbolic state
                ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), charVa), ops->svalueExpr(charVariable),
                                 ops->boolean_(true));
                ExecutionEventId charEventId = database()->id(charEvent);
                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(charVa)
                                   <<"; symbolic = " <<(*charVariable + fmt)
                                   <<"; event = " <<charEventId <<"\n";

                SymbolicExpression::Ptr currentCharIsNul =
                    SymbolicExpression::makeEq(charVariable, SymbolicExpression::makeIntegerConstant(8, 0));
                if (s.size() == j) {
                    // Final byte of the argument's buffer must always be NUL
                    solver->insert(currentCharIsNul);

                } else if (j > 0) {
                    // Linux doesn't allow NUL characters to appear inside environment variables. A NUL terminates the
                    // environment variable and the next environment variable starts immediately thereafter. For concolic
                    // testing, if an environment variable is shortened (by making one of it's non-ending bytes NUL) we don't
                    // want to have to adjust the addresses of all following environment variables and the auxv vector because
                    // that would end up being a lot of changes to the the program. Instead, we reserve some amount of space
                    // for each environment variable (based on the root test case) and write NULs into the interior of
                    // environment variables to shorten them.  In order to prevent some impossible inputs (environment
                    // variables with interior NUL characters) we add solver constraints so that any character of an
                    // environment variable after a NUL is also a NUL.
                    ASSERT_not_null(anyPreviousCharIsNul);
                    auto bothNul = SymbolicExpression::makeAnd(anyPreviousCharIsNul, currentCharIsNul);
                    auto assertion = SymbolicExpression::makeOr(SymbolicExpression::makeInvert(anyPreviousCharIsNul), bothNul);
                    solver->insert(assertion);
                } else {
                    // envp[i] must be empty (i.e., first byte is NUL character) if envc <= i
                    auto argcGreaterThanI = SymbolicExpression::makeGt(argcVariable,
                                                                       SymbolicExpression::makeIntegerConstant(SP.nBits(), i));
                    auto assertion = SymbolicExpression::makeOr(argcGreaterThanI, currentCharIsNul);
                    solver->insert(assertion);
                }

                // Extend or create the expression for any previous character being NUL
                if (anyPreviousCharIsNul) {
                    anyPreviousCharIsNul = SymbolicExpression::makeOr(anyPreviousCharIsNul, currentCharIsNul);
                } else {
                    anyPreviousCharIsNul = currentCharIsNul;
                }

            }
        }
    }
    SAWYER_MESG(debug) <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // auxv
    //---------------------------------------------------------------------------------------------------------------------------
    // Not marking these as inputs because the user that's running the program doesn't have much influence.
    rose_addr_t auxvVa = envpVa + (envc + 1) * wordSizeBytes;
    SAWYER_MESG(debug) <<"  auxv @" <<StringUtility::addrToString(auxvVa) <<" = [\n";
    size_t nAuxvPairs = 0;
    while (true) {
        rose_addr_t va = auxvVa + nAuxvPairs * 2 * wordSizeBytes;
        size_t key = readMemoryUnsigned(va, wordSizeBytes);
        size_t val = readMemoryUnsigned(va + wordSizeBytes, wordSizeBytes);
        SAWYER_MESG(debug) <<"    " <<nAuxvPairs <<": key=" <<StringUtility::addrToString(key)
                           <<", val=" <<StringUtility::addrToString(val) <<"\n";
        if (0 == key)
            break;
        ++nAuxvPairs;
    }
    SAWYER_MESG(debug) <<"  ]\n";
}

uint64_t
Architecture::systemCallFunctionNumber(const P2::PartitionerConstPtr &partitioner, const BS::RiscOperators::Ptr &ops) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);

    const RegisterDescriptor AX = partitioner->instructionProvider().registerDictionary()->findOrThrow("eax");
    BS::SValue::Ptr retvalSValue = ops->readRegister(AX);
    ASSERT_require2(retvalSValue->isConcrete(), "non-concrete system call numbers not handled yet");
    return retvalSValue->toUnsigned().get();
}

BS::SValue::Ptr
Architecture::systemCallArgument(const P2::PartitionerConstPtr &partitioner, const BS::RiscOperators::Ptr &ops, size_t idx) {
    ASSERT_forbid(isFactory());
    switch (idx) {
        case 0: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("ebx");
            return ops->readRegister(r);
        }
        case 1: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("ecx");
            return ops->readRegister(r);
        }
        case 2: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("edx");
            return ops->readRegister(r);
        }
        case 3: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("esi");
            return ops->readRegister(r);
        }
        case 4: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("edi");
            return ops->readRegister(r);
        }
        case 5: {
            const RegisterDescriptor r = partitioner->instructionProvider().registerDictionary()->findOrThrow("ebp");
            return ops->readRegister(r);
        }
    }
    ASSERT_not_reachable("system calls have at most six arguments");

    // For future reference, amd64 are: rdi, rsi, rdx, r10, r8, and r9
}

RegisterDescriptor
Architecture::systemCallReturnRegister() {
    ASSERT_forbid(isFactory());
    return RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 32);
}

BS::SValue::Ptr
Architecture::systemCallReturnValue(const P2::PartitionerConstPtr&, const BS::RiscOperators::Ptr &ops) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    const RegisterDescriptor reg = systemCallReturnRegister();
    return ops->readRegister(reg);
}

BS::SValue::Ptr
Architecture::systemCallReturnValue(const P2::PartitionerConstPtr&, const BS::RiscOperators::Ptr &ops,
                                 const BS::SValue::Ptr &retval) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    const RegisterDescriptor reg = systemCallReturnRegister();
    ops->writeRegister(reg, retval);
    return retval;
}

void
Architecture::systemCall(const P2::PartitionerConstPtr &partitioner, const BS::RiscOperators::Ptr &ops_) {
    // A system call has been encountered. The INT instruction has been processed symbolically (basically a no-op other than
    // to adjust the instruction pointer), and the concrete execution has stepped into the system call but has not yet executed
    // it (i.e., the subordinate process is in the syscall-enter-stop state).
    ASSERT_forbid(isFactory());

    auto ops = Emulation::RiscOperators::promote(ops_);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    const rose_addr_t ip = debugger()->executionAddress(Debugger::ThreadId::unspecified());

    //-------------------------------------
    // Create system call execution event.
    //-------------------------------------

    // Gather info about the system call such as its arguments. On Linux, system calls have up to six arguments stored
    // in registers, so we just grab all six for now since we don't want to maintain a big switch statement to say how
    // many arguments each system call actually uses.
    std::vector<uint64_t> argsConcrete;
    uint64_t functionNumber = systemCallFunctionNumber(partitioner, ops);
    for (size_t i = 0; i < 6; ++i) {
        BS::SValue::Ptr argSymbolic = systemCallArgument(partitioner, ops, i);
        if (auto argConcrete = argSymbolic->toUnsigned()) {
            argsConcrete.push_back(*argConcrete);
        } else {
            ASSERT_not_implemented("non-concrete system call argument");
        }
    }
    if (debug) {
        debug <<"  " <<syscallName(functionNumber) <<" system call (sys" <<functionNumber <<"), potential args:\n";
        for (uint64_t arg: argsConcrete)
            debug <<"    " <<StringUtility::toHex(arg) <<"\n";
    }

    // The execution event records the system call number and arguments, but not any side effects (because side effects haven't
    // happened yet). Since the side effect events (created shortly) are general things like "write this value to this
    // register", the fact that they're preceded by this syscall event is what marks them as being side effects of this system
    // call.
    auto syscallEvent = ExecutionEvent::osSyscall(testCase(), nextEventLocation(When::PRE), ip, functionNumber, argsConcrete);
    syscallEvent->name(syscallName(functionNumber) + "_" + boost::lexical_cast<std::string>(syscallEvent->location().primary()));
    database()->save(syscallEvent);
    SAWYER_MESG(debug) <<"  created " <<syscallEvent->printableName(database()) <<"\n";

    //-------------------------------------
    // Process the system call
    //-------------------------------------

    // Process the system call by invoking callbacks that the user can override.
    SyscallContext ctx(sharedFromThis(), ops, syscallEvent);
    SyscallCallbacks callbacks = systemCalls().getOrDefault(functionNumber);
    bool handled = callbacks.apply(false, ctx);
    if (!handled) {
        callbacks.append(SyscallUnimplemented::instance());
        callbacks.apply(false, ctx);
    }
    ASSERT_not_null(ctx.returnEvent);                   // if the syscall didn't exit, then it must have returned

    //------------------------------------
    // Record any additional side effects
    //------------------------------------

    // If there's a symbolic return value, then it must be the input variable for the return.
    if (ctx.symbolicReturn) {
        ASSERT_not_null(ctx.returnEvent);
        ASSERT_require(ctx.symbolicReturn == ctx.returnEvent->inputVariable());
    } else {
        ASSERT_require(ctx.returnEvent->inputVariable() == nullptr);
    }

    // Make sure all events have been written to the database.
    SAWYER_MESG(mlog[DEBUG]) <<"saving events related to system call:\n";
    database()->save(ctx.syscallEvent);
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<ctx.syscallEvent->printableName(database()) <<"\n";
    database()->save(ctx.returnEvent);                  // should be in relatedEvents, but just in case...
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<ctx.returnEvent->printableName(database()) <<"\n";
    for (const ExecutionEvent::Ptr &event: ctx.relatedEvents) {
        database()->save(event);
        SAWYER_MESG(mlog[DEBUG]) <<"  " <<event->printableName(database()) <<"\n";
    }
}

void
Architecture::advanceExecution(const BS::RiscOperators::Ptr &ops_) {
    ASSERT_not_null(ops_);
    ASSERT_forbid(isFactory());
    auto ops = Emulation::RiscOperators::promote(ops_);

    if (ops->hadSystemCall()) {
        debugger()->stepIntoSystemCall(Debugger::ThreadId::unspecified());
    } else {
        Super::advanceExecution(ops);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
