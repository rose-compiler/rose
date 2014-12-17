#ifndef ROSE_BinaryAnalysis_BinaryDebugger_H
#define ROSE_BinaryAnalysis_BinaryDebugger_H

namespace rose {
namespace BinaryAnalysis {

/** Simple debugger.
 *
 *  This class implements a very simple debugger. */
class BinaryDebugger {
public:
    enum DetachMode { KILL, DETACH, CONTINUE, NOTHING };
private:
    int child_;                                         // process being debugged (int, not pid_t, for Windows portability)
    DetachMode howDetach_;                              // how to detach from the subordinate
    int wstat_;                                         // last status from waitpid
    AddressIntervalSet breakpoints_;                    // list of breakpoint addresses
    int sendSignal_;                                    // pending signal

public:
    BinaryDebugger(): child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0) {}

    BinaryDebugger(int pid)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0) {
        attach(pid);
    }

    BinaryDebugger(const std::string &exeName)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0) {
        attach(exeName);
    }

    BinaryDebugger(const std::vector<std::string> &exeNameAndArgs)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0) {
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

    /** Returns true if the subordinate terminated. */
    bool isTerminated();

    /** String describing how the subordinate process terminated. */
    std::string howTerminated();

private:
    // Wait for subordinate or throw on error
    void waitForChild();

};

} // namespace
} // namespace

#endif
