#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_LINUX
#include <Rose/BinaryAnalysis/Debugger/Linux.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>
#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/NumberToString.h>
#include <ROSE_UNUSED.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <dirent.h>
#include <fcntl.h>
#include <sys/personality.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace Sawyer::Message::Common;
namespace bfs = boost::filesystem;

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux::Specimen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Linux::Specimen::Specimen()
    : persona_(getPersonality()) {}

Linux::Specimen::Specimen(int pid)
    : flags_(Flag::DEFAULT_FLAGS), persona_(getPersonality()), pid_(pid) {}

Linux::Specimen::Specimen(const boost::filesystem::path &name)
    : flags_(Flag::DEFAULT_FLAGS), persona_(getPersonality()), program_(name) {}

Linux::Specimen::Specimen(const boost::filesystem::path &name, const std::vector<std::string> &args)
    : flags_(Flag::DEFAULT_FLAGS), persona_(getPersonality()), program_(name), arguments_(args) {}

Linux::Specimen::Specimen(const std::vector<std::string> &nameAndArgs)
    : flags_(Flag::DEFAULT_FLAGS), persona_(getPersonality()), program_(nameAndArgs.front()),
      arguments_(nameAndArgs.begin()+1, nameAndArgs.end()) {}

const boost::filesystem::path&
Linux::Specimen::program() const {
    return program_;
}

void
Linux::Specimen::program(const boost::filesystem::path &name) {
    program_ = name;
    pid_ = -1;
}

const std::vector<std::string>&
Linux::Specimen::arguments() const {
    return arguments_;
}

void
Linux::Specimen::arguments(const std::vector<std::string> &args) {
    arguments_ = args;
}

void
Linux::Specimen::eraseMatchingEnvironmentVariables(const boost::regex &re) {
    clearEnvVars_.push_back(re);
}

void
Linux::Specimen::eraseAllEnvironmentVariables() {
    clearEnvVars_.clear();
    clearEnvVars_.push_back(boost::regex(".*"));
}

void
Linux::Specimen::eraseEnvironmentVariable(const std::string &s) {
    std::string reStr = "^";
    for (char ch: s) {
        if (strchr(".|*?+(){}[]^$\\", ch))
            reStr += "\\";
        reStr += ch;
    }
    reStr += "$";
    eraseMatchingEnvironmentVariables(boost::regex(reStr));
}

void
Linux::Specimen::insertEnvironmentVariable(const std::string &name, const std::string &value) {
    setEnvVars_[name] = value;
}

boost::filesystem::path
Linux::Specimen::workingDirectory() const {
    return workingDirectory_;
}

void
Linux::Specimen::workingDirectory(const boost::filesystem::path &name) {
    workingDirectory_ = name;
}

const BitFlags<Linux::Flag>&
Linux::Specimen::flags() const {
    return flags_;
}

BitFlags<Linux::Flag>&
Linux::Specimen::flags() {
    return flags_;
}

unsigned long
Linux::Specimen::persona() const {
    return persona_;
}

void
Linux::Specimen::persona(unsigned long bits) {
    persona_ = bits;
}

Architecture::Base::ConstPtr
Linux::Specimen::architecture() const {
    return arch_;                                       // may be null
}

void
Linux::Specimen::architecture(const Architecture::Base::ConstPtr &arch) {
    arch_ = arch;                                       // may be null
}

int
Linux::Specimen::process() const {
    return pid_;
}

void
Linux::Specimen::process(int pid) {
    pid_ = pid;
    program_.clear();
}

