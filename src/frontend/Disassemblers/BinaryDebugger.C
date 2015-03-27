#include "sage3basic.h"
#include "BinaryDebugger.h"
#include "integerOps.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <boost/config.hpp>
#ifdef BOOST_WINDOWS                                    // FIXME[Robb P. Matzke 2014-10-11]: not implemented on Windows

enum __ptrace_request {                                 // Windows dud
    PTRACE_ATTACH,
    PTRACE_CONT,
    PTRACE_DETACH,
    PTRACE_GETREGS,
    PTRACE_KILL,
    PTRACE_SETREGS,
    PTRACE_SINGLESTEP,
    PTRACE_TRACEME
};

static int SIGTRAP;                                     // Windows dud
static int SIGCONT;                                     // Windows dud
static int SIGSTOP;                                     // Windows dud

struct user_regs_struct {                               // Windows dud
    long int eip;
};

static int ptrace(__ptrace_request, int, void*, void*) {// Windows dud
    errno = ENOSYS;
    return -1;
}

static int waitpid(int, int*, int) {                    // Windows dud
    errno = ENOSYS;
    return -1;
}

static int kill(int, int) {                             // Windows dud
    errno = ENOSYS;
    return -1;
}

static int fork() {                                     // Windows dud
    errno = ENOSYS;
    return -1;
}

static int execv(const char*, char *const argv[]) {     // Windows dud
    errno = ENOSYS;
    return -1;
}

static const char *strsignal(int) {                     // Windows dud
    return "unknown";
}

static int WIFEXITED(int) { return 1; }                 // Windows dud
static int WEXITSTATUS(int) { return 1; }               // Windows dud
static int WIFSIGNALED(int) { return 0; }               // Windows dud
static int WTERMSIG(int) { return 0; }                  // Windows dud
static int WIFSTOPPED(int) { return 0; }                // Windows dud
static int WSTOPSIG(int) { return 0; }                  // Windows dud

#elif defined(__APPLE__) && defined(__MACH__)

// I don't have a Mac OSX on which to test things, so whoever does, please replace this comment with whatever's necessary to
// make the rest of the code compile.  See the windows stuff above for examples. [Robb P. Matzke 2015-02-20]
#error "FIXME[Robb P. Matzke 2015-02-20]: Not supported on Mac OSX yet"

#else

# include <fcntl.h>
# include <sys/ptrace.h>
# include <sys/user.h>
# include <sys/wait.h>
# include <unistd.h>

#endif

