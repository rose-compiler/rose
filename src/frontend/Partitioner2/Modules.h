#ifndef ROSE_Partitioner2_Modules_H
#define ROSE_Partitioner2_Modules_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>

#include <sawyer/SharedPointer.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for adjusting basic block successors.
 *
 *  As each instruction of a basic block is discovered the partitioner calculates its control flow successors to decide
 *  what to do.  The successors are calculated primarily by evaluating the basic block instructions in a symbolic domain,
 *  and if that fails, by looking at the final instruction's concrete successors.  Once these successors are obtained, the
 *  partitioner invokes user callbacks so that the user has a chance to make adjustments.
 *
 *  By time this callback is invoked, the basic block's initial successors have been computed and cached in the basic
 *  block. That list can be obtained by invoking @ref Partitioner::basicBlockSuccessors or by accessing the @ref
 *  BasicBlock::successors cache directly. Likewise, the successor list can be adjusted by invoking methods in the partitioner
 *  API or by modifying the cache directly.
 *
 *  The partitioner expects callbacks to have shared ownership and references them only via Sawyer::SharedPointer.  Therefore,
 *  subclasses should implement an @c instance class method that allocates a new object and returns a shared pointer. */
class SuccessorCallback: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<SuccessorCallback> Ptr;

    /** Arguments passed to the callback. */
    struct Args {
        const Partitioner *partitioner;                 /**< Partitioner requesting basic block successors. */
        BasicBlock::Ptr bblock;                         /**< Basic block whose successors are to be computed. */
        Args(const Partitioner *partitioner, const BasicBlock::Ptr &bblock)
            : partitioner(partitioner), bblock(bblock) {}
    };

    /** Callback method.
     *
     *  This is the method invoked for the callback.  The @p chain argument is the return value from the previous callback in
     *  the list (true for the first callback).  The successor callbacks use @p chain to indicate whether subsequent callbacks
     *  should do anything. */
    virtual bool operator()(bool chain, const Args&) = 0;
};


/** Base class for matching an instruction pattern.
 *
 *  Instruction matchers are generally reference from the partitioner via shared-ownership pointers.  Subclasses must
 *  implement a @ref match method that performs the actual matching. */
class InstructionMatcher: public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
     *  partitioners are copied. */
    typedef Sawyer::SharedPointer<InstructionMatcher> Ptr;

    /** Attempt to match an instruction pattern.
     *
     *  If the subclass implementation is able to match instructions, bytes, etc. anchored at the @p anchor address then it
     *  should return true, otherwise false.  The anchor address will always be valid for the situation (e.g., if the
     *  partitioner is trying to match something anchored at an address that is not in the CFG, then the @p anchor will be
     *  such an address; if it is trying to match something that is definitely an instruction then the address will be
     *  mapped with execute permission; etc.).  This precondition makes writing matchers that match against a single
     *  address easier to write, but matchers that match at additional locations must explicitly check those other
     *  locations with the same conditions (FIXME[Robb P. Matzke 2014-08-04]: perhaps we should pass those conditions as an
     *  argument). */
    virtual bool match(const Partitioner*, rose_addr_t anchor) = 0;
};


/** Base class for matching function prologues.
 *
 *  A function prologue is a pattern of bytes or instructions that typically mark the beginning of a function.  For
 *  instance, many x86-based functions start with "PUSH EBX; MOV EBX, ESP" while many M68k functions begin with a single
 *  LINK instruction affecting the A6 register.  A subclass must implement the @ref match method that does the actual
 *  pattern matching.  If the @ref match method returns true, then the partitioner will call the @ref function method to
 *  obtain a function object.
 *
 *  The matcher will be called only with anchor addresses that are mapped with execute permission and which are not a
 *  starting address of any instruction in the CFG.  The matcher should ensure similar conditions are met for any
 *  additional addresses, especially the address returned by @ref functionVa. */
class FunctionPrologueMatcher: public InstructionMatcher {
public:
    /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
     *  partitioners are copied. */
    typedef Sawyer::SharedPointer<FunctionPrologueMatcher> Ptr;

    /** Returns the function for the previous successful match.  If the previous call to @ref match returned true then this
     *  method should return a function for the matched function prologue.  Although the function returned by this method
     *  is often at the same address as the anchor for the match, it need not be.  For instance, a matcher could match
     *  against some amount of padding followed the instructions for setting up the stack frame, in which case it might
     *  choose to return a function that starts at the stack frame setup instructions and includes the padding as static
     *  data. The partitioner will never call @ref function without first having called @ref match. */
    virtual Function::Ptr function() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Generic modules
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Modules {

/** Finds functions for which symbols exist.
 *
 *  Scans the specified AST to find symbols that point to functions and makes a function at each such address.  A function is
 *  made only if an instruction can be disassembled at the address. The return value is a sorted list of unique functions. */
std::vector<Function::Ptr> findSymbolFunctions(const Partitioner&, SgAsmGenericHeader*);

} // namespace

} // namespace
} // namespace
} // namespace

#endif
