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
    PTRACE_GETFPREGS,
    PTRACE_KILL,
    PTRACE_SETREGS,
    PTRACE_SINGLESTEP,
    PTRACE_TRACEME,
    PTRACE_PEEKUSER,
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

static long
sendCommand(__ptrace_request request, int child, void *addr=0, void *data=0) {
    ASSERT_require2(child, "must be attached to a subordinate process");
    errno = 0;
    long result = ptrace(request, child, addr, data);
    if (result == -1 && errno != 0)
        throw std::runtime_error("BinaryDebugger::sendCommand failed: " + boost::to_lower_copy(std::string(strerror(errno))));
    return result;
}

static long
sendCommandInt(__ptrace_request request, int child, void *addr, int i) {
    // Avoid doing a cast because that will cause a G++ warning.
    void *ptr = 0;
    ASSERT_require(sizeof i <= sizeof ptr);
    memcpy(&ptr, &i, sizeof i);
    return sendCommand(request, child, addr, ptr);
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
    //------------------------------------                                                 struct  struct
    // Entries for 64-bit user_regs_struct                                                 offset  size
    //------------------------------------                                                 (byte)  (bytes)
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r15,      0, 64), 0x0000); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r14,      0, 64), 0x0008); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r13,      0, 64), 0x0010); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r12,      0, 64), 0x0018); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bp,       0, 64), 0x0020); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bx,       0, 64), 0x0028); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r11,      0, 64), 0x0030); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r10,      0, 64), 0x0038); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r9,       0, 64), 0x0040); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_r8,       0, 64), 0x0048); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_ax,       0, 64), 0x0050); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_cx,       0, 64), 0x0058); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_dx,       0, 64), 0x0060); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_si,       0, 64), 0x0068); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_di,       0, 64), 0x0070); // 8
    // orig_rax: unused                                                                    0x0078   // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_ip,      0,                0, 64), 0x0080); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_cs,    0, 64), 0x0088); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_flags,   x86_flags_status, 0, 64), 0x0090); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_sp,       0, 64), 0x0098); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ss,    0, 64), 0x00a0); // 8
    // fs_base: unused                                                                     0x00a8   // 8
    // gs_base: unused                                                                     0x00b0   // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ds,    0, 64), 0x00b8); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_es,    0, 64), 0x00c0); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_fs,    0, 64), 0x00c8); // 8
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_gs,    0, 64), 0x00d0); // 8

    //--------------------------------------                                                 struct   struct
    // Entries for 64-bit user_fpregs_struct                                                 offset   size
    //--------------------------------------                                                 (byte)   (bytes)
    // cwd: unused                                                                           0x0000   // 2
    // swd: unused                                                                           0x0002   // 2
    // ftw: unused                                                                           0x0004   // 2
    // fop: unused                                                                           0x0006   // 2
    // rip: unused                                                                           0x0008   // 8
    // rdp: unused                                                                           0x0010   // 8
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_flags,   x86_flags_mxcsr,  0, 64), 0x0018); // 4
    // mxcsr_mask: unused                                                                    0x001c   // 4
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_0,         0, 64), 0x0020); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_1,         0, 64), 0x0030); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_2,         0, 64), 0x0040); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_3,         0, 64), 0x0050); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_4,         0, 64), 0x0060); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_5,         0, 64), 0x0070); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_6,         0, 64), 0x0080); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_7,         0, 64), 0x0090); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     0,                0, 64), 0x00a0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     1,                0, 64), 0x00b0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     2,                0, 64), 0x00c0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     3,                0, 64), 0x00d0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     4,                0, 64), 0x00e0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     5,                0, 64), 0x00f0); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     6,                0, 64), 0x0100); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     7,                0, 64), 0x0110); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     8,                0, 64), 0x0120); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     9,                0, 64), 0x0130); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     10,               0, 64), 0x0140); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     11,               0, 64), 0x0150); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     12,               0, 64), 0x0160); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     13,               0, 64), 0x0170); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     14,               0, 64), 0x0180); // 16
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_xmm,     15,               0, 64), 0x0190); // 16
    //                                                                                       0x01a0

