#ifndef ROSE_Partitioner2_ModulesLinux_H
#define ROSE_Partitioner2_ModulesLinux_H

#include <Partitioner2/Modules.h>

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

/** Adds comments to system call instructions.
 *
 *  Adds a comment to each system call instruction for which the actual system call can be identified. A Linux header file can
 *  be provided to override the default location. */
void
nameSystemCalls(const Partitioner&, const boost::filesystem::path &syscallHeader = "");

} // namespace
} // namespace
} // namespace
} // namespace

#endif

