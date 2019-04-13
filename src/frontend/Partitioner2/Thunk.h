#ifndef ROSE_BinaryAnalysis_Partitioner2_Thunk_H
#define ROSE_BinaryAnalysis_Partitioner2_Thunk_H

#include <Partitioner2/BasicTypes.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Individual thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Test whether x86 instructions begin with "jmp ADDRESS".
 *
 *  Returns 1 if the first instruction is "jmp ADDRESS" where ADDRESS is a constant. Otherwise returns zero. */
size_t isX86JmpImmThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instruction begin with "jmp [ADDRESS]".
 *
 *  Returns 1 if the first instruction is "jmp [ADDRESS]" where ADDRESS is a constant. Otherwise returns zero. */
size_t isX86JmpMemThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with an LEA JMP pair.
 *
 *  Returns 2 if the first two instructions are "lea ecx, [ebp + C]; jmp ADDR" where C and ADDR are constants. Returns zero
 *  otherwise. */
size_t isX86LeaJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with "mov R, [ADDR]; jmp R".
 *
 *  Returns 2 if the first two instructions are "mov R, [ADDR]; jmp R" where R is a register and ADDR is a constant. Returns
 *  zero otherwise. */
size_t isX86MovJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with "add R, C; jmp ADDR".
 *
 *  Returns 2 if the first two instructions are "add R, C; jmp ADDR" where R is one of the cx registers, and C and ADDR are
 *  constants. Returns zero otherwise. */
size_t isX86AddJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collective thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function signature for finding thunks.
 *
 *  These functions take a partitioner and a sequence of one or more instructions disassembled from memory and determine if the
 *  sequence begins with instructions that look like a thunk of some sort. If they do, then the function returns the number of
 *  initial instructions that compose the thunk, otherwise it returns zero. */
typedef size_t(*ThunkPredicate)(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** List of thunk predicates. */
class ThunkPredicates: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<ThunkPredicates> Ptr;

private:
    /** Reference counting pointer to @ref ThunkScanner. */
    std::vector<ThunkPredicate> predicates_;

protected:
    // use 'instance' instead
    ThunkPredicates() {}

public:
    /** Allocating constructor */
    static Ptr instance() {
        return Ptr(new ThunkPredicates);
    }

    /** Construct collective predicate with all built-in predicates.
     *
     *  Returns a collective predicate that contains all the built-in predicates known to this library. */
    static Ptr allThunks();

    /** Construct collective predicate for matching thunks to create functions.
     *
     *  Constructs a collective predicate that contains individual predicates that are suitable for finding thunks when scanning
     *  memory to find new function prologues. */
    static Ptr functionMatcherThunks();

    /** Property: The list of predicates that will test for thunks.
     *
     * @{ */
    const std::vector<ThunkPredicate>& predicates() const { return predicates_; }
    std::vector<ThunkPredicate>& predicates() { return predicates_; }
    /** @} */

    /** Test whether instructions begin with a thunk.
     *
     *  Returns non-zero if the specified list of instructions begins with what appears to be a thunk. The return value is the
     *  number of leading instructions that are part of the thunk. The determination is made by invoking each predicate from
     *  the @ref predicates list in the order of that list and returning the value returned by the first predicate that returns
     *  non-zero. */
    size_t isThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Thunk utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Split thunks off from start of functions.
 *
 *  Splits as many thunks as possible off the front of all functions currently attached to the partitioner's CFG. */
void splitThunkFunctions(Partitioner&, const ThunkPredicates::Ptr&);

} // namespace
} // namespace
} // namespace

#endif
