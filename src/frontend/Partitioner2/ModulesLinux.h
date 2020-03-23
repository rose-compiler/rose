#ifndef ROSE_Partitioner2_ModulesLinux_H
#define ROSE_Partitioner2_ModulesLinux_H

#include <Partitioner2/Modules.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BinarySystemCall.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utilities for Linux. */
namespace ModulesLinux {

/** Create a system call analyzer suitable for this architecture. */
SystemCall systemCallAnalyzer(const Partitioner&, const boost::filesystem::path &syscallHeader = "");

/** Basic block callback to detect system calls that don't return.
 *
 *  Examines the instructions of a basic block to determine if they end with a system call that doesn't return, such as Linux's
 *  "exit". */
class SyscallSuccessors: public BasicBlockCallback {
    SystemCall analyzer_;
protected:
    SyscallSuccessors(const Partitioner&, const boost::filesystem::path &syscallHeader);
public:
    /** Allocating constructor.
     *
     *  An optional Linux system call header file can be provided to override the default. */
    static Ptr instance(const Partitioner&, const boost::filesystem::path &syscallHeader = "");

    virtual bool operator()(bool chain, const Args&) ROSE_OVERRIDE;
};

/** Basic block callback to add "main" address as a function.
 *
 *  If the last instruction of a basic block is a call to __libc_start_main in a shared library, then one of its arguments is
 *  the address of the C "main" function. */
class LibcStartMain: public BasicBlockCallback {
    Sawyer::Optional<rose_addr_t> mainVa_;
public:
    /** Shared ownership pointer to LibcStartMain callback. */
    typedef Sawyer::SharedPointer<LibcStartMain> Ptr;

    static Ptr instance() { return Ptr(new LibcStartMain); } /**< Allocating constructor. */
    virtual bool operator()(bool chain, const Args&) ROSE_OVERRIDE;

    /** Give the name "main" to the main function if it has no name yet. */
    void nameMainFunction(const Partitioner&) const;
};

/** Adds comments to system call instructions.
 *
 *  Adds a comment to each system call instruction for which the actual system call can be identified. A Linux header file can
 *  be provided to override the default location. */
void nameSystemCalls(const Partitioner&, const boost::filesystem::path &syscallHeader = "");

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
