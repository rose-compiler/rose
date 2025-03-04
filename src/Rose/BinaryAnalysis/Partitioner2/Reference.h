#ifndef ROSE_BinaryAnalysis_Partitioner2_Reference_H
#define ROSE_BinaryAnalysis_Partitioner2_Reference_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <set>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Reference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Reference to a function, basic block, instruction, or address.
 *
 *  A reference can be an empty reference, or it points to a function and/or basic block and/or instruction, or it is an
 *  address in any combination.  A reference has a granularity depending on the finest thing to which it points (see @ref
 *  granularity).  All non-empty references have a computable address regardless of whether an address is actually stored.
 *  References are comparable. */
class Reference {
    FunctionPtr function_;
    BasicBlockPtr bblock_;
    SgAsmInstruction *insn_;
    Sawyer::Optional<Address> address_;
public:
    /** Granularity. */
    enum Granularity {
        EMPTY,                                      /**< Reference points to nothing and has no address. */
        ADDRESS,                                    /**< Reference points to nothing but has an address. */
        INSTRUCTION,                                /**< Reference points to an instruction but no stored address. */
        BASIC_BLOCK,                                /**< Reference points to a basic block but not an instruction
                                                     *   or a stored address. */
        FUNCTION                                    /**< Reference points to a function but not a basic block or
                                                     *   instruction, and has no stored address. */
    };

    ~Reference();

    /** Constructor.
     *
     *  A reference can be constructed from any combination of a function, basic block, instruction, and address.
     *
     * @{ */
    explicit Reference(const FunctionPtr&);

    explicit Reference(const FunctionPtr&, const BasicBlockPtr&, SgAsmInstruction* = nullptr,
                       const Sawyer::Optional<Address>& = Sawyer::Nothing());

    explicit Reference(const BasicBlockPtr&, SgAsmInstruction* = nullptr, const Sawyer::Optional<Address>& =Sawyer::Nothing());

    explicit Reference(SgAsmInstruction*, const Sawyer::Optional<Address>& = Sawyer::Nothing());

    explicit Reference(Address address);

    Reference();
    /** @} */

    /** Reference granularity.
     *
     *  Returns the granularity for the reference.  The granularity is the finest thing stored in the reference. The order
     *  from finest to coarsest is: EMPTY, ADDRESS, INSTRUCTION, BASIC_BLOCK, FUNCTION. */
    Granularity granularity() const;

    /** Predicate for emptiness.
     *
     *  Returns true if this reference points to nothing and has no stored address. */
    bool isEmpty() const;

    /** Predicate for having a function.
     *
     *  Returns true if the reference points to a function. */
    bool hasFunction() const;

    /** Optional function pointer. */
    FunctionPtr function() const;

    /** Predicate for having a basic block.
     *
     *  Returns true if the reference points to a basic block. */
    bool hasBasicBlock() const;

    /** Optional basic block pointer. */
    BasicBlockPtr basicBlock() const;

    /** Predicate for having an instruction.
     *
     *  Returns true if the reference points to an instruction. */
    bool hasInstruction() const;

    /** Optional instruction pointer. */
    SgAsmInstruction* instruction() const;

    /** Predicate for having a stored address.
     *
     *  All non-empty references have a computable address, but an address can also be stored explicitly.  This predicate
     *  returns true when an address is stored explicitly. */
    bool hasExplicitAddress() const;

    /** Address.
     *
     *  All non-empty references have either an explicitly stored address or an address that can be obtained from an
     *  instruction, basic block, or function.  This method returns the first address found from that list. */
    Address address() const;

    /** Whether two references are equal.
     *
     *  Two references are equal if they are both empty, or else if they have the same address and granularity. */
    bool operator==(const Reference &other) const;

    /** Compare two references.
     *
     *  Returns true when this reference is less than the @p other reference.  The empty reference is less than all
     *  non-empty references.  A non-empty reference is less than or greater than another non-empty reference if its
     *  address is less than or greater than the other reference's address.  When two references have the same address then
     *  the finer granularity reference is less than the coarser reference. */
    bool operator<(const Reference &other) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
