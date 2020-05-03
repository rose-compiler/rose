#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryDebugger.h>

#include <DisassemblerX86.h>
#include <integerOps.h>
#include <Registers.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <dirent.h>
#include <Sawyer/Message.h>
#include <cstdlib>

#ifdef ROSE_HAVE_SYS_PERSONALITY_H
#include <sys/personality.h>
#endif

#include <boost/config.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

# warning("FIXME[Robb P. Matzke  2015-02-20]: Not supported on macOS yet")
# warning("FIXME[Craig Rasmussen 2017-12-09]: Still not supported on macOS but will now compile")

# include <signal.h>
# include <sys/ptrace.h>

// from /usr/include/sys/ptrace.h (perhaps for future use)
//
# define  PTRACE_TRACEME     PT_TRACE_ME    /* child declares it's being traced */
# define  PTRACE_CONT        PT_CONTINUE    /* continue the child */
# define  PTRACE_KILL        PT_KILL        /* kill the child process */
# define  PTRACE_SINGLESTEP  PT_STEP        /* single step the child */
# define  PTRACE_DETACH      PT_DETACH      /* stop tracing a process */
# define  PTRACE_ATTACH      PT_ATTACHEXC   /* attach to running process with signal exception */

// no direct equivalent
//
#define ROSE_PT_NO_EQUIVALENT  33
#define PTRACE_GETREGS         ROSE_PT_NO_EQUIVALENT
#define PTRACE_SETREGS         ROSE_PT_NO_EQUIVALENT
#define PTRACE_GETFPREGS       ROSE_PT_NO_EQUIVALENT
#define PTRACE_SYSCALL         ROSE_PT_NO_EQUIVALENT

struct user_regs_struct {                               // macOS dud
    long int eip;
};

typedef int __ptrace_request;                           // macOS dud

static int ptrace(__ptrace_request, int, void*, void*) {// macOS dud
    errno = ENOSYS;
    return -1;
}

static char **environ = NULL;                           // macOS dud

#else

# include <fcntl.h>
# include <sys/ptrace.h>
# include <sys/user.h>
# include <sys/wait.h>
# include <unistd.h>

#endif

using namespace Sawyer::Message::Common;
namespace bfs = boost::filesystem;

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Specimen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Debugger::Specimen::eraseMatchingEnvironmentVariables(const boost::regex &re) {
    clearEnvVars_.push_back(re);
}

void
Debugger::Specimen::eraseAllEnvironmentVariables() {
    clearEnvVars_.clear();
    clearEnvVars_.push_back(boost::regex(".*"));
}

void
Debugger::Specimen::eraseEnvironmentVariable(const std::string &s) {
    std::string reStr = "^";
    BOOST_FOREACH (char ch, s) {
        if (strchr(".|*?+(){}[]^$\\", ch))
            reStr += "\\";
        reStr += ch;
    }
    reStr += "$";
    eraseMatchingEnvironmentVariables(boost::regex(reStr));
}

void
Debugger::Specimen::insertEnvironmentVariable(const std::string &name, const std::string &value) {
    setEnvVars_[name] = value;
}

char**
Debugger::Specimen::prepareEnvAdjustments() const {
    // Variables to be erased
    std::vector<std::string> erasures;
    for (char **entryPtr = environ; entryPtr && *entryPtr; ++entryPtr) {
        char *eq = std::strchr(*entryPtr, '=');
        ASSERT_not_null(eq);
        std::string name(*entryPtr, eq);
        BOOST_FOREACH (const boost::regex &re, clearEnvVars_) {
            if (boost::regex_search(name, re)) {
                erasures.push_back(name);
                break;
            }
        }
    }

    // Return value should be a list of strings that are either variable names to be moved, or variables to be added. The
    // variables to be added will have an '=' in the string.
    char **retval = new char*[erasures.size() + setEnvVars_.size() + 1]();
    char **entryPtr = retval;
    BOOST_FOREACH (const std::string &name, erasures) {
        *entryPtr = new char[name.size()+1];
        std::strcpy(*entryPtr, name.c_str());
        ++entryPtr;
    }
    for (std::map<std::string, std::string>::const_iterator iter = setEnvVars_.begin(); iter != setEnvVars_.end(); ++iter) {
        std::string var = iter->first + "=" + iter->second;
        *entryPtr = new char[var.size()+1];
        std::strcpy(*entryPtr, var.c_str());
        ++entryPtr;
    }
    ASSERT_require(entryPtr - retval == erasures.size() + setEnvVars_.size());
    ASSERT_require(NULL == *entryPtr);
    return retval;
}

