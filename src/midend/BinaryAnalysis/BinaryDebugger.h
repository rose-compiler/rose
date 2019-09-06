#ifndef ROSE_BinaryAnalysis_Debugger_H
#define ROSE_BinaryAnalysis_Debugger_H

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <Sawyer/BitVector.h>

namespace Rose {
namespace BinaryAnalysis {

/** Shared-ownership pointer to @ref Debugger. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class Debugger> DebuggerPtr;

/** Simple debugger.
 *
 *  This class implements a very simple debugger. */
class Debugger: private boost::noncopyable, public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer to @ref Debugger. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<Debugger> Ptr;

    /** How to detach from a process when this object is destroyed. */
    enum DetachMode {
        KILL,                                           /**< Kill the process. */
        DETACH,                                         /**< Simply detach leaving process in current state. */
        CONTINUE,                                       /**< Detach from process and cause it to continue running. */
        NOTHING                                         /**< Do nothing. */
    };

    /** Flags controlling operation. */
    enum Flag {
        ATTACH          = 0x00000001,                   /**< Attach to existing process. */
        REDIRECT_INPUT  = 0x00000002,                   /**< Redirect input from /dev/null. */
        REDIRECT_OUTPUT = 0x00000004,                   /**< Redirect output to /dev/null. */
        REDIRECT_ERROR  = 0x00000008,                   /**< Redirect standard error to /dev/null. */
        CLOSE_FILES     = 0x00000010,                   /**< Close all file descriptors > 2. */
        DEFAULT_FLAGS   = 0x00000013                    /**< Default flags. */
    };

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
    unsigned flags_;                                    // operational flags; Flag bit vector
    const RegisterDictionary *regdict_;                 // supported registers

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    Debugger()
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          flags_(DEFAULT_FLAGS), regdict_(NULL) {
        init();
    }

    Debugger(int pid, unsigned flags)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          flags_(flags), regdict_(NULL) {
        init();
        attach(pid, flags);
    }

    Debugger(const boost::filesystem::path &exeName, unsigned flags)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          flags_(flags), regdict_(NULL) {
        init();
        attach(exeName, flags);
    }

    Debugger(const boost::filesystem::path &exeName, const std::vector<std::string> &args, unsigned flags)
        : child_(0), howDetach_(KILL), wstat_(-1), sendSignal_(0), kernelWordSize_(0), regsPageStatus_(REGPAGE_NONE),
          flags_(flags), regdict_(NULL) {
        init();
        attach(exeName, args, flags);
    }

public:
    ~Debugger() {
        detach();
    }

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    /** Create a debugger object that isn't attached to any subordinate process. */
    static Ptr instance() {
        return Ptr(new Debugger);
    }

    /** Create a debugger attached to an already running subordinate process. */
    static Ptr instance(int pid, unsigned flags = DEFAULT_FLAGS) {
        return Ptr(new Debugger(pid, flags));
    }

    /** Create a new debugger by starting the specified program with no arguments. */
    static Ptr instance(const boost::filesystem::path &exeName, unsigned flags = DEFAULT_FLAGS) {
        return Ptr(new Debugger(exeName, flags));
    }

    /** Create a new debugger by starting the specified program with arguments. */
    static Ptr instance(const boost::filesystem::path &exeName, const std::vector<std::string> &args,
                        unsigned flags = DEFAULT_FLAGS) {
        return Ptr(new Debugger(exeName, args, flags));
    }
    
    //----------------------------------------
    // Attaching to subordinate
    //----------------------------------------
public:
    /** Attach to an existing process.
     *
     *  Arranges for an existing process to be debugged.  If the @p ATTACH @ref Flag "flag" is set (the default) then the
     *  debugger attempts to attach to that process and gain control, otherwise it assumes that the calling process has already
     *  done that. */
    void attach(int pid, unsigned flags = DEFAULT_FLAGS);

    /** Program to debug.
     *
     *  The program can be specified as a single name or as a name and arguments.
     *
     * @{ */
    void attach(const boost::filesystem::path &fileName, unsigned flags = DEFAULT_FLAGS);
    void attach(const boost::filesystem::path &fileName, const std::vector<std::string> &args, unsigned flags = DEFAULT_FLAGS);
    /** @} */

    /** Returns true if attached to a subordinate.  Return value is the subordinate process ID. */
    int isAttached() { return child_; }

    /** Detach from the subordinate. */
    void detach();

    /** Terminate the subordinate. */
    void terminate();

    //----------------------------------------
    // Operations on a subordinate
    //----------------------------------------
public:
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

    /** Run until the next system call.
     *
     *  The subordinate is run until it is about to make a system call or has just returned from a system call, or it has
     *  encountered a signal or terminated.  Execution does not stop at break points. */
    void runToSyscall();

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
    Sawyer::Container::BitVector readRegister(RegisterDescriptor);

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. The implementation accesses the subordinate memory via proc filesystem rather than
     *  sending PTRACE_PEEKDATA commands. This allows large areas of memory to be read efficiently. */
    size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer);

    /** Returns true if the subordinate terminated. */
    bool isTerminated();

    /** String describing how the subordinate process terminated. */
    std::string howTerminated();

    /** Available registers. */
    const RegisterDictionary* registerDictionary() const;

private:
    // Initialize tables during construction
    void init();

    // Wait for subordinate or throw on error
    void waitForChild();

    // Open /dev/null with the specified flags as the indicated file descriptor, closing what was previously on that
    // descriptor. If an error occurs, the targetFd is closed anyway.
    void devNullTo(int targetFd, int openFlags);
};

} // namespace
} // namespace

#endif