namespace rose {
namespace BinaryAnalysis {

static void
sendCommand(__ptrace_request request, int child, void *addr=0, void *data=0) {
    ASSERT_require2(child, "must be attached to a subordinate process");
    if (-1==ptrace(request, child, addr, data))
        throw std::runtime_error("BinaryDebugger::sendCommand failed: " + boost::to_lower_copy(std::string(strerror(errno))));
}

#if defined(BOOST_WINDOWS) || __WORDSIZE==32
static rose_addr_t
getInstructionPointer(const user_regs_struct &regs) {
    return regs.eip;
}
static void
setInstructionPointer(user_regs_struct &regs, rose_addr_t va) {
    regs.eip = va;
}
#else
static rose_addr_t
getInstructionPointer(const user_regs_struct &regs) {
    return regs.rip;
}
static void
setInstructionPointer(user_regs_struct &regs, rose_addr_t va) {
    regs.rip = va;
}
#endif

void
BinaryDebugger::init() {

    // Initialize register information.  This is very architecture and OS-dependent. See <sys/user.h> for details, but be
    // warned that even <sys/user.h> is only a guideline!  The header defines two versions of user_regs_struct, one for 32-bit
    // and the other for 64-bit based on whether ROSE is compiled as 32- or 64-bit.  I'm not sure what happens if a 32-bit
    // version of ROSE tries to analyze a 64-bit specimen; does the OS return only the 32-bit registers or does it return the
    // 64-bit versions regardless of how user_regs_struct is defined in ROSE? [Robb P. Matzke 2015-03-24]
    //
    // The userRegDefs map assigns an offset in the returned (not necessarily the defined) user_regs_struct. The register
    // descriptors in this table have sizes that correspond to the data member in the user_regs_struct, not necessarily the
    // natural size of the register (e.g., The 16-bit segment registers are listed as 32 or 64 bits).
#if defined(__linux) && defined(__x86_64) && __WORDSIZE==64
    //------------------------------------
    // Entries for 64-bit user_regs_struct
    //------------------------------------
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r15,      0, 64),   0);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r14,      0, 64),   8);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r13,      0, 64),  16);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r12,      0, 64),  24);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bp,       0, 64),  32);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bx,       0, 64),  40);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r11,      0, 64),  48);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r10,      0, 64),  56);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r9,       0, 64),  64);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r8,       0, 64),  72);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_ax,       0, 64),  80);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_cx,       0, 64),  88);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_dx,       0, 64),  96);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_si,       0, 64), 104);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_di,       0, 64), 112);
    // orig_rax: unused                                                                    120
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_ip,      0,                0, 64), 128);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_cs,    0, 64), 136);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_flags,   x86_flags_status, 0, 64), 144);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_sp,       0, 64), 152);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ss,    0, 64), 160);
    // fs_base: unused                                                                     168
    // gs_base: unused                                                                     176
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ds,    0, 64), 184);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_es,    0, 64), 192);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_fs,    0, 64), 200);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_gs,    0, 64), 208);
#elif defined(__linux) && defined(__x86_64) && __WORDSIZE==32
    //------------------------------------
    // Entries for 32-bit user_regs_struct
    //------------------------------------
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bx,       0, 32),   0);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_cx,       0, 32),   4);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_dx,       0, 32),   8);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_si,       0, 32),  12);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_di,       0, 32),  16);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bp,       0, 32),  20);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_ax,       0, 32),  24);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ds,    0, 32),  28);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_es,    0, 32),  32);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_fs,    0, 32),  36);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_gs,    0, 32),  40);
    // orig_eax: unused                                                                     44
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_ip,      0,                0, 32),  48);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_cs,    0, 32),  52);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_flags,   x86_flags_status, 0, 32),  56);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_sp,       0, 32),  60);
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ss,    0, 32),  64);
# elif defined(_MSC_VER)
#   pragma message("unable to read subordinate process registers on this platform")
# else
#   warning("unable to read subordinate process registers on this platform")
#endif
}

bool
BinaryDebugger::isTerminated() {
    return WIFEXITED(wstat_) || WIFSIGNALED(wstat_);
}

void
BinaryDebugger::waitForChild() {
    ASSERT_require2(child_, "must be attached to a subordinate process");
    if (-1 == waitpid(child_, &wstat_, 0))
        throw std::runtime_error("BinaryDebugger::waitForChild failed: " + boost::to_lower_copy(std::string(strerror(errno))));
    sendSignal_ = WIFSTOPPED(wstat_) && WSTOPSIG(wstat_)!=SIGTRAP ? WSTOPSIG(wstat_) : 0;
}

std::string
BinaryDebugger::howTerminated() {
    if (WIFEXITED(wstat_)) {
        return "exited with status " + StringUtility::numberToString(WEXITSTATUS(wstat_));
    } else if (WIFSIGNALED(wstat_)) {
        return "died with " + boost::to_lower_copy(std::string(strsignal(WTERMSIG(wstat_))));
    } else {
        return "";                                      // not terminated yet
    }
}

void
BinaryDebugger::detach() {
    if (child_ && !isTerminated()) {
        switch (howDetach_) {
            case NOTHING:
                break;
            case CONTINUE:
                kill(child_, SIGCONT);
                break;
            case DETACH:
                sendCommand(PTRACE_DETACH, child_);
                break;
            case KILL:
                sendCommand(PTRACE_KILL, child_);
                waitForChild();
        }
    }
    howDetach_ = NOTHING;
    child_ = 0;
}

