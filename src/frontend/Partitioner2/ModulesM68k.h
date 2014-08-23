#ifndef ROSE_Partitioner2_ModulesM68k_H
#define ROSE_Partitioner2_ModulesM68k_H

#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesM68k {

/** Matches an M68k function prologue with LINK instruction.
 *
 *  Matches a "LINK.W A6, ###" instruction where "###" is zero or negative, and creates a function at that address. */
class MatchLink: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchLink); } /**< Allocating constructor. */
    virtual Function::Ptr function() const /*override*/ { return function_; }
    virtual bool match(const Partitioner *partitioner, rose_addr_t anchor) /*override*/;
};

/** Adjusts basic block successors for M68k "switch" statements. */
class SwitchSuccessors: public SuccessorCallback {
public:
    static Ptr instance() { return Ptr(new SwitchSuccessors); } /**< Allocating constructor. */
    virtual bool operator()(bool chain, const Args&) /*override*/;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
