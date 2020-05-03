#ifndef ROSE_Partitioner2_Reference_H
#define ROSE_Partitioner2_Reference_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Function.h>
#include <set>

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
    Function::Ptr function_;
    BasicBlock::Ptr bblock_;
    SgAsmInstruction *insn_;
    Sawyer::Optional<rose_addr_t> address_;
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

    /** Constructor.
     *
     *  A reference can be constructed from any combination of a function, basic block, instruction, and address.
     *
     * @{ */
    explicit Reference(const Function::Ptr &function, const BasicBlock::Ptr &bblock=BasicBlock::Ptr(),
                       SgAsmInstruction *insn=NULL, const Sawyer::Optional<rose_addr_t> &address=Sawyer::Nothing())
        : function_(function), bblock_(bblock), insn_(insn), address_(address) {}


    explicit Reference(const BasicBlock::Ptr &bblock, SgAsmInstruction *insn=NULL,
                       const Sawyer::Optional<rose_addr_t> &address=Sawyer::Nothing())
        : bblock_(bblock), insn_(insn), address_(address) {}

    explicit Reference(SgAsmInstruction *insn, const Sawyer::Optional<rose_addr_t> &address=Sawyer::Nothing())
        : insn_(insn), address_(address) {}

    explicit Reference(rose_addr_t address)
        : insn_(NULL), address_(address) {}

    Reference()
        : insn_(NULL) {}
    /** @} */

    /** Reference granularity.
     *
     *  Returns the granularity for the reference.  The granularity is the finest thing stored in the reference. The order
     *  from finest to coarsest is: EMPTY, ADDRESS, INSTRUCTION, BASIC_BLOCK, FUNCTION. */
    Granularity granularity() const {
        if (address_)
            return ADDRESS;
        if (insn_)
            return INSTRUCTION;
        if (bblock_)
            return BASIC_BLOCK;
        if (function_)
            return FUNCTION;
        return EMPTY;
    }

    /** Predicate for emptiness.
     *
     *  Returns true if this reference points to nothing and has no stored address. */
    bool isEmpty() const {
        return granularity()==EMPTY;
    }

    /** Predicate for having a function.
     *
     *  Returns true if the reference points to a function. */
    bool hasFunction() const {
        return function_!=NULL;
    }

    /** Optional function pointer. */
    Function::Ptr function() const {
        return function_;
    }

    /** Predicate for having a basic block.
     *
     *  Returns true if the reference points to a basic block. */
    bool hasBasicBlock() const {
        return bblock_!=NULL;
    }

    /** Optional basic block pointer. */
    BasicBlock::Ptr basicBlock() const {
        return bblock_;
    }

    /** Predicate for having an instruction.
     *
     *  Returns true if the reference points to an instruction. */
    bool hasInstruction() const {
        return insn_ != NULL;
    }

    /** Optional instruction pointer. */
    SgAsmInstruction* instruction() const {
        return insn_;
    }

    /** Predicate for having a stored address.
     *
     *  All non-empty references have a computable address, but an address can also be stored explicitly.  This predicate
     *  returns true when an address is stored explicitly. */
    bool hasExplicitAddress() const {
        return address_;
    }

    /** Address.
     *
     *  All non-empty references have either an explicitly stored address or an address that can be obtained from an
     *  instruction, basic block, or function.  This method returns the first address found from that list. */
    rose_addr_t address() const;

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

/** Set of references. */
typedef std::set<Reference> ReferenceSet;

/** Cross references. */
typedef Sawyer::Container::Map<Reference, ReferenceSet> CrossReferences;

} // namespace
} // namespace
} // namespace

#endif
#endif
