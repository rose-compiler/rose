#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesM68k_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesM68k_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utility functions for M68k. */
namespace ModulesM68k {

/** Matches an M68k function prologue with LINK instruction.
 *
 *  Matches a "LINK.W A6, ###" instruction where "###" is zero or negative, and creates a function at that address. */
class MatchLink: public FunctionPrologueMatcher {
protected:
    FunctionPtr function_;

protected:
    MatchLink();
public:
    ~MatchLink();

    /** Allocating constructor. */
    static Ptr instance();
    virtual std::vector<FunctionPtr> functions() const override;
    virtual bool match(const PartitionerConstPtr&, Address anchor) override;
};

/** Matches M68k function padding. */
class MatchFunctionPadding: public FunctionPaddingMatcher {
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new MatchFunctionPadding); }
    virtual Address match(const PartitionerConstPtr&, Address anchor) override;
};

/** Adjusts basic block successors for M68k "switch" statements. */
class SwitchSuccessors: public BasicBlockCallback {
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new SwitchSuccessors); }
    virtual bool operator()(bool chain, const Args&) override;
};

/** Find functions for an interrupt vector.
 *
 *  M68k interrupt vectors have 256 4-byte addresses, for a total of 1024 bytes.  This function, when given an interrupt vector
 *  base address, will read the memory and create up to 256 new functions. */
std::vector<FunctionPtr> findInterruptFunctions(const PartitionerConstPtr&, Address vectorVa);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