// This function should be async signal safe. However, the call to putenv is AS-unsafe. I think the only time this might be an
// issue is if fork() happened to be called when some other thread was also operating on the environment and assuming that
// glibc fails to have an pthread::atfork handler that releases the lock.
static void
adjustEnvironment(char **list) {
    if (list) {
        for (char **entryPtr = list; *entryPtr; ++entryPtr)
            putenv(*entryPtr);                          // NOT ASYNC SIGNAL SAFE
    }
}

// Free an allocated list of C strings.
static char**
freeStringList(char **list) {
    for (char **entryPtr = list; entryPtr && *entryPtr; ++entryPtr)
        delete[] *entryPtr;
    delete[] list;
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debugger
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static long
sendCommand(__ptrace_request request, int child, void *addr=0, void *data=0) {
    ASSERT_require2(child, "must be attached to a subordinate process");
    errno = 0;
    long result = ptrace(request, child, addr, data);
    if (result == -1 && errno != 0)
        throw std::runtime_error("Rose::BinaryAnalysis::Debugger::sendCommand failed: " +
                                 boost::to_lower_copy(std::string(strerror(errno))));
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

#if defined(BOOST_WINDOWS) || __WORDSIZE==32 || (defined(__APPLE__) && defined(__MACH__))
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

bool
Debugger::Specimen::randomizedAddresses() const {
#ifdef ROSE_HAVE_SYS_PERSONALITY_H
    return (persona_ & ADDR_NO_RANDOMIZE) == 0;
#else
    return false;
#endif
}

void
Debugger::Specimen::randomizedAddresses(bool b) {
#ifdef ROSE_HAVE_SYS_PERSONALITY_H
    if (b) {
        persona_ &= ~ADDR_NO_RANDOMIZE;
    } else {
        persona_ |= ADDR_NO_RANDOMIZE;
    }
#else
    // void
#endif
}

void
Debugger::Specimen::print(std::ostream &out) const {
    if (!program_.empty()) {
        out <<program_;
        BOOST_FOREACH (const std::string &arg, arguments_)
            out <<" \"" <<StringUtility::cEscape(arg);
    } else if (-1 != pid_) {
        out <<"pid " <<pid_;
    } else {
        out <<"empty";
    }
}

std::ostream&
operator<<(std::ostream &out, const Debugger::Specimen &specimen) {
    specimen.print(out);
    return out;
}

Sawyer::Message::Facility Debugger::mlog;

// class method
void
Debugger::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Debugger");
        mlog.comment("debugging other processes");
    }
}

const RegisterDictionary*
Debugger::registerDictionary() const {
    ASSERT_not_null(disassembler_);
    return disassembler_->registerDictionary();
}
    