void
BinaryDebugger::terminate() {
    howDetach_ = KILL;
    detach();
}

void
BinaryDebugger::attach(int child, bool attach) {
    if (-1 == child) {
        detach();
    } else if (child == child_) {
        // do nothing
    } else if (attach) {
        child_ = child;
        howDetach_ = NOTHING;
        sendCommand(PTRACE_ATTACH, child_);
        howDetach_ = DETACH;
        waitForChild();
        if (SIGSTOP==sendSignal_)
            sendSignal_ = 0;
    } else {
        child_ = child;
        howDetach_ = NOTHING;
    }
}

void
BinaryDebugger::attach(const std::string &exeName) {
    std::vector<std::string> exeNameAndArgs(1, exeName);
    attach(exeNameAndArgs);
}

void
BinaryDebugger::attach(const std::vector<std::string> &exeNameAndArgs) {
    ASSERT_forbid(exeNameAndArgs.empty());
    detach();

    child_ = fork();
    if (0==child_) {
        char **argv = new char*[exeNameAndArgs.size()+1];
        for (size_t i=0; i<exeNameAndArgs.size(); ++i)
            argv[i] = strdup(exeNameAndArgs[i].c_str());
        argv[exeNameAndArgs.size()] = NULL;

        if (-1 == ptrace(PTRACE_TRACEME, 0, 0, 0)) {
            std::cerr <<"BinaryDebugger::attach: ptrace_traceme failed: " <<strerror(errno) <<"\n";
            exit(1);
        }
        execv(argv[0], argv);
        std::cerr <<"BinaryDebugger::attach: exec \"" <<StringUtility::cEscape(argv[0]) <<"\" failed: " <<strerror(errno) <<"\n";
        exit(1);
    }

    howDetach_ = DETACH;
    waitForChild();
    if (isTerminated())
        throw std::runtime_error("BinaryDebugger::attach: subordinate " + howTerminated() + " before we gained control");
}

void
BinaryDebugger::executionAddress(rose_addr_t va) {
    user_regs_struct regs;
    sendCommand(PTRACE_GETREGS, child_, 0, &regs);
    setInstructionPointer(regs, va);
    sendCommand(PTRACE_SETREGS, child_, 0, &regs);
}

rose_addr_t
BinaryDebugger::executionAddress() {
    user_regs_struct regs;
    sendCommand(PTRACE_GETREGS, child_, 0, &regs);
    return getInstructionPointer(regs);
}

void
BinaryDebugger::setBreakpoint(const AddressInterval &va) {
    breakpoints_.insert(va);
}

void
BinaryDebugger::clearBreakpoint(const AddressInterval &va) {
    breakpoints_.erase(va);
}

void
BinaryDebugger::singleStep() {
    sendCommand(PTRACE_SINGLESTEP, child_, 0, (void*)sendSignal_);
    waitForChild();
}

