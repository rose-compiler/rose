#ifndef ROSE_Partitioner2_ModulesX86_H
#define ROSE_Partitioner2_ModulesX86_H

#include <Partitioner2/Modules.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

/** Matches an x86 function prologue.
 *
 *  The standard x86 function prologue is:
 *
 * @code
 *  push ebp
 *  mov ebp, esp
 * @endcode
 *
 *  The width of ebp and esp must match the word size for the architecture (i.e., they must be EBP and ESP for the i386 family,
 *  and RBP, RSP for the amd64 family). */
class MatchStandardPrologue: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchStandardPrologue); } /**< Allocating constructor. */
    virtual Function::Ptr function() const /*override*/ { return function_; }
    virtual bool match(const Partitioner *partitioner, rose_addr_t anchor) /*override*/;
};

/** Matches an x86 function prologue with hot patch.
 *
 *  A hot-patch prologue is a MOV EDX, EDX instruction followed by a standard prologue. */
class MatchHotPatchPrologue: public MatchStandardPrologue {
public:
    static Ptr instance() { return Ptr(new MatchHotPatchPrologue); } /**< Allocating constructor. */
    virtual Function::Ptr function() const /*override*/ { return function_; }
    virtual bool match(const Partitioner *partitioner, rose_addr_t anchor) /*override*/;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