void
Debugger::init() {

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
    disassembler_ = new DisassemblerX86(8 /*bytes*/);

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
    disassembler_ = new DisassemblerX86(4 /*bytes*/);

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
Debugger::isTerminated() {
    return WIFEXITED(wstat_) || WIFSIGNALED(wstat_);
}

void
Debugger::waitForChild() {
    ASSERT_require2(child_, "must be attached to a subordinate process");
    if (-1 == waitpid(child_, &wstat_, 0))
        throw std::runtime_error("Rose::BinaryAnalysis::Debugger::waitForChild failed: "
                                 + boost::to_lower_copy(std::string(strerror(errno))));
    sendSignal_ = WIFSTOPPED(wstat_) && WSTOPSIG(wstat_)!=SIGTRAP ? WSTOPSIG(wstat_) : 0;
    regsPageStatus_ = REGPAGE_NONE;
}

std::string
Debugger::howTerminated() {
    if (WIFEXITED(wstat_)) {
        return "exited with status " + StringUtility::numberToString(WEXITSTATUS(wstat_));
    } else if (WIFSIGNALED(wstat_)) {
        return "terminated by signal (" + boost::to_lower_copy(std::string(strsignal(WTERMSIG(wstat_)))) + ")";
    } else {
        return "";                                      // not terminated yet
    }
}

void
Debugger::detach(Sawyer::Optional<DetachMode> how) {
    if (child_ && !isTerminated()) {
        switch (how.orElse(autoDetach_)) {
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
    child_ = 0;
    regsPageStatus_ = REGPAGE_NONE;
}

void
Debugger::terminate() {
    detach(KILL);
}

void
Debugger::attach(const Specimen &specimen, Sawyer::Optional<DetachMode> onDelete) {
    if (!specimen.program().empty()) {
        // Attach to an executable program by running it.
        detach(autoDetach_);
        specimen_ = specimen;
        autoDetach_ = onDelete.orElse(KILL);

        // Create the child exec arguments before the fork because heap allocation is not async-signal-safe.
        char **argv = new char*[1 /*name*/ + specimen.arguments().size() + 1 /*null*/]();
        argv[0] = new char[specimen.program().string().size()+1];
        std::strcpy(argv[0], specimen.program().string().c_str());
        for (size_t i = 0; i < specimen.arguments().size(); ++i) {
            argv[i+1] = new char[specimen.arguments()[i].size()+1];
            std::strcpy(argv[i+1], specimen.arguments()[i].c_str());
        }

#ifndef BOOST_WINDOWS
        // Prepare to close files when forking.  This is a race because some other thread might open a file without the
        // O_CLOEXEC flag after we've checked but before we reach the fork. And we can't fix that entirely within ROSE since we
        // have no control over the user program or other libraries. Furthermore, we must do it here in the parent rather than
        // after the fork because opendir, readdir, and strtol are not async-signal-safe and Linux does't have a closefrom
        // syscall.
        if (specimen.flags().isSet(CLOSE_FILES)) {
            static const int minFd = 3;
            if (DIR *dir = opendir("/proc/self/fd")) {
                while (const struct dirent *entry = readdir(dir)) {
                    char *rest = NULL;
                    errno = 0;
                    int fd = strtol(entry->d_name, &rest, 10);
                    if (0 == errno && '\0' == *rest && rest != entry->d_name && fd >= minFd)
                        fcntl(fd, F_SETFD, FD_CLOEXEC);
                }
                closedir(dir);
            }
        }
#endif

        char **envAdjustments = specimen.prepareEnvAdjustments();
        child_ = fork();
        if (0==child_) {
            // Since the parent process may have been multi-threaded, we are now in an async-signal-safe context.
            adjustEnvironment(envAdjustments);
            if (specimen.flags().isSet(REDIRECT_INPUT))
                devNullTo(0, O_RDONLY);                 // async-signal-safe

            if (specimen.flags().isSet(REDIRECT_OUTPUT))
                devNullTo(1, O_WRONLY);                 // async-signal-safe

            if (specimen.flags().isSet(REDIRECT_ERROR))
                devNullTo(2, O_WRONLY);                 // async-signal-safe

            // FIXME[Robb Matzke 2017-08-04]: We should be using a direct system call here instead of the C library wrapper because
            // the C library is adjusting errno, which is not async-signal-safe.
            if (-1 == ptrace(PTRACE_TRACEME, 0, 0, 0)) {
                // errno is set, but no way to access it in an async-signal-safe way
                const char *mesg= "Rose::BinaryAnalysis::Debugger::attach: ptrace_traceme failed\n";
                if (write(2, mesg, strlen(mesg)) == -1)
                    abort();
                _Exit(1);                                   // avoid calling C++ destructors from child
            }

            setPersonality(specimen.persona());
            execv(argv[0], argv);

            // If failure, we must still call only async signal-safe functions.
            const char *mesg = "Rose::BinaryAnalysis::Debugger::attach: exec failed: ";
            if (write(2, mesg, strlen(mesg)) == -1)
                abort();
            mesg = strerror(errno);
            if (write(2, mesg, strlen(mesg)) == -1)
                abort();
            if (write(2, "\n", 1) == -1)
                abort();
            _Exit(1);
        }

        argv = freeStringList(argv);
        envAdjustments = freeStringList(envAdjustments);

        waitForChild();
        if (isTerminated())
            throw std::runtime_error("Rose::BinaryAnalysis::Debugger::attach: subordinate " +
                                     howTerminated() + " before we gained control");
    } else {
        // Attach to an existing process.
        if (-1 == specimen.process()) {
            detach(autoDetach_);
        } else if (specimen.process() == child_) {
            // do nothing
        } else if (specimen.flags().isSet(ATTACH)) {
            child_ = specimen.process();
            sendCommand(PTRACE_ATTACH, child_);
            autoDetach_ = onDelete.orElse(DETACH);
            waitForChild();
            if (SIGSTOP==sendSignal_)
                sendSignal_ = 0;
        } else {
            child_ = specimen.process();
            autoDetach_ = onDelete.orElse(NOTHING);
        }
        specimen_ = specimen;
    }
}

// Must be async signal safe!
void
Debugger::devNullTo(int targetFd, int openFlags) {
    int fd = open("/dev/null", openFlags, 0666);
    if (-1 == fd) {
        close(targetFd);
    } else {
        dup2(fd, targetFd);
        close(fd);
    }
}

void
Debugger::executionAddress(rose_addr_t va) {
    user_regs_struct regs;
    sendCommand(PTRACE_GETREGS, child_, 0, &regs);
    setInstructionPointer(regs, va);
    sendCommand(PTRACE_SETREGS, child_, 0, &regs);
}

rose_addr_t
Debugger::executionAddress() {
    return readRegister(RegisterDescriptor(x86_regclass_ip, 0, 0, kernelWordSize())).toInteger();
}

void
Debugger::setBreakpoint(const AddressInterval &va) {
    breakpoints_.insert(va);
}

void
Debugger::clearBreakpoint(const AddressInterval &va) {
    breakpoints_.erase(va);
}

void
Debugger::singleStep() {
    sendCommandInt(PTRACE_SINGLESTEP, child_, 0, sendSignal_);
    waitForChild();
}

size_t
Debugger::kernelWordSize() {
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
Debugger::readRegister(RegisterDescriptor desc) {
    using namespace Sawyer::Container;

    // Lookup register according to kernel word size rather than the actual size of the register.
    RegisterDescriptor base(desc.majorNumber(), desc.minorNumber(), 0, kernelWordSize());
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
    size_t nUserBytes = (desc.offset() + desc.nBits() + 7) / 8;
    ASSERT_require(userOffset + nUserBytes <= sizeof regsPage_);
    BitVector bits(8 * nUserBytes);
    for (size_t i=0; i<nUserBytes; ++i)
        bits.fromInteger(BitVector::BitRange::baseSize(i*8, 8), regsPage_[userOffset+i]);

    // Adjust the data to return only the bits we want.
    bits.shiftRight(desc.offset());
    bits.resize(desc.nBits());
    return bits;
}

Sawyer::Container::BitVector
Debugger::readMemory(rose_addr_t va, size_t nBytes, ByteOrder::Endianness sex) {
    using namespace Sawyer::Container;

    struct Resources {
        uint8_t *buffer;
        Resources(): buffer(NULL) {}
        ~Resources() {
            delete[] buffer;
        }
    } r;

    r.buffer = new uint8_t[nBytes];
    size_t nRead = readMemory(va, nBytes, r.buffer);
    if (nRead != nBytes)
        throw std::runtime_error("short read at " + StringUtility::addrToString(va));

    BitVector retval(8*nBytes);
    for (size_t i=0; i<nBytes; ++i) {
        BitVector::BitRange where;
        switch (sex) {
            case ByteOrder::ORDER_LSB:
                where = BitVector::BitRange::baseSize(8*i, 8);
                break;
            case ByteOrder::ORDER_MSB:
                where = BitVector::BitRange::baseSize(8*nBytes-(i+1), 8);
                break;
            default:
                ASSERT_not_reachable("invalid byte order");
        }
        retval.fromInteger(where, r.buffer[i]);
    }
    return retval;
}

size_t
Debugger::readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer) {
#ifdef __linux__
    if (0 == nBytes)
        return 0;

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
        return 0;                                       // bad address
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

std::string
Debugger::readCString(rose_addr_t va, size_t maxBytes) {
    std::string retval;
    while (maxBytes > 0) {
        uint8_t buf[32];
        size_t nRead = readMemory(va, std::min(maxBytes, sizeof buf), buf);
        if (0 == nRead)
            break;
        for (size_t i = 0; i < nRead; ++i) {
            if (0 == buf[i]) {
                return retval;                          // NUL terminated
            } else {
                retval += (char)buf[i];
            }
        }
        maxBytes -= nRead;
        va += nRead;
    }
    return retval;                                      // buffer overflow
}

void
Debugger::runToBreakpoint() {
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

void
Debugger::runToSyscall() {
    sendCommandInt(PTRACE_SYSCALL, child_, 0, sendSignal_);
    waitForChild();
}

struct DefaultTraceFilter {
    Debugger::FilterAction operator()(rose_addr_t) {
        return Debugger::FilterAction();
    }
};

Sawyer::Container::Trace<rose_addr_t>
Debugger::trace() {
    DefaultTraceFilter filter;
    return trace(filter);
}

// class method
unsigned long
Debugger::getPersonality() {
#ifdef ROSE_HAVE_SYS_PERSONALITY_H
    return ::personality(0xffffffff);
#else
    return 0;
#endif
}

// class method
void
Debugger::setPersonality(unsigned long bits) {
#ifdef ROSE_HAVE_SYS_PERSONALITY_H
    ::personality(bits);
#else
    if (bits != 0)
        mlog[WARN] <<"unable to set process execution domain for this architecture\n";
#endif
}

} // namespace
} // namespace

#endif
