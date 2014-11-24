#include "sage3basic.h"
#include "BinaryDebugger.h"

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

#else

# include <sys/ptrace.h>
# include <sys/user.h>
# include <sys/wait.h>

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