uint64_t
BinaryDebugger::readRegister(const RegisterDescriptor &desc) {
    // We don't know the size of the user_regs_struct that will be returned by the kernel. It could be the 32-bit or 64-bit
    // version depending on whether the subordinate is 32 or 64 bits.  In any case, it's quite possible that it's not the
    // struct defined (based on how ROSE is compiled) in <sys/user.h>.  We use a heuristic to find the size: we allocate the
    // larger version of the struct, then fill it with magic bytes, then issue PTRACE_GETREGS.
    static const uint8_t magic = 0xb7;                  // arbitrary
    uint8_t userRegs[4096];                             // arbitrary size, but plenty large for any user_regs_struct
    memset(userRegs, magic, sizeof userRegs);
    sendCommand(PTRACE_GETREGS, child_, 0, &userRegs);

    // How much was written, approximately?
    size_t highWater = sizeof userRegs;
    while (highWater>0 && userRegs[highWater-1]==magic)
        --highWater;
    size_t wordSize = 0;
    if (highWater == 216) {
        wordSize = 64;
    } else if (highWater == 68) {
        wordSize = 32;
    } else if (highWater > 100) {
        wordSize = 64;                                  // guess
    } else {
        wordSize = 32;                                  // guess;
    }

#if 0 // DEBUGGING [Robb P. Matzke 2015-03-24]
    std::cerr <<"register=" <<desc <<"; highWater=" <<highWater <<"; wordSize=" <<wordSize <<"\n";
#endif

    // Where is the register in question?
    RegisterDescriptor base(desc.get_major(), desc.get_minor(), 0, wordSize);
    size_t userRegsStructOffset = 0;
    if (!userRegDefs_.getOptional(base).assignTo(userRegsStructOffset))
        throw std::runtime_error("register is not available");
    uint64_t baseValue = 0;
    if (32 == wordSize) {
        baseValue = *(uint32_t*)((uint8_t*)&userRegs + userRegsStructOffset);
    } else {
        baseValue = *(uint64_t*)((uint8_t*)&userRegs + userRegsStructOffset);
    }

    // Convert the baseValue to a subregister (e.g., EAX to AX) if necessary
    if (desc.get_offset() != 0 || desc.get_nbits() != base.get_nbits()) {
        ASSERT_require(desc.get_offset() + desc.get_nbits() <= base.get_nbits());
        ASSERT_require(desc.get_offset() < 64);
        baseValue >>= desc.get_offset();
        baseValue &= IntegerOps::genMask<uint64_t>(desc.get_nbits());
    }

    return baseValue;
}

size_t
BinaryDebugger::readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer) {
#ifdef __linux__
    struct T {
        int fd;
        T(): fd(-1) {}
        ~T() {
            if (-1 != fd)
                close(fd);
        }
    } mem;

    // We could use PTRACE_PEEKDATA, but it can be very slow if we're reading lots of memory since it reads only one word at a
    // time. We'd also need to worry about alignment so we don't inadvertently read past the end of a memory region when we're
    // trying to read the last byte.  Reading /proc/N/mem is faster and easier.
    std::string memName = "/proc/" + StringUtility::numberToString(child_) + "/mem";
    if (-1 == (mem.fd = open(memName.c_str(), O_RDONLY)))
        throw std::runtime_error("cannot open \"" + memName + "\": " + strerror(errno));
    if (-1 == lseek(mem.fd, va, SEEK_SET))
        throw std::runtime_error(memName + " seek failed: " + strerror(errno));
    size_t totalRead = 0;
    while (nBytes > 0) {
        ssize_t nread = read(mem.fd, buffer, nBytes);
        if (-1 == nread) {
            if (EINTR == errno)
                continue;
            return totalRead;                           // error
        } else if (0 == nread) {
            return totalRead;                           // short read
        } else {
            ASSERT_require(nread > 0);
            ASSERT_require((size_t)nread <= nBytes);
            nBytes -= nread;
            buffer += nread;
            totalRead += nread;
        }
    }
    return totalRead;
#else
# ifdef _MSC_VER
#  pragma message("reading from subordinate memory is not implemented")
# else
#  warning "reading from subordinate memory is not implemented"
# endif
    throw std::runtime_error("cannot read subordinate memory (not implemented)");
#endif
}

void
BinaryDebugger::runToBreakpoint() {
    if (breakpoints_.isEmpty()) {
        sendCommand(PTRACE_CONT, child_, 0, (void*)sendSignal_);
        waitForChild();
    } else {
        while (1) {
            singleStep();
            if (isTerminated())
                break;
            user_regs_struct regs;
            sendCommand(PTRACE_GETREGS, child_, 0, &regs);
            if (breakpoints_.exists(getInstructionPointer(regs)))
                break;
        }
    }
}

} // namespace
} // namespace