char**
Linux::Specimen::prepareEnvAdjustments() const {
    // Variables to be erased
    std::vector<std::string> erasures;
    for (char **entryPtr = environ; entryPtr && *entryPtr; ++entryPtr) {
        char *eq = std::strchr(*entryPtr, '=');
        ASSERT_not_null(eq);
        std::string name(*entryPtr, eq);
        for (const boost::regex &re: clearEnvVars_) {
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
    for (const std::string &name: erasures) {
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
    ASSERT_require((size_t)(entryPtr - retval) == erasures.size() + setEnvVars_.size());
    ASSERT_require(NULL == *entryPtr);
    return retval;
}


bool
Linux::Specimen::randomizedAddresses() const {
    return (persona_ & ADDR_NO_RANDOMIZE) == 0;
}

void
Linux::Specimen::randomizedAddresses(bool b) {
    if (b) {
        persona_ &= ~ADDR_NO_RANDOMIZE;
    } else {
        persona_ |= ADDR_NO_RANDOMIZE;
    }
}

void
Linux::Specimen::print(std::ostream &out) const {
    if (!program_.empty()) {
        out <<program_;
        for (const std::string &arg: arguments_)
            out <<" \"" <<StringUtility::cEscape(arg);
    } else if (-1 != pid_) {
        out <<"pid " <<pid_;
    } else {
        out <<"empty";
    }
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
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Linux
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long
Linux::sendCommand(__ptrace_request request, void *addr, void *data) {
    ASSERT_require2(child_, "must be attached to a subordinate process");
    errno = 0;
    long result = ptrace(request, child_, addr, data);
    if (result == -1 && errno != 0) {
        const std::string ts = howTerminated();
        throw Exception("Rose::BinaryAnalysis::Debugger::Linux::sendCommand failed for PID " +
                        boost::lexical_cast<std::string>(child_) + ": " +
                        boost::to_lower_copy(std::string(strerror(errno))) +
                        (ts.empty() ? "" : "; " + ts));
    }
    return result;
}

long
Linux::sendCommandInt(__ptrace_request request, void *addr, int i) {
    // Avoid doing a cast because that will cause a G++ warning.
    void *ptr = 0;
    ASSERT_require(sizeof i <= sizeof ptr);
    memcpy(&ptr, &i, sizeof i);
    return sendCommand(request, addr, ptr);
}

#if __WORDSIZE==32
static Address
getInstructionPointer(const user_regs_struct &regs) {
    return regs.eip;
}
static void
setInstructionPointer(user_regs_struct &regs, Address va) {
    regs.eip = va;
}
#else
static Address
getInstructionPointer(const user_regs_struct &regs) {
    return regs.rip;
}
static void
setInstructionPointer(user_regs_struct &regs, Address va) {
    regs.rip = va;
}
#endif

std::ostream&
operator<<(std::ostream &out, const Debugger::Linux::Specimen &specimen) {
    specimen.print(out);
    return out;
}

void
Linux::declareSystemCalls() {
    const auto h = [this]() -> boost::filesystem::path {
        if (const auto arch = guessSpecimenArchitecture()) {
            if (as<const Architecture::X86>(arch)) {
                return "/usr/include/x86_64-linux-gnu/asm/unistd_" + boost::lexical_cast<std::string>(arch->bitsPerWord()) + ".h";
            } else {
                return {};
            }
        } else {
            return {};
        }
    }();

    if (!h.empty() && boost::filesystem::exists(h)) {
        SAWYER_MESG(mlog[DEBUG]) <<"parsing system call information from " <<h <<"\n";
        syscallDecls_.declare(SystemCall::parseHeaderFile(h));
    } else {
        SAWYER_MESG(mlog[DEBUG]) <<"cannot find system call declarations\n";
    }
}

Linux::Linux() {}

void
Linux::init() {
    syscallVa_.reset();
    memset(regCache_.data(), 0, regCache_.size() * sizeof(RegPage::value_type));

    // Initialize register information.  This is very architecture and OS-dependent. See <sys/user.h> for details, but be
    // warned that even <sys/user.h> is only a guideline!  The header defines two versions of user_regs_struct, one for 32-bit
    // and the other for 64-bit based on whether ROSE is compiled as 32- or 64-bit.  I'm not sure what happens if a 32-bit
    // version of ROSE tries to analyze a 64-bit specimen; does the OS return only the 32-bit registers or does it return the
    // 64-bit versions regardless of how user_regs_struct is defined in ROSE? [Robb P. Matzke 2015-03-24]
    //
    // The userRegDefs map assigns an offset in the returned (not necessarily the defined) user_regs_struct. The register
    // descriptors in this table have sizes that correspond to the data member in the user_regs_struct, not necessarily the
    // natural size of the register (e.g., The 16-bit segment registers are listed as 32 or 64 bits).
#if defined(__x86_64) && __WORDSIZE==64
    declareSystemCalls();
    disassembler_ = Architecture::findByName("amd64").orThrow()->newInstructionDecoder();

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

#elif defined(__x86) && __WORDSIZE==32
    declareSystemCalls();
    disassembler_ = new Disassembler::X86(4 /*bytes*/);

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

#else
    ROSE_PRAGMA_MESSAGE("subordinate process registers not supported on this platform");
    throw Exception("subordinate process registers not supported on this platform");
#endif
}

Linux::~Linux() {
    detach();
}

Linux::Ptr
Linux::instance() {
    return Ptr(new Linux);
}

Linux::Ptr
Linux::instance(const Specimen &s, Sawyer::Optional<DetachMode> onDelete) {
    auto debugger = instance();
    debugger->attach(s, onDelete);
    return debugger;
}

bool
Linux::isAttached() {
    return -1 != child_;
}

Sawyer::Optional<int>
Linux::processId() const {
    if (-1 == child_) {
        return Sawyer::Nothing();
    } else {
        return child_;
    }
}

bool
Linux::isTerminated() {
    return WIFEXITED(wstat_) || WIFSIGNALED(wstat_);
}

int
Linux::waitpidStatus() const {
    return wstat_;
}

void
Linux::waitForChild() {
    ASSERT_require2(child_, "must be attached to a subordinate process");
    if (-1 == waitpid(child_, &wstat_, __WALL))
        throw Exception("Rose::BinaryAnalysis::Debugger::Linux::waitForChild failed: "
                        + boost::to_lower_copy(std::string(strerror(errno))));
    sendSignal_ = WIFSTOPPED(wstat_) && WSTOPSIG(wstat_)!=SIGTRAP ? WSTOPSIG(wstat_) : 0;
    regCacheType_ = RegCacheType::NONE;
}

std::string
Linux::howTerminated() {
    if (WIFEXITED(wstat_)) {
        return "exited with status " + StringUtility::numberToString(WEXITSTATUS(wstat_));
    } else if (WIFSIGNALED(wstat_)) {
        return "terminated by signal (" + boost::to_lower_copy(std::string(strsignal(WTERMSIG(wstat_)))) + ")";
    } else {
        return "";                                      // not terminated yet
    }
}

Linux::DetachMode
Linux::detachMode() const {
    return autoDetach_;
}

void
Linux::detachMode(DetachMode m) {
    autoDetach_ = m;
}

void
Linux::detach() {
    if (child_ && !isTerminated()) {
        mlog[DEBUG] <<"PID " <<child_ <<": detaching\n";
        switch (autoDetach_) {
            case DetachMode::NOTHING:
                break;
            case DetachMode::CONTINUE:
                kill(child_, SIGCONT);
                break;
            case DetachMode::DETACH:
                sendCommand(PTRACE_DETACH);
                break;
            case DetachMode::KILL:
                sendCommand(PTRACE_KILL);
                waitForChild();
        }
    }
    child_ = 0;
    regCacheType_ = RegCacheType::NONE;
    syscallVa_.reset();
}

void
Linux::terminate() {
    detachMode(DetachMode::KILL);
    detach();
}

std::vector<ThreadId>
Linux::threadIds() {
    return std::vector<ThreadId>();
}

void
Linux::attach(const Specimen &specimen, Sawyer::Optional<DetachMode> onDelete) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    if (!specimen.program().empty()) {
        // Attach to an executable program by running it.
        detach();
        specimen_ = specimen;
        autoDetach_ = onDelete.orElse(DetachMode::KILL);
        init();

        // Create the child exec arguments before the fork because heap allocation is not async-signal-safe.
        char **argv = new char*[1 /*name*/ + specimen.arguments().size() + 1 /*null*/]();
        argv[0] = new char[specimen.program().string().size()+1];
        std::strcpy(argv[0], specimen.program().string().c_str());
        for (size_t i = 0; i < specimen.arguments().size(); ++i) {
            argv[i+1] = new char[specimen.arguments()[i].size()+1];
            std::strcpy(argv[i+1], specimen.arguments()[i].c_str());
        }

        // Prepare to close files when forking.  This is a race because some other thread might open a file without the
        // O_CLOEXEC flag after we've checked but before we reach the fork. And we can't fix that entirely within ROSE since we
        // have no control over the user program or other libraries. Furthermore, we must do it here in the parent rather than
        // after the fork because opendir, readdir, and strtol are not async-signal-safe and Linux does't have a closefrom
        // syscall.
        if (specimen.flags().isSet(Flag::CLOSE_FILES)) {
            static const int minFd = 3;
            if (DIR *dir = opendir("/proc/self/fd")) {
                while (const struct dirent *entry = readdir(dir)) {
                    char *rest = nullptr;
                    errno = 0;
                    int fd = strtol(entry->d_name, &rest, 10);
                    if (0 == errno && '\0' == *rest && rest != entry->d_name && fd >= minFd)
                        fcntl(fd, F_SETFD, FD_CLOEXEC);
                }
                closedir(dir);
            }
        }

        if (debug) {
            debug <<"attach: forking child process:\n";
            for (int i = 0; argv[i]; ++i)
                debug <<"  argv[" <<i <<"]: \"" <<StringUtility::cEscape(argv[i]) <<"\n";
        }

        char **envAdjustments = specimen.prepareEnvAdjustments();
        child_ = fork();
        if (0==child_) {
            // Since the parent process may have been multi-threaded, we are now in an async-signal-safe context.
            adjustEnvironment(envAdjustments);
            if (specimen.flags().isSet(Flag::REDIRECT_INPUT))
                devNullTo(0, O_RDONLY);                 // async-signal-safe

            if (specimen.flags().isSet(Flag::REDIRECT_OUTPUT))
                devNullTo(1, O_WRONLY);                 // async-signal-safe

            if (specimen.flags().isSet(Flag::REDIRECT_ERROR))
                devNullTo(2, O_WRONLY);                 // async-signal-safe

            // FIXME[Robb Matzke 2017-08-04]: We should be using a direct system call here instead of the C library wrapper because
            // the C library is adjusting errno, which is not async-signal-safe.
            if (-1 == ptrace(PTRACE_TRACEME, 0, 0, 0)) {
                // errno is set, but no way to access it in an async-signal-safe way
                const char *mesg= "Rose::BinaryAnalysis::Debugger::Linux::attach: ptrace_traceme failed\n";
                if (write(2, mesg, strlen(mesg)) == -1)
                    abort();
                _Exit(1);                                   // avoid calling C++ destructors from child
            }

            setPersonality(specimen.persona());
            execv(argv[0], argv);

            // If failure, we must still call only async signal-safe functions.
            const char *mesg = "Rose::BinaryAnalysis::Debugger::Linux::attach: exec failed: ";
            if (write(2, mesg, strlen(mesg)) == -1)
                abort();
            mesg = strerror(errno);
            if (write(2, mesg, strlen(mesg)) == -1)
                abort();
            if (write(2, "\n", 1) == -1)
                abort();
            _Exit(1);
        }

        SAWYER_MESG(debug) <<"  child PID is " <<child_ <<"\n";
        argv = freeStringList(argv);
        envAdjustments = freeStringList(envAdjustments);

        waitForChild();
        if (isTerminated())
            throw Exception("Rose::BinaryAnalysis::Debugger::Linux::attach: subordinate " +
                            howTerminated() + " before we gained control");
    } else {
        // Attach to an existing process.
        if (-1 == specimen.process()) {
            detach();
        } else if (specimen.process() == child_) {
            // do nothing
        } else if (specimen.flags().isSet(Flag::ATTACH)) {
            specimen_ = specimen;
            init();
            child_ = specimen.process();
            SAWYER_MESG(debug) <<"attach: attaching to PID " <<child_ <<"\n";
            sendCommand(PTRACE_ATTACH);
            autoDetach_ = onDelete.orElse(DetachMode::DETACH);
            waitForChild();
            if (SIGSTOP==sendSignal_)
                sendSignal_ = 0;
        } else {
            specimen_ = specimen;
            init();
            child_ = specimen.process();
            SAWYER_MESG(debug) <<"attach: PID set to " <<child_ <<"\n";
            autoDetach_ = onDelete.orElse(DetachMode::NOTHING);
        }
    }
}

Architecture::Base::ConstPtr
Linux::guessSpecimenArchitecture() const {
    if (const auto arch = specimen_.architecture()) {
        return arch;
    } else if (const auto x86 = Architecture::findByName("intel-pentium4")) {
        mlog[WARN] <<"assuming debug subordinate is " <<(*x86)->name() <<"\n";
        return *x86;
    } else {
        mlog[ERROR] <<x86.unwrapError().what() <<"\n";
        return {};
    }
}

// Must be async signal safe!
void
Linux::devNullTo(int targetFd, int openFlags) {
    int fd = open("/dev/null", openFlags, 0666);
    if (-1 == fd) {
        close(targetFd);
    } else {
        dup2(fd, targetFd);
        close(fd);
    }
}

void
Linux::executionAddress(ThreadId, Address va) {
    user_regs_struct regs;
    sendCommand(PTRACE_GETREGS, 0, &regs);
    setInstructionPointer(regs, va);
    sendCommand(PTRACE_SETREGS, 0, &regs);
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": set execution address to " <<StringUtility::addrToString(va) <<"\n";
}

Address
Linux::executionAddress(ThreadId tid) {
    return readRegister(tid, RegisterDescriptor(x86_regclass_ip, 0, 0, kernelWordSize())).toInteger();
}

void
Linux::setBreakPoint(const AddressInterval &va) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": set breakpoint " <<StringUtility::addrToString(va) <<"\n";
    breakPoints_.insert(va);
}

void
Linux::setBreakPoints(const AddressIntervalSet &where) {
    for (const AddressInterval &interval: where.intervals())
        setBreakPoint(interval);
}

AddressIntervalSet
Linux::breakPoints() {
    return breakPoints_;
}

void
Linux::clearBreakPoint(const AddressInterval &va) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": clear breakpoint " <<StringUtility::addrToString(va) <<"\n";
    breakPoints_.erase(va);
}

void
Linux::clearBreakPoints() {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": clear all breakpoints\n";
    breakPoints_.clear();
}

void
Linux::singleStep(ThreadId) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": single step\n";
    sendCommandInt(PTRACE_SINGLESTEP, 0, sendSignal_);
    waitForChild();
}

