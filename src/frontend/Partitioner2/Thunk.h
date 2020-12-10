#ifndef ROSE_BinaryAnalysis_Partitioner2_Thunk_H
#define ROSE_BinaryAnalysis_Partitioner2_Thunk_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicTypes.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Individual thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Return type for thunk detectors.
 *
 *  Returns the number of instructions in the thunk, and the name of the pattern that matched those instructions. If no thunk
 *  is detected then the size will be zero and the string is empty. */
struct ThunkDetection {
    size_t nInsns;                                      /**< Number of instructions that are part of the thunk. */
    std::string name;                                   /**< Name of the pattern that matched the instructions. */

    /** Constructor for detecting no thunk. */
    ThunkDetection()
        : nInsns(0) {}

    /** Constructor for a detected thunk. */
    ThunkDetection(size_t nInsns, const std::string &name)
        : nInsns(nInsns), name(name) {}

    // C++03 safe bool conversion since we don't have explicit operator bool.
private:
    typedef void(ThunkDetection::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    operator unspecified_bool() const {
        return nInsns ? &ThunkDetection::this_type_does_not_support_comparisons : 0;
    }
};

/** Test whether x86 instructions begin with "jmp ADDRESS".
 *
 *  Tries to match "jmp ADDRESS" where ADDRESS is a constant. */
ThunkDetection isX86JmpImmThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instruction begin with "jmp [ADDRESS]".
 *
 *  Tries to match "jmp [ADDRESS]" where ADDRESS is a constant. */
ThunkDetection isX86JmpMemThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with an LEA JMP pair.
 *
 *  Tries to match "lea ecx, [ebp + C]; jmp ADDR" where C and ADDR are constants. */
ThunkDetection isX86LeaJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with "mov R, [ADDR]; jmp R".
 *
 *  Tries to match "mov R, [ADDR]; jmp R" where R is a register and ADDR is a constant. */
ThunkDetection isX86MovJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);

/** Test whether x86 instructions begin with "add R, C; jmp ADDR".
 *
 *  Tries to match "add R, C; jmp ADDR" where R is one of the cx registers and C and ADDR are constants. */
ThunkDetection isX86AddJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collective thunk predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Function signature for finding thunks.
 *
 *  These functions take a partitioner and a sequence of one or more instructions disassembled from memory and determine if the
 *  sequence begins with instructions that look like a thunk of some sort. If they do, then the function returns the number of
 *  initial instructions that compose the thunk and the name of the pattern that matched, otherwise it returns zero and an empty
 *  string. */
typedef ThunkDetection(*ThunkPredicate)(const Partitioner&, const std::vector<SgAsmInstruction*>&);

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
     *  number of leading instructions that are part of the thunk and the name of the thunk detector that matched. The
     *  determination is made by invoking each predicate from the @ref predicates list in the order of that list and returning
     *  the value returned by the first predicate that returns non-zero. */
    ThunkDetection isThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&) const;
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
#endif
