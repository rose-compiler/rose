#ifndef ROSE_BinaryAnalysis_BinaryDebugger_H
#define ROSE_BinaryAnalysis_BinaryDebugger_H

#include <sawyer/BitVector.h>

namespace rose {
namespace BinaryAnalysis {

/** Simple debugger.
 *
 *  This class implements a very simple debugger. */
class BinaryDebugger {
public:
    enum DetachMode { KILL, DETACH, CONTINUE, NOTHING };
private:
    typedef Sawyer::Container::Map<RegisterDescriptor, size_t> UserRegDefs;
    enum RegPageStatus { REGPAGE_NONE, REGPAGE_REGS, REGPAGE_FPREGS };

    int child_;                                         // process being debugged (int, not pid_t, for Windows portability)
    DetachMode howDetach_;                              // how to detach from the subordinate
    int wstat_;                                         // last status from waitpid
    AddressIntervalSet breakpoints_;                    // list of breakpoint addresses
    int sendSignal_;                                    // pending signal
    UserRegDefs userRegDefs_;                           // how registers map to user_regs_struct in <sys/user.h>
    UserRegDefs userFpRegDefs_;                         // how registers map to user_fpregs_struct in <sys/user.h>
    size_t kernelWordSize_;                             // cached width in bits of kernel's words
    uint8_t regsPage_[512];                             // latest register information read from subordinate
    RegPageStatus regsPageStatus_;                      // what are the contents of regPage_?

public:
    BinaryDebugger()
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE) {
        init();
    }

    BinaryDebugger(int pid)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE) {
        init();
        attach(pid);
    }

    BinaryDebugger(const std::string &exeName)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE) {
        init();
        attach(exeName);
    }

    BinaryDebugger(const std::vector<std::string> &exeNameAndArgs)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE) {
        init();
        attach(exeNameAndArgs);
    }
    
    ~BinaryDebugger() {
        detach();
    }

private:
    BinaryDebugger(const BinaryDebugger&);              // not copyable
    BinaryDebugger& operator=(const BinaryDebugger&);   // not copyable

public:
    /** Attach to an existing process.
     *
     *  Arranges for an existing process to be debugged.  If @p attach is true then the debugger attempts to attach to that
     *  process and gain control, otherwise it assumes that the calling process has already done that. */
    void attach(int pid, bool attach=true);

    /** Program to debug.
     *
     *  The program can be specified as a single name or as a name and arguments.
     *
     * @{ */
    void attach(const std::string &fileName);
    void attach(const std::vector<std::string> &fileNameAndArgs);
    /** @} */

    /** Returns true if attached to a subordinate.  Return value is the subordinate process ID. */
    int isAttached() { return child_; }

    /** Detach from the subordinate. */
    void detach();

    /** Terminate the subordinate. */
    void terminate();

    /** Set execution address. */
    void executionAddress(rose_addr_t va);

    /** Get execution address. */
    rose_addr_t executionAddress();

    /** Set breakpoints. */
    void setBreakpoint(const AddressInterval&);

    /** Remove breakpoints. */
    void clearBreakpoint(const AddressInterval&);

    /** Remove all breakpoints. */
    void clearBreakpoints() { breakpoints_.clear(); }

    /** Execute one instruction. */
    void singleStep();

    /** Run until the next breakpoint is reached. */
    void runToBreakpoint();

    /** Obtain and cache kernel's word size in bits.  The wordsize of the kernel is not necessarily the same as the word size
     * of the compiled version of this header. */
    size_t kernelWordSize();

    /** Read subordinate register.  Some registers are wider than what can be easily represented on this architecture (e.g.,
     *  x86 XMM registers are 128 bits), therefore return the result as a bit vector. If you want just the low-order 64 bits,
     *  invoke it like this:
     *
     * @code
     *  uint64_t value = debugger.readRegister(RIP).toInteger();
     * @endcode */
    Sawyer::Container::BitVector readRegister(const RegisterDescriptor&);

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. The implementation accesses the subordinate memory via proc filesystem rather than
     *  sending PTRACE_PEEKDATA commands. This allows large areas of memory to be read efficiently. */
    size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer);

    /** Returns true if the subordinate terminated. */
    bool isTerminated();

    /** String describing how the subordinate process terminated. */
    std::string howTerminated();

private:
    // Initialize tables during construction
    void init();

    // Wait for subordinate or throw on error
    void waitForChild();

};

} // namespace
} // namespace

#endif