void
Linux::stepIntoSystemCall(ThreadId) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": step into syscall\n";
    sendCommandInt(PTRACE_SYSCALL, 0, sendSignal_);
    waitForChild();
}

#if 0 // [Robb Matzke 2021-05-26]: doesn't seem to work on Linux 5.4: always says PTRACE_SYSCALL_INFO_NONE
Sawyer::Optional<Linux::SyscallEntry>
Linux::syscallEntryInfo() {
    __ptrace_syscall_info info;
    sendCommand(PTRACE_GET_SYSCALL_INFO, reinterpret_cast<void*>(sizeof info), &info);
    if (PTRACE_SYSCALL_INFO_ENTRY == info.op) {
        return SyscallEntry(info.entry.nr, info.entry.args);
    } else {
        return Sawyer::Nothing();
    }
}

Sawyer::Optional<Linux::SyscallExit>
Linux::syscallExitInfo() {
    __ptrace_syscall_info info;
    sendCommand(PTRACE_GET_SYSCALL_INFO, reinterpret_cast<void*>(sizeof info), &info);
    if (PTRACE_SYSCALL_INFO_EXIT == info.op) {
        return SyscallExit(info.exit.rval, info.exit.is_error);
    } else {
        return Sawyer::Nothing();
    }
}
#endif