#elif defined(__linux) && defined(__x86) && __WORDSIZE==32
    //------------------------------------                                                 struct  struct
    // Entries for 32-bit user_regs_struct                                                 offset  size
    //------------------------------------                                                 (byte)  (bytes)
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bx,       0, 32), 0x0000); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_cx,       0, 32), 0x0004); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_dx,       0, 32), 0x0008); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_si,       0, 32), 0x000c); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_di,       0, 32), 0x0010); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_bp,       0, 32), 0x0014); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_ax,       0, 32), 0x0018); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ds,    0, 32), 0x001c); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_es,    0, 32), 0x0020); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_fs,    0, 32), 0x0024); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_gs,    0, 32), 0x0028); // 4
    // orig_eax: unused                                                                    0x002c   // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_ip,      0,                0, 32), 0x0030); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_cs,    0, 32), 0x0034); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_flags,   x86_flags_status, 0, 32), 0x0038); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_gpr,     x86_gpr_sp,       0, 32), 0x003c); // 4
    userRegDefs_.insert(RegisterDescriptor(x86_regclass_segment, x86_segreg_ss,    0, 32), 0x0040); // 4
    
    //--------------------------------------                                               struct   struct
    // Entries for 32-bit user_fpregs_struct                                               offset   size
    //--------------------------------------                                               (byte)   (bytes)
    // cwd: unused                                                                         0x0000   // 4
    // swd: unused                                                                         0x0004   // 4
    // twd: unused                                                                         0x0008   // 4
    // fip: unused                                                                         0x000c   // 4
    // fcs: unused                                                                         0x0010   // 4
    // foo: unused                                                                         0x0014   // 4
    // fos: unused                                                                         0x0018   // 4
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_0,         0, 32),   28); // 10 
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_1,         0, 32),   38); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_2,         0, 32),   48); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_3,         0, 32),   58); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_4,         0, 32),   68); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_5,         0, 32),   78); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_6,         0, 32),   88); // 10
    userFpRegDefs_.insert(RegisterDescriptor(x86_regclass_st,      x86_st_7,         0, 32),   98); // 10
    //                                                                                          108

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
    regsPageStatus_ = REGPAGE_NONE;
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
    regsPageStatus_ = REGPAGE_NONE;
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
    return readRegister(RegisterDescriptor(x86_regclass_ip, 0, 0, kernelWordSize())).toInteger();
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
    sendCommandInt(PTRACE_SINGLESTEP, child_, 0, sendSignal_);
    waitForChild();
}

size_t
BinaryDebugger::kernelWordSize() {
    if (kernelWordSize_ == 0) {
        static const uint8_t magic = 0xb7;              // arbitrary
        uint8_t userRegs[4096];                         // arbitrary size, but plenty large for any user_regs_struct
        memset(userRegs, magic, sizeof userRegs);
        sendCommand(PTRACE_GETREGS, child_, 0, &userRegs);

        // How much was written, approximately?
        size_t highWater = sizeof userRegs;
        while (highWater>0 && userRegs[highWater-1]==magic)
            --highWater;
        if (highWater == 216) {
            kernelWordSize_ = 64;
        } else if (highWater == 68) {
            kernelWordSize_ = 32;
        } else if (highWater > 100) {
            kernelWordSize_ = 64;                       // guess
        } else {
            kernelWordSize_ = 32;                       // guess;
        }
    }
    return kernelWordSize_;
}

Sawyer::Container::BitVector
BinaryDebugger::readRegister(const RegisterDescriptor &desc) {
    using namespace Sawyer::Container;

    // Lookup register according to kernel word size rather than the actual size of the register.
    RegisterDescriptor base(desc.get_major(), desc.get_minor(), 0, kernelWordSize());
    size_t userOffset = 0;
    if (userRegDefs_.getOptional(base).assignTo(userOffset)) {
        if (regsPageStatus_ != REGPAGE_REGS) {
            sendCommand(PTRACE_GETREGS, child_, 0, regsPage_);
            regsPageStatus_ = REGPAGE_REGS;
        }
    } else if (userFpRegDefs_.getOptional(base).assignTo(userOffset)) {
        if (regsPageStatus_ != REGPAGE_FPREGS) {
            sendCommand(PTRACE_GETFPREGS, child_, 0, regsPage_);
            regsPageStatus_ = REGPAGE_FPREGS;
        }
    } else {
        throw std::runtime_error("register is not available");
    }

    // Extract the necessary data members from the struct. Assume that memory is little endian.
    size_t nUserBytes = (desc.get_offset() + desc.get_nbits() + 7) / 8;
    ASSERT_require(userOffset + nUserBytes <= sizeof regsPage_);
    BitVector bits(8 * nUserBytes);
    for (size_t i=0; i<nUserBytes; ++i)
        bits.fromInteger(BitVector::BitRange::baseSize(i*8, 8), regsPage_[userOffset+i]);

    // Adjust the data to return only the bits we want.
    bits.shiftRight(desc.get_offset());
    bits.resize(desc.get_nbits());
    return bits;
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
        sendCommandInt(PTRACE_CONT, child_, 0, sendSignal_);
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