size_t
Linux::kernelWordSize() {
    if (kernelWordSize_ == 0) {
        static const uint8_t magic = 0xb7;              // arbitrary
        uint8_t userRegs[4096];                         // arbitrary size, but plenty large for any user_regs_struct
        memset(userRegs, magic, sizeof userRegs);
        sendCommand(PTRACE_GETREGS, 0, &userRegs);

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

std::vector<RegisterDescriptor>
Linux::availableRegisters() {
    using RegOff = std::pair<RegisterDescriptor, size_t /*offset*/>;
    std::vector<RegOff> all;
    all.reserve(userRegDefs_.size() + userFpRegDefs_.size());
    for (const UserRegDefs::Node &node: userRegDefs_.nodes())
        all.push_back(RegOff(node.key(), node.value()));
    for (const UserRegDefs::Node &node: userFpRegDefs_.nodes())
        all.push_back(RegOff(node.key(), node.value()));
    std::sort(all.begin(), all.end(), [](const RegOff &a, const RegOff &b) {
        return a.second < b.second;
    });
    std::vector<RegisterDescriptor> retval;
    retval.reserve(all.size());
    for (const RegOff &p: all)
        retval.push_back(p.first);
    return retval;
}

Sawyer::Container::BitVector
Linux::readAllRegisters(ThreadId tid) {
    // Load all registers from the low level API, then copy those bytes into the returned bit vector.
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": read all registers\n";
    using BvRange = Sawyer::Container::BitVector::BitRange;
    AllRegValues allBytes = loadAllRegisters(tid);
    size_t nBits = 8 * (allBytes.regs.size() + allBytes.fpregs.size());
    Sawyer::Container::BitVector retval(nBits);
    for (size_t i = 0; i < allBytes.regs.size(); ++i)
        retval.fromInteger(BvRange::baseSize(i*8, 8), allBytes.regs[i]);
    for (size_t i = 0; i < allBytes.regs.size(); ++i)
        retval.fromInteger(BvRange::baseSize(8*(allBytes.regs.size() + i), 8), allBytes.fpregs[i]);
    return retval;
}

void
Linux::writeAllRegisters(ThreadId tid, const Sawyer::Container::BitVector &values) {
    // Copy the bit vector into the parts of the low-level API's data structure, then save those bytes using the low-level API.
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": write all registers\n";
    using BvRange = Sawyer::Container::BitVector::BitRange;
    AllRegValues allBytes;
    ASSERT_require(values.size() == 8 * (allBytes.regs.size() + allBytes.fpregs.size()));
    for (size_t i = 0; i < allBytes.regs.size(); ++i)
        allBytes.regs[i] = values.toInteger(BvRange::baseSize(i*8, 8));
    for (size_t i = 0; i < allBytes.fpregs.size(); ++i)
        allBytes.fpregs[i] = values.toInteger(BvRange::baseSize(8 * (allBytes.regs.size() + i), 8));
    saveAllRegisters(tid, allBytes);

#if 1 // DEBUGGING [Robb Matzke 2022-11-18]
    const Sawyer::Container::BitVector v2 = readAllRegisters(tid);
    ASSERT_require(v2.equalTo(values));
#endif
}

Linux::AllRegValues
Linux::loadAllRegisters(ThreadId) {
    AllRegValues retval;
    if (RegCacheType::REGS == regCacheType_) {
        retval.regs = regCache_;
        sendCommand(PTRACE_GETFPREGS, 0, regCache_.data());
        retval.fpregs = regCache_;
        regCacheType_ = RegCacheType::FPREGS;
    } else if (RegCacheType::FPREGS == regCacheType_) {
        retval.fpregs = regCache_;
        sendCommand(PTRACE_GETREGS, 0, regCache_.data());
        retval.regs = regCache_;
        regCacheType_ = RegCacheType::REGS;
    } else {
        sendCommand(PTRACE_GETFPREGS, 0, regCache_.data());
        retval.fpregs = regCache_;
        sendCommand(PTRACE_GETREGS, 0, regCache_.data());
        retval.regs = regCache_;
        regCacheType_ = RegCacheType::REGS;
    }
    return retval;
}

void
Linux::saveAllRegisters(ThreadId, const AllRegValues &all) {
    sendCommand(PTRACE_SETFPREGS, 0, (void*)all.fpregs.data());
    sendCommand(PTRACE_SETREGS, 0, (void*)all.regs.data());
    regCache_ = all.regs;
    regCacheType_ = RegCacheType::REGS;
}

size_t
Linux::updateRegCache(RegisterDescriptor desc) {
    const RegisterDescriptor base(desc.majorNumber(), desc.minorNumber(), 0, kernelWordSize());
    size_t userOffset = 0;
    if (userRegDefs_.getOptional(base).assignTo(userOffset)) {
        if (regCacheType_ != RegCacheType::REGS) {
            sendCommand(PTRACE_GETREGS, 0, regCache_.data());
            regCacheType_ = RegCacheType::REGS;
        }
    } else if (userFpRegDefs_.getOptional(base).assignTo(userOffset)) {
        if (regCacheType_ != RegCacheType::FPREGS) {
            sendCommand(PTRACE_GETFPREGS, 0, regCache_.data());
            regCacheType_ = RegCacheType::FPREGS;
        }
    } else {
        throw Exception("register is not available");
    }
    return userOffset;
}

Sawyer::Container::BitVector
Linux::readRegister(ThreadId, RegisterDescriptor desc) {
    using namespace Sawyer::Container;
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"PID " <<child_ <<": read register " <<registerName(desc) <<" = ";

    // Lookup register according to kernel word size rather than the actual size of the register.
    const size_t userOffset = updateRegCache(desc);

    // Extract the necessary data members from the struct. Assume that memory is little endian.
    const size_t nUserBytes = (desc.offset() + desc.nBits() + 7) / 8;
    ASSERT_require(userOffset + nUserBytes <= regCache_.size());
    BitVector bits(8 * nUserBytes);
    for (size_t i=0; i<nUserBytes; ++i)
        bits.fromInteger(BitVector::BitRange::baseSize(i*8, 8), regCache_[userOffset+i]);

    // Adjust the data to return only the bits we want.
    bits.shiftRight(desc.offset());
    bits.resize(desc.nBits());
    SAWYER_MESG(debug) <<"0x" <<bits.toHex() <<"\n";
    return bits;
}

void
Linux::writeRegister(ThreadId tid, RegisterDescriptor desc, const Sawyer::Container::BitVector &bits) {
    using namespace Sawyer::Container;
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": write register " <<registerName(desc) <<" = 0x" <<bits.toHex() <<"\n";

    updateRegCache(desc);

    // Look up register according to kernel word size rather than the actual size of the register.
    const RegisterDescriptor base(desc.majorNumber(), desc.minorNumber(), 0, kernelWordSize());

    // Update the register page with the new data and write it to the process. Assume that memory is little endian.
    const size_t nUserBytes = (desc.offset() + desc.nBits() + 7) / 8;
    size_t userOffset = 0;
    if (userRegDefs_.getOptional(base).assignTo(userOffset)) {
        ASSERT_require(userOffset + nUserBytes <= regCache_.size());
        for (size_t i = 0; i < nUserBytes; ++i)
            regCache_[userOffset + i] = bits.toInteger(BitVector::BitRange::baseSize(i*8, 8));
        sendCommand(PTRACE_SETREGS, 0, regCache_.data());
    } else if (userFpRegDefs_.getOptional(base).assignTo(userOffset)) {
        ASSERT_require(userOffset + nUserBytes <= regCache_.size());
        for (size_t i = 0; i < nUserBytes; ++i)
            regCache_[userOffset + i] = bits.toInteger(BitVector::BitRange::baseSize(i*8, 8));
        sendCommand(PTRACE_SETFPREGS, 0, regCache_.data());
    } else {
        throw Exception("register is not available");
    }

#if 1 // DEBUGGING [Robb Matzke 2022-11-18]
    const Sawyer::Container::BitVector bits2 = readRegister(tid, desc);
    regCacheType_ = RegCacheType::NONE;
    ASSERT_require(bits.equalTo(bits2));
#endif
}

void
Linux::writeRegister(ThreadId tid, RegisterDescriptor desc, uint64_t value) {
    using namespace Sawyer::Container;
    BitVector bits(desc.nBits());
    bits.fromInteger(value);
    writeRegister(tid, desc, bits);
}

Sawyer::Container::BitVector
Linux::readMemory(Address va, size_t nBytes, ByteOrder::Endianness sex) {
    using namespace Sawyer::Container;

    struct Resources {
        uint8_t *buffer;
        Resources(): buffer(nullptr) {}
        ~Resources() {
            delete[] buffer;
        }
    } r;

    r.buffer = new uint8_t[nBytes];
    size_t nRead = readMemory(va, nBytes, r.buffer);
    if (nRead != nBytes)
        throw Exception("short read at " + StringUtility::addrToString(va));

    BitVector retval(8*nBytes);
    for (size_t i=0; i<nBytes; ++i) {
        BitVector::BitRange where;
        switch (sex) {
            case ByteOrder::ORDER_LSB:
                where = BitVector::BitRange::baseSize(8*i, 8);
                break;
            case ByteOrder::ORDER_MSB:
                where = BitVector::BitRange::baseSize(8*(nBytes-(i+1)), 8);
                break;
            default:
                ASSERT_not_reachable("invalid byte order");
        }
        retval.fromInteger(where, r.buffer[i]);
    }
    return retval;
}

std::vector<uint8_t>
Linux::readMemory(Address va, size_t nBytes) {
    std::vector<uint8_t> buf(nBytes);
    size_t nRead = readMemory(va, nBytes, buf.data());
    buf.resize(nRead);
    return buf;
}

size_t
Linux::readMemory(Address va, size_t nBytes, uint8_t *buffer) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"PID " <<child_ <<": read " <<StringUtility::plural(nBytes, "bytes")
                       <<" at va " <<StringUtility::addrToString(va) <<"\n";
    if (0 == nBytes)
        return 0;
    const size_t nBytesDesired = nBytes;

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
        throw Exception("cannot open \"" + memName + "\": " + strerror(errno));
    if (-1 == lseek(mem.fd, va, SEEK_SET))
        return 0;                                       // bad address
    size_t totalRead = 0;
    while (nBytes > 0) {
        ssize_t nread = read(mem.fd, buffer, nBytes);
        if (-1 == nread) {
            if (EINTR == errno)
                continue;
            break;                                      // error
        } else if (0 == nread) {
            break;                                      // short read
        } else {
            ASSERT_require(nread > 0);
            ASSERT_require((size_t)nread <= nBytes);
            nBytes -= nread;
            buffer += nread;
            totalRead += nread;
        }
    }

    if (debug) {
        if (totalRead < nBytesDesired)
            debug <<"  short read: only " <<StringUtility::plural(totalRead, "bytes") <<" read from memory\n";
        if (totalRead > 0) {
            HexdumpFormat fmt;
            fmt.prefix = "  ";
            debug <<fmt.prefix;
            hexdump(debug, va, buffer, totalRead, fmt);
            debug <<"\n";
        }
    }

    return totalRead;
}

size_t
Linux::writeMemory(Address va, size_t nBytes, const uint8_t *buffer) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"PID " <<child_ <<": write " <<StringUtility::plural(nBytes, "bytes")
                       <<" at va " <<StringUtility::addrToString(va) <<"\n";

    if (0 == nBytes)
        return 0;
    const size_t nBytesDesired = nBytes;

    struct T {
        int fd;
        T(): fd(-1) {}
        ~T() {
            if (-1 != fd)
                close(fd);
        }
    } mem;

    // We could use PTRACE_POKEDATA, but it can be very slow if we're writing lots of memory since it only reads one word at a
    // time. We'd also need to worry about alignment so we don't inadvertently write past the end of a memory region when we're
    // try to write the last byte. Writing to  /proc/N/mem is faster and easier.
    std::string memName = "/proc/" + StringUtility::numberToString(child_) + "/mem";
    if (-1 == (mem.fd = open(memName.c_str(), O_RDWR)))
        throw Exception("cannot open \"" + memName + "\": " + strerror(errno));
    if (-1 == lseek(mem.fd, va, SEEK_SET))
        return 0;                                       // bad address
    size_t totalWritten = 0;
    const uint8_t *window = buffer;
    while (nBytes > 0) {
        ssize_t nWritten = write(mem.fd, window, nBytes);
        if (-1 == nWritten) {
            if (EINTR == errno) {
                continue;
            } else {
                break;
            }
        } else if (0 == nWritten) {
            break;
        } else {
            ASSERT_require(nWritten > 0);
            ASSERT_require((size_t)nWritten <= nBytes);
            nBytes -= nWritten;
            window += nWritten;
            totalWritten += nWritten;
        }
    }

    if (debug) {
        if (totalWritten < nBytesDesired)
            debug <<"  short write: only " <<StringUtility::plural(totalWritten, "bytes") <<" written to memory\n";
        if (totalWritten > 0) {
            HexdumpFormat fmt;
            fmt.prefix = "  ";
            debug <<fmt.prefix;
            hexdump(debug, va, buffer, totalWritten, fmt);
            debug <<"\n";
        }
    }

    return totalWritten;
}

void
Linux::runToBreakPoint(ThreadId tid) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": run to break point\n";
    if (breakPoints_.isEmpty()) {
        sendCommandInt(PTRACE_CONT, 0, sendSignal_);
        waitForChild();
    } else {
        while (1) {
            singleStep(tid);
            if (isTerminated())
                break;
            user_regs_struct regs;
            sendCommand(PTRACE_GETREGS, 0, &regs);
            if (breakPoints_.exists(getInstructionPointer(regs)))
                break;
        }
    }
}

void
Linux::runToSystemCall(ThreadId) {
    SAWYER_MESG(mlog[DEBUG]) <<"PID " <<child_ <<": run to system call\n";
    sendCommandInt(PTRACE_SYSCALL, 0, sendSignal_);
    waitForChild();
}

int
Linux::pendingSignal() const {
    return sendSignal_;
}

// class method
unsigned long
Linux::getPersonality() {
    return ::personality(0xffffffff);
}

// class method
void
Linux::setPersonality(unsigned long bits) {
    ::personality(bits);
}

Sawyer::Optional<Address>
Linux::findSystemCall() {
    // Byte patterns to search for that are capable of making a system call
    std::vector<std::vector<uint8_t>> needles;
    if (as<const Architecture::X86>(guessSpecimenArchitecture())) {
        needles.push_back(std::vector<uint8_t>{0xcd, 0x80}); // x86: INT 0x80
        needles.push_back(std::vector<uint8_t>{0x0f, 0x05}); // x86: SYSCALL
    }
    
    // Make sure the syscall is still there if we already found it. This is reasonally fast.
    if (syscallVa_) {
        bool found = false;
        for (const auto &needle: needles) {
            std::vector<uint8_t> buf(needle.size());
            size_t nRead = readMemory(*syscallVa_, buf.size(), buf.data());
            if (nRead == buf.size() && std::equal(buf.begin(), buf.end(), needle.begin())) {
                found = true;
                break;
            }
        }
        if (!found)
            syscallVa_.reset();
    }

    // If we haven't found a syscall (even if we previously searched for one) then search now.
    std::vector<MemoryMap::ProcessMapRecord> segments = MemoryMap::readProcessMap(child_);
    for (const MemoryMap::ProcessMapRecord &segment: segments) {
        if (syscallVa_)
            break;
        if ("[vvar]" == segment.comment) {
            // Linux vvar segment cannot be read from /proc/*/mem even though it's marked readable in the map
        } else if ((segment.accessibility & MemoryMap::READ_EXECUTE) != MemoryMap::READ_EXECUTE) {
            // Don't try to read memory which is not readable and executable
        } else {
            auto map = MemoryMap::instance();
            map->insertProcessPid(child_, std::vector<MemoryMap::ProcessMapRecord>{segment});
            for (const auto &needle: needles) {
                if ((syscallVa_ = map->findAny(AddressInterval::whole(), needle, MemoryMap::EXECUTABLE)))
                    break;
            }
        }
    }

    return syscallVa_;
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>());
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1, uint64_t arg2) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1, arg2});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1, arg2, arg3});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1, arg2, arg3, arg4});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
                         uint64_t arg5) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1, arg2, arg3, arg4, arg5});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
                         uint64_t arg5, uint64_t arg6) {
    return remoteSystemCall(tid, syscallNumber, std::vector<uint64_t>{arg1, arg2, arg3, arg4, arg5, arg6});
}

int64_t
Linux::remoteSystemCall(ThreadId tid, int syscallNumber, std::vector<uint64_t> args) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        const auto decl = syscallDecls_.lookup(syscallNumber);
        debug <<"PID " <<child_ <<": remote system call " <<syscallNumber <<(decl ? " (" + decl->name + ")" : "") <<"\n";
        debug <<"  arguments:\n";
        for (size_t i = 0; i < args.size(); ++i)
            debug <<"    arg " <<i <<": " <<StringUtility::toHex(args[i]) <<"\n";
    }

    // Find a system call that we can hijack to do our bidding.
    Sawyer::Optional<Address> syscallVa = findSystemCall();
    if (!syscallVa) {
        SAWYER_MESG(mlog[ERROR]) <<"syscall failed: cannot find a system call instruction\n";
        return -1;
    } else {
        SAWYER_MESG(debug) <<"  subverting system call instruction at " <<StringUtility::addrToString(*syscallVa) <<"\n";
    }

    // Registers that we'll need later, one per syscall argument.
    Architecture::Base::ConstPtr arch = guessSpecimenArchitecture();
    std::vector<RegisterDescriptor> regs;
    RegisterDescriptor syscallReg;
    if (as<const Architecture::X86>(arch)) {
        syscallReg = RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, arch->bitsPerWord());
        if (arch->bitsPerWord() == 32) {
            switch (args.size()) {
                case 6: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_bp, 0, 32)); // falls through
                case 5: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_di, 0, 32)); // falls through
                case 4: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_si, 0, 32)); // falls through
                case 3: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_dx, 0, 32)); // falls through
                case 2: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_cx, 0, 32)); // falls through
                case 1: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_bx, 0, 32));
            }
        } else if (arch->bitsPerWord() == 64) {
            switch (args.size()) {
                case 6: regs.push_back(RegisterDescriptor(x86_regclass_gpr, 9,          0, 64)); // falls through
                case 5: regs.push_back(RegisterDescriptor(x86_regclass_gpr, 8,          0, 64)); // falls through
                case 4: regs.push_back(RegisterDescriptor(x86_regclass_gpr, 10,         0, 64)); // falls through
                case 3: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_dx, 0, 64)); // falls through
                case 2: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_si, 0, 64)); // falls through
                case 1: regs.push_back(RegisterDescriptor(x86_regclass_gpr, x86_gpr_di, 0, 64));
            }
        } else {
            ASSERT_not_reachable("invalid x86 word size: " + boost::lexical_cast<std::string>(arch->bitsPerWord()));
        }
    } else if (arch) {
        ASSERT_not_implemented("unsupported debug architecture: " + arch->name());
    }
    std::reverse(regs.begin(), regs.end());

    // Save registers we're about to overwrite
    std::vector<Sawyer::Container::BitVector> savedRegs(args.size());
    for (size_t i = 0; i < args.size(); ++i)
        savedRegs[i] = readRegister(tid, regs[i]);
    Sawyer::Container::BitVector savedSyscallReg = readRegister(tid, syscallReg);

    // Assign arguments to registers
    for (size_t i = 0; i < args.size(); ++i)
        writeRegister(tid, regs[i], args[i]);
    writeRegister(tid, syscallReg, syscallNumber);

    // Single step through the syscall instruction
    Address ip = executionAddress(tid);
    executionAddress(tid, *syscallVa);
    singleStep(tid);
    executionAddress(tid, ip);
    int64_t retval = readRegister(tid, syscallReg).toSignedInteger();

    // Restore registers
    for (size_t i = 0; i < args.size(); ++i)
        writeRegister(tid, regs[i], savedRegs[i]);
    writeRegister(tid, syscallReg, savedSyscallReg);

    if (debug) {
        const auto decl = syscallDecls_.lookup(syscallNumber);
        debug <<"system call " <<syscallNumber <<(decl ? " (" + decl->name + ")" : "")
              <<" returned " <<StringUtility::toHex(retval) <<"\n";
    }
    return retval;
}

int
Linux::remoteOpenFile(ThreadId tid, const boost::filesystem::path &fileName, unsigned flags, mode_t mode) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"remoteOpenFile(tid=" <<tid
                       <<", fileName=\"" <<StringUtility::cEscape(fileName.string()) <<"\""
                       <<", flags=" <<flags
                       <<", mode=" <<mode
                       <<")\n";
    const Architecture::Base::ConstPtr arch = guessSpecimenArchitecture();
    ASSERT_not_null2(arch, "cannot guess specimen architecture");

    const unsigned openSyscall = [&arch]() {
        if (as<const Architecture::X86>(arch)) {
            if (arch->bitsPerWord() == 32) {
                return 5;
            } else if (arch->bitsPerWord() == 64) {
                return 2;
            } else {
                ASSERT_not_reachable("invalid word size");
            }
        } else {
            mlog[ERROR] <<"SYS_open not known for " <<arch->name() <<"\n";
            return -1;
        }
    }();
    SAWYER_MESG(debug) <<"  open syscall number is " <<openSyscall <<"\n";

    // Allocate space for the file name on the subordinate's stack. This is safer than using arbitrary writable memory because the
    // stack is thread-local (other threads won't interfere) and we're controlling this thread via ptrace so it won't be running
    // concurrently.
    const RegisterDescriptor SP = arch->registerDictionary()->stackPointerRegister();

    // Read current stack pointer
    Sawyer::Container::BitVector savedSp = readRegister(tid, SP);
    Address currentSp = savedSp.toInteger();
    SAWYER_MESG(debug) <<"  current stack pointer: " <<StringUtility::addrToString(currentSp) <<"\n";

    // Calculate space needed for filename (including null terminator) with proper alignment.  x86-64 requires 16-byte alignment,
    // x86-32 typically requires 4-byte alignment. On x86-64 we account for the 128-byte red zone defined by the System V ABI, which
    // is a region below the stack pointer that leaf functions may use without adjusting the stack pointer. Although this thread is
    // paused and we're only going to do a syscall before restoring the original memory contents, we'll avoid the red zone anyway.
    const size_t fileNameSize = fileName.string().size() + 1;
    const size_t alignment = arch->bitsPerWord() == 64 ? 16 : 4;
    const size_t allocSize = (fileNameSize + alignment - 1) & ~(alignment - 1);
    const size_t redZone = (arch->bitsPerWord() == 64) ? 128 : 0;
    Address nameVa = currentSp - allocSize - redZone;
    SAWYER_MESG(debug) <<"  allocating " <<allocSize <<" bytes on stack at " <<StringUtility::addrToString(nameVa) <<"\n";
    writeRegister(tid, SP, nameVa);

    // We want this operation to have no lasting effect, so read the existing memory values and restore them later.
    std::vector<uint8_t> savedName(fileNameSize);
    readMemory(nameVa, fileNameSize, savedName.data());
    writeMemory(nameVa, fileNameSize, (const uint8_t*)fileName.c_str());

    // Execute the system call
    int retval = remoteSystemCall(tid, openSyscall, std::vector<uint64_t>{nameVa, flags, mode});
    SAWYER_MESG(debug) <<"  remote syscall returned " <<retval <<"\n";

    // Restore the original memory contents and the stack pointer (deallocates our stack space)
    writeMemory(nameVa, savedName.size(), savedName.data());
    writeRegister(tid, SP, savedSp);

    return retval;
}

int
Linux::remoteCloseFile(ThreadId tid, unsigned fd) {
    const unsigned closeSyscall = [this]() {
        if (const auto arch = guessSpecimenArchitecture()) {
            if (as<const Architecture::X86>(arch)) {
                if (arch->bitsPerWord() == 32) {
                    return 6;
                } else if (arch->bitsPerWord() == 64) {
                    return 3;
                } else {
                    ASSERT_not_reachable("invalid word size");
                }
            } else {
                mlog[ERROR] <<"SYS_open not known for " <<arch->name() <<"\n";
                return -1;
            }
        } else {
            ASSERT_not_reachable("no architecture");
        }
    }();

    return remoteSystemCall(tid, closeSyscall, std::vector<uint64_t>{fd});
}

Address
Linux::remoteMmap(const ThreadId tid, const Address va, const size_t nBytes, const unsigned prot, const unsigned flags,
                   const boost::filesystem::path &fileName, const off_t offset_) {
    const unsigned mmapSyscall = [this]() {
        if (const auto arch = guessSpecimenArchitecture()) {
            if (as<const Architecture::X86>(arch)) {
                if (arch->bitsPerWord() == 32) {
                    return 192;                         // SYS_mmap2
                } else if (arch->bitsPerWord() == 64) {
                    return 9;                           // SYS_mmap
                } else {
                    ASSERT_not_reachable("invalid word size");
                }
            } else {
                mlog[ERROR] <<"SYS_open not known for " <<arch->name() <<"\n";
                return -1;
            }
        } else {
            ASSERT_not_reachable("no architecture");
        }
    }();

    const uint64_t offset = boost::numeric_cast<uint64_t>(offset_);
    const int fd = remoteOpenFile(tid, fileName, O_RDONLY, 0);
    if (fd < 0)
        return fd;
    const Address retval = remoteSystemCall(tid, mmapSyscall, std::vector<uint64_t>{va, nBytes, prot, flags, (unsigned)fd, offset});
    remoteCloseFile(tid, fd);
    return retval;
}
} // namespace
} // namespace
} // namespace

#endif
